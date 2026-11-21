// Adapted from fixed upstream TouchLayoutEditor.tsx: visibleViewport,
// clampPanelPosition, rememberPanel/positionPanel, and panel pointer ownership.
export function bindFloatingTouchWindow({node,host,handle,collapse,body,window,document,store,orientation,listen,kind}){
 let direction=orientation(),gesture=null,lastBounds=null,disposed=false,collapsed=false;
 const subscriptions=[],on=(target,type,fn,options)=>subscriptions.push(listen(target,type,fn,options));
 function bounds(){
  const r=host.getBoundingClientRect(),v=window.visualViewport;
  const left=(v?.offsetLeft??0)-r.left,top=(v?.offsetTop??0)-r.top;
  const width=v?.width||document.documentElement?.clientWidth||window.innerWidth||r.width;
  const height=v?.height||document.documentElement?.clientHeight||window.innerHeight||r.height;
  return{left,top,right:left+width,bottom:top+height};
 }
 function measure(b){
  node.style.maxWidth=Math.max(1,b.right-b.left-32)+'px';
  node.style.maxHeight=Math.max(1,b.bottom-b.top-32)+'px';
  return node.getBoundingClientRect();
 }
 function put(left,top,b= bounds()){
  const p=measure(b),minLeft=b.left+16,minTop=b.top+16;
  node.style.left=Math.max(minLeft,Math.min(Math.max(minLeft,b.right-p.width-16),left))+'px';
  node.style.top=Math.max(minTop,Math.min(Math.max(minTop,b.bottom-p.height-16),top))+'px';
  node.style.transform='none';lastBounds=b;
 }
 function remember(){
  if(node.hidden||!lastBounds)return;
  const b=lastBounds,p=node.getBoundingClientRect(),h=host.getBoundingClientRect();
  const rangeX=Math.max(0,b.right-b.left-p.width-32),rangeY=Math.max(0,b.bottom-b.top-p.height-32);
  store.set(direction,kind,{x:rangeX?(p.left-h.left-b.left-16)/rangeX:.5,y:rangeY?(p.top-h.top-b.top-16)/rangeY:.5});
 }
 function position(){
  if(node.hidden||disposed)return;
  direction=orientation();store.reload();const b=bounds(),p=measure(b),saved=store.get(direction,kind);
  const width=b.right-b.left,height=b.bottom-b.top;
  const left=saved?b.left+16+saved.x*Math.max(0,width-p.width-32):b.left+(width>=height?width-p.width-16:(width-p.width)/2);
  const top=saved?b.top+16+saved.y*Math.max(0,height-p.height-32):b.top+Math.min(76,Math.max(16,height-p.height-16));
  put(left,top,b);
 }
 function clamp(){
  if(node.hidden||disposed)return;
  const p=node.getBoundingClientRect(),h=host.getBoundingClientRect();put(p.left-h.left,p.top-h.top);
 }
 function end(event){
  if(!gesture||(event&&event.pointerId!==gesture.id))return;
  const id=gesture.id;gesture=null;remember();
  try{if(handle.hasPointerCapture?.(id))handle.releasePointerCapture(id);}catch{}
 }
 on(handle,'pointerdown',event=>{
  if(node.hidden||disposed||gesture||(event.pointerType==='mouse'&&event.button!==0))return;
  event.preventDefault();event.stopPropagation?.();const p=node.getBoundingClientRect(),h=host.getBoundingClientRect();
  gesture={id:event.pointerId,x:event.clientX,y:event.clientY,left:p.left-h.left,top:p.top-h.top};
  try{handle.setPointerCapture(event.pointerId);}catch{}
 });
 on(document,'pointermove',event=>{
  if(!gesture||event.pointerId!==gesture.id)return;
  event.preventDefault();put(gesture.left+event.clientX-gesture.x,gesture.top+event.clientY-gesture.y);
  gesture.left=parseFloat(node.style.left);gesture.top=parseFloat(node.style.top);gesture.x=event.clientX;gesture.y=event.clientY;
 });
 for(const type of ['pointerup','pointercancel'])on(document,type,end);
 on(handle,'lostpointercapture',end);
 on(window,'blur',()=>end());on(document,'visibilitychange',()=>{if(document.hidden)end();});
 const reflow=()=>{end();if(orientation()!==direction)position();else clamp();};
 on(window,'resize',reflow);if(window.visualViewport)on(window.visualViewport,'resize',reflow);
 if(window.screen?.orientation?.addEventListener)on(window.screen.orientation,'change',reflow);
 on(collapse,'click',()=>{
  end();collapsed=!collapsed;body.hidden=collapsed;node.classList.toggle('is-collapsed',collapsed);
  collapse.setAttribute('aria-expanded',String(!collapsed));collapse.textContent=collapsed?'展开':'折叠';clamp();
 });
 collapse.setAttribute('aria-expanded','true');
 return{
  show(){node.hidden=false;position();},
  hide(){end();remember();node.hidden=true;},
  position,clamp,cancel:()=>end(),
  dispose(){if(disposed)return;end();remember();disposed=true;for(const off of subscriptions)off?.();}
 };
}
