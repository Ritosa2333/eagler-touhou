import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';
import createFixture from '../artifacts/spell-tests/core.mjs';
const native=JSON.parse(fs.readFileSync(new URL('../local/reverse/spell-probe.json',import.meta.url)));
const snapshot=c=>[...new Int32Array(c.HEAPU8.buffer,c._spell_snapshot(),20)];
const compare=(c,n)=>{assert.deepEqual(snapshot(c).slice(0,8),[n.flags,n.age,n.previousAge,n.cardId,n.bonus,n.initialBonus,n.limit,n.serial]);assert.deepEqual([...new Float32Array(c.HEAPU8.buffer,c._spell_ring(),2)],n.ring);};
test('spell initialization matches six original difficulty/stage/flag/Bomb cases',async()=>{
 const c=await createFixture();for(const r of native.starts){c._spell_reset();assert.equal(c._spell_start(r.difficulty,r.stage,r.flagsBefore,r.bombActive,2400,1),1);compare(c,r.after);}
});
test('integer bonus decay and survival match all 6906 original subsystem snapshots',async()=>{
 const c=await createFixture();for(const r of native.ticks){c._spell_reset();c._spell_start(1,1,0,0,r.limit,1);if(r.survival)c._spell_survival();compare(c,r.samples[0]);for(let i=1;i<r.samples.length;++i){c._spell_tick(0,300,40,128);compare(c,r.samples[i]);assert.equal(snapshot(c)[9],i===61?1:0);}}
});
test('confirmed Miss, Bomb and timeout preserve native 60-tick grace and survival branches',async()=>{
 const c=await createFixture();for(const r of native.loss){c._spell_reset();c._spell_start(1,1,0,0,2400,1);if(r.survival)c._spell_survival();c._spell_prepare(r.survival?11:3,r.age,4000000,0,0,0);c._spell_loss({miss:1,bomb:2,timeout:3}[r.cause],r.bombActive);compare(c,r.after);c._spell_tick(r.bombActive,300,40,128);compare(c,r.nextTick);}
});
test('capture scoring uses stored score divided by ten, native cap and replay record rules',async()=>{
 const c=await createFixture();for(const r of native.ends){c._spell_reset();c._spell_start(1,1,0,0,2400,r.mode===1);c._spell_prepare(r.flagsBefore,0,r.bonus,r.scoreBefore,99998,99999);c._spell_end();const s=snapshot(c);assert.equal(s[0],r.afterFlags);assert.equal(s[12],r.score);assert.equal(s[13],r.results[0].failed?2:1);assert.equal(s[14],r.localCaptures);assert.equal(s[15],r.allCaptures);assert.equal(s[16],r.results[0].bonus);}
});
test('banner hysteresis and ring interpolation use exact native boundaries and floats',async()=>{
 const c=await createFixture();for(const r of native.hover){c._spell_reset();c._spell_start(1,1,0,0,2400,1);c._spell_prepare(r.flagsBefore,119,4000000,0,0,0);c._spell_tick(0,r.playerY,80,256);compare(c,r.after);assert.equal(snapshot(c)[8],r.interrupts[0]||0);}
});
test('all six first-stage names decode to original Shift-JIS bytes and card IDs resolve by opcode',async()=>{
 const c=await createFixture();for(const r of native.names){const bytes=Buffer.from(r.encrypted,'hex');c.HEAPU8.set(bytes,c._spell_text());const n=c._spell_decrypt(bytes.length),address=c._spell_title();const expected=Buffer.from(r.decrypted,'hex');assert.deepEqual(Buffer.from(c.HEAPU8.slice(address,address+n)),expected.subarray(0,expected.indexOf(0)));}
 for(let difficulty=0;difficulty<5;++difficulty){assert.equal(c._spell_card(422,2,difficulty),2);assert.equal(c._spell_card(428,2,difficulty),2);assert.equal(c._spell_card(437,2,difficulty),2+difficulty);assert.equal(c._spell_card(438,2,difficulty),1+difficulty);assert.equal(c._spell_card(439,2,difficulty),difficulty);}
});
test('attempt records retain original replay suppression and counter saturation',async()=>{
 const c=await createFixture();for(const r of native.attempts){c._spell_reset();c._spell_attempts(r.before);c._spell_retry(r.mode===1);assert.equal(snapshot(c)[10],r.localAttempts);assert.equal(snapshot(c)[11],r.allAttempts);}
});
test('start and result ANM binding order matches original bank/script/layer requests',async()=>{
 const c=await createFixture();for(const r of [...native.stageBindings,...native.presentation]){const data=[...new Int32Array(c.HEAPU8.buffer,c._spell_bindings(r.stage||0,r.stageFrame||0,!r.failed,r.bonus||0),r.bindings.length*3)],expected=r.bindings.flatMap(b=>[b.bank-101,b.script,b.layer]);assert.deepEqual(data,expected);if(!r.failed&&r.bonus!==undefined){const digits=[...new Int32Array(c.HEAPU8.buffer,c._spell_digits(r.bonus),16)];assert.deepEqual(digits.filter((_,i)=>i%2===0),r.sprites.slice(0,8));}}
});
