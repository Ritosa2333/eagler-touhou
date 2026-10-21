import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {lstat,mkdir,readFile,realpath,writeFile} from 'node:fs/promises';
import {dirname,relative,resolve,sep} from 'node:path';
import {PACKAGE_DESCRIPTOR_SCHEMA,canonicalPackagePayload,validatePackageDescriptor} from '../../launcher/package/package-descriptor.mjs';
import {parsePackageZip} from '../../launcher/package/package-zip.mjs';
import {RELEASE_CATALOG_SCHEMA,validateReleaseCatalog} from '../../launcher/lib/contracts/release-catalog.mjs';

export const INSTALLATION_AUTHORITY_JOURNAL_SCHEMA='eagler-touhou/package-installation-authority/1';
const normalizePromptText=value=>typeof value==='string'?value.replace(/\s+/g,' ').trim():'';
const MIDI_START_MESSAGES=new Set([
  '将会使用 MIDI 播放音乐。 如果想听正常的音乐，请在音乐中选择 OGG。',
  'The game will continue with MIDI music. If you want normal music playback, choose OGG under Music.',
]);
const PACKAGE_UPDATE_MESSAGES=new Set([
  '新版已下载并校验完成。立即重启会先保存并结束当前游戏，再使用新版启动。也可以继续游戏，下次启动时使用新版。',
  'The update has been downloaded and verified. Restart now to save and end the current game, then launch the new version. You can also keep playing and use it on the next launch.',
]);

/** Classify only the two real Launcher decisions expected on this route. */
export function classifyPackageAuthorityPrompt(prompt){
  const message=normalizePromptText(prompt?.message),confirm=normalizePromptText(prompt?.confirm);
  if(MIDI_START_MESSAGES.has(message)&&['仍要启动','Start anyway'].includes(confirm))return'midi-launch-consent';
  if(PACKAGE_UPDATE_MESSAGES.has(message)&&['立即重启','Restart now'].includes(confirm))return'published-package-update-ready';
  return null;
}

/** Read one actual iframe epoch from the page-level protocol/DATA journals. */
export function snapshotPackageRuntime({game,generation,expectedData}){
  const frame=document.getElementById('gameFrame'),src=new URL(frame.src,location.href);
  const epoch=Number(src.searchParams.get('runtimeEpoch'));
  const events=window.__offlineRuntimeEvents.filter(event=>event.epoch===epoch);
  const dataReads=window.__offlineManagedDataReads.filter(item=>item.game===game&&item.epoch===epoch);
  return{frameUrl:frame.src,pathname:src.pathname,query:Object.fromEntries(src.searchParams),
    ready:events.find(event=>event.event==='ready'),first:events.find(event=>event.event==='first-frame'),
    pageOnline:navigator.onLine,workerControlled:!!navigator.serviceWorker.controller,
    dataRead:dataReads.find(item=>item.generation===generation&&item.bytes===expectedData.bytes&&item.sha256===expectedData.sha256)??null,
    dataReads};
}

const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const isRecord=value=>!!value&&typeof value==='object'&&!Array.isArray(value);

/**
 * Install in the top Launcher document. The hook only records real readonly
 * Package Store get() results; it never changes a request, transaction or row.
 */
export function installPackageInstallationReadJournal({database,installations,generations,maxEntries=4096,initialPhase='page-load'}){
  if(window!==window.top)return;
  if(typeof database!=='string'||!database||typeof installations!=='string'||!installations||
      typeof generations!=='string'||!generations||!Number.isSafeInteger(maxEntries)||maxEntries<1||
      window.__packageInstallationAuthorityJournal)throw Error('Invalid Package installation authority journal setup');
  if(typeof initialPhase!=='string'||!initialPhase||initialPhase.length>80)throw Error('Invalid Package authority initial phase');
  const reads=[],pending=new Set();let overflow=false,sequence=0,phase=initialPhase;
  const schema='eagler-touhou/package-installation-authority/1';
  const cloneKey=key=>Array.isArray(key)?key.map(value=>typeof value==='number'||typeof value==='string'?value:null):key;
  const summarize=(store,key,value)=>{
    if(!value)return null;
    if(store===installations)return{game:value.game??(typeof key==='string'?key:Array.isArray(key)?key[0]:null),source:value.source??null,
      currentGeneration:value.currentGeneration??null,pendingGeneration:value.pendingGeneration??null};
    const descriptor=value.descriptor;
    return{id:value.id??null,game:value.game??null,revision:descriptor?.revision??null,
      gameVersion:descriptor?.gameVersion??'original',dataFile:descriptor?.runtimeRequirement?.dataFile??null,
      dataObjectId:descriptor?.runtimeRequirement?.dataFile?value.files?.[descriptor.runtimeRequirement.dataFile]?.objectId??null:null};
  };
  const originalGet=IDBObjectStore.prototype.get;
  IDBObjectStore.prototype.get=function(key,...rest){
    const store=this.name,tx=this.transaction,db=tx?.db;
    const capture=db?.name===database&&(store===installations||store===generations);
    const request=originalGet.call(this,key,...rest);
    if(!capture)return request;
    const entry={sequence:++sequence,phase,store,key:cloneKey(key),mode:tx.mode??null,result:undefined,error:null};
    if(reads.length<maxEntries)reads.push(entry);else overflow=true;
    let done;const finished=new Promise(resolve=>{done=resolve;});pending.add(finished);
    const finish=()=>{pending.delete(finished);done();};
    request.addEventListener('success',()=>{entry.result=summarize(store,key,request.result);finish();},{once:true});
    request.addEventListener('error',()=>{entry.error=String(request.error?.message||request.error||'IndexedDB read failed');finish();},{once:true});
    return request;
  };
  Object.defineProperty(window,'__packageInstallationAuthorityJournal',{configurable:true,value:Object.freeze({
    setPhase(next){if(typeof next!=='string'||!next||next.length>80)throw Error('Invalid Package authority phase');phase=next;},
    async snapshot({timeoutMs=5000}={}){
      if(!Number.isSafeInteger(timeoutMs)||timeoutMs<1)throw Error('Invalid Package authority journal timeout');
      const deadline=Date.now()+timeoutMs;
      while(pending.size&&Date.now()<deadline)await Promise.race([Promise.all([...pending]),new Promise(resolve=>setTimeout(resolve,10))]);
      return{schema,overflow,pending:pending.size,
        reads:reads.map(entry=>({...entry,key:Array.isArray(entry.key)?[...entry.key]:entry.key,result:entry.result&&{...entry.result}}))};
    },
  })});
}

async function writeFixtureFile(siteRoot,source,bytes){
  assert.ok(typeof source==='string'&&(source.startsWith('games/')||source.startsWith('shared/')),
    `Published Package source is outside same-origin resource routes: ${source}`);
  assert.ok(!source.includes('\\')&&!source.includes('\0')&&!source.startsWith('/'),`Unsafe Published Package source: ${source}`);
  const parts=source.split('/');assert.ok(parts.every(part=>part&&part!=='.'&&part!=='..'),`Unsafe Published Package source: ${source}`);
  const target=resolve(siteRoot,...parts),rel=relative(siteRoot,target);
  assert.ok(rel&&rel!=='..'&&!rel.startsWith(`..${sep}`),`Published Package source escapes the local site: ${source}`);
  let parent=siteRoot;
  for(const part of parts.slice(0,-1)){
    parent=resolve(parent,part);
    try{const info=await lstat(parent);assert.ok(info.isDirectory()&&!info.isSymbolicLink(),`Published Package route contains a non-directory path: ${source}`);}
    catch(error){if(error.code!=='ENOENT')throw error;await mkdir(parent);}
  }
  try{
    const info=await lstat(target);
    assert.ok(info.isFile()&&!info.isSymbolicLink(),`Published Package route collides with a non-file: ${source}`);
    const existing=await readFile(target);assert.deepEqual(existing,bytes,`Published Package route collides with different site bytes: ${source}`);
  }catch(error){if(error.code!=='ENOENT')throw error;await writeFile(target,bytes,{flag:'wx'});}
  return{source,bytes:bytes.length,sha256:sha256(bytes)};
}

/**
 * Add a localhost-only Release Catalog backed by the explicitly selected real
 * update ZIP. The generated Launcher site is first verified in import mode;
 * this fixture adds only the standard catalog, descriptor and declared files.
 */
export async function createPublishedPackageFixture({siteDirectory,game,archivePath,expectedRevision}){
  assert.ok(typeof siteDirectory==='string'&&siteDirectory&&typeof game==='string'&&game&&
    typeof archivePath==='string'&&archivePath&&typeof expectedRevision==='string'&&expectedRevision,
  'Published Package fixture inputs are required');
  const siteRoot=await realpath(resolve(siteDirectory)),archive=await readFile(resolve(archivePath));
  const parsed=await parsePackageZip(new Blob([archive])),descriptor=validatePackageDescriptor(parsed.descriptor);
  assert.equal(descriptor.schema,PACKAGE_DESCRIPTOR_SCHEMA);
  assert.equal(descriptor.game,game,`Published update ZIP must identify ${game}`);
  assert.equal(descriptor.revision,expectedRevision,`${game}: Published update ZIP revision differs from the selected update manifest`);
  const canonicalRevision=createHash('sha256').update(canonicalPackagePayload(descriptor)).digest('hex').slice(0,16);
  assert.equal(descriptor.revision,canonicalRevision,`${game}: Published update ZIP does not carry its canonical descriptor revision`);
  const existingCatalog=JSON.parse(await readFile(resolve(siteRoot,'release-catalog.json'),'utf8'));
  validateReleaseCatalog(existingCatalog);
  assert.deepEqual(existingCatalog.games,{},'The base import-profile site must not already publish another package');
  const descriptorName=`packages/${game}-${descriptor.revision}.package.json`;
  const descriptorPath=resolve(siteRoot,descriptorName),descriptorBytes=Buffer.from(`${JSON.stringify(descriptor,null,2)}\n`);
  const descriptorRel=relative(siteRoot,descriptorPath);
  assert.ok(descriptorRel&&!descriptorRel.startsWith(`..${sep}`),`${game}: Published Package descriptor escapes the local site`);
  await mkdir(dirname(descriptorPath),{recursive:true});await writeFile(descriptorPath,descriptorBytes,{flag:'wx'});
  const files=[];
  for(const [fileId,declaration] of Object.entries(descriptor.files)){
    const payload=parsed.files.get(fileId);
    assert.ok(payload,`${game}: Published ZIP is missing declared Package file ${fileId}`);
    const bytes=Buffer.from(await payload.blob.arrayBuffer());
    assert.equal(bytes.length,declaration.bytes,`${game}/${fileId}: Published ZIP file length differs from descriptor`);
    assert.equal(sha256(bytes),declaration.sha256,`${game}/${fileId}: Published ZIP file hash differs from descriptor`);
    files.push(await writeFixtureFile(siteRoot,declaration.source,bytes));
  }
  const releaseCatalog=validateReleaseCatalog({schema:RELEASE_CATALOG_SCHEMA,
    games:{[game]:{revision:descriptor.revision,descriptor:descriptorName}}});
  const catalogBytes=Buffer.from(`${JSON.stringify(releaseCatalog,null,2)}\n`);
  await writeFile(resolve(siteRoot,'release-catalog.json'),catalogBytes);
  return{game,revision:descriptor.revision,descriptor:{path:descriptorName,bytes:descriptorBytes.length,sha256:sha256(descriptorBytes)},
    catalog:{path:'release-catalog.json',bytes:catalogBytes.length,sha256:sha256(catalogBytes)},
    sourceArchive:{bytes:archive.length,sha256:sha256(archive)},files};
}

function gameKey(key,game){return key===game||(Array.isArray(key)&&key[0]===game&&key[1]==='original');}
function generationKey(key,game,generationId){return Array.isArray(key)&&key.length===2&&key[0]===game&&key[1]===generationId;}
function phaseHasCanonicalGeneration(reads,phase,game,generationId,revision,{committed=false}={}){
  const installation=reads.some(read=>read.phase===phase&&read.store==='installations'&&read.mode==='readonly'&&
    gameKey(read.key,game)&&read.result?.game===game&&read.result.currentGeneration===generationId&&
    (!committed||read.result.pendingGeneration===null));
  const generation=reads.some(read=>read.phase===phase&&read.store==='generations'&&read.mode==='readonly'&&
    generationKey(read.key,game,generationId)&&read.result?.game===game&&read.result.id===generationId&&
    (revision==null||read.result.revision===revision));
  return installation&&generation;
}

/** Recompute the selected installation chain from actual IndexedDB get() results. */
export function assessPackageInstallationAuthority({journal,game,nonSelectedGame,baseGenerationId,baseRevision,
  updatedGenerationId,updatedRevision,finalRuntimeGenerationId,updatePrompt,offlineRuntimeGenerationId}={}){
  const fail=reason=>({available:true,valid:false,kind:'PACKAGE-06',reason});
  if(!isRecord(journal)||journal.schema!==INSTALLATION_AUTHORITY_JOURNAL_SCHEMA)return{available:false,valid:false,kind:'PACKAGE-06',reason:'Package Store read journal is unavailable'};
  if(journal.overflow||journal.pending||!Array.isArray(journal.reads))return fail('Package Store read journal overflowed or has unfinished reads');
  if(!game||!nonSelectedGame||game===nonSelectedGame||!baseGenerationId||!baseRevision||!updatedGenerationId||!updatedRevision)
    return{available:false,valid:false,kind:'PACKAGE-06',reason:'A selected and published update generation identity is unavailable'};
  if(baseGenerationId===updatedGenerationId||baseRevision===updatedRevision)return fail('Published update did not advance the selected Package Store generation and descriptor revision');
  if(updatePrompt?.observed!==true||updatePrompt?.confirmed!==true||updatePrompt?.publishedRevision!==updatedRevision||
      updatePrompt?.currentGenerationAtPrompt!==updatedGenerationId||
      updatePrompt?.currentRevisionAtPrompt!==updatedRevision||
      updatePrompt?.pinnedRuntimeGenerationAtPrompt!==baseGenerationId||
      updatePrompt?.pendingGenerationAtPrompt!==null||
      !Number.isSafeInteger(updatePrompt?.pinnedRuntimeEpoch)||updatePrompt.pinnedRuntimeEpoch<1)
    return fail('The production update-ready prompt is not bound to the committed update and the still-pinned original Runtime');
  const reads=journal.reads;
  if(!phaseHasCanonicalGeneration(reads,'post-import-card-reopen',game,baseGenerationId,baseRevision))
    return fail('Launcher game-card reopen did not read the selected imported generation from Package Store');
  if(!phaseHasCanonicalGeneration(reads,'resource-preparation-and-update-prompt',game,baseGenerationId,baseRevision))
    return fail('Production resource preparation and update prompt did not read the selected current Package Store generation');
  if(!phaseHasCanonicalGeneration(reads,'update-ready-checkpoint',game,updatedGenerationId,updatedRevision,{committed:true}))
    return fail('The update-ready checkpoint did not read the fully committed published generation');
  if(!phaseHasCanonicalGeneration(reads,'published-update-commit',game,updatedGenerationId,updatedRevision))
    return fail('Production update completion did not refresh the selected generation from Package Store');
  if(!phaseHasCanonicalGeneration(reads,'offline-reopen',game,updatedGenerationId,updatedRevision))
    return fail('Offline Launcher reopen did not read the updated selected generation from Package Store');
  if(!phaseHasCanonicalGeneration(reads,'offline-resource-preparation',game,updatedGenerationId,updatedRevision))
    return fail('Offline production resource preparation did not read the updated selected generation from Package Store');
  const nonSelectedRead=reads.find(read=>read.phase==='post-import-card-reopen'&&read.store==='installations'&&
    read.mode==='readonly'&&gameKey(read.key,nonSelectedGame));
  if(!nonSelectedRead)return fail('Launcher catalog did not read the non-selected game installation slot');
  if(nonSelectedRead.result?.currentGeneration)return fail('Non-selected game read resolved to an installed generation and could contaminate selected-game evidence');
  if(finalRuntimeGenerationId!==updatedGenerationId)return fail('Online Runtime did not use the published current Package Store generation');
  if(offlineRuntimeGenerationId!==updatedGenerationId)return fail('Offline Runtime did not use the published current Package Store generation');
  const relevant=reads.filter(read=>read.store==='installations'||read.store==='generations').map(read=>({sequence:read.sequence,phase:read.phase,
    store:read.store,key:read.key,mode:read.mode,result:read.result,error:read.error}));
  return{available:true,valid:true,kind:'PACKAGE-06',reason:null,game,base:{generationId:baseGenerationId,revision:baseRevision},
    updated:{generationId:updatedGenerationId,revision:updatedRevision},onlineRuntimeGenerationId:finalRuntimeGenerationId,
    offlineRuntimeGenerationId,nonSelectedGame,readCount:relevant.length,reads:relevant};
}
