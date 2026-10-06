import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';
import createFixture from '../artifacts/message-tests/core.mjs';
const native=JSON.parse(fs.readFileSync(new URL('../artifacts/reverse/msg/wait-samples.json',import.meta.url)));
function instruction(time,op,payload=Buffer.alloc(0)){const head=Buffer.alloc(4);head.writeUInt16LE(time);head[2]=op;head[3]=payload.length;return Buffer.concat([head,payload]);}
function file(code){const header=Buffer.alloc(12);header.writeUInt32LE(1);header.writeUInt32LE(12,4);header.writeUInt32LE(256,8);return Buffer.concat([header,...code]);}
function word(n){const b=Buffer.alloc(4);b.writeInt32LE(n);return b;}
function load(c,b){const p=c._malloc(b.length);try{c.HEAPU8.set(b,p);return c._test_load(p,b.length);}finally{c._free(p);}}
const state=c=>[...new Int32Array(c.HEAPU8.buffer,c._test_state(),7)];
function encrypt(text){let key=0x77,step=7;return Buffer.from([...Buffer.from(text),0].map(c=>{const value=c^key;key=(key+step)&255;step=(step+16)&255;return value;}));}
function textActions(c){let at=c._test_text(),end=at;while(c.HEAPU8[end])end++;return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(at,end)));}
test('MSG wait, pressed confirmation and Control skipping match original 41fcc0 ticks',async()=>{
  const c=await createFixture();assert.equal(native.wholeGameOracle,false);
  for(const sample of native.cases){
    assert.equal(load(c,file([instruction(0,10,Buffer.from([sample.mode==='control-disabled'?0:1])),instruction(0,11,word(5)),instruction(1,0)])),1);assert.equal(c._test_start(0),1);
    for(const row of sample.samples){
      const held=row.tick===2?(sample.mode==='held-z'?1:['held-control','control-disabled'].includes(sample.mode)?512:0):0;
      const pressed=row.tick===2?(sample.mode==='pressed-z'?1:sample.mode==='pressed-enter'?0x80000:0):0;
      c._test_tick(held,pressed);assert.deepEqual(state(c).slice(0,4),[row.return|0,row.clock,row.wait,row.pc],`${sample.mode} tick ${row.tick}`);assert.equal(state(c)[6],0);
    }
  }
});
test('MSG formatted text keeps independent line slots, spacing, font and native line control',async()=>{
  const c=await createFixture(),samples=JSON.parse(fs.readFileSync(new URL('./fixtures/message-formatted-original.json',import.meta.url)));
  for(const sample of samples.records){const code=sample.commands.map(([op,text],i)=>instruction(i*2,op,[15,16,17].includes(op)?encrypt(text):Buffer.alloc(0)));code.push(instruction(sample.commands.length*2+10,0));assert.equal(load(c,file(code)),1);assert.equal(c._test_start(0),1);
    for(const frame of sample.frames){c._test_tick(0,0);const s=[...new Int32Array(c.HEAPU8.buffer,c._test_state(),10)],actions=textActions(c),draws=actions.map(({interrupt,textHex,...r})=>({...r,text:Buffer.from(textHex,'hex').toString()}));assert.deepEqual(draws,frame.draws,`${sample.name} tick${frame.tick} draws`);assert.deepEqual(actions.map(a=>[a.slot,a.interrupt]),frame.interrupts,`${sample.name} interrupts`);assert.deepEqual([s[1],s[7],s[8],s[9],s[4]],[frame.clock,frame.line,frame.initialized,frame.speaker,frame.flags],`${sample.name} controls`);assert.equal(s[6],0);}
  }
});
test('retail MSG entries validate atomically for all seven stages and six loadouts',async()=>{
  const c=await createFixture();for(let stage=1;stage<=7;stage++)for(let player=0;player<3;player++)for(const shot of ['a','b']){
    const b=fs.readFileSync(new URL(`../local/retail/st${String(stage).padStart(2,'0')}_0${player}${shot}.msg`,import.meta.url));assert.equal(load(c,b),1);assert.equal(c._test_start(0),1);assert.equal(c._test_start(-1),0);
    const invalid=Buffer.from(b);invalid.writeUInt32LE(b.length+1,4);assert.equal(load(c,invalid),0);assert.equal(c._test_start(0),1);
  }
});
