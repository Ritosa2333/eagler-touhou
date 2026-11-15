import test from 'node:test';
import assert from 'node:assert/strict';
import {Th19SpectatorHistory} from '../src/th19-spectator.mjs';
import {th19StreamBegin,th19StreamFrame,th19StreamEnd} from '../../../common/contracts/th19-recording.mjs';
const config={rules:[0,1,1,4,4,1,1,0,0],protocol:1,build:'a'.repeat(64),seed:1,difficulty:1,characters:[0,1],delay:0,rollback:true},input={keys:0,direct:0,x:0,y:0};
test('TH19 history owns bounded sequential packets and closes after the final frame',()=>{
 const tape=new Th19SpectatorHistory(),begin=th19StreamBegin(config);
 tape.accept(begin);assert.equal(tape.accept(begin),null);begin[0]=0;assert.equal(tape.history[0][0],84);
 tape.accept(th19StreamFrame(0,input,input));tape.accept(th19StreamEnd(1));assert.equal(tape.ended,true);assert.equal(tape.history.length,3);
 assert.throws(()=>tape.accept(th19StreamFrame(1,input,input)));
});
test('TH19 history rejects gaps and input before a valid initializer',()=>{
 const tape=new Th19SpectatorHistory();assert.throws(()=>tape.accept(th19StreamFrame(0,input,input)));
 tape.accept(th19StreamBegin(config));assert.throws(()=>tape.accept(th19StreamFrame(1,input,input)));
});
