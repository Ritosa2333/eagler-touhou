import {TOOL,source} from './bridge.mjs';
import {localeIndex} from './menu.mjs';

const CHEATS=[['invulnerable','F1',['无敌','Invulnerability','無敵']],['infiniteLives','F2',['无限残机','Infinite lives','残機無限']],
  ['infiniteBombs','F3',['无限符卡','Infinite bombs','ボム無限']],['infinitePower','F4',['无限火力','Infinite power','パワー無限']],
  ['timeLock','F5',['锁定时间','Time lock','タイムロック']],['autoBomb','F6',['自动决死','Auto deathbomb','自動喰らいボム']],
  ['everlastingBgm','F7',['持续播放BGM','Everlasting BGM','BGM継続']]];
const TEXT={practice:['练习菜单','Practice menu','プラクティスメニュー'],cheats:['作弊','Cheats','チート'],
  advanced:['高级设置','Advanced settings','詳細設定'],tracker:['追踪表','Tracker','トラッカー'],
  fps:['游戏速度','Game speed','ゲーム速度'],free:['自由调整','Free adjustment','自由調整'],
  slow:['录像慢放','Replay slow speed','リプレイ低速'],fast:['录像快进','Replay fast speed','リプレイ早送り'],
  debug:['调试加速（空格）','Debug acceleration (Space)','デバッグ加速（Space）'],
  bonus:['通关奖励','All-clear bonus','クリアボーナス'],apply:['应用','Apply','適用'],
  close:['关闭','Close','閉じる'],replay:['录像播放中，实时作弊已禁用','Live cheats are disabled during replay','リプレイ中はチート無効'],
  assists:['辅助录像保留参数；播放不重施加作弊，可能不同步。','Assisted replays retain parameters; live cheats are not replayed and may desync.','補助リプレイは設定を保存します。チートを再適用せず、ずれる場合があります。'],
  controls:['↑↓选择　←→调整　Shift大步调整　Z/Enter确认　X/Esc返回','↑↓ select · ←→ adjust · Shift larger steps · Z/Enter confirm · X/Esc return','↑↓選択　←→変更　Shift大幅変更　Z/Enter決定　X/Esc戻る'],
};
const CSS=`.th12-practice-root{position:fixed;z-index:80;pointer-events:none;color:#eee;font:14px/1.35 "ＭＳ ゴシック","Eagler Unicode",sans-serif;--practice-scale:1}
.th12-practice-root *{box-sizing:border-box}.th12-practice-panel{position:absolute;background:rgba(12,12,18,.94);border:1px solid #000;box-shadow:0 3px 16px #0008;pointer-events:auto;max-height:100%;overflow:hidden;min-width:0}
.th12-practice-panel header{display:flex;justify-content:space-between;align-items:center;background:#332b53;padding:6px 9px;font-weight:600;min-height:34px}
.th12-practice-panel button{font:inherit;color:inherit;background:#302a43;border:1px solid #000;padding:4px 8px;min-height:30px;cursor:pointer;touch-action:manipulation}
.th12-practice-panel button:focus-visible{outline:2px solid #ffe18a}.th12-practice-panel button:disabled{opacity:.45;cursor:default}
.th12-practice-body{padding:7px;overflow:auto;max-height:calc(100% - 34px);touch-action:pan-y;overscroll-behavior:contain}
.th12-practice-row{display:grid;grid-template-columns:minmax(90px,30%) 28px minmax(0,1fr) 28px;align-items:center;gap:3px;margin:2px 0;min-height:31px}
.th12-practice-row.is-selected{background:#554268}.th12-practice-row .value{overflow-wrap:anywhere;text-align:center;white-space:pre-line}
.th12-practice-row .name{padding:3px}.th12-practice-note{font-size:11px;color:#d6ccdf;margin:6px 0}.th12-practice-error{color:#ffbaba;white-space:pre-wrap}
.th12-practice-check{display:flex;align-items:center;justify-content:space-between;gap:8px;width:100%;text-align:left;margin:3px 0}.th12-practice-check b{color:#ffe18a}.th12-practice-check.is-selected{background:#554268}
.th12-practice-native-menu{right:2%;top:7%;width:67%;height:89%}.th12-practice-cheats{left:1%;top:1%;width:37%;max-height:85%}
.th12-practice-advanced{left:10%;top:4%;width:80%;height:92%}.th12-practice-tracker{right:1%;bottom:1%;width:35%;max-height:55%}
.th12-practice-mobile{top:0!important;bottom:auto!important;width:33.333%!important;max-height:92%;font-size:12px}.th12-practice-mobile header{padding:3px;min-height:31px}.th12-practice-mobile header button{font-size:11px;padding:2px;width:100%}
.th12-practice-mobile.th12-practice-cheats{left:0}.th12-practice-mobile.th12-practice-advanced{left:33.333%;height:auto}.th12-practice-mobile.th12-practice-tracker{left:66.666%;right:auto}
.th12-practice-mobile .th12-practice-body{max-height:calc(80vh - 40px)}.th12-practice-mobile .th12-practice-row{grid-template-columns:1fr 25px 1fr 25px;font-size:11px}
`;
function element(tag,className='',text='') {const node=document.createElement(tag);node.className=className;node.textContent=text;return node;}

// A source-owned parameter/cheat frontend. Every gameplay mutation calls the
// real Wasm Practice owner. The original Pause remains entirely with the game.
export class PracticeOverlay {
  constructor({bridge,canvas,container=document.body,onError=error=>{throw error;},onMenuAction=null,locale='zh-CN'}) {
    this.bridge=bridge;this.canvas=canvas;this.onError=onError;this.onMenuAction=onMenuAction;this.locale=locale;
    this.enabled=false;this.scene='title';this.replay=false;this.menu=null;this.menuReady=false;
    this.desktop={cheats:false,advanced:false,tracker:false};this.mobile={enabled:false,visible:false,cheats:false,advanced:false,tracker:false};
    this.owners=new Map();this.keys=new Set();this.selection=0;this.signature='';this.state=null;this.draft=null;this.freeFps=false;
    this.root=element('div','th12-practice-root');this.root.setAttribute('aria-label','TH12 practice tools');
    this.style=element('style');this.style.textContent=CSS;container.append(this.style,this.root);
    this.resize=()=>this.layout();window.addEventListener('resize',this.resize);
    this.observer=typeof ResizeObserver==='function'?new ResizeObserver(this.resize):null;this.observer?.observe(canvas);
    this.layout();
  }
  label(key){return TEXT[key]?.[localeIndex(this.locale)]??key;}
  layout(){const rect=this.canvas.getBoundingClientRect();Object.assign(this.root.style,{left:rect.left+'px',top:rect.top+'px',width:rect.width+'px',height:rect.height+'px'});}
  configure({enabled=this.enabled,locale=this.locale}={}){
    this.enabled=!!enabled;this.locale=locale;this.bridge.setEnabled(this.enabled);
    if(!this.enabled){this.desktop={cheats:false,advanced:false,tracker:false};this.owners.clear();this.keys.clear();}
    this.signature='';this.render();
  }
  mobilePanels(snapshot={}){
    if(typeof snapshot.thpracMobilePanelsEnabled!=='boolean')return;
    const enabled=snapshot.thpracMobilePanelsEnabled,visible=!!snapshot.thpracMobilePanelsVisible;
    if(enabled&&visible&&(!this.mobile.enabled||!this.mobile.visible))this.mobile={enabled,visible,cheats:false,advanced:false,tracker:false};
    else Object.assign(this.mobile,{enabled,visible});
    this.signature='';this.render();
  }
  update({scene=this.scene,replay=this.replay,menu=null,menuReady=false,onMenuAction=this.onMenuAction}={}){
    this.scene=scene;this.replay=!!replay;this.menu=menu;this.menuReady=menuReady;this.onMenuAction=onMenuAction;
    if(!this.enabled)return;
    try{this.state=this.bridge.snapshot();this.render();}catch(error){this.onError(error);}
  }
  get capturesGameplay(){return this.enabled&&(this.mobile.enabled?this.mobile.visible&&this.mobile.advanced:this.desktop.advanced);}
  get visible(){return this.enabled&&(!!this.menu||this.mobile.enabled&&this.mobile.visible||Object.values(this.desktop).some(Boolean));}
  togglePanel(name){
    if(this.mobile.enabled){if(!this.mobile.visible)return;this.mobile[name]=!this.mobile[name];}
    else this.desktop[name]=!this.desktop[name];
    if(name==='advanced'){this.draft={...(this.state?.speed??{fps:60,slow:15,fast:60,debug:false})};this.selection=0;}
    this.signature='';this.render();
  }
  toggleTool(key){
    if(this.replay||this.state?.replay)return;
    try{this.bridge.setTools((this.state?.tools??0)^TOOL[key]);this.state=this.bridge.snapshot();this.signature='';this.render();}
    catch(error){this.onError(error);}
  }
  invokeMenu(action){if(!action)return;if(!this.onMenuAction)throw Error('Practice menu has no game lifecycle callback');this.onMenuAction(action);}
  advancedRows(){const d=this.draft??this.state?.speed??{fps:60,slow:15,fast:60,debug:false};return [
    {id:'fps',value:`${d.fps} fps`},{id:'free',value:this.freeFps?'✓':'—'},
    {id:'slow',value:`${d.slow} fps`},{id:'fast',value:d.fast>1200?'∞':`${d.fast} fps`},
    {id:'debug',value:d.debug?'✓':'—'},{id:'bonus',value:this.state?.tools&TOOL.allClearBonus?'✓':'—'},
    {id:'apply',value:'Enter'},{id:'close',value:'Esc'}];}
  changeAdvanced(id,delta,fast=false){
    this.draft??={...(this.state?.speed??{fps:60,slow:15,fast:60,debug:false})};const d=this.draft;
    if(id==='fps')d.fps=Math.min(6000,Math.max(60,d.fps+delta*(this.freeFps?1:15)*(fast?10:1)));
    else if(id==='slow')d.slow=Math.min(60,Math.max(1,d.slow+delta));
    else if(id==='fast')d.fast=Math.min(1260,Math.max(60,d.fast+delta*60));
    else if(id==='debug')d.debug=!d.debug;
    else if(id==='free')this.freeFps=!this.freeFps;
    else if(id==='bonus')this.toggleTool('allClearBonus');
    this.signature='';this.render();
  }
  activateAdvanced(id){
    if(id==='close'){this.togglePanel('advanced');return;}
    if(id==='apply'){try{this.bridge.setSpeed(this.draft??this.state.speed);this.state=this.bridge.snapshot();this.signature='';this.render();}catch(error){this.onError(error);}return;}
    this.changeAdvanced(id,1);
  }
  key(code,down=true,{repeat=false}={}){
    if(!this.enabled)return false;
    const was=this.keys.has(code);down?this.keys.add(code):this.keys.delete(code);
    if(!down)return this.capturesGameplay;
    if(was&&!repeat)return this.capturesGameplay;
    if(code==='F12'){if(!repeat)this.togglePanel('advanced');return true;}
    if(code==='Backspace'){if(!repeat)this.togglePanel('cheats');return true;}
    if(code==='Tab'){if(!repeat)this.togglePanel('tracker');return true;}
    const cheat=CHEATS.find(([,key])=>key===code);if(cheat){if(!repeat)this.toggleTool(cheat[0]);return true;}
    if(!this.capturesGameplay)return false;
    const rows=this.advancedRows(),n=rows.length;
    if(code==='ArrowUp')this.selection=(this.selection+n-1)%n;
    else if(code==='ArrowDown')this.selection=(this.selection+1)%n;
    else if(code==='ArrowLeft'||code==='ArrowRight')this.changeAdvanced(rows[this.selection].id,code==='ArrowLeft'?-1:1,this.keys.has('ShiftLeft')||this.keys.has('ShiftRight'));
    else if(code==='KeyZ'||code==='Enter')this.activateAdvanced(rows[this.selection].id);
    else if(code==='Escape'||code==='KeyX')this.togglePanel('advanced');
    this.signature='';this.render();return true;
  }
  button(text,action,{disabled=false}={}){const button=element('button','',text);button.type='button';button.disabled=disabled;button.dataset.thpracAction='1';button.onclick=event=>{event.stopPropagation();if(!button.disabled)action();};return button;}
  panel(name,title,body,{menu=false}={}){
    const panel=element('section','th12-practice-panel th12-practice-'+(menu?'native-menu':name));
    panel.dataset.thpracPanel=name;
    const mobile=this.mobile.enabled&&!menu,expanded=menu||!mobile||this.mobile[name];
    if(mobile)panel.classList.add('th12-practice-mobile');
    const header=element('header');
    if(mobile)header.append(this.button((expanded?'▾ ':'▸ ')+title,()=>this.togglePanel(name)));
    else {header.append(element('span','',title));if(!menu)header.append(this.button('×',()=>this.togglePanel(name)));}
    panel.append(header);
    if(expanded){const content=element('div','th12-practice-body');content.append(...body);panel.append(content);}
    this.root.append(panel);return panel;
  }
  row(label,value,onChange,onActivate,{selected=false,disabled=false}={}){
    const row=element('div','th12-practice-row'+(selected?' is-selected':''));
    row.append(element('span','name',label));
    row.append(this.button('−',()=>onChange(-1),{disabled}));
    row.append(this.button(value,onActivate,{disabled}));row.children[2].className='value';
    row.append(this.button('+',()=>onChange(1),{disabled}));return row;
  }
  render(){
    this.root.hidden=!this.enabled;if(!this.enabled){this.root.replaceChildren();return;}
    const l=localeIndex(this.locale),menuRows=this.menu?.rows()??[];
    const signature=JSON.stringify([this.scene,this.replay,this.locale,this.desktop,this.mobile,this.menuReady,
      menuRows,this.menu?.selection,this.menu?.notice,this.state?.tools,this.state?.tracker,this.state?.speed,this.draft,this.selection]);
    if(signature===this.signature)return;this.signature=signature;
    const previousScroll=new Map([...this.root.querySelectorAll('[data-thprac-panel]')].map(node=>[node.dataset.thpracPanel,node.querySelector('.th12-practice-body')?.scrollTop??0]));
    this.root.replaceChildren();
    if(this.menu&&this.scene==='practice'){
      const body=menuRows.map((row,index)=>this.row(row.label,row.value,delta=>{this.menu.selection=index;this.menu.change(row.id,delta);this.signature='';this.render();},()=>{
        if(!this.menuReady)return;this.menu.selection=index;this.invokeMenu(this.menu.activate(row.id));this.signature='';this.render();
      },{selected:index===this.menu.selection,disabled:!this.menuReady}));
      body.push(element('p','th12-practice-note',this.label('controls')));
      if(this.menu.notice)body.push(element('p','th12-practice-error',this.menu.notice));
      this.panel('practice',this.label('practice'),body,{menu:true});
    }
    for(const name of ['cheats','advanced','tracker']){
      if(this.mobile.enabled?!this.mobile.visible:!this.desktop[name])continue;
      const body=[];
      if(name==='cheats'){
        for(const [key,hotkey,names]of CHEATS){
          const selected=!!(this.state?.tools&TOOL[key]);const button=this.button(`${selected?'☑':'☐'} ${names[l]}　${hotkey}`,()=>this.toggleTool(key),{disabled:this.replay});
          button.className='th12-practice-check';body.push(button);
        }
        body.push(element('p','th12-practice-note',this.label(this.replay?'replay':'assists')));
      }else if(name==='advanced'){
        body.push(...this.advancedRows().map((row,index)=>this.row(this.label(row.id),row.value,delta=>{this.selection=index;this.changeAdvanced(row.id,delta);},()=>{this.selection=index;this.activateAdvanced(row.id);},
          {selected:index===this.selection,disabled:row.id==='bonus'&&this.replay})));
        body.push(element('p','th12-practice-note','THPrac · '+source.revision.slice(0,12)));
        body.push(element('p','th12-practice-note','MIT · Ack 2022 · touhouworldcup/thprac'));
      }else{
        const tracker=this.state?.tracker??{misses:0,bombs:0,collected:[0,0,0],dropped:[0,0,0,0],summoned:[0,0,0,0]};
        const labels=[['Miss','Miss','ミス'],['Bomb','Bomb','ボム'],['收集UFO','Collected UFOs','取得ベントラー'],['出现UFO','Dropped UFOs','出現ベントラー'],['召唤UFO','Summoned UFOs','召喚UFO']];
        const values=[tracker.misses,tracker.bombs,tracker.collected.join(' / '),tracker.dropped.join(' / '),tracker.summoned.join(' / ')];
        labels.forEach((names,index)=>body.push(element('p','th12-practice-note',names[l]+': '+values[index])));
      }
      this.panel(name,this.label(name),body);
    }
    for(const panel of this.root.querySelectorAll('[data-thprac-panel]')){const body=panel.querySelector('.th12-practice-body');if(body)body.scrollTop=previousScroll.get(panel.dataset.thpracPanel)??0;}
    if(this.menu&&this.menu.selection!==this.lastMenuSelection){this.root.querySelector('.th12-practice-native-menu .is-selected')?.scrollIntoView({block:'nearest'});this.lastMenuSelection=this.menu.selection;}
  }
  // thprac-mouse x/y are Runtime viewport CSS coordinates, as in the existing
  // TH11 adapter. A down decides ownership; a game drag cannot capture a panel
  // merely by crossing it later. id/button are optional for legacy callers.
  pointer(event){
    if(!this.enabled)return false;const id=event.id??'mouse';
    if(event.type==='cancel'){const owned=this.owners.has(id);this.owners.delete(id);return owned;}
    if(!Number.isFinite(event.x)||!Number.isFinite(event.y))return false;
    if(event.type==='down'){
      if((event.button??0)!==0)return false;
      const target=document.elementFromPoint(event.x,event.y),panel=target?.closest('.th12-practice-panel');
      if(!panel||!this.root.contains(panel))return false;
      const button=target.closest('button[data-thprac-action]'),scroll=target.closest('.th12-practice-body');
      this.owners.set(id,{button,scroll,x:event.x,y:event.y,scrollTop:scroll?.scrollTop??0,moved:false});return true;
    }
    const owner=this.owners.get(id);if(!owner)return false;
    owner.moved||=Math.hypot(event.x-owner.x,event.y-owner.y)>6;
    if(event.type==='move'&&owner.moved&&owner.scroll)owner.scroll.scrollTop=owner.scrollTop+owner.y-event.y;
    if(event.type==='up'){
      this.owners.delete(id);
      if(!owner.moved&&owner.button?.isConnected&&!owner.button.disabled){const bounds=owner.button.getBoundingClientRect();if(event.x>=bounds.left&&event.x<=bounds.right&&event.y>=bounds.top&&event.y<=bounds.bottom)owner.button.click();}
    }
    return true;
  }
  cancel(){this.owners.clear();this.keys.clear();}
  destroy(){this.cancel();this.observer?.disconnect();window.removeEventListener('resize',this.resize);this.root.remove();this.style.remove();}
}
