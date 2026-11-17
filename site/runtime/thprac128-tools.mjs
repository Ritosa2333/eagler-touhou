// TH128 tools adapted from fixed upstream thprac; MIT licenses/THPRAC-MIT.txt.
import {readThprac128Native} from './thprac128.mjs';
import {FrameStepper} from './timing.mjs';
export const THPRAC128_SPEED=Object.freeze({fps:60,slow:15,fast:1,free:false});
function record(v){return v&&typeof v==='object'&&!Array.isArray(v)&&(Object.getPrototypeOf(v)===Object.prototype||Object.getPrototypeOf(v)===null)&&Object.values(Object.getOwnPropertyDescriptors(v)).every(d=>Object.hasOwn(d,'value'));}
export function validateThprac128Speed(input={}){
 if(!record(input)||Object.keys(input).some(k=>!['fps','slow','fast','free'].includes(k)))throw new Error('Invalid practice speed fields');
 const value={...THPRAC128_SPEED,...input};
 if(typeof value.free!=='boolean'||!['fps','slow','fast'].every(k=>Number.isSafeInteger(value[k]))||value.fps<60||value.fps>6000||value.slow<1||value.slow>60||value.fast<1||value.fast>21||(!value.free&&(value.fps>180||(value.fps-60)%15)))throw new Error('Practice speed outside upstream bounds');
 return Object.freeze(value);
}
export function validateThprac128ToolHistory(input={events:[],speed:[]},frames=36000001){
 if(!record(input)||Object.keys(input).some(k=>!['events','speed'].includes(k))||!Number.isSafeInteger(frames)||frames<1||frames>36000001)throw new Error('Invalid practice tool history');
 const result={};for(const key of ['events','speed']){const rows=input[key];if(!Array.isArray(rows)||rows.length>4096)throw new Error('Practice tool history budget');let previous=-1;
 result[key]=rows.map(row=>{if(!Array.isArray(row)||row.length!==2||!Number.isSafeInteger(row[0])||row[0]<=previous||row[0]>=frames)throw new Error('Practice schedule frame order or recorded history differs');previous=row[0];
 const value=key==='speed'?validateThprac128Speed(row[1]):row[1];if(key==='events'&&(!Number.isSafeInteger(value)||value<0||value>127))throw new Error('Invalid practice mask');return Object.freeze([row[0],value]);});Object.freeze(result[key]);}
 return Object.freeze(result);
}
export class Thprac128LogicClock{
 constructor(fixed=new FrameStepper()){if(!(fixed instanceof FrameStepper))throw new Error('Fixed60 clock required');this.fixed=fixed;this.reset();}
 reset(){this.previous=null;this.virtual=0;this.debt=0;this.fixed.reset();}
 advance(now,active,fps=60){
  if(!Number.isFinite(now)||!Number.isFinite(fps)||fps<1||fps>15360)throw new Error('Invalid practice scheduler clock or rate');
  if(this.previous!==null&&this.fixed.previous!==this.virtual)this.reset();
  if(this.previous===null||!active){this.previous=now;this.debt=0;this.fixed.advance(this.virtual,false);return{steps:0,stalled:false};}
  const elapsed=now-this.previous;this.previous=now;if(elapsed<0||elapsed>250){this.debt=0;this.fixed.advance(this.virtual,false);return{steps:0,stalled:true};}
  this.debt+=elapsed*fps/60;let steps=0;
  while(this.debt>0&&steps<256){const delta=Math.min(this.debt,(Math.min(4,256-steps))*this.fixed.step);this.debt-=delta;this.virtual+=delta;steps+=this.fixed.advance(this.virtual,true).steps;}
  return{steps,stalled:false};
 }
}
export class Thprac128Settings{
 constructor(storage=globalThis.localStorage){this.storage=storage;this.key='th128.owned-thprac-options.v1';}
 checked(v){if(!record(v)||Object.keys(v).length!==2||typeof v.enabled!=='boolean'||!['zh','en','ja'].includes(v.locale))throw new Error('Invalid owned thprac settings');return Object.freeze({enabled:v.enabled,locale:v.locale});}
 read(){const raw=this.storage.getItem(this.key);return raw===null?this.checked({enabled:true,locale:'zh'}):this.checked(JSON.parse(raw));}
 write(value){const v=this.checked(value),raw=JSON.stringify(v);this.storage.setItem(this.key,raw);if(this.storage.getItem(this.key)!==raw)throw new Error('Owned thprac settings durable full readback differs');return this.read();}
 launch(config={}){const value={...this.read()};if(Object.hasOwn(config.options??{},'thpracEnabled')){if(typeof config.options.thpracEnabled!=='boolean')throw new Error('Launcher thpracEnabled must be boolean');value.enabled=config.options.thpracEnabled;}return Object.freeze(value);}
}
export function installThprac128ToolPlayback(module,events,generation){
 if(typeof module._th128_world_thprac_tools_playback!=='function'){if(events.length)throw new Error('Practice tool playback native ABI missing');return;}
 const bytes=new Uint8Array(events.length*8),view=new DataView(bytes.buffer);for(const[row,[frame,mask]]of events.entries()){view.setUint32(row*8,frame,true);view.setUint32(row*8+4,mask,true);}
 const p=bytes.length?module._malloc(bytes.length):0;try{if(bytes.length)module.HEAPU8.set(bytes,p);if(module._th128_world_thprac_tools_playback(p,bytes.length,generation)!==1)throw new Error(module.UTF8ToString(module._th128_world_error())||'Native practice tool playback rejected');}finally{if(p)module._free(p);}
}
export function createThprac128Tools(module,session,{isCurrent=()=>true,playback=false,history,initial}={}){
 const receipt=readThprac128Native(module,session);if(['_th128_world_thprac_tools_set','_th128_world_thprac_tools_state'].some(k=>typeof module[k]!=='function'))throw new Error('Actual TH128 practice tools native ABI missing');
 let closed=false,speed=validateThprac128Speed(),speeds=[],saved=history?validateThprac128ToolHistory(history):null;
 const current=()=>{if(closed||!isCurrent())throw new DOMException('Practice tools owner closed','AbortError');};
 const snapshot=()=>{current();const s=JSON.parse(module.UTF8ToString(module._th128_world_thprac_tools_state()));if(s.schema!=='th128-thprac-tools/1'||s.generation!==receipt.generation||!Number.isSafeInteger(s.frame)||s.frame<0||!Number.isInteger(s.mask)||s.mask<0||s.mask>127||s.playLock!==playback)throw new Error('Actual practice tool generation/frame/playLock differs');validateThprac128ToolHistory({events:s.events,speed:[]});return Object.freeze(s);};
 const setMask=mask=>{const s=snapshot();if(playback||!Number.isSafeInteger(mask)||mask<0||mask>127)throw new Error('Practice playLock or invalid mask');if(module._th128_world_thprac_tools_set(mask,receipt.generation)!==1)throw new Error(module.UTF8ToString(module._th128_world_error()));return snapshot();};
 const setSpeed=value=>{const s=snapshot();if(playback)throw new Error('Practice replay playLock');const v=validateThprac128Speed(value);if(speeds.length>=4096&&speeds.at(-1)?.[0]!==s.frame)throw new Error('Practice speed history budget');if(speeds.at(-1)?.[0]===s.frame)speeds.pop();speeds.push([s.frame,v]);speed=v;return speed;};
 const speedFor=frame=>{current();if(saved){let v=THPRAC128_SPEED;for(const row of saved.speed){if(row[0]>frame)break;v=row[1];}return v;}return speed;};
 snapshot();if(initial&&!playback){setMask(initial.mask);setSpeed(initial.speed);}
 return Object.freeze({snapshot,setMask,setSpeed,speedFor,generation:receipt.generation,playback,
  history(frames){const s=snapshot();return validateThprac128ToolHistory({events:s.events.filter(row=>row[0]<frames),speed:speeds.filter(row=>row[0]<frames)},frames);},
  close(){closed=true;}
 });
}

const labels={"TH_MUTEKI":["无敌","Invincible","無敵"],"TH_INFLIVES":["锁残","Inf. Lives","残機減らない"],"TH_INFBOMBS":["锁Bomb","Inf. Bombs","ボム減らない"],"TH_INFPOWER":["锁火力","Inf. Power","霊力減らない"],"TH_TIMELOCK":["锁时","Time Lock","残り時間減らない"],"TH_AUTOBOMB":["自动B","Auto Bomb","自動喰らいボム"],"TH_EL_BGM":["永续BGM","Everlasting BGM","永遠に続くBGM"]};
const toolKeys=["TH_MUTEKI","TH_INFLIVES","TH_INFBOMBS","TH_INFPOWER","TH_TIMELOCK","TH_AUTOBOMB","TH_EL_BGM"];
export function mountThprac128ToolsPanel({root,getOwner,getLocale=()=>"zh",onOpen=()=>{},onClose=()=>{},onChange=()=>{}}){
 const check=selector=>{const n=root.querySelector(selector);if(!n)throw new Error('Missing practice tool control '+selector);return n;},message=check('[data-tools-status]'),advanced=check('[data-tools-advanced]'),overlay=root.querySelector('[data-tools-overlay]')??root;let closed=false,advancedOpen=false;
 const inputs=[...root.querySelectorAll('[data-tool-bit]')],locale=()=>['zh','en','ja'].indexOf(getLocale());
 function speedText(key,value){return key==='slow'?String(value):key==='fast'?(value===21?'infinite':'x'+value+'.0 ('+value*60+'fps)'):'x'+(value/60).toFixed(value%60?2:1).replace(/0$/,'0')+' ('+value+'fps)';}
 const refresh=()=>{const owner=getOwner(),s=owner?.snapshot(),lang=Math.max(0,locale());root.setAttribute?.('lang',['zh-CN','en','ja'][lang]);
 for(const n of inputs){const bit=Number(n.dataset.toolBit);n.checked=!!(s?.mask&(1<<bit));n.disabled=!owner||owner.playback;n.parentElement.querySelector('span').textContent='F'+(bit+1)+': '+labels[toolKeys[bit]][lang];}
 const speed=owner?.speedFor(s.frame)??THPRAC128_SPEED;for(const k of ['fps','slow','fast']){const n=check('[data-tool-speed="'+k+'"]');if(k==='fps'){n.max=speed.free?'6000':'180';n.step=speed.free?'1':'15';}n.value=String(speed[k]);n.disabled=!owner||owner.playback;const text=root.querySelector('[data-tool-speed-text="'+k+'"]');if(text)text.textContent=speedText(k,speed[k]);}
 const free=check('[data-tool-speed="free"]');free.checked=speed.free;free.disabled=!owner||owner.playback;check('[data-tools-apply]').disabled=!owner||owner.playback;message.textContent='';};
 const close=()=>{if(root.hidden)return;root.hidden=true;const wasAdvanced=advancedOpen;advancedOpen=false;onClose(wasAdvanced);};
 const open=async(value=false)=>{if(closed)return;if(!root.hidden&&advancedOpen===value){close();return;}advancedOpen=value;root.hidden=false;advanced.hidden=!value;if(overlay!==root)overlay.hidden=value;refresh();await onOpen(value);if(value&&!closed&&!root.hidden)check('[data-tools-close]').focus();};
 const apply=fn=>{try{fn();refresh();onChange();}catch(e){message.hidden=false;message.textContent=e.message;}};
 for(const n of inputs)n.addEventListener('change',()=>apply(()=>{const owner=getOwner();if(!owner)throw new Error('Practice owner retired');const mask=owner.snapshot().mask,bit=1<<Number(n.dataset.toolBit);owner.setMask(n.checked?mask|bit:mask&~bit);}));
 const free=check('[data-tool-speed="free"]');free.addEventListener('change',()=>{const n=check('[data-tool-speed="fps"]');n.max=free.checked?'6000':'180';n.step=free.checked?'1':'15';if(!free.checked)n.value=String(Math.min(180,60+Math.floor((Number(n.value)-60)/15)*15));});
 for(const key of ['fps','slow','fast'])check('[data-tool-speed="'+key+'"]').addEventListener('input',()=>{const text=root.querySelector('[data-tool-speed-text="'+key+'"]');if(text)text.textContent=speedText(key,Number(check('[data-tool-speed="'+key+'"]').value));});
 check('[data-tools-apply]').addEventListener('click',()=>apply(()=>{const owner=getOwner(),value={free:free.checked};for(const key of ['fps','slow','fast']){const text=check('[data-tool-speed="'+key+'"]').value;if(!/^(0|[1-9][0-9]*)$/.test(text))throw new Error('Speed requires an integer');value[key]=Number(text);}owner.setSpeed(value);}));
 check('[data-tools-close]').addEventListener('click',close);root.addEventListener('keydown',e=>{if(!advancedOpen)return;e.stopPropagation();if(['Escape','Backspace','F12'].includes(e.code)){e.preventDefault();close();}else if(e.code==='Tab'){const nodes=[...root.querySelectorAll('button,input')].filter(n=>!n.disabled&&!n.closest('[hidden]'));const a=root.ownerDocument.activeElement;if(nodes.length&&(e.shiftKey?a===nodes[0]:a===nodes.at(-1))){e.preventDefault();(e.shiftKey?nodes.at(-1):nodes[0]).focus();}}});root.addEventListener('keyup',e=>{if(advancedOpen)e.stopPropagation();});root.hidden=true;
 return Object.freeze({open,close,refresh,get isOpen(){return !root.hidden&&!closed&&advancedOpen;},get overlayOpen(){return !root.hidden&&!closed&&!advancedOpen;},dispose(){closed=true;root.hidden=true;}});
}
