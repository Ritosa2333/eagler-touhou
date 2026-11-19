// Shared PC-98 gestures, following eagler-touhou TH06 Touch.cpp.
// Relative movement and focus have independent finger owners.
export function createDirectDrag(surface,{
  allowed,gameplay,menu=()=>!gameplay(),geometry,sensitivity=()=>150,
  doubleTap=()=>false,gesture=()=>{},change=()=>{},pulse=()=>{},
  now=()=>performance.now(),ios=/iPad|iPhone|iPod/.test(navigator.userAgent)||
    (navigator.platform==='MacIntel'&&navigator.maxTouchPoints>1),
}={}){
  const contacts=new Map(),off=[];
  let primary=null,focus=null,x=0,y=0,sampledX=0,sampledY=0;
  let candidate=null,lastTap=null,menuGesture=null,menuBits=0,begin=false;
  function clearMotion(){x=y=sampledX=sampledY=0;candidate=lastTap=null;}
  function reset(){contacts.clear();primary=focus=null;menuGesture=null;menuBits=0;begin=false;clearMotion();}
  function distance(a,b){return Math.hypot(a.clientX-b.clientX,a.clientY-b.clientY);}
  function down(event){
    if(!allowed()||contacts.has(event.pointerId))return;
    gesture();
    const p={id:event.pointerId,clientX:event.clientX,clientY:event.clientY};
    contacts.set(p.id,p);
    if(!gameplay()){
      candidate=lastTap=null;
      if(menu()){
        if(!menuGesture)menuGesture={...p,startX:p.clientX,startY:p.clientY,max:1};
        else menuGesture.max++;
      }
      change();return;
    }
    const time=now(),g=geometry();
    if(doubleTap()&&lastTap&&time-lastTap.time<=320&&distance(lastTap,p)<=Math.min(g.width,g.height)*.08){
      lastTap=candidate=null;p.bomb=true;pulse(16);change();return;
    }
    lastTap=null;candidate={...p,time,moved:false};
    if(primary===null&&p.id!==focus){primary=p.id;begin=true;clearMotion();candidate={...p,time,moved:false};}
    else if(focus===null&&p.id!==primary)focus=p.id;
    if([...contacts.values()].filter(c=>!c.bomb).length>=4){reset();pulse(128);}
    change();
  }
  function move(event){
    const p=contacts.get(event.pointerId);if(!p)return;
    if(candidate?.id===p.id&&distance(candidate,event)>geometry().height*.05)candidate.moved=true;
    if(menuGesture?.id===p.id){
      menuGesture.clientX=event.clientX;menuGesture.clientY=event.clientY;
      const dx=event.clientX-menuGesture.startX,dy=event.clientY-menuGesture.startY;
      menuBits=Math.max(Math.abs(dx),Math.abs(dy))>geometry().height*.05?
        (Math.abs(dx)>Math.abs(dy)?dx>0?8:4:dy>0?2:1):0;
    }
    if(p.id===primary&&gameplay()){
      const {scale}=geometry();
      if(scale>0){const gain=16*sensitivity()/100/scale;x+=(event.clientX-p.clientX)*gain;y+=(event.clientY-p.clientY)*gain;}
    }
    p.clientX=event.clientX;p.clientY=event.clientY;change();
  }
  function up(event,cancelled=false){
    const p=contacts.get(event.pointerId);if(!p)return;
    // UP must not create a final active movement packet after release.
    if(candidate?.id===p.id){
      lastTap=!cancelled&&doubleTap()&&gameplay()&&!candidate.moved&&
        now()-candidate.time<=220&&distance(candidate,event)<=geometry().height*.05?
        {clientX:event.clientX,clientY:event.clientY,time:now()}:null;
      candidate=null;
    }
    if(menuGesture&&(menuGesture.id===p.id||menuGesture.max>=2)){
      if(!cancelled&&!menuBits)pulse(menuGesture.max>=2?16:256);
      menuGesture=null;menuBits=0;
    }
    contacts.delete(p.id);
    if(p.id===primary){primary=null;begin=false;x=y=sampledX=sampledY=0;}
    if(p.id===focus)focus=null;
    change();
  }
  function listen(type,fn,options){surface.addEventListener(type,fn,options);off.push(()=>surface.removeEventListener(type,fn,options));}
  listen('pointerdown',event=>{
    if(ios&&event.pointerType==='touch'||event.button!==0||!allowed())return;
    event.preventDefault();down(event);
    if(contacts.has(event.pointerId))try{surface.setPointerCapture(event.pointerId);}catch{}
  });
  listen('pointermove',event=>{if(ios&&event.pointerType==='touch'||!contacts.has(event.pointerId))return;event.preventDefault();move(event);});
  for(const type of ['pointerup','pointercancel','lostpointercapture'])listen(type,event=>{
    if(ios&&event.pointerType==='touch')return;
    up(event,type!=='pointerup');
  });
  // WebKit receives TouchEvents only, preventing duplicate PointerEvent input.
  const pointer=touch=>({pointerId:touch.identifier,clientX:touch.clientX,clientY:touch.clientY});
  for(const type of ['touchstart','touchmove','touchend','touchcancel'])listen(type,event=>{
    if(!ios||type==='touchstart'&&!allowed())return;
    if(event.cancelable)event.preventDefault();
    for(const touch of Array.from(event.changedTouches)){
      const p=pointer(touch);
      if(type==='touchstart')down(p);else if(type==='touchmove')move(p);else up(p,type==='touchcancel');
    }
  },{passive:false});
  return {
    reset,clearMotion,
    get dragging(){return primary!==null;},get focused(){return focus!==null;},
    get pending(){return x!==0||y!==0;},get menuBits(){return menuBits;},
    get beginning(){return primary!==null&&begin;},
    clearTap(){candidate=lastTap=null;},
    sample(){sampledX=Math.max(-8192,Math.min(8191,Math.trunc(x)));sampledY=Math.max(-8192,Math.min(8191,Math.trunc(y)));return {x:sampledX,y:sampledY,active:primary!==null};},
    consume(){x-=sampledX;y-=sampledY;sampledX=sampledY=0;begin=false;if(primary===null)x=y=0;},
    dispose(){reset();for(const remove of off)remove();},
  };
}
