import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {copyFile,mkdir,mkdtemp,readFile,rm,writeFile} from 'node:fs/promises';
import {statSync,utimesSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {dirname,relative,resolve} from 'node:path';
import {
  RUNTIME_BUILD_CONTROL_FILES,RUNTIME_BUILD_HELPER_FILES,captureRuntimeBuildSources,runtimeBuildInputIdentity,runtimeBuildReceipt,
  expectedRuntimeBuildOutputs,executeRuntimeSourceBuild,runtimeBuildCanReuseCmakeNoop,runtimeBuildPlanIdentity,
} from '../lib/runtime-build-evidence.mjs';
import {
  resolveRuntimeBuildSubmodules,runtimeSubmoduleInitCommands,
} from '../lib/runtime-build-dependencies.mjs';

const tempRoot=await mkdtemp(resolve(tmpdir(),'eagler-runtime-dependencies-'));
const repository=resolve(tempRoot,'repo');
const source=resolve(repository,'ports/th06');
const nodeSource=resolve(repository,'ports/th20');
const common=resolve(repository,'common');
const project=resolve(import.meta.dirname,'../..');
const declarations=[
  {path:'vendored/SDL'},
  {path:'vendored/SDL_image',requiredSubmodules:[{path:'external/libpng'},{path:'external/zlib'}]},
  {path:'vendored/SDL_ttf',requiredSubmodules:[
    {path:'external/freetype'},{path:'external/plutovg'},{path:'external/plutosvg'},
  ]},
];

async function put(path,text='fixture\n') {
  await mkdir(dirname(path),{recursive:true});
  await writeFile(path,text);
}
function git(cwd,...args) {
  return execFileSync('git',args,{cwd,encoding:'utf8',windowsHide:true,stdio:['ignore','pipe','pipe']}).trim();
}
function gitInput(cwd,args,input) {
  return execFileSync('git',args,{cwd,encoding:'utf8',windowsHide:true,input,stdio:['pipe','pipe','pipe']}).trim();
}
async function initRepo(path,file='module.c') {
  await mkdir(path,{recursive:true});
  git(path,'init','--quiet');
  git(path,'config','user.name','Runtime dependency test');
  git(path,'config','user.email','runtime-dependency@example.invalid');
  git(path,'config','commit.gpgsign','false');
  await put(resolve(path,file),'int runtime_module_fixture;\n');
  git(path,'add',file);
  git(path,'commit','--quiet','-m','fixture module');
  return git(path,'rev-parse','HEAD');
}
function pin(owner,path,commit) {
  git(owner,'update-index','--add','--cacheinfo',`160000,${commit},${path}`);
}
function repoPath(path) {
  return relative(repository,path).replaceAll('\\','/');
}
function plan() {
  return {source,common,game:'th06',variant:'normal',requiredSubmoduleDeclarations:declarations,
    environment:{EAGLER_COMMON_NETPLAY_ROOT:common}};
}
function identity() {
  return runtimeBuildInputIdentity(plan(),repository);
}
function identityFor(value) {
  return runtimeBuildInputIdentity(value,repository);
}
function fileHash(files,path) {
  return files.find(file=>file.path===repoPath(path))?.sha256;
}

try {
  await mkdir(repository,{recursive:true});
  git(repository,'init','--quiet');
  git(repository,'config','user.name','Runtime dependency test');
  git(repository,'config','user.email','runtime-dependency@example.invalid');
  git(repository,'config','commit.gpgsign','false');
  await put(resolve(source,'main.cpp'),'int runtime_owner_fixture;\n');
  await put(resolve(source,'CMakeLists.txt'),'cmake_minimum_required(VERSION 3.20)\n');
  await put(resolve(nodeSource,'main.cpp'),'int node_runtime_owner_fixture;\n');
  await put(resolve(common,'include/common.hpp'),'#pragma once\n');
  await put(resolve(repository,'.gitignore'),'ports/th06/build/\nports/th20/web/artifacts/\n');
  git(repository,'add','ports/th06/main.cpp','ports/th06/CMakeLists.txt','ports/th20/main.cpp','common/include/common.hpp','.gitignore');
  git(repository,'commit','--quiet','-m','fixture owner sources');
  for(const name of [...RUNTIME_BUILD_CONTROL_FILES,...RUNTIME_BUILD_HELPER_FILES]) {
    if(['launcher/config/runtime-builds.json','launcher/config/workspace.json'].includes(name)) {
      await mkdir(dirname(resolve(repository,name)),{recursive:true});
      await copyFile(resolve(project,name),resolve(repository,name));
    } else await put(resolve(repository,name),`control fixture: ${name}\n`);
  }

  const sdl=resolve(source,'vendored/SDL');
  const image=resolve(source,'vendored/SDL_image');
  const ttf=resolve(source,'vendored/SDL_ttf');
  const sdlCommit=await initRepo(sdl);
  const png=resolve(image,'external/libpng'),zlib=resolve(image,'external/zlib');
  const pngCommit=await initRepo(png,'png.c'),zlibCommit=await initRepo(zlib,'zlib.c');
  const optionalGitlink='external/libjxl';
  await mkdir(image,{recursive:true});
  git(image,'init','--quiet');
  git(image,'config','user.name','Runtime dependency test');
  git(image,'config','user.email','runtime-dependency@example.invalid');
  git(image,'config','commit.gpgsign','false');
  await put(resolve(image,'image.c'),'int image_fixture;\n');
  git(image,'add','image.c');
  git(image,'commit','--quiet','-m','fixture image module');
  pin(image,'external/libpng',pngCommit);
  pin(image,'external/zlib',zlibCommit);
  pin(image,optionalGitlink,pngCommit);
  git(image,'commit','--quiet','-m','pin image dependencies');
  const imageCommit=git(image,'rev-parse','HEAD');
  const freetype=resolve(ttf,'external/freetype'),plutovg=resolve(ttf,'external/plutovg'),plutosvg=resolve(ttf,'external/plutosvg');
  await initRepo(ttf);
  const freetypeCommit=await initRepo(freetype,'freetype.c');
  const plutovgCommit=await initRepo(plutovg,'plutovg.c');
  const plutosvgCommit=await initRepo(plutosvg,'plutosvg.c');
  pin(ttf,'external/freetype',freetypeCommit);
  pin(ttf,'external/plutovg',plutovgCommit);
  pin(ttf,'external/plutosvg',plutosvgCommit);
  git(ttf,'commit','--quiet','-m','pin font dependencies');
  const finalTtfCommit=git(ttf,'rev-parse','HEAD');
  const topPins=[
    ['vendored/SDL',sdlCommit],['vendored/SDL_image',imageCommit],['vendored/SDL_ttf',finalTtfCommit],
  ];
  for(const [path,commit] of topPins)pin(repository,`ports/th06/${path}`,commit);

  const initialModules=resolveRuntimeBuildSubmodules({source,repository,declarations});
  assert.equal(initialModules.length,8);
  assert.deepEqual(initialModules.map(module=>module.path),[
    'ports/th06/vendored/SDL','ports/th06/vendored/SDL_image','ports/th06/vendored/SDL_image/external/libpng',
    'ports/th06/vendored/SDL_image/external/zlib','ports/th06/vendored/SDL_ttf',
    'ports/th06/vendored/SDL_ttf/external/freetype','ports/th06/vendored/SDL_ttf/external/plutosvg',
    'ports/th06/vendored/SDL_ttf/external/plutovg',
  ]);
  const commands=runtimeSubmoduleInitCommands({source,declarations});
  assert.equal(commands.length,3);
  assert.deepEqual(commands.map(command=>command.args),[
    ['submodule','update','--init','--','vendored/SDL','vendored/SDL_image','vendored/SDL_ttf'],
    ['submodule','update','--init','--','external/libpng','external/zlib'],
    ['submodule','update','--init','--','external/freetype','external/plutovg','external/plutosvg'],
  ]);
  assert(commands.every(command=>!command.args.includes('--recursive')));

  const initialIdentity=identity();
  const dependencyPlan={...plan(),execution:resolve(repository,'ports/th06/build/web/normal/th06.wasm'),requiredSubmodules:initialIdentity.requiredSubmodules,
    features:{thcrap:false,thprac:false},toolchain:{compiler:'fixture-compiler',version:'fixture'},
    steps:[{command:process.execPath,args:['-e',''],cwd:repository}]};
  for(const output of Object.values(expectedRuntimeBuildOutputs('th06','normal',repository)))
    await put(resolve(repository,output.path),output.path.endsWith('.wasm')?'\0asm\x01\0\0\0':'generated CMake Runtime output\n');
  const initialSourceFiles=captureRuntimeBuildSources(dependencyPlan,repository);
  const dependencyReceipt=runtimeBuildReceipt(dependencyPlan,repository,initialSourceFiles);
  assert.equal(dependencyReceipt.schema,'eagler-touhou/runtime-source-build/3');
  assert.deepEqual(dependencyReceipt.requiredSubmodules,initialIdentity.requiredSubmodules);
  const {runtimeDependencyInputs,sourceIdentity}=await import('../../tools/ports/inputs.mjs');
  const statusInputs=runtimeDependencyInputs(initialIdentity.requiredSubmodules,initialSourceFiles,repository);
  const statusDigest=inputs=>sourceIdentity(inputs.inputFiles,{dependencies:inputs.dependencies});
  const initialStatusDigest=statusDigest(statusInputs);
  const optionalStatusRepo=resolve(tempRoot,'optional-libjxl');
  const optionalCommit=await initRepo(optionalStatusRepo,'jxl.c');
  git(image,'update-index','--cacheinfo',`160000,${optionalCommit},${optionalGitlink}`);
  const optionalGitlinkSources=captureRuntimeBuildSources(dependencyPlan,repository);
  assert.equal(statusDigest(runtimeDependencyInputs(initialIdentity.requiredSubmodules,optionalGitlinkSources,repository)),
    initialStatusDigest,'an unused optional gitlink change does not alter status identity');
  git(image,'update-index','--cacheinfo',`160000,${pngCommit},${optionalGitlink}`);
  const pngSource=resolve(png,'png.c'),originalPng=await readFile(pngSource);
  await writeFile(pngSource,Buffer.concat([originalPng,Buffer.from('changed required PNG source\n')]));
  const changedPngSources=captureRuntimeBuildSources(dependencyPlan,repository);
  assert.notEqual(statusDigest(runtimeDependencyInputs(initialIdentity.requiredSubmodules,changedPngSources,repository)),
    initialStatusDigest,'a required nested source change alters status identity');
  await writeFile(pngSource,originalPng);

  // Repeated output lifecycle cases need one real pinned dependency. The
  // complete eight-module closure is exercised independently above and below.
  const cmakePlan={...dependencyPlan,requiredSubmoduleDeclarations:[declarations[0]],
    requiredSubmodules:initialIdentity.requiredSubmodules.filter(module=>module.path==='ports/th06/vendored/SDL')};
  assert.equal(cmakePlan.requiredSubmodules.length,1);
  const cmakeReceipt=runtimeBuildReceipt(cmakePlan,repository,captureRuntimeBuildSources(cmakePlan,repository));
  assert.deepEqual(Object.keys(cmakeReceipt.outputs),['execution','loader','entry']);

  const mutationModePath=resolve(tempRoot,'cmake-output-mode.txt');
  const mutationOutputs=expectedRuntimeBuildOutputs('th06','normal',repository);
  const mutationPaths=Object.fromEntries(Object.entries(mutationOutputs).map(([role,output])=>[role,resolve(repository,output.path)]));
  const originalOutputBytes=Object.fromEntries(await Promise.all(Object.entries(mutationPaths).map(async([role,path])=>[role,await readFile(path)])));
  const mutationScript=`const fs=require('node:fs');const mode=fs.readFileSync(${JSON.stringify(mutationModePath)},'utf8');`+
    `const paths=${JSON.stringify(mutationPaths)};`+
    `if(mode==='compile'){fs.writeFileSync(paths.execution,Buffer.from([0,97,115,109,1,0,0,0,9]));`+
    `fs.writeFileSync(paths.loader,'baseline loader bytes');fs.writeFileSync(paths.entry,'baseline HTML bytes');}`+
    `else if(mode==='entry-only'){fs.writeFileSync(paths.entry,'changed HTML bytes');}`;
  const mutationPlan={...cmakePlan,steps:[{command:process.execPath,args:['-e',mutationScript],cwd:repository}]};
  await writeFile(mutationModePath,'compile');
  const mutationBaseline=await executeRuntimeSourceBuild(mutationPlan,repository);
  await writeFile(mutationModePath,'entry-only');
  await assert.rejects(executeRuntimeSourceBuild(mutationPlan,repository,
    {previousReceipt:mutationBaseline,previousPlan:mutationPlan}),/did not refresh code outputs/,
  'a same-plan step changing only HTML cannot reuse its pre-step output receipt');
  for(const [role,path] of Object.entries(mutationPaths))await writeFile(path,originalOutputBytes[role]);

  const currentControl=resolve(repository,'launcher/scripts/build-runtime.mjs');
  const originalControlBytes=await readFile(currentControl);
  await writeFile(currentControl,Buffer.concat([originalControlBytes,Buffer.from('\ncontrol-only update\n')]));
  const previousPlan={...cmakePlan};
  const emptyExclusionsPlan={...cmakePlan,sourceExclusions:[]};
  const previousReceipt=cmakeReceipt;
  const refreshedControlReceipt=await executeRuntimeSourceBuild(emptyExclusionsPlan,repository,{previousReceipt,previousPlan});
  assert.notEqual(refreshedControlReceipt.sourceFiles.find(file=>file.path===repoPath(currentControl)).sha256,
    previousReceipt.sourceFiles.find(file=>file.path===repoPath(currentControl)).sha256,
    'a CMake control-only change is recorded in the new full receipt');
  assert.notEqual(refreshedControlReceipt.planSha256,previousReceipt.planSha256,
    'the new receipt binds its current complete plan');
  assert.deepEqual(refreshedControlReceipt.outputs,previousReceipt.outputs,
    'a CMake no-op reuse preserves every exact output hash');
  const reusePlan=emptyExclusionsPlan,reuseReceipt=refreshedControlReceipt;
  const canReuse=(currentReceipt=reuseReceipt,currentPlan=reusePlan,oldReceipt=reuseReceipt,oldPlan=reusePlan,recipeKind='cmake')=>
    runtimeBuildCanReuseCmakeNoop({recipeKind,previousReceipt:oldReceipt,previousPlan:oldPlan,currentReceipt,currentPlan});
  for(const [field,value] of [
    ['schema','eagler-touhou/runtime-source-build/2'],['profile','development'],['game','th07'],['variant','multiplayer'],
  ]) assert.equal(canReuse({...reuseReceipt,[field]:value}),false,'prior receipt '+field+' mismatch rejects reuse');
  for(const [field,value] of [
    ['schema','eagler-touhou/runtime-source-build/2'],['profile','development'],['game','th07'],['variant','multiplayer'],
  ]) assert.equal(canReuse(reuseReceipt,reusePlan,{...reuseReceipt,[field]:value},reusePlan),false,
    'current receipt '+field+' mismatch rejects reuse');
  assert.equal(canReuse({...reuseReceipt,sourceFiles:null}),false,'null source inputs reject reuse');
  assert.equal(canReuse({...reuseReceipt,sourceFiles:[]}),false,'empty source inputs reject reuse');
  const forgedOldPlan={...reusePlan,environment:{...reusePlan.environment,TH_FORGED_PLAN:'1'}};
  assert.equal(canReuse(reuseReceipt,reusePlan,reuseReceipt,forgedOldPlan),false,'receipt planSha must bind the supplied old plan');
  assert.equal(canReuse(reuseReceipt,reusePlan,reuseReceipt,reusePlan,'node'),false,'the relaxed branch is CMake-only');
  const oldExcludedPlan={...reusePlan,sourceExclusions:['old-exclusion']};
  const oldExcludedReceipt={...reuseReceipt,planSha256:runtimeBuildPlanIdentity(oldExcludedPlan)};
  assert.equal(canReuse(reuseReceipt,reusePlan,oldExcludedReceipt,oldExcludedPlan),false,
    'a nonempty exclusion on only the previous plan disables relaxed reuse');
  const currentExcludedPlan={...reusePlan,sourceExclusions:['current-exclusion']};
  const currentExcludedReceipt={...reuseReceipt,planSha256:runtimeBuildPlanIdentity(currentExcludedPlan)};
  assert.equal(canReuse(currentExcludedReceipt,currentExcludedPlan),false,
    'a nonempty exclusion on the current plan disables relaxed reuse');

  const sourceBytesPath=resolve(source,'main.cpp');
  const sourceBytes=await readFile(sourceBytesPath),sourceStat=statSync(sourceBytesPath);
  await writeFile(sourceBytesPath,Buffer.concat([sourceBytes,Buffer.from('\\nchanged with preserved mtime\\n')]));
  utimesSync(sourceBytesPath,sourceStat.atime,sourceStat.mtime);
  const preservedMtimeInputs=captureRuntimeBuildSources(reusePlan,repository);
  assert.equal(canReuse({...reuseReceipt,sourceFiles:preservedMtimeInputs}),false,
    'source byte changes invalidate reuse when the filesystem timestamp is preserved');
  await writeFile(sourceBytesPath,sourceBytes);utimesSync(sourceBytesPath,sourceStat.atime,sourceStat.mtime);
  for(const path of ['ports/th06/CMakeLists.txt','common/include/common.hpp']) {
    const changedFiles=reuseReceipt.sourceFiles.map(file=>file.path===path?{...file,sha256:'f'.repeat(64)}:file);
    assert.notDeepEqual(changedFiles,reuseReceipt.sourceFiles,'fixture includes compiler input '+path);
    assert.equal(canReuse({...reuseReceipt,sourceFiles:changedFiles}),false,path+' remains a compiler input');
  }
  for(const role of ['loader','entry','execution']) {
    const digest='0'.repeat(64),outputs={...reuseReceipt.outputs,[role]:{...reuseReceipt.outputs[role],sha256:digest}};
    const changedReceipt={...reuseReceipt,outputs,...(role==='execution'?{executionSha256:digest}:{})};
    assert.equal(canReuse(changedReceipt),false,'changed '+role+' output digest rejects reuse');
  }
  const changedPins=reuseReceipt.requiredSubmodules.map((module,index)=>index===0?{...module,commit:'f'.repeat(40)}:module);
  const changedPinPlan={...reusePlan,requiredSubmodules:changedPins};
  const changedPinReceipt={...reuseReceipt,requiredSubmodules:changedPins,planSha256:runtimeBuildPlanIdentity(changedPinPlan)};
  assert.equal(canReuse(changedPinReceipt,changedPinPlan),false,'changed required submodule pin rejects reuse');
  assert.equal(canReuse({...reuseReceipt,requiredSubmodules:changedPins}),false,'receipt pins must match its plan');
  const flagPlan={...reusePlan,steps:[{...reusePlan.steps[0],args:['-e','','--','-DTH_TEST_FLAG=changed']}]} ;
  assert.equal(canReuse({...reuseReceipt,planSha256:runtimeBuildPlanIdentity(flagPlan)},flagPlan),false,
    'compiler flag changes reject reuse');
  const environmentPlan={...reusePlan,environment:{...reusePlan.environment,TH_TEST_ENVIRONMENT:'changed'}};
  assert.equal(canReuse({...reuseReceipt,planSha256:runtimeBuildPlanIdentity(environmentPlan)},environmentPlan),false,
    'compiler environment changes reject reuse');
  const featurePlan={...reusePlan,features:{...reusePlan.features,thprac:true}};
  assert.equal(canReuse({...reuseReceipt,planSha256:runtimeBuildPlanIdentity(featurePlan)},featurePlan),false,
    'compiler feature changes reject reuse');
  const trueStepPlan={...reusePlan,steps:[{command:process.execPath,args:['-e','process.exit(0)'],cwd:repository}]};
  await assert.rejects(executeRuntimeSourceBuild(trueStepPlan,repository,{previousReceipt:reuseReceipt,previousPlan:reusePlan}),
    /did not refresh code outputs/,'a successful replacement step cannot masquerade as the recorded compiler');

  const configPath=resolve(repository,'launcher/config/runtime-builds.json'),configBytes=await readFile(configPath);
  const config=JSON.parse(configBytes);config.games.th06.sourceBuild.sourceExclusions=['unused-fixture-inputs'];
  await writeFile(configPath,JSON.stringify(config));
  const nonemptyPlan={...reusePlan,sourceExclusions:['unused-fixture-inputs']};
  const nonemptyReceipt=runtimeBuildReceipt(nonemptyPlan,repository,captureRuntimeBuildSources(nonemptyPlan,repository));
  const exactReuse=await executeRuntimeSourceBuild(nonemptyPlan,repository,{previousReceipt:nonemptyReceipt,previousPlan:nonemptyPlan});
  assert.deepEqual(exactReuse.outputs,nonemptyReceipt.outputs,'strict exact reuse remains available with nonempty exclusions');
  const changedControl=await readFile(currentControl);
  await writeFile(currentControl,Buffer.concat([changedControl,Buffer.from('\nsecond control update\n')]));
  await assert.rejects(executeRuntimeSourceBuild(nonemptyPlan,repository,{previousReceipt:nonemptyReceipt,previousPlan:nonemptyPlan}),
    /did not refresh code outputs/,'relaxed reuse is disabled when either plan has nonempty sourceExclusions');
  await writeFile(currentControl,originalControlBytes);await writeFile(configPath,configBytes);

  const nodePlan={...cmakePlan,game:'th20',source:nodeSource,execution:resolve(nodeSource,'web/artifacts/sdl3/th20-sdl.wasm'),
    requiredSubmoduleDeclarations:[],requiredSubmodules:[],environment:{EAGLER_COMMON_NETPLAY_ROOT:common},
    steps:[{command:process.execPath,args:['-e',''],cwd:nodeSource}]};
  const nodeIdentity=identityFor(nodePlan);
  const unrelatedMetadata=resolve(repository,'ports/th06/build/web/normal/build.json');
  const onlyMetadataPlan={...cmakePlan,steps:[{command:process.execPath,
    args:['-e',`require('node:fs').writeFileSync(${JSON.stringify(unrelatedMetadata)},'metadata only');`],cwd:repository}]};
  await assert.rejects(executeRuntimeSourceBuild(onlyMetadataPlan,repository),/did not refresh code outputs/,
    'CMake metadata cannot substitute for refreshing the execution WASM');
  const htmlPath=expectedRuntimeBuildOutputs('th06','normal',repository).entry.path;
  const htmlRefreshPlan={...cmakePlan,steps:[{command:process.execPath,
    args:['-e',`require('node:fs').writeFileSync(${JSON.stringify(resolve(repository,htmlPath))},'refreshed CMake entry');`],cwd:repository}]};
  await assert.rejects(executeRuntimeSourceBuild(htmlRefreshPlan,repository),/did not refresh code outputs/,
    'a changed CMake entry cannot certify an existing execution WASM');
  const wasmPath=resolve(repository,expectedRuntimeBuildOutputs('th06','normal',repository).execution.path);
  const actualCodeRefreshPlan={...htmlRefreshPlan,steps:[{command:process.execPath,args:['-e',
    `const fs=require('node:fs');fs.writeFileSync(${JSON.stringify(wasmPath)},fs.readFileSync(${JSON.stringify(wasmPath)}));`],cwd:repository}]};
  const refreshedCmakeReceipt=await executeRuntimeSourceBuild(actualCodeRefreshPlan,repository);
  assert.notEqual(refreshedCmakeReceipt.outputs.entry.sha256,cmakeReceipt.outputs.entry.sha256,
    'a refreshed execution records the matching CMake HTML output');
  for(const output of Object.values(expectedRuntimeBuildOutputs('th20','normal',repository)))
    await put(resolve(repository,output.path),output.path.endsWith('.wasm')?'\0asm\x01\0\0\0':'generated Node Runtime output\n');
  const nodeSourceFiles=captureRuntimeBuildSources(nodePlan,repository);
  const nodeReceipt=runtimeBuildReceipt(nodePlan,repository,nodeSourceFiles);
  assert.equal(nodeReceipt.schema,'eagler-touhou/runtime-source-build/3');
  assert.deepEqual(nodeReceipt.requiredSubmodules,[]);
  assert.deepEqual(Object.keys(nodeReceipt.outputs),['execution','loader','buildManifest']);
  const optional=resolve(image,optionalGitlink);
  await mkdir(optional,{recursive:true});
  assert.deepEqual(identity(),initialIdentity,'an empty optional gitlink does not change the build identity');
  await rm(optional,{recursive:true,force:true});
  await initRepo(optional,'optional.c');
  await writeFile(resolve(optional,'optional.c'),'int changed_optional_fixture;\n');
  git(optional,'add','optional.c');
  git(optional,'commit','--quiet','-m','advance optional checkout');
  assert.deepEqual(identity(),initialIdentity,'optional content and HEAD changes do not change the build identity');
  await rm(optional,{recursive:true,force:true});
  await mkdir(optional,{recursive:true});
  await writeFile(resolve(optional,'.git'),'damaged optional checkout\n');
  assert.deepEqual(identity(),initialIdentity,'a damaged optional gitlink does not change the build identity');
  await rm(optional,{recursive:true,force:true});

  const beforePng=fileHash(initialSourceFiles,resolve(png,'png.c'));
  await writeFile(resolve(png,'png.c'),'int changed_required_png_fixture;\n');
  const changed=identity();
  const changedSourceFiles=captureRuntimeBuildSources(dependencyPlan,repository);
  assert.notEqual(fileHash(changedSourceFiles,resolve(png,'png.c')),beforePng,'required dirty source content changes the receipt input hash');
  assert.notDeepEqual(changed,initialIdentity,'required source changes invalidate the build identity');

  await rm(resolve(source,'main.cpp'));
  assert.throws(()=>identity(),/Tracked or discovered Runtime build input is missing/);
  await put(resolve(source,'main.cpp'),'int runtime_owner_fixture_restored;\n');

  const missingPlutovg=resolve(ttf,'external/plutovg');
  await rm(missingPlutovg,{recursive:true,force:true});
  assert.throws(()=>identity(),/Required submodule checkout is missing/);
  await initRepo(missingPlutovg,'plutovg.c');
  pin(ttf,'external/plutovg',git(missingPlutovg,'rev-parse','HEAD'));
  git(ttf,'commit','--quiet','-m','restore font dependency pin');
  pin(repository,'ports/th06/vendored/SDL_ttf',git(ttf,'rev-parse','HEAD'));

  await put(resolve(source,'conflict.c'),'int conflicted_runtime_fixture;\n');
  const conflictBlob=git(repository,'hash-object','-w','ports/th06/conflict.c');
  gitInput(repository,['update-index','--index-info'],[
    `100644 ${conflictBlob} 1\tports/th06/conflict.c`,
    `100644 ${conflictBlob} 2\tports/th06/conflict.c`,
    `100644 ${conflictBlob} 3\tports/th06/conflict.c`,
  ].join('\n')+'\n');
  assert.throws(()=>identity(),/Unresolved Git index conflict in Runtime build input/);

  const oldZlibCommit=zlibCommit;
  await writeFile(resolve(zlib,'new-source.c'),'int new_zlib_source;\n');
  git(zlib,'add','new-source.c');
  git(zlib,'commit','--quiet','-m','move required module HEAD');
  assert.throws(()=>identity(),new RegExp(`commit mismatch at .*expected ${oldZlibCommit}`));
} finally {
  assert.equal(dirname(tempRoot),resolve(tmpdir()));
  await rm(tempRoot,{recursive:true,force:true});
}
console.log('Runtime build dependencies: PASS (pins, non-recursive init, optional isolation, mutation and missing-input controls)');
