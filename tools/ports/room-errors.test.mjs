import test from 'node:test';
import assert from 'node:assert/strict';
import {classifyRoomConsoleErrors,roomProtocolErrorEntries,stampRoomProtocolEvent} from './room-errors.mjs';

const message = 'Error: player 2 left the run';
const native = {failed:true,error:message};
const entry = overrides => ({seat:0,checkerMs:101,stage:'peer-departure',text:message,native,...overrides});
const disconnect = overrides => ({closedSeats:[{seat:1,checkerMs:100,reason:'peer-close'}],...overrides});

function protocolErrorEntry(seat,text,receivedAt){
  const snapshots=Array.from({length:seat+1},()=>({events:[]}));
  snapshots[seat].events.push(stampRoomProtocolEvent({event:'error',error:text},receivedAt,
    {failed:true,error:text,transportState:{disconnected:true}}));
  return roomProtocolErrorEntries(snapshots,'peer-departure')[0];
}

test('classifies a matching post-close error for the deliberately closed peer',()=>{
  const observed=entry({text:message+'\n    at network handler',native:{...native,error:message+'\n    at native failure'}});
  assert.deepEqual(classifyRoomConsoleErrors({entries:[observed],disconnect:disconnect()}),
    {expected:[observed],unexpected:[]});
});

test('protocol error conversion preserves page-receipt time instead of collection time',()=>{
  const closed=disconnect();
  const before=stampRoomProtocolEvent({event:'error',error:message,receivedAt:101},99,{failed:true,error:message});
  const after=stampRoomProtocolEvent({event:'error',error:message},101,{failed:true,error:message});
  const snapshots=[{events:[before,after],failed:false,error:''}];
  const entries=roomProtocolErrorEntries(snapshots,'peer-departure');
  assert.deepEqual(entries.map(item=>item.checkerMs),[99,101]);
  assert.equal(entries[0].sampledNative.failed,false);
  assert.equal(entries[0].native.failed,true);
  assert.deepEqual(classifyRoomConsoleErrors({entries,disconnect:closed}),
    {expected:[entries[1]],unexpected:[entries[0]]});
});

test('unstamped protocol errors stay unexpected after the close',()=>{
  const entries=roomProtocolErrorEntries([{events:[{event:'error',error:message,nativeAtReceipt:null}],failed:true,error:message}],
    'peer-departure');
  assert.equal(entries[0].checkerMs,undefined);
  assert.deepEqual(classifyRoomConsoleErrors({entries,disconnect:disconnect()}),
    {expected:[],unexpected:entries});
});

test('accepts the exact RTC input/control close and error messages for their closed seat',()=>{
  for (const [text,nativeState] of [
    ['Error: RTC peer P2 input channel closed',{transportState:{disconnected:true}}],
    ['Error: RTC peer P2 control channel error',{failed:true}]
  ]) {
    const observed=entry({text,native:{...nativeState,error:text}});
    assert.deepEqual(classifyRoomConsoleErrors({entries:[observed],disconnect:disconnect()}),
      {expected:[observed],unexpected:[]});
  }
});

test('accepts the exact raw Runtime protocol messages after their matching client close',()=>{
  const entries=[
    protocolErrorEntry(0,'player 2 left the run',102),
    protocolErrorEntry(0,'player 3 left the run',161),
    protocolErrorEntry(1,'player 3 left the run',162),
    protocolErrorEntry(1,'RTC peer P1 control channel error',344)
  ];
  const result=classifyRoomConsoleErrors({entries,disconnect:{closedSeats:[
    {seat:1,checkerMs:100,reason:'peer-close'},
    {seat:2,checkerMs:100,reason:'peer-close'},
    {seat:0,checkerMs:200,requestedCheckerMs:190,reason:'return-to-room'}
  ],returns:[{seat:0,returnControlClicked:true,returnedToRoom:true}]}});
  assert.deepEqual(result,{expected:entries,unexpected:[]});
});

test('bare transport text does not pass from Console or an unconfirmed room return',()=>{
  const bareConsole=entry({kind:'console-error',text:message.slice('Error: '.length),
    native:{failed:true,error:message.slice('Error: '.length)}});
  const returnProtocol=protocolErrorEntry(0,'RTC peer P2 control channel error',101);
  const disconnectRecord={closedSeats:[{seat:1,checkerMs:100,requestedCheckerMs:90,reason:'return-to-room'}],returns:[]};
  assert.deepEqual(classifyRoomConsoleErrors({entries:[bareConsole,returnProtocol],disconnect:disconnectRecord}),
    {expected:[],unexpected:[bareConsole,returnProtocol]});
});

test('return-to-room errors require the actual button click to precede the error',()=>{
  const observed=protocolErrorEntry(0,'RTC peer P2 control channel error',101);
  const returns=[{seat:1,returnControlClicked:true,returnedToRoom:true}];
  const requestedOnly={closedSeats:[{seat:1,checkerMs:null,requestedCheckerMs:100,reason:'return-to-room'}],returns};
  const clickAfterError={closedSeats:[{seat:1,checkerMs:102,requestedCheckerMs:100,reason:'return-to-room'}],returns};
  assert.deepEqual(classifyRoomConsoleErrors({entries:[observed],disconnect:requestedOnly}),
    {expected:[],unexpected:[observed]});
  assert.deepEqual(classifyRoomConsoleErrors({entries:[observed],disconnect:clickAfterError}),
    {expected:[],unexpected:[observed]});
  const completed={closedSeats:[{seat:1,checkerMs:100,requestedCheckerMs:90,reason:'return-to-room'}],returns};
  assert.deepEqual(classifyRoomConsoleErrors({entries:[observed],disconnect:completed}),
    {expected:[observed],unexpected:[]});
});

test('keeps errors before the close, for another seat, or with mismatched native text unexpected',()=>{
  const cases=[
    entry({checkerMs:99}),
    entry({checkerMs:100}),
    entry({text:'Error: player 3 left the run',native:{failed:true,error:'Error: player 3 left the run'}}),
    entry({text:'Error: player 2 left the run',native:{failed:true,error:'Error: player 1 left the run'}})
  ];
  const result=classifyRoomConsoleErrors({entries:cases,disconnect:disconnect()});
  assert.deepEqual(result,{expected:[],unexpected:cases});
});

test('unknown fatal errors and ordinary errors stay unexpected',()=>{
  const cases=[
    entry({text:'Error: fatal runtime failure',native:{failed:true,error:'Error: fatal runtime failure'}}),
    entry({text:'Error: TypeError: undefined is not a function',native:{failed:true,error:'Error: TypeError: undefined is not a function'}}),
    entry({text:'Error: player 2 left the run',native:{failed:false,error:'Error: player 2 left the run'}})
  ];
  assert.deepEqual(classifyRoomConsoleErrors({entries:cases,disconnect:disconnect()}),
    {expected:[],unexpected:cases});
});

test('missing close or observation times and missing native evidence never pass',()=>{
  const cases=[
    entry({checkerMs:undefined}),
    entry({native:undefined}),
    entry({native:{failed:true,error:message}})
  ];
  const result=classifyRoomConsoleErrors({entries:cases,disconnect:{closedSeats:[{seat:1}]}});
  assert.deepEqual(result,{expected:[],unexpected:cases});
  const missingCloseTime=entry();
  assert.deepEqual(classifyRoomConsoleErrors({entries:[missingCloseTime],disconnect:{closedSeats:[{seat:1,checkerMs:undefined}]}}),
    {expected:[],unexpected:[missingCloseTime]});
  assert.deepEqual(classifyRoomConsoleErrors({entries:[missingCloseTime],disconnect:{}}),
    {expected:[],unexpected:[missingCloseTime]});
});

test('an error observed on the closed page itself is not treated as peer departure evidence',()=>{
  const closedPageError=entry({seat:1,text:'Error: player 2 left the run',native:{failed:true,error:'Error: player 2 left the run'}});
  assert.deepEqual(classifyRoomConsoleErrors({entries:[closedPageError],disconnect:disconnect()}),
    {expected:[],unexpected:[closedPageError]});
});

test('invalid entry collections are rejected as caller errors',()=>{
  assert.throws(()=>classifyRoomConsoleErrors({entries:null}),/entries must be an array/);
});
