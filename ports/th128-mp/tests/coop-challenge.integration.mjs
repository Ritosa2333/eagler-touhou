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
 const settings={players,stageId:3,difficulty:1,mode:'same-stage-coop',seed:34772,rollbackWindow:2,challengeMode:true,...resources.build};
 const game=await prepared.openCoopStage(settings);try{
  const max=3000,visits=Array.from({length:players},()=>new Set());let last;
  for(let frame=0;frame<max;frame++){
   game.step({frame,inputs:Array.from({length:players},(_,player)=>({player,held:(frame%2?2:0)|(player===players-1&&frame>=190&&frame<250?1:0)}))});
   last=game.state();assert.equal(last.frame,frame+1);assert.equal(last.challengeMode,true);
   for(const seat of last.seats){assert.equal(seat.charge,0,'challenge has no spendable or accruing X');assert.equal(seat.lifeDisplay,seat.deaths);visits[seat.seat].add(seat.state);}
   if(frame>500&&last.seats.every(s=>s.respawns>0)&&visits.every(v=>[4,2,0,1].every(x=>v.has(x))))break;
  }
  assert.ok(last.seats.every(s=>s.deaths>0&&s.respawns>0),'Every seat must really be hit and respawn in challenge: '+JSON.stringify(last));
  assert.ok(last.seats[players-1].freezeCount>0,'challenge must preserve ordinary Z');console.log(JSON.stringify({players,challenge:last,visits:visits.map(v=>[...v])}));
 }finally{game.close();}
}
