import {test} from 'node:test';
import assert from 'node:assert/strict';

for(const game of ['01','02','03','04'])test(`TH${game} publishes only corrected confirmed history`,async()=>{
  const {ConfirmedTape,VERSION}=await import(`../../ports/th${game}/web/replay/tape.js`);
  const header={version:VERSION,game,adapter:'confirmed-worker/1',players:2,hostSlot:0,tickMs:1000/60,settings:{},identity:'a'.repeat(64),opening:{}};
  const input=game==='03'?{actions:['shot'],touch:0,focusEnabled:false,points:false,commands:[]}:32;
  const row=(frame,proof='ok')=>({inputs:[input,input],offlineMask:0,proof,hash:(frame+1)%120===0?{hash:'12345678'}:null});
  const tape=new ConfirmedTape(header);
  for(let frame=0;frame<120;frame++)tape.stage(frame,row(frame));
  assert.equal(tape.confirm(120,true),false);assert.equal(tape.length,0);
  tape.rewind(50);
  for(let frame=50;frame<120;frame++)tape.stage(frame,row(frame,'corrected'));
  assert.equal(tape.confirm(120),true);
  assert.equal(tape.length,120);assert.deepEqual(tape.row(49),row(49));assert.deepEqual(tape.row(50),row(50,'corrected'));
  assert.equal(tape.confirm(100),false);assert.equal(tape.length,120);assert.deepEqual(tape.row(50),row(50,'corrected'));
  assert.deepEqual(tape.slice(118,120),[row(118,'corrected'),row(119,'corrected')]);
  assert.throws(()=>tape.rewind(119));
  tape.finish('end');assert.deepEqual(tape.snapshot().rows,tape.slice());
});
