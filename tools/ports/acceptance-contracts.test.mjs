import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtemp,rm,mkdir,readFile,writeFile,copyFile} from 'node:fs/promises';
import {join,resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {execFileSync} from 'node:child_process';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {canonicalRuntimePayload} from '../../launcher/lib/contracts/runtime-generations.mjs';
import {expectedRuntimeBuildSources,expectedRuntimeBuildSubmodules,expectedRuntimeBuildOutputs,runtimeBuildPlanIdentity,RUNTIME_BUILD_CONTROL_FILES,RUNTIME_BUILD_HELPER_FILES,runtimeBuildSourceRoot} from '../../launcher/lib/runtime-build-evidence.mjs';
import {captureRuntimePackageControlInputs,resolveRuntimePackagePlan,runtimePackageIdentity,runtimePackageSourceInputsIdentity,
  createRuntimePackageReceipt,RUNTIME_PACKAGE_CONTROL_FILES} from '../../launcher/lib/runtime-package-evidence.mjs';
import {parseExplicitGames,readVerifiedRuntimeFile,selectOfflineRuntimeTargets,selectRoomTargets,selectRuntimeTargets,verifyPrivateData,verifyPublishedRuntime,verifyRuntimePackage,verifySourceBuildReceipt} from './acceptance-contracts.mjs';

const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const checkoutRoot=resolve(import.meta.dirname,'../..');
const target=(game,variant,id=`${game}-${variant}`,extra={})=>({game,variant,id,runtime:'runtime',entry:`${game}.html`,data:'private.data',
  packageSha256:'a'.repeat(64),sourceRoot:`ports/${game}`,buildReceipt:`artifacts/${id}-build.json`,packageReceipt:`artifacts/${id}-package.json`,...extra});
const manifest=targets=>({targets});
const identities=files=>Object.fromEntries(Object.entries(files).map(([name,bytes])=>[name,{bytes:bytes.length,sha256:hash(bytes)}]));
const packageIdentity=(game,variant,entry,files)=>hash(Buffer.from(JSON.stringify({schema:'eagler-touhou/runtime-package-identity/1',game,variant,entry,
  files:Object.fromEntries(Object.keys(files).sort().map(name=>[name,{bytes:files[name].length,sha256:hash(files[name])}]))})));

test('game and Runtime mode must be explicit, supported, unique, and fully present',()=>{
  assert.throws(()=>parseExplicitGames(undefined,'--game'),/specified explicitly/);
  assert.throws(()=>parseExplicitGames('th08,th08','--game'),/duplicate/);
  assert.throws(()=>parseExplicitGames('th99','--game'),/unsupported/);
  assert.throws(()=>selectRuntimeTargets(manifest([target('th08','normal')]),{game:'th08'}),/normal, multiplayer, or both/);
  assert.throws(()=>selectRuntimeTargets(manifest([target('th08','normal')]),{game:'th08',variant:'both'}),/missing selected Runtime target th08\/multiplayer/);
  assert.throws(()=>selectRuntimeTargets(manifest([target('th20','normal')]),{game:'th20',variant:'multiplayer'}),/has no multiplayer Runtime/);
  assert.throws(()=>selectRuntimeTargets(manifest([target('th08','normal'),target('th08','multiplayer','th08-normal')]),{game:'th08',variant:'both'}),/Duplicate Runtime target ID/);
  const selected=selectRuntimeTargets(manifest([target('th08','multiplayer'),target('th08','normal')]),{game:'th08',variant:'both'});
  assert.deepEqual(selected.expected,['th08/normal','th08/multiplayer']);
  assert.deepEqual(selected.targets.map(value=>value.variant),['normal','multiplayer']);
});

test('normal offline acceptance needs no MP target and ignores unselected Runtime files',()=>{
  const normal=target('th08','normal');
  const single=selectOfflineRuntimeTargets(manifest([normal]),{game:'th08'});
  assert.deepEqual(single.expected,['th08/normal']);assert.deepEqual(single.targets,[normal]);
  const other=target('th07','normal','unselected',{runtime:'missing-unselected-runtime',data:'missing-unselected-data'});
  const mp=target('th08','multiplayer');
  const expanded=manifest([other,normal,mp]);
  assert.deepEqual(selectOfflineRuntimeTargets(expanded,{game:'th08'}),single);
  mp.runtime='another-unselected-MP-runtime';
  assert.deepEqual(selectOfflineRuntimeTargets(expanded,{game:'th08'}),single);
  assert.throws(()=>selectOfflineRuntimeTargets(manifest([mp]),{game:'th08'}),/missing selected Runtime target th08\/normal/);
});

test('room selection rejects every unsupported game/player pair instead of skipping it',()=>{
  assert.throws(()=>selectRoomTargets({game:undefined,players:2,route:'rtc'}),/specified explicitly/);
  assert.throws(()=>selectRoomTargets({game:'th09',players:3,route:'rtc'}),/does not support 3 players/);
  assert.throws(()=>selectRoomTargets({game:'th20',players:2,route:'rtc'}),/no multiplayer product/);
  assert.throws(()=>selectRoomTargets({game:'th11',players:2,route:undefined}),/rtc or relay/);
  assert.deepEqual(selectRoomTargets({game:'th11,th15',players:3,route:'relay'}).expected,
    ['th11/3p/relay','th15/3p/relay']);
});

async function packageFixture({game='th11',variant='multiplayer',generation=false,manifestIdentity=true,submodule=false}={}){
  const root=await mkdtemp(join(tmpdir(),'ports-runtime-package-'));
  const repoRoot=await mkdtemp(join(tmpdir(),'ports-runtime-source-'));
  execFileSync('git',['init'],{cwd:repoRoot,stdio:'ignore',windowsHide:true});
  await writeFile(join(repoRoot,'.gitignore'),'artifacts/\nbuild/\n');
  for(const path of new Set([...RUNTIME_BUILD_CONTROL_FILES,...RUNTIME_BUILD_HELPER_FILES,...RUNTIME_PACKAGE_CONTROL_FILES])){
    const destination=join(repoRoot,path);await mkdir(resolve(destination,'..'),{recursive:true});await copyFile(join(checkoutRoot,path),destination);
  }
  const wasm=Buffer.from(`wasm-${game}-${variant}`),html=Buffer.from(manifestIdentity?`<meta name="eagler-runtime-variant" content="${variant}">`:'<html></html>');
  const runtimeManifest={game,protocol:'eagler-touhou/1',profile:variant==='multiplayer'?'multiplayer':'production',product:variant==='multiplayer'?`${game}mp`:game,variant,
    execution:{sha256:hash(wasm)}};
  const files={...(manifestIdentity?{'manifest.json':Buffer.from(JSON.stringify(runtimeManifest))}:{}),[`${game}.html`]:html,[`${game}-sdl.wasm`]:wasm,[`${game}-sdl.mjs`]:Buffer.from('// current compiled loader\n')};
  // Use each real recipe's filenames, including th15's non-SDL suffix.
  const expectedOutputs=expectedRuntimeBuildOutputs(game,variant,repoRoot);
  const wasmName=expectedOutputs.execution.path.split('/').at(-1),loaderName=expectedOutputs.loader.path.split('/').at(-1);
  files[wasmName]=wasm;files[loaderName]=files[`${game}-sdl.mjs`];
  if(wasmName!==`${game}-sdl.wasm`)delete files[`${game}-sdl.wasm`];
  if(loaderName!==`${game}-sdl.mjs`)delete files[`${game}-sdl.mjs`];
  const fileIds=identities(files),targetValue={...target(game,variant,'current',{runtime:root})};
  const sourceRoot=runtimeBuildSourceRoot(game,variant,repoRoot),sourcePath=`${sourceRoot}/source.c`,sourceBytes=Buffer.from(`current source for ${game} ${variant}`);
  const commonPath='common/include/eagler/test/Shared.cpp',commonBytes=Buffer.from(`current common input for ${game} ${variant}`);
  await mkdir(join(repoRoot,sourceRoot),{recursive:true});
  await writeFile(join(repoRoot,sourcePath),sourceBytes);
  await mkdir(join(repoRoot,'common/include/eagler/test'),{recursive:true});
  await writeFile(join(repoRoot,commonPath),commonBytes);
  const packagePlan=resolveRuntimePackagePlan(game,variant,repoRoot);
  for(const path of [...packagePlan.controlFiles.map(path=>`${sourceRoot}/${path}`),...packagePlan.sharedControlFiles]){
    const destination=join(repoRoot,path);await mkdir(resolve(destination,'..'),{recursive:true});await copyFile(join(checkoutRoot,path),destination);
  }
  if(submodule){
    const configPath=join(repoRoot,'launcher/config/runtime-builds.json');
    const config=JSON.parse(await readFile(configPath,'utf8'));
    config.games[game].sourceBuild.requiredSubmodules=[{path:'vendor/native'}];
    await writeFile(configPath,JSON.stringify(config));
    const modulePath=join(repoRoot,sourceRoot,'vendor/native');
    await mkdir(modulePath,{recursive:true});
    const git=args=>execFileSync('git',args,{cwd:modulePath,stdio:'ignore',windowsHide:true});
    git(['init']);git(['config','user.email','acceptance@example.invalid']);git(['config','user.name','Acceptance fixture']);
    await writeFile(join(modulePath,'Native.hpp'),'int native_value();\n');
    git(['add','.']);git(['commit','-m','fixture']);
    const commit=execFileSync('git',['rev-parse','HEAD'],{cwd:modulePath,encoding:'utf8',windowsHide:true}).trim();
    execFileSync('git',['update-index','--add','--cacheinfo',`160000,${commit},${sourceRoot}/vendor/native`],{cwd:repoRoot,windowsHide:true});
  }
  const requiredSubmodules=expectedRuntimeBuildSubmodules(game,variant,repoRoot);
  const outputs={};
  for(const [role,{path}]of Object.entries(expectedOutputs)){
    const bytes=role==='execution'?wasm:role==='loader'?files[loaderName]:Buffer.from('{}');
    const destination=join(repoRoot,path);await mkdir(resolve(destination,'..'),{recursive:true});await writeFile(destination,bytes);
    outputs[role]={path,sha256:hash(bytes)};
  }
  const buildPlan={game,variant,source:join(repoRoot,sourceRoot),execution:join(repoRoot,outputs.execution.path),
    steps:[{command:process.execPath,args:['fixture compiler'],cwd:join(repoRoot,sourceRoot)}],
    features:{thcrap:true,thprac:false},toolchain:{version:'fixture toolchain'},requiredSubmodules};
  const buildPlanPath=join(repoRoot,`artifacts/validation/source-builds/${game}-${variant}.json`);
  await mkdir(resolve(buildPlanPath,'..'),{recursive:true});await writeFile(buildPlanPath,JSON.stringify(buildPlan));
  const receipt={schema:'eagler-touhou/runtime-source-build/3',game,variant,profile:variant==='multiplayer'?'multiplayer':'production',
    executionSha256:hash(wasm),sourceFiles:expectedRuntimeBuildSources(game,variant,repoRoot),
    requiredSubmodules,outputs,planSha256:runtimeBuildPlanIdentity(buildPlan)};
  targetValue.sourceRoot=sourceRoot;targetValue.buildReceipt='artifacts/current-build.json';
  targetValue.packageSha256=packageIdentity(game,variant,`${game}.html`,files);
  await mkdir(join(repoRoot,'artifacts'),{recursive:true});
  await writeFile(join(repoRoot,targetValue.buildReceipt),JSON.stringify(receipt));
  if(generation){
    const rootPath=variant==='multiplayer'?`runtime/${game}/multiplayer/`:`runtime/${game}/`;
    const descriptorFiles=Object.entries(fileIds).map(([path,file])=>({path,...file})).sort((a,b)=>a.path<b.path?-1:a.path>b.path?1:0);
    const generationId=hash(Buffer.from(canonicalRuntimePayload(`${game}.html`,descriptorFiles)));
    await writeFile(join(root,'runtime-generation.json'),JSON.stringify({schema:'eagler-touhou/runtime-generation/1',protocol:'eagler-touhou/1',root:rootPath,
      generation:generationId,entry:`${game}.html`,files:descriptorFiles}));
    targetValue.expectedGeneration=generationId;
  }else{
    await writeFile(join(root,'runtime-files.json'),JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game,variant,files:fileIds}));
  }
  for(const [name,bytes]of Object.entries(files))await writeFile(join(root,name),bytes);
  const descriptor=generation?Buffer.from(JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game,variant,files:fileIds})):
    await readFile(join(root,'runtime-files.json'));
  const packageReceipt=createRuntimePackageReceipt({game,variant,entry:targetValue.entry,packageSha256:targetValue.packageSha256,
    files:{...fileIds,'runtime-files.json':{bytes:descriptor.length,sha256:hash(descriptor)}},sourceBuildReceiptSha256:hash(Buffer.from(JSON.stringify(receipt))),
    sourceInputsSha256:runtimePackageSourceInputsIdentity(receipt.sourceFiles,requiredSubmodules).sourceInputsSha256,
    packagerInputs:captureRuntimePackageControlInputs(packagePlan,repoRoot)});
  await writeFile(join(repoRoot,targetValue.packageReceipt),JSON.stringify(packageReceipt));
  return {root,repoRoot,target:targetValue,files,fileIds,wasmName,loaderName,cleanup:async()=>{await rm(root,{recursive:true,force:true});await rm(repoRoot,{recursive:true,force:true});}};
}

test('Runtime package checks complete file inventory, product mode, and execution hash',async()=>{
  const fixture=await packageFixture();
  try{
    const identity=await verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot});
    assert.equal(identity.game,'th11');assert.equal(identity.variant,'multiplayer');assert.equal(identity.wasmSha256,hash(fixture.files[fixture.wasmName]));
    assert.match(identity.packageSha256,/^[a-f0-9]{64}$/);
    const receipt=JSON.parse(await readFile(join(fixture.repoRoot,fixture.target.buildReceipt),'utf8'));
    assert.equal(identity.sourceBuild.sourceCount,receipt.sourceFiles.length,'Full producer input inventory includes common cross-inputs and production helper files');
    assert.ok(receipt.sourceFiles.some(file=>file.path.startsWith('common/')));
    assert.ok(receipt.sourceFiles.some(file=>file.path.startsWith('ports/th11-mp/')));
    assert.deepEqual(await readVerifiedRuntimeFile(fixture.target,identity,'th11.html'),fixture.files['th11.html']);
    await assert.rejects(readVerifiedRuntimeFile(fixture.target,identity,'unlisted.mjs'),/undeclared file/);
    await writeFile(join(fixture.root,'th11.html'),'changed after verification');
    await assert.rejects(readVerifiedRuntimeFile(fixture.target,identity,'th11.html'),/changed after identity verification/);
    await writeFile(join(fixture.root,'th11.html'),fixture.files['th11.html']);
    await writeFile(join(fixture.root,'undeclared.mjs'),'x');
    await assert.rejects(verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot}),/inventory is not complete/);
    await rm(join(fixture.root,'undeclared.mjs'));
    fixture.fileIds[fixture.wasmName].sha256='0'.repeat(64);
    await writeFile(join(fixture.root,'runtime-files.json'),JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game:'th11',variant:'multiplayer',files:fixture.fileIds}));
    await assert.rejects(verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot}),/hash mismatch/);
  }finally{await fixture.cleanup();}
});

test('rehashed stale shell bytes and changed packaging controls cannot pass with a WASM-only source receipt',async()=>{
  const fixture=await packageFixture();
  try{
    const original=await readFile(join(fixture.root,'th11.html'));
    fixture.files['th11.html']=Buffer.from('<html>stale but self-consistent shell</html>');
    await writeFile(join(fixture.root,'th11.html'),fixture.files['th11.html']);
    const fileIds=identities(fixture.files);
    await writeFile(join(fixture.root,'runtime-files.json'),JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game:'th11',variant:'multiplayer',files:fileIds}));
    const target={...fixture.target,packageSha256:runtimePackageIdentity({game:'th11',variant:'multiplayer',entry:'th11.html',files:fileIds})};
    await assert.rejects(verifyRuntimePackage(target,{repoRoot:fixture.repoRoot}),/package receipt payload identity mismatch/);
    await writeFile(join(fixture.root,'th11.html'),original);fixture.files['th11.html']=original;
    await writeFile(join(fixture.root,'runtime-files.json'),JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game:'th11',variant:'multiplayer',files:fixture.fileIds}));
    await writeFile(join(fixture.repoRoot,'launcher/lib/runtime-package-evidence.mjs'),'// changed packager control\n');
    await assert.rejects(verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot}),/packager inputs no longer match/);
    const missing={...fixture.target};delete missing.packageReceipt;
    assert.throws(()=>selectRuntimeTargets(manifest([missing]),{game:'th11',variant:'multiplayer'}),/packageReceipt/);
  }finally{await fixture.cleanup();}
});


test('immutable package identity binds its game and normal/MP generation root',async()=>{
  const fixture=await packageFixture({game:'th15',variant:'multiplayer',generation:true});
  try{
    const identity=await verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot});assert.equal(identity.generation,fixture.target.expectedGeneration);
    const descriptorPath=join(fixture.root,'runtime-generation.json'),descriptor=JSON.parse(await readFile(descriptorPath,'utf8'));
    await writeFile(descriptorPath,JSON.stringify({...descriptor,root:'runtime/th15/'}));
    await assert.rejects(verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot}),/generation mode mismatch/);
    await writeFile(descriptorPath,JSON.stringify({...descriptor,root:'runtime/th11/multiplayer/'}));
    await assert.rejects(verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot}),/generation belongs to another game/);
  }finally{await fixture.cleanup();}
});

test('source receipt must include every owned and common input in the current configured inventory',async()=>{
  const fixture=await packageFixture();
  try{
    const receiptPath=join(fixture.repoRoot,fixture.target.buildReceipt),original=JSON.parse(await readFile(receiptPath,'utf8'));
    for(const prefix of ['common/','ports/th11-mp/']){
      const omitted=structuredClone(original),index=omitted.sourceFiles.findIndex(file=>file.path.startsWith(prefix));
      assert.notEqual(index,-1,`Fixture must contain a ${prefix} input`);
      omitted.sourceFiles.splice(index,1);await writeFile(receiptPath,JSON.stringify(omitted));
      await assert.rejects(verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot}),/complete current builder source inventory/);
    }
    await writeFile(receiptPath,JSON.stringify(original));
    await verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot});
  }finally{await fixture.cleanup();}
});

test('required dependency pins cannot be omitted, forged, or downgraded to a legacy receipt',async()=>{
  const fixture=await packageFixture({submodule:true});
  try{
    const receiptPath=join(fixture.repoRoot,fixture.target.buildReceipt);
    const original=JSON.parse(await readFile(receiptPath,'utf8'));
    const verify=()=>verifySourceBuildReceipt(fixture.target,{repoRoot:fixture.repoRoot,wasmSha256:original.executionSha256});
    const valid=await verify();assert.equal(valid.schema,'eagler-touhou/runtime-source-build/3');
    assert.equal(valid.requiredSubmodules.length,1);
    const downgraded={...original,schema:'eagler-touhou/runtime-source-build/1'};delete downgraded.requiredSubmodules;
    await writeFile(receiptPath,JSON.stringify(downgraded));await assert.rejects(verify(),/version 3/);
    const omitted={...original,requiredSubmodules:[]};await writeFile(receiptPath,JSON.stringify(omitted));
    await assert.rejects(verify(),/required submodule receipt does not match/);
    const forged=structuredClone(original);forged.requiredSubmodules[0].commit='0'.repeat(40);
    await writeFile(receiptPath,JSON.stringify(forged));await assert.rejects(verify(),/required submodule receipt does not match/);
    const duplicate=structuredClone(original);duplicate.requiredSubmodules.push(duplicate.requiredSubmodules[0]);
    await writeFile(receiptPath,JSON.stringify(duplicate));await assert.rejects(verify(),/duplicate required submodule/);
    await writeFile(receiptPath,JSON.stringify(original));
    await writeFile(join(fixture.repoRoot,original.requiredSubmodules[0].path,'Native.hpp'),'int changed_native_value();\n');
    await assert.rejects(verify(),/current source hash mismatch/);
  }finally{await fixture.cleanup();}
});

test('source build receipt verification does not require a package identity',async()=>{
  const fixture=await packageFixture();
  try{
    const {packageSha256,...sourceTarget}=fixture.target;
    const identity=await verifySourceBuildReceipt(sourceTarget,{repoRoot:fixture.repoRoot,
      wasmSha256:fixture.files[fixture.wasmName]&&hash(fixture.files[fixture.wasmName])});
    assert.equal(identity.game,fixture.target.game);
    assert.equal(identity.variant,fixture.target.variant);
    assert.match(identity.receiptSha256,/^[a-f0-9]{64}$/);
  }finally{await fixture.cleanup();}
});

test('expected package identity and builder-recorded source hashes must match current files and execution',async()=>{
  const fixture=await packageFixture();
  try{
    await assert.rejects(verifyRuntimePackage({...fixture.target,packageSha256:'0'.repeat(64)},{repoRoot:fixture.repoRoot}),/expected current packageSha256/);
    const receiptPath=join(fixture.repoRoot,fixture.target.buildReceipt),receipt=JSON.parse(await readFile(receiptPath,'utf8'));
    receipt.executionSha256='0'.repeat(64);await writeFile(receiptPath,JSON.stringify(receipt));
    await assert.rejects(verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot}),/not bound to the packaged execution WASM/);
    receipt.executionSha256=hash(fixture.files[fixture.wasmName]);receipt.sourceFiles[0].sha256='0'.repeat(64);
    await writeFile(receiptPath,JSON.stringify(receipt));
    await assert.rejects(verifySourceBuildReceipt(fixture.target,{repoRoot:fixture.repoRoot,wasmSha256:receipt.executionSha256}),/current source hash mismatch/);
    receipt.sourceFiles[0].sha256=hash(await readFile(join(fixture.repoRoot,receipt.sourceFiles[0].path)));
    receipt.sourceFiles[0].path='../outside.c';await writeFile(receiptPath,JSON.stringify(receipt));
    await assert.rejects(verifySourceBuildReceipt(fixture.target,{repoRoot:fixture.repoRoot,wasmSha256:receipt.executionSha256}),/repository-relative/);
  }finally{await fixture.cleanup();}
});

test('a repackaged stale compiler loader cannot pass by rehashing every package declaration',async()=>{
  const fixture=await packageFixture();
  try{
    const receiptPath=join(fixture.repoRoot,fixture.target.buildReceipt),receipt=JSON.parse(await readFile(receiptPath,'utf8'));
    const loaderPath=join(fixture.repoRoot,receipt.outputs.loader.path),original=await readFile(loaderPath);
    await writeFile(loaderPath,'// stale generated loader\n');
    await assert.rejects(verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot}),error=>
      /output changed/.test(error.message)&&error.message.includes(receipt.outputs.loader.path));
    await writeFile(loaderPath,original);
    fixture.files[fixture.loaderName]=Buffer.from('// stale packaged loader\n');
    await writeFile(join(fixture.root,fixture.loaderName),fixture.files[fixture.loaderName]);
    const files=identities(fixture.files),inventory=Buffer.from(JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game:'th11',variant:'multiplayer',files}));
    await writeFile(join(fixture.root,'runtime-files.json'),inventory);
    const target={...fixture.target,packageSha256:runtimePackageIdentity({...fixture.target,files})};
    const packageReceiptPath=join(fixture.repoRoot,target.packageReceipt),packageReceipt=JSON.parse(await readFile(packageReceiptPath,'utf8'));
    packageReceipt.packageSha256=target.packageSha256;packageReceipt.files={...files,'runtime-files.json':{bytes:inventory.length,sha256:hash(inventory)}};
    await writeFile(packageReceiptPath,JSON.stringify(packageReceipt));
    await assert.rejects(verifyRuntimePackage(target,{repoRoot:fixture.repoRoot}),/packaged loader is not the current compiler output/);
  }finally{await fixture.cleanup();}
});

test('edited compiler steps and toolchain metadata invalidate the source build receipt',async()=>{
  const fixture=await packageFixture();
  try{
    const path=join(fixture.repoRoot,`artifacts/validation/source-builds/${fixture.target.game}-${fixture.target.variant}.json`);
    const original=JSON.parse(await readFile(path,'utf8'));
    for(const field of ['steps','toolchain','features']){
      const changed=structuredClone(original);changed[field]=field==='steps'?[{command:'not-a-compiler',args:[]}]:{changed:true};
      await writeFile(path,JSON.stringify(changed));
      await assert.rejects(verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot}),/source build plan changed after compilation/);
    }
  }finally{await fixture.cleanup();}
});

test('missing or empty private DATA fails and valid DATA records its hash',async()=>{
  const root=await mkdtemp(join(tmpdir(),'ports-private-data-'));
  try{
    const data=resolve(root,'game.data');
    await assert.rejects(verifyPrivateData(data),/ENOENT/);
    await writeFile(data,'');await assert.rejects(verifyPrivateData(data),/non-empty regular file/);
    await writeFile(data,'private data');assert.deepEqual(await verifyPrivateData(data),{bytes:12,sha256:hash(Buffer.from('private data'))});
  }finally{await rm(root,{recursive:true,force:true});}
});

async function publishedFixture({stale=false,unsorted=false}={}){
  const game='th11',variant='multiplayer',root=`runtime/${game}/multiplayer/`,entry=`${game}.html`;
  const buildGeneration=(mode,groupRoot)=>{
    const wasm=Buffer.from(`published ${mode} wasm`),runtimeFiles={
      [entry]:Buffer.from(`<html>${mode}</html>`),
    'manifest.json':Buffer.from(JSON.stringify({game,protocol:'eagler-touhou/1',profile:mode==='multiplayer'?'multiplayer':'production',variant:mode,
        product:mode==='multiplayer'?'th11mp':'th11',execution:{sha256:hash(wasm)}})),
      [`${game}.wasm`]:wasm,
    };
    const fileIds=identities(runtimeFiles),files=Object.entries(fileIds).map(([path,item])=>({path,...item})).sort((a,b)=>a.path<b.path?-1:a.path>b.path?1:0);
    const generation=hash(Buffer.from(canonicalRuntimePayload(entry,files)));
    return{root:groupRoot,entry,runtimeFiles,files,generation,
      packageSha256:packageIdentity(game,mode,entry,Object.fromEntries(Object.entries(runtimeFiles).filter(([name])=>name!=='runtime-generation.json')))};
  };
  const multiplayer=buildGeneration(variant,root),normal=buildGeneration('normal',`runtime/${game}/`);
  if(unsorted){multiplayer.files.reverse();normal.files.reverse();}
  const site={schema:'eagler-touhou/site-manifest/1',protocol:'eagler-touhou/1',profile:'production',
    shared:{resourceMode:'hosted',vanillaFont:'shared/msgothic.ttc',unicodeFont:'shared/unifont.otf',runtimeManifest:'runtime-manifest.json'},
    games:{[game]:{runtime:`${normal.root}${normal.generation}/${entry}?hosted=1`,
      multiplayerRuntime:`${root}${multiplayer.generation}/${entry}?hosted=1`,
      gameData:{path:PRODUCT_GAMES[game].package.dataTarget.slice(1),bytes:1,sha256:'a'.repeat(64),version:`sha256-${'a'.repeat(64)}`,layout:`sha256-${'b'.repeat(64)}`},
      music:{midi:{files:[]}}}}};
  const actualGeneration=stale?'f'.repeat(64):multiplayer.generation;
  if(stale)site.games[game].multiplayerRuntime=`${root}${actualGeneration}/${entry}?hosted=1`;
  const siteBytes=Buffer.from(JSON.stringify(site));
  const runtimeManifest={schema:'eagler-touhou/runtime-manifest/1',protocol:'eagler-touhou/1',groups:[normal,multiplayer].map(item=>({root:item.root,
    current:{generation:item.generation,entry:item.entry,files:item.files},previous:[]}))};
  const generationDescriptors=[normal,multiplayer].map(item=>({schema:'eagler-touhou/runtime-generation/1',protocol:'eagler-touhou/1',root:item.root,
    generation:item.generation,entry:item.entry,files:item.files}));
  const bodies=new Map([
    ['/app/site-manifest.json',siteBytes],['/app/runtime-manifest.json',Buffer.from(JSON.stringify(runtimeManifest))],
    ...[normal,multiplayer].map((item,index)=>[`/app/${item.root}${item.generation}/runtime-generation.json`,Buffer.from(JSON.stringify(generationDescriptors[index]))]),
    ...[normal,multiplayer].flatMap(item=>[
      ...Object.entries(item.runtimeFiles).map(([name,bytes])=>[`/app/${item.root}${item.generation}/${name}`,bytes]),
    ]),
  ]);
  const fetchImpl=async input=>{const path=new URL(input).pathname,bytes=bodies.get(path);return new Response(bytes??'missing',{status:bytes?200:404});};
  return{fetchImpl,siteBytes,generation:multiplayer.generation,packageSha256:multiplayer.packageSha256};
}

test('room acceptance validates Launcher current generation and every served package byte',async()=>{
  const fixture=await publishedFixture();
  const result=await verifyPublishedRuntime('https://launcher.invalid/app/','th11','multiplayer',{fetchImpl:fixture.fetchImpl,expectedPackageSha256:fixture.packageSha256});
  assert.equal(result.generation,fixture.generation);assert.equal(result.source,'current-published-generation');
  assert.match(result.packageSha256,/^[a-f0-9]{64}$/);
  const unsorted=await publishedFixture({unsorted:true});
  assert.equal(unsorted.packageSha256,fixture.packageSha256,'Package identity must ignore legal Runtime generation file ordering');
  const unsortedResult=await verifyPublishedRuntime('https://launcher.invalid/app/','th11','multiplayer',
    {fetchImpl:unsorted.fetchImpl,expectedPackageSha256:unsorted.packageSha256});
  assert.equal(unsortedResult.packageSha256,fixture.packageSha256);
  const stale=await publishedFixture({stale:true});
  await assert.rejects(verifyPublishedRuntime('https://launcher.invalid/app/','th11','multiplayer',{fetchImpl:stale.fetchImpl,expectedPackageSha256:stale.packageSha256}),/stale Runtime generation/);
  await assert.rejects(verifyPublishedRuntime('https://launcher.invalid/app/','th11','multiplayer',{fetchImpl:fixture.fetchImpl,expectedPackageSha256:'0'.repeat(64)}),/does not match the source-verified current packageSha256/);
});
