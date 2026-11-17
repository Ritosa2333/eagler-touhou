// TH128 tools adapted from fixed upstream thprac; MIT licenses/THPRAC-MIT.txt.
import {readThprac128Native,thprac128Label} from './thprac128.mjs';
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
 const check=selector=>{const n=root.querySelector(selector);if(!n)throw new Error('Missing practice tool control '+selector);return n;},message=check('[data-tools-status]'),advanced=check('[data-tools-advanced]'),overlay=root.querySelector('[data-tools-overlay]')??root;
 let closed=false,advancedOpen=false,draft=null,draftOwner=null,appliedKey='';
 const inputs=[...root.querySelectorAll('[data-tool-bit]')],locale=()=>['zh','en','ja'].includes(getLocale())?getLocale():'zh';
 const label=key=>thprac128Label(key,locale()),applied=owner=>owner?.speedFor(owner.snapshot().frame)??THPRAC128_SPEED;
 function syncDraft(owner,speed){const key=JSON.stringify(speed);if(owner!==draftOwner||key!==appliedKey){draftOwner=owner;appliedKey=key;draft={...speed};}}
 function speedText(key,value){if(key==='slow')return String(value);if(key==='fast')return value===21?'infinite':'x'+value+'.0 ('+value*60+'fps)';return 'x'+(Number.isInteger(value/60)?(value/60)+'.0':String(value/60))+' ('+value+'fps)';}
 function renderDraft(){
  const owner=getOwner(),speed=applied(owner);syncDraft(owner,speed);const disabled=!owner||owner.playback;
  for(const key of ['fps','slow','fast']){const n=check('[data-tool-speed="'+key+'"]');if(key==='fps'){n.type=draft.free?'number':'range';n.min='60';n.max=draft.free?'6000':'180';n.step=draft.free?'1':'15';}n.value=String(draft[key]);n.disabled=disabled;const text=root.querySelector('[data-tool-speed-text="'+key+'"]');if(text){text.textContent=speedText(key,draft[key]);text.hidden=key==='fps'&&draft.free;}}
  const free=check('[data-tool-speed="free"]');free.checked=draft.free;free.disabled=disabled;
  const button=check('[data-tools-apply]'),pending=['fps','slow','fast'].some(key=>draft[key]!==speed[key]);button.hidden=disabled||!pending;button.disabled=disabled||!pending;button.textContent=label('TH_ADV_OPT_APPLY');
 }
 const refresh=()=>{
  const owner=getOwner(),s=owner?.snapshot(),lang=['zh','en','ja'].indexOf(locale());root.setAttribute?.('lang',['zh-CN','en','ja'][lang]);advanced.setAttribute?.('aria-label',label('TH_ADV_OPT'));
  for(const n of inputs){const bit=Number(n.dataset.toolBit);n.checked=!!(s?.mask&(1<<bit));n.disabled=!owner||owner.playback;n.parentElement.querySelector('span').textContent='F'+(bit+1)+': '+labels[toolKeys[bit]][lang];}
  for(const n of root.querySelectorAll('[data-tools-label]'))n.textContent=label(n.getAttribute('data-tools-label')).replace('%s','You!');
  const version=root.querySelector('[data-tools-version]');if(version){let i=0;version.textContent=label('TH_ABOUT_VERSION').replace(/%d/g,()=>String([2,3,1,1][i++]));}
  const licence=root.querySelector('[data-tools-licence]'),licenceLabel=root.querySelector('[data-tools-licence-label]');if(licenceLabel)licenceLabel.textContent=label(licence?.open?'TH_ABOUT_HIDE_LICENCE':'TH_ABOUT_SHOW_LICENCE');
  check('[data-tools-close]').setAttribute?.('aria-label',label('TH_CLOSE'));message.hidden=true;message.textContent='';renderDraft();
 };
 const close=()=>{if(root.hidden)return;root.hidden=true;const wasAdvanced=advancedOpen;advancedOpen=false;onClose(wasAdvanced);};
 const open=async(value=false)=>{if(closed)return;if(!root.hidden&&advancedOpen===value){close();return;}advancedOpen=value;root.hidden=false;advanced.hidden=!value;if(overlay!==root)overlay.hidden=value;refresh();await onOpen(value);if(value&&!closed&&!root.hidden)check('[data-tools-close]').focus();};
 const action=fn=>{try{fn();refresh();onChange();}catch(e){message.hidden=false;message.textContent=e.message;}};
 const editableOwner=()=>{const owner=getOwner();if(!owner||owner.playback)throw new Error('Practice playLock or retired owner');syncDraft(owner,applied(owner));return owner;};
 const integer=key=>{const text=check('[data-tool-speed="'+key+'"]').value;if(!/^(0|[1-9][0-9]*)$/.test(text)||!Number.isSafeInteger(Number(text)))throw new Error('Speed requires an integer');return Number(text);};
 for(const n of inputs)n.addEventListener('change',()=>action(()=>{const owner=editableOwner(),mask=owner.snapshot().mask,bit=1<<Number(n.dataset.toolBit);owner.setMask(n.checked?mask|bit:mask&~bit);}));
 const free=check('[data-tool-speed="free"]');free.addEventListener('change',()=>{try{editableOwner();draft.free=free.checked;if(!draft.free)draft.fps=Math.max(60,Math.min(180,60+Math.floor((draft.fps-60)/15)*15));renderDraft();}catch(e){message.hidden=false;message.textContent=e.message;}});
 for(const key of ['fps','slow','fast'])check('[data-tool-speed="'+key+'"]').addEventListener('input',()=>{try{editableOwner();const next={...draft,[key]:integer(key)};validateThprac128Speed(next);draft=next;message.hidden=true;message.textContent='';renderDraft();}catch(e){message.hidden=false;message.textContent=e.message;}});
 check('[data-tools-apply]').addEventListener('click',()=>action(()=>{const owner=editableOwner(),value={free:draft.free};for(const key of ['fps','slow','fast'])value[key]=integer(key);owner.setSpeed(validateThprac128Speed(value));}));
 root.querySelector('[data-tools-licence]')?.addEventListener('toggle',()=>{const n=root.querySelector('[data-tools-licence-label]');if(n)n.textContent=label(root.querySelector('[data-tools-licence]').open?'TH_ABOUT_HIDE_LICENCE':'TH_ABOUT_SHOW_LICENCE');});
 check('[data-tools-close]').addEventListener('click',close);root.addEventListener('keydown',e=>{if(!advancedOpen)return;e.stopPropagation();if(['Escape','Backspace','F12'].includes(e.code)){e.preventDefault();close();}else if(e.code==='Tab'){const nodes=[...root.querySelectorAll('button,input')].filter(n=>!n.disabled&&!n.closest('[hidden]'));const a=root.ownerDocument.activeElement;if(nodes.length&&(e.shiftKey?a===nodes[0]:a===nodes.at(-1))){e.preventDefault();(e.shiftKey?nodes.at(-1):nodes[0]).focus();}}});root.addEventListener('keyup',e=>{if(advancedOpen)e.stopPropagation();});root.hidden=true;
 return Object.freeze({open,close,refresh,get isOpen(){return !root.hidden&&!closed&&advancedOpen;},get overlayOpen(){return !root.hidden&&!closed&&!advancedOpen;},dispose(){closed=true;root.hidden=true;}});
}
