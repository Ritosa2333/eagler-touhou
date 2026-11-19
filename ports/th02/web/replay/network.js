import {MAX_FRAMES,MAX_RAW,stable,pointBytes,toBase64,digest,MAX_POINT_BYTES,MAX_POINT_TOTAL} from './tape.js';
import {validScene} from './scenes.js';
// HTTP only stores confirmed rows. Neither retries nor a slow reader block the game.
export function publishNetwork(header,getRows,getEnd,identity,{onLink=()=>{},onStatus=()=>{},getPoints=()=>({points:[],done:true})}={}){
  let watch='',cursor=0,busy=false,stopped=false,ended=false,pointsEnded=false,timer=null,lastProgress=performance.now(),pointIndex=0,pointOffset=0,pointData;
  const controller=new AbortController();
  async function request(action,body){
    const response=await fetch('/api/replay/'+action,{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({...identity,...body}),signal:AbortSignal.any([controller.signal,AbortSignal.timeout(5000)])});
    const value=await response.json();if(!response.ok)throw Error(value.error||'观战服务不可用');return value;
  }
  function stop(){stopped=true;clearTimeout(timer);timer=null;controller.abort();}
  async function pump(){
    if(stopped||busy||pointsEnded)return;busy=true;
    try{
      const rows=getRows();if(rows.length-cursor>3600)throw Error('旁观上传积压，停止网络发布');
      if(!watch){const opened=await request('open',{header});if(stopped)return;if(typeof opened.watch!=='string'||!/^[\w-]{32,80}$/.test(opened.watch)||!Number.isInteger(opened.frames)||opened.frames<0||opened.frames>rows.length)throw Error('观战发布响应无效');watch=opened.watch;cursor=opened.frames;lastProgress=performance.now();const url=new URL('../replay.html',import.meta.url);url.searchParams.set('remote',watch);onLink(url.href);}
      let batches=0;
      while(!stopped&&cursor<getRows().length&&batches++<4){
        const batch=[];let size=0;for(const row of getRows().slice(cursor,cursor+60)){const n=stable(row).length*3;if(batch.length&&size+n>40000)break;batch.push(row);size+=n;}
        const first=cursor,result=await request('push',{watch,first,rows:batch});if(result.frames!==first+batch.length)throw Error('观战确认范围不一致');cursor=result.frames;lastProgress=performance.now();
      }
      const end=getEnd();if(!ended&&!stopped&&end&&cursor===end.frame){const result=await request('end',{watch,frame:end.frame,reason:end.reason});if(result.frames!==cursor)throw Error('观战结束边界不一致');ended=true;}
      // Inputs always go first. Upload at most four small point chunks per turn.
      const state=getPoints();let chunks=0;
      while(!stopped&&cursor===getRows().length&&pointIndex<state.points.length&&chunks++<4){
        const point=state.points[pointIndex];if(point.frame>cursor)break;
        pointData??=pointBytes(point);const chunk=pointData.subarray(pointOffset,pointOffset+32768);
        const r=await request('point',{watch,frame:point.frame,sha:point.sha,size:pointData.length,offset:pointOffset,payload:toBase64(chunk),scene:point.scene??null});
        if(r.offset!==pointOffset+chunk.length)throw Error('检查点上传确认范围不一致');pointOffset=r.offset;lastProgress=performance.now();
        if(pointOffset===pointData.length){pointData=null;pointOffset=0;pointIndex++;}
      }
      if(ended&&state.done&&pointIndex===state.points.length){await request('points-end',{watch});pointsEnded=true;onStatus('网络观战历史与定位点已封存');}
    }catch(e){
      if(stopped)return;
      if(getRows().length-cursor>3600||performance.now()-lastProgress>15000){stop();onStatus('网络观战停止：'+e.message);}
      else onStatus('网络观战重试中：'+e.message);
    }finally{busy=false;if(!stopped&&!pointsEnded){timer=setTimeout(()=>{timer=null;pump();},cursor<getRows().length||pointIndex<getPoints().points.length?20:500);}}
  }
  void pump();return {kick(){clearTimeout(timer);timer=null;void pump();},stop,get done(){return pointsEnded;},get stopped(){return stopped;},
    async seal(timeout=30000){const deadline=performance.now()+timeout;this.kick();while(!pointsEnded&&!stopped&&performance.now()<deadline)await new Promise(resolve=>setTimeout(resolve,50));return pointsEnded;}};
}

export function subscribeNetwork(watch,onSlice,onStatus,onError){
  let stopped=false,timer=null,first=0,bytes=0,lastProgress=performance.now();
  async function read(){
    if(stopped)return;
    try{
      const response=await fetch('/api/replay?'+new URLSearchParams({watch,from:first}),{cache:'no-store',signal:AbortSignal.timeout(5000)});
      if(response.status===403||response.status===404)throw Object.assign(Error('观战链接无效或已过期'),{fatal:true});
      if(!response.ok)throw Error('观战读取失败：'+response.status);
      const text=await response.text();if(text.length>65536)throw Object.assign(Error('观战响应超过容量'),{fatal:true});
      if(stopped)return;
      const slice=JSON.parse(text);if(slice.first!==first||!Array.isArray(slice.rows)||slice.rows.length>60||!Number.isInteger(slice.available)||slice.available>MAX_FRAMES)throw Object.assign(Error('观战批次无效'),{fatal:true});
      if(slice.rows.length)bytes+=stable(slice.rows).length*3;if(bytes>MAX_RAW*2)throw Object.assign(Error('观战下载达到容量上限'),{fatal:true});
      if(slice.rows.length)lastProgress=performance.now();try{onSlice(slice);}catch(e){e.fatal=true;throw e;}first+=slice.rows.length;
      if(slice.end&&first===slice.end.frame&&slice.pointsDone)return;
      if(first<slice.available){if(!stopped)timer=setTimeout(read,0);return;}
    }catch(e){if(stopped)return;if(e.fatal){stopped=true;onError(e);return;}onStatus('观战连接暂时中断，正在重试：'+e.message);}
    if(!stopped)timer=setTimeout(read,performance.now()-lastProgress>15000?1000:250);
  }
  void read();return {stop(){stopped=true;clearTimeout(timer);}};
}

export async function readNetworkPoint(watch,point,signal){
  const result=new Uint8Array(point.size);let offset=0;
  while(offset<result.length){
    let data,lastError;
    for(let retry=0;retry<3;retry++){
      if(signal?.aborted)throw Error('检查点下载已取消');
      try{
        const timeout=AbortSignal.timeout(5000),abort=signal?AbortSignal.any([signal,timeout]):timeout;
        const r=await fetch('/api/replay-point?'+new URLSearchParams({watch,frame:point.frame,offset}),{cache:'no-store',signal:abort});
        if(!r.ok)throw Error('检查点读取失败：'+r.status);const text=await r.text();if(text.length>65536)throw Error('检查点分块超过容量');data=JSON.parse(text);break;
      }catch(e){lastError=e;}
    }
    if(!data)throw lastError;
    if(signal?.aborted)throw Error('检查点下载已取消');
    if(data.frame!==point.frame||data.sha!==point.sha||data.size!==point.size||data.offset!==offset||typeof data.payload!=='string'||data.payload.length>43692)throw Error('检查点分块身份无效');
    const chunk=pointBytes({data:data.payload});if(!chunk.length||chunk.length>32768||offset+chunk.length>result.length)throw Error('检查点分块范围无效');
    result.set(chunk,offset);offset+=chunk.length;
  }
  if(await digest(result)!==point.sha)throw Error('检查点下载摘要不一致');return result;
}

export function validatePointIndex(index,available){
  if(!Array.isArray(index)||index.length>128)throw Error('观战检查点索引无效');let previous=-1,total=0;
  for(const p of index){if(!Number.isInteger(p.frame)||p.frame<0||p.frame>available||p.frame<=previous||previous===-1&&p.frame!==0||!Number.isInteger(p.size)||p.size<1||p.size>MAX_POINT_BYTES||!/^[a-f0-9]{64}$/.test(p.sha)||!validScene(p.scene))throw Error('观战检查点范围无效');previous=p.frame;total+=p.size;}
  if(total>MAX_POINT_TOTAL)throw Error('观战检查点超过容量');return index;
}
