import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdir,mkdtemp,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {buildExecutionOwnership,assertNoSameProfileTestDuplicates} from './execution-ownership.mjs';
import {repository,suites,profiles,trackedFiles} from '../../tests/suites.mjs';
import {isNodeTest} from './discovery.mjs';
import {nativeEnvironmentPolicy} from './native-inputs.mjs';

const command=process.execPath;
function testSuite(id,file,args=['--test',file],resources=[]){
  return {tier:'integration',scope:id,tasks:()=>[{id,command,cwd:repository,args,resources}]};
}

async function oracleFixture(t,{discovered=['candidate.test.mjs','maintenance.test.mjs'],tests=[
  {file:'candidate.test.mjs',role:'candidate',group:'unit'},
  {file:'maintenance.test.mjs',role:'maintenance',task:'maintenance-check'},
]}={}){
  const root=await mkdtemp(resolve(tmpdir(),'execution-ownership-oracle-'));
  t.after(()=>rm(root,{recursive:true,force:true}));
  const web=resolve(root,'ports/th09/web'),cpp=resolve(web,'tests/cpp');
  await mkdir(cpp,{recursive:true});
  await writeFile(resolve(web,'target.json'),JSON.stringify({game:'th09'}));
  for(const file of discovered)await writeFile(resolve(cpp,file),'// discovered test fixture\n');
  const manifest={schema:'eagler-touhou/native-oracle-suite/1',owner:'th09',game:'th09',variant:'normal',
    discovery:{directory:'tests/cpp',pattern:'*.test.mjs'},nativeEnvironment:nativeEnvironmentPolicy('th09'),baseInputs:[],
    groups:[{id:'unit',requiredInputs:[]}],tests,
    maintenance:[{id:'maintenance-check',kind:'test',file:'maintenance.test.mjs',
      outputRoot:'artifacts/validation/oracle-maintenance',requiredInputs:[]}]};
  const manifestPath=resolve(cpp,'oracle-suite.json');
  await writeFile(manifestPath,JSON.stringify(manifest));
  return {root,manifestPath,discovered,manifest};
}

function fixtureFiles(fixture,extra=[]){
  return ['ports/th09/web/tests/cpp/oracle-suite.json',...fixture.discovered.map(file=>`ports/th09/web/tests/cpp/${file}`),...extra];
}

function oracleCatalog(root,targetOwners=['th09']){
  return {'ports-oracle':{tier:'acceptance',scope:'fixture native comparisons',coverageKind:'original-comparison',
    targets:Object.freeze(targetOwners.map(owner=>Object.freeze({owner,root:resolve(root,'ports',owner,'web')}))),tasks:()=>[]}};
}

async function packageFixture(t,script,extraFiles=[]){
  const root=await mkdtemp(resolve(tmpdir(),'execution-ownership-package-'));
  t.after(()=>rm(root,{recursive:true,force:true}));
  const packagePath=resolve(root,'ports/th11-mp/web/package.json');
  await mkdir(resolve(root,'ports/th11-mp/portable/multiplayer'),{recursive:true});
  await mkdir(resolve(root,'ports/th11-mp/web'),{recursive:true});
  await writeFile(packagePath,JSON.stringify({scripts:{'test:multiplayer-audio-output':script}}));
  const tests=['ports/th11-mp/portable/multiplayer/audio-output.test.mjs',...extraFiles];
  for(const file of tests)await writeFile(resolve(root,file),'// package test fixture\n');
  return {root,files:['ports/th11-mp/web/package.json',...tests]};
}

test('the same path can have intentional cross-profile ownership but is rejected inside one profile',async()=>{
  const file='tools/ownership-fixture.test.mjs';
  const catalog={first:testSuite('first',file),second:testSuite('second',file)};
  const crossProfile=await buildExecutionOwnership({root:repository,catalog,
    profileSelections:{quick:['first'],integration:['second']},files:[]});
  assertNoSameProfileTestDuplicates(crossProfile);
  assert.deepEqual(crossProfile.profileReuse,[{file,profiles:['quick','integration']}]);

  const sameProfile=await buildExecutionOwnership({root:repository,catalog,
    profileSelections:{integration:['first','second']},files:[]});
  assert.throws(()=>assertNoSameProfileTestDuplicates(sameProfile),/multiple owners in one profile/);
});

test('package script paths resolve from their package directory, including parent-relative globs',async t=>{
  const direct=await packageFixture(t,'node --experimental-vm-modules --test ../portable/multiplayer/audio-output.test.mjs');
  const one=await buildExecutionOwnership({root:direct.root,catalog:{},profileSelections:{},files:direct.files});
  assert.equal(one.outsideSuiteInventory.packageScriptOwned,1);
  assert.equal(one.outsideSuiteInventory.unownedCandidateNames,0);

  const glob=await packageFixture(t,'node --test ../portable/multiplayer/*.test.mjs',[
    'ports/th11-mp/portable/multiplayer/second.test.mjs']);
  const two=await buildExecutionOwnership({root:glob.root,catalog:{},profileSelections:{},files:glob.files});
  assert.equal(two.outsideSuiteInventory.packageScriptOwned,2);
  assert.equal(two.outsideSuiteInventory.unownedCandidateNames,0);
});

test('package script test paths that escape the repository fail ownership discovery',async t=>{
  const fixture=await packageFixture(t,'node --test ../../../../outside.test.mjs');
  await assert.rejects(buildExecutionOwnership({root:fixture.root,catalog:{},profileSelections:{},files:fixture.files}),
    /Package test script path escapes repository/);
});

test('TH09 frozen smoothness evidence stays package-owned outside the routine suite',async()=>{
  const files=['ports/th09/web/package.json','ports/th09/web/tests/multiplayer/smoothness-window.test.mjs'];
  const report=await buildExecutionOwnership({root:repository,catalog:{},profileSelections:{},files});
  assert.equal(report.outsideSuiteInventory.packageScriptOwned,1);
  assert.equal(report.outsideSuiteInventory.unownedCandidateNames,0);
});

test('an explicit negative control explains its same-profile repeated browser entrypoint',async()=>{
  const file='tools/keyboard-browser.test.mjs';
  const catalog={browser:{tier:'integration',scope:'negative control',tasks:()=>[
    {id:'normal',command,cwd:repository,args:[file],resources:['browser']},
    {id:'negative',command,cwd:repository,args:[file,'--negative-control'],resources:['browser']},
  ]}};
  const report=await buildExecutionOwnership({root:repository,catalog,profileSelections:{integration:['browser']},files:[]});
  assertNoSameProfileTestDuplicates(report);
  assert.match(report.profiles.integration.duplicates[0].reason,/negative-control mutation/);
});

test('real suite paths derive from component manifests and auto-discovery, with only declared-domain gaps blocking',async()=>{
  const allFiles=trackedFiles(repository),catalog=suites();
  const profileSelections=Object.fromEntries(Object.entries(profiles));
  const report=await buildExecutionOwnership({root:repository,catalog,profileSelections,files:allFiles});
  assertNoSameProfileTestDuplicates(report);
  assert.deepEqual(report.managedOrphans,[]);
  for(const [id,suite] of Object.entries(catalog))assert.ok(suite.coverageKind,`${id} must declare its coverage layer`);

  for(const file of allFiles.filter(file=>file.startsWith('tools/')&&isNodeTest(file)))
    assert.ok(report.testOwners[file]?.owners.some(owner=>owner.suite==='tooling'),`new tool test has no owner: ${file}`);
  for(const file of allFiles.filter(file=>file.startsWith('tests/adaptation/')&&isNodeTest(file)))
    assert.ok(report.testOwners[file]?.owners.some(owner=>owner.suite==='tooling'),`new adaptation test has no owner: ${file}`);
  for(const suite of ['launcher-core','launcher-workspace','relay','turn','telemetry','ports-source','ports-unit'])
    assert.ok(Object.values(report.profiles).some(profile=>profile.suiteIds.includes(suite)),`missing suite owner: ${suite}`);
  assert.ok(report.intentionalProfileRepeats.some(item=>item.file==='launcher/tests/test-package-store-contract.mjs'));
  assert.ok(report.intentionalProfileRepeats.some(item=>item.file==='launcher/tests/test-keyboard-mapping-browser.py'));
  const inherited=report.profileReuse.filter(item=>item.profiles.join('+')==='quick+integration');
  assert.ok(inherited.length>0&&inherited.every(item=>/suite IDs from the smaller profile/.test(item.reason)));
  const workspaceReuse=report.profileReuse.filter(item=>item.profiles.join('+')==='integration+acceptance');
  assert.ok(workspaceReuse.length>0&&workspaceReuse.every(item=>/Launcher workspace manifest/.test(item.reason)));
  assert.ok(report.outsideSuiteInventory.portsCandidateNames>report.outsideSuiteInventory.packageScriptOwned,
    'historical and one-off port filenames remain visible for owner review');
  assert.ok(report.outsideSuiteInventory.oracleManifestOwned>900,
    'native files classified by title oracle manifests have a local owner');
  assert.equal(report.outsideSuiteInventory.oracleManifestCandidateFiles+
    report.outsideSuiteInventory.oracleManifestMaintenanceFiles,
  report.outsideSuiteInventory.oracleManifests.reduce((sum,item)=>sum+item.discoveredFiles,0));
  assert.ok(report.outsideSuiteInventory.oracleManifests.every(item=>item.localOwner==='oracle-suite.json'));
  assert.ok(report.outsideSuiteInventory.oracleManifests.every(item=>item.ciScheduling==='not-assessed-by-local-ownership'));
  assert.ok(Object.isFrozen(catalog['ports-oracle'].targets));
  assert.ok(report.outsideSuiteInventory.oracleManifests.every(item=>item.unifiedSuite.targetDeclared));
  assert.deepEqual(catalog['ports-oracle'].targets.map(item=>item.owner),
    report.outsideSuiteInventory.oracleManifests.map(item=>item.owner).sort());
});

test('native files have local manifest owners independently of unified-suite and CI scheduling',async t=>{
  const fixture=await oracleFixture(t),extra='ports/th10/web/tests/cpp/one-off.test.mjs';
  const catalog=oracleCatalog(fixture.root),files=fixtureFiles(fixture,[extra]);
  const localOnly=await buildExecutionOwnership({root:fixture.root,catalog,profileSelections:{local:[],selection:[]},files});
  const inventory=localOnly.outsideSuiteInventory;
  assert.equal(inventory.oracleManifestOwned,2);
  assert.equal(inventory.oracleManifestCandidateFiles,1);
  assert.equal(inventory.oracleManifestMaintenanceFiles,1);
  assert.equal(inventory.unownedCandidateNames,1);
  assert.deepEqual(inventory.unownedExamples,[extra]);
  assert.deepEqual(inventory.oracleManifests[0].unifiedSuite,
    {id:'ports-oracle',targetDeclared:true,targetRoot:'ports/th09/web',registeredProfiles:[],currentRunSelection:false});
  assert.equal(inventory.oracleManifests[0].ciScheduling,'not-assessed-by-local-ownership');

  const scheduled=await buildExecutionOwnership({root:fixture.root,catalog,
    profileSelections:{oracle:['ports-oracle'],selection:['ports-oracle']},files});
  assert.deepEqual(scheduled.outsideSuiteInventory.oracleManifests[0].unifiedSuite,
    {id:'ports-oracle',targetDeclared:true,targetRoot:'ports/th09/web',registeredProfiles:['oracle'],currentRunSelection:true});
  assert.equal(scheduled.outsideSuiteInventory.oracleManifests[0].ciScheduling,
    'not-assessed-by-local-ownership');

  const inventoryOnly=await buildExecutionOwnership({root:fixture.root,catalog,
    profileSelections:{oracle:['ports-oracle']},files});
  assert.deepEqual(inventoryOnly.outsideSuiteInventory.oracleManifests[0].unifiedSuite,
    {id:'ports-oracle',targetDeclared:true,targetRoot:'ports/th09/web',registeredProfiles:['oracle'],currentRunSelection:null},
  'profile membership without an explicit selection is not a current execution claim');
});

test('a local manifest without a matching ports-oracle target is not reported as suite-registered',async t=>{
  const fixture=await oracleFixture(t),catalog=oracleCatalog(fixture.root,['th11']);
  const report=await buildExecutionOwnership({root:fixture.root,catalog,
    profileSelections:{oracle:['ports-oracle'],selection:['ports-oracle']},files:fixtureFiles(fixture)});
  const item=report.outsideSuiteInventory.oracleManifests[0];
  assert.equal(item.candidateFiles,1);
  assert.equal(report.outsideSuiteInventory.oracleManifestOwned,2);
  assert.deepEqual(item.unifiedSuite,
    {id:'ports-oracle',targetDeclared:false,targetRoot:null,registeredProfiles:[],currentRunSelection:false});
});

test('the actual oracle manifest loader rejects unregistered, duplicate, and escaped test paths',async t=>{
  const missing=await oracleFixture(t,{discovered:['candidate.test.mjs','unlisted.test.mjs'],tests:[
    {file:'candidate.test.mjs',role:'candidate',group:'unit'},
  ]});
  await assert.rejects(buildExecutionOwnership({root:missing.root,catalog:oracleCatalog(missing.root),
    profileSelections:{oracle:['ports-oracle']},files:fixtureFiles(missing)}),/classification mismatch.*unclassified=unlisted\.test\.mjs/);

  const duplicate=await oracleFixture(t,{tests:[
    {file:'candidate.test.mjs',role:'candidate',group:'unit'},
    {file:'candidate.test.mjs',role:'candidate',group:'unit'},
  ]});
  await assert.rejects(buildExecutionOwnership({root:duplicate.root,catalog:oracleCatalog(duplicate.root),
    profileSelections:{oracle:['ports-oracle']},files:fixtureFiles(duplicate)}),/Invalid or duplicate test classification/);

  const escaped=await oracleFixture(t,{tests:[
    {file:'../outside.test.mjs',role:'candidate',group:'unit'},
  ]});
  await assert.rejects(buildExecutionOwnership({root:escaped.root,catalog:oracleCatalog(escaped.root),
    profileSelections:{oracle:['ports-oracle']},files:fixtureFiles(escaped)}),/classification mismatch.*absent=\.\.\/outside\.test\.mjs/);
});
