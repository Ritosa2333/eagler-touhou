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
async function checkRescue(){const g=await prepared.openCoopStage({players:2,stageId:3,difficulty:1,mode:'same-stage-coop',seed:34772,rollbackWindow:2,challengeMode:false,...resources.build});let frame=0;
const move=(x,y,tx,ty)=> (tx-x>2?128:x-tx>2?64:0)|(ty-y>2?32:y-ty>2?16:0);
const step=(a,b)=>{g.step({frame:frame++,inputs:[a,b].map((held,player)=>({player,held}))});return g.state();};
try{let ghost,st=g.state();for(let j=0;j<5000&&!ghost;j++){
 const [donor,recipient]=st.seats;const x=donor.rawX/128,y=donor.rawY/128;const threats=g.stageState().bullets.map(b=>[b.x,b.y]).filter(p=>Math.abs(p[0]-x)<140&&Math.abs(p[1]-y)<180);let best=-Infinity,motion=0;
 for(const [dx,dy,keys] of [[0,0,0],[4.5,0,128],[-4.5,0,64],[0,-4.5,16],[0,4.5,32],[-3.18,-3.18,80],[3.18,-3.18,144],[-3.18,3.18,96],[3.18,3.18,160]]){
  const tx=Math.max(-174,Math.min(174,x+dx*5)),ty=Math.max(310,Math.min(421,y+dy*5));let cost=0;
  for(const [bx,by] of threats){const dist=(bx-tx)**2+(by+7-ty)**2;cost+=1800/(16+dist);}
  const value=-cost-.0001*((tx+130)**2+(ty-401)**2);if(value>best){best=value;motion=keys;}}
 let held=512|motion; st=step(held,0);if(st.seats[1].retired&&st.seats[1].state===2)ghost=frame;
 }
 assert.ok(ghost&&st.seats[0].motivation>=10000,'real exhausted P2 and a live donor with a spare are needed: '+JSON.stringify(st));
 // Let the actual death animation complete before trying to revive.
 for(let i=0;i<33;i++)st=step(0,0);
 assert.equal(st.seats[1].ghost,true);assert.equal(g.draws().find(r=>r.id===4000010).color>>>24,128);
 const ghostBefore=st.seats[1];let presses=0;
 for(let i=0;i<240;i++){const [a,b]=st.seats;if(a.motivation<10000)break;const x=a.rawX/128,y=a.rawY/128,tx=b.rawX/128,ty=b.rawY/128;
  const close=(tx-x)**2+(ty-y)**2<=18**2;let keys=move(x,y,tx,ty);if(close&&i%2===0){keys|=8;presses++;}st=step(keys,0);
  if(st.seats[1].state===1&&st.seats[1].motivation>=0){assert.ok(st.seats[1].motivation>=ghostBefore.motivation);assert.ok(st.seats[1].charge>=ghostBefore.charge);assert.ok(st.seats[1].protection>=238);assert.equal(st.seats[1].rescued,1);assert.equal(st.seats[1].ghost,false);const live=g.draws();assert.equal(live.find(r=>r.id===4000010).color>>>24,255);assert.ok(live.some(r=>r.id===12000010));assert.equal(st.seats[1].respawns,b.respawns,'a rescue must not perform the bullet-clearing ordinary respawn');console.log(JSON.stringify({ghostAt:ghost,rescuedAt:frame,presses,donor:st.seats[0].motivation,protected:st.seats[1].protection}));return;}
 }assert.fail('five close personal focus edges did not revive the actual ghost: '+JSON.stringify({ghost,presses,st}));}finally{g.close();}
}await checkRescue();
