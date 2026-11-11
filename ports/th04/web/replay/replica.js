import {stable,fingerprint} from './tape.js';
import {GAME,SOURCE_PATHS,WORKER} from './profile.js';
// A fresh ordinary runtime Worker; no player instance is used for playback.
export async function createReplica(header,{onFrame=()=>{},onEvent=()=>{},onProgress=()=>{},signal}={}){
  if(signal?.aborted)throw Error('回放启动已取消');
  if(header.game!==GAME||header.identity!==await fingerprint(SOURCE_PATHS))throw Error('录像作品或浏览器运行版本不匹配');
  if(signal?.aborted)throw Error('回放启动已取消');
  const worker=new Worker(new URL(WORKER,import.meta.url),{type:'module'}),pending=new Map();let serial=0,closed=false;
  function stop(){if(closed)return;closed=true;signal?.removeEventListener('abort',stop);worker.terminate();for(const p of pending.values()){clearTimeout(p.timer);p.reject(Error('回放实例已停止'));}pending.clear();}
  signal?.addEventListener('abort',stop,{once:true});
  function request(type,args=[],extra={}){
    if(closed)return Promise.reject(Error('回放实例已停止'));
    const id=++serial;return new Promise((resolve,reject)=>{const timer=setTimeout(()=>{pending.delete(id);reject(Error('回放线程响应超时'));stop();},120000);pending.set(id,{resolve,reject,timer});worker.postMessage({type,args,id,...extra});});
  }
  worker.onmessage=({data})=>{
    if(data.type==='frame'){try{onFrame(data.bitmap);}finally{data.bitmap.close();worker.postMessage({type:'frame-ack',sequence:data.sequence});}return;}
    if(data.type==='notify'){onProgress(data.message.message||'');return;}
    if(['music','se','sfx','se-stop','native-audio','battle-audio','replay-audio-reset'].includes(data.type)){onEvent(data);return;}
    const p=pending.get(data.id);if(!p)return;pending.delete(data.id);clearTimeout(p.timer);
    if(data.type==='error')p.reject(Error(data.message));else p.resolve({value:data.result,status:data.status});
  };
  worker.onerror=e=>{for(const p of pending.values()){clearTimeout(p.timer);p.reject(Error(e.message));}pending.clear();stop();};
  try{
    const loaded=await request('load',[],{configure:[header.hostSlot,0,header.players],settings:header.settings,header,spectator:true});
    if(stable(loaded.value)!==stable(header.opening))throw Error('录像开局资源校验不一致');
    await request('setRawPresentation',[true]);await request('start');
    let frame=0,executed=0,scene=null;
    return {
      get frame(){return frame;},get executed(){return executed;},get scene(){return scene;},stop,
      async advance(row){
        const r=await request('execute',[],{frame,inputs:row.inputs,offlineMask:row.offlineMask,replay:false,capture:false,hash:row.hash!==null});
        if(r.status.proof!==row.proof||(row.hash!==null&&stable(r.value)!==stable(row.hash)))throw Error('录像模拟校验不一致：第 '+frame+' 步');
        await request('confirm',[frame+1]);scene=r.status.scene??null;frame++;executed++;return r;
      },
      present:()=>request('present'),
      async save(){const r=await request('replaySave',[header.identity]);return r.value;},
      async restore(bytes,baseBytes){const r=await request('replayLoad',[bytes,baseBytes,header.identity]);frame=r.value.frame;return r.value;},
      request
    };
  }catch(e){stop();throw e;}
}
