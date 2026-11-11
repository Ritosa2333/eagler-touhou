import {NativeLockstep} from './native-lockstep.mjs';
import {FireControl} from './fire-control.js';
import {unpackTouch} from './touch-input.js';
import {createVersusPause} from './native-versus-pause.mjs';

// Both peers drive the complete native lifecycle using the same accepted ticks.
export function createNativeMatch({seat, channel, engine, sample, options = {}, onFrame = () => {}, onConfirmedInput = () => {}, onError = () => {}, now = () => performance.now()}) {
  const fire = [new FireControl(),new FireControl()];
  const pause=createVersusPause();
  let running = true, frames = 0, lastProgress = now();
  const metadata=()=>({scene:engine._th03_classic_scene?.()??0,round:engine._th03_semantic_round?.()??-1,serial:engine._th03_classic_match_serial?.()??0,winner:engine._th03_classic_match_winner?.()??-1});
  const digest = () => {
    return engine._th03_classic_scene?.()===9?(engine._th03_semantic_state_hash?.()??0)>>>0:0;
  };
  const sync = new NativeLockstep({seat, send:message=>channel.send(message), digest:engine._th03_classic_pixels?digest:undefined, onError, step(inputs,frame) {
    const recordedInputs=[...inputs];
    const menu=pause.advance(inputs,engine._th03_classic_scene?.()??9,engine._th03_classic_surrender_allowed?.()??true);
    if(menu.paused){fire.forEach(f=>f.reset());frames++;lastProgress=now();onConfirmedInput(frame,recordedInputs,metadata());onFrame(frame,new Int32Array(0));return;}
    if(menu.resumed){fire.forEach(f=>f.reset());if(menu.result>1)engine._th03_classic_surrender?.(menu.seat,menu.result===3);inputs=[0,0];}
    inputs.forEach((value,seat)=>{const t=unpackTouch(value);engine._th03_classic_touch?.(seat,t.active,t.begin||false,t.unlimited,t.x,t.y);});
    const selecting=engine._th03_classic_scene?.()===3;
    const lanes = inputs.map((bits,index) => {
      const charge = !!(bits & 32);
      const shot = selecting?fire[index].selection(!!(bits & 512),charge):fire[index].sample(!!(bits & 512),charge,frame * 1000 / 60,!(bits & 1024));
      return (bits & 31) | (shot ? 32 : 0);
    });
    engine._th03_classic_assist_pair(options.focus !== false && !!(inputs[0]&64), options.focus !== false && !!(inputs[1]&64));
    const system = lanes[0] | ((engine._th03_classic_scene?.()!==9 && (inputs[0]&128)) ? 0x1000 : 0) | ((inputs[0]&256) ? 0x2000 : 0);
    const charge = ((inputs[0]&32)?1:0) | ((inputs[1]&32)?2:0);
    const result = engine._th03_classic_step_pair(lanes[0],lanes[1],system,charge);
    if (result !== 0) { running = false; onError(Error('游戏结束或同步失败：'+result)); }
    frames++; lastProgress=now(); onConfirmedInput(frame,recordedInputs,metadata());onFrame(frame,undefined,engine.HEAPU8?.slice(engine._th03_classic_audio_commands?.(),(engine._th03_classic_audio_commands?.()??0)+(engine._th03_classic_audio_command_count?.()??0)*4));
  }});
  channel.onmessage = event=>sync.receive(event.data);
  channel.onclose = () => { if (running) {running=false;onError(Error('另一位玩家已断开'));} };
  return {tick() { if(!running)return;if(now()-lastProgress>10000){running=false;sync.close();onError(Error('另一位玩家连接超时'));return;}if(sync.local === null) sync.submit(sample() + (options.rapid === false ? 1024 : 0)); }, stop() {running=false;sync.close();},
    get pause(){return pause;},get frame(){return frames;}, get running(){return running;}};
}
