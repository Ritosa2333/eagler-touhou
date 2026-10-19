import assert from 'node:assert/strict';
import {EventEmitter} from 'node:events';

/** Capture actual network bytes while a streaming Fetch consumer is still active. */
export async function createChromiumResponseSource(page,{
  maxResourceBufferSize=256*1024*1024,maxTotalBufferSize=512*1024*1024,bodyTimeoutMs=30000,
  acceptResponse=()=>true,
}={}){
  for(const value of [maxResourceBufferSize,maxTotalBufferSize,bodyTimeoutMs])
    assert.ok(Number.isSafeInteger(value)&&value>0,'Response capture budgets must be positive integers');
  assert.ok(maxTotalBufferSize>=maxResourceBufferSize,'Total response buffer must cover one resource');
  const session=await page.context().newCDPSession(page),source=new EventEmitter(),requests=new Map();
  source.capture={method:'CDP.Network.streamResourceContent',fallback:'CDP.Network.getResponseBody',
    maxResourceBufferSize,maxTotalBufferSize,bodyTimeoutMs};
  let closed=false,bufferedBytes=0;
  const reserve=(state,length)=>{
    if(state.captureError||state.disposed)return false;
    if(state.bytes+length>maxResourceBufferSize||bufferedBytes+length>maxTotalBufferSize){
      state.captureError=Error(`Browser response capture exceeded its byte budget: ${state.url}`);return false;
    }
    state.bytes+=length;bufferedBytes+=length;return true;
  };
  const append=(state,bytes,{prefix=false}={})=>{
    if(!reserve(state,bytes.length))return;
    if(prefix)state.prefix=bytes;else state.chunks.push(bytes);
  };
  const received=event=>{
    const {requestId,response}=event;if(!acceptResponse(response))return;
    let finish;
    const completed=new Promise(resolve=>{finish=resolve;});
    const state={requestId,url:response.url,finish,completed,chunks:[],bytes:0,streamError:null,terminal:null};
    requests.set(requestId,state);
    // Start immediately: the command rejects resources which have already finished.
    // dataReceived may precede the reply. bufferedData always belongs before those chunks.
    const streaming=session.send('Network.streamResourceContent',{requestId}).then(result=>{
      append(state,Buffer.from(result.bufferedData??'','base64'),{prefix:true});state.streaming=true;
    }).catch(error=>{state.streamError=String(error.message??error);});
    const details=()=>({requestId,url:response.url,resourceType:event.type,terminal:state.terminal,
      method:state.method,streamError:state.streamError,fallbackError:state.fallbackError??null,capturedBytes:state.bytes});
    let bodyPromise;
    const readBody=()=>bodyPromise??=(async()=>{
      let timer;
      const capture=(async()=>{
        state.terminal=await completed;await streaming;
        if(state.captureError)throw state.captureError;
        if(state.terminal.event==='captureClosed')throw Error(state.terminal.error);
        if(state.streaming&&state.bytes>0){
          state.method='Network.streamResourceContent';
          return Buffer.concat([state.prefix??Buffer.alloc(0),...state.chunks],state.bytes);
        }
        state.method='Network.getResponseBody';
        let result;
        try{result=await session.send('Network.getResponseBody',{requestId});}
        catch(error){
          state.fallbackError=String(error.message??error);
          if(state.streaming){state.method='Network.streamResourceContent';return Buffer.alloc(0);}
          throw error;
        }
        const encoding=result.base64Encoded?'base64':'utf8',length=Buffer.byteLength(result.body,encoding);
        if(!reserve(state,length))throw state.captureError??Error('Browser response capture already closed');
        const bytes=Buffer.from(result.body,encoding);
        assert.equal(bytes.length,length,'Browser response byte reservation differs from its decoded size');
        return bytes;
      })();
      try{
        return await Promise.race([capture,new Promise((_,reject)=>{
          timer=setTimeout(()=>reject(Error(`Browser response did not finish within ${bodyTimeoutMs}ms: ${response.url}`)),bodyTimeoutMs);
        })]);
      }catch(error){error.responseCapture=details();throw error;}
      finally{
        clearTimeout(timer);requests.delete(requestId);state.disposed=true;bufferedBytes-=state.bytes;
        state.chunks=[];state.prefix=null;state.finish({event:'captureClosed',error:'Response capture ended'});
      }
    })();
    source.emit('response',{
      url:()=>response.url,status:()=>response.status,allHeaders:async()=>response.headers,
      fromServiceWorker:()=>response.fromServiceWorker===true,body:readBody,captureDetails:details,
      requestId,resourceType:event.type,
    });
  };
  const data=event=>{
    const state=requests.get(event.requestId);
    if(state&&event.data!==undefined)append(state,Buffer.from(event.data,'base64'));
  };
  const finished=event=>requests.get(event.requestId)?.finish({event:'loadingFinished',encodedDataLength:event.encodedDataLength});
  const failed=event=>requests.get(event.requestId)?.finish({event:'loadingFailed',error:event.errorText,canceled:event.canceled===true});
  session.on('Network.responseReceived',received);session.on('Network.dataReceived',data);
  session.on('Network.loadingFinished',finished);
  session.on('Network.loadingFailed',failed);
  source.close=async()=>{
    if(closed)return;closed=true;
    session.off('Network.responseReceived',received);session.off('Network.dataReceived',data);
    session.off('Network.loadingFinished',finished);session.off('Network.loadingFailed',failed);
    for(const state of requests.values())state.finish({event:'captureClosed',error:'Response capture closed before completion'});
    await session.detach();requests.clear();
  };
  try{
    await session.send('Network.enable',{maxResourceBufferSize,maxTotalBufferSize});
    return source;
  }catch(error){await source.close().catch(()=>{});throw error;}
}
