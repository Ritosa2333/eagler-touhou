import {test} from 'node:test';
import assert from 'node:assert/strict';
import {OriginalSfx} from '../pc98/original-sfx.mjs';

test('TH02 shot sounds are spaced while other effects remain immediate',()=>{
  const context={currentTime:0},events=[];
  const sfx=new OriginalSfx(context,{port:{}},{postMessage:event=>events.push(event.id)},'th02');
  sfx.command(0x0c01);
  context.currentTime=0.03;sfx.play(1);sfx.play(2);
  context.currentTime=0.121;sfx.play(1);
  assert.deepEqual(events,[1,2,1]);
  assert.equal(sfx.played,3);
});

test('other games retain their original effect frequency',()=>{
  const events=[],sfx=new OriginalSfx({currentTime:0},{port:{}},{postMessage:event=>events.push(event.id)},'th04');
  sfx.play(1);sfx.play(1);
  assert.deepEqual(events,[1,1]);
});
