import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {hostArtworkFiles} from '../../launcher/lib/host-artwork.mjs';
import {validateReleaseCatalog,releaseCatalogEntryUrl} from '../../launcher/lib/contracts/release-catalog.mjs';
import {parsePackageZip} from '../../launcher/package/package-zip.mjs';
import {resolvePackageSource,validatePackageDescriptor} from '../../launcher/package/package-descriptor.mjs';
import {verifyPrivateData} from './acceptance-contracts.mjs';

export const ROOM_RESOURCE_SCHEMA='eagler-touhou/room-resources/1';
export const ROOM_SITE_ASSET_SCHEMA='eagler-touhou/room-site-assets/1';
export const ROOM_RESOURCE_REQUEST_TIMEOUT_MS=5000;
const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const isRecord=value=>!!value&&typeof value==='object'&&!Array.isArray(value);
const executable=/\.(?:wasm|mjs|js|html?|xhtml)$/i;

class ResourceCheckError extends Error {
  constructor(code,message){super(message);this.name='ResourceCheckError';this.code=code;}
}

function normalizedBase(value){
  let url;
  try{url=new URL(value);}catch{throw new Error('Launcher URL must be an absolute HTTP(S) URL');}
  assert.ok(['http:','https:'].includes(url.protocol),'Launcher URL must use HTTP or HTTPS');
  assert.ok(!url.username&&!url.password&&!url.hash,'Launcher URL must not contain credentials or a fragment');
  if(!url.pathname.endsWith('/'))url.pathname+='/';
  return url;
}

async function responseBytes(fetchImpl,url,label,timeoutMs){
  const controller=new AbortController(),timer=setTimeout(()=>controller.abort(),timeoutMs);
  try{
    const response=await fetchImpl(url,{cache:'no-store',redirect:'error',signal:controller.signal});
    if(!response?.ok)throw new ResourceCheckError('http-unavailable',`${label}: HTTP ${response?.status??'unknown'} at ${url}`);
    return Buffer.from(await response.arrayBuffer());
  }catch(error){
    if(controller.signal.aborted)throw new ResourceCheckError('http-timeout',`${label}: request exceeded ${timeoutMs} ms at ${url}`);
    if(error instanceof ResourceCheckError)throw error;
    throw new ResourceCheckError('http-unavailable',`${label}: ${String(error.message||error)}`);
  }finally{clearTimeout(timer);}
}

async function jsonResponse(fetchImpl,url,label,timeoutMs){
  const bytes=await responseBytes(fetchImpl,url,label,timeoutMs);
  try{return{value:JSON.parse(bytes.toString('utf8')),bytes};}
  catch(error){throw new ResourceCheckError('invalid-json',`${label}: invalid JSON (${error.message})`);}
}

function dataContract(game){
  const product=PRODUCT_GAMES[game];
  if(!product?.package?.dataFileId||!product.package.dataTarget)
    throw new ResourceCheckError('product-data-contract-missing',`${game}: product catalog has no game-data package contract`);
  return{fileId:product.package.dataFileId,target:product.package.dataTarget};
}

function assertDataDescriptor(descriptor,game,contract){
  validatePackageDescriptor(descriptor);
  if(descriptor.game!==game)throw new ResourceCheckError('package-game-mismatch',`${game}: package descriptor identifies ${descriptor.game}`);
  if(descriptor.runtime||descriptor.runtimes||descriptor.defaultRuntime||!descriptor.runtimeRequirement)
    throw new ResourceCheckError('runtime-package-forbidden',`${game}: room DATA source must be a data package without Runtime HTML/JS/WASM`);
  const executableFiles=Object.values(descriptor.files).filter(file=>executable.test(file.source)||executable.test(file.target));
  if(executableFiles.length)
    throw new ResourceCheckError('runtime-package-forbidden',`${game}: package descriptor declares Runtime executable files: ${executableFiles.map(file=>file.source).join(', ')}`);
  if(descriptor.runtimeRequirement.target!==game||descriptor.runtimeRequirement.dataFile!==contract.fileId)
    throw new ResourceCheckError('data-contract-mismatch',`${game}: package runtimeRequirement does not match product data file ${contract.fileId}`);
  if(!descriptor.base.files.includes(contract.fileId))
    throw new ResourceCheckError('data-not-in-base',`${game}: package base does not install ${contract.fileId}`);
  const declaration=descriptor.files[contract.fileId];
  if(!declaration||declaration.target!==contract.target)
    throw new ResourceCheckError('data-target-mismatch',`${game}: package ${contract.fileId} target must be ${contract.target}`);
  if(!Number.isSafeInteger(declaration.bytes)||declaration.bytes<=0||! /^[a-f0-9]{64}$/i.test(declaration.sha256||''))
    throw new ResourceCheckError('data-identity-missing',`${game}: package ${contract.fileId} must declare positive bytes and SHA-256`);
  return declaration;
}

async function verifyLocalPackage({game,path,expectedData,contract}){
  let archive;
  try{archive=await readFile(path);}
  catch(error){throw new ResourceCheckError('package-unreadable',`${game}: cannot read local game package ${path}: ${error.message}`);}
  let parsed;
  try{parsed=await parsePackageZip(new Blob([archive]));}
  catch(error){throw new ResourceCheckError('package-invalid',`${game}: invalid offline package ${path}: ${error.message}`);}
  const {descriptor,files,entries}=parsed;
  assertDataDescriptor(descriptor,game,contract);
  const entryNames=[...entries.keys()];
  const forbidden=entryNames.filter(name=>executable.test(name));
  if(forbidden.length)
    throw new ResourceCheckError('runtime-package-forbidden',`${game}: offline package contains Runtime executable members: ${forbidden.join(', ')}`);
  const declaredSources=Object.entries(descriptor.files).map(([fileId,file])=>({fileId,source:file.source,file}));
  if(new Set(declaredSources.map(item=>item.source)).size!==declaredSources.length)
    throw new ResourceCheckError('package-source-duplicate',`${game}: multiple package file IDs point to the same ZIP member`);
  const expectedNames=new Set(['package.json',...declaredSources.map(item=>item.source)]);
  const extras=entryNames.filter(name=>!expectedNames.has(name));
  if(extras.length)throw new ResourceCheckError('package-entry-undeclared',`${game}: offline package has undeclared ZIP members: ${extras.join(', ')}`);
  if(entryNames.length!==expectedNames.size)
    throw new ResourceCheckError('package-entry-missing',`${game}: offline package is missing one or more declared ZIP members`);
  let verifiedFiles=0,dataIdentity=null;
  for(const {fileId,file} of declaredSources){
    const item=files.get(fileId);
    if(!item)throw new ResourceCheckError('package-file-missing',`${game}: ZIP has no payload for declared file ${fileId} (${file.source})`);
    if(!Number.isSafeInteger(file.bytes)||file.bytes<0||! /^[a-f0-9]{64}$/i.test(file.sha256||''))
      throw new ResourceCheckError('package-file-identity-missing',`${game}: package file ${fileId} must declare bytes and SHA-256`);
    const bytes=Buffer.from(await item.blob.arrayBuffer()),digest=sha256(bytes);
    if(bytes.length!==file.bytes||digest!==file.sha256.toLowerCase())
      throw new ResourceCheckError('package-file-identity-mismatch',`${game}: ZIP payload ${fileId} does not match its declared bytes/SHA-256`);
    if(fileId===contract.fileId)dataIdentity={bytes:bytes.length,sha256:digest};
    verifiedFiles++;
  }
  if(!dataIdentity||dataIdentity.bytes!==expectedData.bytes||dataIdentity.sha256!==expectedData.sha256)
    throw new ResourceCheckError('private-data-mismatch',`${game}: offline package DATA ${dataIdentity?.sha256??'missing'} does not match current private DATA ${expectedData.sha256}`);
  return{source:'offline-package',package:{path:resolve(path),archiveBytes:archive.length,archiveSha256:sha256(archive),
    descriptorRevision:descriptor.revision,fileCount:verifiedFiles,dataFileId:contract.fileId,dataBytes:dataIdentity.bytes,dataSha256:dataIdentity.sha256}};
}

async function getCatalog(fetchImpl,base,timeoutMs){
  const catalogUrl=new URL('release-catalog.json',base).href;
  const {value,bytes}=await jsonResponse(fetchImpl,catalogUrl,'Launcher Release Catalog',timeoutMs);
  let catalog;
  try{catalog=validateReleaseCatalog(value);}
  catch(error){throw new ResourceCheckError('invalid-release-catalog',`Launcher Release Catalog: ${error.message}`);}
  return{catalog,catalogUrl,catalogSha256:sha256(bytes)};
}

async function verifyCatalogPackage({game,expectedData,contract,fetchImpl,base,catalogInfo,timeoutMs}){
  const descriptorUrl=releaseCatalogEntryUrl(catalogInfo.catalogUrl,catalogInfo.catalog,game);
  if(!descriptorUrl)
    throw new ResourceCheckError('package-required',`${game}: Launcher Release Catalog has no game-data package; pass --packages=/absolute/packages.json with a current ${game} offline ZIP`);
  const descriptorLocation=new URL(descriptorUrl);
  if(descriptorLocation.origin!==base.origin)
    throw new ResourceCheckError('cross-origin-package',`${game}: Release Catalog descriptor must be served by the local Launcher origin`);
  const {value:descriptor,bytes:descriptorBytes}=await jsonResponse(fetchImpl,descriptorUrl,`${game} Package Descriptor`,timeoutMs);
  try{validatePackageDescriptor(descriptor);}
  catch(error){throw new ResourceCheckError('invalid-package-descriptor',`${game}: invalid Release Catalog Package Descriptor: ${error.message}`);}
  const entry=catalogInfo.catalog.games[game];
  if(descriptor.revision!==entry.revision)
    throw new ResourceCheckError('package-revision-mismatch',`${game}: Release Catalog revision does not match its Package Descriptor`);
  const declaration=assertDataDescriptor(descriptor,game,contract);
  const dataUrl=resolvePackageSource(descriptorUrl,declaration.source);
  if(new URL(dataUrl).origin!==base.origin)
    throw new ResourceCheckError('cross-origin-data',`${game}: game DATA must be served by the local Launcher origin`);
  const bytes=await responseBytes(fetchImpl,dataUrl,`${game} game DATA`,timeoutMs),digest=sha256(bytes);
  if(bytes.length!==declaration.bytes||digest!==declaration.sha256.toLowerCase())
    throw new ResourceCheckError('published-data-identity-mismatch',`${game}: downloaded game DATA does not match its Package Descriptor bytes/SHA-256`);
  if(bytes.length!==expectedData.bytes||digest!==expectedData.sha256)
    throw new ResourceCheckError('private-data-mismatch',`${game}: Release Catalog DATA ${digest} does not match current private DATA ${expectedData.sha256}`);
  return{source:'release-catalog',catalog:{catalogUrl:catalogInfo.catalogUrl,catalogSha256:catalogInfo.catalogSha256,
    revision:entry.revision,descriptorUrl,descriptorSha256:sha256(descriptorBytes),dataFileId:contract.fileId,
    dataBytes:bytes.length,dataSha256:digest}};
}

/** Check that every selected room can obtain the current game's private DATA before opening Chromium. */
export async function inspectRoomResources({baseUrl,targets,packageMap={},fetchImpl=globalThis.fetch,packageMapError=null,
  requestTimeoutMs=ROOM_RESOURCE_REQUEST_TIMEOUT_MS}){
  const base=normalizedBase(baseUrl);
  assert.ok(Number.isSafeInteger(requestTimeoutMs)&&requestTimeoutMs>0,'requestTimeoutMs must be a positive integer');
  assert.ok(Array.isArray(targets)&&targets.length,'At least one multiplayer Runtime target is required');
  const report={schema:ROOM_RESOURCE_SCHEMA,checkedAt:new Date().toISOString(),baseUrl:base.href,passed:false,games:[]};
  const mapValid=isRecord(packageMap);
  if(packageMapError||!mapValid){
    const detail=packageMapError||'package map must be a JSON object mapping game IDs to offline ZIP paths';
    for(const target of targets)report.games.push({game:target.game,status:'failed',error:{code:'package-map-invalid',message:detail}});
    return{...report,passed:false};
  }
  const invalidKeys=Object.keys(packageMap).filter(game=>!PRODUCT_GAMES[game]||typeof packageMap[game]!=='string'||!packageMap[game]);
  if(invalidKeys.length){
    const message=`Package map contains unsupported games or empty paths: ${invalidKeys.join(', ')}`;
    for(const target of targets)report.games.push({game:target.game,status:'failed',error:{code:'package-map-invalid',message}});
    return{...report,passed:false};
  }
  let catalogTask;
  for(const target of targets){
    const game=target.game;
    const item={game,status:'failed'};
    report.games.push(item);
    try{
      if(!PRODUCT_GAMES[game])throw new ResourceCheckError('unsupported-game',`Unsupported game ${game}`);
      const [expectedData,contract]=await Promise.all([verifyPrivateData(target.data),Promise.resolve(dataContract(game))]);
      item.expectedData=expectedData;
      const packagePath=packageMap[game];
      let result;
      if(packagePath){
        result=await verifyLocalPackage({game,path:packagePath,expectedData,contract});
      }else{
        catalogTask??=getCatalog(fetchImpl,base,requestTimeoutMs);
        const catalogInfo=await catalogTask;
        result=await verifyCatalogPackage({game,expectedData,contract,fetchImpl,base,catalogInfo,timeoutMs:requestTimeoutMs});
        report.catalog={url:catalogInfo.catalogUrl,sha256:catalogInfo.catalogSha256};
      }
      Object.assign(item,result,{status:'ready'});
    }catch(error){
      item.error={code:error?.code||'resource-check-failed',message:String(error?.message||error)};
    }
  }
  report.passed=report.games.every(item=>item.status==='ready');
  return report;
}

/** Prove the private DATA and selected offline ZIP still match the preflight identity after a room case. */
export async function verifyRoomResourcePostflight({target,prepared}){
  if(prepared?.status!=='ready')throw new ResourceCheckError('resource-preflight-incomplete',`${target.game}: resource postflight requires a ready preflight result`);
  const data=await verifyPrivateData(target.data),expectedData=prepared.expectedData;
  if(data.bytes!==expectedData?.bytes||data.sha256!==expectedData?.sha256)
    throw new ResourceCheckError('private-data-changed',`${target.game}: private DATA changed after resource preflight (expected ${expectedData?.bytes} bytes/${expectedData?.sha256}, found ${data.bytes} bytes/${data.sha256})`);
  let packageIdentity=null;
  if(prepared.source==='offline-package'){
    const path=prepared.package?.path;
    if(typeof path!=='string'||!prepared.package?.archiveSha256||!Number.isSafeInteger(prepared.package.archiveBytes))
      throw new ResourceCheckError('package-preflight-identity-missing',`${target.game}: offline package preflight did not record archive identity`);
    let archive;
    try{archive=await readFile(path);}
    catch(error){throw new ResourceCheckError('package-changed-after-preflight',`${target.game}: offline package is no longer readable after preflight: ${path}: ${error.message}`);}
    packageIdentity={path,archiveBytes:archive.length,archiveSha256:sha256(archive)};
    if(packageIdentity.archiveBytes!==prepared.package.archiveBytes||packageIdentity.archiveSha256!==prepared.package.archiveSha256)
      throw new ResourceCheckError('package-changed-after-preflight',`${target.game}: offline ZIP changed after preflight (expected ${prepared.package.archiveBytes} bytes/${prepared.package.archiveSha256}, found ${packageIdentity.archiveBytes} bytes/${packageIdentity.archiveSha256})`);
  }
  return{status:'passed',data,package:packageIdentity};
}

function validAssetBytes(name,bytes){
  if(name.endsWith('.webp'))return bytes.length>=16&&bytes.toString('ascii',0,4)==='RIFF'&&bytes.toString('ascii',8,12)==='WEBP';
  if(name.endsWith('.ico'))return bytes.length>=22&&bytes[0]===0&&bytes[1]===0&&bytes[2]===1&&bytes[3]===0;
  if(name.endsWith('.png'))return bytes.length>=24&&bytes.subarray(0,8).equals(Buffer.from([137,80,78,71,13,10,26,10]));
  return false;
}

/** Record Host cover and site-icon availability once, independently of per-game Runtime results. */
export async function inspectRoomSiteAssets({baseUrl,fetchImpl=globalThis.fetch,games=Object.keys(PRODUCT_GAMES),
  requestTimeoutMs=ROOM_RESOURCE_REQUEST_TIMEOUT_MS}){
  assert.ok(Number.isSafeInteger(requestTimeoutMs)&&requestTimeoutMs>0,'requestTimeoutMs must be a positive integer');
  const base=normalizedBase(baseUrl),assets=hostArtworkFiles(games),results=[];
  await Promise.all(assets.map(async name=>{
    const url=new URL(`assets/${name}`,base).href;
    try{
      const bytes=await responseBytes(fetchImpl,url,`Host asset ${name}`,requestTimeoutMs);
      if(!validAssetBytes(name,bytes))throw new ResourceCheckError('invalid-host-asset',`Host asset ${name}: response bytes are not a valid ${name.split('.').at(-1).toUpperCase()} asset`);
      results.push({path:`/assets/${name}`,status:'ready',bytes:bytes.length,sha256:sha256(bytes)});
    }catch(error){results.push({path:`/assets/${name}`,status:'failed',error:{code:error?.code||'host-asset-check-failed',message:String(error?.message||error)}});}
  }));
  results.sort((a,b)=>a.path.localeCompare(b.path));
  return{schema:ROOM_SITE_ASSET_SCHEMA,checkedAt:new Date().toISOString(),baseUrl:base.href,passed:results.every(item=>item.status==='ready'),assets:results,observedFailures:[]};
}

export function recordRoomSiteAssetFailures(siteAssets,failures){
  const seen=new Set(siteAssets.observedFailures.map(item=>`${item.url}\n${item.status}`));
  for(const failure of failures){
    const url=String(failure.url||'');
    try{if(!new URL(url).pathname.startsWith('/assets/'))continue;}catch{continue;}
    const key=`${url}\n${failure.status}`;
    if(seen.has(key))continue;
    seen.add(key);siteAssets.observedFailures.push({url,status:failure.status});
  }
  if(siteAssets.observedFailures.length)siteAssets.passed=false;
  return siteAssets;
}
