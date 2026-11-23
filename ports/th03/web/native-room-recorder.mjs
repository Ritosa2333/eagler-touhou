const BATCH=120,MAX_FRAMES=216000,MAX_PENDING=2;

export function createRoomRecorder({focus,difficulty,seed,memory,onError,createWorker=()=>new Worker(new URL('./native-room-recorder.worker.mjs',import.meta.url),{type:'module'})}) {
  let worker,stopped=false,batch=[],frames=0,nextId=0;
  const pending=new Set();
  function close(){worker?.terminate();worker=undefined;batch=[];}
  function stop(){stopped=true;batch=[];if(!pending.size)close();}
  function fail(error){stopped=true;close();pending.clear();onError(error);}
  function start(){
    worker=createWorker();
    worker.onmessage=({data})=>{
      if(!pending.delete(data.id))return;
      if(data.error)onError(Error(data.error));
      else memory.set(data.path,data.bytes);
      if(stopped&&!pending.size)close();
    };
    worker.onerror=event=>fail(Error(event.message||'录像后台任务失败'));
    worker.onmessageerror=()=>fail(Error('无法读取录像后台任务结果'));
  }
  function flush(){
    if(!batch.length)return;
    if(!worker)start();
    worker.postMessage({type:'append',rows:batch});batch=[];
  }
  return {
    append(inputs,charge){
      if(stopped)return;
      if(++frames>MAX_FRAMES){stop();return;}
      batch.push(charge?[...inputs,...charge]:inputs);
      if(batch.length===BATCH)try{flush();}catch(error){fail(error);}
    },
    save(engine){
      if(stopped||!frames||pending.size>=MAX_PENDING)return;
      const size=engine._th03_classic_replay_size();
      if(!size||size>16*1024*1024){onError(Error('原生录像大小无效'));return;}
      try{
        const ptr=engine._th03_classic_replay_data(),native=engine.HEAPU8.slice(ptr,ptr+size);
        flush();
        const id=++nextId;
        worker.postMessage({type:'save',id,frames,native,focus,difficulty,seed},[native.buffer]);pending.add(id);
      }catch(error){fail(error);}
    },
    stop,
  };
}
