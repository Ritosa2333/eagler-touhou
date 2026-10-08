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
  const run=spawnSync(path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),['scripts/reverse/compare-world-provider.py','--native',nativePath,'--candidate',candidatePath,'--output',reportPath],{cwd:root,encoding:'utf8'});
  assert.equal(run.status,0,run.stdout+run.stderr);const report=JSON.parse(await readFile(reportPath,'utf8'));
  assert.deepEqual(report.hostilePairing,['physicalSlot']);assert.equal(report.framesCompared,2);
  assert.deepEqual(report.differenceCounts,{'hostile.age':1});
  assert.deepEqual(report.firstDifferenceByField['hostile.age'],{inputTick:1,candidateFrame:2,field:'hostile.age',native:1,candidate:82,actor:351});
  assert.equal(report.wholeGameOracle,false);
});
