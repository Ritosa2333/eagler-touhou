import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compiler=process.env.TH128_EMCC??path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'../..');
const python=process.env.TH128_PYTHON??path.join(sdk,'python/3.13.3_64bit/python.exe');
const environment={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
  EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
  EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
const routes=['st_a1_3','st_a2_3','st_b1_3','st_b2_3','st_c1_3','st_c2_3'];
const fixture=JSON.parse(fs.readFileSync(path.join(root,'local/research/thirdstage-boss-fixtures.json'),'utf8'));

function hex32(value){return (value>>>0).toString(16).padStart(8,'0');}
function float32Bits(value){
  const buffer=new ArrayBuffer(4),view=new DataView(buffer);
  view.setFloat32(0,value,true);
  return hex32(view.getUint32(0,true));
}
function first(chain,opcode,predicate=()=>true){return chain.find(event=>event.opcode===opcode&&predicate(event));}
function countdown(limit,current){
  const remaining=(limit-current)|0;
  return [Math.trunc(remaining/60),Math.trunc((remaining%60)*100/60)];
}

function routeScript(routeName){
  const route=fixture.routes.find(item=>item.main===routeName);
  assert.ok(route,`missing route ${routeName}`);
  assert.deepEqual(Object.keys(route.spawns).sort(),['BossA','BossB','BossC']);
  assert.deepEqual(route.registrations,{BossA:[0],BossB:[1],BossC:[2]});
  assert.deepEqual(route.deathUnregister.map(item=>item.slots),[[-1],[-1]]);
  const lifeA=first(route.chainA,411,event=>event.subroutine==='BossA');
  const phaseA=first(route.chainA,414);
  const gaugeA=route.chainA.find(event=>event.opcode===427&&event.subroutine==='Boss1A');
  assert.ok(route.chainA.some(event=>event.opcode===427&&event.life===0),`${route.main} lacks final gauge retirement`);
  const nextA=route.chainA.find(event=>event.opcode===414&&event!==phaseA);
  const deathB=first(route.chainB,414,event=>event.slot===0);
  const cardB=first(route.chainB,414,event=>event.slot===1);
  const deathC=first(route.chainC,414,event=>event.slot===0);
  const cardC=first(route.chainC,414,event=>event.slot===1);
  const lifeB=first(route.chainB,411,event=>event.subroutine==='BossB');
  const lifeC=first(route.chainC,411,event=>event.subroutine==='BossC');
  assert.ok(lifeA&&phaseA&&gaugeA&&nextA&&deathB&&cardB&&deathC&&cardC&&lifeB&&lifeC,`incomplete chain ${routeName}`);
  assert.equal(phaseA.slot,0);
  assert.equal(phaseA.threshold>0,true);
  assert.equal(gaugeA.slot,0);
  assert.equal(deathB.threshold,0);
  assert.equal(cardB.threshold,0);
  assert.equal(deathC.threshold,0);
  assert.equal(cardC.threshold,0);
  const lines=['Z'];
  const expected=['Z'];
  lines.push(`B 0 0 ${phaseA.threshold} ${phaseA.limit} ${phaseA.target}`);
  lines.push(`B 1 0 ${deathB.threshold} ${deathB.limit} ${deathB.target}`);
  lines.push(`B 1 1 ${cardB.threshold} ${cardB.limit} ${cardB.target}`);
  lines.push(`B 2 0 ${deathC.threshold} ${deathC.limit} ${deathC.target}`);
  lines.push(`B 2 1 ${cardC.threshold} ${cardC.limit} ${cardC.target}`);
  expected.push('B 0 1','B 1 1','B 1 1','B 2 1','B 2 1');
  const initial=countdown(phaseA.limit,0);
  lines.push(`Q 0 ${lifeA.life} 0 16`);
  expected.push(`Q 0 - ${lifeA.life} ${lifeA.life-phaseA.threshold} ${phaseA.threshold} -1 0 ${initial[0]} ${initial[1]} 81000000 00000010 0 0`);
  lines.push(`Q 1 ${lifeB.life} 0 16`);
  expected.push(`Q 1 - ${lifeB.life} ${lifeB.life} 0 -1 0 ${initial[0]} ${initial[1]} 81000000 00000010 0 0`);
  lines.push(`Q 2 ${lifeC.life} 0 16`);
  expected.push(`Q 2 - ${lifeC.life} ${lifeC.life} 0 -1 0 ${initial[0]} ${initial[1]} 81000000 00000010 0 0`);
  lines.push(`Q 0 ${phaseA.threshold} 0 16`);
  expected.push(`Q 0 ${phaseA.target} ${phaseA.threshold} 0 ${phaseA.threshold} -1 0 ${initial[0]} ${initial[1]} 81000000 00000010 0 0`);
  lines.push('C 0 0');
  expected.push(`C 0 0 -1 ${phaseA.limit} ${phaseA.target} ${phaseA.target}`);
  lines.push(`B 0 0 ${nextA.threshold} ${nextA.limit} ${nextA.target}`);
  expected.push('B 0 1');
  const switched=countdown(nextA.limit,0);
  lines.push(`Q 0 ${phaseA.threshold} 0 16`);
  expected.push(`Q 0 - ${phaseA.threshold} ${phaseA.threshold} ${nextA.threshold} -1 0 ${switched[0]} ${switched[1]} 81000000 00000010 0 0`);
  lines.push(`Q 1 ${lifeB.life} 3600 16`);
  expected.push(`Q 1 - ${lifeB.life} ${lifeB.life} 0 3599 3600 ${switched[0]} ${switched[1]} 81000000 00000010 0 0`);
  lines.push(`Q 2 ${lifeC.life} 4200 16`);
  expected.push(`Q 2 - ${lifeC.life} ${lifeC.life} 0 4199 4200 ${switched[0]} ${switched[1]} 81000000 00000010 0 0`);
  lines.push(`Q 0 ${phaseA.threshold} ${nextA.limit} 16`);
  expected.push(`Q 0 ${nextA.target} ${nextA.threshold} ${phaseA.threshold} ${nextA.threshold} -1 0 0 0 83000000 00000010 0 1`);
  lines.push('Q 1 0 0 16');
  expected.push(`Q 1 ${deathB.target} 0 0 0 -1 0 0 0 81000000 00000010 0 0`);
  lines.push('C 1 0');
  expected.push(`C 1 0 -1 ${deathB.limit} ${deathB.target} ${deathB.target}`);
  lines.push('C 1 1');
  expected.push(`C 1 1 ${cardB.threshold} ${cardB.limit} ${cardB.target} ${cardB.target}`);
  lines.push('Q 2 0 0 16');
  expected.push(`Q 2 ${deathC.target} 0 0 0 -1 0 0 0 81000000 00000010 0 0`);
  lines.push('C 2 1');
  expected.push(`C 2 1 ${cardC.threshold} ${cardC.limit} ${cardC.target} ${cardC.target}`);
  lines.push('Q 0 0 0 16');
  expected.push('Q 0 - 0 0 0 -1 0 0 0 83000000 00000010 0 0');
  const gaugeBits=float32Bits(gaugeA.life/lifeA.life);
  lines.push('Z','T 0 0 10');
  expected.push('Z','T 0 1 10 0 0 00000000');
  lines.push('T 1 1 11');
  expected.push('T 1 1 10 11 0 00000000');
  lines.push('T 2 2 12');
  expected.push('T 2 1 10 11 12 00000000');
  lines.push('P');
  expected.push('P 10 11 12 00000000 0 00000000 0 00000000 0 00000000');
  lines.push(`X 0 0 ${gaugeA.life} ${lifeA.life} ${gaugeA.color}`);
  expected.push(`X 0 0 1 ${gaugeBits} 1 ${gaugeA.color}`);
  lines.push('P');
  expected.push(`P 10 11 12 00000000 1 ${gaugeBits} 0 00000000 0 00000000`);
  lines.push('U 0');
  expected.push('U 0 1 0 11 12 00000000 00000000 1 00000000 0');
  lines.push('P');
  expected.push('P 0 11 12 00000000 1 00000000 0 00000000 0 00000000');
  lines.push('T 0 0 13');
  expected.push('T 0 1 13 11 12 00000000');
  lines.push('T 0 0 10');
  expected.push('T 0 1 10 11 12 00000000');
  lines.push('U 1');
  expected.push('U 1 1 10 0 12 00000000 00000000 1 00000000 0');
  return {lines:lines.join('\n')+'\n',expected};
}

test('six third-stage routes share three registered fairies with route-specific records',()=>{
  assert.equal(fixture.schema,'th128-original-thirdstage-boss/1');
  assert.equal(fixture.executable.sha256,fixture.executable.sha256After);
  assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/thirdstage_boss_probe.py'))).digest('hex'),fixture.generatorSha256);
  assert.deepEqual(fixture.routes.map(route=>route.main),routes);
  const skeletons=new Set();
  for(const route of fixture.routes){
    for(const fairy of ['BossA','BossB','BossC'])assert.ok(route.spawns[fairy],`${route.main} misses ${fairy}`);
    assert.deepEqual([route.spawns.BossA.x,route.spawns.BossA.y,route.spawns.BossA.life],[-144,-32,40]);
    assert.deepEqual([route.spawns.BossB.x,route.spawns.BossB.y],[-192,-32]);
    assert.deepEqual([route.spawns.BossC.x,route.spawns.BossC.y],[192,-32]);
    assert.deepEqual(route.registrations,{BossA:[0],BossB:[1],BossC:[2]});
    const phaseB=route.chainB.filter(event=>event.opcode===414);
    assert.ok(phaseB.some(event=>event.slot===0&&event.target==='BossEscape'),`${route.main} lacks escape record`);
    assert.ok(phaseB.some(event=>event.slot===1&&event.target.startsWith('BossCard1')),`${route.main} lacks own card record`);
    const phaseA=route.chainA.filter(event=>event.opcode===414);
    assert.ok(phaseA.length>=5&&phaseA.every(event=>event.slot===0),`${route.main} A records are not slot 0`);
    assert.ok(route.chainA.every(event=>event.opcode!==427||event.slot===0),`${route.main} gauge is not slot 0`);
    assert.ok(route.chainA.some(event=>event.opcode===427),`${route.main} lacks fairy A gauge`);
    assert.ok(route.chainB.every(event=>event.opcode!==427),`${route.main} unexpectedly gauges B`);
    assert.ok(route.chainC.every(event=>event.opcode!==427),`${route.main} unexpectedly gauges C`);
    assert.ok(route.chainA.some(event=>event.opcode===613&&event.argument===1),`${route.main} lacks cross-boss 613`);
    assert.deepEqual(route.deathUnregister.map(item=>item.slots),[[-1],[-1]]);
    assert.deepEqual(route.midbossRegistration.map(item=>item.slots),[[0],[-1],[-1]]);
    assert.deepEqual(route.postMidbossCallers,['main']);
    assert.ok(route.bossPath.missingHostOpcodes.includes(452));
    assert.ok(route.bossPath.missingHostOpcodes.includes(529));
    assert.ok(route.bossPath.missingHostOpcodes.includes(613));
    assert.ok(route.bossPath.missingHostOpcodes.every(opcode=>[274,300,310,311,327,425,445,452,508,529,600,602,613].includes(opcode)),
      `${route.main} exposes unexplained missing host opcodes`);
    const phaseA0=phaseA[0];
    const nextA=phaseA[1];
    assert.ok(phaseA0.threshold>0&&phaseA0.limit>0,`${route.main} first A record is not a timed threshold`);
    assert.equal(nextA.threshold,0);
    assert.ok(nextA.limit>0);
    skeletons.add(`${phaseA0.slot}:${nextA.slot}:${route.registrations.BossB[0]}:${route.registrations.BossC[0]}`);
  }
  assert.equal(skeletons.size,1);
});

test('original 412, 420 and 613 handlers bound the three registered bosses',()=>{
  const byName=Object.fromEntries(fixture.cases.registration.map(item=>[item.case,item]));
  for(const name of ['register-slot0','register-slot1','register-slot2']){
    const item=byName[name];
    assert.equal(item.flagsAfter,0x81000000);
    assert.equal(item.managerFlagsAfter,0);
  }
  assert.equal(byName['register-slot0'].retainedAfter,0);
  assert.equal(byName['register-slot1'].retainedAfter,1);
  assert.equal(byName['register-slot2'].retainedAfter,2);
  assert.deepEqual(byName['clear-retained'].ownersAfter.slice(0,3),[0,0x601000,0x602000]);
  assert.deepEqual(byName['clear-without-flag'].ownersAfter.slice(0,3),[0x600000,0x601000,0x602000]);
  assert.equal(byName['slot8-writes-flags'].managerFlagsAfter,0x330000);
  const wait=fixture.cases.bossWait;
  assert.equal(wait.find(item=>item.owners.join()==='0,0,0').result,0);
  assert.equal(wait.find(item=>item.owners.join()==='5,0,0').result,-1);
  assert.equal(wait.find(item=>item.owners.join()==='0,0,7').result,-1);
  assert.equal(wait.find(item=>item.owners.length===8&&item.owners[3]===9).result,0);
  const cross=fixture.cases.crossBoss;
  const both=cross.find(item=>item.case==='both-active');
  assert.deepEqual(both.stop,{reason:'boundary',address:'0x004708a0'});
  assert.equal(both.esi,'0x3a0000');
  assert.equal(both.ecx,4200);
  assert.equal(both.ownerAAfter.field1018,-1);
  assert.equal(both.ownerAAfter.field101c,'0x3a0000');
  assert.equal(both.ownerCAfter.field101c,'0x000000');
  const skipped=cross.find(item=>item.case==='skip-zero-target');
  assert.equal(skipped.esi,'0x3c0000');
  assert.equal(skipped.ownerAAfter.field101c,'0x000000');
  assert.equal(skipped.ownerCAfter.field101c,'0x3c0000');
  const inactive=cross.find(item=>item.case==='skip-inactive');
  assert.equal(inactive.esi,'0x3c0000');
  const none=cross.find(item=>item.case==='no-target-owner');
  assert.deepEqual(none.stop,{reason:'boundary',address:'0x0041c52c'});
  assert.equal(fixture.code.handlers['412'],'0x41a4bb');
  assert.equal(fixture.code.handlers['420'],'0x41b3dd');
  assert.equal(fixture.code.handlers['427'],'0x41a238');
  assert.equal(fixture.code.handlers['613'],'0x41c42f');
});

test('three fairy phase replay matches original record, timeout and gauge behaviour on every route',()=>{
  assert.ok(fs.existsSync(path.join(root,'src/game/EnemyPhaseRules.hpp')));
  assert.ok(fs.existsSync(path.join(root,'src/game/EnemyRules.hpp')));
  const output=path.join(root,'artifacts/tests/multi-boss-phase.cjs');
  const build=spawnSync(python,[compiler,'tests/native/multi-boss-phase.cpp','src/game/EnemyPhaseRules.cpp',
    'src/game/EnemyRules.cpp','-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math',
    '-ffp-contract=off','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],
    {cwd:root,env:environment,encoding:'utf8',timeout:180000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const lines=[],expected=[];
  for(const route of routes){
    const script=routeScript(route);
    lines.push(script.lines);
    expected.push(...script.expected);
  }
  const run=spawnSync(process.execPath,[output],{cwd:root,input:lines.join(''),encoding:'utf8',timeout:60000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  const actual=run.stdout.trimEnd().split(/\r?\n/);
  const mismatch=actual.findIndex((line,index)=>line!==expected[index]);
  if(mismatch>=0)assert.fail(`line ${mismatch}: actual ${JSON.stringify(actual[mismatch])} expected ${JSON.stringify(expected[mismatch])}`);
  assert.equal(actual.length,expected.length);
});
