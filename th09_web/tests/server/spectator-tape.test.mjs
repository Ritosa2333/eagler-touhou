import test from 'node:test';
import assert from 'node:assert/strict';
import {SpectatorTape} from '../../sdl-runtime/spectator-tape.mjs';
test('receive history preserves multiple complete matches in exact owned packet order',()=>{
  const tape=new SpectatorTape();tape.input(0);tape.input(1);tape.end(2);
  tape.begin(100);tape.input(100);tape.end(101);tape.begin(200);tape.input(200);
  const begin=new Uint8Array(74);begin[8]=100;
  tape.append(begin);begin[8]=255;tape.append(new Uint8Array(46));tape.append(new Uint8Array(24));
  const second=new Uint8Array(74);second[8]=200;tape.append(second);
  assert.equal(tape.peek()[8],100);assert.equal(tape.begins,2);
  tape.shift();assert.equal(tape.begins,1);assert.equal(tape.peek().length,46);
  tape.shift();tape.shift();assert.equal(tape.peek()[8],200);
  tape.shift();assert.equal(tape.bytes,0);assert.equal(tape.begins,0);assert.equal(tape.peek(),undefined);
  assert.throws(()=>tape.begin(201),/缺少上一局结束/);
  assert.throws(()=>tape.end(202),/结束帧无效/);assert.throws(()=>tape.input(202),/不连续/);
});
test('backlog has a fixed count/byte bound without silently evicting old input',()=>{
  const tape=new SpectatorTape(),frame=new Uint8Array(46);frame[8]=77;
  for(let i=0;i<32404;i++)tape.append(frame);
  const bytes=tape.bytes;assert.throws(()=>tape.append(frame),/积压超限/);
  assert.equal(tape.bytes,bytes);assert.equal(tape.peek()[8],77);
  for(let i=0;i<32404;i++)tape.shift();assert.equal(tape.bytes,0);
});
test('round annotations preserve the input cursor and reset only on a new match',()=>{
  const tape=new SpectatorTape();tape.input(0);tape.roundStart(1,1);
  assert.equal(tape.frame,1);assert.throws(()=>tape.roundStart(1,1),/刻度无效/);
  assert.throws(()=>tape.roundStart(2,2),/刻度无效/);tape.input(1);tape.roundStart(2,2);tape.end(2);
  assert.throws(()=>tape.roundStart(2,3),/刻度无效/);tape.begin(100);tape.input(100);tape.roundStart(101,1);
  assert.equal(tape.round,1);assert.equal(tape.frame,101);
});
