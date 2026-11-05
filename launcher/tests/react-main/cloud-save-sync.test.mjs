/** Cloud save journals, locks, account ownership and archive rollback. */
import {test,beforeAll,afterAll,beforeEach,afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp,rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {createHash,webcrypto} from 'node:crypto';
import {zipSync} from 'fflate';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';

const project=fileURLToPath(new URL('../../',import.meta.url));
const game=PRODUCT_GAMES.th06;
const scoreName=game.storage.scoreFile;
const allowed=[scoreName,...game.storage.configFiles,...(game.storage.hintFiles??[]),'score06.dat','spell_capture.dat'];
let work,api,originals=new Map();
const services=new Set();
function expose(name,value){if(!originals.has(name))originals.set(name,Object.getOwnPropertyDescriptor(globalThis,name));Object.defineProperty(globalThis,name,{configurable:true,writable:true,value});}
function hash(bytes){return createHash('sha256').update(new Uint8Array(bytes)).digest('hex');}
function makeFile(name,bytes){return new File([new Uint8Array(bytes)],name,{type:'application/octet-stream'});}
function manifestFile(name,bytes){return {name,bytes:bytes.length,sha256:hash(bytes)};}
function snapshot(files,revision=1){return {product:'th06',revision,uploadedAt:'2026-11-05T00:00:00.000Z',files,statistics:{game:'th06',parserVersion:1,format:'test',complete:false,
  playtimeSeconds:null,applicationTimeSeconds:null,playCount:null,clearCount:null,oneCreditClearCount:null,loadouts:[],progress:[],warnings:[]}};}
function listing({revision=0,files=[],pending=[]}={}){return {products:[{id:'th06',game:'th06',multiplayer:false,files:allowed,requiredFiles:[scoreName],revision}],saves:files.length?[snapshot(files,revision)]:[],pending,
  limits:{fileBytes:4*1024*1024,totalBytes:8*1024*1024,files:6}};}
function deferred(){let resolve,reject;const promise=new Promise((yes,no)=>{resolve=yes;reject=no;});return {promise,resolve,reject};}
class MemoryStorage{
  values=new Map();blockedKey=null;
  get length(){return this.values.size;}
  key(index){return [...this.values.keys()][index]??null;}
  getItem(key){return this.values.get(key)??null;}
  setItem(key,value){if(this.blockedKey===key)throw new Error('Storage write denied');this.values.set(key,String(value));}
  removeItem(key){this.values.delete(key);}
}
function environment(){
  const storage=new MemoryStorage(),events=new EventTarget(),reads=[];
  let current=listing(),available=true,uploaded=[],requests=[],decision=async()=> 'local',restore=async()=>{},completeGate;
  const backend=async(path,body,{signal}={})=>{
    requests.push({path,body});if(signal?.aborted)throw new DOMException('Cancelled','AbortError');
    if(!available)throw new api.AccountRequestError('ACCOUNT_API_UNAVAILABLE',503);
    if(path==='v1/me/cloud-saves')return current;
    if(path==='v1/me/cloud-saves/th06/sessions'){
      const run={deviceId:body.deviceId,runId:body.runId,baseRevision:body.baseRevision,startedAt:'2026-11-05T00:00:00.000Z'};
      if(body.baseRevision!==current.products[0].revision)throw new api.AccountRequestError('SAVE_CONFLICT',409);
      const old=current.pending.find(item=>item.product==='th06'&&item.deviceId===run.deviceId);
      if(old&&(old.runId!==run.runId||old.baseRevision!==run.baseRevision))throw new api.AccountRequestError('SAVE_PENDING',409);
      current={...current,pending:[...current.pending.filter(item=>item.deviceId!==run.deviceId),{product:'th06',...run}]};return {started:true};
    }
    if(path==='v1/me/cloud-saves/th06/sessions/complete'){
      if(body.baseRevision!==current.products[0].revision)throw new api.AccountRequestError('SAVE_CONFLICT',409);
      if(completeGate)await completeGate.promise;
      current={...current,pending:current.pending.filter(item=>item.deviceId!==body.deviceId)};return {completed:true};
    }
    if(path==='v1/me/cloud-saves/th06'){
      const baseRevision=Number(body.get('baseRevision')),deviceId=body.get('deviceId'),runId=body.get('runId');
      if(baseRevision!==current.products[0].revision)throw new api.AccountRequestError('SAVE_CONFLICT',409);
      if(deviceId&& !current.pending.some(item=>item.deviceId===deviceId&&item.runId===runId&&item.baseRevision===baseRevision))throw new api.AccountRequestError('SAVE_SESSION_CHANGED',409);
      const files=await Promise.all(body.getAll('files').map(async file=>({name:file.name,bytes:file.size,sha256:hash(new Uint8Array(await file.arrayBuffer()))})));
      const next=snapshot(files,baseRevision+1);current={...current,products:current.products.map(item=>({...item,revision:next.revision})),saves:[next],pending:current.pending.filter(item=>item.deviceId!==deviceId)};uploaded.push(next);return next;
    }
    if(path.endsWith('/download'))return globalThis.__downloadBlob;
    throw new Error(`Unexpected cloud API request: ${path}`);
  };
  globalThis.__cloudRequest=backend;
  return {storage,reads,requests,uploaded,get current(){return current;},set current(value){current=value;},set available(value){available=value;},set decision(value){decision=value;},set restore(value){restore=value;},set completeGate(value){completeGate=value;},
    makeSync({files=[],user='alice',decide,restore:restorePort,isRunning=()=>false,store=storage}={}){
      api.setAccountSession({status:'member',user:{username:user,name:user}});
      const sync=api.createCloudSaveSync({storage:store,read:async(_product,names)=>{reads.push([...names]);return files;},isRunning,notify:key=>requests.push({notify:key}),
        decide:decide??((...args)=>decision(...args)),restore:restorePort??((...args)=>restore(...args))});
      services.add(sync);return sync;
    }};
}

beforeAll(async()=>{
  expose('crypto',webcrypto);
  expose('navigator',{locks:undefined});
  expose('window',{addEventListener(){},removeEventListener(){},dispatchEvent(){return true;}});
  work=await mkdtemp(resolve(project,'.cache/cloud-save-sync-'));
  await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`
    export {createCloudSaveSync,localSaveFingerprint} from './app/services/cloud-save-sync.ts';
    export {readCloudSaveArchive,writeCloudSaveArchive} from './app/services/cloud-save-restore.ts';
    export {setAccountSession} from 'cloud-session-fixture';
  `},outfile:resolve(work,'cloud.mjs'),bundle:true,platform:'node',format:'esm',packages:'external',logLevel:'silent',
    plugins:[{name:'cloud-test-fixtures',setup(ctx){
      ctx.onResolve({filter:/account-client$/},()=>({path:'account-client-fixture',namespace:'cloud-fixture'}));
      ctx.onResolve({filter:/account-session$/},()=>({path:'account-session-fixture',namespace:'cloud-fixture'}));
      ctx.onResolve({filter:/^cloud-session-fixture$/},()=>({path:'account-session-fixture',namespace:'cloud-fixture'}));
      ctx.onLoad({filter:/.*/,namespace:'cloud-fixture'},args=>({loader:'js',contents:args.path==='account-client-fixture'?`
        export class AccountRequestError extends Error {constructor(code,status){super(code);this.name='AccountRequestError';this.code=code;this.status=status;}}
        export async function accountRequest(path,body,options={}){return globalThis.__cloudRequest(path,body,options);}
      `:`
        let state={status:'loading'},listeners=new Set();
        export const accountSession={getSnapshot:()=>state,subscribe(listener){listeners.add(listener);return()=>listeners.delete(listener);},whenReady:()=>Promise.resolve()};
        export function setAccountSession(value){state=value;for(const listener of listeners)listener();}
      `}));
    }},authoredSourcesPlugin(project)]});
  api=await import(pathToFileURL(resolve(work,'cloud.mjs')).href);
});
beforeEach(()=>{api.setAccountSession({status:'loading'});globalThis.__downloadBlob=new Blob([]);});
afterEach(()=>{for(const sync of services)sync.dispose();services.clear();delete globalThis.__cloudRequest;});
afterAll(async()=>{for(const [key,descriptor] of originals){if(descriptor)Object.defineProperty(globalThis,key,descriptor);else delete globalThis[key];}if(work)await rm(work,{recursive:true,force:true});});

test('a held product lock blocks a second tab before it reads local saves; refresh uploads a live journal immediately',async()=>{
  const f=environment(),bytes=new Uint8Array([1,2,3]),local=[makeFile(scoreName,bytes)];
  const first=f.makeSync({files:[]}),second=f.makeSync({files:local});
  assert.equal(await first.beforeLaunch('th06'),true);
  const readsBefore=f.reads.length;
  assert.equal(await second.beforeLaunch('th06'),false);
  assert.equal(f.reads.length,readsBefore,'the second tab never reads mutable game files');
  assert.ok(f.requests.some(item=>item.notify==='cloudSave.sameBrowserActive'));
  first.dispose();
  await new Promise(resolve=>setTimeout(resolve,5));
  await second.flush();
  assert.equal(f.uploaded.length,1,'the persisted run is reconciled as soon as the new page can acquire the lock');
  assert.equal(f.uploaded[0].files[0].sha256,hash(bytes));
  second.dispose();
});

test('a member can continue after an offline first launch and the durable journal uploads on reconnection',async()=>{
  const f=environment(),bytes=new Uint8Array([4,5,6]),local=[makeFile(scoreName,bytes)];
  f.available=false;
  const sync=f.makeSync({files:local,decide:async kind=>kind==='offline'?'local':'cancel'});
  assert.equal(await sync.beforeLaunch('th06'),true);
  const entry=JSON.parse(f.storage.getItem('eagler-cloud-sync-v1:save:th06'));
  assert.equal(entry.owner,'alice');assert.equal(entry.unclaimed,false);assert.equal(entry.registered,false);
  f.available=true;sync.dispose();
  const resumed=f.makeSync({files:local});
  await resumed.flush();
  assert.equal(f.uploaded.length,1);assert.equal(f.uploaded[0].files[0].sha256,hash(bytes));
  resumed.dispose();
});

test('an account switch cannot take over another member’s unuploaded run',async()=>{
  const f=environment(),syncA=f.makeSync({files:[]});
  assert.equal(await syncA.beforeLaunch('th06'),true);
  const registered=f.requests.find(item=>item.path==='v1/me/cloud-saves/th06/sessions').body;
  syncA.dispose();
  let otherAccountPrompt=0;
  const syncB=f.makeSync({user:'bob',files:[],decide:async kind=>{if(kind==='otherAccount')otherAccountPrompt++;return 'local';}});
  assert.equal(await syncB.beforeLaunch('th06'),false);
  assert.equal(otherAccountPrompt,1);assert.equal(f.storage.getItem('eagler-cloud-sync-v1:save:th06')&&JSON.parse(f.storage.getItem('eagler-cloud-sync-v1:save:th06')).owner,'alice');
  assert.equal(f.requests.filter(item=>item.path==='v1/me/cloud-saves/th06/sessions'&&item.body.runId!==registered.runId).length,0);
  syncB.dispose();
});

test('an identical upload still completes the new server run marker',async()=>{
  const f=environment(),bytes=new Uint8Array([5,5,5]),local=[makeFile(scoreName,bytes)];
  f.current=listing({revision:1,files:[manifestFile(scoreName,bytes)]});
  const sync=f.makeSync({files:local});
  assert.equal(await sync.beforeLaunch('th06'),true);
  sync.finished();await sync.flush();
  assert.equal(f.requests.filter(item=>item.path==='v1/me/cloud-saves/th06/sessions/complete').length,1);
  assert.equal(f.current.pending.length,0);
  assert.equal(JSON.parse(f.storage.getItem('eagler-cloud-sync-v1:save:th06')).runId,null);
  sync.dispose();
});

test('unchanged local bytes keep their old base and the next launch restores a newer cloud revision',async()=>{
  const f=environment(),oldBytes=new Uint8Array([1,2]),newBytes=new Uint8Array([3,4]),local=[makeFile(scoreName,oldBytes)],restored=[];
  f.current=listing({revision:1,files:[manifestFile(scoreName,oldBytes)]});
  const sync=f.makeSync({files:local,restore:async(_product,save)=>{restored.push(save.revision);local[0]=makeFile(scoreName,newBytes);}});
  assert.equal(await sync.beforeLaunch('th06'),true);
  f.current=listing({revision:2,files:[manifestFile(scoreName,newBytes)]});
  sync.finished();await sync.flush();
  const cleaned=JSON.parse(f.storage.getItem('eagler-cloud-sync-v1:save:th06'));
  assert.equal(cleaned.revision,1,'a no-change cleanup preserves its original cloud base');
  assert.equal(cleaned.runId,null);
  assert.equal(await sync.beforeLaunch('th06'),true);
  assert.deepEqual(restored,[2]);
  assert.equal(hash(new Uint8Array(await local[0].arrayBuffer())),hash(newBytes));
  assert.equal(f.requests.filter(item=>item.path==='v1/me/cloud-saves/th06/sessions').at(-1).body.baseRevision,2);
  sync.dispose();
});

test('a concurrent cloud revision leaves the pending local journal intact',async()=>{
  const f=environment(),local=[makeFile(scoreName,[9,8,7])],sync=f.makeSync({files:local});
  assert.equal(await sync.beforeLaunch('th06'),true);
  const run=JSON.parse(f.storage.getItem('eagler-cloud-sync-v1:save:th06'));
  f.current=listing({revision:1,files:[manifestFile(scoreName,[0,1,0])]});
  sync.finished();await sync.flush();
  const after=JSON.parse(f.storage.getItem('eagler-cloud-sync-v1:save:th06'));
  assert.equal(after.runId,run.runId);assert.equal(after.revision,0);assert.equal(sync.getSnapshot()?.error,'cloudSave.syncConflict');
  assert.equal(f.uploaded.length,0);sync.dispose();
});

test('account cancellation after cloud restore prevents session registration',async()=>{
  const f=environment(),cloudBytes=new Uint8Array([2,2,2]),cloud=snapshot([manifestFile(scoreName,cloudBytes)],1),waiting=deferred();
  f.current=listing({revision:1,files:cloud.files});
  let restoreStarted=false;
  const sync=f.makeSync({files:[],restore:async()=>{restoreStarted=true;await waiting.promise;}});
  const signal=new AbortController(),launch=sync.beforeLaunch('th06',signal.signal);
  while(!restoreStarted)await new Promise(resolve=>setTimeout(resolve,1));
  api.setAccountSession({status:'member',user:{username:'bob',name:'Bob'}});waiting.resolve();
  assert.equal(await launch,false);
  assert.equal(f.requests.some(item=>item.path==='v1/me/cloud-saves/th06/sessions'),false);
  sync.dispose();
});

test('a storage failure after registration leaves a recoverable journal and completion clears the marker without launch',async()=>{
  const f=environment(),storage=f.storage,key='eagler-cloud-sync-v1:save:th06',originalSet=storage.setItem.bind(storage),gate=deferred();
  f.completeGate=gate;let journalWrites=0;
  storage.setItem=(name,value)=>{if(name===key&&++journalWrites===2)throw new Error('second journal write denied');originalSet(name,value);};
  const sync=f.makeSync({files:[]});
  assert.equal(await sync.beforeLaunch('th06'),false);
  const run=JSON.parse(storage.getItem(key));
  assert.ok(run.runId);assert.equal(run.registered,false);
  assert.ok(f.current.pending.some(item=>item.runId===run.runId),'server registered the run before the second durable marker write failed');
  for(let i=0;i<80&&!f.requests.some(item=>item.path==='v1/me/cloud-saves/th06/sessions/complete');i++)await new Promise(resolve=>setTimeout(resolve,2));
  assert.ok(f.requests.some(item=>item.path==='v1/me/cloud-saves/th06/sessions/complete'),'the retained journal drives marker cleanup');
  gate.resolve();await sync.flush();
  assert.equal(f.current.pending.length,0);assert.equal(JSON.parse(storage.getItem(key)).runId,null);
  assert.ok(f.requests.some(item=>item.notify==='cloudSave.syncPending'));
  sync.dispose();
});

test('unknown identity preserves Alice’s journal; guest data requires claim; multiplayer bypasses cloud saves',async()=>{
  const f=environment(),local=[makeFile(scoreName,[6,7])],key='eagler-cloud-sync-v1:save:th06',runId='550e8400-e29b-41d4-a716-446655440000';
  f.storage.setItem(key,JSON.stringify({owner:'alice',product:'th06',revision:0,fingerprint:'',runId,activeUntil:0,unclaimed:false,registered:false}));
  const unknown=f.makeSync({files:local,decide:async kind=>kind==='offline'?'local':'cancel'});
  api.setAccountSession({status:'error'});
  assert.equal(await unknown.beforeLaunch('th06'),true);
  const preserved=JSON.parse(f.storage.getItem(key));assert.equal(preserved.owner,'alice');assert.equal(preserved.unclaimed,false);assert.equal(preserved.runId,runId);
  unknown.dispose();
  const resumed=f.makeSync({files:local});await resumed.flush();
  assert.equal(f.uploaded.length,1,'the confirmed owner uploads the pending bytes');resumed.dispose();

  const guestEnv=environment(),guestFiles=[makeFile(scoreName,[8,9])],claimCalls=[];
  const guest=guestEnv.makeSync({files:guestFiles});api.setAccountSession({status:'guest'});
  assert.equal(await guest.beforeLaunch('th06'),true);
  const guestRecord=JSON.parse(guestEnv.storage.getItem(key));assert.equal(guestRecord.owner,'');assert.equal(guestRecord.unclaimed,true);
  guest.finished();guest.dispose();
  let claim='cancel';
  const member=guestEnv.makeSync({files:guestFiles,decide:async kind=>{if(kind==='claim'){claimCalls.push(kind);return claim;}return 'local';}});
  assert.equal(await member.beforeLaunch('th06'),false);assert.equal(guestEnv.requests.some(item=>item.path==='v1/me/cloud-saves/th06/sessions'),false);
  claim='local';assert.equal(await member.beforeLaunch('th06'),true);assert.equal(claimCalls.length,2);
  const readsBefore=guestEnv.reads.length,requestsBefore=guestEnv.requests.length;
  assert.equal(await member.beforeLaunch('th06mp'),true);
  assert.equal(guestEnv.reads.length,readsBefore);assert.equal(guestEnv.requests.length,requestsBefore,'multiplayer bypasses cloud saves');
  member.dispose();
});

test('cloud archive enforces the product allowlist and each file SHA before writes',async()=>{
  environment();
  const bytes=new Uint8Array([7,6,5]),file=manifestFile(scoreName,bytes),save=snapshot([file]);
  globalThis.__downloadBlob=new Blob([zipSync({[scoreName]:bytes})]);
  assert.deepEqual([...((await api.readCloudSaveArchive(save,new AbortController().signal,allowed))[scoreName])],[...bytes]);
  await assert.rejects(api.readCloudSaveArchive(snapshot([manifestFile('../escape.dat',bytes)]),new AbortController().signal,allowed),/manifest/i);
  await assert.rejects(api.readCloudSaveArchive(snapshot([{...file,sha256:'0'.repeat(64)}]),new AbortController().signal,allowed),/checksum/i);
});

test('restore rolls back a cancellation after its final mutation and never retires',async()=>{
  const controller=new AbortController(),original=new Map([[scoreName,[1,1]],[allowed[1],[2,2]]]);
  const files=new Map([...original].map(([name,bytes])=>[name,[...bytes]]));let retireCount=0,syncCount=0;
  const access={async send(command,{path,bytes}={}){
    if(command==='read'){if(!files.has(path))throw Object.assign(new Error('missing'),{errno:44});return {bytes:files.get(path)};}
    if(command==='write')files.set(path,[...bytes]);
    if(command==='remove')files.delete(path);
    if(command==='remove')controller.abort();
    return {};
  },async sync(){syncCount++;},async retire(){retireCount++;}};
  await assert.rejects(api.writeCloudSaveArchive(access,{[scoreName]:new Uint8Array([9,9])},controller.signal,allowed),{name:'AbortError'});
  assert.deepEqual([...files], [...original]);assert.equal(retireCount,0);assert.equal(syncCount,1,'rollback commits the original bytes');
});

test('restore rolls back earlier files when a later write fails',async()=>{
  const original=new Map([[scoreName,[1,1]],[allowed[1],[2,2]]]),files=new Map([...original].map(([name,bytes])=>[name,[...bytes]]));
  let writes=0,retireCount=0;
  const access={async send(command,{path,bytes}={}){
    if(command==='read'){if(!files.has(path))throw Object.assign(new Error('missing'),{errno:44});return {bytes:files.get(path)};}
    if(command==='write'){writes++;if(writes===2)throw new Error('write failed');files.set(path,[...bytes]);}
    if(command==='remove')files.delete(path);return {};
  },async sync(){},async retire(){retireCount++;}};
  await assert.rejects(api.writeCloudSaveArchive(access,{[scoreName]:new Uint8Array([3,3]),[allowed[1]]:new Uint8Array([4,4])},new AbortController().signal,allowed),/write failed/);
  assert.deepEqual([...files],[...original]);assert.equal(retireCount,0);
});
