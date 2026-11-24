import assert from 'node:assert/strict';import {readFile,mkdir} from 'node:fs/promises';import {dirname,join,resolve} from 'node:path';import {createRequire} from 'node:module';import {pathToFileURL} from 'node:url';
import {packageManagedRuntime} from '../tools/package-managed-runtime.mjs';import {readManagedProgram} from '../runtime/managed-program.mjs';import {verifyManagedStage} from './managed-package.integration.mjs';import {prepareCoopStage} from '../runtime/preview-world.mjs';import {makeCoopRules} from '../runtime/coop-rules.mjs';import {openManagedRoomTitle} from '../runtime/managed-stage-resources.mjs';
// Usage: original package request, intact Core127 site, author peer program directory, separate co-op core, empty private output, acorn's package dir.
const [fixture,coreSite,transportSite,coopSite,out,parser]=process.argv.slice(2).map(p=>resolve(p));await mkdir(out,{recursive:true});const parseModule=createRequire(import.meta.url)(parser).parse;
const {directory,descriptor}=packageManagedRuntime({coreSite,transportSite,coopSite,out,parseModule});const base=`http://127.0.0.1:42812/runtime/th128/multiplayer/${descriptor.generation}/`,url=base+'th128.html?runtimeVariant=multiplayer&managedData=1';
const program=await readManagedProgram(url,{fetchImpl:async u=>{assert.equal(u.href,base+'runtime/program.json');return new Response(await readFile(join(directory,'runtime/program.json')));}});
const files=Object.values(program).filter(p=>p?.url),basePath=new URL(base).pathname,programPaths=Object.fromEntries(files.map(r=>{assert.ok(r.url.startsWith(basePath));return [r.url,join(directory,r.url.slice(basePath.length))];}));
const {archivePath,comparison}=JSON.parse(await readFile(fixture));const signal=new AbortController().signal;
const report=await verifyManagedStage({archivePath,comparison,program,programPaths,signal,async probeCoop(owner){
 const bindings=JSON.parse(new TextDecoder().decode(await owner.read('soundBindings')));assert.ok(Object.keys(bindings.sounds).length>20,'author Package must contain original SE bindings');
 const factory=(await import(pathToFileURL(programPaths[program.coopCore.url]))).default;
 const cases=[];for(const players of [2,3])for(const difficulty of [0,1,2,3,4]){
  const r=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:'managed-coop-package',epoch:1,seed:4812,inputDelay:0,rollbackWindow:2,challengeMode:players===3,players,difficulty,route:difficulty===4?'EX':'A',coreSHA256:owner.identity.coreSHA256,resourceSHA256:owner.identity.resourceSHA256});
  const set=await prepareCoopStage({rules:r,resources:owner,factory,onFactory(){},onTick(){},onWorldStart(){},signal});
  for(const [,asset] of set.banks){const meta=JSON.parse(new TextDecoder().decode(await owner.read(`anim_${asset}`)));assert.ok(meta.entries.length>0);}
  const world=await set.openCoopStage(r);try{for(let frame=0;frame<80;frame++)world.step({frame,inputs:Array.from({length:players},(_,player)=>({player,held:frame<4?(16<<player):512}))});
   const state=world.state();assert.equal(state.ecl,r.ecl);assert.equal(state.rank,r.difficulty);assert.equal(state.challengeMode,r.challengeMode);assert.equal(state.seats.length,players);const draws=world.draws();assert.ok(state.seats.every(s=>draws.some(rec=>rec.id===4000000+10*s.seat&&rec.drawPass==='player')));cases.push([players,r.ecl,r.difficulty,r.challengeMode,state.frame,draws.length]);
  }finally{world.close();}
 }return cases;}});
assert.equal(report.results.length,10);console.log(JSON.stringify({generation:descriptor.generation,directory,files:descriptor.files.length,coreSHA256:program.coopWasm.sha256,resourcePackage:report}));
