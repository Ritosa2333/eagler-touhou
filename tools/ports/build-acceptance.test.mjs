import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtemp,readFile,rm,writeFile} from 'node:fs/promises';
import {join,resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {assessCheckoutDirectories,inspectExistingBuildAcceptance,inspectMinimalCheckout,loadBuildAcceptanceInputs,parseBuildAcceptanceOptions,
  packageOfficialRequest,resolveBuildAcceptanceTarget,runBuildAcceptance} from './build-acceptance.mjs';

const root=resolve(import.meta.dirname,'../..');

test('build target ownership follows the official workspace and Runtime recipes',()=>{
  const normal=resolveBuildAcceptanceTarget(root,'th08','normal');
  const mp=resolveBuildAcceptanceTarget(root,'th08','multiplayer');
  assert.equal(normal.sourcePath,'ports/th08');
  assert.equal(mp.sourcePath,'ports/th08-mp');
  assert.equal(normal.buildReceipt,'artifacts/validation/source-builds/th08-normal.runtime.json');
  assert.equal(normal.packageReceipt,'artifacts/validation/runtime-packages/th08-normal.json');
  assert.notEqual(normal.runtimeRoot,mp.runtimeRoot);
});

test('single-product checkout proof requires the selected source and excludes other product owners',()=>{
  const owners=[{game:'th08',variant:'normal',path:'ports/th08'},
    {game:'th08',variant:'multiplayer',path:'ports/th08-mp'},
    {game:'th09',variant:'normal',path:'ports/th09'}];
  const required=['launcher','common','tools','docs','tests','ports/th08'];
  const proof=assessCheckoutDirectories({game:'th08',variant:'normal',requiredPaths:required,owners,presentPaths:required});
  assert.deepEqual(proof.selectedOwners,['ports/th08','ports/th08-mp']);
  assert.deepEqual(proof.absentOtherProductOwners,['ports/th09']);
  assert.deepEqual(proof.unexpectedOtherProductOwners,[]);
  assert.throws(()=>assessCheckoutDirectories({game:'th08',variant:'normal',requiredPaths:required,
    owners,presentPaths:[...required,'ports/th09']}),/another product's source owner/);
  assert.throws(()=>assessCheckoutDirectories({game:'th08',variant:'normal',requiredPaths:required,
    owners,presentPaths:required.filter(path=>path!=='ports/th08')}),/missing required paths/);
});

test('a full checkout is not mistaken for BUILD-05 sparse source isolation',()=>{
  assert.throws(()=>inspectMinimalCheckout({root,game:'th08',variant:'normal'}),/another product's source owner/);
});

test('local build-input manifest selects one target and resolves declared paths from its directory',async()=>{
  const directory=await mkdtemp(join(tmpdir(),'build-inputs-'));
  try{
    const manifestPath=join(directory,'inputs.json');
    await writeFile(manifestPath,JSON.stringify({schema:'eagler-touhou/build-acceptance-inputs/1',targets:[
      {game:'th08',variant:'normal',fontRoot:'../font-root',options:{emsdk:'./sdk',assets:'../assets',cmake:'cmake',jobs:'6',thprac:'off'}},
      {game:'th08',variant:'multiplayer',options:{emsdk:'./mp-sdk'}}
    ]}));
    const inputs=loadBuildAcceptanceInputs({root,manifestPath,game:'th08',variant:'normal'});
    assert.equal(inputs.fontRoot,resolve(directory,'../font-root'));
    assert.equal(inputs.options.emsdk,resolve(directory,'sdk'));
    assert.equal(inputs.options.assets,resolve(directory,'../assets'));
    assert.equal(inputs.options.cmake,'cmake');
    assert.equal(inputs.options.jobs,'6');
    assert.deepEqual(inputs.buildArguments,[`--emsdk=${resolve(directory,'sdk')}`,
      `--jobs=6`,`--cmake=cmake`,`--assets=${resolve(directory,'../assets')}`,`--thprac=off`]);
    assert.match(inputs.sha256,/^[a-f0-9]{64}$/);
  }finally{await rm(directory,{recursive:true,force:true});}
});

test('build-input manifest rejects missing, duplicate, unknown, and override targets',async()=>{
  const directory=await mkdtemp(join(tmpdir(),'build-input-invalid-'));
  const manifestPath=join(directory,'inputs.json');
  const write=targets=>writeFile(manifestPath,JSON.stringify({schema:'eagler-touhou/build-acceptance-inputs/1',targets}));
  try{
    await write([{game:'th08',variant:'normal',options:{jobs:'4'}},{game:'th08',variant:'normal',options:{jobs:'8'}}]);
    assert.throws(()=>loadBuildAcceptanceInputs({root,manifestPath,game:'th08',variant:'normal'}),/Duplicate build-input target/);
    await write([{game:'th08',variant:'normal',options:{game:'th09'}}]);
    assert.throws(()=>loadBuildAcceptanceInputs({root,manifestPath,game:'th08',variant:'normal'}),/Unknown official build option game/);
    await write([{game:'th08',variant:'normal',options:{},fontroot:'../fonts'}]);
    assert.throws(()=>loadBuildAcceptanceInputs({root,manifestPath,game:'th08',variant:'normal'}),/unknown field/);
    await write([{game:'th99',variant:'normal',options:{}}]);
    assert.throws(()=>loadBuildAcceptanceInputs({root,manifestPath,game:'th08',variant:'normal'}),/Unknown build-input target/);
    await write([{game:'th08',variant:'normal',options:{jobs:'4'}}]);
    assert.throws(()=>loadBuildAcceptanceInputs({root,manifestPath,game:'th08',variant:'multiplayer'}),/exactly one th08\/multiplayer/);
  }finally{await rm(directory,{recursive:true,force:true});}
});

test('package request reuses only fontRoot and excludes build-only manifest options',async()=>{
  const directory=await mkdtemp(join(tmpdir(),'build-package-inputs-'));
  try{
    const manifestPath=join(directory,'inputs.json');
    await writeFile(manifestPath,JSON.stringify({schema:'eagler-touhou/build-acceptance-inputs/1',targets:[
      {game:'th08',variant:'normal',fontRoot:'../fonts',options:{emsdk:'../sdk',assets:'../assets',jobs:'12',cmake:'cmake'}}
    ]}));
    const inputs=loadBuildAcceptanceInputs({root,manifestPath,game:'th08',variant:'normal'});
    const request=packageOfficialRequest({game:'th08',variant:'normal',fontRoot:inputs.fontRoot});
    assert.deepEqual(request.args,['--game=th08','--variant=normal']);
    assert.deepEqual(request.environmentOverrides,{EAGLER_FONT_ROOT:resolve(directory,'../fonts')});
    assert.equal(Object.keys(request.environmentOverrides).length,1);
    assert.ok(inputs.buildArguments.some(argument=>argument.startsWith('--emsdk=')));
    assert.ok(inputs.buildArguments.some(argument=>argument==='--jobs=12'));
    assert.ok(!request.args.some(argument=>/emsdk|assets|jobs|cmake/.test(argument)));
  }finally{await rm(directory,{recursive:true,force:true});}
});

test('manifest CLI cannot be overridden or mixed with direct build options',()=>{
  assert.throws(()=>parseBuildAcceptanceOptions(['--game=th08','--variant=normal','--stage=source-build',
    '--output=out','--manifest=inputs.json','--emsdk=C:/elsewhere']),/either --manifest or command-line build options/);
  assert.throws(()=>parseBuildAcceptanceOptions(['--game=th08','--game=th09','--variant=normal',
    '--stage=source-build','--output=out']),/Duplicate --game/);
  const options=parseBuildAcceptanceOptions(['--game=th08','--variant=normal','--stage=verify-existing',
    '--runtime-manifest=.local/runtime.json','--output=out']);
  assert.equal(options.runtimeManifest,'.local/runtime.json');
  assert.throws(()=>parseBuildAcceptanceOptions(['--game=th08','--variant=normal','--stage=verify-existing',
    '--manifest=build.json','--runtime-manifest=runtime.json','--output=out']),/only by source-build and package/);
});

test('a build invocation that fails SDK preflight is sealed and cannot claim BUILD-03',async()=>{
  const output=await mkdtemp(join(tmpdir(),'build-acceptance-'));
  try{
    const manifest=join(output,'inputs.json');
    await writeFile(manifest,JSON.stringify({schema:'eagler-touhou/build-acceptance-inputs/1',targets:[
      {game:'th08',variant:'normal',fontRoot:'./private-fonts',options:{emsdk:'C:/missing-eagler-test-sdk'}}]}));
    const report=await runBuildAcceptance({root,game:'th08',variant:'normal',stage:'source-build',output,manifest});
    assert.equal(report.schema,'eagler-touhou/build-acceptance/1');
    assert.equal(report.status,'failed');
    assert.equal(report.scope.game,'th08');
    assert.equal(report.stage,'source-build');
    assert.equal(report.inputManifest.schema,'eagler-touhou/build-acceptance-inputs/1');
    assert.match(report.inputManifest.sha256,/^[a-f0-9]{64}$/);
    assert.equal(report.inputManifest.fontRoot,resolve(output,'private-fonts'));
    assert.equal(report.inputManifest.options.emsdk,resolve('C:/missing-eagler-test-sdk'));
    assert.notEqual(report.invocations[0].exitCode,0);
    assert.equal(report.invocations[0].inputManifestSha256,report.inputManifest.sha256);
    assert.match(report.invocations[0].argumentSha256,/^[a-f0-9]{64}$/);
    assert.deepEqual(report.invocations[0].environmentOverrideNames,['EAGLER_FONT_ROOT']);
    assert.match(report.invocations[0].environmentOverridesSha256,/^[a-f0-9]{64}$/);
    assert.deepEqual(report.checks,[]);
    const saved=JSON.parse(await readFile(join(output,'report.json'),'utf8'));
    assert.equal(saved.status,'failed');
  }finally{await rm(output,{recursive:true,force:true});}
});

test('provenance stage stays incomplete and names the required human source review',async()=>{
  const output=await mkdtemp(join(tmpdir(),'build-provenance-'));
  try{
    const report=await runBuildAcceptance({root,game:'th08',variant:'normal',stage:'provenance',output});
    assert.equal(report.status,'incomplete');
    assert.equal(report.passed,false);
    assert.equal(report.checks[0].id,'BUILD-06');
    assert.equal(report.checks[0].complete,false);
    assert.deepEqual(report.checks[0].evidence.observation.humanReviewRequired,
      ['upstream source or revision','license and author attribution','external game asset/font origins and hashes']);
  }finally{await rm(output,{recursive:true,force:true});}
});

function existingReportFixture(directory){
  const now='2026-10-21T00:00:00.000Z',later='2026-10-21T00:00:01.000Z';
  const digest=letter=>letter.repeat(64);
  const sourceInvocation={script:'launcher/scripts/build-runtime.mjs',startedAt:now,finishedAt:later,exitCode:0,
    processId:120,signal:null,argumentSha256:digest('a')};
  const packageInvocation={script:'launcher/scripts/package-runtime.mjs',startedAt:now,finishedAt:later,exitCode:0,
    processId:121,signal:null,argumentSha256:digest('b')};
  const sourceObservation={invocation:sourceInvocation,plan:{path:'artifacts/validation/source-builds/th08-normal.json',
    sha256:digest('c'),planSha256:digest('d'),game:'th08',variant:'normal',owner:'th08',source:'ports/th08',common:'common',
    toolchain:{version:'emsdk-34',compiler:'emcc',root:'toolchains/emsdk'},features:{thprac:true},requiredSubmodules:[]},
    receipt:{path:'artifacts/validation/source-builds/th08-normal.runtime.json',sha256:digest('e'),
      schema:'eagler-touhou/runtime-source-build/3',sourceInputsSha256:digest('f'),sourceFilesSha256:digest('1'),
      executionSha256:digest('2'),planSha256:digest('d'),outputCount:3,sourceCount:42}};
  const target={game:'th08',variant:'normal',owner:'th08',sourcePath:'ports/th08',sourceRoot:'ports/th08',
    buildPlan:'artifacts/validation/source-builds/th08-normal.json',buildReceipt:sourceObservation.receipt.path,
    packageReceipt:'artifacts/validation/runtime-packages/th08-normal.json',runtimeRoot:'build-eagler',runtime:'build-eagler',
    entry:'th08.html',packageSha256:digest('3')};
  const packageObservation={invocation:packageInvocation,package:{path:target.packageReceipt,output:target.runtimeRoot,
    runtimeRoot:'D:/build-eagler',receiptSha256:digest('4'),schema:'eagler-touhou/runtime-package-build/1',
    packageSha256:target.packageSha256,wasmSha256:sourceObservation.receipt.executionSha256,manifestSha256:digest('5'),
    sourceBuildReceiptSha256:sourceObservation.receipt.sha256,sourceInputsSha256:sourceObservation.receipt.sourceInputsSha256,
    fileCount:12,entry:target.entry}};
  const producer=(stage,observation,invocation,ids)=>({schema:'eagler-touhou/build-acceptance/1',runId:`${stage}-run`,
    startedAt:now,finishedAt:later,status:'passed',passed:true,scope:{game:'th08',variant:'normal'},stage,
    invocations:[invocation],observations:{[stage==='source-build'?'sourceBuild':'package']:observation},
    checks:ids.map(id=>({id,status:'passed',complete:false,scope:'th08/normal',
      evidence:{game:'th08',variant:'normal',adaptationIds:[id],observation:{fixture:true}}}))});
  const sourceReport=producer('source-build',sourceObservation,sourceInvocation,['BUILD-01','BUILD-02','BUILD-03']);
  const packageReport=producer('package',packageObservation,packageInvocation,['BUILD-04']);
  const sourcePath=join(directory,'reports/source-build.json'),packagePath=join(directory,'reports/package.json');
  const manifestPath=join(directory,'runtime-manifest.json');
  const manifest={targets:[{game:'th08',variant:'normal',runtime:target.runtime,entry:target.entry,sourceRoot:target.sourceRoot,
    buildReceipt:target.buildReceipt,packageReceipt:target.packageReceipt,packageSha256:target.packageSha256,
    buildAcceptance:{sourceBuildReport:'reports/source-build.json',packageReport:'reports/package.json'}}]};
  return{target,sourceObservation,packageObservation,sourceReport,packageReport,sourcePath,packagePath,manifestPath,manifest,
    async write(){
      const {mkdir}=await import('node:fs/promises');
      await mkdir(join(directory,'reports'),{recursive:true});
      await writeFile(sourcePath,JSON.stringify(sourceReport,null,2)+'\n');
      await writeFile(packagePath,JSON.stringify(packageReport,null,2)+'\n');
      await writeFile(manifestPath,JSON.stringify(manifest,null,2)+'\n');
    }};
}

test('verify-existing re-reads and binds stage reports, rejecting replacement errors and stale receipts',async t=>{
  const directory=await mkdtemp(join(tmpdir(),'build-existing-evidence-'));t.after(()=>rm(directory,{recursive:true,force:true}));
  const fixture=existingReportFixture(directory);await fixture.write();
  const common={root:directory,manifestPath:fixture.manifestPath,game:'th08',variant:'normal',
    targetResolver:()=>fixture.target,sourceObserver:async()=>fixture.sourceObservation,
    packageObserver:async()=>fixture.packageObservation};
  const first=await inspectExistingBuildAcceptance(common);
  assert.equal(first.sourceBuild.report.stage,'source-build');assert.equal(first.package.report.stage,'package');
  assert.equal(first.package.observation.package.sourceBuildReceiptSha256,first.sourceBuild.observation.receipt.sha256);
  const originalHash=first.sourceBuild.report.sha256;
  await writeFile(fixture.sourcePath,`${JSON.stringify(fixture.sourceReport,null,2)}\n `);
  const replaced=await inspectExistingBuildAcceptance(common);
  assert.notEqual(replaced.sourceBuild.report.sha256,originalHash,'same-path replacement changes the bound report identity');

  const rewrite=async mutate=>{
    const report=JSON.parse(await readFile(fixture.packagePath,'utf8'));mutate(report);
    await writeFile(fixture.packagePath,JSON.stringify(report,null,2)+'\n');
  };
  await rewrite(report=>{report.stage='source-build';});
  await assert.rejects(inspectExistingBuildAcceptance(common),/not the package stage/);
  await rewrite(report=>{report.stage='package';report.scope.variant='multiplayer';});
  await assert.rejects(inspectExistingBuildAcceptance(common),/another product or variant/);
  await rewrite(report=>{report.stage='package';report.scope={game:'th09',variant:'normal'};});
  await assert.rejects(inspectExistingBuildAcceptance(common),/another product or variant/);
  await rewrite(report=>{report.scope.game='th08';report.status='failed';report.passed=false;});
  await assert.rejects(inspectExistingBuildAcceptance(common),/did not pass/);
  await rewrite(report=>{report.status='passed';report.passed=true;delete report.finishedAt;});
  await assert.rejects(inspectExistingBuildAcceptance(common),/valid timestamp/);
  await rewrite(report=>{report.finishedAt='2026-10-21T00:00:01.000Z';report.observations.package.package.packageSha256='9'.repeat(64);});
  await assert.rejects(inspectExistingBuildAcceptance(common),/no longer match current source, plan, toolchain or package receipts/);
  await rewrite(report=>{report.observations.package=fixture.packageObservation;});
  await assert.rejects(inspectExistingBuildAcceptance({...common,sourceObserver:async()=>({...fixture.sourceObservation,
    receipt:{...fixture.sourceObservation.receipt,sha256:'8'.repeat(64)}})}),/no longer match current source/);
  await rewrite(report=>{report.observations.package=fixture.packageObservation;});
  await assert.rejects(inspectExistingBuildAcceptance({...common,packageObserver:async()=>({...fixture.packageObservation,
    package:{...fixture.packageObservation.package,packageSha256:'7'.repeat(64)}})}),/no longer match current source/);
  const manifest=JSON.parse(await readFile(fixture.manifestPath,'utf8'));
  manifest.targets[0].buildAcceptance.packageReport='reports/missing.json';
  await writeFile(fixture.manifestPath,JSON.stringify(manifest));
  await assert.rejects(inspectExistingBuildAcceptance(common),error=>error.code==='ERR_BUILD_EVIDENCE_MISSING');
  manifest.targets[0].buildAcceptance.packageReport='reports/package.json';
  manifest.targets[0].buildAcceptance.sourceBuildReport='reports/missing-source.json';
  await writeFile(fixture.manifestPath,JSON.stringify(manifest));
  await assert.rejects(inspectExistingBuildAcceptance(common),error=>error.code==='ERR_BUILD_EVIDENCE_MISSING');
});

test('verify-existing accepts sealed reports outside the checkout by absolute or manifest-relative path',async t=>{
  const parent=await mkdtemp(join(tmpdir(),'build-existing-external-'));t.after(()=>rm(parent,{recursive:true,force:true}));
  const root=join(parent,'repo'),external=join(parent,'evidence'),{mkdir}=await import('node:fs/promises');
  await mkdir(root);await mkdir(external);
  const fixture=existingReportFixture(root);await fixture.write();
  const sourcePath=join(external,'source-build.json'),packagePath=join(external,'package.json');
  await writeFile(sourcePath,JSON.stringify(fixture.sourceReport,null,2)+'\n');
  await writeFile(packagePath,JSON.stringify(fixture.packageReport,null,2)+'\n');
  const manifest=JSON.parse(await readFile(fixture.manifestPath,'utf8'));
  manifest.targets[0].buildAcceptance={sourceBuildReport:sourcePath,packageReport:packagePath};
  await writeFile(fixture.manifestPath,JSON.stringify(manifest,null,2)+'\n');
  const common={root,manifestPath:fixture.manifestPath,game:'th08',variant:'normal',targetResolver:()=>fixture.target,
    sourceObserver:async()=>fixture.sourceObservation,packageObserver:async()=>fixture.packageObservation};
  const absolute=await inspectExistingBuildAcceptance(common);
  assert.equal(absolute.sourceBuild.report.sha256,createHash('sha256').update(await readFile(sourcePath)).digest('hex'));
  manifest.targets[0].buildAcceptance={sourceBuildReport:'../evidence/source-build.json',packageReport:'../evidence/package.json'};
  await writeFile(fixture.manifestPath,JSON.stringify(manifest,null,2)+'\n');
  const relative=await inspectExistingBuildAcceptance(common);
  assert.equal(relative.package.report.sha256,createHash('sha256').update(await readFile(packagePath)).digest('hex'));
});

test('verify-existing emits complete revalidation claims without a compiler or packager invocation',async()=>{
  const output=await mkdtemp(join(tmpdir(),'build-existing-run-'));
  try{
    const verified={target:{game:'th08',variant:'normal',owner:'th08',sourceRoot:'ports/th08',buildPlan:'plan.json',
      buildReceipt:'receipt.json',packageReceipt:'package.json',runtime:'build-eagler',entry:'th08.html',packageSha256:'a'.repeat(64)},
      sourceBuild:{report:{path:'source.json',bytes:1,sha256:'b'.repeat(64),rawJson:'{}',runId:'source',
        startedAt:'2026-10-21T00:00:00.000Z',finishedAt:'2026-10-21T00:00:01.000Z',stage:'source-build',invocation:{}},observation:{}},
      package:{report:{path:'package.json',bytes:1,sha256:'c'.repeat(64),rawJson:'{}',runId:'package',
        startedAt:'2026-10-21T00:00:00.000Z',finishedAt:'2026-10-21T00:00:01.000Z',stage:'package',invocation:{}},observation:{}},
      manifest:{path:'runtime.json',sha256:'d'.repeat(64)},verification:{mode:'revalidated-existing-stage-reports',checkedAt:'2026-10-21T00:00:00.500Z'}};
    let inspected=0;
    const report=await runBuildAcceptance({root,game:'th08',variant:'normal',stage:'verify-existing',output,runtimeManifest:'runtime.json'},
      {inspectExisting:async args=>{inspected++;assert.equal(args.game,'th08');return verified;}});
    assert.equal(inspected,1);assert.equal(report.status,'passed');assert.equal(report.passed,true);
    assert.deepEqual(report.checks.map(item=>item.id),['BUILD-01','BUILD-02','BUILD-03','BUILD-04']);
    assert.ok(report.checks.every(item=>item.status==='passed'&&item.complete===true));
    assert.equal(report.invocations,undefined,'verify-existing never launches build or package commands');
  }finally{await rm(output,{recursive:true,force:true});}
});

test('verify-existing seals missing report references as blocked without launching a producer',async()=>{
  const output=await mkdtemp(join(tmpdir(),'build-existing-blocked-'));
  try{
    const error=Object.assign(new Error('Referenced package report is missing'),{code:'ERR_BUILD_EVIDENCE_MISSING'});
    const report=await runBuildAcceptance({root,game:'th08',variant:'normal',stage:'verify-existing',output,
      runtimeManifest:'runtime.json'},{inspectExisting:async()=>{throw error;}});
    assert.equal(report.status,'blocked');assert.equal(report.passed,false);
    assert.equal(report.invocations,undefined);
    assert.match(report.blockedInputs?.[0]?.reason,/package report is missing/);
  }finally{await rm(output,{recursive:true,force:true});}
});
