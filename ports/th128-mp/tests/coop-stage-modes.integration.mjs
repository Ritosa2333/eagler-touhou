// Offline integration with the owned original DAT, native ANM/SHT and compiled MP core.
// Usage: node coop-world.integration.mjs <world-input.json> <MP-core.mjs> <MP-core.wasm>
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {prepareCoopStage} from '../runtime/preview-world.mjs';
const [pin,core,wasmPath]=process.argv.slice(2),config=JSON.parse(await readFile(pin,'utf8'));
const sha=b=>createHash('sha256').update(b).digest('hex');
const read=async p=>new Uint8Array(await readFile(p));
const dat=await read(config.dat),wasm=await read(wasmPath); assert.equal(sha(dat),config.datSHA256);
const paths={...config,bullet9:config.type9,bullet32:config.type32,...Object.fromEntries(Object.entries(config.bullets).map(([k,v])=>['bullet'+k,v]))};
const factory=(await import(pathToFileURL(resolve(core)).href)).default;
const resources={build:{coreSHA256:sha(wasm),resourceSHA256:sha(dat)},read:k=>k==='dat'?dat:k==='wasm'?wasm:read(paths[k])};
const {makeCoopRules}=await import('../runtime/coop-rules.mjs');
const seen=[];
for(const route of ['A','B','C','EX'])for(const players of [2,3])for(const difficulty of route==='EX'?[4]:[0,1,2,3]){
 const rules=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:'modes-offline',epoch:1,route,players,difficulty,challengeMode:false,seed:5314,inputDelay:0,rollbackWindow:0,...resources.build});
 let starts=0;
 const prepared=await prepareCoopStage({rules,resources,factory,onFactory(){},onWorldStart(){starts++},onTick(){}});
 const game=await prepared.openCoopStage(rules);try{
  assert.equal(starts,1);for(let frame=0;frame<360;frame++)game.step({frame,inputs:Array.from({length:players},(_,i)=>({player:i,held:512}))});
  const state=game.state();assert.deepEqual([state.ecl,state.rank,state.frame,state.seats.length],[rules.ecl,difficulty,360,players]);
  assert.ok(game.draws().filter(x=>x.drawPass==='player'&&x.id%10===0).length>=players);
  assert.ok(state.spawns>0,'native authored ECL must run');seen.push([players,route,difficulty,state.spawns]);
 }finally{game.close();}
}
console.log(JSON.stringify({actualStageOpening:seen,sharedWorlds:seen.length,ticks:360*seen.length,coreSHA256:resources.build.coreSHA256}));
