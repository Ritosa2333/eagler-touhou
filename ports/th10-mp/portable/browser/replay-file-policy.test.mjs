import test from 'node:test';
import assert from 'node:assert/strict';
import {createReplayFilePolicy} from './replay-file-policy.mjs';

const bytes=new TextEncoder().encode('EAGLRPY1-test');
test('multiplayer imports validate actual contents before mapping the physical name',()=>{
 let calls=0;
 const policy=createReplayFilePolicy({game:10,multiplayer:true,validateMultiplayer:value=>{calls++;return value===bytes;}});
 assert.equal(policy.imported('replay/th10_01.rpyx',bytes),'replay/th10_01.rpy');
 assert.equal(policy.imported('replay/th10_01.rpy',bytes),'replay/th10_01.rpy');
 assert.equal(calls,2);
 assert.throws(()=>policy.imported('replay/th10_01.rpyx',bytes.slice()),/多人录像/);
 assert.throws(()=>policy.imported('replay/th10_01.rpy',new Uint8Array(8)),/多人录像/);
 assert.equal(policy.imported('th10.cfg',new Uint8Array()),'th10.cfg');
 assert.equal(policy.exported('replay/th10_01.rpy',bytes),'replay/th10_01.rpyx');
 assert.equal(policy.exported('replay/th10_01.rpy',new Uint8Array(0)),'replay/th10_01.rpyx');
 assert.equal(policy.physical('replay/th10_01.rpyx'),'replay/th10_01.rpy');
});
test('ordinary import cannot accidentally feed an all-seat Replay to the retail reader',()=>{
 const policy=createReplayFilePolicy({game:10});
 for(const extension of ['rpy','rpyx'])assert.throws(()=>policy.imported('replay/th10_01.'+extension,bytes),/多人录像/);
 const retail=new Uint8Array([0x74,0x31,0x30,0x72]);
 assert.equal(policy.imported('replay/th10_01.rpy',retail),'replay/th10_01.rpy');
 assert.equal(policy.exported('replay/th10_01.rpy',retail),'replay/th10_01.rpy');
 assert.throws(()=>policy.imported('replay/th10_01.rpyx',retail),/移动记录/);
 assert.equal(policy.exported('replay/th10_01.rpy',bytes),'replay/th10_01.rpyx');
});
test('a multiplayer policy cannot silently omit its native validation gate',()=>{
 assert.throws(()=>createReplayFilePolicy({game:10,multiplayer:true}),TypeError);
 assert.throws(()=>createReplayFilePolicy({game:0}),TypeError);
});
