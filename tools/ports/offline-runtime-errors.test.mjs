import assert from 'node:assert/strict';
import test from 'node:test';
import {classifyOfflineBrowserEvents,matchExpectedOfflinePathTraversal} from './offline-runtime-errors.mjs';

const origin='https://127.0.0.1:49152';

test('offline console errors and uncaught page errors fail the scenario',()=>{
  const consoleError={type:'offline-console',text:'Runtime storage failed'};
  const pageError={type:'offline-pageerror',text:'Error: Runtime startup failed'};
  const result=classifyOfflineBrowserEvents([consoleError,pageError],{origin});
  assert.deepEqual(result.blockingEvents,[consoleError,pageError]);
  assert.deepEqual(result.sameOriginTransportEvents,[]);
  assert.deepEqual(result.crossOriginTransportEvents,[]);
  assert.strictEqual(result.blockingEvents[0],consoleError,'Keep the original event payload');
});

test('same-origin HTTP failures fail regardless of path or file suffix',()=>{
  const event={type:'offline-http',url:`${origin}/shared/renderer-resource`,status:404};
  const result=classifyOfflineBrowserEvents([event],{origin});
  assert.deepEqual(result.blockingEvents,[event]);
  assert.deepEqual(result.sameOriginTransportEvents,[event]);
  assert.deepEqual(result.crossOriginTransportEvents,[]);
});

test('same-origin request failures fail regardless of path or file suffix',()=>{
  const event={type:'offline-requestfailed',url:`${origin}/generated/chunk`,error:'net::ERR_FAILED'};
  const result=classifyOfflineBrowserEvents([event],{origin});
  assert.deepEqual(result.blockingEvents,[event]);
  assert.deepEqual(result.sameOriginTransportEvents,[event]);
  assert.deepEqual(result.crossOriginTransportEvents,[]);
});

test('a failed external font request blocks offline acceptance and retains the event',()=>{
  const event={type:'offline-requestfailed',url:'https://fonts.gstatic.com/s/site/font.woff2',error:'net::ERR_INTERNET_DISCONNECTED'};
  const result=classifyOfflineBrowserEvents([event],{origin});
  assert.deepEqual(result.blockingEvents,[event]);
  assert.deepEqual(result.sameOriginTransportEvents,[]);
  assert.deepEqual(result.crossOriginTransportEvents,[event]);
  assert.strictEqual(result.crossOriginTransportEvents[0],event,'Keep the original event payload');
});

test('unclassifiable requests and new event types fail closed',()=>{
  const malformed={type:'offline-requestfailed',url:'not a URL',error:'failed'};
  const unknown={type:'offline-network-policy',text:'new recorder event'};
  const result=classifyOfflineBrowserEvents([malformed,unknown],{origin});
  assert.deepEqual(result.blockingEvents,[malformed,unknown]);
  assert.deepEqual(result.sameOriginTransportEvents,[]);
  assert.deepEqual(result.crossOriginTransportEvents,[]);
});

test('only the exact request-bound read/write/remove traversal responses are expected negatives',()=>{
  const expectation={requestId:'offline-runtime-unique-1',game:'th08',epoch:7,command:'read',
    path:'../outside-offline-acceptance.dat'};
  const event={protocol:'eagler-touhou/1',game:'th08',epoch:7,request:expectation.requestId,ok:false,
    error:'Error: Invalid save path: ../outside-offline-acceptance.dat'};
  assert.deepEqual(matchExpectedOfflinePathTraversal(event,expectation),{
    kind:'expected-storage-path-traversal-rejection',requestId:expectation.requestId,game:'th08',epoch:7,
    command:'read',path:'../outside-offline-acceptance.dat',outcome:'rejected',error:event.error,
  });
  assert.ok(matchExpectedOfflinePathTraversal({...event,error:'Error: Invalid save path'},expectation),
    'Retain the exact generic path-guard diagnostic used by runtimes which do not echo the rejected path');
  for(const command of ['read','write','remove'])
    assert.ok(matchExpectedOfflinePathTraversal(event,{...expectation,command}),`${command} must be supported`);
});

test('a changed request, identity, outcome, path or rejection text remains a protocol error',()=>{
  const expectation={requestId:'offline-runtime-unique-2',game:'th08',epoch:3,command:'remove',
    path:'../outside-offline-acceptance.dat'};
  const event={protocol:'eagler-touhou/1',game:'th08',epoch:3,request:expectation.requestId,ok:false,
    error:'Error: Invalid save path: ../outside-offline-acceptance.dat'};
  for(const changedEvent of [
    {...event,request:'offline-runtime-other'},
    {...event,game:'th09'},
    {...event,epoch:4},
    {...event,ok:true},
    {...event,error:'Error: not found'},
    {...event,protocol:'other'},
  ])assert.equal(matchExpectedOfflinePathTraversal(changedEvent,expectation),null);
  for(const changedExpectation of [
    {...expectation,command:'sync'},
    {...expectation,path:'../other.dat'},
    {...expectation,requestId:''},
    {...expectation,epoch:0},
  ])assert.equal(matchExpectedOfflinePathTraversal(event,changedExpectation),null);
});
