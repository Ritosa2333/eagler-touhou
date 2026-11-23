// Offline integration with the owned original DAT, native ANM/SHT and compiled MP core.
// Usage: node coop-world.integration.mjs <world-input.json> <MP-core.mjs> <MP-core.wasm>
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {prepareA13Preview} from '../runtime/preview-world.mjs';
const [pin,core,wasmPath]=process.argv.slice(2),config=JSON.parse(await readFile(pin,'utf8'));
const sha=b=>createHash('sha256').update(b).digest('hex');
const read=async p=>new Uint8Array(await readFile(p));
const dat=await read(config.dat),wasm=await read(wasmPath); assert.equal(sha(dat),config.datSHA256);
const paths={...config,bullet9:config.type9,bullet32:config.type32,...Object.fromEntries(Object.entries(config.bullets).map(([k,v])=>['bullet'+k,v]))};
const factory=(await import(pathToFileURL(resolve(core)).href)).default;
const resources={build:{coreSHA256:sha(wasm),resourceSHA256:sha(dat)},read:k=>k==='dat'?dat:k==='wasm'?wasm:read(paths[k])};
const counts={instances:0,starts:0,ticks:0};const prepared=await prepareA13Preview({resources,factory,onFactory(){counts.instances++},onWorldStart(){counts.starts++},onTick(){counts.ticks++}});
assert.equal(typeof prepared.openCoopStage,'function','real shared-stage opener is missing');
for(const players of [2,3]){
 const settings={players,difficulty:1,stageId:3,mode:'same-stage-coop',seed:34772,rollbackWindow:2,challengeMode:false,...resources.build};
 const before={...counts},a=await prepared.openCoopStage(settings),b=await prepared.openCoopStage(settings);
 try{
  assert.equal(counts.starts-before.starts,2);assert.equal(counts.instances-before.instances,2);
  const row=(frame,...held)=>({frame,inputs:held.map((held,player)=>({player,held}))});const initial=a.state();assert.equal(initial.frame,0);assert.equal(initial.seats.length,players);assert.deepEqual(initial.seats.map(s=>[s.motivation,s.charge,s.icePower]),Array.from({length:players},()=>[20000,0,10000]),'co-op starts each real actor at original unassisted pl00 resources, not preview maximum');
  const step=(engine,...keys)=>engine.step(row(engine.state().frame,...keys.slice(0,players)));
  assert.throws(()=>a.step(row(0,64,128,...(players===3?[1024]:[0]))));assert.deepEqual(a.state(),initial);
  step(a,64,128,16);step(b,64,128,16);const first=a.state();assert.equal(first.frame,1);assert.deepEqual(first,b.state());
  assert.equal(first.seats[0].rawX,initial.seats[0].rawX-576);assert.equal(first.seats[1].rawX,initial.seats[1].rawX+576);if(players===3)assert.equal(first.seats[2].rawY,initial.seats[2].rawY-576);
  const draws=a.draws();for(let i=0;i<players;i++)assert.ok(draws.some(d=>d.id===4000000+i*10&&d.drawPass==='player'),'missing real player ANM '+i);
  const checkpoint=a.checkpoint();step(a,0,0,0);assert.deepEqual(a.state().seats.map(s=>s.rawY),first.seats.map(s=>s.rawY));a.restore(checkpoint);a.release(checkpoint);
  step(a,0,16,128);step(b,0,16,128);assert.deepEqual(a.state(),b.state());assert.deepEqual(a.draws(),b.draws());assert.equal(a.state().seats[0].rawX,first.seats[0].rawX);
  for(let i=0;i<360;i++){step(a,512,512,512);step(b,512,512,512);}
  const state=a.state();assert.equal(state.frame,362);assert.deepEqual(state,b.state());assert.deepEqual(a.draws(),b.draws());
  assert.ok(state.seats.every(s=>s.damage>0),'every seat actually damaged enemies in the original stage: '+JSON.stringify(state));
  assert.ok(state.commonTarget>0,'at least two seats hit the same native enemy');
  assert.equal(counts.ticks-before.ticks,725);console.log(JSON.stringify({players,instances:counts.instances-before.instances,worldStarts:counts.starts-before.starts,worldTicks:counts.ticks-before.ticks,state,drawCount:a.draws().length,coreSHA256:resources.build.coreSHA256}));
 }finally{a.close();b.close();}
}