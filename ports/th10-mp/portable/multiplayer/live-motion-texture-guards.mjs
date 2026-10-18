// One-shot, test-only LMT1 boundary probes. This module never draws, schedules
// a game callback, queries GL, or writes game/renderer state. Only the optional
// texture observer is toggled, after both real packets have been copied.
// Call afterCallback() INSIDE the capture's existing post-native microtask.
// finish() must run after the caller has disabled the main observer.
const FRAME_WORDS=352,RECORD_WORDS=80,HEADER_WORDS=32,STATUS_WORDS=16,RENDERER_WORDS=17;
const MAGIC=0x4c4d5431,MAX_SIDE=128,MAX_PAYLOAD=MAX_SIDE*MAX_SIDE*4;
const MAX_TEXTURE_CALLS=48,MAX_STATE_READS=48,MAX_SUBMITS=4096,MAX_ERRORS=16;
const EMPTY=new Uint8Array(0);
const RESULT={ok:1,disabled:2,noPending:3,mismatch:4,unsupportedRecord:5,noStamp:18};
const CHECKS=[
 ['default-main-off','beforeEnable was not called before observer enable'],
 ['invalid-enable-value','beforeEnable did not complete'],
 ['disabled-zero-request','beforeEnable did not complete'],
 ['default-state-unchanged','beforeEnable did not complete'],
 ['pending-boundary','No real default blit with two already-copied OK player packets'],
 ['bad-observer','The pending boundary probes did not complete'],
 ['bad-identity','The pending boundary probes did not complete'],
 ['bad-callback','The pending boundary probes did not complete'],
 ['bad-draw','The pending boundary probes did not complete'],
 ['bad-slot','The pending boundary probes did not complete'],
 ['bad-surface','The pending boundary probes did not complete'],
 ['bad-owner','The pending boundary probes did not complete'],
 ['null-app','The pending boundary probes did not complete'],
 ['correct-packet-seat-0','The pending boundary probes did not complete'],
 ['correct-packet-seat-1','The pending boundary probes did not complete'],
 ['request-state-unchanged','The pending boundary probes did not complete'],
 ['toggle-invalidates-current-stamps','The pending boundary probes did not complete'],
 ['toggle-state-unchanged','The pending boundary probes did not complete'],
 ['next-draw-rebuilds-stamps','No subsequent observed Draw with two OK player packets'],
 ['callback-closed','No post-native callback microtask after the successful pending probes'],
 ['old-pending-rejected','The callback-close probe did not complete'],
 ['callback-probe-state-unchanged','The callback-close probe did not complete'],
 ['main-disabled','finish was not called after main observer disable'],
 ['disabled-old-tuple','No saved real pending tuple, or finish did not complete'],
 ['finish-state-unchanged','The final disabled probe did not complete'],
];

export function createLiveMotionTextureGuards({core,app}){
 const checks=CHECKS.map(([id,reason])=>({id,status:'not-run',reason}));
 const byId=new Map(checks.map(check=>[check.id,check]));
 const errors=[],counts={textureGetters:0,stateSnapshots:0,submits:0};
 let beforeAttempted=false,submitAttempted=false,callbackAttempted=false;
 let halted=false,finished=false,report=null,tuples=null,probeDraw=null;

 function assert(value,message){if(!value)throw Error(message);}
 function fail(where,error){
  const message=String(error?.message??error).slice(0,768);
  const check=byId.get(where);
  if(check){check.status='failed';check.reason=message;delete check.details;}
  if(errors.length<MAX_ERRORS)errors.push({where,message});
  halted=true;
  throw Error('LMT1 guards ['+where+']: '+message);
 }
 function run(id,fn){
  try{
   const details=fn(),check=byId.get(id);
   check.status='passed';check.reason=null;
   if(details!==undefined)check.details=details;
   return details;
  }catch(error){return fail(id,error);}
 }
 function guarded(where,fn){
  if(finished||halted)return;
  try{return fn();}catch(error){if(!halted)return fail(where,error);throw error;}
 }
 function ensureApi(){
  assert(core&&core.memory&&core.memory.buffer,'Missing WASM memory');
  assert(Number.isInteger(app)&&app>0&&app<=0xffffffff,'Expected the real nonzero WASM32 app pointer');
  for(const key of ['multiplayer_motion_observation','multiplayer_motion_observation_status',
                   'multiplayer_motion_texture_capture','multiplayer_motion_texture_set_enabled','sdl_stats'])
   assert(typeof core[key]==='function','Missing export '+key);
 }
 function address(raw,bytes,buffer,label,allowNull=false){
  assert(Number.isInteger(raw)&&raw>=-0x80000000&&raw<=0xffffffff,label+' pointer is not WASM32');
  const pointer=raw>>>0;
  if(!pointer&&allowNull)return 0;
  assert(pointer>0&&pointer%4===0&&pointer+bytes<=buffer.byteLength,label+' pointer is out of bounds');
  return pointer;
 }
 function wordsFrom(raw,count,label,allowNull=false){
  // No cached HEAP view survives a native call.
  const buffer=core.memory.buffer,pointer=address(raw,count*4,buffer,label,allowNull);
  return pointer?new Uint32Array(new Uint32Array(buffer,pointer,count)):null;
 }
 function state(){
  assert(++counts.stateSnapshots<=MAX_STATE_READS,'State getter budget exceeded');
  const pending=wordsFrom(core.multiplayer_motion_observation(app,1),FRAME_WORDS,'pending',true);
  const status=wordsFrom(core.multiplayer_motion_observation_status(),STATUS_WORDS,'observer status');
  const renderer=wordsFrom(core.sdl_stats(),RENDERER_WORDS,'renderer statistics');
  assert(status[0]===1,'Unexpected observer status schema');
  if(pending)assert(pending[0]===1&&pending[1]===FRAME_WORDS&&pending[2]===RECORD_WORDS&&pending[3]===4,
                    'Unexpected pending frame schema');
  return {pending,status,renderer};
 }
 function equal(a,b,label){
  assert((a===null)===(b===null),label+' presence changed');
  if(a===null)return;
  assert(a.length===b.length,label+' length changed');
  for(let i=0;i<a.length;++i)assert(a[i]===b[i],label+' changed at index '+i);
 }
 function unchanged(before){
  const after=state();
  equal(before.pending,after.pending,'pending352');
  equal(before.status,after.status,'observerStatus16');
  equal(before.renderer,after.renderer,'sdl_stats17');
 }
 function validateHeader(header){
  assert(header.length===HEADER_WORDS&&header[0]===MAGIC&&header[1]===1&&header[2]===HEADER_WORDS,
         'Unexpected LMT1 header schema');
  assert(header[24]===HEADER_WORDS*4&&header[25]===MAX_SIDE,'Unexpected LMT1 packet layout');
  assert(header[3]>=RESULT.ok&&header[3]<=RESULT.noStamp,'Unknown LMT1 result '+header[3]);
  const bytes=header[23];
  assert(bytes<=MAX_PAYLOAD,'LMT1 payload exceeds fixed capacity');
  if(header[3]!==RESULT.ok){
   assert(header[22]===0&&bytes===0,'Failed LMT1 request exposed pitch or payload');
   return 0;
  }
  const width=header[20],height=header[21];
  assert(width>0&&width<=MAX_SIDE&&height>0&&height<=MAX_SIDE,'Invalid LMT1 crop size');
  assert(header[22]===width*4&&bytes===header[22]*height,'Invalid LMT1 RGBA extent');
  assert(header[18]+width<=header[15]&&header[19]+height<=header[16],'LMT1 crop is outside source');
  return bytes;
 }
 function readTexture(tuple,requestApp=app){
  assert(++counts.textureGetters<=MAX_TEXTURE_CALLS,'Texture getter budget exceeded');
  const raw=core.multiplayer_motion_texture_capture(requestApp,tuple.observer,tuple.identity,
   tuple.callback,tuple.draw,tuple.slot,tuple.surface,tuple.owner);
  const buffer=core.memory.buffer,pointer=address(raw,HEADER_WORDS*4,buffer,'LMT1');
  const header=new Uint32Array(new Uint32Array(buffer,pointer,HEADER_WORDS));
  const bytes=validateHeader(header);
  // Failures never form a payload view, even if old bytes remain in native storage.
  if(header[3]!==RESULT.ok)return {header,rgba:EMPTY};
  assert(pointer+HEADER_WORDS*4+bytes<=buffer.byteLength,'LMT1 payload pointer is out of bounds');
  const rgba=new Uint8Array(new Uint8Array(buffer,pointer+HEADER_WORDS*4,bytes));
  return {header,rgba};
 }
 function echo(packet,tuple){
  const h=packet.header;
  assert(h[4]===tuple.observer&&h[5]===tuple.identity&&h[6]===tuple.callback&&h[7]===tuple.draw&&
         h[8]===tuple.slot&&h[9]===tuple.owner&&h[10]===tuple.surface,'LMT1 tuple echo changed');
 }
 function rejection(tuple,expected,requestApp=app){
  const packet=readTexture(tuple,requestApp);echo(packet,tuple);
  const allowed=Array.isArray(expected)?expected:[expected];
  assert(allowed.includes(packet.header[3]),'Expected result '+allowed.join('/')+', got '+packet.header[3]);
  return packet.header[3];
 }
 function tupleFor(words,slot){
  const base=HEADER_WORDS+slot*RECORD_WORDS;
  return Object.freeze({observer:words[4],identity:words[5],callback:words[6],draw:words[7],
                        slot,surface:words[base+19],owner:words[base+20]});
 }
 function inputPair(rois){
  assert(Array.isArray(rois)&&rois.length<=3,'Expected at most three copied ROIs');
  const pair=[null,null];
  for(const roi of rois){
   if(roi.slot!==0&&roi.slot!==1)continue;
   assert(!pair[roi.slot],'Duplicate player ROI');
   pair[roi.slot]=roi.texture??null;
  }
  return pair;
 }
 function okPair(pair){return pair.every(packet=>packet?.header?.[3]===RESULT.ok);}
 function savedPacket(packet,tuple){
  assert(packet.header instanceof Uint32Array&&packet.rgba instanceof Uint8Array,
         'Expected already-copied native header and RGBA arrays');
  assert(packet.header.buffer!==core.memory.buffer&&packet.rgba.buffer!==core.memory.buffer,
         'Caller passed a live native packet view instead of an immediate copy');
  const bytes=validateHeader(packet.header);
  assert(packet.header[3]===RESULT.ok&&packet.rgba.length===bytes,'Copied OK packet has invalid payload');
  const saved={header:new Uint32Array(packet.header),rgba:new Uint8Array(packet.rgba)};
  echo(saved,tuple);return saved;
 }
 function matchesOriginal(tuple,original){
  const actual=readTexture(tuple);echo(actual,tuple);
  assert(actual.header[3]===RESULT.ok,'Correct request did not recover after rejection');
  equal(original.header,actual.header,'original LMT1 header');
  equal(original.rgba,actual.rgba,'original LMT1 payload');
 }
 function defaultBlit(sample){
  const w=sample?.words,a=sample?.blit,size=sample?.bufferSize;
  return w instanceof Uint32Array&&w.length===FRAME_WORDS&&w[9]===2&&
   (w[10]&31)===31&&!(w[10]&(64|128|256|512|1024))&&size?.[0]===640&&size?.[1]===480&&
   a?.length===10&&a[0]===0&&a[1]===0&&a[2]===w[25]&&a[3]===w[26]&&
   a[4]===0&&a[5]===480&&a[6]===640&&a[7]===0&&a[8]===0x4000&&a[9]===0x2600&&
   w[25]>0&&w[26]>0;
 }
 function beforeEnable(){return guarded('beforeEnable',()=>{
  if(beforeAttempted)return;
  beforeAttempted=true;let before;
  run('default-main-off',()=>{ensureApi();before=state();assert(before.status[1]===0,'Main observer was already enabled');});
  run('invalid-enable-value',()=>{assert(core.multiplayer_motion_texture_set_enabled(app,2)===0,'Invalid enable value 2 was accepted');});
  run('disabled-zero-request',()=>({result:rejection({observer:0,identity:0,callback:0,draw:0,slot:0,surface:0,owner:0},RESULT.disabled)}));
  run('default-state-unchanged',()=>unchanged(before));
 });}
 function afterSubmit(sample,rois){return guarded('afterSubmit',()=>{
  if(submitAttempted&&byId.get('next-draw-rebuilds-stamps').status==='passed')return;
  assert(++counts.submits<=MAX_SUBMITS,'Submit observation budget exceeded');
  if(!defaultBlit(sample))return;
  const pair=inputPair(rois);
  if(submitAttempted){
   const w=sample.words;
   if(w[4]!==tuples[0].observer||w[5]!==tuples[0].identity||w[7]===tuples[0].draw)return;
   for(const packet of pair)if(packet?.header?.[3]===RESULT.noStamp)
    run('next-draw-rebuilds-stamps',()=>{throw Error('A new observed Draw still has NoSubmitStamp');});
   if(!okPair(pair))return;
   run('next-draw-rebuilds-stamps',()=>{
    const before=state();equal(sample.words,before.pending,'next pending352');
    for(let slot=0;slot<2;++slot){
     const tuple=tupleFor(w,slot),original=savedPacket(pair[slot],tuple);
     assert((original.header[30]&1)===1,'New Draw packet is not submit-stamped');
     matchesOriginal(tuple,original);
    }
    unchanged(before);
    return {callback:w[6],draw:w[7]};
   });
   return;
  }
  if(!okPair(pair))return;
  submitAttempted=true;let before,originals;
  run('pending-boundary',()=>{
   ensureApi();
   assert(byId.get('default-state-unchanged').status==='passed','beforeEnable did not complete');
   before=state();equal(sample.words,before.pending,'original pending352');
   assert(before.status[1]===1&&before.status[15]===1,'Not inside the live native callback');
   const w=sample.words;
   assert(before.status[2]===w[4]&&before.status[3]===w[5]&&before.status[4]===w[6],
          'Pending tuple does not match current observer status');
   tuples=[tupleFor(w,0),tupleFor(w,1)];
   originals=pair.map((packet,slot)=>savedPacket(packet,tuples[slot]));
   probeDraw={sample:Number.isInteger(sample.index)?sample.index:null,observer:w[4],identity:w[5],
              callback:w[6],draw:w[7],logicalFrame:w[17]};
   return probeDraw;
  });
  const base=tuples[0],flip=value=>(value^1)>>>0;
  const bad=[
   ['bad-observer',{...base,observer:flip(base.observer)},RESULT.mismatch],
   ['bad-identity',{...base,identity:flip(base.identity)},RESULT.mismatch],
   ['bad-callback',{...base,callback:flip(base.callback)},RESULT.mismatch],
   ['bad-draw',{...base,draw:flip(base.draw)},RESULT.mismatch],
   ['bad-slot',{...base,slot:2},RESULT.unsupportedRecord],
   ['bad-surface',{...base,surface:flip(base.surface)},RESULT.mismatch],
   ['bad-owner',{...base,owner:flip(base.owner)},RESULT.mismatch],
  ];
  for(const [id,tuple,expected] of bad)run(id,()=>{
   const result=rejection(tuple,expected);unchanged(before);return {result};
  });
  run('null-app',()=>{const result=rejection(base,RESULT.disabled,0);unchanged(before);return {result};});
  for(let slot=0;slot<2;++slot)run('correct-packet-seat-'+slot,()=>{
   matchesOriginal(tuples[slot],originals[slot]);unchanged(before);
  });
  run('request-state-unchanged',()=>unchanged(before));
  run('toggle-invalidates-current-stamps',()=>{
   assert(core.multiplayer_motion_texture_set_enabled(app,0)===0,'Texture disable returned an unexpected value');
   assert(core.multiplayer_motion_texture_set_enabled(app,1)===1,'Texture re-enable was rejected');
   return {results:tuples.map(tuple=>rejection(tuple,RESULT.noStamp))};
  });
  run('toggle-state-unchanged',()=>unchanged(before));
 });}
 function afterCallback(){return guarded('afterCallback',()=>{
  if(!tuples||callbackAttempted)return;
  callbackAttempted=true;let before;
  run('callback-closed',()=>{before=state();assert(before.status[15]===0,'Call afterCallback from the existing post-native microtask');});
  run('old-pending-rejected',()=>({results:tuples.map(tuple=>rejection(tuple,[RESULT.noPending,RESULT.disabled]))}));
  run('callback-probe-state-unchanged',()=>unchanged(before));
 });}
 function finish(){
  if(report)return report;
  // finish reports failures instead of throwing, including earlier probe errors.
  try{
   let before;
   run('main-disabled',()=>{ensureApi();before=state();assert(before.status[1]===0,'Main observer must be disabled before guards.finish');});
   if(tuples){
    run('disabled-old-tuple',()=>({results:tuples.map(tuple=>rejection(tuple,RESULT.disabled))}));
    run('finish-state-unchanged',()=>unchanged(before));
   }
  }catch{}
  finished=true;
  report={schema:'th10-live-motion-texture-guards/1',
   defaultOff:byId.get('default-main-off').status==='passed',
   scope:'One-shot native LMT1 boundary probes; no Draw, GL query, resource submission, or canonical-state write',
   probeDraw,counts:{...counts},
   checks:checks.map(check=>({...check})),errors:errors.map(error=>({...error})),
   passed:errors.length===0&&checks.every(check=>check.status==='passed')};
  return report;
 }
 return {beforeEnable,afterSubmit,afterCallback,finish};
}
