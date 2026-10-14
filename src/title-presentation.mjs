import {AnimationSampler,MenuAnimation} from './animation.mjs';
import {MENU_SCRIPTS,musicComment} from './menu-resources.mjs';
import {CHARACTERS,DIFFICULTIES} from './title-scenes.mjs';
import {nativeQuadCorners} from './renderer.mjs';

const MENU_ROOTS={title:0,options:1,'key-config':2,difficulty:131,character:143};
const HEADINGS={difficulty:110,character:111,shot:112,replay:113,'replay-stage':113,'player-data':114,'music-room':115,'replay-save':117,practice:118};
const ascii=(text,x,y,colorARGB=0xffffffff,font=1)=>({text:String(text),x,y,font,colorARGB,scaleX:1,scaleY:1,alignX:1,alignY:1});
export const MANUAL_PAGES=[
  ['操作方法','方向キー：移動　Z：ショット　X：スペルカード','Shift：低速移動・当たり判定表示　Esc：一時停止','Ctrl：会話の早送り　リプレイ再生中は4倍速','キー設定は Option → Key Config から変更できます。'],
  ['ゲームの進め方','Start：1～6面の連続プレイ。Extra Start：Extraのプレイ。','Practice Start：到達したステージを練習します。','ミスすると残機を消費します。残機がなくなるとゲームオーバー。','コンティニューは現在のステージの初めから再開します。'],
  ['ベントラーとUFO','赤・青・緑のベントラーを3つ集めるとUFOが現れます。','同じ色3つ、または3色1つずつで出現します。','UFOにアイテムを吸収させ、制限時間内に撃破しましょう。','上部の自動回収ラインに入るとアイテムが自機を追います。'],
  ['スペルカード','Xキーでスペルカードを使用します。被弾直後も決死の機会があります。','ボスのスペルカードには残り時間と獲得ボーナスが表示されます。','ノーミス・ノーボムで撃破するとスペルカードボーナスを獲得。','取得履歴は Player Data とカード表示に記録されます。'],
  ['リプレイと記録','プレイ後に名前とリプレイを保存できます。','Replayから保存したリプレイを選び、ステージを指定して再生。','データはこのブラウザに保存され、サーバーへ送信されません。','この手引きは移植版の案内です。原作付属マニュアルの転載ではありません。']
];

export class TitlePresentation {
  constructor(renderer,resources,profile){
    this.renderer=renderer;this.resources=resources;this.profile=profile;this.sampler=new AnimationSampler(renderer.atlases);this.bindings=new Map();this.serial=-1;this.intro=false;
  }
  bind(key,script,bank='title'){const vm=new MenuAnimation(this.sampler,bank,script);this.bindings.set(key,vm);return vm;}
  remove(key){this.bindings.get(key)?.interrupt(1);this.bindings.delete(key);}
  enter(state){
    this.serial=state.serial;this.scene=state.scene;
    if(state.scene==='pause-confirm'&&this.bindings.has('popup')){
      this.bindings.get('popup').interrupt(14,{immediate:true});return;
    }
    for(const key of [...this.bindings.keys()])if(!['background','title-intro','version'].includes(key))this.remove(key);
    if(state.scene==='title'){
      this.bindings.clear();this.bind('background',99);this.bind('title-intro',106);this.bind('version',0,'title_v');this.intro=!!state.intro;
      if(!state.intro){this.bindings.get('background').interrupt(2,{immediate:true});for(let i=0;i<180;i++)for(const vm of this.bindings.values())vm.tick();this.bind('pane',0);}
    }else if(['pause','pause-confirm','continue'].includes(state.scene)){
      const vm=this.bind('popup',state.scene==='continue'?111:state.context?.mode==='replay'?103:102,'front');
      vm.interrupt(3,{immediate:true});
      if(state.scene==='pause-confirm')vm.interrupt(14,{immediate:true});
    }else if(!['playing','demo','result','name'].includes(state.scene)){
      this.remove('title-intro');if(!this.bindings.has('background'))this.bind('background',99);this.bind('select-background',107);this.intro=false;
      const root=MENU_ROOTS[state.scene]??(state.scene==='shot'?175+state.run.character+(state.run.mode==='extra'?3:0):null);
      if(root!==null&&root!==undefined)this.bind('pane',root);
      if(HEADINGS[state.scene]!==undefined)this.bind('heading',HEADINGS[state.scene]);
      if(state.scene==='music-room')for(const id of MENU_SCRIPTS.music.headers)this.bind('music-'+id,id);
      if(state.scene==='player-data')for(const id of [...MENU_SCRIPTS.result.headers,...MENU_SCRIPTS.result.totals])this.bind('result-'+id,id);
    }
  }
  select(state,event){
    const popup=this.bindings.get('popup');
    if(popup){
      if(state.scene==='pause-confirm'){
        for(const script of [100,101])popup.interrupt(3,{script,immediate:true});
        popup.interrupt((event.type==='ready'?25:15)+state.selection,{script:100+state.selection});
      }else{
        popup.interrupt(3,{immediate:true});
        const row=state.context?.mode==='replay'&&state.selection===2?3:state.selection;
        popup.interrupt((event.type==='ready'?17:7)+row);
        if(state.choices.some(c=>c.disabled))popup.interrupt(14,{script:state.scene==='continue'?109:92});
      }
      return;
    }
    const pane=this.bindings.get('pane');if(!pane)return;
    pane.interrupt(event.resetInterrupt??3,{immediate:true});pane.interrupt(event.selectionInterrupt??7+state.selection);
    if(state.scene==='title'||state.scene==='options'||state.scene==='key-config'){
      const count=state.choices.length,first=state.scene==='title'?3:state.scene==='options'?19:45,other=state.scene==='title'?11:state.scene==='options'?24:52;
      for(let i=0;i<count;i++)if(i!==state.selection){const code=i<state.selection?30:31;pane.interrupt(code,{script:first+i});pane.interrupt(code,{script:other+i});}
      for(let i=0;i<count;i++)if(state.choices[i].disabled){pane.interrupt(27,{script:first+i});pane.interrupt(27,{script:other+i});}
    }
  }
  tick(state){
    if(this.serial!==state.serial)this.enter(state);
    if(state.scene==='title'&&this.intro&&state.age===180)this.bind('pane',0);
    for(const event of state.events??[])if(event.type==='ready'||event.type==='selection')this.select(state,event);
    const pane=this.bindings.get('pane'),run=state.run,cleared=(character,shot,difficulty)=>!!this.profile.data.clears[`${character*2+shot}:${difficulty}`];
    if(pane&&state.scene==='difficulty')for(let d=0;d<4;d++)if(!cleared(run.character,run.shot,d))pane.interrupt(6,{script:170+d});
    if(pane&&state.scene==='character')for(let c=0;c<3;c++)if(![0,1].some(s=>cleared(c,s,run.difficulty)))pane.interrupt(6,{script:136+c});
    if(pane&&state.scene==='shot')for(let s=0;s<2;s++)if(!cleared(run.character,s,run.difficulty))pane.interrupt(6,{script:168+s});
    if(state.scene==='options'){
      for(const [row,value]of [state.settings.bgmVolume,state.settings.seVolume].entries())for(let i=0;i<3;i++){
        const digit=Number(String(value).padStart(3,'0')[i]);pane?.setSprite(29+row*8+i,44+digit);pane?.setSprite(33+row*8+i,55+digit);
      }
    }
    for(const vm of this.bindings.values())vm.tick();
    if(state.scene==='music-room')this.updateMusic(state);
  }
  updateMusic(state){
    //211..232 are dynamic text VM slots, not static song-title textures.
  }
  titleLogo(code){this.bindings.get('background')?.interrupt(code,{immediate:true});}
  hitTest(state,x,y){
    const ids=new Map(),scene=state.scene,put=(scripts,row)=>scripts.forEach(id=>ids.set(id,row));
    if(scene==='title')state.choices.forEach((_,i)=>put([3+i,11+i],i));
    else if(scene==='options')state.choices.forEach((_,i)=>put([19+i,24+i],i));
    else if(scene==='key-config')state.choices.forEach((_,i)=>put([45+i,52+i],i));
    else if(scene==='difficulty')state.choices.forEach((_,i)=>put([126+i],i));
    else if(scene==='character')state.choices.forEach((_,i)=>put([133+i,140+i],i));
    else if(scene==='shot'){const loadout=state.run.character+(state.run.mode==='extra'?3:0);state.choices.forEach((_,i)=>put([144+loadout*2+i,181+loadout*2+i],i));}
    else if(scene==='pause')state.choices.forEach((_,i)=>put([(state.context?.mode==='replay'?94:90)+i],i));
    else if(scene==='pause-confirm')state.choices.forEach((_,i)=>put([100+i],i));
    else if(scene==='continue')state.choices.forEach((_,i)=>put([107+i],i));
    const bank=['pause','pause-confirm','continue'].includes(scene)?'front':'title';
    for(const vm of this.bindings.values())for(const a of vm.poses()){
      if(a.name!==bank||!ids.has(a.script)||!a.visible||a.alpha<.05)continue;
      const sprite=this.renderer.atlases[bank]?.sprites[a.sprite];if(!sprite)continue;
      const corners=nativeQuadCorners(a.x,a.y,sprite.w*a.sx,sprite.h*a.sy,a.rotation??0,a.anchorX,a.anchorY,a.mode),xs=corners.map(p=>p[0]),ys=corners.map(p=>p[1]);
      if(x>=Math.min(...xs)-4&&x<=Math.max(...xs)+4&&y>=Math.min(...ys)-4&&y<=Math.max(...ys)+4)return ids.get(a.script);
    }
    if(scene==='music-room'&&x>=56&&x<=576&&y>=96&&y<296)return (state.musicTop||0)+Math.floor((y-96)/20);
    if(scene==='name'&&x>=40&&x<612&&y>=160&&y<349)return Math.floor((y-160)/27)*13+Math.floor((x-40)/44);
    const list=scene==='replay-save'?[90,65,14,250]:scene==='replay'?[48,98,18,576]:['replay-stage','practice'].includes(scene)?[90,95,24,576]:scene==='quit'?[168,220,36,528]:null;
    if(list&&x>=list[0]&&x<=list[3]&&y>=list[1])return Math.floor((y-list[1])/list[2])+(scene==='replay'?Math.floor(state.selection/15)*15:0);
    return -1;
  }
  poses(){return [...this.bindings.values()].flatMap(vm=>vm.poses().map(a=>({a,x:0,y:0,alpha:1})));}
  texts(state){
    const output=[],scene=state.scene,selected=state.selection,run=state.run;
    const line=(text,x,y,color=0xffffff,size=14,width=570,height=20)=>output.push({text,x,y,color,size,width,height});
    const list=(choices=state.choices,from=0,to=choices.length,x=90,y=95,step=24)=>{
      for(let i=from;i<to;i++){const item=choices[i];if(!item)continue;line((i===selected?'▶ ':'  ')+item.label,x,y+(i-from)*step,item.disabled?0x777777:i===selected?0xffff88:0xffffff);}
    };
    if(scene==='key-config'){
      ['shoot','bomb','focus','pause','skip'].forEach((key,i)=>output.push(ascii(state.captureKey===key?'PRESS KEY':state.settings.keyMap[key],540,180+i*18)));
    }else if(scene==='practice'){list();line(`${CHARACTERS[run.character]} ${run.shot?'B':'A'}　${DIFFICULTIES[run.difficulty]}`,76,55);}
    else if(scene==='replay'){
      line('Name          Replay                         Player / Level',55,68,0xffffff,12);
      const from=Math.floor(selected/15)*15;
      for(let i=from;i<Math.min(from+15,state.choices.length);i++){const r=state.choices[i],meta=this.profile.corpus?.find(m=>m.id===r.id);line((i===selected?'▶ ':'  ')+r.label,48,98+(i-from)*18,i===selected?0xffff88:0xffffff,12);}
      if(!state.choices.length)line('No replay data',96,120);
    }else if(scene==='replay-stage'){list();line(state.replay?.id??'',60,55);}
    else if(scene==='music-room'){
      const from=state.musicTop??0;
      for(let i=from;i<Math.min(from+10,state.choices.length);i++){
        const item=state.choices[i],heard=item.id==='th12_01'||this.profile.data.music.includes(item.id),title=heard?item.label:`No.${i+1} ??????`;
        line(title,i===selected?60:64,96+(i-from)*20,i===selected?0xffff88:0xffffff,16,512,20);
      }
      const comment=musicComment(this.resources,state.playingTrack??'th12_01');
      if(comment)comment.lines.forEach((text,i)=>line(text,64,320+i*18,0xffffff,16,512,18));
    }else if(scene==='player-data'){
      const c=selected===30?undefined:Math.floor(selected/10),shot=selected===30?undefined:selected%10>=5?1:0,difficulty=selected===30?undefined:selected%5;
      const filter={character:c,shot,difficulty},page=state.dataPage||0;
      line(state.choices[selected]?.label??'Total',68,55,0xffff88,16);
      line(['Score Ranking','Spell Card History','Play Data'][page],68,84,0xffffff,16);
      if(page===0){
        const rows=this.profile.rankings(c,shot,difficulty);
        rows.forEach((r,i)=>output.push(ascii(String(i+1).padStart(2,'0')+'  '+r.name.padEnd(8,' ')+'  '+String(r.score).padStart(10,' ')+'  '+(r.clear?'Clear':'Stage '+r.stage),64,126+i*22)));
        if(!rows.length)line('No score data',90,126);
      }else if(page===1){
        const rows=this.profile.spellRecords(filter),pages=Math.max(1,Math.ceil(rows.length/10)),first=((state.spellPage||0)%pages)*10;
        rows.slice(first,first+10).forEach((r,i)=>{line(`${String(r.id+1).padStart(3,'0')}  ${r.title||'Spell Card'}   ${r.captures} / ${r.attempts}`,60,126+i*24,0xffffff,13,550);});
        if(!rows.length)line('No spell card data',90,126);line(`Z : Next  ${Math.floor(first/10)+1} / ${pages}`,80,408,0xffffff,12);
      }else{
        const s=this.profile.dataSummary(filter);
        [`High Score : ${s.bestScore.toLocaleString('en-US')}`,`Saved Plays : ${s.runs}`,`Game Clear : ${s.noContinueClears}`,`Practice : ${s.practiceRuns}`,`Continue : ${s.continueRuns}`,`Spell Card : ${s.spellCaptures} / ${s.spellAttempts}`,`Play Time : ${Math.floor(s.playSeconds/3600)}:${String(Math.floor(s.playSeconds/60)%60).padStart(2,'0')}:${String(Math.floor(s.playSeconds)%60).padStart(2,'0')}`].forEach((text,i)=>line(text,80,126+i*30));
      }
      line('↑ / ↓ : Player・Level  ← / → : Page  X : Return',64,448,0xffffff,12);
    }else if(scene==='manual'){
      const page=MANUAL_PAGES[selected]??MANUAL_PAGES[0];line(page[0],62,60,0xffff88,22);
      page.slice(1).forEach((text,i)=>line(text,48,125+i*42,0xffffff,15,550,36));line('← / → : Page　 X / F1 : Return',52,448,0xffffff,12);
    }else if(scene==='closed'){
      line('ゲームを終了しました。',160,200,0xffffff,22);line('Z / Enter : Title',220,258);
    }else if(scene==='quit'){
      line('Quit Game ?',180,128,0xffff88,24,360,32);list(state.choices,0,state.choices.length,168,220,36);
    }else if(scene==='result'){
      const r=state.result;line(r.clear?'Game Clear':'Game Over',180,110,0xffff88,26);line('Score   '+Number(r.score??0).toLocaleString('en-US'),165,192,0xffffff,22);line(`${CHARACTERS[r.character??0]} ${r.shot?'B':'A'}  ${DIFFICULTIES[r.difficulty]??'Extra'}`,165,235);line('Z : Next',250,340);
    }else if(scene==='name'){
      line('Enter Your Name',180,62,0xffff88,24);output.push(ascii(state.name.padEnd(8,'_'),265,112,0xffffffff,0));
      const columns=13;state.choices.forEach((item,i)=>line(item.label,40+(i%columns)*44,160+Math.floor(i/columns)*27,i===selected?0xffff88:0xffffff,12,44,20));
    }else if(scene==='replay-save'){list(state.choices,0,state.choices.length,90,65,14);line('Save Replay',310,100,0xffff88,22);line('Z : Save　 X : Cancel',310,148,0xffffff,14,290);}
    if(state.notice)line(state.notice,64,412,0xffaaaa,13);
    return output;
  }
  render(state,fps){
    const popup=['pause','pause-confirm','continue'].includes(state.scene),overlay=popup||['result','name'].includes(state.scene);
    const poses=popup?[...this.bindings.entries()].filter(([k])=>k.startsWith('popup')).flatMap(([,vm])=>vm.poses().map(a=>({a,x:0,y:0,alpha:1}))):overlay?[]:this.poses();
    this.renderer.renderMenu(poses,this.texts(state),fps,{overlay,shade:popup?.35:overlay?.65:0});
  }
}
