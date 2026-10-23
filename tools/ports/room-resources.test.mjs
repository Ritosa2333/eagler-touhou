import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createServer} from 'node:http';
import {mkdtemp,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {once} from 'node:events';
import test from 'node:test';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {inspectRoomResources,inspectRoomSiteAssets,recordRoomSiteAssetFailures,verifyRoomResourcePostflight} from './room-resources.mjs';

const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const game='th08',data=Buffer.from('private game data bytes');

const crcTable=Uint32Array.from({length:256},(_,index)=>{
  let value=index;
  for(let bit=0;bit<8;bit++)value=value&1?0xedb88320^(value>>>1):value>>>1;
  return value>>>0;
});
function crc32(bytes){let value=0xffffffff;for(const byte of bytes)value=crcTable[(value^byte)&0xff]^(value>>>8);return(value^0xffffffff)>>>0;}
function storedZip(entries){
  const locals=[],centrals=[];let offset=0;
  for(const [path,payload] of Object.entries(entries)){
    const name=Buffer.from(path,'utf8'),bytes=Buffer.from(payload),crc=crc32(bytes);
    const local=Buffer.alloc(30);local.writeUInt32LE(0x04034b50,0);local.writeUInt16LE(20,4);local.writeUInt16LE(0,6);
    local.writeUInt16LE(0,8);local.writeUInt32LE(0,10);local.writeUInt32LE(crc,14);local.writeUInt32LE(bytes.length,18);
    local.writeUInt32LE(bytes.length,22);local.writeUInt16LE(name.length,26);local.writeUInt16LE(0,28);
    locals.push(local,name,bytes);
    const central=Buffer.alloc(46);central.writeUInt32LE(0x02014b50,0);central.writeUInt16LE(20,4);central.writeUInt16LE(20,6);
    central.writeUInt16LE(0,8);central.writeUInt16LE(0,10);central.writeUInt32LE(0,12);central.writeUInt32LE(crc,16);
    central.writeUInt32LE(bytes.length,20);central.writeUInt32LE(bytes.length,24);central.writeUInt16LE(name.length,28);
    central.writeUInt16LE(0,30);central.writeUInt16LE(0,32);central.writeUInt16LE(0,34);central.writeUInt16LE(0,36);
    central.writeUInt32LE(0,38);central.writeUInt32LE(offset,42);centrals.push(central,name);offset+=local.length+name.length+bytes.length;
  }
  const centralBytes=Buffer.concat(centrals),end=Buffer.alloc(22);end.writeUInt32LE(0x06054b50,0);
  end.writeUInt16LE(0,4);end.writeUInt16LE(0,6);end.writeUInt16LE(centrals.length/2,8);end.writeUInt16LE(centrals.length/2,10);
  end.writeUInt32LE(centralBytes.length,12);end.writeUInt32LE(offset,16);end.writeUInt16LE(0,20);
  return Buffer.concat([...locals,centralBytes,end]);
}

function descriptorFor(payload,{game:gameId=game,extraFiles={},dataSha=hash(payload)}={}){
  const product=PRODUCT_GAMES[gameId],files={'game-data':{source:`games/${gameId}/game.data`,target:product.package.dataTarget,
    revision:'fixture-data',bytes:payload.length,sha256:dataSha},...extraFiles};
  return{schema:'eagler-touhou/package/1',game:gameId,revision:'fixture-package',runtimeRequirement:{protocol:'eagler-touhou/1',target:gameId,
    dataFile:product.package.dataFileId,dataLayout:'fixture-layout'},files,base:{files:Object.keys(files)},components:{}};
}

function packageZip(descriptor,payload,{extraEntries={}}={}){
  const entries={'package.json':Buffer.from(JSON.stringify(descriptor)),[descriptor.files['game-data'].source]:payload,...extraEntries};
  for(const [fileId,declaration] of Object.entries(descriptor.files)){
    if(fileId==='game-data')continue;
    const match=Object.entries(extraEntries).find(([path])=>path===declaration.source);
    if(match)entries[match[0]]=match[1];
  }
  return storedZip(entries);
}

async function privateTarget(temporaryRoot,bytes=data){
  const dataPath=resolve(temporaryRoot,'private.dat');await writeFile(dataPath,bytes);
  return{game,data:dataPath};
}

async function withTemp(callback){
  const root=await mkdtemp(resolve(tmpdir(),'eagler-room-resources-'));
  try{return await callback(root);}finally{await rm(root,{recursive:true,force:true});}
}

function assetBytes(path){
  if(path.endsWith('.webp')){const b=Buffer.alloc(16);b.write('RIFF',0,'ascii');b.write('WEBP',8,'ascii');return b;}
  if(path.endsWith('.ico')){const b=Buffer.alloc(22);b[2]=1;b[4]=1;return b;}
  const b=Buffer.alloc(24);Buffer.from([137,80,78,71,13,10,26,10]).copy(b);return b;
}

async function withHttpFixture(routes,callback){
  const server=createServer((request,response)=>{
    const route=routes[new URL(request.url,'http://fixture.invalid').pathname];
    if(!route){response.writeHead(404);response.end('not found');return;}
    const respond=()=>{if(response.destroyed)return;response.writeHead(route.status??200,{'content-type':route.contentType??'application/octet-stream',...route.headers});response.end(route.body);};
    if(route.delayMs)setTimeout(respond,route.delayMs);else respond();
  });
  server.listen(0,'127.0.0.1');await once(server,'listening');
  const address=server.address(),baseUrl=`http://127.0.0.1:${address.port}/`;
  try{return await callback(baseUrl);}finally{await new Promise((resolveClose,reject)=>server.close(error=>error?reject(error):resolveClose()));}
}

test('room preflight accepts a real offline ZIP only when every declared member and current DATA match',async()=>withTemp(async root=>{
  const target=await privateTarget(root),font=Buffer.from('font bytes');
  const descriptor=descriptorFor(data,{extraFiles:{'shared-font':{source:'shared/unifont.otf',target:'/unifont.otf',revision:'font',bytes:font.length,sha256:hash(font)}}});
  const archive=packageZip(descriptor,data,{extraEntries:{'shared/unifont.otf':font}}),path=resolve(root,'th08.zip');await writeFile(path,archive);
  let fetchCalls=0;
  const result=await inspectRoomResources({baseUrl:'http://127.0.0.1:8130/',targets:[target],packageMap:{th08:path},fetchImpl:async()=>{fetchCalls++;throw Error('local package should avoid catalog fetch');}});
  assert.equal(fetchCalls,0);assert.equal(result.passed,true);assert.equal(result.games[0].source,'offline-package');
  assert.equal(result.games[0].package.archiveSha256,hash(archive));assert.equal(result.games[0].package.fileCount,2);
  assert.equal(result.games[0].package.dataSha256,hash(data));
  assert.equal((await verifyRoomResourcePostflight({target,prepared:result.games[0]})).status,'passed');
}));

test('postflight rejects an offline ZIP changed after resource preflight',async()=>withTemp(async root=>{
  const target=await privateTarget(root),descriptor=descriptorFor(data),path=resolve(root,'th08.zip');
  await writeFile(path,packageZip(descriptor,data));
  const result=await inspectRoomResources({baseUrl:'http://127.0.0.1:8130/',targets:[target],packageMap:{th08:path}});
  await writeFile(path,packageZip(descriptor,Buffer.from('replaced archive payload')));
  await assert.rejects(verifyRoomResourcePostflight({target,prepared:result.games[0]}),error=>error.code==='package-changed-after-preflight');
}));

test('postflight rejects private DATA changed after resource preflight',async()=>withTemp(async root=>{
  const target=await privateTarget(root),descriptor=descriptorFor(data),path=resolve(root,'th08.zip');
  await writeFile(path,packageZip(descriptor,data));
  const result=await inspectRoomResources({baseUrl:'http://127.0.0.1:8130/',targets:[target],packageMap:{th08:path}});
  await writeFile(target.data,Buffer.from('replaced private DATA'));
  await assert.rejects(verifyRoomResourcePostflight({target,prepared:result.games[0]}),error=>error.code==='private-data-changed');
}));

test('room preflight rejects a local ZIP whose current DATA differs from the manifest private input',async()=>withTemp(async root=>{
  const target=await privateTarget(root),stale=Buffer.from('stale DATA'),descriptor=descriptorFor(stale),path=resolve(root,'th08-stale.zip');
  await writeFile(path,packageZip(descriptor,stale));
  const result=await inspectRoomResources({baseUrl:'http://127.0.0.1:8130/',targets:[target],packageMap:{th08:path}});
  assert.equal(result.passed,false);assert.equal(result.games[0].error.code,'private-data-mismatch');
}));

test('room preflight rejects a local ZIP containing Runtime executable bytes',async()=>withTemp(async root=>{
  const target=await privateTarget(root),descriptor=descriptorFor(data),path=resolve(root,'th08-runtime.zip');
  await writeFile(path,packageZip(descriptor,data,{extraEntries:{'runtime/th08.wasm':Buffer.from('old runtime')}}));
  const result=await inspectRoomResources({baseUrl:'http://127.0.0.1:8130/',targets:[target],packageMap:{th08:path}});
  assert.equal(result.passed,false);assert.equal(result.games[0].error.code,'runtime-package-forbidden');
}));

test('a resource failure is isolated to that game and later games are still checked',async()=>withTemp(async root=>{
  const target=await privateTarget(root),otherTarget={...target,game:'th09'};
  const descriptor=descriptorFor(data,{game:'th09'}),path=resolve(root,'th09.zip');
  await writeFile(path,packageZip(descriptor,data));
  const result=await inspectRoomResources({baseUrl:'http://127.0.0.1:8130/',targets:[target,otherTarget],
    packageMap:{th08:resolve(root,'missing-th08.zip'),th09:path},fetchImpl:async()=>{throw Error('local package selection must not use the catalog');}});
  assert.equal(result.passed,false);assert.equal(result.games[0].status,'failed');
  assert.equal(result.games[1].status,'ready');assert.equal(result.games[1].source,'offline-package');
}));

test('room preflight validates every local package file declaration, not only game DATA',async()=>withTemp(async root=>{
  const target=await privateTarget(root),font=Buffer.from('real font bytes');
  const wrongSha='0'.repeat(64),descriptor=descriptorFor(data,{extraFiles:{'shared-font':{source:'shared/font.otf',target:'/font.otf',revision:'font',bytes:font.length,sha256:wrongSha}}});
  const path=resolve(root,'th08-bad-font.zip');await writeFile(path,packageZip(descriptor,data,{extraEntries:{'shared/font.otf':font}}));
  const result=await inspectRoomResources({baseUrl:'http://127.0.0.1:8130/',targets:[target],packageMap:{th08:path}});
  assert.equal(result.passed,false);assert.equal(result.games[0].error.code,'package-file-identity-mismatch');
}));

test('same-origin Release Catalog descriptor and DATA satisfy an uninstalled fresh browser context',async()=>withTemp(async root=>{
  const target=await privateTarget(root),descriptor=descriptorFor(data);
  const result=await withHttpFixture({
    '/release-catalog.json':{contentType:'application/json',body:JSON.stringify({schema:'eagler-touhou/release-catalog/1',games:{th08:{revision:descriptor.revision,descriptor:'th08.package.json'}}})},
    '/th08.package.json':{contentType:'application/json',body:JSON.stringify(descriptor)},
    '/games/th08/game.data':{body:data},
  },baseUrl=>inspectRoomResources({baseUrl,targets:[target],packageMap:{}}));
  assert.equal(result.passed,true);assert.equal(result.games[0].source,'release-catalog');
  assert.equal(result.games[0].catalog.dataSha256,hash(data));assert.equal(result.catalog.url.endsWith('/release-catalog.json'),true);
}));

test('empty Release Catalog reports an actionable package requirement before browser launch',async()=>withTemp(async root=>{
  const target=await privateTarget(root);
  const result=await withHttpFixture({'/release-catalog.json':{contentType:'application/json',body:JSON.stringify({schema:'eagler-touhou/release-catalog/1',games:{}})}},
    baseUrl=>inspectRoomResources({baseUrl,targets:[target],packageMap:{}}));
  assert.equal(result.passed,false);assert.equal(result.games[0].error.code,'package-required');
  assert.match(result.games[0].error.message,/--packages=.*th08/);
}));

test('a stalled Release Catalog request aborts within its configured preflight timeout',async()=>withTemp(async root=>{
  const target=await privateTarget(root);
  const result=await withHttpFixture({'/release-catalog.json':{delayMs:100,contentType:'application/json',body:JSON.stringify({schema:'eagler-touhou/release-catalog/1',games:{}})}},
    baseUrl=>inspectRoomResources({baseUrl,targets:[target],packageMap:{},requestTimeoutMs:20}));
  assert.equal(result.passed,false);assert.equal(result.games[0].error.code,'http-timeout');
  assert.match(result.games[0].error.message,/exceeded 20 ms/);
}));

test('Release Catalog redirects are rejected during same-origin resource preflight',async()=>withTemp(async root=>{
  const target=await privateTarget(root);
  const result=await withHttpFixture({'/release-catalog.json':{status:302,headers:{location:'https://example.invalid/release-catalog.json'},body:''}},
    baseUrl=>inspectRoomResources({baseUrl,targets:[target],packageMap:{}}));
  assert.equal(result.passed,false);assert.equal(result.games[0].error.code,'http-unavailable');
}));

test('Release Catalog DATA must match the current private input and remote paths stay same-origin',async()=>withTemp(async root=>{
  const target=await privateTarget(root),published=Buffer.from('different published DATA'),descriptor=descriptorFor(published);
  const mismatch=await withHttpFixture({
    '/release-catalog.json':{contentType:'application/json',body:JSON.stringify({schema:'eagler-touhou/release-catalog/1',games:{th08:{revision:descriptor.revision,descriptor:'th08.package.json'}}})},
    '/th08.package.json':{contentType:'application/json',body:JSON.stringify(descriptor)},
    '/games/th08/game.data':{body:published},
  },baseUrl=>inspectRoomResources({baseUrl,targets:[target],packageMap:{}}));
  assert.equal(mismatch.passed,false);assert.equal(mismatch.games[0].error.code,'private-data-mismatch');
  const external=descriptorFor(data);external.files['game-data'].source='https://example.invalid/game.data';
  const blocked=await withHttpFixture({
    '/release-catalog.json':{contentType:'application/json',body:JSON.stringify({schema:'eagler-touhou/release-catalog/1',games:{th08:{revision:external.revision,descriptor:'th08.package.json'}}})},
    '/th08.package.json':{contentType:'application/json',body:JSON.stringify(external)},
  },baseUrl=>inspectRoomResources({baseUrl,targets:[target],packageMap:{}}));
  assert.equal(blocked.passed,false);assert.match(blocked.games[0].error.message,/invalid Package Descriptor|source path/i);
}));

test('Release Catalog data package cannot declare Runtime executable files',async()=>withTemp(async root=>{
  const target=await privateTarget(root),descriptor=descriptorFor(data);
  descriptor.files['old-runtime']={source:'runtime/th08.wasm',target:'/runtime/th08.wasm',revision:'old-runtime',bytes:12,sha256:hash(Buffer.from('old runtime'))};
  const result=await withHttpFixture({
    '/release-catalog.json':{contentType:'application/json',body:JSON.stringify({schema:'eagler-touhou/release-catalog/1',games:{th08:{revision:descriptor.revision,descriptor:'th08.package.json'}}})},
    '/th08.package.json':{contentType:'application/json',body:JSON.stringify(descriptor)},
  },baseUrl=>inspectRoomResources({baseUrl,targets:[target],packageMap:{}}));
  assert.equal(result.passed,false);assert.equal(result.games[0].error.code,'runtime-package-forbidden');
  assert.match(result.games[0].error.message,/runtime\/th08\.wasm/);
}));

test('site asset failures are recorded once outside game Runtime pass state',async()=>{
  const siteAssets=await inspectRoomSiteAssets({baseUrl:'http://127.0.0.1:8130/',fetchImpl:async url=>{
    const path=new URL(url).pathname;
    if(path.endsWith('/th20-card.webp'))return new Response('missing',{status:404});
    return new Response(assetBytes(path),{status:200});
  }});
  assert.equal(siteAssets.passed,false);assert.equal(siteAssets.assets.filter(asset=>asset.status==='failed').length,1);
  recordRoomSiteAssetFailures(siteAssets,[{url:'http://127.0.0.1:8130/assets/th20-card.webp',status:404},
    {url:'http://127.0.0.1:8130/runtime/th08.wasm',status:404}]);
  assert.equal(siteAssets.observedFailures.length,1);assert.equal(siteAssets.observedFailures[0].status,404);
});
