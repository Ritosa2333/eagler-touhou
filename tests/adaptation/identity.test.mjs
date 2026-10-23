import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdir,mkdtemp,rm,writeFile} from 'node:fs/promises';
import {relative,resolve} from 'node:path';
import {fileIdentity,identityDigest} from '../../tools/testing/dependency-snapshot.mjs';
import {adaptationBrowserImplementation,adaptationBrowserMode,createCaseIdentityReader,diagnosticCaseIdentity,
  fingerprintPythonPlaywrightDependencies} from './identity.mjs';

test('the browser identity follows actual desktop driver flags rather than a group label',()=>{
  const task=(...flags)=>({group:'desktop-input',task:{args:['tools/ports/runtime-desktop-probes.mjs',...flags]}});
  for(const flag of ['--input-only','--touch-only','--audio-latency-only'])
    assert.equal(adaptationBrowserMode(task(flag)),'headless');
  assert.equal(adaptationBrowserMode(task('--background-only')),'headed');
  assert.equal(adaptationBrowserMode(task()),'headed');
  assert.equal(adaptationBrowserMode({group:'desktop-background',task:{args:['tools/ports/runtime-browser.mjs']}}),'headless');
  assert.throws(()=>adaptationBrowserMode(task('--background-only','--touch-only')),/one|combine|exclusive|mode/i);
  assert.equal(adaptationBrowserImplementation(task('--input-only')),'node-playwright');
  assert.equal(adaptationBrowserImplementation({task:{args:['tools/ports/diagnostic-browser.mjs']}}),'python-playwright');
});

test('diagnostic reuse follows selected bytes and rejects changed native sources before starting a server',async t=>{
  const root=resolve(import.meta.dirname,'../..');
  await mkdir(resolve(root,'.local'),{recursive:true});
  const directory=await mkdtemp(resolve(root,'.local/diagnostic-identity-test-'));
  t.after(()=>rm(directory,{recursive:true,force:true}));
  const local=path=>relative(root,resolve(directory,path)).replaceAll('\\','/');
  const put=async(path,value)=>{const target=resolve(directory,path);await mkdir(resolve(target,'..'),{recursive:true});await writeFile(target,value);};
  for(const path of ['font.ttf','data.dat','inputs/demo0.rpy','daily.rpy','unused.rpy','runtime/game.wasm',
    'golden/manifest.json','lab/style.css','native.cpp','corpus.json'])await put(path,path);
  for(const path of ['lab/server.mjs','quick-gate.mjs','daily-gate.mjs'])await put(path,'export const version=1;');
  for(const path of ['lab/verify.py','quick.py','daily.py'])await put(path,'# capture source\n');
  await put('build.json',JSON.stringify({diagnostic:true,sourceFiles:{'native.cpp':(await fileIdentity(resolve(directory,'native.cpp'))).sha256}}));
  const profile={serverScript:local('lab/server.mjs'),browserVerifier:local('lab/verify.py'),quickCapture:local('quick.py'),
    dailyCapture:local('daily.py'),quickGate:local('quick-gate.mjs'),dailyGate:local('daily-gate.mjs'),
    buildReceipt:local('build.json'),sourceRoot:local('.'),runtimeDirectory:local('runtime'),
    presentationDirectory:local('lab'),goldenManifest:local('golden/manifest.json'),quickInputs:['demo0.rpy'],
    dailyFixtures:[{id:'sample',replayKey:'sample',stage:0}],decisionInputs:{quick:[local('corpus.json')]}};
  const manifestPath=resolve(directory,'diagnostics.json');
  await put('diagnostics.json',JSON.stringify({schema:'eagler-touhou/diagnostic-inputs/1',products:{th08:{normal:{
    font:resolve(directory,'font.ttf'),data:resolve(directory,'data.dat'),inputRoot:resolve(directory,'inputs'),
    replayFiles:{sample:resolve(directory,'daily.rpy'),unused:resolve(directory,'unused.rpy')},
  }}}}));
  const read=lanes=>diagnosticCaseIdentity({root,manifestPath,game:'th08',variant:'normal',lanes,profile});
  const presentation=identityDigest(await read(['presentation'])),quick=identityDigest(await read(['quick']));
  await put('unused.rpy','unselected bytes changed');
  assert.equal(identityDigest(await read(['presentation'])),presentation);
  assert.equal(identityDigest(await read(['quick'])),quick);
  await put('inputs/demo0.rpy','selected bytes changed');
  assert.equal(identityDigest(await read(['presentation'])),presentation);
  assert.notEqual(identityDigest(await read(['quick'])),quick);
  const beforeCorpus=identityDigest(await read(['quick']));
  await put('corpus.json','new fixture selection or Replay hash');
  assert.notEqual(identityDigest(await read(['quick'])),beforeCorpus);
  assert.equal(identityDigest(await read(['presentation'])),presentation);
  await put('runtime/game.wasm','new build bytes');
  assert.notEqual(identityDigest(await read(['presentation'])),presentation);
  await put('native.cpp','modified native authority');
  await assert.rejects(read(['presentation']),/Rebuild modified diagnostic source/);
});

test('Python Playwright identity includes transitive installed distribution files',async t=>{
  const root=resolve(import.meta.dirname,'../..');
  await mkdir(resolve(root,'.local'),{recursive:true});
  const directory=await mkdtemp(resolve(root,'.local/python-playwright-identity-test-'));
  t.after(()=>rm(directory,{recursive:true,force:true}));
  const put=async(path,value)=>{const target=resolve(directory,path);await mkdir(resolve(target,'..'),{recursive:true});await writeFile(target,value);return target;};
  const playwright=await put('playwright/__init__.py','playwright');
  const greenlet=await put('greenlet/__init__.py','greenlet-v1');
  const pyee=await put('pyee/__init__.py','pyee');
  const dependencies=[
    {name:'playwright',version:'1.0',files:[playwright]},
    {name:'greenlet',version:'3.0',files:[greenlet]},
    {name:'pyee',version:'12.0',files:[pyee]},
  ];
  const first=await fingerprintPythonPlaywrightDependencies(dependencies);
  assert.deepEqual(first.map(item=>item.name),['greenlet','playwright','pyee']);
  await put('greenlet/__init__.py','greenlet-v2');
  const changed=await fingerprintPythonPlaywrightDependencies(dependencies);
  assert.notDeepEqual(changed,first);
  await assert.rejects(fingerprintPythonPlaywrightDependencies([
    {name:'greenlet',version:'3.0',files:[]},
  ]),/file inventory/);
});

test('build identity binds existing evidence without private DATA or browser discovery',async()=>{
  const root=resolve(import.meta.dirname,'../..'),manifest='runtime.json';let revision='a'.repeat(64),
    checkedAt='2026-10-21T00:00:00.000Z',calls=0;
  const reader=createCaseIdentityReader({root,environment:{EAGLER_TEST_RUNTIME_MANIFEST:manifest},
    inspectBuildEvidence:async request=>{
      calls++;assert.equal(request.manifestPath,manifest);assert.equal(request.game,'th08');assert.equal(request.variant,'normal');
      return{manifest:{path:'runtime.json',sha256:'b'.repeat(64)},
        target:{game:'th08',variant:'normal',sourceRoot:'ports/th08',packageSha256:'c'.repeat(64)},
        sourceBuild:{report:{path:'source.json',sha256:revision,runId:'source-run'},observation:{receipt:{sha256:'d'.repeat(64)}}},
        package:{report:{path:'package.json',sha256:'e'.repeat(64),runId:'package-run'},observation:{package:{sourceBuildReceiptSha256:'d'.repeat(64)}}},
        verification:{mode:'revalidated-existing-stage-reports',checkedAt}};
    }});
  const testCase={group:'build',game:'th08',variant:'normal',checks:['BUILD-01'],inputs:['EAGLER_TEST_RUNTIME_MANIFEST'],
    task:{command:process.execPath,args:['tools/ports/build-acceptance.mjs','--stage=verify-existing',
      '--runtime-manifest=runtime.json','--game=th08','--variant=normal','--output=out']}};
  const first=await reader(testCase);
  assert.ok(first.fingerprint);assert.ok(first.identity.buildEvidence.sourceBuild.report.sha256===revision);
  assert.equal(Object.hasOwn(first.identity,'tools'),false);
  assert.equal(Object.hasOwn(first.identity.buildEvidence,'data'),false);
  checkedAt='2026-10-21T00:05:00.000Z';
  const sameEvidence=await reader(testCase);
  assert.equal(sameEvidence.fingerprint,first.fingerprint,'revalidation wall time does not invalidate stable evidence identity');
  assert.notEqual(sameEvidence.identity.buildEvidence.verification.checkedAt,first.identity.buildEvidence.verification.checkedAt,
    'the actual revalidation time remains available in the identity record');
  const preflight=await createCaseIdentityReader({root,environment:{EAGLER_TEST_RUNTIME_MANIFEST:manifest},inputOnly:true,
    inspectBuildEvidence:async()=>({})})(testCase);
  assert.equal(preflight.fingerprint,undefined);assert.equal(preflight.inputPreflight.status,'passed');
  revision='f'.repeat(64);
  const changed=await reader(testCase);
  assert.notEqual(changed.fingerprint,first.fingerprint,'replacing a report at the same path changes the identity');
  assert.equal(calls,3);
});
