import {PlaybackClock} from './playback-clock.mjs';

// Playback re-simulates the saved inputs; the core is recreated for backward seeks.
export function createNativeReplayPlayer({replay,path,boot,step,onState,onFrame,onError}){
 const clock=new PlaybackClock({tickRate:526315/9328});let cursor=0,raf=0,closed=false,starting=false,lastReport=0;
 const state=()=>({kind:'replay',path,frame:cursor,total:replay.rows.length,paused:clock.paused,seeking:starting||clock.target!==null,target:clock.target,rate:clock.rate,ended:cursor>=replay.rows.length,difficulty:-1,left:-1,right:-1,stage:0,seekable:true,rateControl:true});
 const report=()=>onState(state());
 async function restart(){starting=true;report();try{await boot();if(closed)throw Error('录像已关闭');cursor=0;clock.reset();}finally{starting=false;if(!closed)report();}}
 function draw(time){raf=0;if(closed)return;try{if(document.hidden||starting){clock.last=null;raf=requestAnimationFrame(draw);return;}const silent=clock.target!==null||clock.rate!==1;
  const n=clock.tick(time,{position:()=>cursor,ended:()=>cursor>=replay.rows.length,step:()=>{step(replay.rows[cursor],cursor,!silent);cursor++;return true;}});
  if(n)onFrame();if(time-lastReport>=150||n&&cursor===replay.rows.length){lastReport=time;report();}raf=requestAnimationFrame(draw);
 }catch(error){clock.pause(true);report();onError(error);}}
 return {state,async start(){await restart();clock.pause(false);report();raf=requestAnimationFrame(draw);return state();},
  async control(action,value){if(closed||starting)throw Error('录像尚未就绪');
   if(action==='pause')clock.pause(true);
   else if(action==='play'){if(cursor>=replay.rows.length)await restart();clock.pause(false);}
   else if(action==='rate')clock.speed(value);
   else if(action==='seek'){if(!Number.isSafeInteger(value)||value<0||value>replay.rows.length)throw Error('录像位置无效');const paused=clock.paused;if(value<cursor)await restart();clock.pause(paused);clock.seek(value);}
   else throw Error('录像命令无效');report();return state();},
  stop(){closed=true;cancelAnimationFrame(raf);raf=0;}};
}
