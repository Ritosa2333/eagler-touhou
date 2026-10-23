#!/usr/bin/env node
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {lstatSync, readFileSync} from 'node:fs';
import {lstat,readFile,realpath} from 'node:fs/promises';
import {dirname, isAbsolute, relative, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawn} from 'node:child_process';
import {runWithReport} from '../testing/cli-report.mjs';
import {selectedPaths} from '../workspace.mjs';
import {verifyRuntimePackage, verifySourceBuildReceipt} from './acceptance-contracts.mjs';
import {runtimeBuildPlanIdentity, runtimeBuildSourceRoot} from '../../launcher/lib/runtime-build-evidence.mjs';
import {resolveRuntimePackagePlan, runtimePackageReceiptPath} from '../../launcher/lib/runtime-package-evidence.mjs';

const REPOSITORY=resolve(dirname(fileURLToPath(import.meta.url)),'../..');
const SCHEMA='eagler-touhou/build-acceptance/1';
const INPUTS_SCHEMA='eagler-touhou/build-acceptance-inputs/1';
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const own=(value,key)=>Object.prototype.hasOwnProperty.call(value,key);
const BUILD_OPTION_NAMES=Object.freeze(['emsdk','emscripten-version','python','jobs','cmake','ninja','assets','font','imgui',
  'runtime-extension','source','common','netplay-common','build-directory','thcrap','thprac']);
const PATH_OPTIONS=new Set(['emsdk','assets','font','imgui','runtime-extension','source','common','netplay-common','build-directory']);
const EXECUTABLE_OPTIONS=new Set(['python','cmake','ninja']);
const buildReceiptPath=(game,variant)=>`artifacts/validation/source-builds/${game}-${variant}.runtime.json`;
const buildPlanPath=(game,variant)=>`artifacts/validation/source-builds/${game}-${variant}.json`;

function readJson(root,path){return JSON.parse(readFileSync(resolve(root,path),'utf8'));}

/** Resolve source ownership from the same workspace and build catalog used by the official commands. */
export function resolveBuildAcceptanceTarget(root,game,variant){
  root=resolve(root);
  assert.ok(typeof game==='string'&&game&&['normal','multiplayer'].includes(variant),
    'A product and normal or multiplayer variant are required');
  const workspace=readJson(root,'launcher/config/workspace.json');
  const builds=readJson(root,'launcher/config/runtime-builds.json');
  const entry=builds.games?.[game],definition=entry?.variants?.[variant];
  assert.ok(entry&&definition,`Runtime build is not registered for ${game}/${variant}`);
  const owner=definition.workspaceRepository??entry.workspaceRepository;
  const sourcePath=workspace.repositories?.[owner];
  assert.ok(typeof sourcePath==='string'&&sourcePath,`Workspace has no source owner ${owner}`);
  const sourceRoot=resolve(root,sourcePath);
  const repoRelative=relative(root,sourceRoot).replaceAll('\\','/');
  assert.ok(repoRelative&&!repoRelative.startsWith('../')&&repoRelative!=='..',
    `Source owner escapes the repository: ${sourcePath}`);
  const runtime=resolveRuntimePackagePlan(game,variant,root);
  assert.equal(resolve(runtime.source),sourceRoot,`${game}/${variant} package and build owners differ`);
  return Object.freeze({game,variant,owner,sourceRoot,sourcePath:repoRelative,
    buildReceipt:buildReceiptPath(game,variant),buildPlan:buildPlanPath(game,variant),
    packageReceipt:relative(root,runtimePackageReceiptPath(game,variant,root)).replaceAll('\\','/'),
    runtimeRoot:runtime.output,entry:runtime.entry,packagePlan:runtime});
}

/** Read one product/mode's private build inputs without allowing the manifest to change the requested target. */
export function loadBuildAcceptanceInputs({root=REPOSITORY,manifestPath,game,variant}={}){
  root=resolve(root);
  assert.ok(typeof manifestPath==='string'&&manifestPath,'A local build-input manifest path is required');
  assert.ok(typeof game==='string'&&game&&['normal','multiplayer'].includes(variant),
    'Manifest selection requires an explicit product and variant');
  const absoluteManifest=resolve(root,manifestPath);
  const bytes=readFileSync(absoluteManifest);
  const manifest=JSON.parse(bytes.toString('utf8'));
  assert.ok(manifest&&typeof manifest==='object'&&!Array.isArray(manifest),'Build-input manifest must be an object');
  assert.deepEqual(Object.keys(manifest).sort(),['schema','targets'],'Build-input manifest has unknown fields');
  assert.equal(manifest?.schema,INPUTS_SCHEMA,`Unsupported build-input manifest schema in ${manifestPath}`);
  assert.ok(Array.isArray(manifest.targets)&&manifest.targets.length>0,'Build-input manifest has no targets');

  const builds=readJson(root,'launcher/config/runtime-builds.json');
  const seen=new Set();
  for(const target of manifest.targets){
    assert.ok(target&&typeof target==='object'&&!Array.isArray(target),'Manifest target must be an object');
    assert.ok(Object.keys(target).every(key=>['game','variant','options','fontRoot'].includes(key)),
      'Manifest targets contain an unknown field');
    assert.ok(['game','options','variant'].every(key=>own(target,key)),
      'Manifest targets require game, variant, and options');
    assert.ok(typeof target.game==='string'&&target.game&&['normal','multiplayer'].includes(target.variant),
      'Manifest target requires a product and normal or multiplayer variant');
    const key=`${target.game}/${target.variant}`;
    assert.ok(!seen.has(key),`Duplicate build-input target ${key}`);
    seen.add(key);
    assert.ok(builds.games?.[target.game]?.variants?.[target.variant],`Unknown build-input target ${key}`);
    const options=target.options??{};
    assert.ok(options&&typeof options==='object'&&!Array.isArray(options),'Manifest target options must be an object');
    for(const [name,value] of Object.entries(options)){
      assert.ok(BUILD_OPTION_NAMES.includes(name),`Unknown official build option ${name}`);
      assert.ok(typeof value==='string'&&value.length>0,`Build option ${name} must be a non-empty string`);
      if(name==='thcrap'||name==='thprac')assert.ok(value==='on'||value==='off',`${name} must be on or off`);
    }
    if(own(target,'fontRoot'))assert.ok(typeof target.fontRoot==='string'&&target.fontRoot.length>0,
      'fontRoot must be a non-empty directory path');
  }
  const selected=manifest.targets.filter(target=>target.game===game&&target.variant===variant);
  assert.equal(selected.length,1,`Build-input manifest must contain exactly one ${game}/${variant} target`);
  const target=selected[0];
  const manifestDirectory=dirname(absoluteManifest);
  const fontRoot=own(target,'fontRoot')?resolve(manifestDirectory,target.fontRoot):null;
  const resolvedOptions={};
  for(const name of BUILD_OPTION_NAMES){
    if(!own(target.options??{},name))continue;
    const value=target.options[name];
    const explicitExecutablePath=EXECUTABLE_OPTIONS.has(name)&&
      (isAbsolute(value)||value.startsWith('.')||value.includes('/')||value.includes('\\'));
    resolvedOptions[name]=(PATH_OPTIONS.has(name)||explicitExecutablePath)?resolve(manifestDirectory,value):value;
  }
  const buildArguments=BUILD_OPTION_NAMES.filter(name=>own(resolvedOptions,name))
    .map(name=>`--${name}=${resolvedOptions[name]}`);
  return {schema:INPUTS_SCHEMA,path:relative(root,absoluteManifest).replaceAll('\\','/'),
    sha256:hash(bytes),game,variant,fontRoot,options:resolvedOptions,buildArguments};
}

/** Pure directory ownership check so single-product checkouts can be verified without building. */
export function assessCheckoutDirectories({game,variant,requiredPaths,owners,presentPaths}){
  assert.ok(typeof game==='string'&&game&&['normal','multiplayer'].includes(variant),
    'Checkout scope requires a product and variant');
  assert.ok(Array.isArray(requiredPaths)&&requiredPaths.length&&Array.isArray(owners)&&Array.isArray(presentPaths),
    'Checkout inventory is incomplete');
  const present=new Set(presentPaths);
  const missing=requiredPaths.filter(path=>!present.has(path));
  const selectedOwners=new Set(owners.filter(owner=>owner.game===game).map(owner=>owner.path));
  assert.ok(selectedOwners.size>0,'Checkout inventory does not identify the selected product source owner');
  assert.ok([...selectedOwners].some(path=>requiredPaths.includes(path)),
    'Required checkout paths do not include the selected product source owner');
  const excluded=[...new Set(owners.filter(owner=>owner.game!==game&&!selectedOwners.has(owner.path))
    .map(owner=>owner.path))].sort();
  const unexpected=excluded.filter(path=>present.has(path));
  assert.deepEqual(missing,[],`Minimal checkout is missing required paths: ${missing.join(', ')}`);
  assert.deepEqual(unexpected,[],`Minimal checkout contains another product's source owner: ${unexpected.join(', ')}`);
  return {game,variant,requiredPaths:[...requiredPaths],selectedOwners:[...selectedOwners].sort(),
    absentOtherProductOwners:excluded,unexpectedOtherProductOwners:unexpected};
}

/** Inspect the current sparse checkout. BUILD-05 still needs the browser's RUNTIME-05 journey. */
export function inspectMinimalCheckout({root=REPOSITORY,game,variant}){
  root=resolve(root);
  assert.equal(root,REPOSITORY,'Checkout inspection must examine the repository that owns this acceptance driver');
  const workspace=readJson(root,'launcher/config/workspace.json');
  const builds=readJson(root,'launcher/config/runtime-builds.json');
  const selected=resolveBuildAcceptanceTarget(root,game,variant);
  const requiredPaths=selectedPaths(game,variant);
  assert.ok(requiredPaths.includes(selected.sourcePath),
    `Workspace selection does not include the configured ${game}/${variant} source owner`);
  const owners=[];
  for(const [ownerGame,entry] of Object.entries(builds.games??{})){
    for(const [ownerVariant,definition] of Object.entries(entry.variants??{})){
      const owner=definition.workspaceRepository??entry.workspaceRepository;
      const path=workspace.repositories?.[owner];
      if(typeof path==='string')owners.push({game:ownerGame,variant:ownerVariant,path});
    }
  }
  const allPaths=[...new Set([...requiredPaths,...owners.map(owner=>owner.path)])];
  const presentPaths=allPaths.filter(path=>{
    try{return lstatSync(resolve(root,path)).isDirectory()||lstatSync(resolve(root,path)).isSymbolicLink();}catch{return false;}
  });
  const invalidRequired=requiredPaths.filter(path=>{
    try{const stat=lstatSync(resolve(root,path));return !stat.isDirectory()||stat.isSymbolicLink();}catch{return true;}
  });
  assert.deepEqual(invalidRequired,[],`Minimal checkout required paths are missing or not directories: ${invalidRequired.join(', ')}`);
  const directories=assessCheckoutDirectories({game,variant,requiredPaths,owners,presentPaths});
  return {schema:'eagler-touhou/minimal-checkout/1',...directories,
    targetSourceRoot:selected.sourcePath,
    sourceBuild:{receipt:selected.buildReceipt,plan:selected.buildPlan},
    browserAcceptanceRequired:'RUNTIME-05'};
}

function check(id,game,variant,observation,status='passed',complete=false){
  return {id,status,complete,scope:`${game}/${variant}`,evidence:{game,variant,adaptationIds:[id],observation}};
}

async function runOfficial(script,args,{root,environmentOverrides={},invoke=invokeOfficial}={}){
  const startedAt=new Date().toISOString();
  const result=await invoke({script,args,cwd:root,environmentOverrides});
  const finishedAt=new Date().toISOString();
  assert.ok(result&&Number.isInteger(result.exitCode),`${script}: official command did not return an exit code`);
  return {script:relative(root,script).replaceAll('\\','/'),startedAt,finishedAt,
    exitCode:result.exitCode,processId:Number.isSafeInteger(result.pid)?result.pid:null,
    signal:result.signal??null,
    argumentSha256:hash(Buffer.from(JSON.stringify(args))),
    ...(Object.keys(environmentOverrides).length?{environmentOverrideNames:Object.keys(environmentOverrides).sort(),
      environmentOverridesSha256:hash(Buffer.from(JSON.stringify(environmentOverrides)))}:{})};
}

function invokeOfficial({script,args,cwd,environmentOverrides={}}){
  assert.ok(environmentOverrides&&typeof environmentOverrides==='object'&&!Array.isArray(environmentOverrides),
    'Official subprocess environment overrides must be an object');
  assert.ok(Object.keys(environmentOverrides).every(key=>key==='EAGLER_FONT_ROOT'),
    'Only EAGLER_FONT_ROOT may be overridden by build acceptance inputs');
  if(Object.hasOwn(environmentOverrides,'EAGLER_FONT_ROOT'))
    assert.ok(typeof environmentOverrides.EAGLER_FONT_ROOT==='string'&&environmentOverrides.EAGLER_FONT_ROOT,
      'EAGLER_FONT_ROOT override must be a non-empty path');
  return new Promise((resolvePromise,reject)=>{
    const child=spawn(process.execPath,[script,...args],{cwd,windowsHide:true,stdio:'inherit',
      ...(Object.keys(environmentOverrides).length?{env:{...process.env,...environmentOverrides}}:{})});
    child.once('error',reject);
    child.once('exit',(code,signal)=>resolvePromise({exitCode:code??1,signal,pid:child.pid}));
  });
}

/** Package stages reuse the selected font directory; compile-only options stay out of package-runtime. */
export function packageOfficialRequest({game,variant,fontRoot}={}){
  assert.ok(typeof game==='string'&&game&&['normal','multiplayer'].includes(variant));
  return {args:[`--game=${game}`,`--variant=${variant}`],
    environmentOverrides:fontRoot?{EAGLER_FONT_ROOT:fontRoot}:{}};
}

function sourceBuildTarget(target){return {id:`build-${target.game}-${target.variant}`,
  game:target.game,variant:target.variant,sourceRoot:target.sourcePath,buildReceipt:target.buildReceipt,
  packageReceipt:target.packageReceipt};}

async function sourceBuildObservation({root,target,invocation}){
  assert.equal(invocation.exitCode,0,'Official Runtime build command did not succeed');
  const planBytes=await readFile(resolve(root,target.buildPlan));
  const receiptBytes=await readFile(resolve(root,target.buildReceipt));
  const plan=JSON.parse(planBytes.toString('utf8'));
  const receipt=JSON.parse(receiptBytes.toString('utf8'));
  assert.equal(plan.game,target.game,'Official build plan belongs to another product');
  assert.equal(plan.variant,target.variant,'Official build plan belongs to another variant');
  assert.equal(resolve(root,plan.source),target.sourceRoot,'Official build plan uses another source owner');
  const sourceBuild=await verifySourceBuildReceipt(sourceBuildTarget(target),{
    repoRoot:root,wasmSha256:receipt.executionSha256,
  });
  assert.equal(runtimeBuildPlanIdentity(plan),receipt.planSha256,
    'Runtime receipt is not bound to the official source build plan');
  assert.ok(typeof plan.toolchain?.version==='string'&&plan.toolchain.version,
    'Build plan does not identify the selected Emscripten version');
  return {invocation,plan:{path:target.buildPlan,sha256:hash(planBytes),planSha256:receipt.planSha256,
      game:plan.game,variant:plan.variant,owner:plan.owner,source:target.sourcePath,
      common:relative(root,plan.common).replaceAll('\\','/'),toolchain:{version:plan.toolchain.version,
        compiler:plan.toolchain.compiler,root:plan.toolchain.root},features:plan.features,
      requiredSubmodules:receipt.requiredSubmodules},
    receipt:{path:target.buildReceipt,sha256:hash(receiptBytes),schema:sourceBuild.schema,
      sourceInputsSha256:sourceBuild.sourceInputsSha256,sourceFilesSha256:sourceBuild.sourceFilesSha256,
      executionSha256:sourceBuild.executionSha256,planSha256:sourceBuild.planSha256,
      outputCount:Object.keys(sourceBuild.outputs).length,sourceCount:sourceBuild.sourceCount}};
}

async function packageObservation({root,target,invocation}){
  assert.equal(invocation.exitCode,0,'Official Runtime packager did not succeed');
  const packageReceiptPath=resolve(root,target.packageReceipt);
  const bytes=await readFile(packageReceiptPath),receipt=JSON.parse(bytes.toString('utf8'));
  // Official workspace outputs may be junctions to an external build volume.
  // Resolve that configured output here; arbitrary package inputs stay strict.
  const runtimeRoot=await realpath(target.runtimeRoot);
  const identity=await verifyRuntimePackage({id:`build-${target.game}-${target.variant}`,game:target.game,
    variant:target.variant,runtime:runtimeRoot,entry:target.entry,
    sourceRoot:target.sourcePath,buildReceipt:target.buildReceipt,
    packageReceipt:target.packageReceipt,packageSha256:receipt.packageSha256},{repoRoot:root});
  return {invocation,package:{path:target.packageReceipt,output:target.runtimeRoot,runtimeRoot,receiptSha256:hash(bytes),schema:identity.packageBuild.schema,
    packageSha256:identity.packageSha256,wasmSha256:identity.wasmSha256,manifestSha256:identity.manifestSha256,
    sourceBuildReceiptSha256:identity.packageBuild.sourceBuildReceiptSha256,
    sourceInputsSha256:identity.packageBuild.sourceInputsSha256,
    fileCount:Object.keys(identity.files).length,entry:identity.entry}};
}

function validReportTime(value,label){
  const time=Date.parse(value);
  assert.ok(typeof value==='string'&&Number.isFinite(time),`${label} is missing a valid timestamp`);
  return time;
}

function validateProducerInvocation(invocation,{root,stage,game,variant}){
  assert.ok(invocation&&typeof invocation==='object'&&!Array.isArray(invocation),`${stage} report has no invocation`);
  const script=stage==='source-build'?'launcher/scripts/build-runtime.mjs':'launcher/scripts/package-runtime.mjs';
  assert.equal(invocation.script,script,`${stage} report did not use the official command`);
  assert.equal(invocation.exitCode,0,`${stage} producer invocation did not succeed`);
  const started=validReportTime(invocation.startedAt,`${stage} invocation start`);
  const finished=validReportTime(invocation.finishedAt,`${stage} invocation finish`);
  assert.ok(finished>=started,`${stage} invocation time range is invalid`);
  assert.match(invocation.argumentSha256,/^[a-f0-9]{64}$/,`${stage} invocation arguments are unidentified`);
  if(invocation.environmentOverrideNames!==undefined){
    assert.deepEqual(invocation.environmentOverrideNames,['EAGLER_FONT_ROOT'],`${stage} report records an unsupported environment override`);
    assert.match(invocation.environmentOverridesSha256,/^[a-f0-9]{64}$/,`${stage} environment override is unidentified`);
  }
  if(invocation.inputManifestSha256!==undefined)
    assert.match(invocation.inputManifestSha256,/^[a-f0-9]{64}$/,`${stage} input manifest is unidentified`);
  return{...invocation,game,variant};
}

async function readProducerReport({root,manifestPath,reference,stage,game,variant,target,observe}){
  if(typeof reference!=='string'||!reference.trim()){
    const error=new Error(`Runtime manifest has no ${stage} report reference`);error.code='ERR_BUILD_EVIDENCE_MISSING';throw error;
  }
  const path=isAbsolute(reference)?resolve(reference):resolve(dirname(manifestPath),reference);
  let info;
  try{info=await lstat(path);}catch(error){
    if(error?.code==='ENOENT'){const missing=new Error(`Referenced ${stage} report is missing: ${reference}`);missing.code='ERR_BUILD_EVIDENCE_MISSING';throw missing;}
    throw error;
  }
  assert.ok(info.isFile()&&!info.isSymbolicLink(),`${stage} report must be a regular file`);
  const actualPath=await realpath(path);
  const bytes=await readFile(actualPath),raw=bytes.toString('utf8'),producer=JSON.parse(raw);
  assert.equal(producer.schema,SCHEMA,`${stage} report schema mismatch`);
  assert.equal(producer.stage,stage,`Referenced report is not the ${stage} stage`);
  assert.deepEqual(producer.scope,{game,variant},`${stage} report belongs to another product or variant`);
  assert.ok(typeof producer.runId==='string'&&producer.runId.trim(),`${stage} report is not sealed with a run ID`);
  const started=validReportTime(producer.startedAt,`${stage} report start`);
  const finished=validReportTime(producer.finishedAt,`${stage} report finish`);
  assert.ok(finished>=started,`${stage} report time range is invalid`);
  assert.equal(producer.status,'passed',`${stage} report did not pass`);
  assert.equal(producer.passed,true,`${stage} report is not sealed as passed`);
  const invocation=validateProducerInvocation(producer.invocations?.length===1?producer.invocations[0]:null,
    {root,stage,game,variant});
  assert.ok(Date.parse(invocation.startedAt)>=started&&Date.parse(invocation.finishedAt)<=finished,
    `${stage} invocation is outside the sealed report time range`);
  const current=await observe({root,target,invocation:producer.invocations[0]});
  assert.deepEqual(producer.observations?.[stage==='source-build'?'sourceBuild':'package'],current,
    `${stage} report observations no longer match current source, plan, toolchain or package receipts`);
  const expectedIds=stage==='source-build'?['BUILD-01','BUILD-02','BUILD-03']:['BUILD-04'];
  const claims=producer.checks;
  assert.ok(Array.isArray(claims)&&claims.length===expectedIds.length&&
    expectedIds.every(id=>claims.filter(claim=>claim?.id===id).length===1),
  `${stage} report does not contain its full official check set`);
  for(const id of expectedIds){
    const claim=claims.find(item=>item.id===id);
    assert.equal(claim.status,'passed',`${stage} report check ${id} did not pass`);
    assert.equal(claim.complete,false,`${stage} stage report cannot supply a revalidation completion claim`);
    assert.equal(claim.scope,`${game}/${variant}`,`${stage} report check ${id} has another scope`);
    assert.deepEqual(claim.evidence?.adaptationIds,[id],`${stage} report check ${id} is not scoped to its ID`);
  }
  return{report:{path:reference.replaceAll('\\','/'),bytes:bytes.length,sha256:hash(bytes),rawJson:raw,
    runId:producer.runId,startedAt:producer.startedAt,finishedAt:producer.finishedAt,stage,
    invocation:producer.invocations[0]},observation:current};
}

/** Re-read old producer reports and independently verify them against the current source and package receipts. */
export async function inspectExistingBuildAcceptance({root=REPOSITORY,manifestPath,game,variant,
  targetResolver=resolveBuildAcceptanceTarget,sourceObserver=sourceBuildObservation,packageObserver=packageObservation}={}){
  root=resolve(root);
  const absoluteManifest=resolve(root,manifestPath);
  const manifestBytes=await readFile(absoluteManifest),manifest=JSON.parse(manifestBytes.toString('utf8'));
  assert.ok(manifest&&typeof manifest==='object'&&!Array.isArray(manifest),'Runtime manifest must be an object');
  assert.ok(Array.isArray(manifest.targets),'Runtime manifest has no targets');
  const matches=manifest.targets.filter(item=>item?.game===game&&item?.variant===variant);
  assert.equal(matches.length,1,`Runtime manifest must contain exactly one ${game}/${variant} target`);
  const manifestTarget=matches[0],references=manifestTarget.buildAcceptance;
  if(!references||typeof references!=='object'||Array.isArray(references)){
    const error=new Error(`Runtime target ${game}/${variant} has no buildAcceptance report references`);error.code='ERR_BUILD_EVIDENCE_MISSING';throw error;
  }
  assert.deepEqual(Object.keys(references).sort(),['packageReport','sourceBuildReport'],
    'buildAcceptance requires only sourceBuildReport and packageReport');
  const target=targetResolver(root,game,variant);
  for(const [field,expected] of Object.entries({game,variant,entry:target.entry,sourceRoot:target.sourcePath,
    buildReceipt:target.buildReceipt,packageReceipt:target.packageReceipt}))
    assert.equal(field==='game'?manifestTarget.game:field==='variant'?manifestTarget.variant:manifestTarget[field],expected,
      `Runtime manifest ${field} does not match the official ${game}/${variant} target`);
  const manifestRuntime=resolve(root,manifestTarget.runtime),officialRuntime=resolve(root,target.runtimeRoot);
  let sameRuntime=false;
  try{sameRuntime=(await realpath(manifestRuntime))===(await realpath(officialRuntime));}
  catch(error){if(error?.code!=='ENOENT')throw error;sameRuntime=manifestRuntime===officialRuntime;}
  assert.ok(sameRuntime,'Runtime manifest points to another package output');
  const sourceBuild=await readProducerReport({root,manifestPath:absoluteManifest,reference:references.sourceBuildReport,
    stage:'source-build',game,variant,target,observe:sourceObserver});
  const packaged=await readProducerReport({root,manifestPath:absoluteManifest,reference:references.packageReport,
    stage:'package',game,variant,target,observe:packageObserver});
  assert.equal(manifestTarget.packageSha256,packaged.observation.package.packageSha256,
    'Runtime manifest package identity differs from the current official package');
  assert.equal(packaged.observation.package.sourceBuildReceiptSha256,sourceBuild.observation.receipt.sha256,
    'Package report is not bound to the selected source-build receipt');
  assert.equal(packaged.observation.package.sourceInputsSha256,sourceBuild.observation.receipt.sourceInputsSha256,
    'Package and source-build reports identify different source input sets');
  assert.equal(packaged.observation.package.wasmSha256,sourceBuild.observation.receipt.executionSha256,
    'Package and source-build reports identify different Runtime execution files');
  const manifestIdentity={path:relative(root,absoluteManifest).replaceAll('\\','/'),sha256:hash(manifestBytes),
    target:{game,variant,runtime:manifestTarget.runtime,entry:manifestTarget.entry,sourceRoot:manifestTarget.sourceRoot,
      buildReceipt:manifestTarget.buildReceipt,packageReceipt:manifestTarget.packageReceipt,
      packageSha256:manifestTarget.packageSha256},
    reports:{sourceBuildReport:references.sourceBuildReport,packageReport:references.packageReport}};
  return{manifest:manifestIdentity,target:{game,variant,owner:target.owner,sourceRoot:target.sourcePath,
    buildPlan:target.buildPlan,buildReceipt:target.buildReceipt,packageReceipt:target.packageReceipt,
    runtime:target.runtimeRoot,entry:target.entry,packageSha256:packaged.observation.package.packageSha256},
    sourceBuild,package:packaged,verification:{mode:'revalidated-existing-stage-reports',checkedAt:new Date().toISOString()}};
}

export function parseBuildAcceptanceOptions(argv){
  const options={buildArguments:[]};
  for(const arg of argv){
    const match=arg.match(/^--([^=]+)=(.*)$/);
    if(!match)throw new Error(`Invalid build acceptance argument: ${arg}`);
    const [,name,value]=match;
    if(['game','variant','stage','output','manifest','runtime-manifest'].includes(name)){
      const key=name==='runtime-manifest'?'runtimeManifest':name;
      if(own(options,key))throw new Error(`Duplicate --${name}`);
      options[key]=value;
    }else if(BUILD_OPTION_NAMES.includes(name)){
      options.buildArguments.push(arg);
    }else throw new Error(`Unknown build acceptance option: --${name}`);
  }
  assert.ok(options.game&&['normal','multiplayer'].includes(options.variant),
    'Specify --game and --variant=normal|multiplayer');
  assert.ok(['source-build','package','checkout','provenance','verify-existing'].includes(options.stage),
    'Specify --stage=source-build|package|checkout|provenance|verify-existing');
  assert.ok(options.output,'Specify --output=<evidence directory>');
  if(own(options,'manifest'))assert.ok(options.manifest,'Specify a non-empty --manifest path');
  if(own(options,'runtimeManifest'))assert.ok(options.runtimeManifest,'Specify a non-empty --runtime-manifest path');
  assert.ok(!options.manifest||options.stage==='source-build'||options.stage==='package',
    'Build-input --manifest is used only by source-build and package stages');
  assert.ok(!options.runtimeManifest||options.stage==='verify-existing',
    '--runtime-manifest is used only by verify-existing');
  assert.ok(!options.manifest||options.buildArguments.length===0,
    'Use either --manifest or command-line build options, not both');
  return options;
}

/** Execute one official build stage and seal its exact receipt identity for the adaptation runner. */
export async function runBuildAcceptance({root=REPOSITORY,game,variant,stage,output,manifest,runtimeManifest,buildArguments=[]}={},
  dependencies={}){
  root=resolve(root);output=resolve(output);
  let inputManifest=null;
  if(manifest){
    assert.ok(['source-build','package'].includes(stage),
      'Build-input manifests are used only by source-build and package stages');
    assert.deepEqual(buildArguments,[],'Do not combine manifest inputs with command-line build options');
    inputManifest=loadBuildAcceptanceInputs({root,manifestPath:manifest,game,variant});
    if(stage==='source-build')buildArguments=inputManifest.buildArguments;
  }
  const target=resolveBuildAcceptanceTarget(root,game,variant);
  return runWithReport({output,schema:SCHEMA},async(report,save)=>{
    report.scope={game,variant};report.stage=stage;report.checks=[];report.observations={};
    if(inputManifest)report.inputManifest={schema:inputManifest.schema,path:inputManifest.path,
      sha256:inputManifest.sha256,fontRoot:inputManifest.fontRoot,options:inputManifest.options};
    if(stage==='source-build'){
      const invocation=await runOfficial(resolve(root,'launcher/scripts/build-runtime.mjs'),
        [`--game=${game}`,`--variant=${variant}`,...buildArguments],{root,
          environmentOverrides:inputManifest?.fontRoot?{EAGLER_FONT_ROOT:inputManifest.fontRoot}:{}});
      if(inputManifest)invocation.inputManifestSha256=inputManifest.sha256;
      report.invocations=[invocation];await save();
      const evidence=await sourceBuildObservation({root,target,invocation});
      report.observations.sourceBuild=evidence;
      const ownerObservation={owner:target.owner,sourceRoot:target.sourcePath,game,variant,
        runtimeSourceRoot:runtimeBuildSourceRoot(game,variant,root)};
      report.checks.push(check('BUILD-01',game,variant,ownerObservation));
      report.checks.push(check('BUILD-02',game,variant,{toolchain:evidence.plan.toolchain,
        common:evidence.plan.common,features:evidence.plan.features,requiredSubmodules:evidence.plan.requiredSubmodules,
        planSha256:evidence.plan.planSha256,sourceInputsSha256:evidence.receipt.sourceInputsSha256}));
      report.checks.push(check('BUILD-03',game,variant,{invocation:evidence.invocation,
        planSha256:evidence.receipt.planSha256,executionSha256:evidence.receipt.executionSha256,
        sourceBuildReceipt:{path:evidence.receipt.path,sha256:evidence.receipt.sha256}}));
      report.status='passed';report.passed=true;
    }else if(stage==='package'){
      assert.equal(buildArguments.length,0,'Build-only options are not accepted by the package stage');
      const request=packageOfficialRequest({game,variant,fontRoot:inputManifest?.fontRoot});
      const invocation=await runOfficial(resolve(root,'launcher/scripts/package-runtime.mjs'),
        request.args,{root,environmentOverrides:request.environmentOverrides});
      if(inputManifest)invocation.inputManifestSha256=inputManifest.sha256;
      report.invocations=[invocation];await save();
      const evidence=await packageObservation({root,target,invocation});
      report.observations.package=evidence;
      report.checks.push(check('BUILD-04',game,variant,evidence));
      report.status='passed';report.passed=true;
    }else if(stage==='checkout'){
      assert.equal(buildArguments.length,0,'Build options are not accepted by checkout inspection');
      const observation={...inspectMinimalCheckout({root,game,variant}),status:'passed'};
      report.observations.checkout=observation;
      report.status='incomplete';report.passed=false;
      report.incompleteReason='Checkout scope is verified; BUILD-05 also requires the same run to pass the browser RUNTIME-05 first-frame journey.';
    }else if(stage==='provenance'){
      assert.equal(buildArguments.length,0,'Build options are not accepted by provenance review');
      let sourceBuild=null;
      try{
        const receipt=readJson(root,target.buildReceipt);
        sourceBuild=await verifySourceBuildReceipt(sourceBuildTarget(target),{repoRoot:root,wasmSha256:receipt.executionSha256});
      }catch(error){report.observations.buildReceiptUnavailable=String(error.message);}
      report.observations.sourceInventory=sourceBuild?{
        buildReceipt:target.buildReceipt,sourceFiles:sourceBuild.sourceCount,
        sourceInputsSha256:sourceBuild.sourceInputsSha256,requiredSubmodules:sourceBuild.requiredSubmodules,
        productionSources:sourceBuild.sourceCount,
      }:null;
      report.checks.push(check('BUILD-06',game,variant,{sourceInventory:report.observations.sourceInventory,
        humanReviewRequired:['upstream source or revision','license and author attribution','external game asset/font origins and hashes']},
      'incomplete',false));
      report.status='incomplete';report.passed=false;
      report.incompleteReason='BUILD-06 requires a human source and asset provenance review.';
    }else if(stage==='verify-existing'){
      assert.equal(buildArguments.length,0,'Build options are not accepted by existing-report verification');
      assert.ok(runtimeManifest,'verify-existing requires --runtime-manifest');
      const inspect=dependencies.inspectExisting??inspectExistingBuildAcceptance;
      try{
        const verified=await inspect({root,manifestPath:runtimeManifest,game,variant});
        report.observations.revalidation=verified;
        const sourceBuild={producer:verified.sourceBuild.report,current:verified.sourceBuild.observation};
        const packageStage={producer:verified.package.report,current:verified.package.observation};
        const target=verified.target;
        report.checks.push(check('BUILD-01',game,variant,{target,sourceBuild,claim:'selected-source-owner'},'passed',true));
        report.checks.push(check('BUILD-02',game,variant,{target,sourceBuild,claim:'current-build-plan-and-toolchain'},'passed',true));
        report.checks.push(check('BUILD-03',game,variant,{target,sourceBuild,claim:'current-source-build-output'},'passed',true));
        report.checks.push(check('BUILD-04',game,variant,{target,sourceBuild,package:packageStage,
          claim:'current-package-and-source-binding'},'passed',true));
        report.status='passed';report.passed=true;
      }catch(error){
        if(error?.code==='ERR_BUILD_EVIDENCE_MISSING'){
          report.status='blocked';report.passed=false;report.blockedInputs=[{name:'buildAcceptance',reason:error.message}];
          report.incompleteReason=error.message;
        }else throw error;
      }
    }else throw new Error(`Unknown build acceptance stage: ${stage}`);
    await save();
  });
}

async function main(){
  const options=parseBuildAcceptanceOptions(process.argv.slice(2));
  const report=await runBuildAcceptance(options);
  console.log(JSON.stringify({status:report.status,stage:report.stage,game:report.scope?.game,
    variant:report.scope?.variant,report:resolve(options.output,'report.json')},null,2));
  if(report.status==='failed')process.exitCode=1;
}

if(process.argv[1]&&resolve(process.argv[1])===fileURLToPath(import.meta.url))
  main().catch(error=>{console.error(error.stack||error);process.exitCode=1;});
