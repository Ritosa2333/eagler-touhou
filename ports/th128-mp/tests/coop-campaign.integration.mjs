// Offline integration with the owned original DAT, native ANM/SHT and compiled MP core.
// Usage: node coop-campaign.integration.mjs <world-input.json> <MP-core.mjs> <MP-core.wasm>
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {openTh128Dat} from '../../th128/site/runtime/th128-dat.mjs';
import {openOriginalEndingFixture} from './coop-ending-readback.mjs';
import {prepareCoopStage} from '../runtime/preview-world.mjs';
const [pin,core,wasmPath]=process.argv.slice(2),config=JSON.parse(await readFile(pin,'utf8'));
const fixture=process.argv[5]?await openOriginalEndingFixture(process.argv[5]):null;
const sha=b=>createHash('sha256').update(b).digest('hex');
const read=async p=>new Uint8Array(await readFile(p));
const dat=await read(config.dat),wasm=await read(wasmPath); assert.equal(sha(dat),config.datSHA256);
const paths={...config,bullet9:config.type9,bullet32:config.type32,...Object.fromEntries(Object.entries(config.bullets).map(([k,v])=>['bullet'+k,v]))};
const factory=(await import(pathToFileURL(resolve(core)).href)).default;
const datOwner=fixture?openTh128Dat(dat):null;
const resources={...(fixture?{raw:name=>fixture.raw(name,value=>datOwner.read(value))}:{}),build:{coreSHA256:sha(wasm),resourceSHA256:sha(dat)},read:k=>k==='dat'?dat:k==='wasm'?wasm:read(paths[k])};
const {makeCoopRules}=await import('../runtime/coop-rules.mjs');
const arg=(name,allowed,fallback)=>{if(process.env[name]===undefined)return fallback;const v=Number(process.env[name]);assert.ok(allowed.includes(v),'unsupported '+name);return v;};
const rank=arg('TH128_TEST_RANK',[0,1,2,3],0);
const cases=[['A',2,rank,1],['B',3,rank,0],['C',3,rank,1],['EX',3,4,0]].map(([r,n,d,b])=>[r,arg('TH128_TEST_PLAYERS',[2,3],n),d,arg('TH128_TEST_BRANCH',[0,1],b)]);
for(const [route,players,difficulty,branch] of cases.filter(([route])=>!process.env.TH128_TEST_ROUTE||process.env.TH128_TEST_ROUTE===route)){
 const rules=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:'natural-clear',epoch:1,players,difficulty,route,challengeMode:true,seed:7381,inputDelay:0,rollbackWindow:2,...resources.build});let instances=0;
 const set=await prepareCoopStage({rules,resources,factory,onFactory(){instances++},onWorldStart(){},onTick(){}}),g=await set.openCoopStage(rules);
 const chain=[],events=[],opening=new Map();let at=0,target=0,dialog=false,choice=null,selected=false,last,f,terminalFrame=null;
 try{for(f=0;f<85000;f++){
  const state=g.state();if(at!==state.stageId){at=state.stageId;chain.push(at);opening.set(at,new Set());dialog=false;choice=null;selected=false;target=0;events.push(['stage',f,at]);}
  if(state.stageFrame<=220&&f%8===0)for(const record of g.draws().filter(r=>r.bank===30))opening.get(at).add(record.script);
  if(f%6===0||choice?.started&&choice?.result==null){const st=g.stageState();if(fixture)fixture.inspect(g.presentation());const enemies=st.entities.filter(e=>e.active&&!e.root&&!(e.flags&33)&&e.life>0);
   if(enemies.length)target=enemies.sort((a,b)=>a.life-b.life).at(-1).x;if(st.message.active!==dialog)events.push(['msg',f,at,st.message.active,st.message.entry]);dialog=st.message.active;choice=st.message.routeChoice;}
  const keys=state.seats.map((s,i)=>{const x=s.rawX/128,motion=target>x+2?128:target<x-2?64:0;if(!dialog)return {player:i,held:512|motion};if(choice?.started&&choice.result==null){if(i!==0)return {player:i,held:0};let h=0;if(choice.phase==='select'&&choice.message74===1&&choice.message70>=12)h=branch?32:16;else if(choice.message74===2&&choice.message78===branch&&choice.message70>=8&&!selected){selected=true;h=1;}return {player:i,held:h};}return {player:i,held:i===players-1&&f%4===0?1:0};});
  let effect;try{effect=g.step({frame:f,inputs:keys});}catch(e){throw Error(`original ${route} stage ${at} global ${f}: ${e.message}; trace ${JSON.stringify(events.slice(-8))}`);}
  if(effect?.cleared){terminalFrame=f+1;last=g.state();assert.equal(last.wipeFrames,0);assert.equal(g.stageState().normalRecord.bound,false);break;}
 }
 assert.ok(terminalFrame,JSON.stringify({route,chain,events,st:g.stageState().stageTransition}));const first=rules.stageId;
 assert.deepEqual(chain,route==='EX'?[16]:[first,first+(branch?3:1),first+(branch?4:2)]);for(const [id,scripts] of opening){assert.ok(scripts.has(1),`stage ${id} has the original title sprite`);assert.ok(scripts.has(3),`stage ${id} has the original track sprite`);}
 assert.equal(instances,1);assert.ok(last.seats.every(s=>s.deaths>0&&s.charge===0));assert.equal(last.clearStage,chain.at(-1));if(route==='EX'){assert.ok(last.seats.every(s=>Number.isSafeInteger(s.laserColumns)),'Every seat has actual EX selector-3 column attribution');assert.ok(last.seats.slice(1).some(s=>s.laserColumns>0),'At least one non-host ship is grazed/hit by the native Marisa light columns');}
 for(let i=0;i<3;i++){assert.ok(g.step({frame:terminalFrame+i,inputs:last.seats.map(s=>({player:s.seat,held:512}))}).cleared);assert.equal(g.state().frame,terminalFrame);}
 console.log(JSON.stringify({players,difficulty,route,chain,opening:[...opening].map(([id,s])=>[id,[...s]]),events,clearFrame:terminalFrame,stats:last.seats.map(s=>s.deaths),columns:last.seats.map(s=>s.laserColumns??null),score:g.stageState().scoreRaw,coreSHA256:resources.build.coreSHA256}));
 if(fixture)await fixture.end(g,last.clearStage);
 }finally{g.close();}
}
