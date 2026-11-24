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
const g=await prepared.openCoopStage({players:2,stageId:3,difficulty:1,mode:'same-stage-coop',seed:34772,rollbackWindow:2,challengeMode:false,...resources.build});
let frame=0;const tick=(a,b)=>{g.step({frame:frame++,inputs:[a,b].map((held,player)=>({player,held}))});return g.state();};
try{
 for(let i=0;i<4;i++)tick(128,64);let st=g.state();assert.ok(Math.abs(st.seats[1].rawX-st.seats[0].rawX)<=20*128);
 for(let i=0;i<3;i++){tick(8,0);st=tick(0,0);}assert.equal(st.seats[0].transferTaps,3,'five Shift taps with fire released must be owned by the donor');
 assert.ok(g.draws().some(r=>r.id===0x39000000&&r.sprite===76-32),"third Shift tap renders authored L 3/5 next to the donor");
 for(let i=0;i<25;i++)st=tick(0,0);assert.equal(st.seats[0].transferTaps,0,'timed-out tap chains must be forgotten');
 for(let i=0;i<4;i++){tick(8,0);st=tick(0,0);}assert.equal(st.seats[0].transferTaps,4);
 const snap=g.checkpoint(),before=st;const predicted=tick(512|8,0);assert.equal(predicted.seats[0].motivation,before.seats[0].motivation);g.restore(snap);frame--;g.release(snap);
 st=tick(8,0);assert.equal(st.seats[0].motivation,before.seats[0].motivation-10000);assert.equal(st.seats[1].motivation,before.seats[1].motivation);assert.equal(st.lifeGifts,1);assert.equal(st.seats[0].transfers,1);
 for(let i=0;i<20;i++)st=tick(8,0);assert.equal(st.seats[1].motivation,before.seats[1].motivation+10000);assert.equal(st.lifeGifts,0);assert.equal(st.seats[0].transfers,1,'held Shift is not another press');
 console.log(JSON.stringify({frames:frame,motivation:st.seats.map(s=>s.motivation),transfers:st.seats.map(s=>s.transfers),rollBackNoGift:true}));
}finally{g.close();}
