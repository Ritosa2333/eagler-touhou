import assert from 'node:assert/strict';
import {ConfirmedTape,encodeBattleMatch,decodeTape,VERSION} from '../web/replay/tape.js';
const header={version:VERSION,game:'03',adapter:'confirmed-worker/1',players:2,hostSlot:0,tickMs:1000/60,settings:{},identity:'a'.repeat(64),opening:{}};
const input={actions:[],touch:0,focusEnabled:true,points:false,commands:[]};
const tape=new ConfirmedTape(header);
for(let frame=0;frame<1200;frame++){
  tape.stage(frame,{inputs:[input,input],offlineMask:0,proof:'x'.repeat(128),hash:(frame+1)%120===0?'hash':null,scene:1});
  tape.confirm(frame+1);
}
const pending=encodeBattleMatch(tape,500,1000);
for(let frame=1200;frame<1400;frame++){
  tape.stage(frame,{inputs:[input,input],offlineMask:0,proof:'y',hash:(frame+1)%120===0?'hash':null,scene:1});
  tape.confirm(frame+1);
}
const result=await decodeTape(await pending);
assert.equal(result.rows.length,1000);assert.equal(result.matchStart,500);assert.equal(result.rows[999].proof,'x'.repeat(128));assert.equal(tape.length,1400);
await assert.rejects(()=>encodeBattleMatch(tape,0,1401));console.log('TH03 streaming match export preserves confirmed prefix PASS');
