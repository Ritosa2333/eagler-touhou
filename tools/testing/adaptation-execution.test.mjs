import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdir,mkdtemp,readFile,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {fileIdentity,identityDigest} from './dependency-snapshot.mjs';
import {runAdaptationVerification} from '../../tests/adaptation/execution.mjs';

const fixture=`import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
const [root,game,out]=process.argv.slice(2), startedAt=new Date().toISOString();
const behavior=JSON.parse(await readFile(resolve(root,game+'.json'),'utf8'));
await writeFile(resolve(root,game+'.marker'),(Number(await readFile(resolve(root,game+'.marker'),'utf8').catch(()=>0))+1).toString());
await mkdir(out,{recursive:true});
if(!behavior.missing)await writeFile(resolve(out,'report.json'),JSON.stringify({schema:'eagler-touhou/runtime-browser/1',runId:game+'-'+startedAt,
startedAt,finishedAt:new Date().toISOString(),status:'passed',passed:true,targets:[{game:behavior.wrongGame?'th20':game,variant:'normal',status:'passed',
checks:behavior.empty?[]:[{id:'movement',checks:['INPUT-01'],status:behavior.fail?'failed':'passed',complete:false}]}]}));
process.exitCode=behavior.exit??0;`;

async function setup(t,{offlineGames=[]}={}){
  const root=await mkdtemp(resolve(tmpdir(),'adaptation-execution-'));t.after(()=>rm(root,{recursive:true,force:true}));
  await writeFile(resolve(root,'driver.mjs'),fixture);
  for(const game of ['th06','th07'])await writeFile(resolve(root,game+'.json'),'{}');
  const buildPlan=({output})=>({unmappedChecks:[],cases:['th06','th07'].map(game=>({id:game,game,variant:'normal',
    group:offlineGames.includes(game)?'offline':'runtime',requiresLauncherBuild:offlineGames.includes(game),
    scenarioIds:['native-runtime-contract'],checks:['INPUT-01'],
    status:'ready',reportPath:resolve(output,game,'report.json'),requiredInputs:[],blockedInputs:[],
    task:{command:process.execPath,args:[resolve(root,'driver.mjs'),root,game,resolve(output,game)],cwd:root,timeoutMs:30000,resources:[]}}))});
  const createIdentityReader=()=>async item=>({fingerprint:identityDigest({driver:await fileIdentity(resolve(root,'driver.mjs')),
    input:await fileIdentity(resolve(root,item.game+'.json'))})});
  const createInputReader=()=>async()=>({inputPreflight:{status:'passed',targetIds:['th06','th07'],inputs:['runtimeManifest']}});
  const run=(name,options={},dependencies={})=>runAdaptationVerification({root,output:resolve(root,name),environment:{},...options},
    {buildPlan,createIdentityReader,createInputReader,skipPreparation:true,...dependencies});
  const count=async game=>Number(await readFile(resolve(root,game+'.marker'),'utf8').catch(()=>0));
  return{root,run,count};
}

test('real queue continues independent cases and rejects zero-exit false green',async t=>{
  const {root,run,count}=await setup(t);await writeFile(resolve(root,'th06.json'),'{"fail":true}');
  const report=await run('first');assert.equal(report.status,'failed');
  assert.deepEqual(report.cases.map(item=>item.status),['failed','passed']);assert.equal(await count('th07'),1);
  const queued=JSON.parse(await readFile(resolve(root,'first/processes/report.json'),'utf8'));
  assert.equal(queued.status,'failed');assert.equal(queued.results[0].processStatus,'passed');
});

test('failed Launcher preparation blocks its consumers while an independent Runtime still executes',async t=>{
  const {root,run,count}=await setup(t,{offlineGames:['th06']});await mkdir(resolve(root,'launcher'));
  const report=await run('prepare-failed',{}, {skipPreparation:false});
  assert.equal(report.preparation.status,'failed');assert.equal(report.status,'failed');assert.equal(report.passed,false);
  assert.equal(report.cases[0].status,'blocked');assert.equal(await count('th06'),0);
  assert.equal(report.cases[1].status,'passed');assert.equal(report.cases[1].execution,'executed');assert.equal(await count('th07'),1);
});

test('several Launcher consumers share one successful preparation',async t=>{
  const {root,run}=await setup(t,{offlineGames:['th06','th07']});await mkdir(resolve(root,'launcher/scripts'),{recursive:true});
  await writeFile(resolve(root,'launcher/scripts/build-launcher.mjs'),
    "import{appendFileSync}from'node:fs';appendFileSync('../prepared.log','prepared\\n');");
  const report=await run('prepare-passed',{}, {skipPreparation:false});
  assert.equal(report.status,'passed');assert.equal(report.executed,2);
  assert.equal(await readFile(resolve(root,'prepared.log'),'utf8'),'prepared\n');
});

test('resume retains sealed evidence, failed rerun executes only failures, and corrupted evidence reruns',async t=>{
  const {root,run,count}=await setup(t);await writeFile(resolve(root,'th06.json'),'{"fail":true}');
  const first=await run('first');const second=await run('second',{resume:resolve(root,'first')});
  assert.equal(second.executed,0);assert.equal(second.retained,2);assert.equal(await count('th06'),1);
  const third=await run('third',{rerunFailed:resolve(root,'second')});
  assert.equal(third.executed,1);assert.equal(third.retained,1);assert.equal(await count('th06'),2);assert.equal(await count('th07'),1);
  await writeFile(first.cases[1].report.path,'corrupted');
  const fourth=await run('fourth',{resume:resolve(root,'third')});
  assert.equal(fourth.executed,1);assert.equal(await count('th07'),2);
});

test('changed input reruns its product; unrelated documents preserve cached evidence',async t=>{
  const {root,run,count}=await setup(t);await run('first');await writeFile(resolve(root,'notes.md'),'changed docs');
  const unchanged=await run('second',{resume:resolve(root,'first')});assert.equal(unchanged.executed,0);
  await writeFile(resolve(root,'th06.json'),'{"fail":true}');
  const changed=await run('third',{resume:resolve(root,'second')});assert.equal(changed.executed,1);assert.equal(changed.retained,1);
  assert.equal(await count('th07'),1);assert.equal(changed.cases[0].status,'failed');
});

test('preview never prepares inputs or launches a process, and existing evidence is preserved',async t=>{
  const {root,run,count}=await setup(t);const report=await run('plan',{planOnly:true});
  assert.equal(report.status,'planned');assert.equal(report.passed,false);assert.equal(await count('th06'),0);
  assert.deepEqual(report.cases.map(item=>item.inputPreflight.status),['passed','passed']);
  const bytes=await readFile(resolve(root,'plan/summary.json'),'utf8');
  await assert.rejects(run('plan'),/new evidence directory/);assert.equal(await readFile(resolve(root,'plan/summary.json'),'utf8'),bytes);
});

test('preview blocks an absent, invalid or wrong-target Runtime manifest without launching its case',async t=>{
  const root=await mkdtemp(resolve(tmpdir(),'adaptation-plan-inputs-'));t.after(()=>rm(root,{recursive:true,force:true}));
  const driver=resolve(root,'driver.mjs'),marker=resolve(root,'launched.marker');
  await writeFile(driver,`import{writeFile}from'node:fs/promises';await writeFile(${JSON.stringify(marker)},'launched');`);
  const manifest=resolve(root,'runtime.json');
  const selections=[
    {name:'missing',write:false,reason:/ENOENT|no such file/i},
    {name:'invalid',write:'{',reason:/JSON|position/i},
    {name:'wrong-target',write:'{"targets":[]}',reason:/Missing Runtime target th08\/normal/},
  ];
  for(const selection of selections){
    await rm(manifest,{force:true});
    if(selection.write)await writeFile(manifest,selection.write);
    const output=resolve(root,selection.name);
    const report=await runAdaptationVerification({root,output,environment:{EAGLER_TEST_RUNTIME_MANIFEST:manifest},
      games:['th08'],variant:'normal',groups:['runtime'],planOnly:true},{buildPlan:({output:planOutput})=>({scope:'scenes',unmappedChecks:[],coverageTargets:[],cases:[{
        id:'runtime-normal-th08',game:'th08',variant:'normal',group:'runtime',scenarioIds:['native-runtime-contract'],checks:['INPUT-01'],
        status:'ready',requiredInputs:['EAGLER_TEST_RUNTIME_MANIFEST'],blockedInputs:[],reportPath:resolve(planOutput,'runtime/report.json'),
        task:{command:process.execPath,args:[driver,'--manifest='+manifest],cwd:root,timeoutMs:30000,resources:[]},
      }]})});
    assert.equal(report.status,'planned');assert.equal(report.passed,false);
    assert.equal(report.cases[0].status,'blocked',selection.name);
    assert.equal(report.cases[0].inputPreflight.status,'blocked',selection.name);
    assert.match(report.cases[0].issues.join(' '),selection.reason,selection.name);
    assert.ok(report.cases[0].blockedInputs.some(input=>input.name==='selected-local-inputs'),selection.name);
  }
  await assert.rejects(readFile(marker,'utf8'),{code:'ENOENT'});
});

test('selecting an adaptation ID requires complete evidence beyond a passed scene',async t=>{
  const {run}=await setup(t);const report=await run('explicit-check',{checks:['INPUT-01']});
  assert.equal(report.counts.passed,2);assert.equal(report.coverage.complete,false);
  assert.equal(report.status,'incomplete');assert.equal(report.passed,false);
});

test('a passing driver with no result rows cannot pass a planned scene group',async t=>{
  const {root,run}=await setup(t);await writeFile(resolve(root,'th06.json'),'{"empty":true}');
  const report=await run('empty-scene');
  assert.equal(report.cases[0].status,'incomplete');
  assert.match(report.cases[0].incompleteReasons.join(' '),/INPUT-01/);
  assert.equal(report.cases[1].status,'passed');
  assert.equal(report.status,'incomplete');assert.equal(report.passed,false);
});

test('resume reruns a prior case without proven postflight identity stability',async t=>{
  const {root,run,count}=await setup(t);await run('first');
  const summaryPath=resolve(root,'first','summary.json'),summary=JSON.parse(await readFile(summaryPath,'utf8'));
  delete summary.cases[0].identityStable;
  await writeFile(summaryPath,JSON.stringify(summary));
  const resumed=await run('resumed',{resume:resolve(root,'first')});
  assert.equal(resumed.cases[0].execution,'executed');
  assert.equal(resumed.cases[1].execution,'retained');
  assert.equal(await count('th06'),2);assert.equal(await count('th07'),1);
});

test('whole-product runs cannot pass while an applicable checklist requirement has no driver',async t=>{
  const {root,run}=await setup(t);
  const report=await run('whole-product',{}, {buildPlan:({output})=>({scope:'product',unmappedChecks:[],
    coverageTargets:[{game:'th06',variant:'normal',requirements:[{id:'DISPLAY-03'}]}],
    cases:[{id:'th06',game:'th06',variant:'normal',group:'runtime',requiresLauncherBuild:false,checks:['INPUT-01'],
      status:'ready',reportPath:resolve(output,'th06/report.json'),requiredInputs:[],blockedInputs:[],
      task:{command:process.execPath,args:[resolve(root,'driver.mjs'),root,'th06',resolve(output,'th06')],cwd:root,timeoutMs:30000,resources:[]}}]})});
  assert.equal(report.counts.passed,1);assert.equal(report.status,'incomplete');
  assert.deepEqual(report.coverage.rows.map(({id,status})=>({id,status})),[{id:'DISPLAY-03',status:'unmapped'}]);
  assert.equal(report.verificationScope,'product');assert.equal(report.passed,false);
  const review=await readFile(resolve(root,'whole-product/review.md'),'utf8');
  assert.match(review,/DISPLAY-03/);assert.match(review,/未关联自动任务/);
  assert.doesNotMatch(review,/\| 待填写 \||\| 待审查 \|/);
  assert.match(review,/固定逻辑状态重复绘制，比较状态摘要/);
  assert.match(review,/额外绘制只改变画面，逻辑与存档状态相同/);
});

test('a tampered parent pass and missing or mismatched child reports cannot be retained',async t=>{
  const {root,run}=await setup(t);await writeFile(resolve(root,'th06.json'),'{"missing":true}');
  await writeFile(resolve(root,'th07.json'),'{"wrongGame":true}');const first=await run('first');
  assert.deepEqual(first.cases.map(item=>item.status),['failed','failed']);
  first.cases[1].status='passed';await writeFile(resolve(root,'first/summary.json'),JSON.stringify(first));
  const next=await run('second',{resume:resolve(root,'first')});assert.equal(next.executed,2);assert.equal(next.status,'failed');
});

test('resume cannot upgrade numbered coverage through an edited parent summary',async t=>{
  const {root,run,count}=await setup(t);const first=await run('first');
  const check=first.cases[0].checks[0];assert.equal(check.complete,false);
  check.complete=true;check.claimedComplete=true;
  check.proof={source:'driver-report',references:[{pointer:'/invented',sha256:'f'.repeat(64)}]};
  await writeFile(resolve(root,'first/summary.json'),JSON.stringify(first));
  const resumed=await run('resumed',{resume:resolve(root,'first')});
  assert.equal(resumed.executed,1);assert.equal(resumed.retained,1);assert.equal(await count('th06'),2);
  assert.equal(resumed.cases[0].checks[0].complete,false);assert.equal(resumed.coverage.complete,false);
});

test('runtime-selected native judging input changes invalidate retained Replay evidence',async t=>{
  const {root,run,count}=await setup(t);const source=resolve(root,'oracle.cpp');await writeFile(source,'native reference 1');
  const first=await run('first'),item=first.cases[0],driver=JSON.parse(await readFile(item.report.path,'utf8'));
  driver.decisionInputs=[{path:'oracle.cpp',...await fileIdentity(source)}];
  await writeFile(item.report.path,JSON.stringify(driver));item.report={path:item.report.path,...await fileIdentity(item.report.path)};
  await writeFile(resolve(root,'first/summary.json'),JSON.stringify(first));await writeFile(source,'native reference 2');
  const next=await run('second',{resume:resolve(root,'first')});assert.equal(next.executed,1);assert.equal(await count('th06'),2);
});
