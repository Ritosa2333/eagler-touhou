import assert from 'node:assert/strict';
import test from 'node:test';
import {assessRestartKeyboardForwarding,focusRestartKeyboardHost} from './runtime-touch-restart-keyboard.mjs';

test('restart input corrects iframe focus before the host captures accepted physical key edges',async()=>{
  let active='IFRAME',broughtToFront=0;
  const hostEvents=[];
  const dispatchPhysicalKey=(code,down)=>{
    if(active==='BODY')hostEvents.push({code,down,accepted:true,epoch:1});
  };
  dispatchPhysicalKey('KeyR',true);dispatchPhysicalKey('KeyR',false);
  assert.equal(assessRestartKeyboardForwarding({events:hostEvents,epoch:1,expectedCodes:['KeyR']}).status,'failed',
    'an iframe-focused raw key cannot stand in for the host keyboard-forward receipt');
  const result=await focusRestartKeyboardHost({
    bringToFront:async()=>{broughtToFront++;},
    focusHostBody:async()=>{active='BODY';},
    readFocus:async()=>({documentHasFocus:true,visibilityState:'visible',hidden:false,
      activeElementTag:active,activeElementIsBody:active==='BODY',activeElementIsRuntimeFrame:active==='IFRAME'}),
  });
  assert.equal(broughtToFront,1);
  assert.equal(result.status,'ready');
  assert.equal(result.before.activeElementIsRuntimeFrame,true);
  assert.equal(result.after.activeElementIsBody,true);
  dispatchPhysicalKey('KeyR',true);dispatchPhysicalKey('KeyR',false);
  const delivery=assessRestartKeyboardForwarding({events:hostEvents,epoch:1,expectedCodes:['KeyR']});
  assert.equal(delivery.status,'passed');
  assert.deepEqual(delivery.actual,[{code:'KeyR',down:true},{code:'KeyR',down:false}]);
});

test('restart input remains incomplete when the host cannot take focus from the Runtime iframe',async()=>{
  const result=await focusRestartKeyboardHost({bringToFront:async()=>{},focusHostBody:async()=>{},
    readFocus:async()=>({documentHasFocus:true,visibilityState:'visible',hidden:false,
      activeElementTag:'IFRAME',activeElementIsBody:false,activeElementIsRuntimeFrame:true})});
  assert.equal(result.status,'incomplete');
  assert.ok(result.missing.includes('Runtime iframe is not the active host element'));
});

test('restart proof accepts only exact host-forwarded down/up events accepted in the selected epoch',()=>{
  const events=[
    {code:'KeyR',down:true,accepted:true,epoch:1},
    {code:'KeyR',down:false,accepted:true,epoch:1},
    {code:'KeyR',down:true,accepted:true,epoch:2},
  ];
  const result=assessRestartKeyboardForwarding({events,epoch:1,expectedCodes:['KeyR']});
  assert.equal(result.status,'passed');
  assert.equal(result.events.length,2);
});

test('iframe-only raw keys, missing host receipt, wrong ordering or bridge rejection cannot pass restart proof',()=>{
  const noHostReceipt=assessRestartKeyboardForwarding({events:[],epoch:1,expectedCodes:['KeyR']});
  assert.equal(noHostReceipt.status,'failed');
  assert.match(noHostReceipt.reason,/observed/);
  const wrongEpoch=assessRestartKeyboardForwarding({events:[
    {code:'KeyR',down:true,accepted:true,epoch:2},{code:'KeyR',down:false,accepted:true,epoch:2},
  ],epoch:1,expectedCodes:['KeyR']});
  assert.equal(wrongEpoch.status,'failed');
  const reversed=assessRestartKeyboardForwarding({events:[
    {code:'KeyR',down:false,accepted:true,epoch:1},{code:'KeyR',down:true,accepted:true,epoch:1},
  ],epoch:1,expectedCodes:['KeyR']});
  assert.equal(reversed.status,'failed');
  const rejected=assessRestartKeyboardForwarding({events:[
    {code:'KeyR',down:true,accepted:false,epoch:1},{code:'KeyR',down:false,accepted:true,epoch:1},
  ],epoch:1,expectedCodes:['KeyR']});
  assert.equal(rejected.status,'failed');
});
