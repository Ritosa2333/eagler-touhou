import {AnimationSampler,MenuAnimation} from './animation.mjs';
import {MENU_SCRIPTS,musicComment} from './menu-resources.mjs';
import {CHARACTERS,DIFFICULTIES} from './title-scenes.mjs';
import {nativeQuadCorners} from './renderer.mjs';
import {nativeDataHeaderScripts,nativePlayerDataScripts,nativeNameHeaderScripts,nativeNameDescriptors,nativeScoreDescriptors,nativePauseScoreDescriptors,nativeDataTotalDescriptors,nativeSpellDescriptors,NATIVE_DATA_LAYOUT} from './native-data-presentation.mjs';
import {nativeReplayListDescriptors,nativeReplayStageDescriptors,nativePracticeDescriptors,nativeReplayNameDescriptors,nativePauseReplayListDescriptors,nativePauseReplayNameDescriptors,NATIVE_REPLAY_LAYOUT} from './native-replay-presentation.mjs';

const MENU_ROOTS={title:0,options:1,'key-config':2,difficulty:131,character:143};
const HEADINGS={difficulty:110,character:111,shot:112,replay:113,'replay-stage':113,'player-data':114,'music-room':115,'replay-save':117,practice:118};
const pausedName=state=>state.scene==='name'&&state.nameContext==='pause';
const popupPage=state=>['pause','pause-confirm','continue','result-menu'].includes(state.scene)||pausedName(state)||state.scene==='replay-save'&&state.nameContext==='pause';
const ascii=(text,x,y,colorARGB=0xffffffff,font=1)=>({text:String(text),x,y,font,colorARGB,scaleX:1,scaleY:1,alignX:1,alignY:1});
export const MANUAL_PAGES=[
  ['操作方法','方向キー：移動　Z：ショット　X：スペルカード','Shift：低速移動・当たり判定表示　Esc：一時停止','Ctrl：会話の早送り　リプレイ再生中は4倍速','ゲームパッドのボタンは Option → Key Config で設定できます。'],
  ['ゲームの進め方','Start：1～6面の連続プレイ。Extra Start：Extraのプレイ。','Practice Start：到達したステージを練習します。','ミスすると残機を消費します。残機がなくなるとゲームオーバー。','コンティニューは現在のステージの初めから再開します。'],
  ['ベントラーとUFO','赤・青・緑のベントラーを3つ集めるとUFOが現れます。','同じ色3つ、または3色1つずつで出現します。','UFOにアイテムを吸収させ、制限時間内に撃破しましょう。','上部の自動回収ラインに入るとアイテムが自機を追います。'],
  ['スペルカード','Xキーでスペルカードを使用します。被弾直後も決死の機会があります。','ボスのスペルカードには残り時間と獲得ボーナスが表示されます。','ノーミス・ノーボムで撃破するとスペルカードボーナスを獲得。','取得履歴は Player Data とカード表示に記録されます。'],
  ['リプレイと記録','プレイ後に名前とリプレイを保存できます。','Replayから保存したリプレイを選び、ステージを指定して再生。','データはこのブラウザに保存され、サーバーへ送信されません。','この手引きは移植版の案内です。原作付属マニュアルの転載ではありません。']
];

export class TitlePresentation {
  constructor(renderer,resources,profile){
    this.renderer=renderer;this.resources=resources;this.profile=profile;this.sampler=new AnimationSampler(renderer.atlases);this.bindings=new Map();this.retiring=[];this.serial=-1;this.intro=false;this.scene=null;this.heading=null;
  }
  bind(key,script,bank='title'){if(this.bindings.has(key))this.remove(key);const vm=new MenuAnimation(this.sampler,bank,script);this.bindings.set(key,vm);return vm;}
  remove(key){const vm=this.bindings.get(key);if(vm){if(!vm.retirementStarted)vm.interrupt(1);vm.retirementStarted=true;this.retiring.push({vm,age:0});this.bindings.delete(key);}}
  preservePane(key){const pane=this.bindings.get('pane');if(pane){this.remove(key);this.bindings.set(key,pane);this.bindings.delete('pane');}}
  selectBackdrop(){
    if(!this.bindings.has('background'))this.bind('background',99);
    this.remove('title-intro');this.remove('version');
    if(!this.bindings.has('select-background'))this.bind('select-background',107);
  }
  setHeading(script){if(this.heading===script&&this.bindings.has('heading'))return;this.remove('heading');this.heading=script;if(script!==undefined)this.bind('heading',script);}
  enter(state){
    const previous=this.scene;this.serial=state.serial;this.scene=state.scene;this.initialSelection=false;this.pendingPopupScript=null;
    if(state.scene==='pause-confirm'&&this.bindings.has('popup')){
      const row=state.context?.mode==='replay'?94+state.pauseSelection:90+state.pauseSelection;
      this.bindings.get('popup').interrupt(6,{script:row});return;
    }
    if(previous==='pause-confirm'&&state.scene==='pause'&&this.bindings.has('popup')){
      this.bindings.get('popup').interrupt(7+state.selection);this.initialSelection=true;return;
    }
    if(previous==='name'&&['continue','result-menu'].includes(state.scene)&&this.bindings.has('popup-backdrop')){
      const vm=this.bind('popup',state.scene==='continue'?111:state.failed?117:116,'front');vm.interrupt(3,{immediate:true});vm.interrupt(7+state.selection,{immediate:true});this.initialSelection=true;return;
    }
    if(popupPage(state)&&state.nameContext==='pause'&&this.bindings.has('popup-backdrop')&&['name','continue','result-menu','replay-save'].includes(previous)){
      this.remove('popup');return;
    }
    if(previous==='difficulty'&&state.scene==='character')this.preservePane('difficulty-card');
    else if(previous==='character'&&state.scene==='shot')this.preservePane('character-portrait');
    else if(previous==='shot'&&state.scene==='practice')this.preservePane('practice-shot');
    else this.remove('pane');
    if(previous==='practice'&&state.scene!=='practice')this.remove('practice-shot');
    for(const key of [...this.bindings.keys()])if(key.startsWith('popup')||key.startsWith('music-')||key.startsWith('track-')||key.startsWith('comment-')||key.startsWith('data-')||key==='practice-highlight')this.remove(key);
    if(state.scene==='title'){
      this.setHeading(undefined);this.remove('difficulty-card');this.remove('character-portrait');
      if(!this.bindings.has('background'))this.bind('background',99);
      this.intro=!!state.intro;
      if(state.intro){this.remove('select-background');if(!this.bindings.has('title-intro'))this.bind('title-intro',106);}
      else{
        if(!['options','key-config'].includes(previous))this.bindings.get('background').interrupt(2,{immediate:true});
        if(!this.bindings.has('title-intro')&&!this.bindings.has('select-background'))this.bind('select-background',107);
        this.bind('pane',0);this.positionMenuRows(state);this.bind('version',0,'title_v');
      }
    }else if(pausedName(state)||state.scene==='replay-save'&&state.nameContext==='pause'){
      this.setHeading(undefined);this.bind('popup-backdrop',75,'text');this.capturePause=true;
    }else if(['pause','pause-confirm','continue','result-menu'].includes(state.scene)){
      const script=state.scene==='continue'?111:state.scene==='result-menu'?state.failed?117:116:state.context?.mode==='replay'?103:102;
      const vm=state.scene==='result-menu'&&state.phase==='in'?null:this.bind('popup',script,'front');
      if(!vm)this.pendingPopupScript=script;
      this.bind('popup-backdrop',75,'text');this.capturePause=true;
      vm?.interrupt(3,{immediate:true});
    }else if(['playing','demo','ending','closed'].includes(state.scene)){
      this.setHeading(undefined);this.remove('difficulty-card');this.remove('character-portrait');
    }else if(state.scene!=='manual'){
      if(!['options','key-config'].includes(state.scene))this.selectBackdrop();this.intro=false;
      if(state.scene==='difficulty'){this.remove('difficulty-card');this.remove('character-portrait');}
      if(state.scene==='character')this.remove('character-portrait');
      const root=state.scene==='difficulty'&&state.run.mode==='extra'?132:MENU_ROOTS[state.scene]??(state.scene==='shot'?175+state.run.character+(state.run.mode==='extra'?3:0):null);
      if(root!==null&&root!==undefined)this.bind('pane',root);
      this.setHeading(state.scene==='name'?(state.nameKind==='replay'?117:116):HEADINGS[state.scene]);
      if(['difficulty','character','shot'].includes(state.scene)){this.select(state,{type:'ready',selectionInterrupt:17+state.selection});this.initialSelection=true;}
      if(state.scene==='practice')this.bind('practice-highlight',120);
      if(state.scene==='music-room'){
        for(const id of MENU_SCRIPTS.music.headers)this.bind('music-'+id,id);
        for(let i=0;i<state.choices.length;i++){const vm=this.bind('track-'+i,211+i);vm.setText(211+i,{text:this.trackTitle(state,i),width:512,height:18});vm.interrupt(i===state.selection?2:3);}
        for(let i=0;i<8;i++)this.bind('comment-'+i,41+i,'text');
        this.musicSelection=state.selection;this.musicCommentId=null;this.commentUpdate=null;
      }
      if(state.scene==='player-data')this.updateData(state,true);
      if(state.scene==='name'&&state.nameKind!=='replay')for(const id of nativeDataHeaderScripts(state.result.character,state.result.shot,state.result.difficulty))this.bind('data-header-'+id,id);
    }
  }
  select(state,event){
    const popup=this.bindings.get('popup');
    if(popup){
      if(state.scene==='pause-confirm'){
        popup.interrupt(15+state.selection);
      }else{
        popup.interrupt(3,{immediate:true});
        const row=state.context?.mode==='replay'&&state.selection===2?3:state.selection;
        popup.interrupt((event.type==='ready'?17:7)+row);
        for(let i=0;i<state.choices.length;i++)if(state.choices[i].disabled)popup.interrupt(14,{script:state.scene==='continue'?107+i:state.scene==='result-menu'?113+i:state.context?.mode==='replay'?94+(i===2?3:i):90+i});
      }
      return;
    }
    const pane=this.bindings.get('pane');if(!pane)return;
    pane.interrupt(event.resetInterrupt??3,{immediate:true});pane.interrupt(event.selectionInterrupt??7+state.selection);
    this.positionMenuRows(state);
  }
  positionMenuRows(state){
    const pane=this.bindings.get('pane');if(!pane)return;
    if(state.scene==='title'||state.scene==='options'||state.scene==='key-config'){
      const count=state.choices.length,first=state.scene==='title'?3:state.scene==='options'?19:45,other=state.scene==='title'?11:state.scene==='options'?24:52;
      //43ffcb..4401a7 executes the title positioning IRQ before queuing
      //the lock IRQ29. Deferring both replaces script12's saved return state.
      for(let i=0;i<count;i++)if(i!==state.selection){const code=i<state.selection?30:31,immediate=state.scene==='title';pane.interrupt(code,{script:first+i,immediate});pane.interrupt(code,{script:other+i,immediate});}
      for(let i=0;i<count;i++)if(state.choices[i].disabled){pane.interrupt(state.scene==='title'&&i===1?29:27,{script:first+i});pane.interrupt(state.scene==='title'&&i===1?29:27,{script:other+i});}
    }
  }
  confirmAnimation(state){
    const vm=this.bindings.get('pane');if(!vm)return;
    if(state.scene==='title'){
      vm.interrupt(6);
      if(state.selection<=5){this.bindings.get('background')?.interrupt(3);this.remove('title-intro');this.remove('version');if(!this.bindings.has('select-background'))this.bind('select-background',107);}
    }else if(state.scene==='difficulty'){
      vm.interrupt(6);vm.interrupt(2,{script:state.run.mode==='extra'?125:121+state.selection});
    }else if(state.scene==='character'){
      for(const script of [140+state.selection,133+state.selection,95,96])vm.interrupt(6,{script});
      for(let i=0;i<3;i++){if(i!==state.selection)vm.interrupt(1,{script:133+i});vm.interrupt(1,{script:136+i});}
    }else if(state.scene==='shot'){
      const group=state.run.character+(state.run.mode==='extra'?3:0);vm.interrupt(6,{script:144+group*2+state.selection});
    }else vm.interrupt(6);
  }
  tick(state){
    if(this.serial!==state.serial)this.enter(state);
    if(state.scene==='title'&&this.intro&&state.age===180){this.bind('pane',0);this.positionMenuRows(state);this.bind('version',0,'title_v');}
    if(['pause','continue','result-menu'].includes(state.scene)&&state.age===10&&state.phase==='in'){
      if(this.pendingPopupScript!==null){this.bind('popup',this.pendingPopupScript,'front').interrupt(3,{immediate:true});this.pendingPopupScript=null;}
      this.select(state,{type:'selection',selectionInterrupt:7+state.selection});this.initialSelection=true;
    }
    if(state.scene==='pause-confirm'&&state.age===20)this.bindings.get('popup')?.interrupt(14);
    for(const event of state.events??[]){
      if(event.type==='ready'&&!this.initialSelection||event.type==='selection')this.select(state,event);
      else if(event.type==='confirm')this.confirmAnimation(state);
      else if(event.type==='cancel'&&state.scene==='difficulty'){const vm=this.bindings.get('pane');if(vm){vm.interrupt(1);vm.retirementStarted=true;}}
      else if(event.type==='resume'){this.bindings.get('popup')?.interrupt(1);this.bindings.get('popup-backdrop')?.interrupt(1);}
      else if(event.type==='retry-shortcut')this.bindings.get('popup')?.interrupt(6,{script:93});
      else if(event.type==='confirm-no')this.bindings.get('popup')?.interrupt(6,{script:101});
    }
    state.events=[];
    const pane=this.bindings.get('pane'),run=state.run,cleared=(character,shot,difficulty)=>!!this.profile.data.clears[`${character*2+shot}:${difficulty}`];
    if(pane&&state.scene==='difficulty')for(let d=0;d<5;d++)pane.setVisible(170+d,cleared(run.character,run.shot,d));
    if(pane&&state.scene==='character')for(let c=0;c<3;c++)pane.setVisible(136+c,[0,1].some(s=>cleared(c,s,run.difficulty)));
    if(pane&&state.scene==='shot')for(let s=0;s<2;s++)pane.setVisible(168+s,cleared(run.character,s,run.difficulty));
    if(state.scene==='options'){
      for(const [row,value]of [state.settings.bgmVolume,state.settings.seVolume].entries())for(let i=0;i<3;i++){
        const digit=Number(String(value).padStart(3,'0')[i]);pane?.setSprite(29+row*8+i,44+digit);pane?.setSprite(33+row*8+i,55+digit);
      }
    }
    if(state.scene==='key-config')for(const [row,key]of ['shoot','bomb','focus','pause','skip'].entries()){
      const value=state.joyDraft?.[key]??state.settings.joyButtons[key];
      for(let i=0;i<2;i++){const digit=i?value%10:Math.floor(value/10);pane?.setSprite(59+row*2+i,44+digit);pane?.setSprite(69+row*2+i,55+digit);}
    }
    if(state.scene==='music-room')this.updateMusic(state);
    if(state.scene==='player-data')this.updateData(state);
    for(const vm of this.bindings.values())vm.tick();
    for(const item of this.retiring){item.vm.tick();item.age++;}this.retiring=this.retiring.filter(item=>item.vm.alive&&item.age<120);
  }
  trackTitle(state,index){const item=state.choices[index],heard=item.id==='th12_01'||this.profile.data.music.includes(item.id);return heard?item.label:`No.${index+1} ??????`;}
  updateMusic(state){
    const top=state.musicTop??0;
    for(let i=0;i<state.choices.length;i++){
      const vm=this.bindings.get('track-'+i),script=211+i;vm?.setVisible(script,i>=top&&i<top+10);vm?.setPosition(script,i===state.selection?60:64,96+(i-top)*20,4);
      vm?.setText(script,{text:this.trackTitle(state,i),width:512,height:18});
      if(this.musicSelection!==state.selection)vm?.interrupt(i===state.selection?2:3);
    }
    this.musicSelection=state.selection;
    const id=state.commentTrack??state.playingTrack??'th12_01',commentKey=id+':'+(state.commentSerial??0);
    if(this.musicCommentId!==commentKey){this.musicCommentId=commentKey;this.commentUpdate={comment:state.musicWarning?this.resources.musicWarning:musicComment(this.resources,id),age:0};for(let i=0;i<8;i++)this.bindings.get('comment-'+i)?.interrupt(3);}
    const pending=this.commentUpdate;if(pending){if(pending.age%2===0&&pending.age<16){const i=pending.age/2,vm=this.bindings.get('comment-'+i);vm?.setText(41+i,{text:pending.comment?.lines[i]??'',colorRGB:pending.comment?.colorRGB??0xffffff,width:512,height:17});vm?.interrupt(2);}if(++pending.age>=16)this.commentUpdate=null;}
  }
  updateData(state,force=false){
    const c=Math.floor(state.selection/2),shot=state.selection%2,d=state.dataDifficulty??1,page=state.dataPage??0,key=`${c}:${shot}:${d}:${page}`;
    if(!force&&key===this.dataKey)return;this.dataKey=key;
    const headerKey=`${c}:${shot}:${d}`;
    if(force||headerKey!==this.dataHeaderKey){for(const name of [...this.bindings.keys()])if(name.startsWith('data-header-'))this.remove(name);for(const id of nativePlayerDataScripts(c,shot,d).filter(id=>id!==114))this.bind('data-header-'+id,id);this.dataHeaderKey=headerKey;}
    if(page>0){
      const selected=this.profile.spellRecords({character:c,shot,difficulty:d,includeUnseen:true}),all=this.profile.spellRecords({difficulty:d,includeUnseen:true}),records={};
      for(const r of selected){const cached=this.resources.spellNames?.[r.id],any=all.find(a=>a.id===r.id);records[r.id]={name:r.title||cached?.title||'',nameByteLength:cached?.byteLength,attempts:r.attempts,captures:r.captures,anyAttempts:any?.attempts??0};}
      const rows=nativeSpellDescriptors(d,page,records),visible=new Set();
      for(const row of rows){const name='data-row-'+row.script;visible.add(name);const vm=this.bindings.get(name)??this.bind(name,row.script,row.bank);vm.setText(row.script,{text:row.text,colorRGB:row.colorRGB,width:row.width,height:row.height});}
      for(const name of [...this.bindings.keys()])if(name.startsWith('data-row-')&&!visible.has(name))this.remove(name);
    }else for(const name of [...this.bindings.keys()])if(name.startsWith('data-row-'))this.remove(name);
  }
  titleLogo(code){this.bindings.get('background')?.interrupt(code,{immediate:true});}
  hitTest(state,x,y){
    const ids=new Map(),scene=state.scene,put=(scripts,row)=>scripts.forEach(id=>ids.set(id,row));
    if(scene==='title')state.choices.forEach((_,i)=>put([3+i,11+i],i));
    else if(scene==='options')state.choices.forEach((_,i)=>put([19+i,24+i],i));
    else if(scene==='key-config')state.choices.forEach((_,i)=>put([45+i,52+i],i));
    else if(scene==='difficulty')state.choices.forEach((_,i)=>put([state.run.mode==='extra'?125:126+i],i));
    else if(scene==='character')state.choices.forEach((_,i)=>put([133+i,140+i],i));
    else if(scene==='shot'){const loadout=state.run.character+(state.run.mode==='extra'?3:0);state.choices.forEach((_,i)=>put([144+loadout*2+i,181+loadout*2+i],i));}
    else if(scene==='pause')state.choices.forEach((_,i)=>put([(state.context?.mode==='replay'?94:90)+i],i));
    else if(scene==='pause-confirm')state.choices.forEach((_,i)=>put([100+i],i));
    else if(scene==='continue')state.choices.forEach((_,i)=>put([107+i],i));
    else if(scene==='result-menu')state.choices.forEach((_,i)=>put([113+i],i));
    const bank=['pause','pause-confirm','continue','result-menu'].includes(scene)?'front':'title';
    for(const vm of this.bindings.values())for(const a of vm.poses()){
      if(a.name!==bank||!ids.has(a.script)||!a.visible||a.alpha<.05)continue;
      const sprite=this.renderer.atlases[bank]?.sprites[a.sprite];if(!sprite)continue;
      const corners=nativeQuadCorners(a.x,a.y,sprite.w*a.sx,sprite.h*a.sy,a.rotation??0,a.anchorX,a.anchorY,a.mode),xs=corners.map(p=>p[0]),ys=corners.map(p=>p[1]);
      if(x>=Math.min(...xs)-4&&x<=Math.max(...xs)+4&&y>=Math.min(...ys)-4&&y<=Math.max(...ys)+4)return ids.get(a.script);
    }
    if(scene==='music-room'&&x>=56&&x<=576&&y>=96&&y<296)return (state.musicTop||0)+Math.floor((y-96)/20);
    if(scene==='name'){const gx=pausedName(state)?112:212,gy=pausedName(state)?320:360;if(x>=gx&&x<gx+234&&y>=gy&&y<gy+112)return Math.floor((y-gy)/16)*13+Math.floor((x-gx)/18);}
    const list=scene==='replay-save'&&state.nameContext==='pause'?[48,64,15,416]:['replay-save','replay'].includes(scene)?[58,80,15,608]:scene==='replay-stage'?[220,128,18,440]:scene==='practice'?[296,216,18,530]:null;
    if(list&&x>=list[0]&&x<=list[3]&&y>=list[1])return Math.floor((y-list[1])/list[2])+(scene==='replay'?Math.floor(state.selection/25)*25:0);
    return -1;
  }
  poses(){return [...this.bindings.values(),...this.retiring.map(item=>item.vm)].flatMap(vm=>vm.poses().map(a=>({a,x:0,y:0,alpha:1})));}
  texts(state){
    const output=[],scene=state.scene,selected=state.selection,run=state.run;
    const line=(text,x,y,color=0xffffff,size=14,width=570,height=20)=>output.push({text,x,y,color,size,width,height});
    if(scene==='practice'){
      if(state.phase==='in'||state.phase==='out'&&state.transitionAction==='cancel')return output;
      const rows=state.choices.map(item=>({disabled:item.disabled,scoreStored:Math.trunc((this.profile.rankings(run.character,run.shot,run.difficulty,item.id)[0]?.score??0)/10)}));
      output.push(...nativePracticeDescriptors(rows,{selection:selected,age:state.phase==='out'?state.transitionAge:Math.max(0,state.age-state.readyAt+1),confirming:state.phase==='out'}));
    }else if(scene==='replay')output.push(...nativeReplayListDescriptors(state.replayRecords??[],{page:Math.floor(selected/25),selection:selected%25}));
    else if(scene==='replay-stage')output.push(...nativeReplayStageDescriptors(state.replay,{selection:selected,fileIndex:state.fileIndex??0,page:Math.floor((state.fileIndex??0)/25),age:state.age}));
    else if(scene==='replay-save')output.push(...(state.nameContext==='pause'?nativePauseReplayListDescriptors:nativeReplayListDescriptors)(state.replayRecords??[],{selection:selected,save:true}));
    else if(scene==='player-data'){
      const c=Math.floor(selected/2),shot=selected%2,d=state.dataDifficulty??1;
      if((state.dataPage??0)===0)output.push(...nativeScoreDescriptors(this.profile.rankings(c,shot,d)));
      output.push(...nativeDataTotalDescriptors(this.profile.nativeTotals(c,shot,d)));
    }else if(scene==='name'){
      if(state.nameKind==='replay'){
        const r=state.result;output.push(...(pausedName(state)?nativePauseReplayNameDescriptors:nativeReplayNameDescriptors)({...r,scoreStored:Math.trunc(r.score/10),finalStage:r.clear?8:r.stage,continues:r.continues??0},{slot:state.slot,name:state.name,cursor:state.name.length,selection:selected,age:state.age}));
      }else{
        if(state.age<(state.revealAt??0))return output;
        output.push(...(pausedName(state)?nativePauseScoreDescriptors:nativeScoreDescriptors)(state.ranking??[],{selectedRank:state.rank,nameEntry:true,ranked:state.ranked!==false}));
        output.push(...nativeNameDescriptors({name:state.name,selection:selected,rank:state.rank??0,ranked:state.ranked!==false,context:pausedName(state)?'pause':'title'}));
      }
    }else if(scene==='manual'){
      const page=MANUAL_PAGES[selected]??MANUAL_PAGES[0];line(page[0],62,60,0xffff88,22);
      page.slice(1).forEach((text,i)=>line(text,48,125+i*42,0xffffff,15,550,36));line('← / → : Page　 X / F1 : Return',52,448,0xffffff,12);
    }
    return output;
  }
  render(state,fps){
    const popup=popupPage(state),poses=popup?[...this.bindings.entries()].filter(([k])=>k==='popup').flatMap(([,vm])=>vm.poses().map(a=>({a,x:0,y:0,alpha:1}))):state.scene==='closed'?[]:this.poses();
    if(popup){this.renderer.renderPauseBackdrop(this.bindings.get('popup-backdrop')?.poses()??[],this.capturePause);this.capturePause=false;}
    this.renderer.renderMenu(poses,this.texts(state),fps,{overlay:popup});
    const fade=state.phase==='out'&&state.transitionAction!=='cancel'&&(state.scene==='practice'||state.scene==='replay-stage'||state.scene==='shot'&&state.run.mode!=='practice');
    const fadeAt=state.scene==='replay-stage'?2:10;
    if(fade&&state.transitionAge>=fadeAt)this.renderer.renderMenu([],[],fps,{overlay:true,shade:Math.min(1,(state.transitionAge-fadeAt)/32)});
  }
}

