import {workerEnvironment} from '../solo-worker-environment.js';
import {NetworkSimulation} from './simulation.js';
import {bootToSelection} from '../dosbox-boot.js';
let emulator,simulation,canvas,failed=false,sequence=0,diagnosticsAt=-Infinity,steps=0,publishedStep=0,presentedKey=null;
const transfers=new Set();let events=[];
const send=(message,transfer=[])=>postMessage(message,transfer);
function status(method){
  const now=performance.now(),diagnostics=!['execute','replay-slice'].includes(method)||steps-publishedStep>=30||now-diagnosticsAt>=1000;
  // Keep controls current; copy snapshot/native diagnostics at TH05's cadence.
  if(diagnostics){diagnosticsAt=now;publishedStep=steps;}
  return simulation.describe({diagnostics,compact:!diagnostics});
}
function present(staged=false){
  const key=`${emulator.module.frameUploads||0}:${simulation.renderEpoch||0}`;
  if(transfers.size>=2||(staged&&presentedKey===key)||!emulator.module.netPresent())return false;
  const bitmap=canvas.transferToImageBitmap(),id=++sequence;transfers.add(id);presentedKey=key;
  send({type:'frame',sequence:id,bitmap,staged,producedAt:performance.timeOrigin+performance.now()},[bitmap]);return true;
}
async function dispatch(method,args){
  if(method==='init'){
    const setup=args[0];canvas=new OffscreenCanvas(640,400);workerEnvironment(canvas,setup.sampleRate);
    const {DOSBoxPC98}=await import('../dosbox-adapter.js');
    emulator=await DOSBoxPC98.create({...setup.config,canvas,workerRuntime:true,lockstep:true,independentSfx:true,
      onError:error=>{failed=true;send({type:'fatal',message:error.message});},onExit:()=>send({type:'exit'})});
    simulation=new NetworkSimulation(emulator,setup.bridge,event=>events.push(event));
    return {runtime:emulator.module.mobileRuntime};
  }
  if(!simulation)throw Error('联机模拟线程尚未初始化');
  if(method==='addDiskImage'){emulator.addDiskImage(...args);return;}
  if(method==='setHdd')return emulator.setHdd(...args);
  if(method==='getDiskImage')return emulator.getDiskImage(...args);
  if(method==='boot'){
    const bootMusic=await bootToSelection({emulator,pause:simulation.pause,music:simulation.music,
      progress:message=>send({type:'progress',message})});
    return {events:bootMusic,hash:simulation.hash()};
  }
  if(method==='execute'){
    const [inputs,options]=args;let captureMs=0,hashMs=0,hash;
    if(options.capture){const at=performance.now();simulation.capture(options.frame);captureMs=performance.now()-at;}
    const at=performance.now();simulation.step(inputs,options.frame,options.replay);const simulateMs=performance.now()-at;steps++;
    if(options.hash){const at=performance.now();hash=simulation.hash();hashMs=performance.now()-at;}
    const proof=simulation.proof();
    if(options.prepareFrame&&!options.replay)present(true);
    return {hash,captureMs,hashMs,simulateMs,proof};
  }
  if(method==='replay-slice'){
    const [frame,plans,prepareFrame=false]=args;
    if(!Number.isSafeInteger(frame)||frame<0||!Array.isArray(plans)||!plans.length||plans.length>4||plans.some((plan,index)=>!plan||plan.frame!==frame+index||!Array.isArray(plan.inputs)))throw Error('TH03 回滚补算批次无效');
    const started=performance.now(),records=[];let exited=false;
    for(const plan of plans){
      if(records.length&&performance.now()-started>=6)break;
      // Reuse the ordinary atomic capture/step/hash path and retain the proof
      // at each boundary, even when a slice crosses a checksum interval.
      records.push({frame:plan.frame,...await dispatch('execute',[plan.inputs,{...plan,replay:!plan.end}])});
      if(emulator.state==='exited'){exited=true;break;}
    }
    if(prepareFrame&&plans[records.length-1]?.end)present(true);
    return {applied:records.length,records,exited,sliceMs:performance.now()-started};
  }
  if(method==='restore')return simulation.restore(...args);
  if(method==='confirm'){simulation.confirm(...args);return;}
  if(method==='hash')return simulation.hash();
  if(method==='run'){emulator.run();return;}
  if(method==='pause'){emulator.pause();return;}
  if(method==='present')return present();
  throw Error('未知联机线程指令：'+method);
}
let chain=Promise.resolve();
onmessage=({data})=>{
  if(data?.type==='frame-ack'){
    if(transfers.delete(data.sequence)&&data.discarded&&data.sequence===sequence)presentedKey=null;
    return;
  }
  // DOSBox posts private scheduling messages on this scope as well.
  if(!data?.id||typeof data.method!=='string')return;
  chain=chain.then(async()=>{
    if(failed){send({type:'error',id:data.id,message:'联机模拟线程已失败'});return;}
    try{
      const at=performance.now(),result=await dispatch(data.method,data.args||[]);
      const audio=events;events=[];
      // A queued confirmation has no RPC result or diagnostic snapshot. Audio
      // still leaves the worker only after the actual confirmation executes.
      if(data.notify&&data.method==='confirm'){
        if(audio.length)send({type:'confirmed-audio',events:audio});
      }else send({type:'reply',id:data.id,result,status:simulation?status(data.method):null,events:audio,executionMs:performance.now()-at},result instanceof Uint8Array?[result.buffer]:[]);
    }catch(error){failed=true;send({type:'error',id:data.id,message:error.stack||String(error)});}
  });
};
addEventListener('error',event=>{failed=true;send({type:'fatal',message:event.message});});
addEventListener('unhandledrejection',event=>{failed=true;send({type:'fatal',message:String(event.reason?.stack||event.reason)});});
