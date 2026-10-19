import test from 'node:test';
import assert from 'node:assert/strict';
import {attributeRuntimeRequest,createNavigationAbortEvidence,shouldRecordHttpFailure,completedSharedResourceAbort} from './navigation-abort-evidence.mjs';

const frame=(url,parent=null)=>({url:()=>url,parentFrame:()=>parent});
const request=({url='https://inject.invalid/pagehide',navigation=false,current='http://127.0.0.1/host/th08?epoch=1',parent=null,failure='net::ERR_ABORTED'}={})=>({
  url:()=>url,isNavigationRequest:()=>navigation,frame:()=>frame(current,parent),failure:()=>({errorText:failure})
});

test('only exact aborts captured from a retired runtime epoch are expected',()=>{
  const evidence=createNavigationAbortEvidence();
  const old=request();evidence.captureRequest(old);
  assert.deepEqual(evidence.retirePage({mainFrame:()=>frame('http://127.0.0.1/host/th08?epoch=1')}),
    {epoch:1,source:'host-document',url:'http://127.0.0.1/host/th08?epoch=1'});
  assert.equal(evidence.requestFailure(old).expectedNavigationAbort,true);

  const current=request({current:'http://127.0.0.1/host/th08?epoch=2'});
  evidence.captureRequest(current);
  assert.equal(evidence.requestFailure(current).expectedNavigationAbort,false,'current epoch abort must fail');

  const unknown=request({current:'https://inject.invalid/unknown'});
  evidence.captureRequest(unknown);
  assert.equal(evidence.requestFailure(unknown).attribution.source,'unknown');
  assert.equal(evidence.requestFailure(unknown).expectedNavigationAbort,false,'unknown source abort must fail');

  const connectionReset=request({failure:'net::ERR_CONNECTION_RESET'});
  evidence.captureRequest(connectionReset);
  assert.equal(evidence.requestFailure(connectionReset).expectedNavigationAbort,false,'other network errors must fail');

  const unobserved=request();
  assert.equal(evidence.requestFailure(unobserved).attribution.source,'unknown');
  assert.equal(evidence.requestFailure(unobserved).expectedNavigationAbort,false,'requests without start evidence must fail');
});

test('a new document navigation URL takes precedence over the old frame URL',()=>{
  const next=request({url:'http://127.0.0.1/host/th08?epoch=2',navigation:true,current:'http://127.0.0.1/host/th08?epoch=1'});
  assert.deepEqual(attributeRuntimeRequest(next),{epoch:2,source:'request-document',documentKind:'host-document',frameUrl:null});
  assert.deepEqual(attributeRuntimeRequest(request({url:'http://127.0.0.1/runtime/th08/app.mjs?runtimeEpoch=4',navigation:true,
    current:'http://127.0.0.1/host/th08?epoch=3'})),
  {epoch:4,source:'request-document',documentKind:'runtime-document',frameUrl:null});
  const childRequest=request({current:'about:blank',parent:frame('http://127.0.0.1/host/th08?epoch=3')});
  assert.equal(attributeRuntimeRequest(childRequest).epoch,3,'ancestor host epoch is used when the child URL has no epoch');

  const evidence=createNavigationAbortEvidence();
  evidence.retirePage({mainFrame:()=>frame('http://127.0.0.1/host/th08?epoch=1')});
  const nextNavigation=request({url:'http://127.0.0.1/host/th08?epoch=2',navigation:true,current:'http://127.0.0.1/host/th08?epoch=1'});
  evidence.captureRequest(nextNavigation);
  assert.equal(evidence.requestFailure(nextNavigation).attribution.epoch,2);
  assert.equal(evidence.requestFailure(nextNavigation).expectedNavigationAbort,false,'new epoch navigation cannot inherit retirement from old frame');
});

test('HTTP 404 and 5xx remain failures regardless of navigation retirement',()=>{
  assert.equal(shouldRecordHttpFailure(404),true);
  assert.equal(shouldRecordHttpFailure(503),true);
  assert.equal(shouldRecordHttpFailure(200),false);
});

test('closing the final instance cannot retire its pending subresource requests',()=>{
  const evidence=createNavigationAbortEvidence();
  for(const epoch of [1,2,3])evidence.retirePage({mainFrame:()=>frame(`https://127.0.0.1/host/th07?epoch=${epoch}`)});
  const pending=request({url:'https://127.0.0.1/shared/msgothic.ttc',current:'https://127.0.0.1/runtime/th07/th07.html?runtimeEpoch=4'});
  evidence.captureRequest(pending);
  assert.equal(evidence.requestFailure(pending).attribution.epoch,4);
  assert.equal(evidence.requestFailure(pending).expectedNavigationAbort,false);
});

function completedFont(){
  const url='https://127.0.0.1:8140/shared/msgothic.ttc',identity={bytes:120,sha256:'a'.repeat(64)},
    attribution={epoch:1,documentKind:'runtime-document'};
  return{failure:{url,failure:'net::ERR_ABORTED',isNavigationRequest:false,attribution},
    proof:{requests:[{url,attribution}],responses:[{url,status:200,requestId:'1.7',contentIdentity:'passed',
      expected:identity,actual:{...identity},transport:{event:'loadingFailed',error:'net::ERR_ABORTED',canceled:true}}],
    consumers:[{url,path:'/msgothic.ttc',epoch:1,stage:'installed-after-configure',...identity}]}};
}

test('completed aborts require independent full response and installed Runtime font bytes',()=>{
  const {failure,proof}=completedFont();
  const result=completedSharedResourceAbort(failure,proof);
  assert.equal(result.classification,'complete-response-and-runtime-consumption');
  assert.equal(result.responseRequestId,'1.7');
  assert.deepEqual(result.consumer,proof.consumers[0]);
});

test('incomplete, ambiguous, foreign or unsuccessful resource requests remain errors',()=>{
  const corruptions={
    'no consumer':({proof})=>{proof.consumers=[];},
    'short consumer':({proof})=>{proof.consumers[0].bytes--;},
    'altered consumer':({proof})=>{proof.consumers[0].sha256='b'.repeat(64);},
    'short response':({proof})=>{proof.responses[0].actual.bytes--;},
    'altered response':({proof})=>{proof.responses[0].actual.sha256='b'.repeat(64);},
    'wrong epoch':({proof})=>{proof.consumers[0].epoch=2;},
    'zero epoch':({failure})=>{failure.attribution.epoch=0;},
    'duplicate request':({proof})=>{proof.requests.push(structuredClone(proof.requests[0]));},
    'duplicate response':({proof})=>{proof.responses.push(structuredClone(proof.responses[0]));},
    'HTTP error':({proof})=>{proof.responses[0].status=404;},
    'connection reset':({failure})=>{failure.failure='net::ERR_CONNECTION_RESET';},
    'unknown owner':({failure})=>{delete failure.attribution;},
    'different destination':({proof})=>{proof.consumers[0].path='/elsewhere';},
    'unverified response':({proof})=>{proof.responses[0].contentIdentity='failed';},
  };
  for(const [label,corrupt] of Object.entries(corruptions)){
    const sample=completedFont();corrupt(sample);
    assert.equal(completedSharedResourceAbort(sample.failure,sample.proof),null,label);
  }
});
