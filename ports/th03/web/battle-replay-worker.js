import {workerEnvironment} from './solo-worker-environment.js';
import {NetworkSimulation} from './netplay/simulation.js';
import {DOSBoxPC98} from './dosbox-adapter.js';
import {bootToSelection} from './dosbox-boot.js';
import {bootBattleResources} from './replay/battle-boot.js';
import {createMusicState} from './replay/music-state.js';
const replayMusic=createMusicState('battle-audio');
let emulator,simulation,canvas,sequence=0,closed=false,nextFrame=0,confirmed=0,identity,portable,base;
async function load(header){
  if(emulator)throw Error('对战旁观实例不能重新加载');identity=header.identity;
  canvas=new OffscreenCanvas(640,400);workerEnvironment(canvas,44100);
  const s=header.settings,r=await bootBattleResources(s);
  emulator=await DOSBoxPC98.create({canvas,workerRuntime:true,lockstep:true,independentSfx:true,rollbackEnabled:s.rollback,wasmBinary:r.wasmBinary,lockstepEpoch:946684800000,
    DIPswtch:[0x3e,0xf3,0x7b],ExMemory:7,Latencys:40,SampleHz:44100,SNDboard:4,no_mouse:true,use_menu:false,fontfile:s.language==='cn'?'font_cn.bmp':'font.bmp'});
  simulation=new NetworkSimulation(emulator,{patch:r.patch,music:r.music,focusEnabled:s.focusEnabled,touchUnlimitedAllowed:s.touchUnlimitedAllowed},event=>{if((event[0]>>>8)!==0x0c)replayMusic.observe(event);postMessage({type:'battle-audio',event});});
  emulator.addDiskImage('3-'+s.language+'.hdi',r.data);await emulator.setHdd(0,'3-'+s.language+'.hdi');
  await bootToSelection({emulator,pause:simulation.pause,music:simulation.music,progress:message=>postMessage({type:'notify',message:{message}})});
  return {...r.opening,boot:simulation.hash()};
}
let chain=Promise.resolve();
onmessage=({data})=>{
  if(data.type==='frame-ack'||!data.id)return;
  chain=chain.then(async()=>{
    if(closed)return;let result;
    if(data.type==='load')result=await load(data.header);
    else if(data.type==='execute'){if(data.frame!==nextFrame)throw Error('对战历史缺帧');simulation.step(data.inputs,data.frame,false);nextFrame++;if(data.hash)result=simulation.hash();}
    else if(data.type==='confirm'){if(data.args[0]!==nextFrame)throw Error('对战确认边界无效');simulation.confirm(data.args[0],data.args[0]);confirmed=nextFrame;}
    else if(data.type==='replaySave'){
      if(nextFrame!==confirmed||simulation.snapshots.size||emulator.snapshots||data.args[0]!==identity)throw Error('对战检查点不在独立确认边界');
      const {createPortableCheckpoint,encodeCheckpoint}=await import('./replay/portable.js');portable??=createPortableCheckpoint(emulator.module,emulator.ci);
      const host={identity,frame:nextFrame,tick:emulator.tick,keys:[...emulator.keys],state:emulator.state,pauseSeat:simulation.pauseSeat,selection:simulation.selection,hidden:[...simulation.hidden],
        pause:simulation.pause.capture(),music:simulation.music.capture(),presentationMusic:replayMusic.capture(),controls:simulation.controls.capture(),nativeState:simulation.state,proof:simulation.proof(),check:simulation.hash()};
      const state=portable.capture(host),bytes=await encodeCheckpoint(state,nextFrame?base:null);if(!nextFrame)base=state;result={frame:nextFrame,bytes,proof:host.proof,check:host.check};
    }else if(data.type==='replayLoad'){
      if(nextFrame!==confirmed||simulation.snapshots.size||emulator.snapshots||data.args[2]!==identity)throw Error('对战检查点恢复边界无效');
      const {createPortableCheckpoint,decodeCheckpoint}=await import('./replay/portable.js');portable??=createPortableCheckpoint(emulator.module,emulator.ci);
      const baseState=data.args[1]?await decodeCheckpoint(data.args[1]):null,state=await decodeCheckpoint(data.args[0],baseState),h=state.host;
      if(h?.identity!==identity||!Number.isSafeInteger(h.frame)||h.frame<0||h.frame>216000)throw Error('对战检查点身份无效');
      portable.restore(state);emulator.tick=h.tick;emulator.keys=new Set(h.keys);emulator.state=h.state;emulator.outputs.clear();
      simulation.pauseSeat=h.pauseSeat;simulation.selection=h.selection;simulation.hidden=new Set(h.hidden);simulation.pause.restore(h.pause);simulation.music.restore(h.music);simulation.controls.restore(h.controls);simulation.state=h.nativeState;
      simulation.audio.frames.clear();simulation.audio.confirmed=h.frame;nextFrame=confirmed=h.frame;
      if(simulation.proof()!==h.proof||simulation.hash()!==h.check)throw Error('对战检查点恢复证明不一致');result={frame:nextFrame,proof:h.proof,check:h.check};
      postMessage({type:'replay-audio-reset',events:replayMusic.restore(h.presentationMusic)});
    }
    else if(data.type==='start')emulator.run();
    else if(data.type==='setRawPresentation'){}
    else if(data.type==='present'){if(emulator.module.netPresent()){const bitmap=canvas.transferToImageBitmap();postMessage({type:'frame',sequence:++sequence,bitmap},[bitmap]);}}
    else throw Error('未知对战回放命令');
    const s=simulation.state,scene=simulation.pause.selecting?{kind:'selection'}:s?.phase===1?{kind:'round',round:s.generation}:null;
    postMessage({type:'result',id:data.id,result,status:{proof:simulation.proof(),game:s,scene}});
  }).catch(error=>{closed=true;postMessage({type:'error',id:data.id,message:error.stack||String(error)});});
};
