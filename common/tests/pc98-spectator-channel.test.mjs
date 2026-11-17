import test from 'node:test';
import assert from 'node:assert/strict';
import {createPc98SpectatorChannel} from '../browser/pc98-spectator-channel.mjs';

test('a completed history accepts the relay bootstrap marker',async()=>{
  let socket,error;
  class MockSocket{constructor(){socket=this;} close(){} }
  const channel=createPc98SpectatorChannel({netplayUrl:'ws://localhost/?room=test&run=1',netplaySpectatorId:'viewer',netplayPlayerCount:2},e=>{error=e;},MockSocket);
  const packet=(kind,body='')=>{const content=new TextEncoder().encode(body),bytes=new Uint8Array(16+content.length);bytes.set([0x50,0x39,0x38,0x53,1,kind,2,0]);bytes.set(content,16);return bytes.buffer;};
  socket.onmessage({data:packet(1,JSON.stringify({game:'01'}))});
  socket.onmessage({data:packet(3)});
  socket.onmessage({data:packet(5)});
  await new Promise(resolve=>setTimeout(resolve,20));
  assert.equal(error,undefined);assert.equal(channel.ready,true);channel.close();
});
