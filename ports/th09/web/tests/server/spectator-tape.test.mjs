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
  for(let i=0;i<262144;i++)tape.append(frame);
  const bytes=tape.bytes;assert.throws(()=>tape.append(frame),/积压超限/);
  assert.equal(tape.bytes,bytes);assert.equal(tape.peek()[8],77);
  for(let i=0;i<262144;i++)tape.shift();assert.equal(tape.bytes,0);
});
test('round annotations preserve the input cursor and reset only on a new match',()=>{
  const tape=new SpectatorTape();tape.input(0);tape.roundStart(1,1);
  assert.equal(tape.frame,1);assert.throws(()=>tape.roundStart(1,1),/刻度无效/);
  assert.throws(()=>tape.roundStart(2,2),/刻度无效/);tape.input(1);tape.roundStart(2,2);tape.end(2);
  assert.throws(()=>tape.roundStart(2,3),/刻度无效/);tape.begin(100);tape.input(100);tape.roundStart(101,1);
  assert.equal(tape.round,1);assert.equal(tape.frame,101);
});

const packet=(length,frame,round=0)=>{const bytes=new Uint8Array(length),data=new DataView(bytes.buffer);data.setUint32(8,frame,true);if(length===28)data.setUint32(24,round,true);return bytes;};
test('starting the next played match releases the old archive and keeps current-match seeking',()=>{
  const tape=new SpectatorTape();
  tape.append(packet(102,100));tape.append(packet(46,100));tape.append(packet(28,101,1));tape.append(packet(46,101));tape.append(packet(24,102));
  tape.append(packet(102,900));for(let n=900;n<1100;n++)tape.append(packet(46,n));
  while(tape.peek())tape.shift();
  assert.equal(tape.bytes,0);assert.equal(tape.packets.length,201,'only the current match is retained');
  const view=tape.timeline(950);assert.equal(view.min,2);assert.equal(view.position,52);assert.equal(view.edge,112);assert.equal(view.total,202);
  assert.deepEqual(view.marks.map(({position,kind})=>[position,kind]),[[2,'match']]);
  assert.equal(tape.locate(1),null,'the previous match is released');
  const target=tape.locate(3);assert.equal(target.frame,901);tape.restore(target.match);
  assert.equal(tape.peek().length,46);assert.equal(tape.current.frame,900);assert.equal(tape.begins,0);assert.equal(tape.bytes,0,'rewind is local and does not charge unread-network quota');
  assert.equal(tape.timeline(901).position,3);assert.equal(tape.timeline(901).edge,112,'live edge is independent of the native refill cursor');
  while(tape.peek())tape.shift();assert.equal(tape.bytes,0);assert.equal(tape.begins,0);
});
test('new input during rewind extends the archive exactly once and preserves its receive frontier',()=>{
  const tape=new SpectatorTape();tape.append(packet(102,30));for(let n=30;n<230;n++)tape.append(packet(46,n));
  while(tape.peek())tape.shift();tape.restore(tape.matches[0]);
  tape.append(packet(46,230));assert.equal(tape.bytes,46);assert.equal(tape.timeline(30).total,201);
  while(tape.peek())tape.shift();assert.equal(tape.bytes,0);assert.equal(tape.matches[0].length,201);assert.equal(tape.locate(201).frame,231);
  tape.restore(tape.matches[0]);assert.equal(tape.bytes,0);assert.equal(tape.packets[1][8],30);
});
test('forward seek releases unread quota while preserving skipped local history',()=>{
  const tape=new SpectatorTape();tape.append(packet(102,0));tape.append(packet(46,0));tape.append(packet(24,1));tape.append(packet(102,100));tape.append(packet(46,100));
  tape.restore(tape.matches[1]);assert.equal(tape.bytes,46);assert.equal(tape.begins,0);
  tape.restore(tape.matches[0]);assert.equal(tape.bytes,46);while(tape.peek())tape.shift();assert.equal(tape.bytes,0);
  assert.equal(tape.locate(-1),null);assert.equal(tape.locate(NaN),null);assert.equal(tape.locate(3),null);
});
