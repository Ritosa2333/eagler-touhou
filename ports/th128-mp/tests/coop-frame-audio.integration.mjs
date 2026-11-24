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
const settings={players:2,stageId:3,difficulty:1,mode:'same-stage-coop',seed:34772,rollbackWindow:2,challengeMode:false,...resources.build};const g=await prepared.openCoopStage(settings);let n=0,restored=false;
try{for(let f=0;f<240;f++){const row={frame:f,inputs:[{player:0,held:0},{player:1,held:512}]},snap=restored?null:g.checkpoint();const effect=g.step(row),sounds=effect?.sounds??[];
 if(sounds.length){assert.ok(sounds.every(r=>Number.isInteger(r.id)&&[0,1,2].includes(r.op)&&Number.isFinite(r.x)));n+=sounds.length;if(snap){g.restore(snap);assert.deepEqual(g.step(row)?.sounds,sounds);restored=true;}}
 if(snap)g.release(snap);assert.equal(g.stageState().soundRequests,0,'drained inside the speculative snapshot, not accumulated for the entire game');
}assert.ok(n>0&&restored,'actual original firing/ECL audio was retained for the confirmed frame, not discarded');console.log(JSON.stringify({originalSoundRequests:n,rewind:true,frames:240}));}finally{g.close();}
