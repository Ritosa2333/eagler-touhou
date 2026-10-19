import test from 'node:test';
import assert from 'node:assert/strict';
import {attributeRuntimeRequest,createNavigationAbortEvidence,shouldRecordHttpFailure} from './navigation-abort-evidence.mjs';

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
