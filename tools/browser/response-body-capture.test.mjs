import test from 'node:test';
import assert from 'node:assert/strict';
import {EventEmitter} from 'node:events';
import {createChromiumResponseSource} from './response-body-capture.mjs';

function fixture({stream=async()=>{throw Error('Streaming unavailable');},body=Buffer.from([0,1,255])}={}){
  const session=new EventEmitter(),calls=[];let detached=false;
  session.send=async(method,options)=>{
    calls.push({method,options});
    if(method==='Network.streamResourceContent')return stream(options);
    if(method==='Network.getResponseBody')return{body:body.toString('base64'),base64Encoded:true};
  };
  session.detach=async()=>{detached=true;};
  return{session,calls,detached:()=>detached,page:{context:()=>({newCDPSession:async()=>session})}};
}

test('a resource which finishes before streaming starts uses the exact binary body fallback',async()=>{
  const f=fixture(),source=await createChromiumResponseSource(f.page);
  let body;
  source.on('response',response=>{assert.equal(response.url(),'https://fixture.test/font');body=response.body();});
  try{
    assert.deepEqual(f.calls,[{method:'Network.enable',options:{maxResourceBufferSize:268435456,maxTotalBufferSize:536870912}}]);
    f.session.emit('Network.responseReceived',{requestId:'font',type:'Font',response:{url:'https://fixture.test/font',status:200,headers:{}}});
    await Promise.resolve();assert.equal(f.calls.filter(call=>call.method==='Network.getResponseBody').length,0);
    f.session.emit('Network.loadingFinished',{requestId:'font',encodedDataLength:3});
    assert.deepEqual(await body,Buffer.from([0,1,255]));
    assert.equal(f.calls.at(-1).options.requestId,'font');
  }finally{await source.close();}
  assert.equal(f.detached(),true);assert.equal(f.session.listenerCount('Network.responseReceived'),0);
});

test('actual stream bytes include the buffered prefix before chunks delivered ahead of the command reply',async()=>{
  let enableStream;
  const f=fixture({stream:()=>new Promise(resolve=>{enableStream=resolve;})});
  const source=await createChromiumResponseSource(f.page);let response,body;
  source.on('response',value=>{response=value;body=value.body();});
  try{
    f.session.emit('Network.responseReceived',{requestId:'font',type:'Fetch',response:{url:'https://fixture.test/font',status:200,headers:{}}});
    f.session.emit('Network.dataReceived',{requestId:'font',data:Buffer.from([2,255]).toString('base64')});
    enableStream({bufferedData:Buffer.from([0,1]).toString('base64')});
    f.session.emit('Network.dataReceived',{requestId:'font',data:Buffer.from([3]).toString('base64')});
    f.session.emit('Network.loadingFailed',{requestId:'font',errorText:'net::ERR_ABORTED',canceled:true});
    assert.deepEqual(await body,Buffer.from([0,1,2,255,3]));
    assert.deepEqual(response.captureDetails().terminal,{event:'loadingFailed',error:'net::ERR_ABORTED',canceled:true});
    assert.equal(response.captureDetails().method,'Network.streamResourceContent');
    assert.equal(f.calls.some(call=>call.method==='Network.getResponseBody'),false);
  }finally{await source.close();}
});

test('an empty successful streaming command still reads a small script from the response-body fallback',async()=>{
  const script=Buffer.from('window.scriptExecuted=true;'),f=fixture({stream:async()=>({bufferedData:''}),body:script});
  const source=await createChromiumResponseSource(f.page);let body,response;
  source.on('response',value=>{response=value;body=value.body();});
  try{
    f.session.emit('Network.responseReceived',{requestId:'script',response:{url:'https://fixture.test/script.js',status:200,headers:{'Content-Length':script.length}}});
    f.session.emit('Network.loadingFinished',{requestId:'script',encodedDataLength:script.length});
    assert.deepEqual(await body,script);
    assert.equal(response.captureDetails().method,'Network.getResponseBody');
    assert.equal(response.captureDetails().terminal.event,'loadingFinished');
  }finally{await source.close();}
});

test('body capture preserves failed transport separately and rejects a stalled response',async()=>{
  const f=fixture(),source=await createChromiumResponseSource(f.page,{bodyTimeoutMs:20});
  const reads=[],responses=[];source.on('response',response=>{responses.push(response);reads.push(response.body().then(bytes=>bytes,error=>error));});
  try{
    f.session.emit('Network.responseReceived',{requestId:'broken',response:{url:'https://fixture.test/broken',status:200,headers:{}}});
    f.session.emit('Network.loadingFailed',{requestId:'broken',errorText:'net::ERR_FAILED'});
    assert.deepEqual(await reads[0],Buffer.from([0,1,255]));
    assert.equal(responses[0].captureDetails().terminal.error,'net::ERR_FAILED');
    f.session.emit('Network.responseReceived',{requestId:'stalled',response:{url:'https://fixture.test/stalled',status:200,headers:{}}});
    assert.match(String(await reads[1]),/did not finish within 20ms.*stalled/);
    assert.equal(f.calls.filter(call=>call.method==='Network.getResponseBody').length,1);
  }finally{await source.close();}
});

test('streaming cannot bypass resource byte limits',async()=>{
  const f=fixture({stream:async()=>({bufferedData:Buffer.from([1,2]).toString('base64')})});
  const source=await createChromiumResponseSource(f.page,{maxResourceBufferSize:2,maxTotalBufferSize:3});
  let result;source.on('response',response=>{result=response.body().catch(error=>error);});
  try{
    f.session.emit('Network.responseReceived',{requestId:'large',response:{url:'https://fixture.test/large',status:200,headers:{}}});
    f.session.emit('Network.dataReceived',{requestId:'large',data:Buffer.from([3]).toString('base64')});
    f.session.emit('Network.loadingFinished',{requestId:'large'});
    assert.match(String(await result),/exceeded its byte budget/);
  }finally{await source.close();}
});

test('concurrent fallback bodies share the same total capture budget',async()=>{
  const f=fixture({body:Buffer.from([1,2])}),source=await createChromiumResponseSource(f.page,
    {maxResourceBufferSize:2,maxTotalBufferSize:3});
  const reads=[];source.on('response',response=>reads.push(response.body().then(body=>body,error=>error)));
  try{
    for(const requestId of ['first','second'])
      f.session.emit('Network.responseReceived',{requestId,response:{url:'https://fixture.test/'+requestId,status:200,headers:{}}});
    for(const requestId of ['first','second'])f.session.emit('Network.loadingFinished',{requestId});
    const results=await Promise.all(reads);
    assert.deepEqual(results[0],Buffer.from([1,2]));
    assert.match(String(results[1]),/exceeded its byte budget/);
  }finally{await source.close();}
});
