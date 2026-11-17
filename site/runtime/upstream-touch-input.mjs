// Gesture ownership and wall-clock recognition only. The player consumes one
// motion command inside its normal fixed-tick movement pass.
const clamp=(n,a,b)=>Math.max(a,Math.min(b,n));
const distance=(a,b)=>Math.hypot(a.x-b.x,a.y-b.y);
export class TouchInput {
  constructor(){
    this.options={touchEnabled:false,touchMovementMode:'touch-unlimited',touchSensitivity:150,touchFocusMode:'two-finger',doubleTapBombEnabled:false};
    this.points=new Map();this.primary=null;this.menu=null;this.context=null;this.pending=0;this.tap=null;
    this.fire=false;this.focus=false;this.axes=[0,0];this.bombSerial=this.escapeSerial=0;
  }
  configure(options){
    const next={...this.options,...options};
    if(next.touchEnabled!==this.options.touchEnabled||next.touchMovementMode!==this.options.touchMovementMode)this.cancel();
    this.options=next;
  }
  cancel({preserveFocus=false}={}){this.points.clear();this.primary=null;this.menu=null;this.pending=0;this.tap=null;if(!preserveFocus||this.options.touchFocusMode!=='toggle-button')this.focus=false;this.axes=[0,0];}
  sync(state){
    if(this.context===state.key)return;
    const toggledFocus=this.focus&&this.options.touchFocusMode==='toggle-button'&&
      (['game','dialogue'].includes(this.lastKind)||this.paused)&&
      (['game','dialogue'].includes(state.kind)||state.paused)&&
      (!(this.paused||state.paused)||this.instance===state.instance);
    const continuous=this.context&&this.lastKind!=='menu'&&state.kind!=='menu'&&this.lastKind!=='replay'&&state.kind!=='replay'&&
      (this.instance===state.instance||state.gestureScope!=null&&this.gestureScope===state.gestureScope);
    if(!continuous)this.cancel();else {this.pending=0;this.menu=null;this.tap=null;}
    // Keep physical contacts through the local pilot's respawn. Rebase the
    // target and pointer origin, so the next move cannot reuse a pre-death aim.
    if(continuous&&this.instance!==state.instance&&this.primary){
      const drag=this.primary,point=this.points.get(drag.id);
      drag.targetX=state.x;drag.targetY=state.y;drag.instance=state.instance;drag.blocked=!state.ready;
      if(point){drag.previousX=point.x;drag.previousY=point.y;}
    }
    if(toggledFocus)this.focus=true;
    this.context=state.key;this.lastKind=state.kind;this.instance=state.instance;this.gestureScope=state.gestureScope;this.paused=!!state.paused;
  }
  controls(snapshot,state){
    this.sync(state);this.fire=snapshot.fireEnabled;this.focus=snapshot.focusEnabled;
    this.axes=[snapshot.joystickX,snapshot.joystickY].map(n=>clamp(n/32767,-1,1));
    if(snapshot.touchSensitivity!==undefined)this.options.touchSensitivity=snapshot.touchSensitivity;
    if(this.options.touchEnabled){
      if(snapshot.bombSerial>this.bombSerial&&state.kind==='game')this.pending|=2;
      if(snapshot.escapeSerial>this.escapeSerial)this.pending|=256;
    }
    this.bombSerial=snapshot.bombSerial;this.escapeSerial=snapshot.escapeSerial;
  }
  focused(){return this.focus||(this.options.touchFocusMode==='two-finger'&&this.points.size>=2);}
  pointer(event,state,now=performance.now()){
    if(!this.options.touchEnabled)return;this.sync(state);
    const {id,type,x,y}=event;if(type==='cancel'){this.release(id,true);return;}
    if(type==='down'){
      if(this.points.has(id)){this.release(id,true);this.pending=0;}
      const point={id,x,y,startX:x,startY:y,start:now,moved:false,blocked:!state.ready,instance:state.instance};this.points.set(id,point);
      if(this.points.size>=4){this.cancel({preserveFocus:true});this.pending|=256;return;}
      if(state.kind==='menu'){
        if(!this.menu)this.menu={ids:new Set(),peak:0,start:now,x,y,lastX:x,lastY:y,consumed:false};
        this.menu.ids.add(id);this.menu.peak=Math.max(this.menu.peak,this.menu.ids.size);return;
      }
      if(state.kind==='replay')return;
      if(state.kind==='game'&&this.options.doubleTapBombEnabled&&this.tap&&now-this.tap.time<=320&&distance(point,this.tap)<=38.4){this.pending|=2;this.tap=null;point.bombTap=true;return;}
      this.tap=null;
      if(!this.primary&&!this.options.touchMovementMode.startsWith('joystick'))this.primary={id,previousX:x,previousY:y,targetX:state.x,targetY:state.y,instance:state.instance,blocked:!state.ready};
      return;
    }
    const point=this.points.get(id);if(!point)return;
    point.x=x;point.y=y;point.moved||=Math.hypot(x-point.startX,y-point.startY)>24;
    if(this.menu){
      const menu=this.menu;
      if(!menu.consumed&&menu.ids.has(id)){
        menu.lastX=x;menu.lastY=y;const dx=x-menu.x,dy=y-menu.y;
        if(type==='move'&&(Math.abs(dx)>24||Math.abs(dy)>24)){this.pending|=Math.abs(dx)>Math.abs(dy)?dx>0?128:64:dy>0?32:16;menu.consumed=true;}
      }
      if(type==='up'){
        menu.ids.delete(id);this.release(id);
        if(!menu.ids.size){if(!menu.consumed&&now-menu.start<500)this.pending|=menu.peak>=2?2:1;this.menu=null;}
      }
      return;
    }
    if(type==='move'&&this.primary?.id===id){
      const drag=this.primary;
      if(!state.ready){drag.blocked=true;drag.previousX=x;drag.previousY=y;return;}
      if(drag.blocked||drag.instance!==state.instance){drag.targetX=state.x;drag.targetY=state.y;drag.previousX=x;drag.previousY=y;drag.blocked=false;drag.instance=state.instance;return;}
      const ratio=this.options.touchMovementMode==='touch'&&(this.focused()||state.keyFocused)&&state.fast>0?state.slow/state.fast:1;
      const gain=this.options.touchSensitivity/100*ratio;
      drag.targetX=clamp(drag.targetX+clamp(x-drag.previousX,-640,640)*128*gain,-0x5c00,0x5c00);
      drag.targetY=clamp(drag.targetY+clamp(y-drag.previousY,-480,480)*128*gain,0x1000,0xd800);
      drag.previousX=x;drag.previousY=y;
    }
    if(type==='up'){
      if(state.kind==='dialogue'&&!point.moved&&now-point.start<500)this.pending|=1;
      else if(state.kind==='game'&&!point.bombTap&&!point.moved&&now-point.start<=220)this.tap={x,y,time:now};
      this.release(id);
    }
  }
  release(id,cancelled=false){
    if(!this.points.has(id))return;
    this.points.delete(id);if(this.primary?.id===id)this.primary=null;
    // A cancelled menu contact invalidates this gesture's tap, but the other
    // contacts retain their ownership until their own release. In particular,
    // their later up events must not turn cancellation into Confirm/Cancel.
    if(this.menu?.ids.has(id)){
      if(cancelled)this.menu.consumed=true;
      this.menu.ids.delete(id);if(!this.menu.ids.size)this.menu=null;
    }
    if(cancelled)this.tap=null;
  }
  sample(state,{arrows=false,keyFocused=false,now=performance.now()}={}){
    this.sync(state);const pulses=this.pending;this.pending=0;
    if(!this.options.touchEnabled)return {held:0,pressed:0,motion:null};
    let held=pulses,motion=null;const mode=this.options.touchMovementMode;
    if(state.kind==='menu'){
      const [x,y]=this.axes;held|=(x<-.25?64:x>.25?128:0)|(y<-.25?16:y>.25?32:0);
    }else if(state.kind!=='replay'){
      if(state.kind==='game'&&this.fire)held|=1;
      if(this.focused())held|=8;
      if(state.kind==='dialogue'&&[...this.points.values()].some(p=>!p.moved&&now-p.start>=500))held|=512;
      if(arrows)this.primary=null;
      if(state.ready){
        if(this.primary){
          const drag=this.primary;
          if(drag.blocked||drag.instance!==state.instance){drag.targetX=state.x;drag.targetY=state.y;drag.blocked=false;drag.instance=state.instance;}
          motion={mode:mode==='touch-unlimited'?2:1,x:Math.trunc(drag.targetX),y:Math.trunc(drag.targetY)};
        }else if(!arrows&&mode==='joystick-free'&&(this.axes[0]||this.axes[1]))motion={mode:3,x:Math.trunc(this.axes[0]*32767),y:Math.trunc(this.axes[1]*32767)};
        else if(mode==='joystick'){const [x,y]=this.axes;held|=(x<-.25?64:x>.25?128:0)|(y<-.25?16:y>.25?32:0);}
      }else if(this.primary)this.primary.blocked=true;
    }
    return {held,pressed:pulses,motion};
  }
}
