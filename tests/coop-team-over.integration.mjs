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
 const g=await prepared.openCoopStage(settings);const row=f=>({frame:f,inputs:Array.from({length:players},(_,player)=>({player,held:0}))});
 let ended=false,firstGhost=0,allGhosts=0;try{for(let f=0;f<30000;f++){const result=g.step(row(f)),st=g.state();const ghosts=st.seats.filter(s=>s.retired);if(ghosts.length&&!firstGhost)firstGhost=st.frame;
  if(ghosts.length===players&&!allGhosts)allGhosts=st.frame;if(st.wipeFrames>=180){assert.equal(result?.teamOver,true,'native terminal must propagate under confirmed input');assert.equal(st.frame-allGhosts,179);
   const cens=st.frame;const snap=g.checkpoint();assert.equal(g.step(row(f+1)).teamOver,true);assert.equal(g.state().frame,cens);g.restore(snap);g.release(snap);
   console.log(JSON.stringify({players,firstGhost,allGhosts,teamEnd:st.frame,counts:st.seats.map(s=>s.deaths)}));ended=true;break;}
 }assert.ok(ended,'team did not finish');assert.ok(firstGhost<allGhosts,'individual death must not end the team');}finally{g.close();}
}
