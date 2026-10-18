import test from 'node:test';
import assert from 'node:assert/strict';
import {expectedDisconnectConsole} from '../../ports/th11-mp/portable/multiplayer/acceptance-errors.mjs';

test('only the observed, deliberately closed peer notification is expected',()=>{
  const entry={seat:0,checkerMs:101,kind:'console-error',error:'Error: RTC peer P2 input channel closed\n at native bridge'};
  const disconnect={observed:true,requestedCheckerMs:100,after:[{disconnected:true,error:'RTC peer P2 input channel closed'}]};
  assert.equal(expectedDisconnectConsole(entry,disconnect),true);
  for(const change of [{checkerMs:99},{kind:'pageerror'},{seat:1},{error:'Error: unrelated failure'}])
    assert.equal(expectedDisconnectConsole({...entry,...change},disconnect),false);
  assert.equal(expectedDisconnectConsole(entry,{...disconnect,observed:false}),false);
  assert.equal(expectedDisconnectConsole(entry,{...disconnect,after:[{disconnected:false,error:'RTC peer P2 input channel closed'}]}),false);
});
