import {parameterWords,validateParams} from './bridge.mjs';
import {ImGuiRenderer} from './imgui-renderer.mjs';
const localeIndex=value=>value==='en-US'?1:value==='ja-JP'?2:0;
function virtualKey(code){
  if(/^Key[A-Z]$/.test(code))return code.charCodeAt(3);
  if(/^Digit[0-9]$/.test(code))return code.charCodeAt(5);
  if(/^Numpad[0-9]$/.test(code))return 96+Number(code.at(-1));
  if(/^F(?:[1-9]|1[0-2])$/.test(code))return 111+Number(code.slice(1));
  return {Backspace:8,Tab:9,Enter:13,ShiftLeft:160,ShiftRight:161,ControlLeft:162,ControlRight:163,AltLeft:18,AltRight:18,Escape:27,Space:32,PageUp:33,PageDown:34,End:35,Home:36,ArrowLeft:37,ArrowUp:38,ArrowRight:39,ArrowDown:40,Insert:45,Delete:46,Minus:189,Period:190,NumpadSubtract:109,NumpadDecimal:110}[code]??-1;
}
// Thin platform adapter. The same native ImGui widgets/common purple tools used
// by TH10/11 own layout, navigation, checkboxes, speed, HUD and mobile panels.
export class PracticeOverlay {
  constructor({bridge,canvas,container=document.body,onError=error=>{throw error;},onMenuAction=null,onRetry=null,locale='zh-CN'}){
    this.bridge=bridge;this.core=bridge.core;this.canvas=canvas;this.onError=onError;this.onMenuAction=onMenuAction;this.locale=locale;
    this.enabled=false;this.initialized=false;this.keys=new Set();this.ids=new Map();this.serial=0;this.scene='title';this.menu=null;this.replay=false;
    this.onRetry=onRetry;
    this.root=document.createElement('canvas');this.root.width=640;this.root.height=480;this.root.className='th12-practice-native';this.root.setAttribute('aria-label','Purple THPrac');
    Object.assign(this.root.style,{position:'fixed',zIndex:'80',pointerEvents:'none'});this.root.hidden=true;container.append(this.root);
    this.resize=()=>this.layout();window.addEventListener('resize',this.resize);this.observer=typeof ResizeObserver==='function'?new ResizeObserver(this.resize):null;this.observer?.observe(canvas);this.layout();
  }
  async initialize(fonts){
    if(!this.enabled||this.initialized)return;
    const font=fonts.get('/unifont.otf');if(!font)throw Error('THPrac requires the shared /unifont.otf resource before launch');
    const bytes=new Uint8Array(await font.arrayBuffer());this.font=this.core._malloc(bytes.length);if(!this.font)throw Error('THPrac font allocation failed');
    this.core.HEAPU8.set(bytes,this.font);
    if(this.core._th12_practice_ui_init(this.font,bytes.length,localeIndex(this.locale))!==1){this.core._free(this.font);this.font=0;throw Error('Purple THPrac initialization failed');}
    this.initialized=true;this.renderer=new ImGuiRenderer(this.core,this.root);this.root.hidden=false;
  }
  configure({enabled=this.enabled,coreEnabled=enabled,locale=this.locale}={}){
    this.enabled=!!enabled;this.locale=locale;this.bridge.setEnabled(!!coreEnabled);this.root.hidden=!this.enabled;
    if(!this.enabled)this.cancel();
  }
  layout(){const rect=this.canvas.getBoundingClientRect();Object.assign(this.root.style,{left:rect.left+'px',top:rect.top+'px',width:rect.width+'px',height:rect.height+'px'});}
  mobilePanels(snapshot={}){
    if(!this.initialized||typeof snapshot.thpracMobilePanelsEnabled!=='boolean')return;
    this.core._th12_practice_ui_mobile(Number(snapshot.thpracMobilePanelsEnabled),Number(snapshot.thpracMobilePanelsVisible===true));
  }
  update({scene=this.scene,replay=this.replay,menu=null,menuReady=false,onMenuAction=this.onMenuAction}={}){
    this.scene=scene;this.replay=!!replay;this.onMenuAction=onMenuAction;
    if(!this.initialized)return;
    this.core._th12_practice_ui_context(scene==='practice'?1:scene==='playing'||scene==='demo'?2:['pause','pause-confirm'].includes(scene)?3:0,Number(menuReady),Number(this.replay),menu?.difficulty??1,localeIndex(this.locale));
    if(menu&&menu!==this.menu){const words=parameterWords(menu.params),pointer=this.core._malloc(words.byteLength);try{this.core.HEAPU8.set(new Uint8Array(words.buffer),pointer);this.core._th12_practice_ui_params(pointer,words.length);}finally{this.core._free(pointer);}}
    this.menu=menu;
  }
  get capturesGameplay(){return this.enabled&&this.initialized&&this.core._th12_practice_ui_capture()!==0;}
  key(code,down=true){
    if(!this.enabled||!this.initialized)return false;
    down?this.keys.add(code):this.keys.delete(code);const vk=virtualKey(code);if(vk>=0)this.core._th12_practice_ui_key(vk,Number(down));
    const flags=this.core._th12_practice_ui_flags();
    return this.capturesGameplay||['Backspace','Tab','F12'].includes(code)||(flags&1)&&['playing','demo','pause','pause-confirm'].includes(this.scene)&&(/^(F[1-7])$/.test(code)||code==='KeyU');
  }
  tick(pad=0){
    if(!this.enabled||!this.initialized)return;
    this.core._th12_practice_ui_tick(pad);
    const action=this.core._th12_practice_ui_action();if(action===3){this.onRetry?.();return true;}if(!action||!this.menu)return;
    if(action===2){this.onMenuAction?.({type:'cancel'});return;}
    const w=new Int32Array(this.core.HEAPU8.buffer,this.core._th12_practice_ui_parameters(),20);
    const score=Number(BigInt(w[6]>>>0)+(BigInt(w[7])<<32n));
    const params=validateParams({mode:w[1],stage:w[2],section:w[3],phase:w[4],dlg:!!w[5],score,life:w[8],life_fragment:w[9],bomb:w[10],bomb_fragment:w[11],power:w[12],value:w[13],graze:w[14],ufo_side:w[15],ventra_1:w[16],ventra_2:w[17],ventra_3:w[18]},w[2]===6?4:this.menu.difficulty);
    this.menu.params=params;this.onMenuAction?.({type:'start',params,difficulty:params.stage===6?4:this.menu.difficulty});
  }
  render(){if(!this.initialized||!this.enabled)return;this.layout();this.renderer.render();}
  pointer(event){
    if(!this.enabled||!this.initialized)return false;
    const id=event.id??'mouse',type={down:0,move:1,up:2,cancel:3}[event.type];if(type===undefined)return false;
    if(!this.ids.has(id))this.ids.set(id,++this.serial);
    const rect=this.canvas.getBoundingClientRect(),x=(event.x-rect.left)*640/rect.width,y=(event.y-rect.top)*480/rect.height;
    const result=this.core._th12_practice_ui_pointer(type,this.ids.get(id),Number.isFinite(x)?x:0,Number.isFinite(y)?y:0,Number(event.pointerType==='mouse'||id==='mouse'))!==0;
    if(type>=2)this.ids.delete(id);return result;
  }
  clearKeyboard(){this.keys.clear();if(this.initialized)this.core._th12_practice_ui_clear_keys();}
  wheel(event){if(!this.enabled||!this.initialized)return false;const rect=this.canvas.getBoundingClientRect(),scale=event.deltaMode===1?1:1/100;return this.core._th12_practice_ui_wheel((event.clientX-rect.left)*640/rect.width,(event.clientY-rect.top)*480/rect.height,-event.deltaX*scale,-event.deltaY*scale)!==0;}
  cancel(){this.keys.clear();this.ids.clear();if(this.initialized)this.core._th12_practice_ui_cancel();}
  destroy(){this.cancel();this.observer?.disconnect();window.removeEventListener('resize',this.resize);this.renderer?.destroy();if(this.initialized)this.core._th12_practice_ui_shutdown();if(this.font)this.core._free(this.font);this.root.remove();this.initialized=false;}
}
