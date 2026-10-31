import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {mkdir,writeFile,readFile} from 'node:fs/promises';

const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);

test('world comparator pairs physical hostile slots and preserves cancellation age differences',async()=>{
  // Comparator-only synthetic inputs, never original-game samples or a golden.
  const folder=path.join(root,'artifacts/reverse/world-comparator-test');await mkdir(folder,{recursive:true});
  const nativePath=path.join(folder,'native.json'),candidatePath=path.join(folder,'candidate.json'),reportPath=path.join(folder,'comparison.json');
  const economy={scoreStored:0,power:0,pivStored:0,maxPivStored:0,rank:0,lifeBombCounters:[0,0,0,0],ufoColors:[0,0,0],bombState:0};
  const player={fixedPosition:[0,0],state:1,stateAge:0,invulnerability:0,shootAge:0};
  const nativeShot=(id,slot,age,state)=>({id,slot,age,state,position:[slot,0],velocity:[2,0],angle:0,speed:2,typeColor:[7,6],collisionDelay:0,extensionCursor:1});
  const candidateShot=(id,slot,age,state)=>({id,physicalSlot:slot,age,friendly:false,position:[slot,0],motion:[0,2],type:7,color:6,hostile:[2,0,state,0,0,0,0,0,0,0,0,1]});
  const nativeFrame=(frame,bullets)=>({frame,enemies:[],bullets,items:[],friendly:[],economy,player,rngSeed:0,rngCalls:0});
  const candidateFrame=(frame,bullets)=>({frame,enemies:[],ecl:[],bullets,items:[],economy:{resources:Array(11).fill(0),ufoColors:[0,0,0]},player:{xFixed:0,yFixed:0,state:1,stateAge:0,invulnerability:0,fireFrame:0,bombState:{active:false}},rng:{seed:0,calls:0}});
  const native={scope:'Synthetic comparator unit test',nativePlayerExecuted:true,nativeItemsUfoSpellMsgExecuted:true,activeCollisionExecuted:true,nativeAnmExecuted:true,nativeBulletUpdateExecuted:true,creationGuardPrimed:false,events:[{kind:'bulletCreated',id:1000,frame:0},{kind:'bulletCreated',id:55,frame:0}],frames:[{frame:-1,enemies:[]},nativeFrame(0,[nativeShot(55,20,1,1),nativeShot(1000,351,81,1)]),nativeFrame(1,[nativeShot(55,20,2,1),nativeShot(1000,351,1,3)])]};
  const candidate={initial:{enemies:[],ecl:[]},ticks:[candidateFrame(1,[candidateShot(5004,351,81,1),candidateShot(2,20,1,1)]),candidateFrame(2,[candidateShot(5004,351,82,3),candidateShot(2,20,2,1)])]};
  await Promise.all([writeFile(nativePath,JSON.stringify(native)),writeFile(candidatePath,JSON.stringify(candidate))]);
  const run=spawnSync(emscripten(root).python,['scripts/reverse/compare-world-provider.py','--native',nativePath,'--candidate',candidatePath,'--output',reportPath],{cwd:root,encoding:'utf8'});
  assert.equal(run.status,0,run.stdout+run.stderr);const report=JSON.parse(await readFile(reportPath,'utf8'));
  assert.deepEqual(report.hostilePairing,['physicalSlot']);assert.equal(report.framesCompared,2);
  assert.deepEqual(report.differenceCounts,{'hostile.age':1});
  assert.deepEqual(report.firstDifferenceByField['hostile.age'],{inputTick:1,candidateFrame:2,field:'hostile.age',native:1,candidate:82,actor:351});
  assert.equal(report.wholeGameOracle,false);
  candidate.fixtureExecution={mask:15,scope:'Explicit passive fixture'};
  await writeFile(candidatePath,JSON.stringify(candidate));
  const command=['scripts/reverse/compare-world-provider.py','--native',nativePath,'--candidate',candidatePath,'--output',reportPath];
  const runRejected=()=>spawnSync(emscripten(root).python,command,{cwd:root,encoding:'utf8'});
  let rejected=runRejected();assert.notEqual(rejected.status,0);assert.match(rejected.stderr,/rejects candidate diagnostic fixtureExecution.mask=15/);
  candidate.fixtureExecution.mask=0;candidate.initial.fixtureFlags=1;
  await writeFile(candidatePath,JSON.stringify(candidate));
  rejected=runRejected();assert.notEqual(rejected.status,0);assert.match(rejected.stderr,/rejects candidate initial fixtureFlags=1/);
  candidate.initial.fixtureFlags=0;candidate.ticks[1].fixtureFlags=8;
  await writeFile(candidatePath,JSON.stringify(candidate));
  rejected=runRejected();assert.notEqual(rejected.status,0);assert.match(rejected.stderr,/rejects candidate inputTick1 fixtureFlags=8/);
});

test('opt-in laser comparison keeps independent IDs and exposes retirement differences',async()=>{
  // Deliberately unrelated native observation IDs, raw Timed lookup IDs and
  // candidate serials. These are comparator-only fixtures, not retail data.
  const folder=path.join(root,'artifacts/reverse/world-comparator-laser-test');await mkdir(folder,{recursive:true});
  const nativePath=path.join(folder,'native.json'),candidatePath=path.join(folder,'candidate.json'),reportPath=path.join(folder,'comparison.json');
  const player={fixedPosition:[0,0],state:1,stateAge:0,invulnerability:0,shootAge:0};
  const economy={scoreStored:0,power:0,pivStored:0,maxPivStored:0,rank:0,lifeBombCounters:[0,0,0,0],ufoColors:[0,0,0],bombState:0};
  const nativeFrame=(frame,phase)=>({frame,enemies:[],bullets:[],items:[],friendly:[],economy,player,rngSeed:0,rngCalls:0,lasers:[{observedObject:{id:77,firstObservedInputTick:0},nativeField80:56,kind:1,phase,position:[0,144,0],geometry:[0,480,2,0],headOffset:0,age:[frame,frame+1,frame+1],retirePending:0,cancellable:1}]});
  const candidateFrame=(frame,phase)=>({frame,enemies:[],ecl:[],bullets:[],items:[],economy:{resources:Array(11).fill(0),ufoColors:[0,0,0]},player:{xFixed:0,yFixed:0,state:1,stateAge:0,invulnerability:0,fireFrame:0,bombState:{active:false}},rng:{seed:0,calls:0},lasers:{nodes:[{serial:10001,lookupId:56,kind:1,phase,position:[0,144,0],geometry:[0,480,2,0,0],age:[frame-1,frame,frame],retirePending:0,cancellable:true}]}});
  const native={scope:'Synthetic comparator unit test',nativePlayerExecuted:true,nativeItemsUfoSpellMsgExecuted:true,activeCollisionExecuted:true,nativeAnmExecuted:true,nativeBulletUpdateExecuted:true,nativeLaserUpdateExecuted:true,creationGuardPrimed:false,events:[],frames:[{frame:-1,enemies:[]},nativeFrame(0,2),nativeFrame(1,1)]};
  const candidate={initial:{frame:0,enemies:[],ecl:[],lasers:{nodes:[]}},ticks:[candidateFrame(1,2),candidateFrame(2,2)]};
  await Promise.all([writeFile(nativePath,JSON.stringify(native)),writeFile(candidatePath,JSON.stringify(candidate))]);
  const run=spawnSync(emscripten(root).python,['scripts/reverse/compare-world-provider.py','--native',nativePath,'--candidate',candidatePath,'--output',reportPath,'--compare-lasers'],{cwd:root,encoding:'utf8'});
  assert.equal(run.status,0,run.stdout+run.stderr);const report=JSON.parse(await readFile(reportPath,'utf8'));
  assert.equal(report.framesCompared,2);assert.deepEqual(report.differenceCounts,{'laser.phase':1});
  assert.deepEqual(report.firstDifferenceByField['laser.phase'],{inputTick:1,candidateFrame:2,field:'laser.phase',native:1,candidate:2,actor:[0,1,0]});
  assert.equal(report.nativeLaserExecutionScope.laserFieldsCompared,true);assert.equal(report.wholeGameOracle,false);
  delete candidate.ticks[0].lasers;await writeFile(candidatePath,JSON.stringify(candidate));
  const missing=spawnSync(emscripten(root).python,['scripts/reverse/compare-world-provider.py','--native',nativePath,'--candidate',candidatePath,'--output',reportPath,'--compare-lasers'],{cwd:root,encoding:'utf8'});
  assert.notEqual(missing.status,0);assert.match(missing.stderr,/requires every candidate snapshot/);
});

test('graze comparison uses direct native and candidate counters and skips missing historical fields',async()=>{
  // Synthetic comparator input only; these values are not original-game goldens.
  const folder=path.join(root,'artifacts/reverse/world-comparator-graze-test');await mkdir(folder,{recursive:true});
  const nativePath=path.join(folder,'native.json'),candidatePath=path.join(folder,'candidate.json'),reportPath=path.join(folder,'comparison.json');
  const economy={scoreStored:0,power:0,pivStored:0,maxPivStored:0,rank:0,lifeBombCounters:[0,0,0,0],ufoColors:[0,0,0],bombState:0};
  const nativeFrame=(frame,graze)=>({frame,enemies:[],bullets:[],items:[],friendly:[],economy:{...economy,...(graze===undefined?{}:{graze})},player:{fixedPosition:[0,0],state:1,stateAge:0,invulnerability:0,shootAge:0},rngSeed:0,rngCalls:0});
  const candidateFrame=(frame,graze)=>({frame,enemies:[],ecl:[],bullets:[],items:[],economy:{resources:Array(11).fill(0),ufoColors:[0,0,0]},player:{xFixed:0,yFixed:0,state:1,stateAge:0,invulnerability:0,fireFrame:0,bombState:{active:false},...(graze===undefined?{}:{graze})},rng:{seed:0,calls:0}});
  const native={scope:'Synthetic comparator unit test',nativePlayerExecuted:true,nativeItemsUfoSpellMsgExecuted:true,activeCollisionExecuted:true,nativeAnmExecuted:true,nativeBulletUpdateExecuted:true,events:[],frames:[{frame:-1,enemies:[]},nativeFrame(0,42),nativeFrame(1,43),nativeFrame(2),nativeFrame(3,55)]};
  const candidate={initial:{enemies:[],ecl:[]},ticks:[candidateFrame(1,42),candidateFrame(2,44),candidateFrame(3,999),candidateFrame(4)]};
  const compare=async()=>{
    await Promise.all([writeFile(nativePath,JSON.stringify(native)),writeFile(candidatePath,JSON.stringify(candidate))]);
    const run=spawnSync(emscripten(root).python,['scripts/reverse/compare-world-provider.py','--native',nativePath,'--candidate',candidatePath,'--output',reportPath],{cwd:root,encoding:'utf8'});
    assert.equal(run.status,0,run.stdout+run.stderr);return JSON.parse(await readFile(reportPath,'utf8'));
  };
  let report=await compare();
  assert.deepEqual(report.differenceCounts,{'economy.graze':1});
  assert.deepEqual(report.firstDifferenceByField['economy.graze'],{inputTick:1,candidateFrame:2,field:'economy.graze',native:43,candidate:44});
  assert.equal(report.grazeComparisonScope.framesCompared,2);
  assert.equal(report.grazeComparisonScope.candidateField,'player.graze');
  for(const frame of native.frames)if(frame.economy)delete frame.economy.graze;
  report=await compare();assert.deepEqual(report.differenceCounts,{});assert.equal(report.grazeComparisonScope.framesCompared,0);
});
