import {workerEnvironment} from './solo-worker-environment.js';
import {createRestartVisualGate} from './restart-visual.js';

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
  if(Number.isInteger(value.pauseSelection)){
    if(h[at+patch.fields.phase]===2&&value.pauseSelection>=0&&value.pauseSelection<=3)h[at+patch.fields.seat]=0x80|value.pauseSelection;
    return;
  }
  if(value.quit||value.restart){
    const ds=new DataView(h.buffer).getUint16(at+patch.fields.ds,true),cs=ds-patch.dataSegment+patch.codeSegment;
    const ram=at-patch.mailboxCsOffset-cs*16,quit=ram+ds*16+0x65a0;
    if(cs>0&&ram>=0&&ram+0x100000<=h.length&&quit>=ram&&quit<ram+0xa0000&&[1,2].includes(h[at+patch.fields.phase])){
      if(value.restart||value.scoreExit){
        // Route the native round exit through MAINL with the first stage and
        // the current character, instead of returning to OP's title screen.
        const data=ram+ds*16,view=new DataView(h.buffer),resident=ram+view.getUint16(data+0x1d90,true)+view.getUint16(data+0x1d92,true)*16,branch=ram+cs*16+0x7c;
        // GameExecl normally hides graphics before loading MAINL. MAINL's
        // first-stage path assumes graphics are already visible, so retain
        // them for this restart just as OP does when starting a fresh game.
        const hide=ram+cs*16+0x1d8a;
        if(resident<ram||resident+58>ram+0xa0000||h[branch]!==0x75||h[branch+1]!==6||h[hide]!==0x9a||view.getUint16(hide+1,true)!==0x1590)return;
        h[branch]=0xeb;h.fill(0x90,hide,hide+5);
        if(value.restart){
          h[resident+0x17]=0;h.fill(0,resident+0x18,resident+0x28);h[resident+0x33]=255;h[resident+0x34]=2;h[resident+0x35]=0;h[resident+0x36]=3;h[resident+0x38]=70+h[resident+0x0b]*25;
          if(h[resident+0x28]===1)h[resident+0x0d]=h[resident+0x29];
        }else{h[resident+0x17]=1;h[resident+0x34]=0;h[resident+0x35]=1;h[resident+0x36]=0;}
      }
      h[quit]=1;
      if(h[at+patch.fields.phase]===2)h[at+patch.fields.ack]=1;
    }
    return;
  }
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
  const visual=emulator.module.restartVisual;if(visual){visual.observe(state);if(visual.closed)emulator.module.restartVisual=null;}
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
    emulator=await DOSBoxPC98.create({...setup.config,canvas:setup.canvas,workerRuntime:true,replayPatch:patch,applyReplayInput:applyAssist,
      ...(externalReplay?{externalReplay:{...externalReplay,onChunk:value=>postReplay('chunk',value),onEnd:value=>postReplay('end',value),onError:error=>postReplay('error',error),onRecordingError:error=>postReplay('error',error)}}:{}),
      onDiskChange:name=>postMessage({type:'diskChange',name}),onScoreSaved:()=>postMessage({type:'scoreSaved'}),onExit:()=>{clear();postMessage({type:'exit'});}});
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
  if(method==='reconcileKeys'){
    const expected=new Set(args[0]);
    for(const [code] of held)if(!expected.has(code)){held.delete(code);emulator.key(code,false);}
    return;
  }
  if(method==='input'){
    lastInput=performance.now();queueAssist(args[0]);return;
  }
  if(method==='nativeRequest'){
    const state=bridge.update();
    if(!state||state.generation!==args[2])return false;
    return bridge.request(args[0],args[1]);
  }
  if(method==='soloQuit'){
    const h=emulator.module.HEAPU8,at=args[2],view=new DataView(h.buffer);
    if(game!=='03'||!bridge.valid(h,at)||h[at+patch.fields.phase]!==2||view.getUint16(at+patch.fields.generation,true)!==args[0])return false;
    if(args[1]||args[3]){const code=at-patch.mailboxCsOffset,branch=code+0x7c,hide=code+0x1d8a;if(h[branch]!==0x75||h[branch+1]!==6||h[hide]!==0x9a||view.getUint16(hide+1,true)!==0x1590)return false;}
    if(args[1]){emulator.module.restartVisual?.release();emulator.module.restartVisual=createRestartVisualGate(emulator.module.framePresenter);}
    applyAssist({address:at,generation:args[0],focus:0,touch:[0,0],...(args[1]?{restart:true}:args[3]?{quit:true,scoreExit:true}:{quit:true})});
    return true;
  }
  if(method==='soloPauseCancel'){
    const h=emulator.module.HEAPU8,at=args[1],view=new DataView(h.buffer);
    if(game!=='03'||!bridge.valid(h,at)||h[at+patch.fields.phase]!==2||view.getUint16(at+patch.fields.generation,true)!==args[0])return false;
    h[at+patch.fields.ack]=2;return true;
  }
  if(method==='soloPauseSelect'){
    const h=emulator.module.HEAPU8,at=args[1],view=new DataView(h.buffer),choice=args[2];
    if(game!=='03'||!bridge.valid(h,at)||h[at+patch.fields.phase]!==2||view.getUint16(at+patch.fields.generation,true)!==args[0]||!Number.isInteger(choice)||choice<0||choice>3)return false;
    queueAssist({address:at,generation:args[0],focus:0,touch:[0,0],pauseSelection:choice});return true;
  }
  if(method==='nativeReset'){bridge.reset();pendingResults=[];return;}
  if(method==='clear'){clear();return;}
  if(method==='pause'){clear();emulator.pause();return;}
  if(method==='reset'){
    pendingMusic=[];pendingResults=[];clear();bridge.reset();music?.reset();await emulator.reset();return;
  }
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
