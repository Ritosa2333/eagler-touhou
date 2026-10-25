import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,readFileSync,writeFileSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join,resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {assertUserFileBoundary} from './user-file-boundary-evidence.mjs';
import {resolveAdaptationAdapter} from './registry.mjs';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {TH08_STORAGE_CONTRACT} from './titles/th08-storage.mjs';
import {deriveReplayStorageLocations,parseReplayStoragePolicy} from '../../../tools/ports/runtime-storage-contract.mjs';
import {matchExpectedOfflinePathTraversal} from '../../../tools/ports/offline-runtime-errors.mjs';
const hash=bytes=>createHash('sha256').update(bytes).digest('hex'),product={game:'th08',variant:'normal'};
function fixture(t){
  const directory=mkdtempSync(join(tmpdir(),'user-file-boundary-'));t.after(()=>rmSync(directory,{recursive:true,force:true}));
  const file=(path,kind,value)=>{const bytes=Buffer.from([value]),evidencePath=join(directory,path);writeFileSync(evidencePath,bytes);
    return{path,kind,bytes:1,sha256:hash(bytes),contentBase64:bytes.toString('base64'),evidencePath};};
  const statistics=value=>Array.from({length:7},(_,i)=>({total:i===0||i===6?value:0,characters:Array.from({length:12},(_,s)=>(i===0||i===6)&&s===0?value:0)}));
  const contract={source:'synthetic-native-storage',menu:TH08_STORAGE_CONTRACT.menu,selection:TH08_STORAGE_CONTRACT.selection,
    configuration:{path:'config',field:'volume',minimum:0,maximum:100,step:5},files:[
      {path:'config',kind:'configuration',inspect:b=>({values:{volume:b[0]},semanticSha256:hash(b)})},
      {path:'score',kind:'score',inspect:b=>({values:{statistics:statistics(b[0])},semanticSha256:hash(b)})}]};
  const snapshot=(volume,count)=>({files:[file('config','configuration',volume),file('score','score',count)].map(row=>({...row,inspection:contract.files.find(f=>f.path===row.path).inspect(Buffer.from(row.contentBase64,'base64'))}))});
  const replay={...file('replay.rpy','replay',17),runtimePath:'replay.rpy'};replay.sourceFixture={...replay};
  const native={schema:'eagler-touhou/native-storage-journey/1',source:contract.source,before:snapshot(80,0),authored:snapshot(80,1),reopened:snapshot(75,1),
    configuration:{path:'config',field:'volume',before:80,after:75},selection:{difficulty:0,character:0},
    selectionObservations:{difficulty:{scene:1,page:4,phase:1,cursor:0},character:{scene:1,page:5,phase:1,cursor:0}},
    entered:{schema:'eagler-touhou/runtime-observation/1',...product,phase:{name:'stage'},capabilities:{phase:true,input:true,position:true},input:{value:0},position:{x:1,y:1},pause:{active:false}},
    enteredSelection:{difficulty:0,character:0,practice:0,replay:0,cheats:0,assisted:0,gameFlags:4,rawWords:[0,0,0,0,0,0,0]},
    lifecycle:{beforeEpoch:1,afterEpoch:2,exitAbsentBeforeStop:true,stopError:null,exit:{event:'exit',epoch:1,code:0,status:'success'},first:{event:'first-frame',epoch:2}},
    replay,replayRestored:{reopened:{...replay}}};
  const source=readFileSync(resolve(import.meta.dirname,'../../../launcher/src/launcher/replay-storage-policy.mts')),
    locations=deriveReplayStorageLocations({game:'th08',product:PRODUCT_GAMES.th08,policy:parseReplayStoragePolicy(source.toString('utf8')),language:'ja'}),
    adapter={...resolveAdaptationAdapter('th08','normal'),storage:contract},outside={path:'/savesth08/../outside-offline-acceptance.dat',exists:false};
  const document=epoch=>({url:`https://fixture/runtime?runtimeEpoch=${epoch}`,epoch,app:9});
  const e={language:'ja',mapping:adapter.runtime,locations,policySource:{bytes:source.length,sha256:hash(source)},packageSha256:'a'.repeat(64),
    mount:{available:true,idbfs:true,mountpoint:'/savesth08',expectedDatabase:'/savesth08',runtimeGlobal:adapter.runtime.global},
    snapshot:{root:'/savesth08',document:document(2),outside:{...outside},files:[...native.reopened.files,replay].map(row=>({...row,
      absolutePath:'/savesth08/'+row.path,resolvedPath:'/savesth08/'+row.path,mountpoint:'/savesth08',artifact:row.evidencePath}))},
    beforeTraversal:{root:'/savesth08',document:document(1),outside:{...outside}},afterTraversal:{root:'/savesth08',document:document(1),outside:{...outside}},
    lists:{before:[{path:'config',size:1}],after:[{path:'config',size:1}]},negatives:['read','write','remove'].map((command,index)=>{
      const response={protocol:'eagler-touhou/1',game:'th08',epoch:1,request:'negative-'+index,ok:false,error:'Error: Invalid save path'};
      const classification=matchExpectedOfflinePathTraversal(response,{game:'th08',epoch:1,command,path:'../outside-offline-acceptance.dat',requestId:response.request});return{classification,response};})};
  return{e,product,adapter,driver:{schema:'eagler-touhou/offline-runtime-browser/1',scope:product,
    runtimePackages:{normal:{packageSha256:e.packageSha256}},nativeUserData:{observation:native},userFiles:{sync:replay}}};
}
test('native user-file proof binds all three file classes to the actual normal mount',t=>{
  assert.deepEqual(assertUserFileBoundary(fixture(t)),{paths:['config','score','replay.rpy'],root:'/savesth08'});
});
for(const [name,mutate] of Object.entries({
  'MP mount reused':a=>a.e.mount.mountpoint='/savesth08-multiplayer',
  'MEMFS substituted':a=>a.e.mount.idbfs=false,
  'canonical path escaped':a=>a.e.snapshot.files[0].resolvedPath='/config',
  'score class missing':a=>a.e.snapshot.files.splice(1,1),
  'readback artifact changed':a=>writeFileSync(a.e.snapshot.files[0].artifact,Buffer.from([0])),
  'wrong file mount':a=>a.e.snapshot.files[0].mountpoint='/',
  'outside file created':a=>a.e.afterTraversal.outside.exists=true,
  'no explicit path rejection':a=>a.e.negatives[0].response.error='ENOENT',
  'foreign request':a=>a.e.negatives[1].response.request='other-request',
  'foreign epoch':a=>a.e.negatives[1].response.epoch=2,
  'list changed after rejection':a=>a.e.lists.after.push({path:'outside',size:1}),
  'missing remove negative':a=>a.e.negatives.pop(),
  'stale reopened epoch':a=>a.e.snapshot.document.epoch=1,
  'wrong package':a=>a.e.packageSha256='b'.repeat(64),
  'policy changed':a=>a.e.policySource.sha256='b'.repeat(64),
}))test(`native user-file proof rejects ${name}`,t=>{const args=fixture(t);mutate(args);assert.throws(()=>assertUserFileBoundary(args));});
