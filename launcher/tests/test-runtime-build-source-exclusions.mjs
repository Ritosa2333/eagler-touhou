import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {copyFile,mkdir,mkdtemp,readFile,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {dirname,resolve} from 'node:path';
import {planRuntimeSourceBuild} from '../lib/runtime-source-plan.mjs';
import {RUNTIME_BUILD_CONTROL_FILES,RUNTIME_BUILD_HELPER_FILES,expectedRuntimeBuildSources,
  captureRuntimeBuildSources,runtimeBuildInputIdentity,runtimeBuildPlanIdentity,runtimeBuildSourceFilesSha256} from '../lib/runtime-build-evidence.mjs';

const project=resolve(import.meta.dirname,'../..');
const hashFiles=files=>runtimeBuildSourceFilesSha256(files);
const temp=await mkdtemp(resolve(tmpdir(),'runtime-source-exclusions-'));
const fixture=resolve(temp,'repo');
const copyPaths=new Set([...RUNTIME_BUILD_CONTROL_FILES,...RUNTIME_BUILD_HELPER_FILES,
  'launcher/config/runtime-builds.json','launcher/config/workspace.json',
  'launcher/lib/contracts/load-compiled-contract.mjs']);
const put=async(path,contents)=>{const target=resolve(fixture,path);await mkdir(dirname(target),{recursive:true});await writeFile(target,contents);return target;};
const git=(...args)=>execFileSync('git',args,{cwd:fixture,encoding:'utf8',windowsHide:true}).trim();
try {
  for(const path of copyPaths){const destination=resolve(fixture,path);await mkdir(dirname(destination),{recursive:true});await copyFile(resolve(project,path),destination);}
  const buildConfig=JSON.parse(await readFile(resolve(project,'launcher/config/runtime-builds.json'),'utf8'));
  const workspace=JSON.parse(await readFile(resolve(project,'launcher/config/workspace.json'),'utf8'));
  await put('launcher/config/runtime-builds.json',JSON.stringify(buildConfig,null,2)+'\n');
  await put('launcher/config/workspace.json',JSON.stringify(workspace,null,2)+'\n');
  const sourceOwners=[
    {owner:'th09',game:'th09',builder:'web/scripts/build-sdl.mjs'},
    {owner:'th11',game:'th11',builder:'portable/build.mjs'},
    {owner:'th11mp',game:'th11',builder:'portable/build.mjs'},
    {owner:'th15',game:'th15',builder:'web/scripts/build-sdl-application.mjs'},
    {owner:'th15mp',game:'th15',builder:'web/scripts/build-sdl-application.mjs'},
  ];
  for(const item of sourceOwners){
    const owner=workspace.repositories[item.owner];
    await put(`${owner}/web/cpp/game/runtime.cpp`,`int ${item.game}_game(){return 1;}\n`);
    await put(`${owner}/web/cpp/sdl/runtime.cpp`,`int ${item.game}_sdl(){return 1;}\n`);
    await put(`${owner}/${item.builder}`,'// formal production builder control\n');
    await put(`${owner}/web/tests/cpp/core-exports.cpp`,`int ${item.game}_test_export(){return 1;}\n`);
    await put(`${owner}/web/tests/cpp/oracle-suite.json`,'{"groups":[]}\n');
  }
  await put(`${workspace.repositories.th15}/web/tests/sdl/application-exports.cpp`,'int diagnostic_export(){return 1;}\n');
  const th09Owner=workspace.repositories.th09;
  await put(`${th09Owner}/web/tests/sdl/application-exports.cpp`,'int diagnostic_export(){return 1;}\n');
  await put('common/include/eagler/input/DialogueGesture.hpp','#pragma once\n');
  await put('common/include/eagler/netplay/SessionChannel.hpp','#pragma once\n');
  await put('common/include/eagler/text/RuntimeFontRaster.hpp','#pragma once\n');
  git('init','--quiet');git('config','core.autocrlf','false');git('add','--all');

  const layout={root:fixture,path:(name,...segments)=>resolve(fixture,workspace.repositories[name],...segments)};
  const sdk=resolve(temp,'emsdk'),compiler=resolve(sdk,'install/emscripten/emcc.py');
  await mkdir(dirname(compiler),{recursive:true});await writeFile(compiler,'');
  await writeFile(resolve(dirname(compiler),'emscripten-version.txt'),'3.1.68\n');
  const formalPlans=[];
  for(const [game,variant] of [['th09','normal'],['th09','multiplayer'],['th11','normal'],
      ['th11','multiplayer'],['th15','normal'],['th15','multiplayer']]){
    const plan=await planRuntimeSourceBuild({game,variant,emsdk:sdk,jobs:2},{environment:{},layout,config:buildConfig});
    assert.deepEqual(plan.sourceExclusions,['web/tests'],`${game}/${variant} planner carries its recipe exclusion`);
    if(game==='th15')assert.ok(plan.steps[0].args.includes('--release'),`${game}/${variant} formal recipe remains production release`);
    const expected=expectedRuntimeBuildSources(game,variant,fixture);
    assert.equal(hashFiles(expected),runtimeBuildInputIdentity(plan,fixture).sourceFilesSha256,
      `${game}/${variant} producer and expected inventories share the same exclusion strategy`);
    const owner=workspace.repositories[game+(variant==='multiplayer'?'mp':'')];
    assert.ok(!expected.some(file=>file.path.startsWith(`${owner}/web/tests/`)),`${game}/${variant} excludes owner web/tests`);
    formalPlans.push(plan);
  }
  const plan=formalPlans[0],owner=workspace.repositories.th09;
  const initial=runtimeBuildInputIdentity(plan,fixture);
  assert.deepEqual(initial.sourceExclusions,['web/tests']);
  const initialPlanIdentity=runtimeBuildPlanIdentity(plan);
  assert.notEqual(initialPlanIdentity,runtimeBuildPlanIdentity({...plan,sourceExclusions:[]}),
    'the recipe exclusion strategy is bound into the plan identity');

  await writeFile(resolve(fixture,owner,'web/tests/cpp/core-exports.cpp'),'int changed_test_export(){return 9;}\n');
  await writeFile(resolve(fixture,owner,'web/tests/cpp/oracle-suite.json'),'{"groups":[{"id":"changed"}]}\n');
  git('add','--all');
  assert.deepEqual(runtimeBuildInputIdentity(plan,fixture),initial,
    'test exports and native oracle policy are outside the production compiler closure');
  const newTest=resolve(fixture,owner,'web/tests/cpp/new-contract.cpp');
  await writeFile(newTest,'int new_test_contract(){return 2;}\n');
  assert.deepEqual(runtimeBuildInputIdentity(plan,fixture),initial,'new test inventory files do not change production identity');
  await rm(newTest);

  for(const path of [`${owner}/web/cpp/game/runtime.cpp`,`${owner}/web/cpp/sdl/runtime.cpp`,
      `${owner}/web/scripts/build-sdl.mjs`,'launcher/scripts/build-runtime.mjs']){
    const file=resolve(fixture,path),before=await readFile(file);
    await writeFile(file,Buffer.concat([before,Buffer.from('\n// changed build input\n')]));
    assert.notDeepEqual(runtimeBuildInputIdentity(plan,fixture),initial,`${path} must invalidate production identity`);
    await writeFile(file,before);
  }
  assert.throws(()=>runtimeBuildInputIdentity({...plan,sourceExclusions:[]},fixture),/source exclusions changed/,
    'a saved plan cannot remove the recipe strategy or widen its inventory');

  const diagnosticPlan={source:resolve(fixture,'ports/th15'),common:resolve(fixture,'common'),
    requiredSubmoduleDeclarations:[],requiredSubmodules:[],sourceExclusions:[],
    environment:{EAGLER_COMMON_NETPLAY_ROOT:resolve(fixture,'common')}};
  const diagnosticBefore=captureRuntimeBuildSources(diagnosticPlan,fixture);
  assert.ok(diagnosticBefore.some(file=>file.path==='ports/th15/web/tests/sdl/application-exports.cpp'),
    'a direct diagnostic source plan without production exclusions includes the SDL diagnostic source');
  await writeFile(resolve(fixture,'ports/th15/web/tests/sdl/application-exports.cpp'),'int diagnostic_export(){return 2;}\n');
  assert.notEqual(hashFiles(captureRuntimeBuildSources(diagnosticPlan,fixture)),hashFiles(diagnosticBefore),
    'diagnostic test sources stay bound when that source plan consumes them');

  for(const game of ['th09','th11','th15'])
    assert.deepEqual(buildConfig.games[game].sourceBuild.sourceExclusions,['web/tests'],`${game} owns the exact production exclusion`);
  for(const game of ['th06','th07','th08','th10','th20'])
    assert.equal(buildConfig.games[game].sourceBuild.sourceExclusions,undefined,`${game} gets no global exclusion`);
  console.log(JSON.stringify({status:'passed',planIdentityBound:true,formalVariants:formalPlans.length,
    excluded:['web/tests/cpp/core-exports.cpp','web/tests/cpp/oracle-suite.json','web/tests/new-contract.cpp'],
    stillBound:['web/cpp/game/runtime.cpp','web/cpp/sdl/runtime.cpp','web/scripts/build-sdl.mjs','launcher/scripts/build-runtime.mjs'],
    diagnosticTestSourceIncluded:true}));
} finally {assert.equal(dirname(temp),resolve(tmpdir()));await rm(temp,{recursive:true,force:true});}
