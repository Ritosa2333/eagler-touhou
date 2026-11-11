import test from 'node:test';
import assert from 'node:assert/strict';
import {encodeReplay,decodeReplay} from '../web/native-replay.mjs';
import {packTouch} from '../web/touch-input.js';

test('native replay retains both players and touch input and rejects corruption',async()=>{
 const rows=[[0,512],[packTouch(64,{active:true,begin:true,x:21,y:-16}),32]];
 const bytes=await encodeReplay(rows,true,Uint8Array.of(1,2,3));
 const parsed=await decodeReplay(bytes);assert.deepEqual(parsed.rows,rows);assert.deepEqual(parsed.native,[1,2,3]);
 const envelope=JSON.parse(new TextDecoder().decode(bytes));envelope.body[10]^=1;
 await assert.rejects(decodeReplay(new TextEncoder().encode(JSON.stringify(envelope))),/校验/);
 await assert.rejects(decodeReplay(new TextEncoder().encode('{}')),/格式/);
});

test('solo replay validates input and initial files while preserving legacy multiplayer',async()=>{
 const rows=[[32,32,0,packTouch(0,{active:true,x:-10,y:4}),1],[0,0,0,0,0]];
 const files={'YUME.CFG':[1,2,3]},bytes=await encodeReplay(rows,true,Uint8Array.of(5),{mode:'solo',files});
 const replay=await decodeReplay(bytes);assert.equal(replay.mode,'solo');assert.deepEqual(replay.rows,rows);assert.deepEqual(replay.files,files);
 const invalid=await encodeReplay([[64,0,0,0,1]],true,Uint8Array.of(5),{mode:'solo',files});await assert.rejects(decodeReplay(invalid),/无效/);
 const badFile=await encodeReplay(rows,true,Uint8Array.of(5),{mode:'solo',files:{'../config':[1]}});await assert.rejects(decodeReplay(badFile),/无效/);
});
