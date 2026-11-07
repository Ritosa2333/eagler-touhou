import {workerEnvironment} from './solo-worker-environment.js';
import {readSoloDiagnostics,writeSoloTestCommand} from './solo-test-tools.js';
import {readSoloFault} from './solo-fault.js';

let emulator,environment,bridge,music,game,patch,coop,mailbox=-1,cursor=0;
const seen=new Map(),signature=new TextEncoder().encode('TH04SOLOINPUTv1!');
let awaitingSnapshot=false,pendingMusic=[],lastPublish=0,costs=[],lastInput=performance.now(),held=new Map();
let input={focus:0,points:0,touch:[0,0]},timer;
let drawTimes=[],lastRenderPublish=-Infinity;
function postReplay(type,value){
  if(type==='chunk'&&value.clocks)postMessage({type:'external-replay-chunk',chunk:value},[value.clocks.buffer]);
  else postMessage({type:'external-replay-'+type,...(type==='chunk'?{chunk:value}:type==='end'?{result:value}:{message:value?.message||String(value)})});
}
function applyAssist(value){
  if(coop){coop.input(value);return;}
  if(bridge){
    const h=emulator.module.HEAPU8;
    if(!bridge.valid(h,bridge.live)||value.generation!==new DataView(h.buffer).getUint16(bridge.live+patch.fields.generation,true))return;
    bridge.setFocus(value.focus,0);bridge.setTouch(value.touch||[0,0]);return;
  }
  const at=Number.isInteger(value.address)?value.address:mailbox;
  const h=emulator.module.HEAPU8,v=new DataView(h.buffer);
  if(at<0||at+36>h.length||!signature.every((b,i)=>h[at+i]===b))return;
  if(!ownsSoloMailbox(h,v,at))return;
  if(value.generation!==v.getUint16(at+24,true))return;
  if(Number.isInteger(value.pauseInput)){
    const entries=patch.practice?.entries,base=at-patch.mailbox_cs_offset;
    if(entries&&Number.isInteger(entries.pauseOpen)){
      if(value.openPause&&!h[base+entries.resultVisible])h[base+entries.pauseOpen]=1;
      if(h[base+entries.pauseOpen])v.setUint16(base+entries.pauseInput,value.pauseInput&15,true);
    }
    return;
  }
  if(Number.isInteger(value.resultInput)){
    const entries=patch.practice?.entries,base=at-patch.mailbox_cs_offset;
    if(entries&&h[base+entries.resultVisible])v.setUint16(base+entries.resultInput,value.resultInput&15,true);
    return;
  }
  if(value.quit||value.restart){
    const data=at-patch.mailbox_cs_offset+(patch.data_segment-patch.code_segment)*16,ds=v.getUint16(at+20,true),base=data-ds*16;
    if(base<0||base+0x100000>h.length||data<base||data+0xba8a>base+0xa0000)return;
    if(value.restart){
      const resident=base+v.getUint16(data+0xba86,true)+v.getUint16(data+0xba88,true)*16;
      if(resident<base||resident+80>base+0xa0000||new TextDecoder().decode(h.subarray(resident,resident+11))!=='HUMAConfig\0')return;
      const config=emulator.config.practice,stage=config?.mode==='target'?config.setup.stage:h[resident+15]===4?6:0;
      const code=at-patch.mailbox_cs_offset;
      if(h[code+0x86]!==0x75||h[code+0x87]!==5||h[code+0x8d]!==0x1e||h[code+0x8e]!==0x68||v.getUint16(code+0x8f,true)!==0xcd)return;
      h[resident+17]=stage;h[resident+19]=48+stage;emulator.restartPractice();
      h.set([0x6d,0x61,0x69,0x6e,0],code+0x88);h[code+0x8d]=0x0e;v.setUint16(code+0x8f,0x88,true);h[data+0x5392]=1;
    }else{emulator.practice?.exitPractice();h[data+0x5392]=1;}
    return;
  }
  h[at+26]=value.points||0;
  const t=value.touch;
  if(emulator.state==='running'&&h[at+22]===1&&!h[at+23]&&t?.active){
    if(t.begin)v.setInt32(at+30,0,true);
    v.setUint16(at+28,t.unlimited?3:1,true);
    for(const [offset,delta]of [[30,t.x],[32,t.y]])v.setInt16(at+offset,Math.max(-8192,Math.min(8191,v.getInt16(at+offset,true)+delta)),true);
  }else h.fill(0,at+28,at+36);
}
function ownsSoloMailbox(h,v,at){
  const data=at-patch.mailbox_cs_offset+(patch.data_segment-patch.code_segment)*16;
  if(at<0||at+36>h.length||data<0||data+0xba8a>h.length||![1,2].includes(h[at+22])||!signature.every((b,i)=>h[at+i]===b))return false;
  const base=data-v.getUint16(at+20,true)*16;
  if(base<0||base+0x100000>h.length)return false;
  const resident=base+v.getUint16(data+0xba86,true)+v.getUint16(data+0xba88,true)*16;
  return resident>=base&&resident+80<=base+0xa0000&&[72,85,77,65,67,111,110,102,105,103,0].every((b,i)=>h[resident+i]===b)&&![0xfd,0xfe,0xff].includes(h[resident+48]);
}
function queueAssist(value){if(emulator?.assist)emulator.assist(value);else applyAssist(value);}
function observeFrames(){
  emulator.module.observeLocalFrame=cost=>{if(costs.length<120)costs.push(cost);};
  emulator.module.observeFrameDraw=now=>{if(drawTimes.length<120)drawTimes.push(now);};
}
function scan04(now){
  const h=emulator.module.HEAPU8,v=new DataView(h.buffer);mailbox=-1;
  for(const [at,candidate] of seen){
    if(!signature.every((b,i)=>h[at+i]===b)){seen.delete(at);continue;}
    const ticks=v.getUint32(at+16,true);
    if(ticks!==candidate.ticks){candidate.ticks=ticks;candidate.changed=now;}
    if((now-candidate.changed<200||h[at+22]===2)&&ownsSoloMailbox(h,v,at))mailbox=at;
  }
  if(mailbox<0){
    if(cursor>=h.length)cursor=0;
    const end=Math.min(cursor+1024*1024,h.length),slice=h.subarray(cursor,Math.min(end+35,h.length));
    for(let i=slice.indexOf(signature[0]);i>=0&&cursor+i<end;i=slice.indexOf(signature[0],i+1)){
      const at=cursor+i;
      if(at+(patch.mailbox_size||36)<=h.length&&signature.every((b,j)=>h[at+j]===b)&&v.getUint32(at+16,true)&&v.getUint16(at+20,true)&&!seen.has(at))seen.set(at,{ticks:0,changed:-Infinity});
    }
    cursor=end;
  }
  emulator.module.sidebarPaletteAddress=mailbox<0?-1:mailbox-patch.mailbox_cs_offset+(patch.data_segment-patch.code_segment)*16+0x2a82;
  if(mailbox<0)return null;
  const at=mailbox,data=at-patch.mailbox_cs_offset+(patch.data_segment-patch.code_segment)*16;
  return {at,mode:h[at+22],flags:h[at+23],generation:v.getUint16(at+24,true),ticks:v.getUint32(at+16,true),pointOptions:h[at+26],
    x:v.getInt16(data+0x464e,true)/16,y:v.getInt16(data+0x4650,true)/16,stage:h[data+0x5394],focus:h[data+0x3976],touchFlags:v.getUint16(at+28,true),
    ...readSoloDiagnostics(h,at,patch)};
}
function clear(){
  for(const event of held.values())if(emulator?.key)emulator.key(event.code,false);else environment.key({...event,type:'keyup'});
  held.clear();input={focus:0,points:0,touch:[0,0]};
  if(bridge)queueAssist({generation:bridge.update()?.generation,focus:0,touch:[0,0]});
  else if(coop)coop.clear();
  else if(mailbox>=0)queueAssist({address:mailbox,generation:new DataView(emulator.module.HEAPU8.buffer).getUint16(mailbox+24,true),points:0,touch:null});
}
function update(force=false){
  if(!emulator||emulator.practiceStandby&&!force)return;
  const now=performance.now();
  if(now-lastInput>750)clear();
  const h=emulator.module.HEAPU8;
  let state,markers=[];
  if(coop)state=coop.read();
  else if(bridge){state=bridge.update(now);markers=bridge.markers();if(state)state.ticks=new DataView(h.buffer).getUint32(state.at+patch.fields.ticks,true);}
  else state=scan04(now);
  pendingMusic.push(...(music?.read(h)||[]));
  if(!awaitingSnapshot&&(now-lastPublish>=15||pendingMusic.length)){
    lastPublish=now;awaitingSnapshot=true;const events=pendingMusic;pendingMusic=[];
    const render=now-lastRenderPublish>=250?emulator.module.framePresenter?.info():undefined;
    if(render)lastRenderPublish=now;
    postMessage({type:'snapshot',state,practice:emulator.practice?.status,markers,events,costs,drawTimes,render,heapBytes:h.length,frames:emulator.module.localFrameCount||0,uploads:emulator.module.frameUploads||0});costs=[];drawTimes=[];
  }
}
async function dispatch(message){
  const {method,args=[]}=message;
  if(method==='soloFault')return readSoloFault(emulator?.module.HEAPU8,emulator?.module);
  if(method==='init'){
    const setup=args[0];game=setup.game;patch=setup.patch;
    environment=workerEnvironment(setup.canvas,setup.sampleRate);
    const {NP21}=await import(game==='03'?'./vendor/np2/np2-original.js':'./dosbox-adapter.js');
    const externalReplay=setup.config.externalReplay;
    emulator=await NP21.create({...setup.config,artPatch:game==='04coop'?patch:null,canvas:setup.canvas,workerRuntime:true,nativeSoloAudio:false,
      applyReplayInput:applyAssist,
      ...(externalReplay?{externalReplay:{...externalReplay,onChunk:value=>postReplay('chunk',value),onEnd:value=>postReplay('end',value),onError:error=>postReplay('error',error),onRecordingError:error=>postReplay('error',error)}}:{}),
      onPracticeParked:practice=>{const at=emulator.practice.debug.csBase+patch.mailbox_cs_offset;seen.set(at,{ticks:0,changed:performance.now()});awaitingSnapshot=false;update(true);postMessage({type:'practice-parked',practice});},
      onError:error=>postMessage({type:'error',message:error.message}),
      onDiskChange:()=>postMessage({type:'diskChange'}),onExit:()=>{clear();postMessage({type:'exit'});}});
    if(game==='03'){
      const {NativePause}=await import('./native-pause.js');bridge=new NativePause(()=>emulator,patch);
    }
    if(game==='04coop'){const {createLocalWorkerState}=await import('./local-worker-state.js');coop=createLocalWorkerState(emulator,patch);}
    if(setup.music){const {NativeMusic}=await import(game==='03'?'./native-music.js':'./solo-native-music.js');music=new NativeMusic(setup.music);}
    emulator.module.observeReplayBoundary=()=>{pendingMusic.push(...(music?.read(emulator.module.HEAPU8)||[]));};
    observeFrames();
    timer=setInterval(update,16);
    return {heapBytes:emulator.module.HEAPU8.length,runtime:emulator.module.mobileRuntime};
  }
  if(method==='snapshotAck'){awaitingSnapshot=false;return;}
  if(method==='practiceActivate'){lastInput=performance.now();emulator.activatePractice(args[0]);awaitingSnapshot=false;update(true);return;}
  if(method==='practiceOptions'){emulator.practiceOptions(args[0]);return;}
  if(method==='practiceResultAck'){
    const [at,generation,ack]=args,h=emulator.module.HEAPU8,v=new DataView(h.buffer),entries=patch?.practice?.entries;
    if(game!=='04'||!entries||![1,2].includes(ack)||at<0||at+36>h.length||!signature.every((b,i)=>h[at+i]===b)||v.getUint16(at+24,true)!==generation)return false;
    const base=at-patch.mailbox_cs_offset;
    if(!h[base+entries.resultVisible]||!h[base+entries.resultRequest])return false;
    h[base+entries.resultAck]=ack;return true;
  }
  if(method==='practicePauseAck'){
    const [at,generation,ack]=args,h=emulator.module.HEAPU8,v=new DataView(h.buffer),entries=patch?.practice?.entries;
    if(game!=='04'||!entries||![1,2].includes(ack)||at<0||at+36>h.length||!signature.every((b,i)=>h[at+i]===b)||v.getUint16(at+24,true)!==generation)return false;
    const base=at-patch.mailbox_cs_offset;if(!h[base+entries.pauseOpen]||!h[base+entries.pauseRequest])return false;
    h[base+entries.pauseAck]=ack;return true;
  }
  if(method==='audio')return emulator.state==='running'?(emulator.readAudio?.()||environment.audio()):null;
  if(method==='key'){
    lastInput=performance.now();const event=args[0];
    if(event.type==='keydown')held.set(event.code,event);else if(event.type==='keyup')held.delete(event.code);
    if(emulator.key)emulator.key(event.code,event.type==='keydown');else environment.key(event);return;
  }
  if(method==='input'){
    lastInput=performance.now();input=args[0];if(Number.isInteger(input.resultInput)||Number.isInteger(input.pauseInput)){applyAssist(input);if(emulator.state==='running'&&emulator.externalHost?.status.held)emulator.externalHost.pause(false);}else queueAssist(input);return;
  }
  if(method==='clear'){clear();return;}
  if(method==='coopHit'){if(!coop)throw Error('本地合作尚未启动');coop.hit();return;}
  if(method==='soloTest'){
    if(game!=='04')throw Error('测试工具仅用于 TH04 独立单机。');
    writeSoloTestCommand(emulator.module.HEAPU8,mailbox,patch,args[0]);return;
  }
  if(method==='soloQuit'){
    const h=emulator.module.HEAPU8,at=args[2],v=new DataView(h.buffer);
    if(game!=='04'||at<0||at+36>h.length||!signature.every((b,i)=>h[at+i]===b)||h[at+22]!==1||v.getUint16(at+24,true)!==args[0])return false;
    emulator.assist({address:at,generation:args[0],...(args[1]?{restart:true}:{quit:true})});return true;
  }
  if(method==='pause'){clear();emulator.pause();return;}
  if(method==='reset'){pendingMusic=[];clear();bridge?.reset();music?.reset();seen.clear();mailbox=-1;cursor=0;await emulator.reset();costs=[];drawTimes=[];lastRenderPublish=-Infinity;observeFrames();return;}
  if(method==='run'){lastInput=performance.now();emulator.run();return;}
  if(method==='externalReplayFinish'){emulator?.finishReplay();return;}
  if(method==='externalReplayRearm'){emulator?.rearmBrowserRecording();return;}
  if(method==='externalReplayFast'){emulator?.fastForwardReplay(args[0]);return;}
  if(method==='externalReplayDisable'){emulator?.stopBrowserRecording();return;}
  if(method==='externalReplayPause'){const paused=!!args[0];if(paused)emulator?.pause();else emulator?.run();return;}
  if(method==='getDiskImage')return emulator.getDiskImage(...args);
  if(method==='addDiskImage'){emulator.addDiskImage(...args);return;}
  if(method==='setHdd'){await emulator.setHdd(...args);return;}
  throw Error('Unknown worker command: '+method);
}
// Requests arrive in order; serialize startup/disk operations as well as input.
let queue=Promise.resolve();
onmessage=({data})=>{queue=queue.then(async()=>{
  if(data?.name){if(data.name==='wc-sync-sleep')emulator?.module.receive?.({data});return;}
  try{
    const result=await dispatch(data);
    if(data.id){const transfer=Array.isArray(result)?result.map(channel=>channel.buffer):result instanceof Uint8Array?[result.buffer]:[];postMessage({type:'reply',id:data.id,result},transfer);}
  }catch(error){postMessage({type:'error',id:data.id,message:error?.stack||String(error),diagnostics:readSoloFault(emulator?.module.HEAPU8,emulator?.module)});}
});};
addEventListener('message',({data})=>{if(data?.name==='ws-sync-sleep')postMessage({name:'wc-sync-sleep',props:data.props});});
addEventListener('error',event=>postMessage({type:'error',message:event.message,diagnostics:readSoloFault(emulator?.module.HEAPU8,emulator?.module)}));
addEventListener('unhandledrejection',event=>postMessage({type:'error',message:String(event.reason?.stack||event.reason),diagnostics:readSoloFault(emulator?.module.HEAPU8,emulator?.module)}));
