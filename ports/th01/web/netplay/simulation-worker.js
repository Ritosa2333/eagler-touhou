import {NativeSimulation} from './runtime-core.js';

// This core owns gameplay. Display cores remain isolated: rendering must never
// mutate the memory used for checksums, menus, scores or confirmed sound.
let simulation,core,settings,scoreRevision,failed=false,renderers=[],layout,before,after;
let renderFrame=-1,sequence=0;const transfers=new Set();
const send=(message,transfer=[])=>postMessage(message,transfer);
function summary(){return {frame:simulation.frame,state:simulation.state(),checksum:simulation.checksum()};}
function audio(){
  const end=core.th_get(11,0)>>>0,start=Math.max(0,end-128);
  return {end,start,cue:core.th_get(79,0),events:Array.from({length:end-start},(_,i)=>core.th_sound_event(start+i))};
}
async function init(data){
  if(simulation)throw Error('联机模拟线程不能重复初始化');
  settings=data.settings;core=(await WebAssembly.instantiate(data.module,{})).exports;
  if(!core.th_net_checksum||!core.th_set_netplay_rules||core.memory.buffer.byteLength!==4194304)throw Error('游戏核心需要更新');
  core.th_score_data();scoreRevision=core.th_get(110,0);
  simulation=new NativeSimulation(core,settings,data.hostSlot);simulation.reset();
  const bytes=core.th_render_snapshot_info?.(0)===1?core.th_render_snapshot_info(1):0,offset=bytes?core.th_pixels():0;
  if(bytes&&(offset<0||bytes!==640*400*4||offset+bytes>core.memory.buffer.byteLength))throw Error('游戏核心绘图快照布局无效');
  layout=[bytes?1:0,core.memory.buffer.byteLength,offset,bytes];
  before=new Uint8Array(core.memory.buffer,0,offset);after=new Uint8Array(core.memory.buffer,offset+bytes);
  await Promise.all([0,1].map(()=>new Promise((resolve,reject)=>{
    const worker=new Worker(data.rendererURL),lane={worker,pending:null,snapshot:new ArrayBuffer(layout[1]-layout[3])};renderers.push(lane);
    const timer=setTimeout(()=>reject(Error('绘图线程启动超时')),5000);
    worker.onerror=event=>{clearTimeout(timer);reject(Error(event.message||'绘图线程失败'));fatal(Error(event.message||'绘图线程失败'));};
    worker.onmessage=({data:message})=>{
      if(failed){message.bitmap?.close();return;}
      if(message.type==='ready'){
        clearTimeout(timer);
        if(!Array.isArray(message.layout)||!message.layout.every((n,i)=>n===layout[i])||message.layout.length!==layout.length){reject(Error('绘图线程快照布局不匹配'));return;}
        resolve();return;
      }
      if(message.type==='error'){fatal(Error(message.message));return;}
      const job=lane.pending;
      if(message.type!=='frame'||!job||job.frame!==message.frame||!(message.snapshot instanceof ArrayBuffer)||message.snapshot.byteLength!==layout[1]-layout[3]){message.bitmap?.close();fatal(Error('绘图线程返回了无效帧'));return;}
      lane.pending=null;lane.snapshot=message.snapshot;
      const id=++sequence;transfers.add(id);
      send({type:'frame',sequence:id,frame:message.frame,bitmap:message.bitmap,cost:message.cost+job.snapshotMs,snapshotMs:job.snapshotMs,latency:performance.now()-job.started},[message.bitmap]);
    };
    worker.postMessage({type:'init',module:data.module,layout});
  })));
  return {...summary(),audio:audio()};
}
function present(localSlot){
  if(renderFrame===simulation.frame||transfers.size+renderers.filter(lane=>lane.pending).length>=2)return false;
  const lane=renderers.find(lane=>!lane.pending);if(!lane)return false;
  const at=performance.now(),snapshot=lane.snapshot,packed=new Uint8Array(snapshot);
  if(before.length)packed.set(before);packed.set(after,before.length);
  lane.pending={frame:simulation.frame,started:at,snapshotMs:performance.now()-at};lane.snapshot=null;renderFrame=simulation.frame;
  lane.worker.postMessage({type:'render',frame:simulation.frame,localSlot,snapshot},[snapshot]);return true;
}
function fatal(error,id){
  if(failed)return;failed=true;for(const lane of renderers)lane.worker.terminate();
  send({type:'error',id,message:error.stack||String(error)});
}
let chain=Promise.resolve();
onmessage=({data})=>{
  if(data.type==='frame-ack'){transfers.delete(data.sequence);return;}
  chain=chain.then(async()=>{
    if(failed)return;const started=performance.now();let result;
    if(data.type==='init')result=await init(data);
    else if(!simulation)throw Error('联机模拟线程尚未初始化');
    else if(data.type==='execute'){
      if(data.frame!==simulation.frame||!Array.isArray(data.inputs)||data.inputs.length!==settings.players||data.inputs.some(n=>!Number.isInteger(n)||n<0||n>4294967295))throw Error('联机模拟输入边界无效');
      simulation.step(data.inputs);result={...summary(),audio:audio()};
      const revision=core.th_get(110,0);
      if(revision!==scoreRevision){scoreRevision=revision;result.scores=new Uint8Array(core.memory.buffer,core.th_score_data(),core.th_score_size()).slice();}
    }else if(data.type==='present')result=present(data.localSlot);
    else throw Error('未知联机模拟指令');
    send({type:'result',id:data.id,result,executionMs:performance.now()-started},result?.scores?[result.scores.buffer]:[]);
  }).catch(error=>fatal(error,data.id));
};
addEventListener('unhandledrejection',event=>fatal(event.reason));
