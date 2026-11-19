import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'../..');
const {QualificationOwner,evaluateOwnedQualification}=await import(pathToFileURL(root+'/site/runtime/ex-qualification.mjs'));
const {OwnedLastNameFieldStore:RealNameStore,originalNameProfilePacket}=await import(pathToFileURL(root+'/site/runtime/last-name-field.mjs'));
const {nativeRouteScorePacket}=await import(pathToFileURL(root+'/site/runtime/sound.mjs'));
const source=fs.readFileSync(root+'/site/play.mjs','utf8'),start=source.indexOf('async function observeOriginalExtraQualification('),end=source.indexOf('async function prepareOriginalMain(){',start);
assert(start>=0&&end>start);const body=source.slice(start,end);
const sha=async b=>Buffer.from(await crypto.subtle.digest('SHA-256',b)).toString('hex');
const blanks=Uint8Array.from([32,32,32,32,32,32,32,32,0]);
async function profile(clears=0){
 const profile=new Uint8Array(52232),v=new DataView(profile.buffer);
 for(let i=0;i<8;i++){v.setUint32(i*1644,0x00035243,true);v.setUint32(i*1644+8,1644,true);}
 for(const [p,tag,n]of [[13152,0x00025453,1068],[14220,0x00014443,38012]]){v.setUint32(p,tag,true);v.setUint32(p+8,n,true);}
 profile.set(blanks,13164);v.setUint32(16,1234560,true);
 for(let i=0;i<clears;i++)v.setUint32(i*1644+0x658,1,true);
 return {schema:'th128-owned-native-title-record/1',revision:1,provenance:'ordinary-native-postrun',tainted:false,profile,sha256:await sha(profile),native:{route:0,rank:0},runId:'test-record-from-failed-run'};
}
function harness(record,{stored,readFailure,beforeRead}={}){
 const module={},controller=new AbortController(),state={module,action:8,stageSelection:null,controller},events={reads:0,writes:0,closed:0};
 const backend={async read(){events.reads++;if(beforeRead)await beforeRead();if(readFailure)throw readFailure;return stored;},async compareAndSwap(){events.writes++;assert.fail('startup must not write a name');},close(){events.closed++;}};
 class TitleStore{async observe(){return record;}}
 class NameStore extends RealNameStore{constructor(){super({backend});}}
 const read=new Function('state','QualificationOwner','OwnedNativeTitleRecordStore','OwnedLastNameFieldStore','evaluateOwnedQualification','originalNameProfilePacket',body+';return observeOriginalExtraQualification;')(state,QualificationOwner,TitleStore,NameStore,evaluateOwnedQualification,originalNameProfilePacket);
 return {state,events,run:()=>read(module,controller.signal)};
}
test('a saved failed-run score without a saved replay name still reaches the normal Main readback',async()=>{
 const r=await profile(),h=harness(r),out=await h.run();
 assert.equal(out.record,r);assert.equal(out.qualification,null);assert.equal(out.name.absent,true);
 assert.equal(new DataView(nativeRouteScorePacket(out.record.profile).buffer).getUint32(8,true),1234560);
 assert.deepEqual(h.events,{reads:1,writes:0,closed:1});
});
test('missing name does not discard six saved route counters or fabricate EX eligibility',async()=>{
 const r=await profile(6),h=harness(r),out=await h.run();assert.equal(out.record,r);assert.equal(out.qualification,null);
 for(let i=0;i<6;i++)assert.equal(new DataView(out.record.profile.buffer).getUint32(i*1644+0x658,true),1);
 assert.equal(h.events.writes,0);
});
test('first startup without any record does not need a name database',async()=>{
 const h=harness(null),out=await h.run();assert.equal(out.record,null);assert.equal(out.qualification,null);assert.equal(h.events.reads,0);
});
test('a real matching independent name and six route counters still derives EX eligibility',async()=>{
 const r=await profile(6),stored={kind:'th128-original-stats-last-name-field9-v1',revision:1,bytes:blanks.slice(),digest:await sha(blanks)};
 const h=harness(r,{stored}),out=await h.run();assert.equal(out.qualification.eligible,true);assert.equal(out.qualification.mask,63);assert.equal(h.events.writes,0);
});
test('record checksum failure is never hidden behind name absence',async()=>{
 const r=await profile();r.profile[20]^=1;await assert.rejects(harness(r).run(),e=>e.code==='record-digest');
});
test('invalid route counters are still rejected before name availability is handled',async()=>{
 const r=await profile();new DataView(r.profile.buffer).setUint32(0x658,100000,true);r.sha256=await sha(r.profile);
 await assert.rejects(harness(r).run(),e=>e.code==='clear-count-cap');
});
test('a different valid replay name preserves the original score profile for Main without granting EX',async()=>{
 const r=await profile(6),bytes=blanks.slice();bytes[0]=65;
 const stored={kind:'th128-original-stats-last-name-field9-v1',revision:1,bytes,digest:await sha(bytes)};
 const original=r.profile.slice(),h=harness(r,{stored}),out=await h.run();
 assert.equal(out.record,r);assert.deepEqual(out.record.profile,original);
 assert.deepEqual(out.name.bytes,bytes);assert.equal(out.qualification,null);
 assert.equal(new DataView(nativeRouteScorePacket(out.record.profile).buffer).getUint32(8,true),1234560);
 assert.deepEqual(h.events,{reads:1,writes:0,closed:1});
 await assert.rejects(evaluateOwnedQualification({record:r,name:out.name}),e=>e.code==='name-mismatch');
});
test('a bad replay name digest and damaged embedded field are never accepted as a valid mismatch',async()=>{
 const r=await profile(),bytes=blanks.slice();bytes[0]=65;
 const stored={kind:'th128-original-stats-last-name-field9-v1',revision:1,bytes,digest:await sha(blanks)};
 await assert.rejects(harness(r,{stored}).run(),/complete field digest differs/);
 r.profile[13172]=1;r.sha256=await sha(r.profile);stored.digest=await sha(bytes);
 await assert.rejects(harness(r,{stored}).run(),e=>e.code==='name-field-bytes');
});
test('storage read errors propagate and close the temporary name owner',async()=>{
 const h=harness(await profile(),{readFailure:new Error('read unavailable')});
 await assert.rejects(h.run(),/read unavailable/);assert.equal(h.events.closed,1);
});
test('an action change while the name read awaits rejects its obsolete qualification',async()=>{
 let entered,release;const enteredPromise=new Promise(r=>entered=r),held=new Promise(r=>release=r);
 const h=harness(await profile(),{beforeRead:async()=>{entered();await held;}});
 const pending=h.run();await enteredPromise;h.state.action++;release();await assert.rejects(pending,e=>e.name==='AbortError');assert.equal(h.events.closed,1);
});
test('malformed stored name records are not treated as an absent database field',async()=>{
 await assert.rejects(harness(await profile(),{stored:{kind:'bad',revision:1}}).run(),/identity/);
});
