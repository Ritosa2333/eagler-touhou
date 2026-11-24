// Current production archive caller regression after adding the file-backed source.
// Store is an explicit memory receipt seam, never actual IndexedDB or a durable save.
import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {installDatPackage,reopenDatPackage} from '../../site/runtime/dat-package-install.mjs';
import {fixture,packageZip,origin,rawBytes,sha} from './file-fixture.mjs';
function memoryStore(){let current=null;return{writes:0,async readCurrent(){return current;},async commit(value){this.writes++;current=value;}};}
test('unchanged archive install caller validates all targets and reopens its exact archive record',async()=>{
 const f=fixture(),zip=packageZip(f),store=memoryStore(),progress=[];
 let installed=await installDatPackage(zip,{store,origin,onProgress:row=>progress.push(row)});
 try{
  const saved=await store.readCurrent();
  assert.equal(store.writes,1);assert.equal(saved.revision,'authored-files-fixture-1');assert.equal(saved.bytes,zip.length);
  assert.equal(saved.sha256,sha(zip));assert.equal(sha(new Uint8Array(await saved.blob.arrayBuffer())),sha(zip));
  assert.equal(installed.targets.length,4);assert(progress.some(row=>row.verified===1&&row.total===4));
  assert.deepEqual(new Uint8Array(await(await installed.fetch(origin+'/assets/runtime/front.anm')).arrayBuffer()),rawBytes);
 }finally{installed.dispose();}
 installed=await reopenDatPackage({store,origin});
 try{
  assert.equal(store.writes,1);
  assert.deepEqual(new Uint8Array(await(await installed.fetch(origin+'/assets/runtime/front.anm')).arrayBuffer()),rawBytes);
 }finally{installed.dispose();}
});
test('bad archive update leaves the existing install record and live provider intact',async()=>{
 const f=fixture(),zip=packageZip(f),store=memoryStore(),live=await installDatPackage(zip,{store,origin}),before=await store.readCurrent();
 try{
  const bad=new Uint8Array(zip);bad[31]^=1;
  await assert.rejects(installDatPackage(bad,{store,origin}),/CRC|ZIP|name|directory|descriptor|package/i);
  assert.equal(store.writes,1);assert.equal(await store.readCurrent(),before);
  assert.deepEqual(new Uint8Array(await(await live.fetch(origin+'/assets/runtime/front.anm')).arrayBuffer()),rawBytes);
 }finally{live.dispose();}
});
test('cancelled current archive caller never commits a candidate after validation yields',async()=>{
 const f=fixture(),store=memoryStore(),controller=new AbortController();
 await assert.rejects(installDatPackage(packageZip(f),{store,origin,signal:controller.signal,onProgress:row=>{if(row.completed===1)controller.abort();}}),error=>error.name==='AbortError');
 assert.equal(store.writes,0);assert.equal(await store.readCurrent(),null);
});
test('complete changed provider and actual archive consumer modules parse without extraction seams',()=>{
 for(const name of ['dat-ogg-provider','dat-package-install','th128-dat','zip-reader','crc32','ecl-includes']){
  const file=new URL('../../site/runtime/'+name+'.mjs',import.meta.url);
  const result=spawnSync(process.execPath,['--check',file.pathname.replace(/^\/([A-Za-z]:)/,'$1')],{encoding:'utf8',windowsHide:true});
  assert.equal(result.status,0,result.stderr);
 }
});
