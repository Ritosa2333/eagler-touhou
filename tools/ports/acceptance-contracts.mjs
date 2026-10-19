import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {lstat,readdir,readFile,realpath} from 'node:fs/promises';
import {resolve,relative,isAbsolute,sep,basename} from 'node:path';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {canonicalRuntimePayload,parseRuntimeGenerationPath,runtimeGenerationBase,validateRuntimeManifest} from '../../launcher/lib/contracts/runtime-generations.mjs';
import {validateSiteManifest} from '../../launcher/lib/contracts/site-manifest.mjs';
import {expectedRuntimeBuildSources,expectedRuntimeBuildSubmodules,runtimeBuildSourceRoot,verifyRuntimeBuildOutputs,runtimeBuildPlanIdentity} from '../../launcher/lib/runtime-build-evidence.mjs';
import {captureRuntimePackageControlInputs,resolveRuntimePackagePlan,runtimePackageIdentity,runtimePackageSourceInputsIdentity,
  RUNTIME_PACKAGE_BUILD_SCHEMA} from '../../launcher/lib/runtime-package-evidence.mjs';

const sha256 = bytes => createHash('sha256').update(bytes).digest('hex');
const safeRelativePath = value => typeof value === 'string' && value.length > 0 &&
  !value.includes('\\') && !value.includes(':') && !value.startsWith('/') &&
  value.split('/').every(part => part && part !== '.' && part !== '..');
const safeSourceRoot = value => value === '.' || safeRelativePath(value);
const inside = (root,path) => {
  const pathFromRoot=relative(root,path);
  return pathFromRoot===''||(!isAbsolute(pathFromRoot)&&pathFromRoot!=='..'&&!pathFromRoot.startsWith(`..${sep}`));
};
const own = (value,key) => Object.prototype.hasOwnProperty.call(value,key);

async function checkedWorkspacePath(repoRoot,relativePath,label,{directory=false}={}) {
  assert.ok(safeSourceRoot(relativePath),`${label} must be a safe repository-relative path`);
  const root=await realpath(repoRoot),path=resolve(root,relativePath);
  assert.ok(inside(root,path),`${label} escapes the repository`);
  let cursor=root;
  for(const part of relativePath==='.'?[]:relativePath.split('/')){
    cursor=resolve(cursor,part);
    const component=await lstat(cursor);
    assert.ok(!component.isSymbolicLink(),`${label} may not traverse a symbolic link`);
  }
  const info=await lstat(path);
  assert.ok(directory?info.isDirectory():info.isFile(),`${label} must be a ${directory?'directory':'regular file'}`);
  assert.ok(!info.isSymbolicLink(),`${label} must not be a symbolic link`);
  const resolved=await realpath(path);
  assert.ok(inside(root,resolved),`${label} resolves outside the repository`);
  return resolved;
}

/** Bind a package's execution WASM to builder-recorded hashes of current repository inputs. */
export async function verifySourceBuildReceipt(target,{repoRoot,wasmSha256}) {
  assert.ok(repoRoot,'Repository root is required to verify a source build receipt');
  repoRoot=resolve(repoRoot);
  assert.match(target.packageSha256,/^[a-f0-9]{64}$/,`${target.id}: expected packageSha256 is required`);
  const expectedOwner=runtimeBuildSourceRoot(target.game,target.variant,repoRoot);
  assert.equal(target.sourceRoot,expectedOwner,`${target.id}: sourceRoot must identify the current ${target.game}/${target.variant} source owner`);
  const sourceRoot=await checkedWorkspacePath(repoRoot,target.sourceRoot,`${target.id}: sourceRoot`,{directory:true});
  const receiptPath=await checkedWorkspacePath(repoRoot,target.buildReceipt,`${target.id}: buildReceipt`);
  const receiptBytes=await readFile(receiptPath),receipt=JSON.parse(receiptBytes.toString('utf8'));
  assert.equal(receipt.schema,'eagler-touhou/runtime-source-build/3',
    `${target.id}: current Runtime acceptance requires a version 3 source build receipt`);
  assert.deepEqual(Object.keys(receipt).sort(),['executionSha256','game','profile','schema','sourceFiles','variant',
    'requiredSubmodules','outputs','planSha256'].sort(),
    `${target.id}: build receipt has unsupported fields`);
  assert.equal(receipt.game,target.game,`${target.id}: source build receipt game mismatch`);
  assert.equal(receipt.variant,target.variant,`${target.id}: source build receipt variant mismatch`);
  assert.equal(receipt.profile,target.variant==='multiplayer'?'multiplayer':'production',`${target.id}: source build receipt profile mismatch`);
  assert.match(receipt.executionSha256,/^[a-f0-9]{64}$/,`${target.id}: source build receipt execution SHA-256 is invalid`);
  assert.equal(receipt.executionSha256,wasmSha256,`${target.id}: receipt is not bound to the packaged execution WASM`);
  const expectedModules=expectedRuntimeBuildSubmodules(target.game,target.variant,repoRoot);
  const requiredSubmodules=receipt.requiredSubmodules;
  assert.ok(Array.isArray(requiredSubmodules),`${target.id}: requiredSubmodules must be an array`);
  const modulePaths=new Set();
  for(const module of requiredSubmodules){
    assert.deepEqual(Object.keys(module).sort(),['commit','path'],`${target.id}: malformed required submodule receipt`);
    assert.ok(safeRelativePath(module.path),`${target.id}: submodule path must be repository-relative`);
    assert.match(module.commit,/^(?:[a-f0-9]{40}|[a-f0-9]{64})$/,`${target.id}: invalid submodule commit`);
    assert.ok(!modulePaths.has(module.path),`${target.id}: duplicate required submodule ${module.path}`);modulePaths.add(module.path);
    await checkedWorkspacePath(repoRoot,module.path,`${target.id}: required submodule ${module.path}`,{directory:true});
  }
  const byPath=(a,b)=>a.path<b.path?-1:a.path>b.path?1:0;
  assert.deepEqual([...requiredSubmodules].sort(byPath),[...expectedModules].sort(byPath),
    `${target.id}: required submodule receipt does not match the current build recipe and gitlink pins`);
  assert.ok(Array.isArray(receipt.sourceFiles)&&receipt.sourceFiles.length>0,`${target.id}: source build receipt has no source files`);
  const expectedSources=expectedRuntimeBuildSources(target.game,target.variant,repoRoot);
  const seen=new Set();let ownedProductionSources=0;
  for(const source of receipt.sourceFiles){
    assert.deepEqual(Object.keys(source).sort(),['path','sha256'],`${target.id}: malformed source file receipt`);
    assert.ok(safeRelativePath(source.path),`${target.id}: source path must be repository-relative: ${source.path}`);
    assert.ok(!seen.has(source.path),`${target.id}: duplicate source file receipt ${source.path}`);seen.add(source.path);
    assert.match(source.sha256,/^[a-f0-9]{64}$/,`${target.id}: invalid source SHA-256 for ${source.path}`);
    const sourcePath=await checkedWorkspacePath(repoRoot,source.path,`${target.id}: source file ${source.path}`);
    if(inside(sourceRoot,sourcePath)&&/\.(?:c|cpp)$/i.test(source.path))ownedProductionSources++;
    const digest=sha256(await readFile(sourcePath));
    assert.equal(digest,source.sha256,`${target.id}: current source hash mismatch for ${source.path}`);
  }
  assert.deepEqual([...seen].sort(),expectedSources.map(source=>source.path).sort(),
    `${target.id}: source receipt does not match the complete current builder source inventory`);
  assert.ok(ownedProductionSources>0,`${target.id}: receipt has no owned C/C++ production source`);
  verifyRuntimeBuildOutputs(receipt,target.game,target.variant,repoRoot);
  const buildPlan=`artifacts/validation/source-builds/${target.game}-${target.variant}.json`;
  const planPath=await checkedWorkspacePath(repoRoot,buildPlan,`${target.id}: source build plan`);
  const planBytes=await readFile(planPath),plan=JSON.parse(planBytes.toString('utf8'));
  assert.equal(plan.game,target.game,`${target.id}: source build plan game mismatch`);
  assert.equal(plan.variant,target.variant,`${target.id}: source build plan variant mismatch`);
  assert.equal(resolve(repoRoot,plan.source),sourceRoot,`${target.id}: source build plan owner mismatch`);
  assert.equal(resolve(repoRoot,plan.execution),resolve(repoRoot,receipt.outputs.execution.path),`${target.id}: source build plan output mismatch`);
  assert.equal(runtimeBuildPlanIdentity(plan),receipt.planSha256,`${target.id}: source build plan changed after compilation`);
  return {schema:receipt.schema,game:receipt.game,variant:receipt.variant,profile:receipt.profile,
    executionSha256:receipt.executionSha256,sourceRoot:target.sourceRoot,buildReceipt:target.buildReceipt,
    receiptSha256:sha256(receiptBytes),sourceCount:receipt.sourceFiles.length,requiredSubmodules,
    outputs:receipt.outputs,planSha256:receipt.planSha256,buildPlan,planReceiptSha256:sha256(planBytes),
    ...runtimePackageSourceInputsIdentity(receipt.sourceFiles,requiredSubmodules)};
}

/** Bind every packaged byte to a completed packager run over the current source build and controls. */
export async function verifyPackageBuildReceipt(target,{repoRoot,sourceBuild,files,packageSha256}) {
  const receiptPath=await checkedWorkspacePath(repoRoot,target.packageReceipt,`${target.id}: packageReceipt`);
  const bytes=await readFile(receiptPath),receipt=JSON.parse(bytes.toString('utf8'));
  assert.equal(receipt.schema,RUNTIME_PACKAGE_BUILD_SCHEMA,`${target.id}: unsupported package build receipt`);
  assert.deepEqual(Object.keys(receipt).sort(),['schema','game','variant','entry','packageSha256','files',
    'sourceBuildReceiptSha256','sourceInputsSha256','packagerInputs'].sort(),`${target.id}: package receipt has unsupported fields`);
  for(const key of ['game','variant','entry'])assert.equal(receipt[key],target[key],`${target.id}: package receipt ${key} mismatch`);
  assert.equal(receipt.packageSha256,packageSha256,`${target.id}: package receipt payload identity mismatch`);
  assert.equal(receipt.sourceBuildReceiptSha256,sourceBuild.receiptSha256,`${target.id}: package receipt is not bound to the current source build receipt`);
  assert.equal(receipt.sourceInputsSha256,sourceBuild.sourceInputsSha256,`${target.id}: package receipt is not bound to current source inputs`);
  const canonical=map=>Object.fromEntries(Object.keys(map).sort().map(name=>[name,map[name]]));
  assert.ok(receipt.files&&typeof receipt.files==='object'&&!Array.isArray(receipt.files),`${target.id}: package receipt files must be a map`);
  assert.equal(runtimePackageIdentity({...target,files:receipt.files}),packageSha256,`${target.id}: package receipt file identity mismatch`);
  if(own(files,'runtime-generation.json')){
    const payload=map=>Object.fromEntries(Object.entries(map).filter(([name])=>!['runtime-files.json','runtime-generation.json'].includes(name)));
    assert.deepEqual(canonical(payload(receipt.files)),canonical(payload(files)),`${target.id}: package receipt does not match the published Runtime payload`);
  }else assert.deepEqual(canonical(receipt.files),canonical(files),`${target.id}: package receipt does not match the complete Runtime directory`);
  const plan=resolveRuntimePackagePlan(target.game,target.variant,repoRoot);
  const expected=captureRuntimePackageControlInputs(plan,repoRoot);
  assert.ok(Array.isArray(receipt.packagerInputs),`${target.id}: package receipt has no packager inputs`);
  for(const input of expected)await checkedWorkspacePath(repoRoot,input.path,`${target.id}: packager input ${input.path}`);
  assert.deepEqual(receipt.packagerInputs,expected,`${target.id}: packager inputs no longer match the current Runtime package recipe`);
  return {schema:receipt.schema,packageReceipt:target.packageReceipt,receiptSha256:sha256(bytes),
    sourceBuildReceiptSha256:receipt.sourceBuildReceiptSha256,sourceInputsSha256:receipt.sourceInputsSha256,packagerInputCount:expected.length};
}

export function parseExplicitGames(value, option) {
  assert.equal(typeof value,'string',`${option} must be specified explicitly`);
  const games=value.split(',');
  assert.ok(games.length&&games.every(Boolean),`${option} must contain comma-separated game IDs`);
  assert.equal(new Set(games).size,games.length,`${option} must not contain duplicate game IDs`);
  for(const game of games)assert.ok(PRODUCT_GAMES[game],`${option} contains unsupported game ${game}`);
  return games;
}

export function selectRuntimeTargets(config,{game,variant}) {
  const games=parseExplicitGames(game,'--game');
  assert.ok(['normal','multiplayer','both'].includes(variant),'--variant must be normal, multiplayer, or both');
  assert.ok(config&&Array.isArray(config.targets),'Input manifest must declare a targets array');
  const byKey=new Map(),ids=new Set();
  for(const target of config.targets){
    assert.ok(target&&typeof target==='object'&&!Array.isArray(target),'Runtime target must be an object');
    assert.ok(PRODUCT_GAMES[target.game],`Runtime target has unsupported game ${target.game}`);
    assert.ok(['normal','multiplayer'].includes(target.variant),`Runtime target has unsupported variant ${target.variant}`);
    if(target.variant==='multiplayer')assert.ok(PRODUCT_GAMES[target.game].multiplayerRuntime,`${target.game} has no multiplayer Runtime`);
    assert.ok(typeof target.id==='string'&&/^[A-Za-z0-9][A-Za-z0-9_-]*$/.test(target.id),`Invalid Runtime target ID ${target.id}`);
    assert.ok(!ids.has(target.id),`Duplicate Runtime target ID ${target.id}`);ids.add(target.id);
    assert.ok(typeof target.runtime==='string'&&target.runtime,`${target.id}: Runtime directory is required`);
    assert.ok(safeRelativePath(target.entry),`${target.id}: entry must be a safe relative path`);
    assert.ok(typeof target.data==='string'&&target.data,`${target.id}: private DATA path is required`);
    assert.match(target.packageSha256,/^[a-f0-9]{64}$/,`${target.id}: packageSha256 must be provided as a SHA-256`);
    assert.ok(safeRelativePath(target.buildReceipt),`${target.id}: buildReceipt must be a safe repository-relative path`);
    assert.ok(safeRelativePath(target.packageReceipt),`${target.id}: packageReceipt must be a safe repository-relative path`);
    assert.ok(safeSourceRoot(target.sourceRoot),`${target.id}: sourceRoot must be a safe repository-relative path`);
    assert.ok(!own(target,'files')||Array.isArray(target.files),`${target.id}: files must be an array`);
    const key=`${target.game}/${target.variant}`;
    assert.ok(!byKey.has(key),`Duplicate Runtime target ${key}`);byKey.set(key,target);
  }
  const expected=[];
  for(const id of games){
    const modes=variant==='both'?['normal','multiplayer']:[variant];
    for(const mode of modes){
      if(mode==='multiplayer')assert.ok(PRODUCT_GAMES[id].multiplayerRuntime,`${id} has no multiplayer Runtime`);
      const key=`${id}/${mode}`,target=byKey.get(key);
      assert.ok(target,`Input manifest is missing selected Runtime target ${key}`);
      expected.push(key);
    }
  }
  assert.ok(expected.length>0,'Runtime selection is empty');
  return {games,variant,expected,targets:expected.map(key=>byKey.get(key))};
}

export function selectRoomTargets({game,players,route}) {
  const games=parseExplicitGames(game,'--game');
  assert.ok(Number.isSafeInteger(players), '--players must be specified as an integer');
  assert.ok(['rtc','relay'].includes(route),'--route must be rtc or relay');
  for(const id of games){
    const multiplayer=PRODUCT_GAMES[id].multiplayer;
    assert.ok(multiplayer,`${id} has no multiplayer product`);
    assert.ok(multiplayer.playerCounts.includes(players),`${id} does not support ${players} players`);
  }
  assert.ok(games.length>0,'Room selection is empty');
  return {games,players,route,expected:games.map(game=>`${game}/${players}p/${route}`)};
}

async function inventoryFiles(root) {
  const files=[];
  async function visit(directory){
    for(const item of await readdir(directory,{withFileTypes:true})){
      const path=resolve(directory,item.name),name=relative(root,path).replaceAll('\\','/');
      if(item.isSymbolicLink())throw Error(`Runtime package may not contain symbolic links: ${name}`);
      if(item.isDirectory())await visit(path);
      else if(item.isFile())files.push(name);
      else throw Error(`Runtime package contains a non-file entry: ${name}`);
    }
  }
  await visit(root);return files.sort();
}

function validateMode({game,variant,inventory,manifest,entryBytes,generationRoot}) {
  const claims=[inventory.variant,manifest?.variant].filter(value=>value!==undefined);
  for(const claim of claims)assert.ok(['normal','multiplayer'].includes(claim),`${game}: invalid package variant ${claim}`);
  assert.ok(new Set(claims).size<=1,`${game}: package inventory and manifest disagree on variant`);
  const productClaims=[inventory.product,manifest?.product].filter(value=>value!==undefined);
  const expectedProduct=variant==='multiplayer'?`${game}mp`:game;
  for(const claim of productClaims)assert.equal(claim,expectedProduct,`${game}: package product does not match ${variant}`);
  if(manifest?.profile!==undefined){
    const expectedProfile=variant==='multiplayer'?'multiplayer':'production';
    assert.equal(manifest.profile,expectedProfile,`${game}: package profile does not match ${variant}`);
  }
  const html=entryBytes.toString('utf8');
  const htmlVariant=html.match(/<meta\s+name=["']eagler-runtime-variant["']\s+content=["'](normal|multiplayer)["']/i)?.[1];
  if(htmlVariant)assert.equal(htmlVariant,variant,`${game}: entry HTML variant mismatch`);
  if(generationRoot){
    const segments=generationRoot.split('/').filter(Boolean);
    assert.equal(segments[0],'runtime',`${game}: invalid Runtime generation root`);
    assert.equal(segments[1],game,`${game}: Runtime generation belongs to another game`);
    const generationVariant=segments.at(-1)==='multiplayer'?'multiplayer':'normal';
    assert.equal(generationVariant,variant,`${game}: Runtime generation mode mismatch`);
  }
  const hasMultiplayerClaim=claims.includes('multiplayer')||productClaims.includes(`${game}mp`)||manifest?.profile==='multiplayer'||htmlVariant==='multiplayer'||generationRoot?.endsWith('/multiplayer/');
  const hasNormalClaim=claims.includes('normal')||productClaims.includes(game)||manifest?.profile==='production'||htmlVariant==='normal'||generationRoot&&!generationRoot.endsWith('/multiplayer/');
  if(variant==='multiplayer')assert.ok(hasMultiplayerClaim,`${game}: multiplayer package has no variant identity`);
  else assert.ok(hasNormalClaim,`${game}: normal package has no variant identity`);
}

/** Verify a closed Runtime package and return the identity that the browser test must record. */
export async function verifyRuntimePackage(target,{repoRoot}={}) {
  const root=resolve(target.runtime),rootInfo=await lstat(root);
  assert.ok(rootInfo.isDirectory(),`${target.id}: Runtime package must be a directory`);
  const actual=await inventoryFiles(root),hasInventory=actual.includes('runtime-files.json'),hasGeneration=actual.includes('runtime-generation.json');
  assert.ok(hasInventory||hasGeneration,`${target.id}: missing runtime-files.json or runtime-generation.json`);
  assert.ok(!(hasInventory&&hasGeneration),`${target.id}: package has conflicting Runtime inventories`);
  let inventoryBytes,inventory,generationRoot=null,files;
  if(hasInventory){
    inventoryBytes=await readFile(resolve(root,'runtime-files.json'));inventory=JSON.parse(inventoryBytes.toString('utf8'));
    assert.equal(inventory.schema,'eagler-touhou/runtime-directory/1',`${target.id}: unsupported Runtime inventory`);
    files=inventory.files;
    assert.ok(files&&typeof files==='object'&&!Array.isArray(files),`${target.id}: Runtime inventory files must be an object`);
  }else{
    inventoryBytes=await readFile(resolve(root,'runtime-generation.json'));inventory=JSON.parse(inventoryBytes.toString('utf8'));
    assert.equal(inventory.schema,'eagler-touhou/runtime-generation/1',`${target.id}: unsupported Runtime generation descriptor`);
    assert.equal(inventory.protocol,'eagler-touhou/1',`${target.id}: Runtime generation protocol mismatch`);
    generationRoot=inventory.root;
    assert.equal(inventory.entry,target.entry,`${target.id}: generation entry mismatch`);
    const canonical=canonicalRuntimePayload(inventory.entry,inventory.files);
    assert.equal(sha256(Buffer.from(canonical)),inventory.generation,`${target.id}: Runtime generation hash mismatch`);
    files=Object.fromEntries(inventory.files.map(({path,bytes,sha256})=>[path,{bytes,sha256}]));
  }
  for(const name of Object.keys(files))assert.ok(safeRelativePath(name),`${target.id}: unsafe Runtime inventory path ${name}`);
  assert.ok(own(files,target.entry),`${target.id}: Runtime entry is not in the package inventory`);
  for(const name of target.files??[])assert.ok(own(files,name),`${target.id}: requested package file is missing: ${name}`);
  const metadataFiles=Object.keys(files).sort(),packageFiles=actual.filter(name=>name!=='runtime-files.json'&&name!=='runtime-generation.json');
  assert.deepEqual(packageFiles,metadataFiles,`${target.id}: Runtime package inventory is not complete`);
  const identities={};
  for(const name of metadataFiles){
    const bytes=await readFile(resolve(root,name)),expected=files[name];
    assert.ok(expected&&Number.isSafeInteger(expected.bytes)&&expected.bytes>0,`${target.id}: invalid file identity for ${name}`);
    assert.match(expected.sha256,/^[a-f0-9]{64}$/,`${target.id}: invalid SHA-256 for ${name}`);
    assert.equal(bytes.length,expected.bytes,`${target.id}: Runtime size mismatch for ${name}`);
    const digest=sha256(bytes);assert.equal(digest,expected.sha256,`${target.id}: Runtime hash mismatch for ${name}`);
    identities[name]={bytes:bytes.length,sha256:digest};
  }
  const entryBytes=await readFile(resolve(root,target.entry));
  const manifest=own(files,'manifest.json')?JSON.parse((await readFile(resolve(root,'manifest.json'))).toString('utf8')):null;
  if(manifest){
    assert.equal(manifest.game,target.game,`${target.id}: package manifest game mismatch`);
    assert.equal(manifest.protocol,'eagler-touhou/1',`${target.id}: package protocol mismatch`);
    if(manifest.profile!==undefined)assert.equal(manifest.profile,target.variant==='multiplayer'?'multiplayer':'production',`${target.id}: Runtime package profile mismatch`);
  }else assert.ok(generationRoot,`${target.id}: package identity manifest is missing`);
  if(inventory.game!==undefined)assert.equal(inventory.game,target.game,`${target.id}: inventory game mismatch`);
  if(inventory.variant!==undefined)assert.equal(inventory.variant,target.variant,`${target.id}: inventory variant mismatch`);
  validateMode({game:target.game,variant:target.variant,inventory,manifest,entryBytes,generationRoot});
  const wasmNames=metadataFiles.filter(name=>name.endsWith('.wasm'));
  let wasmSha256=null;
  if(manifest?.execution?.sha256){
    assert.equal(wasmNames.length,1,`${target.id}: production manifest must identify exactly one Runtime WASM`);
    wasmSha256=identities[wasmNames[0]].sha256;
    assert.equal(wasmSha256,manifest.execution.sha256,`${target.id}: WASM does not match package execution identity`);
  }else if(target.game==='th09'){
    const version=own(files,'version.json')?JSON.parse((await readFile(resolve(root,'version.json'))).toString('utf8')):null;
    assert.equal(version?.game,'th09',`${target.id}: TH09 build identity is missing`);
    assert.equal(wasmNames.length,1,`${target.id}: TH09 package must contain exactly one WASM`);
    wasmSha256=identities[wasmNames[0]].sha256;
    assert.equal(version.build,wasmSha256.slice(0,24),`${target.id}: TH09 version does not match Runtime WASM`);
  }else if(generationRoot){
    wasmSha256=wasmNames.length===1?identities[wasmNames[0]].sha256:null;
  }else assert.fail(`${target.id}: package does not bind a build identity to its Runtime WASM`);
  const normalizedFiles=Object.fromEntries(metadataFiles.map(name=>[name,identities[name]]));
  const packageSha256=runtimePackageIdentity({game:target.game,variant:target.variant,entry:target.entry,files:normalizedFiles});
  assert.match(target.packageSha256,/^[a-f0-9]{64}$/,`${target.id}: expected packageSha256 is required`);
  assert.equal(packageSha256,target.packageSha256,`${target.id}: package identity does not match the expected current packageSha256`);
  const sourceBuild=await verifySourceBuildReceipt(target,{repoRoot,wasmSha256});
  for(const role of ['execution','loader','entry']){
    const output=sourceBuild.outputs[role];if(!output)continue;
    const name=basename(output.path);
    assert.equal(identities[name]?.sha256,output.sha256,`${target.id}: packaged ${role} is not the current compiler output`);
  }
  const descriptorName=hasInventory?'runtime-files.json':'runtime-generation.json';
  const packageBuild=await verifyPackageBuildReceipt(target,{repoRoot,sourceBuild,packageSha256,
    files:{...normalizedFiles,[descriptorName]:{bytes:inventoryBytes.length,sha256:sha256(inventoryBytes)}}});
  return {game:target.game,variant:target.variant,entry:target.entry,packageSha256,
    inventorySha256:sha256(inventoryBytes),generation:inventory.generation??null,wasmSha256,
    manifestSha256:identities['manifest.json']?.sha256??null,sourceBuild,packageBuild,files:normalizedFiles};
}

/** Serve only bytes that still match the package verified before browser launch. */
export async function readVerifiedRuntimeFile(target,identity,name) {
  assert.ok(safeRelativePath(name),`${target.id}: unsafe Runtime request path ${name}`);
  const expected=identity?.files?.[name];
  assert.ok(expected,`${target.id}: Runtime requested an undeclared file ${name}`);
  const root=resolve(target.runtime),rootInfo=await lstat(root);
  assert.ok(rootInfo.isDirectory()&&!rootInfo.isSymbolicLink(),`${target.id}: Runtime package root changed after identity verification`);
  let path=root;
  for(const part of name.split('/')){
    path=resolve(path,part);
    const info=await lstat(path);
    assert.ok(!info.isSymbolicLink(),`${target.id}: Runtime package path changed to a symbolic link: ${name}`);
  }
  const info=await lstat(path);
  assert.ok(info.isFile(),`${target.id}: Runtime package path is no longer a file: ${name}`);
  assert.ok(inside(root,await realpath(path)),`${target.id}: Runtime package path resolves outside its verified package: ${name}`);
  const bytes=await readFile(path);
  assert.equal(bytes.length,expected.bytes,`${target.id}: Runtime file size changed after identity verification: ${name}`);
  assert.equal(sha256(bytes),expected.sha256,`${target.id}: Runtime file hash changed after identity verification: ${name}`);
  return bytes;
}

export async function verifyPrivateData(path) {
  const info=await lstat(path);
  assert.ok(info.isFile()&&info.size>0,'Private DATA must be a non-empty regular file');
  const bytes=await readFile(path);
  return {bytes:bytes.length,sha256:sha256(bytes)};
}

async function responseBytes(fetchImpl,url,label) {
  const response=await fetchImpl(url,{cache:'no-store',redirect:'error'});
  assert.ok(response.ok,`${label}: HTTP ${response.status} ${url}`);
  return Buffer.from(await response.arrayBuffer());
}

function jsonBytes(bytes,label) {
  try{return JSON.parse(bytes.toString('utf8'));}
  catch(error){throw Error(`${label}: invalid JSON: ${error.message}`);}
}

async function verifyRemoteInventory({fetchImpl,entryUrl,game,variant,source,siteManifestSha256}) {
  const inventoryUrl=new URL('runtime-files.json',entryUrl),inventoryBytes=await responseBytes(fetchImpl,inventoryUrl,'Runtime inventory');
  const inventory=jsonBytes(inventoryBytes,'Runtime inventory');
  assert.equal(inventory.schema,'eagler-touhou/runtime-directory/1','Unsupported Runtime inventory');
  assert.ok(inventory.files&&typeof inventory.files==='object'&&!Array.isArray(inventory.files),'Runtime inventory files must be an object');
  if(inventory.game!==undefined)assert.equal(inventory.game,game,'Runtime inventory game mismatch');
  if(inventory.variant!==undefined)assert.equal(inventory.variant,variant,'Runtime inventory variant mismatch');
  const entry=decodeURIComponent(entryUrl.pathname.slice(entryUrl.pathname.lastIndexOf('/')+1));
  assert.ok(own(inventory.files,entry),'Runtime entry is not in its package inventory');
  const names=Object.keys(inventory.files).sort(),identities={};let total=0;
  for(const name of names){
    assert.ok(safeRelativePath(name),`Unsafe Runtime inventory path ${name}`);
    const expected=inventory.files[name];
    assert.ok(expected&&Number.isSafeInteger(expected.bytes)&&expected.bytes>0,`Invalid Runtime file identity ${name}`);
    assert.match(expected.sha256,/^[a-f0-9]{64}$/,`Invalid Runtime file SHA-256 ${name}`);
    const url=new URL(name,inventoryUrl),bytes=await responseBytes(fetchImpl,url,`Runtime file ${name}`);
    assert.equal(bytes.length,expected.bytes,`Runtime size mismatch ${name}`);
    const digest=sha256(bytes);assert.equal(digest,expected.sha256,`Runtime hash mismatch ${name}`);
    identities[name]={bytes:bytes.length,sha256:digest};total+=bytes.length;
  }
  assert.ok(total<=512*1024*1024,'Runtime package exceeds the 512 MiB package limit');
  const entryBytes=await responseBytes(fetchImpl,entryUrl,'Runtime entry');
  const manifest=own(inventory.files,'manifest.json')?jsonBytes(await responseBytes(fetchImpl,new URL('manifest.json',inventoryUrl),'Runtime manifest'),'Runtime manifest'):null;
  if(manifest){
    assert.equal(manifest.game,game,'Runtime manifest game mismatch');
    assert.equal(manifest.protocol,'eagler-touhou/1','Runtime protocol mismatch');
    if(manifest.profile!==undefined)assert.equal(manifest.profile,variant==='multiplayer'?'multiplayer':'production','Runtime package profile mismatch');
  }
  validateMode({game,variant,inventory,manifest,entryBytes});
  const wasmNames=names.filter(name=>name.endsWith('.wasm'));
  let wasmSha256=null;
  if(manifest?.execution?.sha256){
    assert.equal(wasmNames.length,1,'Runtime manifest must identify exactly one WASM');
    wasmSha256=identities[wasmNames[0]].sha256;
    assert.equal(wasmSha256,manifest.execution.sha256,'WASM does not match package execution identity');
  }else if(game==='th09'){
    const version=own(inventory.files,'version.json')?jsonBytes(await responseBytes(fetchImpl,new URL('version.json',inventoryUrl),'TH09 build identity'),'TH09 build identity'):null;
    assert.equal(version?.game,'th09','TH09 build identity is missing');
    assert.equal(wasmNames.length,1,'TH09 package must contain exactly one WASM');
    wasmSha256=identities[wasmNames[0]].sha256;
    assert.equal(version.build,wasmSha256.slice(0,24),'TH09 version does not match Runtime WASM');
  }else assert.fail('Runtime package does not bind a build identity to its WASM');
  const files=Object.fromEntries(names.map(name=>[name,identities[name]]));
  return {source,game,variant,entryUrl:entryUrl.href,packageSha256:runtimePackageIdentity({game,variant,entry,files}),inventorySha256:sha256(inventoryBytes),
    siteManifestSha256,generation:null,wasmSha256,manifestSha256:identities['manifest.json']?.sha256??null,files};
}

/** Verify the exact Runtime generation the Launcher advertises for a game/mode. */
export async function verifyPublishedRuntime(base,game,variant,{fetchImpl=globalThis.fetch,expectedPackageSha256}={}) {
  assert.ok(PRODUCT_GAMES[game],`Unsupported game ${game}`);
  assert.ok(variant==='normal'||variant==='multiplayer',`Unsupported variant ${variant}`);
  if(variant==='multiplayer')assert.ok(PRODUCT_GAMES[game].multiplayerRuntime,`${game} has no multiplayer Runtime`);
  assert.match(expectedPackageSha256,/^[a-f0-9]{64}$/,`${game}/${variant}: expected current packageSha256 is required`);
  const baseUrl=new URL(base);assert.ok(['http:','https:'].includes(baseUrl.protocol),'Launcher URL must use HTTP or HTTPS');
  assert.ok(!baseUrl.username&&!baseUrl.password&&!baseUrl.hash,'Launcher URL must not contain credentials or a fragment');
  if(!baseUrl.pathname.endsWith('/'))baseUrl.pathname+='/';
  const siteBytes=await responseBytes(fetchImpl,new URL('site-manifest.json',baseUrl),'Launcher site manifest');
  const site=validateSiteManifest(jsonBytes(siteBytes,'Launcher site manifest'));
  const siteManifestSha256=sha256(siteBytes),host=site.games[game];
  assert.ok(host,`Launcher site does not include ${game}`);
  const declared=variant==='multiplayer'?host.multiplayerRuntime:host.runtime;
  assert.ok(typeof declared==='string'&&declared,`Launcher site has no ${game}/${variant} Runtime`);
  const entryUrl=new URL(declared,baseUrl);
  assert.equal(entryUrl.origin,baseUrl.origin,`${game}/${variant}: Runtime is not served by this Launcher origin`);
  assert.ok(!entryUrl.hash,'Runtime entry must not contain a fragment');
  if(site.shared.runtimeManifest){
    assert.equal(site.shared.runtimeManifest,'runtime-manifest.json','Unsupported Runtime manifest pointer');
    const manifestBytes=await responseBytes(fetchImpl,new URL(site.shared.runtimeManifest,baseUrl),'Runtime manifest');
    const runtimeManifest=validateRuntimeManifest(jsonBytes(manifestBytes,'Runtime manifest'));
    const relativePath=entryUrl.pathname.startsWith(baseUrl.pathname)?entryUrl.pathname.slice(baseUrl.pathname.length):entryUrl.pathname.replace(/^\//,'');
    const parsed=parseRuntimeGenerationPath(relativePath);
    assert.ok(parsed,`${game}/${variant}: site Runtime is not an immutable generation`);
    assert.equal(parsed.file,`${game}.html`,`${game}/${variant}: site entry does not match product`);
    const group=runtimeManifest.groups.find(item=>item.root===parsed.root);
    assert.ok(group,`${game}/${variant}: Runtime root is absent from Runtime manifest`);
    assert.equal(group.current.generation,parsed.generation,`${game}/${variant}: Launcher points to a stale Runtime generation`);
    assert.equal(group.current.entry,parsed.file,`${game}/${variant}: Runtime entry mismatch`);
    const generation=group.current;
    assert.equal(sha256(Buffer.from(canonicalRuntimePayload(generation.entry,generation.files))),generation.generation,
      `${game}/${variant}: Runtime generation ID does not match its file identities`);
    const generationBase=runtimeGenerationBase(group.root,generation.generation);
    const generationDescriptorBytes=await responseBytes(fetchImpl,new URL(`${generationBase}runtime-generation.json`,baseUrl),'Runtime generation descriptor');
    const generationDescriptor=jsonBytes(generationDescriptorBytes,'Runtime generation descriptor');
    assert.equal(generationDescriptor.schema,'eagler-touhou/runtime-generation/1',`${game}/${variant}: unsupported Runtime generation descriptor`);
    assert.equal(generationDescriptor.protocol,'eagler-touhou/1',`${game}/${variant}: Runtime generation protocol mismatch`);
    assert.equal(generationDescriptor.root,group.root,`${game}/${variant}: Runtime generation root mismatch`);
    assert.equal(generationDescriptor.generation,generation.generation,`${game}/${variant}: Runtime generation descriptor is stale`);
    assert.equal(generationDescriptor.entry,generation.entry,`${game}/${variant}: Runtime generation descriptor entry mismatch`);
    assert.deepEqual(generationDescriptor.files,generation.files,`${game}/${variant}: Runtime generation descriptor differs from the current manifest`);
    let total=0;const files={};
    for(const file of generation.files){
      const url=new URL(`${generationBase}${file.path}`,baseUrl);
      assert.equal(url.origin,baseUrl.origin,`${game}/${variant}: Runtime file escaped Launcher origin`);
      const bytes=await responseBytes(fetchImpl,url,`Runtime generation file ${file.path}`);
      assert.equal(bytes.length,file.bytes,`Runtime generation size mismatch ${file.path}`);
      const digest=sha256(bytes);assert.equal(digest,file.sha256,`Runtime generation hash mismatch ${file.path}`);
      files[file.path]={bytes:bytes.length,sha256:digest};total+=bytes.length;
    }
    assert.ok(total<=512*1024*1024,'Runtime generation exceeds the 512 MiB package limit');
    const entryBytes=await responseBytes(fetchImpl,entryUrl,'Runtime generation entry');
    const manifest=files['manifest.json']?jsonBytes(await responseBytes(fetchImpl,new URL(`${generationBase}manifest.json`,baseUrl),'Runtime manifest'),'Runtime manifest'):null;
    if(manifest){
      assert.equal(manifest.game,game,`${game}/${variant}: package manifest game mismatch`);
      assert.equal(manifest.protocol,'eagler-touhou/1',`${game}/${variant}: package protocol mismatch`);
      if(manifest.profile!==undefined)assert.equal(manifest.profile,variant==='multiplayer'?'multiplayer':'production',`${game}/${variant}: Runtime package profile mismatch`);
    }
    validateMode({game,variant,inventory:{game:group.root.split('/')[1]},manifest,entryBytes,generationRoot:group.root});
    const wasmNames=Object.keys(files).filter(name=>name.endsWith('.wasm'));
    let wasmSha256=null;
    if(manifest?.execution?.sha256){
      assert.equal(wasmNames.length,1,`${game}/${variant}: manifest must identify exactly one Runtime WASM`);
      wasmSha256=files[wasmNames[0]].sha256;
      assert.equal(wasmSha256,manifest.execution.sha256,`${game}/${variant}: WASM does not match package execution identity`);
    }else if(game==='th09'){
      const version=jsonBytes(await responseBytes(fetchImpl,new URL(`${generationBase}version.json`,baseUrl),'TH09 build identity'),'TH09 build identity');
      assert.equal(version.game,'th09',`${game}/${variant}: TH09 build identity is missing`);
      assert.equal(wasmNames.length,1,`${game}/${variant}: TH09 package must contain exactly one WASM`);
      wasmSha256=files[wasmNames[0]].sha256;
      assert.equal(version.build,wasmSha256.slice(0,24),`${game}/${variant}: version does not match Runtime WASM`);
    }else assert.fail(`${game}/${variant}: package does not bind a build identity to its WASM`);
    const normalizedFiles=Object.fromEntries(Object.keys(files).sort().map(name=>[name,files[name]]));
    const packageSha256=runtimePackageIdentity({game,variant,entry:generation.entry,files:normalizedFiles});
    assert.equal(packageSha256,expectedPackageSha256,`${game}/${variant}: published Runtime package does not match the source-verified current packageSha256`);
    return {source:'current-published-generation',game,variant,entryUrl:entryUrl.href,root:group.root,generation:generation.generation,
      packageSha256,runtimeManifestSha256:sha256(manifestBytes),siteManifestSha256,
      generationDescriptorSha256:sha256(generationDescriptorBytes),wasmSha256,manifestSha256:normalizedFiles['manifest.json']?.sha256??null,files:normalizedFiles};
  }
  assert.equal(site.profile,'web-development','Launcher has no current immutable Runtime manifest');
  assert.equal(site.shared.testBuild,true,'Unpublished Runtime requires the explicit development test-build marker');
  const result=await verifyRemoteInventory({fetchImpl,entryUrl,game,variant,source:'validated-development-package',siteManifestSha256});
  assert.equal(result.packageSha256,expectedPackageSha256,`${game}/${variant}: development Runtime package does not match the source-verified current packageSha256`);
  return result;
}
