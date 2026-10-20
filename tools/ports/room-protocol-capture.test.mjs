import test from 'node:test';
import assert from 'node:assert/strict';
import {runInNewContext} from 'node:vm';
import {installRoomProtocolCapture} from './room-protocol-capture.mjs';
import {stampRoomProtocolEvent,roomProtocolErrorEntries} from './room-errors.mjs';

test('the actual host observer captures aborts at receipt and ignores foreign messages',async()=>{
  const callbacks={},captured=[],runtime={__eaglerNetplayFailed:true,__eaglerNetplayError:'late abort'};
  const window={__captureRoomProtocolFailure:async event=>captured.push(event)};window.top=window;
  const context={window,location:{origin:'https://local.test'},document:{querySelector:()=>({contentWindow:runtime})},
    addEventListener:(type,callback)=>{callbacks[type]=callback;}};
  runInNewContext(`(${installRoomProtocolCapture.toString()})({game:'th06',stamp:(${stampRoomProtocolEvent.toString()})})`,context);
  const data={protocol:'eagler-touhou/1',game:'th06',event:'abort',error:'late abort'};
  for(const event of [{origin:'https://foreign.test',source:runtime,data},{origin:'https://local.test',source:{},data},
    {origin:'https://local.test',source:runtime,data:{...data,game:'th07'}}])callbacks.message(event);
  assert.equal(captured.length,0);
  callbacks.message({origin:'https://local.test',source:runtime,data});
  assert.equal(await window.__flushRoomProtocolCaptures(),0);
  assert.equal(captured.length,1);assert.equal(captured[0].event,'abort');
  assert.equal(captured[0].nativeAtReceipt.failed,true);
  assert.ok(Number.isFinite(captured[0].receivedAt));
  assert.equal(roomProtocolErrorEntries([{events:captured}],'teardown').length,1);
});

test('a failed protocol binding cannot drain as successful evidence',async()=>{
  const callbacks={},runtime={},window={__captureRoomProtocolFailure:()=>Promise.reject(Error('binding lost'))};window.top=window;
  runInNewContext(`(${installRoomProtocolCapture.toString()})({game:'th06',stamp:(${stampRoomProtocolEvent.toString()})})`,
    {window,location:{origin:'https://local.test'},document:{querySelector:()=>({contentWindow:runtime})},
      addEventListener:(type,callback)=>{callbacks[type]=callback;}});
  callbacks.message({origin:'https://local.test',source:runtime,data:{protocol:'eagler-touhou/1',game:'th06',event:'error'}});
  await assert.rejects(window.__flushRoomProtocolCaptures(),/binding lost/);
});
