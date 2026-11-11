import {workerEnvironment} from './solo-worker-environment.js';

let emulator, environment, bridge, music, patch, game;
let awaitingSnapshot=false, pendingMusic=[], pendingResults=[], lastPublish=0, costs=[], lastInput=performance.now();
const held=new Map();
function postReplay(type,value){
  if(type==='chunk'&&value.clocks)postMessage({type:'external-replay-chunk',chunk:value},[value.clocks.buffer]);
  else postMessage({type:'external-replay-'+type,...(type==='chunk'?{chunk:value}:type==='end'?{result:value}:{message:value?.message||String(value)})});
}
function applyAssist(value){
  if(!bridge)return;
  const h=emulator.module.HEAPU8;
  const at=Number.isInteger(value.address)?value.address:bridge.live;
  if(!bridge.valid(h,at)||new DataView(h.buffer).getUint16(at+patch.fields.generation,true)!==value.generation)return;
  const previous=bridge.live;bridge.live=at;
  try{bridge.setFocus(value.focus,0);bridge.setTouch(value.touch||[0,0]);}finally{bridge.live=previous;}
}
function queueAssist(value){value={...value,address:value.address??bridge?.live};if(emulator?.assist)emulator.assist(value);else applyAssist(value);}
function clear(){
  for(const event of held.values())emulator?.key(event.code,false);
  held.clear();if(bridge)queueAssist({generation:bridge.update()?.generation,focus:0,touch:[0,0]});
}
function update(){
  if(!emulator)return;
  const now=performance.now();
  if(now-lastInput>750)clear();
  const h=emulator.module.HEAPU8,state=bridge.update(now),markers=bridge.markers();
  if(state)state.ticks=new DataView(h.buffer).getUint32(state.at+patch.fields.ticks,true);
  if(game==='03local'){
    const result=bridge.result(now);
    if(result&&result!=='pending')pendingResults.push(result);
  }
  pendingMusic.push(...(music?.read(h)||[]));
  if(!awaitingSnapshot&&(now-lastPublish>=15||pendingMusic.length||pendingResults.length)){
    lastPublish=now;awaitingSnapshot=true;
    postMessage({type:'snapshot',state,markers,selecting:bridge.selecting,nativePending:bridge.pending,
      events:pendingMusic,pauseResults:pendingResults,costs,heapBytes:h.length,
      frames:emulator.module.localFrameCount||0,uploads:emulator.module.frameUploads||0});
    pendingMusic=[];pendingResults=[];costs=[];
  }
}
async function dispatch({method,args=[]}){
  if(method==='init'){
    const setup=args[0];game=setup.game;patch=setup.patch;
    environment=workerEnvironment(setup.canvas,setup.sampleRate);
    const {DOSBoxPC98}=await import('./dosbox-adapter.js');
    const externalReplay=setup.config.externalReplay;
    emulator=await DOSBoxPC98.create({...setup.config,canvas:setup.canvas,workerRuntime:true,applyReplayInput:applyAssist,
      ...(externalReplay?{externalReplay:{...externalReplay,onChunk:value=>postReplay('chunk',value),onEnd:value=>postReplay('end',value),onError:error=>postReplay('error',error),onRecordingError:error=>postReplay('error',error)}}:{}),
      onDiskChange:name=>postMessage({type:'diskChange',name}),onExit:()=>{clear();postMessage({type:'exit'});}});
    const {NativePause}=await import('./native-pause.js');bridge=new NativePause(()=>emulator,patch);
    if(setup.music){const {NativeMusic}=await import('./native-music.js');music=new NativeMusic(setup.music);}
    setInterval(update,16);
    return {heapBytes:emulator.module.HEAPU8.length,runtime:emulator.module.mobileRuntime};
  }
  if(method==='snapshotAck'){awaitingSnapshot=false;return;}
  if(method==='performance'){
    costs=[];
    if(args[0])emulator.module.observeLocalFrame=cost=>{if(costs.length<120)costs.push(cost);};
    else delete emulator.module.observeLocalFrame;
    return;
  }
  if(method==='audio')return emulator.state==='running'?emulator.readAudio():null;
  if(method==='key'){
    lastInput=performance.now();const event=args[0];
    if(event.type==='keydown')held.set(event.code,event);else if(event.type==='keyup')held.delete(event.code);
    if(event.type==='keydown'||event.type==='keyup')emulator.key(event.code,event.type==='keydown');return;
  }
  if(method==='input'){
    lastInput=performance.now();queueAssist(args[0]);return;
  }
  if(method==='nativeRequest'){
    const state=bridge.update();
    if(!state||state.generation!==args[2])return false;
    return bridge.request(args[0],args[1]);
  }
  if(method==='nativeReset'){bridge.reset();pendingResults=[];return;}
  if(method==='clear'){clear();return;}
  if(method==='pause'){clear();emulator.pause();return;}
  if(method==='reset'){
    pendingMusic=[];pendingResults=[];clear();bridge.reset();music?.reset();await emulator.reset();return;
  }
  if(method==='run'){lastInput=performance.now();emulator.run();return;}
  if(method==='externalReplayFinish'){emulator?.finishReplay();return;}
  if(method==='externalReplayFast'){emulator?.fastForwardReplay(args[0]);return;}
  if(method==='externalReplayDisable'){emulator?.stopBrowserRecording();return;}
  if(method==='externalReplayPause'){const paused=!!args[0];if(paused)emulator?.pause();else emulator?.run();return;}
  if(method==='getDiskImage')return emulator.getDiskImage(...args);
  if(method==='addDiskImage'){emulator.addDiskImage(...args);return;}
  if(method==='setHdd'){await emulator.setHdd(...args);return;}
  throw Error('Unknown worker command: '+method);
}
let queue=Promise.resolve();
onmessage=({data})=>{queue=queue.then(async()=>{
  if(data?.name)return;
  try{
    const result=await dispatch(data);
    if(data.id){const transfer=Array.isArray(result)?result.map(channel=>channel.buffer):result instanceof Uint8Array?[result.buffer]:[];
      postMessage({type:'reply',id:data.id,result},transfer);}
  }catch(error){postMessage({type:'error',id:data.id,message:error?.stack||String(error)});}
});};
addEventListener('error',event=>postMessage({type:'error',message:event.message}));
addEventListener('unhandledrejection',event=>postMessage({type:'error',message:String(event.reason?.stack||event.reason)}));
