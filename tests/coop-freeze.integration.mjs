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
 const rules={players,difficulty:1,stageId:3,mode:'same-stage-coop',seed:34772,rollbackWindow:2,challengeMode:false,...resources.build};
 const game=await prepared.openCoopStage(rules), trigger=players-1;
 try{
  const step=(frame,held)=>game.step({frame,inputs:Array.from({length:players},(_,player)=>({player,held:player===trigger?held:0}))});
  let last,peak=0,firstAward=null;
  for(let frame=0;frame<560;frame++){
    const held=frame<32?16:frame>=190&&frame<250?1:0;
    if(frame===250){const snap=game.checkpoint();step(frame,1);game.restore(snap);game.release(snap);}
    step(frame,held);last=game.state();peak=Math.max(peak,last.seats[trigger].iceObjects);if(!firstAward&&last.teamPixels>0)firstAward=last;
  }
  assert.ok(peak>0);assert.ok(firstAward,'the participant released Z and the original ice mask completed');const source=firstAward.seats[trigger];
  assert.ok(firstAward.teamPixels>0);assert.equal(firstAward.teamPixels,source.lastPixels);assert.equal(source.freezeCount,1);assert.equal(firstAward.seats[0].freezeCount,0);
  const expected=firstAward.teamPixels*(players===2?80:75)/172032;assert.ok(Math.abs(expected-firstAward.firepower)<1e-5);const award=source.lastChargeReward;
  assert.ok(award>0);assert.equal(source.charge,award);assert.equal(firstAward.seats[0].charge,firstAward.teamPixels*20>=172032?award:0);
  console.log(JSON.stringify({players,trigger,peak,awarded:firstAward,ended:last}));
 }finally{game.close();}
}
