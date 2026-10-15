import {PlaybackClock} from './playback-clock.mjs';

// Owns only a selected local recording. The WASM instance, DAT, textures,
// filesystem and message/document epoch stay alive across play/seek/switch.
export function createReplayPlayer({core,onState=()=>{},onError=()=>{},onFrame=()=>{}}) {
  const clock=new PlaybackClock();let bytes=null,path='',raf=0,closed=false,lastReport=-Infinity;
  const native=()=>{const p=core._th09_replay_view_info()/4;return Array.from(core.HEAPU32.subarray(p,p+8));};
  const state=()=>{const [,frame,total,ended,difficulty,left,right,stage]=native();return {kind:'replay',path,frame,total,start:0,paused:clock.paused,seeking:clock.target!==null,target:clock.target,rate:clock.rate,ended:!!ended,difficulty,left,right,stage,canRewind:true};};
  function report(){if(bytes)onState(state());}
  function restart(){
    const p=core._th09_file_buffer(bytes.length);if(!p)throw Error('录像为空或过大');
    core.HEAPU8.set(bytes,p);if(!core._th09_play_replay(bytes.length))throw Error('录像无法播放');
    core._th09_loop_stop();core._th09_loop_pause(0);clock.reset();
    if(!core._th09_replay_view_step(1))throw Error('录像首帧无法播放');
  }
  function draw(time){
    raf=0;
    if(closed||!bytes)return;
    try {
      if(document.hidden){clock.last=null;raf=requestAnimationFrame(draw);return;}
      const seeking=clock.target!==null;
      core._th09_replay_view_silent(+seeking);
      clock.tick(time,{position:()=>native()[1],ended:()=>!!native()[3],step:(index,last)=>core._th09_replay_view_step(+(index===0||last||(clock.target!==null&&native()[1]+1>=clock.target)))!==0});
      core._th09_replay_view_silent(+(clock.target!==null));
      core._th09_audio_pump();
      core._th09_suspend(+(clock.paused||clock.target!==null||!!native()[3]));
      onFrame();if(time-lastReport>=100){lastReport=time;report();}
      raf=requestAnimationFrame(draw);
    }catch(error){clock.pause(true);report();onError(error);}
  }
  return {get active(){return !!bytes&&!closed;},state,
    play(nextPath,nextBytes){if(closed)throw Error('播放器已关闭');const prior=bytes,priorPath=path;bytes=nextBytes.slice();path=nextPath;
      try{restart();clock.pause(false);}catch(error){bytes=prior;path=priorPath;throw error;}
      if(!raf)raf=requestAnimationFrame(draw);report();return state();},
    control(action,value){if(!bytes||closed)throw Error('请先选择录像');
      if(action==='pause')clock.pause(true);
      else if(action==='play'){if(native()[3])restart();clock.pause(false);}
      else if(action==='rate')clock.speed(value);
      else if(action==='seek'){const current=native();if(!Number.isSafeInteger(value)||value<0||value>current[2])throw Error('录像位置无效');
        const paused=clock.paused;if(value<current[1]||current[3])restart();clock.pause(paused);clock.seek(value);}
      else throw Error('不支持的播放操作');
      report();return state();},
    close(){closed=true;cancelAnimationFrame(raf);raf=0;bytes=null;core._th09_replay_view_silent(0);},
  };
}
