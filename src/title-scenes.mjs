// TH12 1.00b TitleInf (43f760/43fde0). Resource loading and gameplay stay in
// the runtime; this controller owns only menu input, clocks and scene requests.
import {nativeSpellPageCount} from './native-data-presentation.mjs';
import {PracticeMenu} from './practice/menu.mjs';
export const TITLE_CHOICES=Object.freeze(['Start','Extra Start','Practice Start','Replay','Player Data','Music Room','Option','Quit']);
export const DIFFICULTIES=Object.freeze(['Easy','Normal','Hard','Lunatic']);
export const CHARACTERS=Object.freeze(['博麗 霊夢','霧雨 魔理沙','東風谷 早苗']);
export const DEFAULT_KEYS=Object.freeze({up:'ArrowUp',down:'ArrowDown',left:'ArrowLeft',right:'ArrowRight',shoot:'KeyZ',bomb:'KeyX',focus:'ShiftLeft',pause:'Escape',skip:'ControlLeft'});
export const DEFAULT_JOY_BUTTONS=Object.freeze({shoot:0,bomb:1,focus:2,pause:4,skip:3}); //4978b0
export const NAME_CHARACTERS='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*';
const DEMOS=['demo2.rpy','demo1.rpy','demo0.rpy'];
const KEY_BITS={ArrowUp:16,ArrowDown:32,ArrowLeft:64,ArrowRight:128,KeyZ:1,KeyX:2,Escape:256,Enter:0x80000,KeyR:0x200000,ShiftLeft:8,ShiftRight:8,ControlLeft:512,ControlRight:512};
const CONFIRM=0x80001,CANCEL=0x102,DIRECTIONS=0xf0;
const choice=(label,id=label,disabled=false)=>({label,id,disabled});
const clamp=(n,a,b)=>Math.max(a,Math.min(b,n));
const clone=value=>value===undefined?undefined:JSON.parse(JSON.stringify(value));

/**
 * tick(mask|{held,pressed}) is one 60 Hz UI tick. Commands are synchronous
 * plain objects from drainActions(). Runtime handles start, playReplay, demo,
 * audio, persistence and core operations; this module never consumes game RNG.
 * state.choices is the current selectable list; state.age is the scene clock.
 * state.events describes animation changes, including native IRQ numbers.
 */
export class TitleScenes {
  constructor({profile={},corpus=[],settings={},musicTracks=[],practiceEnabled=false,practiceLocale='zh-CN'}={}) {
    this.profile=profile;this.corpus=Array.isArray(corpus)?corpus:corpus.fixtures||[];
    this.musicTracks=musicTracks;this.settings={bgmVolume:100,seVolume:80,keyMap:{...DEFAULT_KEYS},...profile.settings,...settings};
    this.settings.keyMap={...DEFAULT_KEYS,...profile.settings?.keyMap,...settings.keyMap};
    this.settings.joyButtons={...DEFAULT_JOY_BUTTONS,...profile.settings?.joyButtons,...settings.joyButtons};
    this.settings.bgmVolume=clamp(Number(this.settings.bgmVolume)||0,0,100);
    this.settings.seVolume=clamp(Number(this.settings.seVolume)||0,0,100);
    this.actions=[];this.keys=new Set();this.previousHeld=0;this.holdAges=Array(32).fill(0);
    this.practiceEnabled=!!practiceEnabled;this.practiceLocale=practiceLocale;this.practiceMenu=null;this.practiceSettings=null;
    this.demoIndex=0;this.serial=0;this.idleFrames=0;this.pending=null;
    this.run={mode:'story',difficulty:1,character:0,shot:0,stage:1};
    this.remember={title:0,difficulty:1,character:0,shot:0,practice:0,replay:0,replaySave:0,music:0};
    this.state={};this.openTitle({intro:true});
  }
  emit(type,values={}) {this.actions.push({type,...values});}
  drainActions() {const actions=this.actions;this.actions=[];return actions;}
  setCorpus(corpus) {this.corpus=Array.isArray(corpus)?corpus:corpus.fixtures||[];if(['replay','replay-save'].includes(this.state.scene))this.refreshChoices();}
  setProfile(profile) {this.profile=profile;this.refreshChoices();}
  canExtra(character,shot) {
    if(typeof this.profile.canExtra==='function')return !!this.profile.canExtra(character,shot);
    const flags=this.profile.extraUnlocked;
    if(flags===true)return true;
    if(Array.isArray(flags))return !!flags[character*2+shot];
    return false;
  }
  anyExtra() {return [0,1,2].some(c=>[0,1].some(s=>this.canExtra(c,s)));}
  canPractice(stage) {
    if(typeof this.profile.canPractice==='function')return !!this.profile.canPractice(this.run.character,this.run.shot,this.run.difficulty,stage);
    return stage===1;
  }
  userReplays() {return this.corpus.filter(r=>r.provenance!=='built-in-demo'&&!/^demo\d+\.rpy$/.test(r.id||''));}
  replaySlots(){
    const files=this.userReplays(),slots=Array.from({length:25},(_,i)=>files.find(r=>r.id===`th12_${String(i+1).padStart(2,'0')}.rpy`)??null);
    const imported=files.filter(r=>!/^th12_\d\d\.rpy$/i.test(r.id||''));
    return slots.concat(Array.from({length:50},(_,i)=>imported[i]??null));
  }
  refreshChoices() {
    const s=this.state;
    switch(s.scene) {
      case 'title':s.choices=TITLE_CHOICES.map((label,i)=>choice(label,i,i===1&&!this.anyExtra()));break;
      case 'difficulty':s.choices=this.run.mode==='extra'?[choice('Extra',4)]:DIFFICULTIES.map((label,i)=>choice(label,i));break;
      case 'character':s.choices=CHARACTERS.map((label,i)=>choice(label,i,this.run.mode==='extra'&&![0,1].some(shot=>this.canExtra(i,shot))));break;
      case 'shot':s.choices=['A','B'].map((label,i)=>choice(label,i,this.run.mode==='extra'&&!this.canExtra(this.run.character,i)));break;
      case 'practice':s.choices=this.practiceEnabled&&this.practiceMenu?this.practiceMenu.rows().map(row=>choice(row.label,row.id)):Array.from({length:6},(_,i)=>choice(`Stage ${i+1}`,i+1,!this.canPractice(i+1)));break;
      case 'replay':s.choices=this.replaySlots().map((r,i)=>choice(r?r.name||r.id:i<25?`No.${String(i+1).padStart(2,'0')} --------`:'User --------',r?.id??null,!r));break;
      case 'replay-stage':s.choices=Array.from({length:7},(_,i)=>choice(i===6?'Extra':`Stage ${i+1}`,i+1,!s.replay?.stages?.some(stage=>stage.number===i+1)));break;
      case 'player-data':s.choices=Array.from({length:6},(_,i)=>choice(`${CHARACTERS[Math.floor(i/2)]} ${i%2?'B':'A'}`,i));s.dataDifficulty??=1;break;
      case 'music-room':s.choices=this.musicTracks.map((m,i)=>choice(m.label||m.title||m.name||m.id||`Track ${i+1}`,m.id||String(i)));break;
      case 'options':s.choices=['BGM Volume','SE Volume','Key Config','Default','Quit'].map((v,i)=>choice(v,i));break;
      case 'key-config':s.choices=['Shoot','Bomb','Focus','Pause','Skip','Default','Quit'].map((v,i)=>choice(v,i));break;
      case 'manual':s.choices=['操作方法','ゲームの進め方','ベントラーとUFO','スペルカード','リプレイと記録'].map((v,i)=>choice(v,i));break;
      case 'quit':s.choices=[choice('Return','return'),choice('Quit','quit')];break;
      case 'pause':s.choices=[choice('Return to Game','resume'),choice('Return to Title','title'),...(s.context?.mode==='replay'?[choice('Stop Replay','stop-replay')]:[choice('Save Replay and Quit','save-exit',s.context?.canSaveReplay===false),choice('Retry','retry')])];break;
      case 'pause-confirm':s.choices=[choice('Yes','yes'),choice('No','no')];break;
      case 'continue':s.choices=[choice('Continue','continue',s.context?.canContinue===false),choice('Return to Title','title'),choice('Save Replay','save-exit',s.context?.canSaveReplay===false),choice('Retry','retry')];break;
      case 'result-menu':s.choices=[choice('Return to Title','title'),choice('Save Replay','save-exit',s.context?.canSaveReplay===false),choice('Retry','retry')];break;
      case 'result':s.choices=[choice('Next','next')];break;
      case 'name':s.choices=s.ranked===false?[choice('Next','end')]:[...NAME_CHARACTERS].map((v,i)=>choice(v,i)).concat([choice('Space','space'),choice('Delete','delete'),choice('End','end')]);break;
      case 'replay-save':s.choices=Array.from({length:25},(_,i)=>choice(`No.${String(i+1).padStart(2,'0')}`,i+1));break;
      default:s.choices=[];
    }
    if(s.choices.length)s.selection=clamp(s.selection||0,0,s.choices.length-1);
    if(['replay','replay-save'].includes(s.scene))s.replayRecords=this.replaySlots();
    if(s.scene==='music-room')this.scrollMusic();
    s.page=Math.floor((s.selection||0)/(s.scene==='music-room'?8:25));
    s.settings=this.settings;s.run=this.run;
  }
  enter(scene,selection=0,values={}) {
    const from=this.state.scene;
    this.pending=null;this.state={scene,from,selection,age:0,readyAt:7,phase:'in',serial:++this.serial,events:[],notice:'',...values};
    if(scene==='practice'&&this.practiceEnabled){
      this.practiceMenu=new PracticeMenu({params:this.practiceSettings??{stage:this.run.stage-1},difficulty:this.run.difficulty,locale:this.practiceLocale});
      this.practiceMenu.selection=0;this.state.practiceMenu=this.practiceMenu;this.state.selection=0;
      this.emit('practiceReset');
    }
    if(scene==='key-config'&&!values.joyDraft)this.state.joyDraft={...this.settings.joyButtons};
    this.refreshChoices();this.emit('scene',{scene,from,selection:this.state.selection});
  }
  openTitle({intro=false}={}) {
    this.enter('title',this.remember.title,{intro,age:intro?0:180,readyAt:191});this.idleFrames=0;
    this.emit('menuMusic',{track:'th12_01'});
  }
  openPause(context={}) {this.enter('pause',0,{context:{...this.run,...context},readyAt:11});this.emit('pause');this.emit('sound',{id:14});}
  openContinue(context={}) {
    context={...this.run,...context};
    if(context.mode==='practice'||context.mode==='replay'){this.finish({kind:'gameover',...context});return;}
    const result={kind:'gameover',...context},eligible=result.ranked!==false&&!(result.continues>0||result.continueCount>0);
    const ranking=this.scoreRanking(result,eligible);
    this.emit('stopEffects');this.emit('menuMusic',{track:'th10_17',restart:true}); // Native4337ef..433827.
    //433710 binds the death background immediately, then reveals the table
    //after ten UI ticks and opens name/ranking input after another ten.
    this.enter('name',ranking.ranked?90:0,{result,context,name:String(this.profile.data?.lastName||'').slice(0,8),
      nameKind:'score',nameContext:'pause',afterName:'continue',...ranking,revealAt:10,readyAt:21});
    this.emit('result',{result,needsName:ranking.ranked,nameContext:'pause',afterName:'continue'});
  }
  resume() {this.animate('resume');this.wait(12,()=>{this.enter('playing',0,{readyAt:0});this.emit('resume');});}
  quickRetry() {
    const s=this.state;if(s.scene!=='pause'||s.context?.mode==='replay'||!this.ready())return false;
    this.emit('sound',{id:7});this.animate('retry-shortcut');const context=s.context;
    //432c5f..432ccf bypasses confirmation, IRQ6 on front93;4335fe waits12ticks.
    this.wait(12,()=>this.pauseAction('retry',context),'retry-shortcut');return true;
  }
  returnFromPauseConfirm(){const s=this.state;this.animate('confirm-no');this.wait(20,()=>this.enter('pause',s.pauseSelection,{context:s.context,readyAt:0,phase:'active'}));}
  finish(result={}) {
    const mode=result.mode||this.run.mode;
    if(mode==='demo'||mode==='replay'||result.kind==='replay') {this.returnTitle();return;}
    result={...this.run,...result};
    if(mode==='practice'){
      this.emit('stopEffects');this.emit('menuMusic',{track:'th10_17',restart:true}); // Native433870, practice results.
      this.openResultMenu(result);this.emit('result',{result,needsName:false,nameContext:'pause',afterName:'result-menu'});return;
    }
    if(mode==='extra'&&!result.clear){this.openContinue({...result,canContinue:false});return;}
    const extraClear=mode==='extra'&&result.clear;
    if(extraClear)this.emit('pause'); // Native432850, freeze gameplay before score music.
    this.emit('stopEffects');this.emit('menuMusic',{track:'th10_17',restart:true});
    const eligible=result.ranked!==false&&!(result.continues>0||result.continueCount>0);
    if(!eligible){this.emit('result',{result,needsName:false});this.openReplaySave(result);return;}
    const ranking=this.scoreRanking(result);
    //Extra clear uses the common in-game ranking/name flow before root116.
    this.enter('name',ranking.ranked?90:0,{result,name:String(this.profile.data?.lastName||'').slice(0,8),nameKind:'score',
      nameContext:extraClear?'pause':'title',afterName:extraClear?'result-menu':'replay-save',...ranking,
      ...(extraClear?{context:result,revealAt:10,readyAt:21}:{})});
    this.emit('result',{result,needsName:ranking.ranked,nameContext:extraClear?'pause':'title',afterName:extraClear?'result-menu':'replay-save'});
  }
  openResultMenu(result,{fromName=false,nameContext='pause'}={}){
    this.enter('result-menu',0,{context:result,result,failed:!result.clear,nameContext,
      readyAt:fromName?0:11,phase:fromName?'active':'in'});
    if(fromName)this.state.events.push({type:'selection',previous:0,selection:0,resetInterrupt:3,selectionInterrupt:7});
  }
  scoreRanking(result,eligible=true){
    const rows=(this.profile.rankings?.(result.character,result.shot,result.difficulty)??[]).slice(0,10);
    //Native insertion accepts an equal score ahead of that existing row.
    let rank=eligible?rows.findIndex(r=>result.score>=r.score):-1;
    if(eligible&&rank<0&&rows.length<10)rank=rows.length;
    const ranking=rows.slice();
    if(rank>=0)ranking.splice(rank,0,{...result,name:'        ',date:result.date??new Date().toISOString()});
    return {rank,ranked:rank>=0,ranking:ranking.slice(0,10)};
  }
  returnTitle() {this.emit('returnTitle');this.openTitle({intro:false});}
  start() {const settings={...this.run};this.enter('playing',0,{readyAt:0});this.emit('start',settings);}
  wait(frames,callback,action='confirm') {this.state.phase='out';this.state.transitionAge=0;this.state.transitionAction=action;this.pending={frames,callback};}
  animate(type='confirm',values={}){this.state.events.push({type,selection:this.state.selection,...values});}
  ready() {return !this.pending&&this.state.age>=this.state.readyAt;}
  select(index,{initial=false}={}) {
    const s=this.state;if(!s.choices.length)return false;
    index=clamp(index,0,s.choices.length-1);
    if(index===s.selection||s.choices[index].disabled&&!['practice','replay'].includes(s.scene))return false;
    const previous=s.selection;s.selection=index;s.page=Math.floor(index/(s.scene==='music-room'?8:25));
    if(s.scene==='music-room')this.scrollMusic();
    s.events.push({type:'selection',previous,selection:index,resetInterrupt:3,selectionInterrupt:(initial?17:7)+index});
    this.emit('sound',{id:10});return true;
  }
  move(delta) {
    const s=this.state,count=s.choices.length;if(!count)return;
    let next=s.selection;
    if(s.scene==='replay'){const page=Math.floor(next/25);this.select(page*25+(next%25+delta+25)%25);return;}
    for(let i=0;i<count;++i){next=(next+delta+count)%count;if(!s.choices[next].disabled||s.scene==='practice'){this.select(next);break;}}
  }
  scrollMusic() {
    const s=this.state;s.musicTop=Math.max(0,s.musicTop||0);
    if(s.selection<s.musicTop)s.musicTop=s.selection;
    if(s.selection>=s.musicTop+10)s.musicTop=s.selection-9;
  }
  settingsChanged() {this.state.settings=this.settings;this.emit('settings',{settings:clone(this.settings)});}
  key(code,down=true) {
    if(this.state.scene==='key-config'&&this.state.captureJoy&&down&&(code==='Escape'||code==='KeyX')){this.state.captureJoy=null;return;}
    if(code==='F1'&&down&&this.state.scene!=='playing'&&this.state.scene!=='demo') {
      if(this.state.scene==='manual'){this.cancel();return;}
      if(this.pending)return;
      const {scene,selection,serial,events,choices,settings,run,...returnValues}=this.state;
      this.enter('manual',0,{returnScene:scene,returnSelection:selection,returnValues,readyAt:0});return;
    }
    if(KEY_BITS[code])down?this.keys.add(code):this.keys.delete(code);
  }
  clearKeys() {this.keys.clear();this.previousHeld=0;this.holdAges.fill(0);}
  captureGamepad(button){if(this.state.scene!=='key-config'||!this.state.captureJoy||!Number.isInteger(button)||button<0||button>31)return false;this.state.joyDraft[this.state.captureJoy]=button;this.state.captureJoy=null;return true;}
  tick(input=0) {
    const s=this.state;s.age++;
    let held=typeof input==='number'?input:(input.held||0);
    for(const code of this.keys)held|=KEY_BITS[code]||0;
    const pressed=(typeof input==='object'?input.pressed||0:held&~this.previousHeld)|held&~this.previousHeld;
    this.previousHeld=held;
    let repeat=0;
    for(let bit=0;bit<32;++bit){const mask=1<<bit;if(held&mask){const age=++this.holdAges[bit];if(age>=26&&(age-26)%8===0)repeat|=mask;}else this.holdAges[bit]=0;}
    if(this.pending){
      s.transitionAge++;
      const loadingFade=s.transitionAction==='confirm'&&(s.scene==='shot'&&this.run.mode!=='practice'||s.scene==='practice'||s.scene==='replay-stage');
      if(loadingFade&&s.transitionAge===(s.scene==='replay-stage'?2:10)){this.animate('fade',{frames:32});this.emit('loadingIndicator');}
      if(--this.pending.frames<=0){const callback=this.pending.callback;this.pending=null;callback();}return this.state;
    }
    if(s.scene==='demo') {
      if(held&0x80103||s.age>=3600)this.returnTitle();
      //422af3 starts a60tick screen fade at3540; the title music continues.
      //The runtime renders that visual fade without issuing a BGM command.
      return this.state;
    }
    // The original title controller is absent during gameplay. Z and X belong
    // to Player16 / MSG25; they must never confirm or cancel a title menu here.
    if(s.scene==='playing'||s.scene==='ending')return this.state;
    if(s.scene==='title') {
      this.idleFrames=(held&0xffff)?0:this.idleFrames+1;
      if(this.idleFrames>=1800){const id=DEMOS[this.demoIndex];this.demoIndex=(this.demoIndex+1)%DEMOS.length;this.enter('demo',0,{id,readyAt:0});this.emit('demo',{id});return this.state;}
    }
    if(!this.ready())return this.state;
    if(s.captureJoy){if(pressed&CANCEL)s.captureJoy=null;return this.state;}
    if(s.phase==='in'){s.phase='active';s.events.push({type:'ready',resetInterrupt:3,selectionInterrupt:17+s.selection});}
    if(s.scene==='practice'&&this.practiceEnabled&&this.practiceMenu){
      const action=this.practiceMenu.tick(held,pressed);this.syncPracticeRows();if(action)this.practiceAction(action);return this.state;
    }
    if(s.scene==='pause'&&(pressed&0x200000)&&this.quickRetry())return this.state;
    const direction=(pressed|repeat)&DIRECTIONS;
    if(s.scene==='name'&&s.ranked===false) {
      //The unqualified score still shows all ten rows, with no character grid.
    }else if(s.scene==='name') {
      if(direction&16)this.select((s.selection-13+s.choices.length)%s.choices.length);
      if(direction&32)this.select((s.selection+13)%s.choices.length);
      if(direction&64)this.select(Math.floor(s.selection/13)*13+(s.selection%13+12)%13);if(direction&128)this.select(Math.floor(s.selection/13)*13+(s.selection%13+1)%13);
    }else if(s.scene==='player-data'){
      if(direction&16)this.dataDifficulty(-1);if(direction&32)this.dataDifficulty(1);
      if(direction&64)this.move(-1);if(direction&128)this.move(1);
    }else {
      if(direction&16)this.move(-1);if(direction&32)this.move(1);
      if(direction&64)this.horizontal(-1);if(direction&128)this.horizontal(1);
    }
    if(pressed&CANCEL)this.cancel();else if(pressed&CONFIRM)this.confirm();
    return this.state;
  }
  horizontal(delta) {
    const s=this.state;
    if(s.scene==='options'&&s.selection<2) {
      const key=s.selection===0?'bgmVolume':'seVolume';this.settings[key]=clamp(this.settings[key]+delta*5,0,100);this.settingsChanged();
    }else if(s.scene==='replay') {
      this.select((s.selection+delta*25+75)%75);
    }else if(s.scene==='music-room'||s.scene==='manual')this.move(delta);
  }
  dataDifficulty(delta){const s=this.state;s.dataDifficulty=(s.dataDifficulty+delta+5)%5;s.dataPage=0;this.animate('data-header');this.emit('sound',{id:10});}
  choose(index) {if(!this.ready())return false;const item=this.state.choices[index];if(!item)return false;if(this.state.scene==='practice'&&this.practiceEnabled&&this.practiceMenu){this.practiceMenu.selection=index;const action=this.practiceMenu.activate(item.id);this.syncPracticeRows();if(action)this.practiceAction(action);return true;}if(item.disabled){if(['practice','replay'].includes(this.state.scene))this.select(index);this.emit('sound',{id:17});return false;}this.select(index);this.confirm();return true;}
  syncPracticeRows(){if(this.state.scene!=='practice'||!this.practiceMenu)return;this.refreshChoices();this.state.selection=this.practiceMenu.selection;this.state.notice=this.practiceMenu.notice;}
  practiceAction(action){
    if(!action||this.state.scene!=='practice'||!this.practiceEnabled)return;
    if(action.type==='cancel'){this.cancel();return;}
    if(action.type==='start'){
      this.practiceSettings={...action.params};this.run.practice={...action.params};this.run.stage=action.params.stage+1;this.run.difficulty=action.difficulty;
      this.emit('sound',{id:7});this.animate();this.emit('fadeMusic',{seconds:6});this.wait(40,()=>this.start());
    }
  }
  cancel() {
    const s=this.state;if(s.scene==='playing'||s.scene==='ending'||s.scene==='demo')return;
    this.emit('sound',{id:9});
    switch(s.scene) {
      case 'title':if(s.selection===7)this.confirm();else this.select(7);break;
      case 'difficulty':this.animate('cancel');this.wait(6,()=>this.openTitle());break;
      case 'character':this.animate('cancel');this.wait(6,()=>this.enter('difficulty',this.run.mode==='extra'?0:this.run.difficulty));break;
      case 'shot':this.animate('cancel');this.wait(6,()=>this.enter('character',this.run.character));break;
      case 'practice':this.animate('cancel');this.wait(6,()=>this.enter('shot',this.run.shot),'cancel');break;
      case 'replay-stage':this.enter('replay',this.remember.replay);break;
      case 'options':if(s.selection===4){this.animate();this.wait(10,()=>this.returnTitle());}else this.select(4);break;
      case 'key-config':if(s.selection===6){this.animate();this.wait(10,()=>this.enter('options',2));}else this.select(6);break;
      case 'manual':this.enter(s.returnScene||'options',s.returnSelection||0,s.returnValues||{});break;
      case 'pause':this.resume();break;
      case 'pause-confirm':if(s.selection===0)this.select(1);else this.returnFromPauseConfirm();break;
      case 'continue':this.returnTitle();break;
      case 'result-menu':this.returnTitle();break;
      case 'name':if(s.ranked===false)this.completeName();else if(s.name.length)s.name=s.name.slice(0,-1);else if(s.nameKind==='replay')this.enter('replay-save',s.slot-1,{result:s.result,name:s.name,nameContext:s.nameContext});break;
      case 'replay-save':this.animate('cancel');this.wait(10,()=>this.returnTitle());break;
      case 'result':this.confirm();break;
      default:this.returnTitle();
    }
  }
  confirm() {
    const s=this.state;if(s.scene==='playing'||s.scene==='ending'||s.scene==='demo')return;
    const item=s.choices[s.selection];if(item?.disabled){this.emit('sound',{id:17});return;}
    //449b61..449c72 changes the Music Room track without a menu OK sound.
    if(s.scene!=='music-room'&&!(s.scene==='name'&&s.nameKind==='replay'&&item?.id==='end'))this.emit('sound',{id:7});
    switch(s.scene) {
      case 'title': {
        this.remember.title=s.selection;
        s.events.push({type:'confirm',interrupt:6});
        const selected=s.selection;
        this.wait(20,()=>{
          if(selected===0||selected===2){delete this.run.practice;this.run.mode=selected===2?'practice':'story';this.enter('difficulty',this.remember.difficulty);}
          else if(selected===1){delete this.run.practice;this.run.mode='extra';this.run.difficulty=4;this.run.stage=7;this.enter('difficulty',0);}
          else if(selected===3)this.enter('replay',this.remember.replay);
          else if(selected===4)this.enter('player-data',0,{dataPage:0,dataDifficulty:1});
          else if(selected===5)this.enter('music-room',this.remember.music);
          else if(selected===6)this.enter('options',0);
          else{this.enter('closed',0,{readyAt:0});this.emit('quit');}
        });break;
      }
      case 'difficulty':this.animate();this.run.difficulty=item.id;if(this.run.mode!=='extra')this.remember.difficulty=s.selection;this.wait(14,()=>this.enter('character',this.remember.character));break;
      case 'character':this.animate();this.run.character=s.selection;this.remember.character=s.selection;this.wait(14,()=>{this.enter('shot',this.remember.shot);if(this.state.choices[this.state.selection]?.disabled)this.move(1);});break;
      case 'shot':this.animate();this.run.shot=s.selection;this.remember.shot=s.selection;if(this.run.mode==='practice')this.wait(10,()=>this.enter('practice',this.remember.practice,{readyAt:11}));else {this.emit('fadeMusic',{seconds:1});this.run.stage=this.run.mode==='extra'?7:1;this.wait(40,()=>this.start());}break;
      case 'practice':if(this.practiceEnabled&&this.practiceMenu){const action=this.practiceMenu.activate(item?.id);this.syncPracticeRows();if(action)this.practiceAction(action);}else if(item){this.animate();this.emit('fadeMusic',{seconds:6});this.run.stage=item.id;this.remember.practice=s.selection;this.wait(40,()=>this.start());}break;
      case 'replay':if(item){this.remember.replay=s.selection;const replay=this.replaySlots()[s.selection],first=replay.stages[0]?.number??1;this.enter('replay-stage',first-1,{replay,fileIndex:s.selection,readyAt:15});}break;
      case 'replay-stage':if(item){this.animate();this.emit('fadeMusic',{seconds:6});const replay=s.replay,stage=item.id;this.wait(32,()=>{this.enter('playing',0,{readyAt:0});this.emit('playReplay',{id:replay.id,stage});});}break;
      case 'music-room':if(item){
        this.remember.music=s.selection;
        const heard=item.id==='th12_01'||this.profile.data?.music?.includes?.(item.id);
        //449b61..449bbe warns once before playing an unheard track. The next
        //confirm accepts that warning; selecting a track alone does not play it.
        s.commentTrack=item.id;s.commentSerial=(s.commentSerial??0)+1;
        if(!heard&&!s.musicWarning){s.musicWarning=true;this.emit('musicWarning',{track:item.id});break;}
        s.musicWarning=false;this.emit('menuMusic',{track:item.id,musicRoom:true});s.playingTrack=item.id;
      }break;
      case 'options':if(s.selection===2){this.animate();this.wait(10,()=>this.enter('key-config',0));}else if(s.selection===3){this.settings.bgmVolume=100;this.settings.seVolume=80;this.settingsChanged();}else if(s.selection===4){this.animate();this.wait(10,()=>this.returnTitle());}break;
      case 'key-config':if(s.selection<5){s.captureJoy=['shoot','bomb','focus','pause','skip'][s.selection];s.captureReady=false;}else if(s.selection===5)s.joyDraft={...this.settings.joyButtons};else {this.settings.joyButtons={...s.joyDraft};this.settingsChanged();this.animate();this.wait(10,()=>this.enter('options',2));}break;
      case 'quit':if(item?.id==='quit'){this.enter('closed',0,{readyAt:0});this.emit('quit');}else this.openTitle();break;
      case 'closed':this.openTitle();break;
      case 'pause':if(item?.id==='resume')this.resume();else this.enter('pause-confirm',1,{context:s.context,pauseSelection:s.selection,pauseAction:item?.id,readyAt:30});break;
      case 'pause-confirm':if(item?.id==='no')this.returnFromPauseConfirm();else this.pauseAction(s.pauseAction,s.context);break;
      case 'continue':if(item?.id==='continue'){this.enter('playing',0,{readyAt:0});this.emit('continue',{...s.context});}else if(item?.id==='retry')this.pauseAction('retry',s.context);else if(item?.id==='save-exit')this.openReplaySave(s.context,this.profile.data?.lastName);else this.returnTitle();break;
      case 'result-menu':if(item?.id==='retry')this.pauseAction('retry',s.context);else if(item?.id==='save-exit')this.openReplaySave(s.context,this.profile.data?.lastName);else this.returnTitle();break;
      case 'player-data':s.dataPage=((s.dataPage||0)+1)%nativeSpellPageCount(s.dataDifficulty);this.animate('data-header');break;
      case 'result': {
        const result=s.result;
        if(result.mode==='practice'||result.ranked===false||result.continues>0)this.openReplaySave(result);
        else this.enter('name',90,{result,name:String(this.profile.data?.lastName||'PLAYER').slice(0,8),readyAt:0});break;
      }
      case 'name':if(s.ranked===false||item?.id==='end')this.completeName();else if(item?.id==='delete')s.name=s.name.slice(0,-1);else {if(s.name.length>=8)s.name=s.name.slice(0,7);s.name+=item?.id==='space'?' ':item?.label||'';if(s.name.length>=8)this.select(90);}break;
      case 'replay-save':if(item){this.remember.replaySave=s.selection;this.enter('name',90,{nameKind:'replay',nameContext:s.nameContext,slot:item.id,result:s.result,name:s.name??this.profile.data?.lastName??'',readyAt:s.nameContext==='pause'?11:7});}break;
    }
  }
  pauseAction(action,context) {
    if(action==='retry'){
      // Result/pause context uses practice as a boolean. The running selection
      // owns the parameter object used to restart the same practice session.
      const run={...this.run,...context,practice:this.run.practice},stage=run.mode==='practice'?run.stage:run.mode==='extra'?7:1;
      this.run={mode:run.mode,character:run.character,shot:run.shot,difficulty:run.difficulty,stage,...(run.practice?{practice:{...run.practice}}:{})};
      this.enter('playing',0,{readyAt:0});this.emit('retry',{...this.run});
    }
    else if(action==='save-exit')this.emit('saveExit');
    else this.returnTitle();
  }
  completeName() {
    const {name,result,nameKind,slot,ranked,nameContext,afterName,context}=this.state;
    if(nameKind==='replay'){this.emit('sound',{id:18});this.emit('saveReplay',{slot,name:name.trim()||'        ',result});this.enter('replay-save',slot-1,{result,name,nameContext,readyAt:nameContext==='pause'?11:7});return;}
    if(ranked!==false)this.emit('saveName',{name:name.trim()||'        ',result,nameContext,afterName});
    if(afterName==='continue'){
      const selection=(context??result).canContinue===false?1:0;
      this.enter('continue',selection,{context:context??result,result,nameContext,readyAt:0,phase:'active'});
      //Native creates front111 and sends IRQ3 then IRQ7 in this same tick.
      //Keeping phase active prevents a later ready IRQ17 and another wait.
      this.state.events.push({type:'selection',previous:selection,selection,resetInterrupt:3,selectionInterrupt:7+selection});
    }else if(afterName==='result-menu')this.openResultMenu(result,{fromName:true,nameContext});
    else this.openReplaySave(result,name);
  }
  openReplaySave(result,name) {if(result.canSaveReplay===false){this.returnTitle();return;}const nameContext=this.state.nameContext??'title';this.enter('replay-save',this.remember.replaySave,{result,name,nameContext,readyAt:nameContext==='pause'?11:7});}
  view() {return {...this.state,choices:this.state.choices.map(c=>({...c})),settings:clone(this.settings),run:{...this.run}};}
}
