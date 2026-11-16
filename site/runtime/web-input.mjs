// Touch inputs enter the existing keyboard route; no native input or frame calls live here.
const INPUT_KEYS=new Set(['ArrowUp','ArrowDown','ArrowLeft','ArrowRight','KeyZ','KeyX','KeyC','Enter','Escape','ShiftLeft','ShiftRight']);
export function mountWebInput({canvas,container=canvas?.parentElement,document=canvas?.ownerDocument??globalThis.document,
 window=document?.defaultView??globalThis.window,getContext=()=>null,isEnabled=()=>true}={}){
 if(!canvas||!container||!document||!window)throw new Error('Web input needs a canvas container');
 const layer=document.createElement('div');layer.className='web-input';layer.setAttribute('aria-label','触控游戏操作');
 const pad=document.createElement('div');pad.className='web-input-pad';pad.setAttribute('data-web-drag','1');pad.setAttribute('aria-label','拖动方向');
 const legend=document.createElement('span');legend.textContent='移动';pad.appendChild(legend);
 const actions=document.createElement('div');actions.className='web-input-actions';
 const specs=[['focus','低速','ShiftLeft'],['shot','射击 / 确认','KeyZ'],['freeze','完美冻结','KeyX'],['pause','暂停','Escape'],['back','返回','KeyX']];
 const targets=[pad];
 for(const [action,label,code]of specs){const node=document.createElement('button');node.type='button';node.textContent=label;node.className='web-input-button';
  node.setAttribute('data-web-action',action);node.setAttribute('aria-label',label);node.setAttribute('data-web-code',code);actions.appendChild(node);targets.push(node);}
 layer.appendChild(pad);layer.appendChild(actions);container.appendChild(layer);
 const listeners=[],pointers=new Map(),sources=new Map(),counts=new Map(),physical=new Set(),activeCodes=new Map(),synthetic=new WeakSet();let context=getContext(),disposed=false,muted=false;
 const listen=(node,type,fn)=>{node.addEventListener(type,fn,{passive:false});listeners.push([node,type,fn]);};
 const send=(type,code)=>{const event=new window.KeyboardEvent(type,{code,bubbles:true,cancelable:true});synthetic.add(event);window.dispatchEvent(event);};
 const group=code=>code==='ShiftRight'?'ShiftLeft':code;
 const heldPhysical=key=>[...physical].find(code=>group(code)===key);
 function add(code){const key=group(code),n=counts.get(key)??0;counts.set(key,n+1);if(!n){
  const held=heldPhysical(key),actual=activeCodes.get(key)??held??code;activeCodes.set(key,actual);if(!held)send('keydown',actual);
 }}
 function remove(code){const key=group(code),n=counts.get(key)??0;if(n<=1){counts.delete(key);
  if(n&&!heldPhysical(key)){const actual=activeCodes.get(key)??code;activeCodes.delete(key);if(!muted)send('keyup',actual);}
 }else counts.set(key,n-1);}
 function keys(pointer,next){for(const code of pointer.keys)if(!next.has(code))remove(code);for(const code of next)if(!pointer.keys.has(code))add(code);pointer.keys=next;}
 function end(id){const p=pointers.get(id);if(!p)return;pointers.delete(id);keys(p,new Set());try{if(p.node.hasPointerCapture?.(id))p.node.releasePointerCapture(id);}catch{}}
 function release(notify=true){const previous=muted;muted=!notify;try{for(const id of [...pointers.keys()])end(id);for(const source of sources.values())keys(source,new Set());sources.clear();}finally{muted=previous;}}
 function syncContext(){if(!pointers.size&&!sources.size)return;const next=getContext();if(next!==context||!isEnabled()){context=next;release();}}
 function content(){const r=canvas.getBoundingClientRect(),scale=Math.min(r.width/1280,r.height/960);return scale>0?
  {left:r.left+(r.width-1280*scale)/2,top:r.top+(r.height-960*scale)/2,scale}:null;}
 function down(event){
  if(disposed||event.pointerType!=='touch'&&event.pointerType!=='pen'||pointers.has(event.pointerId))return;
  syncContext();if(!isEnabled()||document.hidden)return;context=getContext();
  const node=event.currentTarget,drag=node===pad,r=content();if(drag&&(!r||event.clientX<r.left||event.clientX>r.left+1280*r.scale||event.clientY<r.top||event.clientY>r.top+960*r.scale))return;
  const p={node,drag,x:event.clientX,y:event.clientY,keys:new Set(),context};pointers.set(event.pointerId,p);
  try{node.setPointerCapture?.(event.pointerId);}catch{pointers.delete(event.pointerId);return;}
  if(!drag)keys(p,new Set([node.getAttribute('data-web-code')]));
  event.preventDefault();
 }
 function move(event){
  syncContext();const p=pointers.get(event.pointerId);if(!p||!p.drag||disposed)return;
  const r=content();if(!r){end(event.pointerId);return;}
  const dx=(event.clientX-p.x)/r.scale,dy=(event.clientY-p.y)/r.scale,next=new Set();
  if(dx<=-11)next.add('ArrowLeft');else if(dx>=11)next.add('ArrowRight');
  if(dy<=-11)next.add('ArrowUp');else if(dy>=11)next.add('ArrowDown');
  keys(p,next);event.preventDefault();
 }
 function finish(event){if(!pointers.has(event.pointerId))return;end(event.pointerId);event.preventDefault?.();}
 for(const node of targets){listen(node,'pointerdown',down);listen(node,'pointermove',move);listen(node,'pointerup',finish);listen(node,'pointercancel',finish);listen(node,'lostpointercapture',finish);}
 const forget=()=>{release(false);physical.clear();activeCodes.clear();};
 listen(window,'blur',forget);listen(window,'pagehide',forget);listen(document,'visibilitychange',()=>{if(document.hidden)forget();});
 return {layer,syncContext,releaseAll:release,
  setKeySource(owner,codes){
   if(typeof owner!=='symbol')throw new TypeError('Input source owner must be a symbol');
   if(!Array.isArray(codes)||codes.length>16||codes.some(code=>!INPUT_KEYS.has(code)))throw new TypeError('Invalid input source keys');
   if(disposed)return false;syncContext();if(!isEnabled()||document.hidden)return false;context=getContext();
   const source=sources.get(owner)??{keys:new Set()};sources.set(owner,source);keys(source,new Set(codes));if(!source.keys.size)sources.delete(owner);return true;
  },
  clearKeySource(owner){const source=sources.get(owner);if(!source)return;keys(source,new Set());sources.delete(owner);},
  readKeySource(owner){return [...(sources.get(owner)?.keys??[])];},
  acceptKeyDown(event){if(synthetic.has(event))return true;physical.add(event.code);const key=group(event.code);return !counts.has(key)&&(!activeCodes.has(key)||activeCodes.get(key)===event.code);},
  acceptKeyUp(event){if(synthetic.has(event))return true;physical.delete(event.code);const key=group(event.code);if(counts.has(key))return false;
   if(activeCodes.has(key)){if(heldPhysical(key))return false;const actual=activeCodes.get(key);activeCodes.delete(key);if(actual!==event.code){send('keyup',actual);return false;}}
   return true;
  },
  dispose(){if(disposed)return;release();disposed=true;physical.clear();activeCodes.clear();for(const [node,type,fn]of listeners)node.removeEventListener(type,fn);layer.remove();}
 };
}
