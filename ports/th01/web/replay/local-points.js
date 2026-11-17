import {pointBytes,digest} from './tape.js';
export const LOCAL_CHUNK=24576;
export const pointSize=p=>p.data.length/4*3-(p.data.endsWith('==')?2:p.data.endsWith('=')?1:0);
export const pointIndex=points=>points.map(p=>({frame:p.frame,sha:p.sha,size:pointSize(p),...(p.scene?{scene:p.scene}:{})}));

export async function readLocalPoint(channel,viewer,point,signal){
  const bytes=new Uint8Array(point.size);let offset=0;
  while(offset<bytes.length){
    let data,lastError;
    for(let retry=0;retry<3;retry++){
      if(signal?.aborted)throw Error('检查点读取已取消');
      try{
        data=await new Promise((resolve,reject)=>{
          const cleanup=()=>{clearTimeout(timer);channel.removeEventListener('message',receive);signal?.removeEventListener('abort',abort);};
          const abort=()=>{cleanup();reject(Error('检查点读取已取消'));};
          const receive=({data})=>{if(data?.type!=='point'||data.viewer!==viewer||data.frame!==point.frame||data.offset!==offset)return;cleanup();resolve(data);};
          const timer=setTimeout(()=>{cleanup();reject(Error('本机检查点来源暂时无响应'));},3000);
          channel.addEventListener('message',receive);signal?.addEventListener('abort',abort,{once:true});
          channel.postMessage({type:'pull-point',viewer,frame:point.frame,offset});
        });break;
      }catch(e){lastError=e;}
    }
    if(!data)throw lastError;
    if(data.sha!==point.sha||data.size!==point.size||typeof data.payload!=='string'||data.payload.length>32768)throw Error('本机检查点分块身份无效');
    const chunk=pointBytes({data:data.payload});if(chunk.length!==Math.min(LOCAL_CHUNK,bytes.length-offset))throw Error('本机检查点分块范围无效');bytes.set(chunk,offset);offset+=chunk.length;
  }
  if(await digest(bytes)!==point.sha)throw Error('本机检查点摘要不一致');return bytes;
}
