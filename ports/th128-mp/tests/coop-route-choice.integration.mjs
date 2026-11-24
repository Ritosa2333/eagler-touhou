// Offline integration with the owned original DAT, native ANM/SHT and compiled MP core.
// Usage: node coop-route-choice.integration.mjs <world-input.json> <MP-core.mjs> <MP-core.wasm>
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
for(const players of [2,3]){
 const rules=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:'route-check',epoch:1,players,route:'A',difficulty:0,challengeMode:true,freezeMotivationMultiplier:0.75,seed:7381,inputDelay:0,rollbackWindow:2,...resources.build});let instances=0;const set=await prepareCoopStage({rules,resources,factory,onFactory(){instances++;},onWorldStart(){},onTick(){}}),g=await set.openCoopStage(rules);
 let target=0,dialog=false,choice=null,trial=0,startingSelection,last,frame,decision=null;
 try{for(frame=0;frame<18000;frame++){
  const state=g.state();if(frame%6===0||choice?.started){const st=g.stageState(),enemies=st.entities.filter(e=>e.active&&!e.root&&!(e.flags&33)&&e.life>0);if(enemies.length)target=enemies.sort((a,b)=>a.life-b.life).at(-1).x;dialog=st.message.active;choice=st.message.routeChoice;last=st.stageTransition;
   if(choice.result!==null&&!decision){decision={result:choice.result,frame,selected:choice.message78};assert.equal(decision.selected,1,'host must choose the second branch');}
   if(decision&&last.nextId===4&&last.requested)break;
  }
  const keys=state.seats.map((v,i)=>{const x=v.rawX/128,move=target>x+2?128:target<x-2?64:0;
   if(!dialog)return {player:i,held:i===players-1&&frame<5300&&frame%180<65?1:(512|move)};
   if(!choice?.started)return {player:i,held:i===players-1&&frame%4===0?1:0};
   return {player:i,held:0};});
  if(dialog&&choice?.started&&choice.phase==='select'&&choice.message74===1&&choice.message70>=25&&!trial){
   startingSelection=choice.message78;keys.slice(1).forEach(k=>k.held=32);trial=1;
  }else if(trial===1){
   assert.equal(choice.message78,startingSelection,'P2/P3 cannot move the shared map cursor');
   assert.equal(choice.message74,1);keys[0].held=32;trial=2;
  }else if(trial===2){
   assert.equal(choice.message78,1,'host controls the shared map cursor');assert.equal(choice.message74,2);
   keys.slice(1).forEach(k=>k.held=1);trial=3;
  }else if(trial===3){
   assert.equal(choice.result,null,'P2/P3 cannot confirm the shared map');assert.equal(choice.message74,2);
   keys[0].held=1;trial=4;
  }
  g.step({frame,inputs:keys});
 }assert.ok(decision,'original route selector never accepted its host');assert.equal(last.nextId,4,JSON.stringify(last));assert.ok(last.requested,JSON.stringify(last));const source=g.state(),oldDraw=g.draws(),oldStage=g.stageState();assert.ok(source.teamPixels>0&&source.firepower>0,'verify nonzero original freeze carry');const token=g.checkpoint();const next=(f,move)=>({frame:f,inputs:source.seats.map((_,i)=>({player:i,held:i===players-1?move:0}))});
 g.step(next(frame,128));const predicted=g.state();assert.equal(predicted.ecl,'st_a2_2.ecl','load the requested real stage on the same native World owner');assert.equal(predicted.frame,frame+1);assert.equal(predicted.stageFrame,1);assert.equal(predicted.stageId,4);assert.equal(predicted.teamPixels,source.teamPixels);assert.equal(predicted.firepower,source.firepower);
 assert.equal(source.freezeMotivationMultiplier,0.75);assert.equal(predicted.freezeMotivationMultiplier,0.75);
 assert.deepEqual(predicted.seats.map(s=>[s.deaths,s.charge,s.freezeCount,s.starCredits]),source.seats.map(s=>[s.deaths,s.charge,s.freezeCount,s.starCredits]));assert.ok(predicted.seats.some(s=>s.deaths>0),'this is not an all-zero carry');assert.equal(g.stageState().normalRecord.bound,false);assert.ok(g.stageState().scoreRaw>=oldStage.scoreRaw);const over=g.checkpoint();
 g.restore(token);g.release(over);assert.deepEqual(g.state(),source);assert.deepEqual(g.draws(),oldDraw);g.step(next(frame,16));const corrected=g.state();assert.equal(corrected.stageId,4);assert.notDeepEqual(corrected.seats.at(-1),predicted.seats.at(-1));g.restore(token);g.release(token);g.step(next(frame,16));assert.deepEqual(g.state(),corrected);
 for(let i=1;i<=360;i++)g.step(next(frame+i,0));assert.equal(instances,1,'no second Wasm world is spawned for branch transfer');assert.equal(g.state().stageFrame,361);assert.equal(g.state().frame,frame+361);assert.ok(g.state().spawns>0);assert.equal(set.banksFor(4).find(([i])=>i===8)[1],'stage_c2');
 console.log(JSON.stringify({players,decision,nextId:last.nextId,globalFrame:g.state().frame,stageFrame:g.state().stageFrame,worlds:instances,score:g.stageState().scoreRaw,spawns:g.state().spawns,deaths:corrected.seats.map(s=>s.deaths)}));}finally{g.close();}
}
