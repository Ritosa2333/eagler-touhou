// Input ownership is kept separate so releasing a touch cannot release a
// physical keyboard key. The game records the merged scan mask in Replay.
export const virtualKey=code=>({ArrowUp:38,ArrowDown:40,ArrowLeft:37,ArrowRight:39,Enter:13,NumpadEnter:13,Escape:27,Backspace:8,Tab:9,Space:32,ShiftLeft:160,ShiftRight:161,ControlLeft:162,ControlRight:163,PageUp:33,PageDown:34,Home:36,End:35,Delete:46,Insert:45}[code]??(/^Key[A-Z]$/.test(code)?code.charCodeAt(3):/^F(?:[1-9]|1[0-2])$/.test(code)?111+Number(code.slice(1)):0));
export function createTouchInput({send,position,options,move,scene=()=>4,clock=()=>performance.now()}){
 let held=new Set(),pulse=new Map(),points=new Map(),target=null,serials={bombSerial:0,escapeSerial:0},lastTap=-Infinity,snapshot={fireEnabled:false,focusEnabled:false,joystickX:0,joystickY:0};
 function publish(next){for(const key of held)if(!next.has(key))send(key,false);for(const key of next)if(!held.has(key))send(key,true);held=next;}
 function cancel(){points.clear();target=null;pulse.clear();snapshot={fireEnabled:false,focusEnabled:false,joystickX:0,joystickY:0};publish(new Set());}
 return {
  controls(message){for(const key of ['bombSerial','escapeSerial'])if(message[key]!==serials[key]){if(message[key]>serials[key])pulse.set(key==='bombSerial'?'KeyX':'Escape',2);serials[key]=message[key];}snapshot=message;},
  pointer(message){if(!options().touchEnabled)return;
   if(message.type==='down'){const now=clock();if(scene()===4&&points.size===0&&options().doubleTapBombEnabled&&now-lastTap<280)pulse.set('KeyX',2);lastTap=now;points.set(message.id,{x:message.x,y:message.y,startX:message.x,startY:message.y,dragged:false,startTime:now});if(points.size>1)for(const point of points.values())point.paired=true;if(scene()===4)target??=position();}
   else if(message.type==='up'){const old=points.get(message.id);if(old&&!old.dragged){if(!old.paired&&!(scene()===13&&clock()-old.startTime>=280))pulse.set('KeyZ',2);else if(old.paired&&scene()!==4&&points.size===1)pulse.set('Escape',2);}points.delete(message.id);if(!points.size)target=null;}
   else {const old=points.get(message.id);if(!old)return;const dx=message.x-old.x,dy=message.y-old.y;if(Math.hypot(message.x-old.startX,message.y-old.startY)>12)old.dragged=true;if(points.keys().next().value===message.id){if(scene()!==4){if(Math.abs(dx)>24||Math.abs(dy)>24)pulse.set(Math.abs(dx)>Math.abs(dy)?(dx>0?'ArrowRight':'ArrowLeft'):(dy>0?'ArrowDown':'ArrowUp'),2);else return;}else {target??=position();const sensitivity=(snapshot.touchSensitivity??options().touchSensitivity)/100;target.x+=dx*sensitivity;target.y+=dy*sensitivity;target.x=Math.max(40,Math.min(408,target.x));target.y=Math.max(64,Math.min(408,target.y));}}points.set(message.id,{...old,x:message.x,y:message.y});}
  },
  tick(){const next=new Set();if(options().touchEnabled){if(snapshot.fireEnabled)next.add('KeyZ');if(scene()===13&&points.size===1){const point=points.values().next().value;if(!point.paired&&clock()-point.startTime>=280)next.add('ControlLeft');}if(snapshot.focusEnabled||options().touchFocusMode==='two-finger'&&points.size>1)next.add('ShiftLeft');
   const x=snapshot.joystickX,y=snapshot.joystickY;if(scene()===4&&options().touchMovementMode==='joystick-free'){if(Math.hypot(x,y)>4096)move(x/32767*64,y/32767*64,false);}else{if(x< -4096)next.add('ArrowLeft');if(x>4096)next.add('ArrowRight');if(y< -4096)next.add('ArrowUp');if(y>4096)next.add('ArrowDown');}
   if(scene()===4&&target&&points.size&&options().touchMovementMode.startsWith('touch')){const p=position();move(target.x-p.x,target.y-p.y,options().touchMovementMode==='touch-unlimited');}}
   for(const [key,left] of pulse){if(left===2)next.add(key);if(left>1)pulse.set(key,left-1);else pulse.delete(key);}publish(next);
  },cancel,
 };
}
