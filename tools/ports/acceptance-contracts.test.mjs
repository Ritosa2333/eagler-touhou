import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtemp,rm,mkdir,readFile,writeFile,copyFile} from 'node:fs/promises';
import {join,resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {execFileSync} from 'node:child_process';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {canonicalRuntimePayload} from '../../launcher/lib/contracts/runtime-generations.mjs';
import {expectedRuntimeBuildSources,RUNTIME_BUILD_CONTROL_FILES,RUNTIME_BUILD_HELPER_FILES,runtimeBuildSourceRoot} from '../../launcher/lib/runtime-build-evidence.mjs';
import {parseExplicitGames,readVerifiedRuntimeFile,selectRoomTargets,selectRuntimeTargets,verifyPrivateData,verifyPublishedRuntime,verifyRuntimePackage,verifySourceBuildReceipt} from './acceptance-contracts.mjs';

const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const checkoutRoot=resolve(import.meta.dirname,'../..');
const target=(game,variant,id=`${game}-${variant}`,extra={})=>({game,variant,id,runtime:'runtime',entry:`${game}.html`,data:'private.data',
  packageSha256:'a'.repeat(64),sourceRoot:`ports/${game}`,buildReceipt:`artifacts/${id}-build.json`,...extra});
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

test('room selection rejects every unsupported game/player pair instead of skipping it',()=>{
  assert.throws(()=>selectRoomTargets({game:undefined,players:2,route:'rtc'}),/specified explicitly/);
  assert.throws(()=>selectRoomTargets({game:'th09',players:3,route:'rtc'}),/does not support 3 players/);
  assert.throws(()=>selectRoomTargets({game:'th20',players:2,route:'rtc'}),/no multiplayer product/);
  assert.throws(()=>selectRoomTargets({game:'th11',players:2,route:undefined}),/rtc or relay/);
  assert.deepEqual(selectRoomTargets({game:'th11,th15',players:3,route:'relay'}).expected,
    ['th11/3p/relay','th15/3p/relay']);
});

async function packageFixture({game='th11',variant='multiplayer',generation=false,manifestIdentity=true}={}){
  const root=await mkdtemp(join(tmpdir(),'ports-runtime-package-'));
  const repoRoot=await mkdtemp(join(tmpdir(),'ports-runtime-source-'));
  execFileSync('git',['init'],{cwd:repoRoot,stdio:'ignore',windowsHide:true});
  for(const path of [...RUNTIME_BUILD_CONTROL_FILES,...RUNTIME_BUILD_HELPER_FILES]){
    const destination=join(repoRoot,path);await mkdir(resolve(destination,'..'),{recursive:true});await copyFile(join(checkoutRoot,path),destination);
  }
  const wasm=Buffer.from(`wasm-${game}-${variant}`),html=Buffer.from(manifestIdentity?`<meta name="eagler-runtime-variant" content="${variant}">`:'<html></html>');
  const runtimeManifest={game,protocol:'eagler-touhou/1',profile:variant==='multiplayer'?'multiplayer':'production',product:variant==='multiplayer'?`${game}mp`:game,variant,
    execution:{sha256:hash(wasm)}};
  const files={...(manifestIdentity?{'manifest.json':Buffer.from(JSON.stringify(runtimeManifest))}:{}),[`${game}.html`]:html,[`${game}.wasm`]:wasm};
  const fileIds=identities(files),targetValue={...target(game,variant,'current',{runtime:root})};
  const sourceRoot=runtimeBuildSourceRoot(game,variant,repoRoot),sourcePath=`${sourceRoot}/source.c`,sourceBytes=Buffer.from(`current source for ${game} ${variant}`);
  const commonPath='common/include/eagler/test/Shared.cpp',commonBytes=Buffer.from(`current common input for ${game} ${variant}`);
  await mkdir(join(repoRoot,sourceRoot),{recursive:true});
  await writeFile(join(repoRoot,sourcePath),sourceBytes);
  await mkdir(join(repoRoot,'common/include/eagler/test'),{recursive:true});
  await writeFile(join(repoRoot,commonPath),commonBytes);
  const receipt={schema:'eagler-touhou/runtime-source-build/1',game,variant,profile:variant==='multiplayer'?'multiplayer':'production',
    executionSha256:hash(wasm),sourceFiles:expectedRuntimeBuildSources(game,variant,repoRoot)};
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
  return {root,repoRoot,target:targetValue,files,fileIds,cleanup:async()=>{await rm(root,{recursive:true,force:true});await rm(repoRoot,{recursive:true,force:true});}};
}

test('Runtime package checks complete file inventory, product mode, and execution hash',async()=>{
  const fixture=await packageFixture();
  try{
    const identity=await verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot});
    assert.equal(identity.game,'th11');assert.equal(identity.variant,'multiplayer');assert.equal(identity.wasmSha256,hash(fixture.files['th11.wasm']));
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
    fixture.fileIds['th11.wasm'].sha256='0'.repeat(64);
    await writeFile(join(fixture.root,'runtime-files.json'),JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game:'th11',variant:'multiplayer',files:fixture.fileIds}));
    await assert.rejects(verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot}),/hash mismatch/);
  }finally{await fixture.cleanup();}
});


test('immutable package identity binds its game and normal/MP generation root',async()=>{
  const fixture=await packageFixture({game:'th15',variant:'multiplayer',generation:true,manifestIdentity:false});
  try{
    const identity=await verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot});assert.equal(identity.generation,fixture.target.expectedGeneration);
    const wrongMode={...fixture.target,variant:'normal'};
    await assert.rejects(verifyRuntimePackage(wrongMode,{repoRoot:fixture.repoRoot}),/generation mode mismatch/);
    const wrongGame={...fixture.target,game:'th11'};
    await assert.rejects(verifyRuntimePackage(wrongGame,{repoRoot:fixture.repoRoot}),/generation belongs to another game/);
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

test('expected package identity and builder-recorded source hashes must match current files and execution',async()=>{
  const fixture=await packageFixture();
  try{
    await assert.rejects(verifyRuntimePackage({...fixture.target,packageSha256:'0'.repeat(64)},{repoRoot:fixture.repoRoot}),/expected current packageSha256/);
    const receiptPath=join(fixture.repoRoot,fixture.target.buildReceipt),receipt=JSON.parse(await readFile(receiptPath,'utf8'));
    receipt.executionSha256='0'.repeat(64);await writeFile(receiptPath,JSON.stringify(receipt));
    await assert.rejects(verifyRuntimePackage(fixture.target,{repoRoot:fixture.repoRoot}),/not bound to the packaged execution WASM/);
    receipt.executionSha256=hash(fixture.files['th11.wasm']);receipt.sourceFiles[0].sha256='0'.repeat(64);
    await writeFile(receiptPath,JSON.stringify(receipt));
    await assert.rejects(verifySourceBuildReceipt(fixture.target,{repoRoot:fixture.repoRoot,wasmSha256:receipt.executionSha256}),/current source hash mismatch/);
    receipt.sourceFiles[0].sha256=hash(await readFile(join(fixture.repoRoot,receipt.sourceFiles[0].path)));
    receipt.sourceFiles[0].path='../outside.c';await writeFile(receiptPath,JSON.stringify(receipt));
    await assert.rejects(verifySourceBuildReceipt(fixture.target,{repoRoot:fixture.repoRoot,wasmSha256:receipt.executionSha256}),/repository-relative/);
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
