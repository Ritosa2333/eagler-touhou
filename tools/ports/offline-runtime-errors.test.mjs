import assert from 'node:assert/strict';
import test from 'node:test';
import {classifyOfflineBrowserEvents} from './offline-runtime-errors.mjs';

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
