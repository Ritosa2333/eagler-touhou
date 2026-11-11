import {NativeSimulation} from '../netplay/runtime-core.js';
import {digest} from './tape.js';
import {replicaBinary,captureNative,encodeNative,decodeNative,restoreNative} from './native-state.js';
import {NativeLocalSimulation,nativeLocalOpening,loadLocalOpening} from './native-local-core.js';
let simulation,canvas,context,image,serial=0,closed=false,confirmed=0,identity,base,loaded=false;
async function read(path){const r=await fetch(new URL('../'+path,import.meta.url),{cache:'no-cache'});if(!r.ok)throw Error('原生资源读取失败：'+path);return new Uint8Array(await r.arrayBuffer());}
async function load(header){
  if(loaded)throw Error('原生回放实例不能重新加载');loaded=true;identity=header.identity;
  const version=JSON.parse(new TextDecoder().decode(await read('netplay/runtime.json')));let binary;
  await Promise.all(Object.entries(version.files).map(async([path,hash])=>{const b=await read(path);if(await digest(b)!==hash)throw Error('原生资源身份不一致：'+path);if(path==='game.wasm')binary=b;}));
  const mod=await WebAssembly.compile(await replicaBinary(binary));if(WebAssembly.Module.imports(mod).length)throw Error('原生核心含外部导入');
  const core=(await WebAssembly.instantiate(mod,{})).exports;if(core.memory.buffer.byteLength!==4194304)throw Error('原生核心内存布局不符');
  core.th_score_data();
  if(header.adapter==='native-local/1'){loadLocalOpening(core,header.opening);simulation=new NativeLocalSimulation(core,header.settings,header.hostSlot);}
  else{simulation=new NativeSimulation(core,header.settings,header.hostSlot);simulation.reset();}
  canvas=new OffscreenCanvas(640,400);context=canvas.getContext('2d',{alpha:false});image=new ImageData(new Uint8ClampedArray(core.memory.buffer,core.th_pixels(),640*400*4),640,400);
  return header.adapter==='native-local/1'?nativeLocalOpening(core):digest(new TextEncoder().encode(JSON.stringify({version,settings:header.settings,hostSlot:header.hostSlot})));
}
let chain=Promise.resolve();
onmessage=({data})=>{if(data.type==='frame-ack')return;chain=chain.then(async()=>{
  if(closed)return;let result;
  if(data.type==='load')result=await load(data.header);
  else if(data.type==='execute'){if(data.frame!==simulation.frame)throw Error('原生录像缺帧');if(simulation instanceof NativeLocalSimulation)simulation.command(data.inputs[0]);else simulation.step(data.inputs);if(data.hash)result=simulation.checksum();const e=simulation.core,end=e.th_get(11,0)>>>0,ids=[];for(let n=Math.max(0,end-128);n<end;n++)ids.push(e.th_sound_event(n));postMessage({type:'native-audio',event:{cue:e.th_get(79,0),end,ids}});}
  else if(data.type==='present'){simulation.core.th_render();context.putImageData(image,0,0);const bitmap=canvas.transferToImageBitmap();postMessage({type:'frame',bitmap,sequence:++serial},[bitmap]);}
  else if(data.type==='confirm'){if(data.args[0]!==simulation.frame)throw Error('原生回放确认边界无效');confirmed=simulation.frame;}
  else if(data.type==='replaySave'){
    if(confirmed!==simulation.frame||data.args[0]!==identity)throw Error('原生检查点不在独立确认边界');
    const state=captureNative(simulation,identity),bytes=await encodeNative(state,simulation.frame?base:null);if(!simulation.frame)base=state;result={frame:simulation.frame,bytes,proof:state.host.proof};
  }else if(data.type==='replayLoad'){
    if(confirmed!==simulation.frame||data.args[2]!==identity)throw Error('原生检查点恢复边界无效');
    const baseline=data.args[1]?await decodeNative(data.args[1]):null,state=await decodeNative(data.args[0],baseline);result=restoreNative(simulation,state,identity);confirmed=simulation.frame;
    const e=simulation.core,end=e.th_get(11,0)>>>0;postMessage({type:'replay-audio-reset',nativeEnd:end,events:[{type:'native-audio',event:{cue:e.th_get(79,0),end,ids:[]}}]});
  }
  else if(!['start','confirm','setRawPresentation'].includes(data.type))throw Error('未知原生回放命令');
  const game=simulation.state();
  const scene=headerScene(game);
  postMessage({type:'result',id:data.id,result,status:{proof:JSON.stringify(simulation.checksum()),game,scene}});
 }).catch(error=>{closed=true;postMessage({type:'error',id:data.id,message:error.stack||String(error)});});};
function headerScene(game){return game?.stage>=1&&game.stage<=20?{kind:'stage',stage:game.stage,route:game.route}:null;}
