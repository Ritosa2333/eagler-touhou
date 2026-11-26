// The Launcher owns gesture surfaces and button layout. This controller
// translates the shared controls into TH02 keys and Q4 movement.
export function createHostedTouch({options,scene,panelPointer,now=()=>performance.now()}){
  let controls={},pulse=0,menuPulse=0,tap=-Infinity,point=null;
  let motion={x:0,y:0,active:false};
  const fingers=new Set(),panels=new Set();
  const game=()=>scene()===1;
  const free=()=>options().touchMovementMode==='joystick-free';
  const direct=()=>options().touchMovementMode?.startsWith('touch');
  const limited=()=>options().touchMovementMode==='touch';
  const resetMotion=()=>{motion={x:0,y:0,active:false};};
  return {
    controls(next){
      if(options().touchEnabled){
        if(next.bombSerial>(controls.bombSerial??0))pulse|=16;
        if(next.escapeSerial>(controls.escapeSerial??0))pulse|=128;
        if(scene()===0&&!direct()&&controls.fireEnabled!==undefined&&next.fireEnabled!==controls.fireEnabled)menuPulse|=32;
      }
      controls=next;
    },
    pointer(message){
      if(!options().touchEnabled)return;
      const {type,id,x,y}=message;
      if(type==='down'&&scene(x,y)===2)panels.add(id);
      if(panels.has(id)){
        panelPointer(message);
        if(type==='up')panels.delete(id);
        return;
      }
      if(type==='down'){
        fingers.add(id);
        if(point){point.paired=true;return;}
        point={id,x,y,startX:x,startY:y,game:game(),dragged:false,paired:false};
        if(point.game&&now()-tap<280&&options().doubleTapBombEnabled){
          pulse|=16;tap=-Infinity;point.bomb=true;
        }
        if(point.game&&direct())motion={...motion,active:true,begin:true,release:0};
      }else if(type==='move'&&point?.id===id){
        const fromStart=Math.hypot(x-point.startX,y-point.startY);
        if(fromStart>12)point.dragged=true;
        if(point.game&&game()&&direct()){
          const scale=16*(controls.touchSensitivity??options().touchSensitivity??150)/100;
          motion.x+=(x-point.x)*scale;motion.y+=(y-point.y)*scale;
          point.x=x;point.y=y;
        }else if(point.game&&limited()){
          point.x=x;point.y=y;resetMotion();
        }else if(!point.game&&!game()){
          const dx=x-point.x,dy=y-point.y;
          if(Math.hypot(dx,dy)>24){
            menuPulse|=Math.abs(dx)>Math.abs(dy)?(dx<0?4:8):(dy<0?1:2);
            point.x=x;point.y=y;
          }
        }
      }else if(type==='up'&&fingers.delete(id)){
        if(point?.id===id){
          if(!point.dragged&&!point.paired&&!point.bomb&&game()===point.game){
            if(point.game)tap=now();
            menuPulse|=32;
          }
          if(point.game&&motion.active){
            if(limited())resetMotion();
            else motion.release=4;
          }
          if(!point.paired)point=null;
          else point.id=null;
        }
        if(!fingers.size){
          if(point?.paired&&!point.dragged&&!point.game&&!game())menuPulse|=128;
          point=null;
        }
      }
    },
    sample(){
      let bits=pulse|menuPulse;pulse=menuPulse=0;
      if(!options().touchEnabled)return 0;
      if(game()&&controls.fireEnabled)bits|=32;
      if(game()&&(controls.focusEnabled||options().touchFocusMode==='two-finger'&&fingers.size>1))bits|=64;
      if(!game()||!free()){
        if(controls.joystickX< -4096)bits|=4;if(controls.joystickX>4096)bits|=8;
        if(controls.joystickY< -4096)bits|=1;if(controls.joystickY>4096)bits|=2;
      }
      return bits;
    },
    motion(){
      if(!options().touchEnabled||!game()){resetMotion();return;}
      if(free()){
        const {joystickX:x=0,joystickY:y=0}=controls,length=Math.hypot(x,y);
        if(length>4096)return {active:true,begin:true,x:x*80/Math.max(32767,length),y:y*80/Math.max(32767,length)};
        return;
      }
      if(!direct())return;
      if(point?.game&&point.id!==null&&!motion.active)motion={x:0,y:0,active:true,begin:true};
      if(limited()){
        const value={...motion,x:Math.round(motion.x),y:Math.round(motion.y),radial:true};
        motion.x-=value.x;motion.y-=value.y;motion.begin=false;
        return value;
      }
      const value={...motion,unlimited:options().touchMovementMode==='touch-unlimited'};
      if(motion.release===1)resetMotion();
      else {if(motion.release)motion.release--;motion.x=motion.y=0;motion.begin=false;}
      return value;
    },
    cancel(){
      if(panels.size)panelPointer({type:'cancel'});
      panels.clear();fingers.clear();point=null;tap=-Infinity;pulse=menuPulse=0;
      controls={bombSerial:controls.bombSerial,escapeSerial:controls.escapeSerial};resetMotion();
    },
  };
}
