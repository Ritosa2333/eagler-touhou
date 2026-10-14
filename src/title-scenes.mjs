// TH12 1.00b TitleInf (43f760/43fde0). Resource loading and gameplay stay in
// the runtime; this controller owns only menu input, clocks and scene requests.
export const TITLE_CHOICES=Object.freeze(['Start','Extra Start','Practice Start','Replay','Player Data','Music Room','Option','Quit']);
export const DIFFICULTIES=Object.freeze(['Easy','Normal','Hard','Lunatic']);
export const CHARACTERS=Object.freeze(['博麗 霊夢','霧雨 魔理沙','東風谷 早苗']);
export const DEFAULT_KEYS=Object.freeze({up:'ArrowUp',down:'ArrowDown',left:'ArrowLeft',right:'ArrowRight',shoot:'KeyZ',bomb:'KeyX',focus:'ShiftLeft',pause:'Escape',skip:'ControlLeft'});
export const NAME_CHARACTERS='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*';
const DEMOS=['demo2.rpy','demo1.rpy','demo0.rpy'];
const KEY_BITS={ArrowUp:16,ArrowDown:32,ArrowLeft:64,ArrowRight:128,KeyZ:1,KeyX:2,Escape:256,Enter:0x80000,ShiftLeft:8,ShiftRight:8,ControlLeft:512,ControlRight:512};
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
  constructor({profile={},corpus=[],settings={},musicTracks=[]}={}) {
    this.profile=profile;this.corpus=Array.isArray(corpus)?corpus:corpus.fixtures||[];
    this.musicTracks=musicTracks;this.settings={bgmVolume:100,seVolume:80,keyMap:{...DEFAULT_KEYS},...profile.settings,...settings};
    this.settings.keyMap={...DEFAULT_KEYS,...profile.settings?.keyMap,...settings.keyMap};
    this.settings.bgmVolume=clamp(Number(this.settings.bgmVolume)||0,0,100);
    this.settings.seVolume=clamp(Number(this.settings.seVolume)||0,0,100);
    this.actions=[];this.keys=new Set();this.previousHeld=0;this.holdAges=Array(32).fill(0);
    this.demoIndex=0;this.serial=0;this.idleFrames=0;this.pending=null;
    this.run={mode:'story',difficulty:1,character:0,shot:0,stage:1};
    this.remember={title:0,difficulty:1,character:0,shot:0,practice:0,replay:0,music:0};
    this.state={};this.openTitle({intro:true});
  }
  emit(type,values={}) {this.actions.push({type,...values});}
  drainActions() {const actions=this.actions;this.actions=[];return actions;}
  setCorpus(corpus) {this.corpus=Array.isArray(corpus)?corpus:corpus.fixtures||[];if(this.state.scene==='replay')this.refreshChoices();}
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
  refreshChoices() {
    const s=this.state;
    switch(s.scene) {
      case 'title':s.choices=TITLE_CHOICES.map((label,i)=>choice(label,i,i===1&&!this.anyExtra()));break;
      case 'difficulty':s.choices=DIFFICULTIES.map((label,i)=>choice(label,i));break;
      case 'character':s.choices=CHARACTERS.map((label,i)=>choice(label,i,this.run.mode==='extra'&&![0,1].some(shot=>this.canExtra(i,shot))));break;
      case 'shot':s.choices=['A','B'].map((label,i)=>choice(label,i,this.run.mode==='extra'&&!this.canExtra(this.run.character,i)));break;
      case 'practice':s.choices=Array.from({length:6},(_,i)=>choice(`Stage ${i+1}`,i+1,!this.canPractice(i+1)));break;
      case 'replay':s.choices=this.userReplays().map(r=>choice(r.name?`${r.name}  ${r.id}`:r.id,r.id));break;
      case 'replay-stage':s.choices=(s.replay?.stages||[]).map(stage=>choice(stage.number===7?'Extra':`Stage ${stage.number}`,stage.number));break;
      case 'player-data':s.choices=Array.from({length:31},(_,i)=>choice(i===30?'Total':`${CHARACTERS[Math.floor(i/10)]} ${i%10<5?'A':'B'} ${DIFFICULTIES[i%5]??'Extra'}`,i));break;
      case 'music-room':s.choices=this.musicTracks.map((m,i)=>choice(m.title||m.name||m.id||`Track ${i+1}`,m.id||String(i)));break;
      case 'options':s.choices=['BGM Volume','SE Volume','Key Config','Default','Quit'].map((v,i)=>choice(v,i));break;
      case 'key-config':s.choices=['Shoot','Bomb','Focus','Pause','Skip','Default','Quit'].map((v,i)=>choice(v,i));break;
      case 'manual':s.choices=['操作方法','ゲームの進め方','ベントラーとUFO','スペルカード','リプレイと記録'].map((v,i)=>choice(v,i));break;
      case 'quit':s.choices=[choice('Return','return'),choice('Quit','quit')];break;
      case 'pause':s.choices=[choice('Return to Game','resume'),choice('Return to Title','title'),...(s.context?.mode==='replay'?[choice('Stop Replay','stop-replay')]:[choice('Save Replay and Quit','save-exit',s.context?.canSaveReplay===false),choice('Retry','retry')])];break;
      case 'pause-confirm':s.choices=[choice('Yes','yes'),choice('No','no')];break;
      case 'continue':s.choices=[choice('Continue','continue'),choice('Return to Title','title'),choice('Save Replay','save-exit',s.context?.canSaveReplay===false),choice('Retry','retry')];break;
      case 'result':s.choices=[choice('Next','next')];break;
      case 'name':s.choices=[...NAME_CHARACTERS].map((v,i)=>choice(v,i)).concat([choice('Space','space'),choice('Delete','delete'),choice('End','end')]);break;
      case 'replay-save':s.choices=Array.from({length:25},(_,i)=>choice(`No.${String(i+1).padStart(2,'0')}`,i+1)).concat([choice('Do not save','cancel')]);break;
      default:s.choices=[];
    }
    if(s.choices.length)s.selection=clamp(s.selection||0,0,s.choices.length-1);
    if(s.scene==='music-room')this.scrollMusic();
    s.page=Math.floor((s.selection||0)/(s.scene==='music-room'?8:25));
    s.settings=this.settings;s.run=this.run;
  }
  enter(scene,selection=0,values={}) {
    const from=this.state.scene;
    this.pending=null;this.state={scene,selection,age:0,readyAt:7,phase:'in',serial:++this.serial,events:[],notice:'',...values};
    this.refreshChoices();this.emit('scene',{scene,from,selection:this.state.selection});
  }
  openTitle({intro=false}={}) {
    this.enter('title',this.remember.title,{intro,readyAt:intro?191:7});this.idleFrames=0;
    this.emit('menuMusic',{track:'th12_01'});
  }
  openPause(context={}) {this.enter('pause',0,{context:{...this.run,...context},readyAt:0});}
  openContinue(context={}) {
    context={...this.run,...context};
    if(context.mode==='extra'||context.mode==='practice'||context.mode==='replay'){this.finish({kind:'gameover',...context});return;}
    this.enter('continue',0,{context,readyAt:0});
  }
  resume() {this.enter('playing',0,{readyAt:0});this.emit('resume');}
  finish(result={}) {
    const mode=result.mode||this.run.mode;
    if(mode==='demo'||mode==='replay'||result.kind==='replay') {this.returnTitle();return;}
    this.enter('result',0,{result:{...this.run,...result},readyAt:30});
    this.emit('result',{result:this.state.result});
  }
  returnTitle() {this.emit('returnTitle');this.openTitle({intro:false});}
  start() {const settings={...this.run};this.enter('playing',0,{readyAt:0});this.emit('start',settings);}
  wait(frames,callback) {this.state.phase='out';this.pending={frames,callback};}
  ready() {return !this.pending&&this.state.age>=this.state.readyAt;}
  select(index,{initial=false}={}) {
    const s=this.state;if(!s.choices.length)return false;
    index=clamp(index,0,s.choices.length-1);
    if(index===s.selection||s.choices[index].disabled)return false;
    const previous=s.selection;s.selection=index;s.page=Math.floor(index/(s.scene==='music-room'?8:25));
    if(s.scene==='music-room')this.scrollMusic();
    s.events.push({type:'selection',previous,selection:index,resetInterrupt:3,selectionInterrupt:(initial?17:7)+index});
    this.emit('sound',{id:10});return true;
  }
  move(delta) {
    const s=this.state,count=s.choices.length;if(!count)return;
    let next=s.selection;
    for(let i=0;i<count;++i){next=(next+delta+count)%count;if(!s.choices[next].disabled){this.select(next);break;}}
  }
  scrollMusic() {
    const s=this.state;s.musicTop=Math.max(0,s.musicTop||0);
    if(s.selection<s.musicTop)s.musicTop=s.selection;
    if(s.selection>=s.musicTop+10)s.musicTop=s.selection-9;
  }
  settingsChanged() {this.state.settings=this.settings;this.emit('settings',{settings:clone(this.settings)});}
  key(code,down=true) {
    if(this.state.scene==='key-config'&&this.state.captureKey&&down) {
      if(code==='Escape'){this.state.captureKey=null;return;}
      const action=this.state.captureKey;
      // Reusing another command's key swaps its old binding, avoiding two
      // gameplay actions firing from one key after a menu rebinding.
      const other=Object.keys(this.settings.keyMap).find(k=>k!==action&&this.settings.keyMap[k]===code);
      if(other)this.settings.keyMap[other]=this.settings.keyMap[action];
      this.settings.keyMap[action]=code;this.state.captureKey=null;this.settingsChanged();return;
    }
    if(code==='F1'&&down&&this.state.scene!=='playing'&&this.state.scene!=='demo') {
      if(this.state.scene==='manual'){this.cancel();return;}
      if(this.pending)return;
      const {scene,selection,serial,events,choices,settings,run,...returnValues}=this.state;
      this.enter('manual',0,{returnScene:scene,returnSelection:selection,returnValues,readyAt:0});return;
    }
    if(KEY_BITS[code])down?this.keys.add(code):this.keys.delete(code);
  }
  clearKeys() {this.keys.clear();this.previousHeld=0;this.holdAges.fill(0);}
  tick(input=0) {
    const s=this.state;s.events=[];s.age++;
    let held=typeof input==='number'?input:(input.held||0);
    for(const code of this.keys)held|=KEY_BITS[code]||0;
    const pressed=(typeof input==='object'?input.pressed||0:held&~this.previousHeld)|held&~this.previousHeld;
    this.previousHeld=held;
    let repeat=0;
    for(let bit=0;bit<32;++bit){const mask=1<<bit;if(held&mask){const age=++this.holdAges[bit];if(age>=26&&(age-26)%8===0)repeat|=mask;}else this.holdAges[bit]=0;}
    if(this.pending){if(--this.pending.frames<=0){const callback=this.pending.callback;this.pending=null;callback();}return this.state;}
    if(s.scene==='demo') {
      if(held&0x80103||s.age>=3600)this.returnTitle();
      else if(s.age===3540)this.emit('demoFade',{frames:60});
      return this.state;
    }
    if(s.scene==='title') {
      this.idleFrames=(held&0xffff)?0:this.idleFrames+1;
      if(this.idleFrames>=1800){const id=DEMOS[this.demoIndex];this.demoIndex=(this.demoIndex+1)%DEMOS.length;this.enter('demo',0,{id,readyAt:0});this.emit('demo',{id});return this.state;}
    }
    if(!this.ready()||s.captureKey)return this.state;
    if(s.phase==='in'){s.phase='active';s.events.push({type:'ready',resetInterrupt:3,selectionInterrupt:17+s.selection});}
    const direction=(pressed|repeat)&DIRECTIONS;
    if(s.scene==='name') {
      if(direction&16)this.select((s.selection-13+s.choices.length)%s.choices.length);
      if(direction&32)this.select((s.selection+13)%s.choices.length);
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
    }else if(s.scene==='player-data') {
      s.dataPage=((s.dataPage||0)+delta+3)%3;s.spellPage=0;this.emit('sound',{id:10});
    }else if(s.scene==='replay') {
      this.select(clamp(s.selection+delta*25,0,s.choices.length-1));
    }else if(s.scene==='difficulty'||s.scene==='character'||s.scene==='shot'||s.scene==='music-room'||s.scene==='manual')this.move(delta);
  }
  choose(index) {if(!this.ready())return false;const item=this.state.choices[index];if(!item||item.disabled)return false;this.select(index);this.confirm();return true;}
  cancel() {
    const s=this.state;this.emit('sound',{id:9});
    switch(s.scene) {
      case 'title':if(s.selection===7)this.confirm();else this.select(7);break;
      case 'difficulty':this.openTitle();break;
      case 'character':if(this.run.mode==='extra')this.openTitle();else this.enter('difficulty',this.run.difficulty);break;
      case 'shot':this.enter('character',this.run.character);break;
      case 'practice':this.enter('shot',this.run.shot);break;
      case 'replay-stage':this.enter('replay',this.remember.replay);break;
      case 'options':if(s.selection===4)this.returnTitle();else this.select(4);break;
      case 'key-config':if(s.selection===6)this.enter('options',2);else this.select(6);break;
      case 'manual':this.enter(s.returnScene||'options',s.returnSelection||0,s.returnValues||{});break;
      case 'pause':this.resume();break;
      case 'pause-confirm':this.enter('pause',s.pauseSelection,{context:s.context,readyAt:0});break;
      case 'continue':this.finish({kind:'gameover',...s.context});break;
      case 'name':if(s.name.length)s.name=s.name.slice(0,-1);else this.completeName();break;
      case 'replay-save':this.returnTitle();break;
      case 'result':this.confirm();break;
      default:this.returnTitle();
    }
  }
  confirm() {
    const s=this.state,item=s.choices[s.selection];if(item?.disabled)return;
    this.emit('sound',{id:7});
    switch(s.scene) {
      case 'title': {
        this.remember.title=s.selection;
        s.events.push({type:'confirm',interrupt:6});
        const selected=s.selection;
        this.wait(20,()=>{
          if(selected<=5)this.emit('titleLogo',{interrupt:3,immediate:true});
          if(selected===0||selected===2){this.run.mode=selected===2?'practice':'story';this.enter('difficulty',this.remember.difficulty);}
          else if(selected===1){this.run.mode='extra';this.run.difficulty=4;this.run.stage=7;this.enter('character',this.remember.character);}
          else if(selected===3)this.enter('replay',this.remember.replay);
          else if(selected===4)this.enter('player-data',0,{dataPage:0});
          else if(selected===5)this.enter('music-room',this.remember.music);
          else if(selected===6)this.enter('options',0);
          else this.enter('quit',0);
        });break;
      }
      case 'difficulty':this.run.difficulty=s.selection;this.remember.difficulty=s.selection;this.enter('character',this.remember.character);break;
      case 'character':this.run.character=s.selection;this.remember.character=s.selection;this.enter('shot',this.remember.shot);if(this.state.choices[this.state.selection]?.disabled)this.move(1);break;
      case 'shot':this.run.shot=s.selection;this.remember.shot=s.selection;if(this.run.mode==='practice')this.enter('practice',this.remember.practice);else {this.run.stage=this.run.mode==='extra'?7:1;this.wait(40,()=>this.start());}break;
      case 'practice':if(item){this.run.stage=item.id;this.remember.practice=s.selection;this.wait(40,()=>this.start());}break;
      case 'replay':if(item){this.remember.replay=s.selection;this.enter('replay-stage',0,{replay:this.userReplays()[s.selection]});}break;
      case 'replay-stage':if(item){const replay=s.replay,stage=item.id;this.wait(40,()=>{this.enter('playing',0,{readyAt:0});this.emit('playReplay',{id:replay.id,stage});});}break;
      case 'music-room':if(item){
        this.remember.music=s.selection;
        const heard=item.id==='th12_01'||this.profile.data?.music?.includes?.(item.id);
        //449b61..449bbe warns once before playing an unheard track. The next
        //confirm accepts that warning; selecting a track alone does not play it.
        if(!heard&&!s.musicWarning){s.musicWarning=true;s.notice='この曲はまだゲーム中で聴いていません。もう一度決定すると再生します。';this.emit('musicWarning',{track:item.id});break;}
        s.musicWarning=false;s.notice='';this.emit('menuMusic',{track:item.id,musicRoom:true});s.playingTrack=item.id;
      }break;
      case 'options':if(s.selection===2)this.enter('key-config',0);else if(s.selection===3){this.settings={bgmVolume:100,seVolume:80,keyMap:{...DEFAULT_KEYS}};this.settingsChanged();}else if(s.selection===4)this.returnTitle();break;
      case 'key-config':if(s.selection<5)s.captureKey=['shoot','bomb','focus','pause','skip'][s.selection];else if(s.selection===5){this.settings.keyMap={...DEFAULT_KEYS};this.settingsChanged();}else this.enter('options',2);break;
      case 'quit':if(item?.id==='quit'){this.enter('closed',0,{readyAt:0});this.emit('quit');}else this.openTitle();break;
      case 'closed':this.openTitle();break;
      case 'pause':if(item?.id==='resume')this.resume();else this.enter('pause-confirm',1,{context:s.context,pauseSelection:s.selection,pauseAction:item?.id,readyAt:0});break;
      case 'pause-confirm':if(item?.id==='no')this.enter('pause',s.pauseSelection,{context:s.context,readyAt:0});else this.pauseAction(s.pauseAction,s.context);break;
      case 'continue':if(item?.id==='continue'){this.enter('playing',0,{readyAt:0});this.emit('continue',{...s.context});}else if(item?.id==='retry')this.pauseAction('retry',s.context);else if(item?.id==='save-exit')this.emit('saveExit');else this.finish({kind:'gameover',...s.context});break;
      case 'player-data':if(s.dataPage===1){s.spellPage=(s.spellPage||0)+1;}else {s.dataPage=((s.dataPage||0)+1)%3;s.spellPage=0;}break;
      case 'result': {
        const result=s.result;
        if(result.mode==='practice'||result.ranked===false||result.continues>0)this.openReplaySave(result);
        else this.enter('name',90,{result,name:String(this.profile.data?.lastName||'PLAYER').slice(0,8),readyAt:0});break;
      }
      case 'name':if(item?.id==='end')this.completeName();else if(item?.id==='delete')s.name=s.name.slice(0,-1);else if(s.name.length<8)s.name+=item?.id==='space'?' ':item?.label||'';break;
      case 'replay-save':if(item?.id==='cancel')this.returnTitle();else if(item){this.emit('saveReplay',{slot:item.id,name:s.name||this.profile.data?.lastName||'PLAYER',result:s.result});this.returnTitle();}break;
    }
  }
  pauseAction(action,context) {
    if(action==='retry'){this.enter('playing',0,{readyAt:0});this.emit('retry',{...context});}
    else if(action==='save-exit')this.emit('saveExit');
    else this.returnTitle();
  }
  completeName() {const {name,result}=this.state;this.emit('saveName',{name:name.trim()||'PLAYER',result});this.openReplaySave(result,name);}
  openReplaySave(result,name) {if(result.canSaveReplay===false){this.returnTitle();return;}this.enter('replay-save',0,{result,name,readyAt:7});}
  view() {return {...this.state,choices:this.state.choices.map(c=>({...c})),settings:clone(this.settings),run:{...this.run}};}
}
