import assert from 'node:assert/strict';
import {readFile,lstat} from 'node:fs/promises';
import {dirname,resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {runTasks} from '../testing/runner.mjs';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {fileIdentity,dependencySnapshot} from '../testing/dependency-snapshot.mjs';
import {inspectExistingBuildAcceptance} from './build-acceptance.mjs';

/** Resolve the same minimal checkout for inspection, official receipts and the real Launcher journey. */
export async function inspectMinimalRuntimeInputs({manifest,game,variant}={}){
  const manifestPath=resolve(manifest),input=JSON.parse(await readFile(manifestPath,'utf8'));
  assert.equal(input.schema,'eagler-touhou/minimal-runtime-inputs/1','Unsupported minimal Runtime input manifest');
  const selected=input.products?.[game]?.[variant];
  assert.ok(selected,`Minimal checkout inputs have no ${game}/${variant}`);
  assert.equal(variant,'normal','Register the room-owned single-game journey for multiplayer');
  const paths={};
  for(const name of ['checkoutRoot','runtimeManifest','packages','artwork']){
    assert.ok(typeof selected[name]==='string'&&selected[name].trim(),`Minimal checkout requires ${name}`);
    paths[name]=resolve(dirname(manifestPath),selected[name]);
    const info=await lstat(paths[name]);
    assert.ok(!info.isSymbolicLink()&&(['checkoutRoot','artwork'].includes(name)?info.isDirectory():info.isFile()),
      `Minimal ${name} must be a regular ${['checkoutRoot','artwork'].includes(name)?'directory':'file'}`);
  }
  const proof=await inspectExistingBuildAcceptance({root:paths.checkoutRoot,manifestPath:paths.runtimeManifest,game,variant});
  const dependencies=await dependencySnapshot(paths.checkoutRoot,[
    'tools/ports/build-acceptance.mjs','tools/ports/offline-runtime-browser.mjs',
    'launcher/scripts/build-launcher.mjs','launcher/scripts/package-server.mjs',
  ]);
  const inputs={manifest:await fileIdentity(manifestPath),runtimeManifest:await fileIdentity(paths.runtimeManifest),
    packages:await fileIdentity(paths.packages),checkoutRoot:paths.checkoutRoot,dependencies,proof};
  // Only wall-clock verification metadata is excluded from the resume identity.
  if(inputs.proof.verification)delete inputs.proof.verification.checkedAt;
  return{paths,inputs};
}

export function assessMinimalRuntimeJourney({game,variant,checkout,build,offline,assessEvidence}={}){
  assert.equal(typeof assessEvidence,'function','A complete-evidence assessor is required for the nested journey');
  assert.equal(checkout?.schema,'eagler-touhou/build-acceptance/1','Checkout report schema is unsupported');
  assert.equal(build?.schema,'eagler-touhou/build-acceptance/1','Build report schema is unsupported');
  assert.equal(offline?.schema,'eagler-touhou/offline-runtime-browser/1','Launcher report schema is unsupported');
  for(const [name,report] of Object.entries({checkout,build,offline})){
    const started=Date.parse(report?.startedAt),finished=Date.parse(report?.finishedAt);
    assert.ok(report?.runId&&Number.isFinite(started)&&Number.isFinite(finished)&&finished>=started&&
      report.status!=='running',`${name} evidence is not sealed`);
    assert.equal(report.scope?.game,game,`${name} belongs to another product`);
    assert.equal(report.scope?.variant,variant,`${name} belongs to another variant`);
  }
  assert.equal(new Set([checkout.runId,build.runId,offline.runId]).size,3,
    'Checkout, build and Launcher reports must come from separate nested executions');
  const selected=checkout.observations?.checkout;
  assert.equal(checkout.stage,'checkout','Missing checkout inspection stage');
  assert.equal(checkout.status,'incomplete','Checkout-only producer must not claim the full BUILD-05 journey');
  assert.equal(selected?.status,'passed','Minimal checkout ownership did not pass');
  assert.deepEqual(selected.unexpectedOtherProductOwners,[],'Another game source owner is present');
  assert.equal(build.status,'passed','Current minimal source/build/package identities did not pass');
  assert.equal(build.passed,true,'Current source/build report is not sealed as passed');
  assert.equal(build.stage,'verify-existing','Expected official producer revalidation');
  for(const id of ['BUILD-01','BUILD-02','BUILD-03','BUILD-04']){
    const checks=build.checks?.filter(c=>c.id===id)??[];
    assert.equal(checks.length,1,`Missing or duplicate ${id} evidence`);
    assert.equal(checks[0].status,'passed');assert.equal(checks[0].complete,true);
    const assessed=assessEvidence({driver:build,entry:checks[0],product:{game,variant}});
    assert.ok(assessed?.available&&assessed.valid,`${id} nested evidence failed recomputation: ${assessed?.reason??'unavailable'}`);
  }
  assert.equal(offline.status,'passed','The minimal Launcher browser journey did not pass');
  assert.equal(offline.passed,true,'Launcher browser report is not sealed as passed');
  const runtime=offline.checks?.filter(c=>c.id==='RUNTIME-05')??[];
  assert.equal(runtime.length,1,'Missing or duplicate same-run Runtime ownership evidence');
  assert.equal(runtime[0].status,'passed');assert.equal(runtime[0].complete,true);
  const runtimeAssessment=assessEvidence({driver:offline,entry:runtime[0],product:{game,variant}});
  assert.ok(runtimeAssessment?.available&&runtimeAssessment.valid,
    `RUNTIME-05 nested evidence failed recomputation: ${runtimeAssessment?.reason??'unavailable'}`);
  const buildPackage=build.checks.find(c=>c.id==='BUILD-04').evidence?.observation?.package?.current?.package?.packageSha256;
  assert.match(buildPackage??'',/^[a-f0-9]{64}$/,'Official package revalidation has no package identity');
  assert.equal(offline.runtimePackages?.normal?.packageSha256,buildPackage,'Browser did not use the minimal checkout package');
  assert.ok(offline.onlineRuntime?.observation?.first?.event==='first-frame','No real Launcher first-frame event');
  assert.ok(offline.onlineRuntime.observation.dataRead,'Runtime did not read installed DATA through the Launcher');
  return{id:'BUILD-05',status:'passed',complete:true,scope:`${game}/${variant} | single-game source checkout and real Launcher installation`,
    evidence:{game,variant,adaptationIds:['BUILD-05'],observation:{checkout:selected,packageSha256:buildPackage,
      firstFrame:offline.onlineRuntime.observation.first,managedData:offline.onlineRuntime.observation.dataRead,
      journey:{checkout,build,offline}}}};
}

async function main(){
  if(!process.argv[1]||resolve(process.argv[1])!==fileURLToPath(import.meta.url))return;
  assertCliArguments(process.argv.slice(2),{values:['manifest','game','variant','output'],flags:[]});
  const arg=name=>process.argv.find(v=>v.startsWith(`--${name}=`))?.slice(name.length+3);
  assert.ok(arg('output'),'Required: --output');const output=resolve(arg('output'));
  const {assessCompleteEvidence}=await import('../../tests/adaptation/complete-evidence.mjs');
  const report=await runWithReport({output,schema:'eagler-touhou/minimal-runtime-browser/1'},async(report,save)=>{
    report.scope={game:arg('game'),variant:arg('variant')};
    if(!arg('manifest')){report.status='blocked';report.reason='Provide the single-game checkout input manifest';return;}
    const before=await inspectMinimalRuntimeInputs({manifest:arg('manifest'),...report.scope});report.inputs=before.inputs;await save();
    const cwd=before.paths.checkoutRoot,task=(id,args,timeoutMs)=>({id,command:process.execPath,args,cwd,timeoutMs,resources:['browser','runtime']});
    const cases=[
      task('checkout',['tools/ports/build-acceptance.mjs','--stage=checkout',`--game=${report.scope.game}`,`--variant=${report.scope.variant}`,`--output=${resolve(output,'checkout')}`],60000),
      task('build',['tools/ports/build-acceptance.mjs','--stage=verify-existing',`--game=${report.scope.game}`,`--variant=${report.scope.variant}`,
        `--runtime-manifest=${before.paths.runtimeManifest}`,`--output=${resolve(output,'build')}`],180000),
      task('offline',['tools/ports/offline-runtime-browser.mjs',`--manifest=${before.paths.runtimeManifest}`,`--packages=${before.paths.packages}`,
        `--artwork-dir=${before.paths.artwork}`,`--game=${report.scope.game}`,`--output=${resolve(output,'offline')}`],900000),
    ];
    const execution=await runTasks(cases,{output:resolve(output,'execution'),concurrency:1,failFast:true});
    assert.equal(execution.status,'passed','A minimal checkout task failed; see the preserved execution report');
    const evidence=Object.fromEntries(await Promise.all(['checkout','build','offline'].map(async name=>[name,JSON.parse(await readFile(resolve(output,name,'report.json'),'utf8'))])));
    report.checks=[assessMinimalRuntimeJourney({...report.scope,...evidence,assessEvidence:assessCompleteEvidence})];
    const after=await inspectMinimalRuntimeInputs({manifest:arg('manifest'),...report.scope});
    assert.deepEqual(after.inputs,before.inputs,'Minimal checkout, package or input identities changed during the run');
    Object.assign(report,{status:'passed',passed:true,evidenceReports:cases.map(c=>resolve(output,c.id,'report.json'))});await save();
  });
  if(report.status!=='passed')process.exitCode=1;
}

if(process.argv[1]&&resolve(process.argv[1])===fileURLToPath(import.meta.url))
  void main().catch(error=>{console.error(error?.stack??error);process.exitCode=1;});
