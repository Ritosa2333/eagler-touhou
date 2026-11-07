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
  let current=listing(),available=true,unauthorized=false,uploaded=[],requests=[];
  const backend=async(path,body,{signal}={})=>{
    requests.push({path,body});
    if(signal?.aborted)throw new DOMException('Cancelled','AbortError');
    if(unauthorized)throw new api.AccountRequestError('SIGN_IN_REQUIRED',401);
    if(!available)throw new api.AccountRequestError('ACCOUNT_API_UNAVAILABLE',503);
    if(path==='v1/me/cloud-saves')return current;
    if(path==='v1/me/cloud-saves/th06/sessions'){
      assert.equal(body.allowPending,true);
      if(body.baseRevision!==current.products[0].revision)throw new api.AccountRequestError('SAVE_CONFLICT',409);
      const existing=current.pending.find(item=>item.deviceId===body.deviceId);
      if(existing){
        if(existing.runId===body.runId&&existing.baseRevision===body.baseRevision)return {started:true};
        throw new api.AccountRequestError('SAVE_PENDING',409);
      }
      const marker={product:'th06',deviceId:body.deviceId,runId:body.runId,baseRevision:body.baseRevision,startedAt:'2026-11-06T00:00:00.000Z'};
      current={...current,pending:[...current.pending,marker]};return {started:true};
    }
    if(path==='v1/me/cloud-saves/th06/sessions/complete'){
      if(body.baseRevision!==current.products[0].revision)throw new api.AccountRequestError('SAVE_CONFLICT',409);
      const marker=current.pending.find(item=>item.deviceId===body.deviceId);
      if(marker&&marker.runId!==body.runId)throw new api.AccountRequestError('SAVE_SESSION_CHANGED',409);
      current={...current,pending:current.pending.filter(item=>item.deviceId!==body.deviceId)};return {completed:true};
    }
    if(path==='v1/me/cloud-saves/th06'){
      const baseRevision=Number(body.get('baseRevision'));
      if(baseRevision!==current.products[0].revision)throw new api.AccountRequestError('SAVE_CONFLICT',409);
      assert.equal(body.has('runId'),false,'new clients upload by revision alone');
      const files=await Promise.all(body.getAll('files').map(async file=>({name:file.name,bytes:file.size,sha256:hash(new Uint8Array(await file.arrayBuffer()))})));
      if(baseRevision!==current.products[0].revision)throw new api.AccountRequestError('SAVE_CONFLICT',409);
      const next=snapshot(files,baseRevision+1);current={...current,products:current.products.map(item=>({...item,revision:next.revision})),saves:[next]};uploaded.push(next);return next;
    }
    if(path.endsWith('/preview'))return {product:'th06',files:[],statistics:snapshot([]).statistics};
    if(path.endsWith('/download'))return globalThis.__downloadBlob;
    throw new Error(`Unexpected cloud API request: ${path}`);
  };
  globalThis.__cloudRequest=backend;
  return {storage,reads,requests,uploaded,get current(){return current;},set current(value){current=value;},set available(value){available=value;},set unauthorized(value){unauthorized=value;},
    makeSync({files=[],user='alice',decide,decideStart=async kind=>kind==='other-device',restore:restorePort,isRunning=()=>false,store=storage}={}){
      api.setAccountSession({status:'member',user:{username:user,name:user}});
      const sync=api.createCloudSaveSync({storage:store,read:async(_product,names)=>{reads.push([...names]);return files;},isRunning,notify:key=>requests.push({notify:key}),
        decide:decide??(async()=>'local'),decideStart,closeForHandoff:async()=>false,retirePrepared:async()=>true,restore:restorePort??(async()=>{})});
      services.add(sync);return sync;
    }};
}

beforeAll(async()=>{
  expose('crypto',webcrypto);
  expose('navigator',{locks:undefined});
  expose('window',new EventTarget());
  work=await mkdtemp(resolve(project,'.cache/cloud-save-sync-'));
  await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`
    export {createCloudSaveSync} from './app/services/cloud-save-sync.ts';
    export {localSaveFingerprint} from './app/services/cloud-save-comparison.ts';
    export {readCloudSaveArchive,writeCloudSaveArchive} from './app/services/cloud-save-restore.ts';
    export {setAccountSession} from 'cloud-session-fixture';
    export {AccountRequestError} from './app/services/account-client';
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
        export const accountSession={getSnapshot:()=>state,subscribe(listener){listeners.add(listener);return()=>listeners.delete(listener);},expire(username){if(state.status==='member'&&state.user.username===username)setAccountSession({status:'guest',expired:true});}};
        export function setAccountSession(value){state=value;for(const listener of listeners)listener();}
      `}));
    }},authoredSourcesPlugin(project)]});
  api=await import(pathToFileURL(resolve(work,'cloud.mjs')).href);
});
beforeEach(()=>{api.setAccountSession({status:'loading'});globalThis.__downloadBlob=new Blob([]);});
afterEach(()=>{for(const sync of services)sync.dispose();services.clear();delete globalThis.__cloudRequest;});
afterAll(async()=>{for(const [key,descriptor] of originals){if(descriptor)Object.defineProperty(globalThis,key,descriptor);else delete globalThis[key];}if(work)await rm(work,{recursive:true,force:true});});

const journalKey='eagler-cloud-sync-v1:save:th06';
const journal=f=>JSON.parse(f.storage.getItem(journalKey));
const sessions=f=>f.requests.filter(item=>item.path==='v1/me/cloud-saves/th06/sessions');
const signal=()=>new AbortController().signal;
const beforeStart=sync=>sync.beforeStart('th06',signal());
const oldMarker=(deviceId='550e8400-e29b-41d4-a716-446655440000',baseRevision=0)=>({
  product:'th06',deviceId,runId:'550e8400-e29b-41d4-a716-446655440001',baseRevision,startedAt:'2026-11-06T00:00:00.000Z',
});
async function until(test) {
  for(let i=0;i<200;i++){if(test())return;await new Promise(resolve=>setTimeout(resolve,2));}
  assert.fail('cloud save state did not settle');
}
function startRun(sync,product='th06') {sync.prepareLocalLaunch();sync.started(product);}

test('preparing a local game makes no cloud request and an offline run uploads after reconnection',async()=>{
  const f=environment(),bytes=new Uint8Array([4,5,6]),local=[makeFile(scoreName,bytes)];
  f.available=false;
  const sync=f.makeSync({files:local,decide:()=>assert.fail('an offline launch must not open a dialog')});
  sync.prepareLocalLaunch();
  await Promise.resolve();
  assert.equal(f.requests.length,0);
  sync.started('th06');
  await until(()=>journal(f)?.runId);
  assert.equal(journal(f).owner,'alice');
  assert.equal(f.requests.length,0);
  assert.equal(f.reads.length,0,'launch and live checks do not read or replace native saves');
  assert.equal(f.uploaded.length,0);
  sync.dispose();
  f.available=true;
  const reopened=f.makeSync({files:local});
  await reopened.flush();
  assert.equal(f.uploaded.length,1);
  assert.equal(f.uploaded[0].files[0].sha256,hash(bytes));
  assert.equal(journal(f).runId,null);
});

test('a tab without a compatible handoff receiver leaves the original game running',async()=>{
  const f=environment(),local=[makeFile(scoreName,[1,2,3])],seen=[];
  const first=f.makeSync({files:local});
  const second=f.makeSync({files:local,decideStart:async kind=>{seen.push(kind);return false;}});
  startRun(first);await until(()=>journal(f)?.runId);
  const original=journal(f);
  assert.equal(await beforeStart(second),'cancel');
  assert.deepEqual(seen,['tab-unavailable']);
  assert.equal(f.reads.length,0);
  assert.equal(journal(f).runId,original.runId);
  assert.equal(f.storage.getItem('eagler-cloud-sync-v1:review:th06'),null);
  first.finished();await first.flush();
  assert.equal(f.uploaded.length,1);
});

test('automatic cloud sync is account-scoped, pauses without losing its journal, and resumes after reload',async()=>{
  const f=environment(),local=[makeFile(scoreName,[1,2,3])],alicePref='eagler-cloud-sync-v1:paused:alice',bobPref='eagler-cloud-sync-v1:paused:bob';
  const alice=f.makeSync({files:local});
  assert.equal(alice.getEnabled(),true);
  startRun(alice);await until(()=>journal(f)?.runId);
  const registered=journal(f);
  local[0]=makeFile(scoreName,[4,5,6]);
  alice.setEnabled(false);alice.finished();await alice.flush();
  assert.equal(journal(f).runId,registered.runId);
  assert.equal(f.current.pending.length,0);assert.equal(f.uploaded.length,0);
  alice.dispose();
  const bob=f.makeSync({files:local,user:'bob'});
  assert.equal(bob.getEnabled(),true);
  bob.setEnabled(false);assert.equal(f.storage.getItem(alicePref),'1');assert.equal(f.storage.getItem(bobPref),'1');bob.dispose();
  const reopened=f.makeSync({files:local});
  assert.equal(reopened.getEnabled(),false);
  const requests=f.requests.length;
  startRun(reopened);await Promise.resolve();reopened.finished();await reopened.flush();
  assert.equal(f.requests.length,requests);
  assert.equal(journal(f).runId,registered.runId);
  reopened.setEnabled(true);await reopened.flush();
  assert.equal(f.uploaded.length,1);assert.equal(f.current.pending.length,0);
});

test('same-account tabs follow cloud sync preference storage events',()=>{
  const f=environment(),aliceOne=f.makeSync(),aliceTwo=f.makeSync(),pref='eagler-cloud-sync-v1:paused:alice';
  const changed=value=>{const event=new Event('storage');Object.defineProperty(event,'key',{value:pref});window.dispatchEvent(event);assert.equal(aliceTwo.getEnabled(),value);};
  aliceOne.setEnabled(false);changed(false);
  aliceOne.setEnabled(true);changed(true);
});

test('an account switch keeps the original journal and quarantines subsequent local changes',async()=>{
  const f=environment(),local=[makeFile(scoreName,[1,2])],alice=f.makeSync({files:local});
  startRun(alice);await until(()=>journal(f)?.runId);
  const original=journal(f);alice.dispose();
  const bob=f.makeSync({user:'bob',files:local,decide:()=>assert.fail('a different account must not gate play')});
  startRun(bob);await until(()=>bob.getSnapshot()?.error==='cloudSave.otherAccountPending');
  local[0]=makeFile(scoreName,[8,9]);bob.finished();await bob.flush();bob.dispose();
  assert.equal(journal(f).owner,'alice');assert.equal(journal(f).runId,original.runId);
  assert.equal(sessions(f).length,0);assert.equal(f.uploaded.length,0);
  const reopened=f.makeSync({files:local});await reopened.flush();
  assert.equal(f.uploaded.length,0,'Alice cannot automatically upload bytes changed during Bob’s run');
  assert.equal(reopened.hasLocalPending('th06'),true);
});

test('an obsolete marker for this browser is retired without uploading unchanged bytes',async()=>{
  const f=environment(),bytes=new Uint8Array([5,5,5]),local=[makeFile(scoreName,bytes)];
  const marker=oldMarker();
  f.storage.setItem('eagler-cloud-sync-v1:device',marker.deviceId);
  f.current=listing({revision:1,files:[manifestFile(scoreName,bytes)],pending:[marker]});
  const sync=f.makeSync({files:local});
  assert.equal(await beforeStart(sync),'continue');
  sync.started('th06');await until(()=>journal(f)?.runId);
  sync.finished();await sync.flush();
  assert.equal(f.requests.filter(item=>item.path==='v1/me/cloud-saves/th06/sessions/complete').length,1);
  assert.equal(f.current.pending.length,0);assert.equal(journal(f).runId,null);
  assert.equal(f.uploaded.length,0);
});

test('newer cloud data never replaces the running save and can be restored explicitly after exit',async()=>{
  const f=environment(),oldBytes=new Uint8Array([1,2]),newBytes=new Uint8Array([3,4]),local=[makeFile(scoreName,oldBytes)],restored=[];
  f.current=listing({revision:2,files:[manifestFile(scoreName,newBytes)]});
  const sync=f.makeSync({files:local,decide:async()=> 'cloud',restore:async(_product,save)=>{restored.push(save.revision);local[0]=makeFile(scoreName,newBytes);}});
  startRun(sync);await until(()=>journal(f)?.runId);
  const signal=new AbortController().signal;
  await sync.resolve('th06',signal);assert.deepEqual(restored,[]);
  sync.finished();await sync.flush();
  assert.equal(journal(f).revision,0);assert.ok(journal(f).runId);
  await sync.resolve('th06',signal);
  assert.deepEqual(restored,[2]);assert.equal(journal(f).revision,2);assert.equal(journal(f).runId,null);
  assert.equal(f.uploaded.length,0);
});

test('reopening the same browser after a forced close resumes committed files without an other-device decision',async()=>{
  const f=environment(),local=[makeFile(scoreName,[1])];
  f.current=listing({revision:1,files:[manifestFile(scoreName,[1])]});
  const previous=f.makeSync({files:local});
  assert.equal(await beforeStart(previous),'continue');
  previous.started('th06');
  await until(()=>f.current.pending.length===1);
  const marker=f.current.pending[0];
  local[0]=makeFile(scoreName,[5]);
  previous.dispose();
  let preparing=true;
  const reopened=f.makeSync({files:local,isRunning:()=>preparing,
    decideStart:()=>assert.fail('the retained marker belongs to this browser'),
    decide:()=>assert.fail('only the browser save changed')});
  reopened.prepareLocalLaunch();
  assert.equal(await beforeStart(reopened),'continue');
  assert.deepEqual(f.current.pending,[marker]);
  reopened.started('th06');
  preparing=false;
  reopened.finished();
  await reopened.flush();
  assert.equal(f.uploaded.length,1);
  assert.equal(f.uploaded[0].files[0].sha256,hash([5]));
  assert.equal(f.current.pending.length,0);
});

test('other-device progress asks before reading local files and Stop leaves both versions intact',async()=>{
  const f=environment(),seen=[];
  f.current=listing({revision:1,files:[manifestFile(scoreName,[1])],pending:[oldMarker()]});
  const sync=f.makeSync({files:[makeFile(scoreName,[9])],decideStart:async kind=>{seen.push(kind);return false;},
    decide:()=>assert.fail('pending progress gets a separate decision')});
  assert.equal(await beforeStart(sync),'cancel');
  assert.equal(await beforeStart(sync),'cancel','Stop does not acknowledge the other device');
  assert.deepEqual(seen,['other-device','other-device']);
  assert.equal(f.reads.length,0);
  assert.equal(journal(f),null);
  assert.equal(f.uploaded.length,0);
  assert.equal(sessions(f).length,0);
  assert.deepEqual(f.current.pending,[oldMarker()]);
});

test('Continue and overwrite uses browser progress and uploads only after exit',async()=>{
  const f=environment(),local=[makeFile(scoreName,[4,5,6])],seen=[];
  f.current=listing({revision:1,files:[manifestFile(scoreName,[1,2,3])],pending:[oldMarker()]});
  const sync=f.makeSync({files:local,decideStart:async kind=>{seen.push(kind);return true;},
    decide:()=>assert.fail('explicit overwrite needs no second confirmation')});
  assert.equal(await beforeStart(sync),'continue');
  assert.equal(f.uploaded.length,0);
  sync.started('th06');await until(()=>journal(f)?.runId);
  assert.equal(f.uploaded.length,0);
  sync.finished();await sync.flush();
  assert.equal(f.uploaded.length,1);
  assert.equal(f.uploaded[0].files[0].sha256,hash([4,5,6]));
  assert.equal(await beforeStart(sync),'continue');
  assert.deepEqual(seen,['other-device']);
  assert.equal(f.requests.some(item=>item.notify),false);
  assert.deepEqual(f.current.pending,[oldMarker()]);
});

test('a cloud revision that changes during confirmation requires renewed consent',async()=>{
  const f=environment(),local=[makeFile(scoreName,[8])],seen=[];
  f.current=listing({revision:1,files:[manifestFile(scoreName,[1])],pending:[oldMarker()]});
  const sync=f.makeSync({files:local,decideStart:async kind=>{
    seen.push(kind);
    if(seen.length===1){f.current=listing({revision:2,files:[manifestFile(scoreName,[2])],pending:[oldMarker()]});return true;}
    return false;
  }});
  assert.equal(await beforeStart(sync),'cancel');
  assert.deepEqual(seen,['other-device','other-device']);
  assert.equal(journal(f),null);
  assert.equal(f.uploaded.length,0);
});

test('a failed local journal write does not register a remote run or upload untracked bytes',async()=>{
  const f=environment(),sync=f.makeSync({files:[makeFile(scoreName,[1])]});
  f.storage.blockedKey=journalKey;
  startRun(sync);await until(()=>sync.getSnapshot()?.error==='cloudSave.syncPending');
  sync.finished();await sync.flush();
  assert.equal(journal(f),null);assert.equal(f.requests.some(item=>item.path),false);
});

test('an unverified account cannot auto-upload guest files, and multiplayer skips cloud saves',async()=>{
  const f=environment(),local=[makeFile(scoreName,[6,7])],sync=f.makeSync({files:local});
  api.setAccountSession({status:'error'});
  startRun(sync,'th06mp');assert.equal(journal(f),null);
  startRun(sync);await until(()=>journal(f)?.unclaimed===true);
  api.setAccountSession({status:'member',user:{username:'alice',name:'Alice'}});
  sync.finished();await sync.flush();
  assert.equal(f.uploaded.length,0);assert.equal(sessions(f).length,0);
  assert.equal(sync.hasLocalPending('th06'),true);
});

test('an expired account sends no requests until a verified login is restored',async()=>{
  const f=environment(),local=[makeFile(scoreName,[7,8,9])],sync=f.makeSync({files:local});
  f.unauthorized=true;
  startRun(sync);await until(()=>journal(f)?.runId);
  sync.finished();await sync.flush();
  assert.equal(sync.getEnabled(),null);
  assert.equal(journal(f).owner,'alice');
  assert.equal(f.requests.filter(item=>item.path==='v1/me/cloud-saves').length,1);
  sync.finished();await sync.flush();
  window.dispatchEvent(new Event('online'));await sync.flush();
  assert.equal(f.requests.filter(item=>item.path==='v1/me/cloud-saves').length,1);
  assert.equal(f.uploaded.length,0);
  f.unauthorized=false;
  api.setAccountSession({status:'member',user:{username:'alice',name:'Alice'}});
  await sync.flush();
  assert.equal(f.uploaded.length,1);
});

test('a locally known expired session is skipped before the first cloud request',async()=>{
  const f=environment(),sync=f.makeSync();
  api.setAccountSession({status:'member',user:{username:'alice',name:'Alice'},expiresAt:Date.now()-1});
  startRun(sync);await until(()=>journal(f)?.unclaimed===true);
  sync.finished();await sync.flush();
  assert.equal(sync.getEnabled(),null);
  assert.equal(f.requests.some(item=>item.path),false);
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

test('launch comparison shows both save sources and browser choice defers its explicit overwrite until exit',async()=>{
 const f=environment(),local=[makeFile(scoreName,[9,8,7])];f.current=listing({revision:3,files:[manifestFile(scoreName,[1,2,3])]});
 let seen;
 const sync=f.makeSync({files:local,decide:async(kind,product,signal,comparison,context)=>{seen={kind,product,comparison,context};return 'local';}});
 sync.prepareLocalLaunch('th06');
 assert.equal(await sync.beforeStart('th06',new AbortController().signal),'continue');
 assert.equal(seen.context,'launch');assert.equal(seen.comparison.cloud.revision,3);assert.equal(seen.comparison.local.files[0].bytes,3);
 assert.equal(f.uploaded.length,0,'launch never uploads before gameplay');
 assert.equal(journal(f).revision,3);sync.started('th06');await until(()=>journal(f)?.runId);
 sync.finished();await sync.flush();await until(()=>f.uploaded.length===1);
 assert.equal(f.uploaded[0].files[0].sha256,hash([9,8,7]));assert.equal(journal(f).runId,null);
});

test('launch cloud choice restores before native start and requests fresh native preparation',async()=>{
 const f=environment();f.current=listing({revision:2,files:[manifestFile(scoreName,[1])]});let restores=0;
 const sync=f.makeSync({files:[makeFile(scoreName,[2])],decide:async()=> 'cloud',restore:async(product,snapshot,signal,names,preparing)=>{assert.equal(preparing,true);assert.equal(snapshot.revision,2);restores++;}});
 sync.prepareLocalLaunch('th06');assert.equal(await sync.beforeStart('th06',new AbortController().signal),'reload');
 assert.equal(restores,1);assert.equal(f.uploaded.length,0);assert.equal(journal(f).revision,2);assert.equal(journal(f).runId,null);
});

test('skipping launch comparison plays browser bytes and never overwrites cloud on exit',async()=>{
 const f=environment();f.current=listing({revision:2,files:[manifestFile(scoreName,[1])]});
 const sync=f.makeSync({files:[makeFile(scoreName,[2])],decide:async()=> 'cancel'});
 sync.prepareLocalLaunch('th06');assert.equal(await sync.beforeStart('th06',new AbortController().signal),'continue');
 sync.started('th06');await Promise.resolve();sync.finished();await sync.flush();assert.equal(f.uploaded.length,0);assert.equal(f.requests.some(item=>item.notify),false);
});

test('identical browser and cloud versions start directly and claim the matching baseline',async()=>{
 const f=environment();f.current=listing({revision:4,files:[manifestFile(scoreName,[1])]});
 const sync=f.makeSync({files:[makeFile(scoreName,[1])],decide:()=>assert.fail('identical saves need no dialog')});
 sync.prepareLocalLaunch('th06');assert.equal(await sync.beforeStart('th06',new AbortController().signal),'continue');assert.equal(journal(f).revision,4);
});

test('a cloud head changed during the dialog is compared again instead of silently overwritten',async()=>{
 const f=environment();f.current=listing({revision:2,files:[manifestFile(scoreName,[1])]});let decisions=0;
 const sync=f.makeSync({files:[makeFile(scoreName,[8])],decide:async()=>{decisions++;if(decisions===1)f.current=listing({revision:3,files:[manifestFile(scoreName,[3])]});return 'local';}});
 sync.prepareLocalLaunch('th06');await sync.beforeStart('th06',new AbortController().signal);assert.equal(decisions,2);assert.equal(journal(f).revision,3);assert.equal(f.uploaded.length,0);
});

test('known expired accounts skip prelaunch metadata and preview requests',async()=>{
 const f=environment(),sync=f.makeSync({files:[makeFile(scoreName,[1])]});
 api.setAccountSession({status:'member',user:{username:'alice',name:'alice'},expiresAt:Date.now()-1});
 sync.prepareLocalLaunch('th06');assert.equal(await sync.beforeStart('th06',new AbortController().signal),'continue');assert.equal(f.requests.length,0);
});

test('an unresponsive cloud listing has a bounded wait and still allows local play',async()=>{
 const f=environment();globalThis.__cloudRequest=async(path)=>{f.requests.push({path});return new Promise(()=>{});};
 const sync=f.makeSync({files:[makeFile(scoreName,[1])],decide:()=>assert.fail('no remote data means no misleading comparison')});
 const start=Date.now();sync.prepareLocalLaunch('th06');
 assert.equal(await sync.beforeStart('th06',new AbortController().signal),'continue');
 assert.ok(Date.now()-start<3500);assert.equal(f.reads.length,0);assert.equal(f.uploaded.length,0);
});

test('a failed cloud restore falls back to local play and warns only once',async()=>{
 const f=environment();f.current=listing({revision:2,files:[manifestFile(scoreName,[1])]});const contexts=[];
 const sync=f.makeSync({files:[makeFile(scoreName,[2])],decide:async(_kind,_product,_signal,_comparison,context)=>{contexts.push(context);return 'cloud';},restore:async()=>{throw new Error('network failed');}});
 assert.equal(await beforeStart(sync),'continue');
 assert.deepEqual(contexts,['launch']);assert.equal(f.uploaded.length,0);
 assert.deepEqual(f.requests.filter(item=>item.notify).map(item=>item.notify),['cloudSave.restoreFailed']);
 assert.ok(f.storage.getItem('eagler-cloud-sync-v1:review:th06'));
});

test('explicit browser overwrite keeps cloud CAS protection after another device saves',async()=>{
 const f=environment(),local=[makeFile(scoreName,[8])];f.current=listing({revision:2,files:[manifestFile(scoreName,[1])]});
 const sync=f.makeSync({files:local,decide:async()=> 'local'});sync.prepareLocalLaunch('th06');await sync.beforeStart('th06',new AbortController().signal);
 sync.started('th06');await until(()=>journal(f)?.runId);f.current=listing({revision:3,files:[manifestFile(scoreName,[3])]});
 sync.finished();await sync.flush();assert.equal(f.uploaded.length,0);assert.equal(sync.getSnapshot()?.error,'cloudSave.syncConflict');
});

test('an unchanged browser fast-forwards across many cloud saves with only one latest snapshot',async()=>{
  const f=environment(),a=[makeFile(scoreName,[1])],b=[makeFile(scoreName,[1])],deviceB=new MemoryStorage(),restored=[];
  f.current=listing({revision:1,files:[manifestFile(scoreName,[1])]});
  const noDialog=()=>assert.fail('a fast-forward must not ask for a choice');
  const syncA=f.makeSync({files:a,decide:noDialog,restore:async(_product,save)=>{
    restored.push(save.revision);a[0]=makeFile(scoreName,[8]);
  }});
  const syncB=f.makeSync({files:b,decide:noDialog,store:deviceB});
  assert.equal(await beforeStart(syncA),'continue');
  assert.equal(await beforeStart(syncB),'continue');
  for(let n=2;n<=8;n++){
    assert.equal(await beforeStart(syncB),'continue');
    syncB.started('th06');await until(()=>JSON.parse(deviceB.getItem(journalKey))?.runId);
    b[0]=makeFile(scoreName,[n]);
    syncB.finished();await syncB.flush();
    assert.equal(f.current.saves.length,1);
  }
  assert.equal(await beforeStart(syncA),'reload');
  assert.deepEqual(restored,[8]);assert.equal(journal(f).revision,8);
  assert.equal(journal(f).fingerprint,await api.localSaveFingerprint(a));
  assert.equal(await beforeStart(syncA),'continue');
  assert.equal(sessions(f).length,7);
});

test('simultaneous edits preserve the second browser and report a real divergence',async()=>{
  const f=environment(),a=[makeFile(scoreName,[1])],b=[makeFile(scoreName,[1])],deviceB=new MemoryStorage();
  f.current=listing({revision:1,files:[manifestFile(scoreName,[1])]});
  const seen=[];
  const syncA=f.makeSync({files:a,decide:async(_kind,_product,_signal,comparison)=>{seen.push(comparison.relation);return 'cancel';},
    restore:()=>assert.fail('a fork is never restored automatically')});
  const syncB=f.makeSync({files:b,store:deviceB});
  assert.equal(await beforeStart(syncA),'continue');
  syncA.started('th06');
  await until(()=>f.current.pending.length===1);
  const markerA=f.current.pending[0];
  assert.equal(await beforeStart(syncB),'continue');
  syncB.started('th06');
  await until(()=>f.current.pending.length===2);
  a[0]=makeFile(scoreName,[5]);b[0]=makeFile(scoreName,[9]);
  syncB.finished();await syncB.flush();
  syncA.finished();await syncA.flush();
  assert.equal(syncA.getSnapshot()?.error,'cloudSave.syncConflict');
  assert.equal(await beforeStart(syncA),'continue');
  assert.deepEqual(seen,['diverged']);assert.equal(f.uploaded.length,1);
  assert.equal(f.current.saves[0].files[0].sha256,hash([9]));
  assert.deepEqual(f.current.pending,[markerA],'a rejected upload keeps the unresolved device marker');
  syncA.dispose();
  const reopened=f.makeSync({files:a,decide:async(_kind,_product,_signal,comparison)=>{
    assert.equal(comparison.relation,'diverged');return 'local';
  }});
  assert.equal(await beforeStart(reopened),'continue');
  assert.deepEqual(f.current.pending,[markerA],'choosing the browser schedules upload after play');
  reopened.started('th06');await until(()=>f.storage.getItem('eagler-cloud-sync-v1:lock:th06')!==null);
  reopened.finished();await reopened.flush();
  assert.equal(f.current.saves[0].files[0].sha256,hash([5]));
  assert.equal(f.current.pending.length,0,'an accepted reconciliation ends the old marker');
});

test('missing local bytes load cloud automatically while an unverified legacy baseline asks once',async()=>{
  const f=environment(),local=[];
  f.current=listing({revision:3,files:[manifestFile(scoreName,[3])]});
  const seen=[];
  const sync=f.makeSync({files:local,decide:async(_kind,_product,_signal,comparison)=>{seen.push(comparison.relation);return 'cloud';},
    restore:async()=>{local[0]=makeFile(scoreName,[3]);}});
  assert.equal(await beforeStart(sync),'reload');assert.deepEqual(seen,[]);
  const current=journal(f);
  delete current.verifiedBase;
  f.storage.setItem(journalKey,JSON.stringify(current));
  f.current=listing({revision:4,files:[manifestFile(scoreName,[4])]});
  assert.equal(await beforeStart(sync),'reload');assert.deepEqual(seen,['unknown']);
});

test('a cloud deletion is not silently resurrected from an old browser',async()=>{
  const f=environment(),local=[makeFile(scoreName,[1])];
  f.current=listing({revision:1,files:[manifestFile(scoreName,[1])]});
  let asked=0;
  const sync=f.makeSync({files:local,decide:async(kind)=>{assert.equal(kind,'claim');asked++;return 'cancel';}});
  assert.equal(await beforeStart(sync),'continue');
  f.current=listing({revision:2});
  assert.equal(await beforeStart(sync),'continue');
  sync.started('th06');await Promise.resolve();sync.finished();await sync.flush();
  assert.equal(asked,1);assert.equal(f.uploaded.length,0);
});
