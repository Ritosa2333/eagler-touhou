import {test} from 'node:test';
import assert from 'node:assert/strict';
import {analyzeWindow,decodeRecords,groupFindings,compactReport,mergeIssueGroups,keyOf} from './analyzer.mjs';
import {decodeTimingRing,summarizeDisplayTiming} from './timing.mjs';
import {Th08ObservationAdapter} from './adapter.mjs';
import {LabController} from './controller.mjs';
import {TH08_PRESENTATION_LAB_EXPORTS,TH08_PRESENTATION_LAB_ABI} from './native-abi.mjs';
import {STATE_AUTHORITY,STATE_FIELD_MAP,STATE_GROUPS} from './owners.mjs';
import {TH08_VISUAL_CONTRACT,readNativeWindow,decodeNativeVisualRecord,newbornSubmissionMatches,
  assertPauseRecoveryEvidence,assertVisualMaterialEvidence} from './visual-contract.mjs';
import {assertNativeVisualEvidence} from '../../../../tests/adaptation/adapters/presentation-native-visual-evidence.mjs';
import {assertContinuousVisualEvidence} from '../../../../common/testkit/presentation-lab/continuous-fields.mjs';
import {assertPoolReuseBoundary} from '../../../../common/testkit/presentation-lab/pool-boundary.mjs';
import {compareStateEvidence} from '../../../../common/testkit/presentation-lab/analyzer.mjs';
import {LIFECYCLE_TRACKED_OWNER_IDS} from './owners.mjs';
function record(x=0,{owner=5,scale=1,angle=0,opacity=1,sprite=1,age=10,flags=1<<8,screenX=x,z=0,generation=LIFECYCLE_TRACKED_OWNER_IDS.has(owner)?1:0,identityFlags=LIFECYCLE_TRACKED_OWNER_IDS.has(owner)?3:0,object=100}={}){
  const r={meta:[owner,object,0,0,0,1,1,sprite,age,4,4,flags,generation,identityFlags],values:Array(32).fill(0),vertices:[]};
  r.values[0]=x;r.values[2]=z;r.values[6]=r.values[7]=scale;r.values[10]=angle;r.values[16]=opacity;
  const c=Math.cos(angle),s=Math.sin(angle);
  for(const [px,py] of [[-2,-2],[2,-2],[-2,2],[2,2]])r.vertices.push(screenX+(px*c-py*s)*scale,100+(px*s+py*c)*scale,0,0,0,1,1,1,opacity);
  const xs=r.vertices.filter((_,i)=>i%9===0),ys=r.vertices.filter((_,i)=>i%9===1);
  r.values.splice(21,6,Math.min(...xs),Math.min(...ys),Math.max(...xs),Math.max(...ys),screenX,100);r.values[30]=opacity;r.values[31]=1;return r;
}
const alphas=[0,.25,.5,.75,1,.5,.5,1];
const state=digest=>({coverageVersion:'th08/state/test',groups:STATE_AUTHORITY.domains.flatMap(domain=>domain.groups).filter((id,index,all)=>all.indexOf(id)===index).map(id=>({id,digest:digest??1,includedFields:STATE_FIELD_MAP[id]})),missingGroups:[],authority:STATE_AUTHORITY});

test('interpolation eligibility alone does not prove extra presented frames',()=>{
  const normal=summarizeDisplayTiming({durationMs:16.7,rows:[{flags:{ready:true,tickDue:true,presented:true}}]});
  assert.equal(normal.presented,1);assert.equal(normal.highRefreshPresented,0);assert.equal(normal.betweenTickPresentations,0);
  const callbackOnly=summarizeDisplayTiming({rows:[{flags:{ready:true,highRefresh:true,tickDue:false,presented:false}}]});
  assert.equal(callbackOnly.highRefreshPresented,0);assert.equal(callbackOnly.betweenTickPresentations,0);
});

test('native timing separates submissions between ticks from hardware observation',()=>{
  const result=summarizeDisplayTiming({durationMs:13.9,rows:[
    {flags:{highRefresh:true,tickDue:true,presented:true}},
    {flags:{highRefresh:true,tickDue:false,presented:true}},
  ]});
  assert.equal(result.presented,2);assert.equal(result.highRefreshPresented,2);assert.equal(result.betweenTickPresentations,1);
  assert.equal(result.hardwareRefresh,'requires-human-review');
  assert.equal(summarizeDisplayTiming(null).status,'unavailable');
});
test('timing ring decodes wrapped samples and the normal-speed flag in chronological order',()=>{
  const buffer=new ArrayBuffer(512*128),view=new DataView(buffer),put=(slot,time,frame)=>{const o=slot*128;view.setFloat64(o,time,true);view.setFloat32(o+8,6.1,true);view.setFloat32(o+12,.5,true);view.setUint32(o+16,0x1ff,true);view.setUint32(o+20,frame,true);view.setFloat32(o+24,frame+.25,true);};
  put(511,100,7);put(0,106,8);const ring=decodeTimingRing(buffer,{base:0,capacity:512,count:2,next:1,stride:128});assert.deepEqual(ring.rows.map(r=>r.simulationFrame),[7,8]);assert.equal(ring.durationMs,6);assert.equal(ring.rows[0].camera.previous[0],7.25);assert.equal(ring.rows[0].flags.interpolate,true);assert.equal(ring.rows[0].flags.cameraRebased,true);assert.equal(ring.rows[0].flags.customSpeed,true);
});
test('timing ring preserves native cursors for exact production-callback append checks',()=>{
  const buffer=new ArrayBuffer(512*128),metadata={base:0,capacity:512,count:0,next:0,stride:128};
  const before=decodeTimingRing(buffer,metadata);
  new DataView(buffer).setFloat64(0,16.7,true);
  const after=decodeTimingRing(buffer,{...metadata,count:1,next:1});
  assert.deepEqual([before.base,before.capacity,before.count,before.next,before.stride],[0,512,0,0,128]);
  assert.deepEqual([after.base,after.capacity,after.count,after.next,after.stride],[0,512,1,1,128]);
  assert.equal(after.next,(before.next+1)%before.capacity);
  assert.equal(after.rows.length,1);
});
function lifecycleFor(r){return {available:true,capacity:16,count:1,next:1,dropped:0,events:[{owner:r.meta[0],object:r.meta[1],generation:r.meta[12],tick:1,operation:1}]};}
function analyze(previous,current,maker,extra={}){return analyzeWindow({previous:{tick:1,records:[previous]},current:{tick:2,records:[current]},samples:alphas.map(alpha=>({alpha,records:[maker(alpha)]})),stateBefore:state(),statesAfter:alphas.map(()=>state()),lifecycle:lifecycleFor(current),...extra});}
const find=(report,id)=>report.objects[0].fields.find(f=>f.id===id)?.status;
test('known linear interpolation is measured at final vertices',()=>{const r=analyze(record(10),record(14),a=>record(10+4*a));assert.equal(find(r,'position'),'interpolated');assert.equal(find(r,'screen-position'),'interpolated');assert.equal(r.valid,true);});
test('missing owner interpolation (independent previous endpoint)',()=>{const r=analyze(record(10),record(14),()=>record(14));assert.equal(find(r,'position'),'missing-interpolation');});
test('owner-marked discrete motion boundary is lifecycle',()=>{const r=analyze(record(10),record(14,{flags:1<<21}),()=>record(14,{flags:1<<21}));assert.equal(find(r,'position'),'lifecycle');assert.equal(find(r,'screen-position'),'lifecycle');});
test('2D owner Z-only draw-layer changes stay discrete',()=>{const r=analyze(record(10,{owner:8,z:.25}),record(10,{owner:8,z:.3}),()=>record(10,{owner:8,z:.3}));assert.equal(find(r,'position'),'depth-change');assert.equal(find(r,'screen-position'),'static');});
test('3D background Z-only motion is still observable',()=>{const r=analyze(record(10,{owner:11,z:.25,flags:1<<8}),record(10,{owner:11,z:.3,flags:1<<8}),()=>record(10,{owner:11,z:.3,flags:1<<8}));assert.equal(find(r,'position'),'missing-interpolation');});
test('late/overwritten interpolation endpoint is still detected',()=>{assert.equal(find(analyze(record(10),record(14),()=>record(14)),'screen-position'),'missing-interpolation');});
test('unclassified deliberate discrete attribute is review, not failure',()=>{assert.equal(find(analyze(record(10,{scale:1,flags:0}),record(10,{scale:2,flags:0}),()=>record(10,{scale:2,flags:0})),'scale'),'held-review');});
test('authored scale-growth proves a missing continuous attribute',()=>{const r=analyze(record(10,{flags:1<<9}),record(10,{scale:2,flags:1<<9}),()=>record(10,{scale:2,flags:1<<9}));assert.equal(find(r,'scale'),'missing-interpolation');});
test('subpixel loss at final submission',()=>{const r=analyze(record(10),record(10.2),a=>record(10+.2*a,{screenX:10.2}));assert.equal(find(r,'position'),'interpolated');assert.equal(find(r,'screen-position'),'downstream-held');});
test('authored-current endpoint mismatch',()=>{assert.equal(find(analyze(record(10),record(14),a=>record(10+5*a)),'position'),'endpoint-mismatch');});
test('same-alpha order dependence',()=>{let n=0;const r=analyze(record(10),record(14),a=>record(10+4*a+(a===.5?n++:0)));assert.equal(find(r,'position'),'non-idempotent');});
test('static region is not claimed as high-refresh coverage',()=>{const r=analyze(record(10),record(10),()=>record(10));assert.equal(find(r,'position'),'static');assert.equal(r.coverage['敌弹'].changing,0);});
test('world pause is not a missing-interpolation report',()=>{assert.equal(analyze(record(10),record(14),()=>record(14),{worldFrozen:true}).objects[0].status,'frozen');});
test('pause UI can still be examined while world is frozen',()=>{assert.equal(find(analyze(record(10,{owner:3}),record(14,{owner:3}),()=>record(14,{owner:3}),{worldFrozen:true}),'position'),'missing-interpolation');});
test('sprite change does not excuse position stepping',()=>{const r=analyze(record(10),record(14,{sprite:2}),()=>record(14,{sprite:2}));assert.equal(find(r,'position'),'missing-interpolation');assert.equal(find(r,'screen-shape'),'sprite-change');});
test('object reuse with age reset',()=>{assert.equal(analyze(record(10,{age:99}),record(14,{age:0}),()=>record(14,{age:0})).objects[0].status,'lifecycle');});
test('bomb state is part of diagnostic identity rather than a forced cross-state lerp',()=>{
  const previous=record(10,{owner:16}),current=record(14,{owner:16});previous.meta[2]=1<<8;current.meta[2]=2<<8;
  const r=analyzeWindow({previous:{tick:1,records:[previous]},current:{tick:2,records:[current]},samples:alphas.map(alpha=>({alpha,records:[current]})),stateBefore:state(),statesAfter:alphas.map(()=>state()),lifecycle:lifecycleFor(current)});
  assert.equal(r.objects[0].status,'unobserved');assert.equal(r.objects[0].lifecycleState,2);assert.equal(r.objects[0].part,0);assert.equal(r.disappeared.length,1);
});
test('script/lifecycle switch snaps rather than blends unrelated VMs',()=>{const current=record(14);current.meta[6]=2;assert.equal(analyze(record(10),current,()=>current).objects[0].status,'lifecycle');});
test('unscoped stack addresses cannot become object identities',()=>{assert.equal(analyze(record(10,{owner:0}),record(14,{owner:0}),()=>record(14,{owner:0})).objects[0].status,'unscoped');});
test('absent/cull samples are unobserved rather than passed',()=>{const r=analyze(record(10),record(14),a=>record(10+4*a),{samples:alphas.map(a=>({alpha:a,records:a===0?[]:[record(10+4*a)]}))});assert.equal(r.objects[0].status,'unobserved');});
test('duplicate semantic keys fail closed',()=>{const r=analyze(record(10),record(14),a=>record(10+4*a),{current:{tick:2,records:[record(14),record(14)]}});assert.equal(r.objects[0].status,'ambiguous');});
test('nonconsecutive references invalidate window',()=>{const r=analyze(record(10),record(14),a=>record(10+4*a),{previous:{tick:0,records:[record(10)]}});assert.equal(r.valid,false);assert.equal(r.objects[0].status,'unobserved');});
test('bounded capture overflow never counts as complete',()=>{const r=analyze(record(10),record(14),a=>record(10+4*a),{current:{tick:2,records:[record(14)],dropped:1}});assert.equal(r.valid,false);});
test('logic state mutation is identified by owner group',()=>{const r=analyze(record(10),record(14),a=>record(10+4*a),{statesAfter:[state(2)]});assert.equal(r.purity,false);assert.equal(r.purityStatus,'fail');assert.equal(r.stateChanges[0].group,'determinism-and-game-values');});
test('incomplete state coverage remains unknown rather than purity pass',()=>{const incomplete={...state(),missingGroups:['audio']};const r=analyze(record(10),record(14),a=>record(10+4*a),{stateBefore:incomplete,statesAfter:alphas.map(()=>incomplete)});assert.equal(r.purity,false);assert.equal(r.purityStatus,'unknown');});
test('angle wrap follows shortest arc',()=>{const r=analyze(record(10,{angle:Math.PI-.1}),record(10,{angle:-Math.PI+.1}),a=>record(10,{angle:Math.PI-.1+.2*a}));assert.equal(find(r,'rotation'),'interpolated');});
test('visibility is distinct from final geometry evidence',()=>{assert.equal(analyze(record(1000),record(1004),a=>record(1000+4*a)).objects[0].status,'offscreen');});
test('invalid record buffer does not read WASM memory out of bounds',()=>{assert.throws(()=>decodeRecords(new ArrayBuffer(8),0,1));assert.throws(()=>decodeRecords(new ArrayBuffer(760),0,1,752));});
test('wire layout roundtrip preserves generation, flags and subpixels',()=>{const b=new ArrayBuffer(760);new Uint32Array(b,0,14).set([5,100,1,0,0,3,8,2,9,4,4,256,7,3]);new Float32Array(b,56,32)[0]=10.25;const r=decodeRecords(b,0,1)[0];assert.equal(r.meta[12],7);assert.equal(r.meta[13],3);assert.equal(r.values[0],10.25);assert.equal(r.vertices.length,36);});
test('pooled object generations do not interpolate across pointer reuse',()=>{
  const previous=record(10,{generation:4}),current=record(14,{generation:5}),samples=alphas.map(alpha=>({alpha,records:[current]}));
  const lifecycle={available:true,capacity:16,count:3,next:3,dropped:0,events:[
    {owner:5,object:100,generation:4,tick:1,operation:1},{owner:5,object:100,generation:4,tick:1,operation:2},{owner:5,object:100,generation:5,tick:2,operation:1},
  ]};
  const r=analyzeWindow({previous:{tick:1,records:[previous]},current:{tick:2,records:[current]},samples,stateBefore:state(),statesAfter:samples.map(()=>state()),lifecycle});
  assert.notEqual(previous.meta[12],current.meta[12]);assert.notEqual(keyOf(previous),keyOf(current));
  assert.equal(r.objects[0].status,'unobserved');assert.equal(r.disappeared[0].status,'lifecycle');
});
test('dynamic records without a matching native birth event remain incomplete',()=>{
  const r=analyzeWindow({previous:{tick:1,records:[record(10,{identityFlags:0,generation:0})]},current:{tick:2,records:[record(14,{identityFlags:0,generation:0})]},samples:alphas.map(alpha=>({alpha,records:[record(14,{identityFlags:0,generation:0})]})),lifecycle:{available:true,capacity:16,count:0,next:0,dropped:0,events:[]}});
  assert.equal(r.objects[0].status,'identity-uncertain');assert.equal(r.lifecycle.status,'incomplete');
});
test('lifecycle ring loss and duplicate events cannot prove pooled identity',()=>{
  const current=record(14),previous=record(10),samples=alphas.map(alpha=>({alpha,records:[current]}));
  const overflow={...lifecycleFor(current),dropped:1};
  const lost=analyzeWindow({previous:{tick:1,records:[previous]},current:{tick:2,records:[current]},samples,lifecycle:overflow});
  assert.equal(lost.objects[0].status,'identity-uncertain');assert.equal(lost.valid,false);
  const duplicate={...lifecycleFor(current),count:2,next:2,events:[...lifecycleFor(current).events,{...lifecycleFor(current).events[0],operation:3}]};
  const bad=analyzeWindow({previous:{tick:1,records:[previous]},current:{tick:2,records:[current]},samples,lifecycle:duplicate});
  assert.equal(bad.objects[0].status,'identity-uncertain');assert.equal(bad.lifecycle.status,'incomplete');
});
test('native adapter decodes all state groups and the bounded lifecycle ring',()=>{
  const memory={buffer:new ArrayBuffer(4096)},base=64,ring=512,groups=17;
  new Uint32Array(memory.buffer,base,groups).set(Array.from({length:groups},(_,i)=>i+1));
  const events=[{owner:5,object:100,generation:1,tick:2,operation:1},{owner:5,object:101,generation:3,tick:4,operation:1},{owner:5,object:100,generation:1,tick:5,operation:2}];
  const words=new Uint32Array(memory.buffer,ring,4*5);for(let i=0;i<events.length;i++){const e=events[i],at=(3+i)%4*5;words.set([e.owner,e.object,e.generation,e.tick,e.operation],at);}
  let fault=0;
  const core={memory,audit_state:()=>base,audit_state_words:()=>groups,audit_lifecycle_records:()=>ring,audit_lifecycle_capacity:()=>4,audit_lifecycle_count:()=>3,audit_lifecycle_next:()=>2,audit_lifecycle_dropped:()=>0,audit_fault:value=>{fault=value;}};
  const adapter=new Th08ObservationAdapter({core}),stateEvidence=adapter.readStateEvidence(),lifecycle=adapter.readLifecycleEvidence();
  assert.equal(stateEvidence.groups.length,17);assert.equal(stateEvidence.groups[13].id,'synchronous-audio-operations');assert.equal(stateEvidence.missingGroups.length,0);assert.ok(stateEvidence.authority.domains.length>=10);
  assert.equal(compareStateEvidence(stateEvidence,[stateEvidence]).status,'pass');
  assert.ok(stateEvidence.groups[0].includedFields.some(field=>field.includes('records[0..221]')));
  assert.ok(stateEvidence.groups[11].includedFields.some(field=>field.includes('ReplayRecording')));
  assert.ok(stateEvidence.groups[15].includedFields.some(field=>field.includes('TouchController::')));
  assert.deepEqual(lifecycle.events,events);adapter.setNegativeControl(true);assert.equal(fault,3);
});
test('Lab facade routes production callback cadence through the declared native ABI',()=>{
  const runtimeWith=(result=1)=>{
    const calls=[],core={memory:{buffer:new ArrayBuffer(4096)}};
    for(const name of TH08_PRESENTATION_LAB_EXPORTS)core[name]=()=>0;
    core.sdl_presentation_audit_frame_at_v1=timestamp=>{calls.push(timestamp);return result;};
    return {runtime:{core,app:{},Module:{},status:()=>[],stop:async()=>{}},calls};
  };
  const {runtime,calls}=runtimeWith(1),controller=new LabController(runtime,{runtimeEpoch:9});
  assert.equal(controller.capabilities.features.productionFrameCallbackCadence,true);
  assert.ok(TH08_PRESENTATION_LAB_EXPORTS.includes('sdl_presentation_audit_frame_at_v1'));
  assert.throws(()=>controller.frameAt(0),/frozen Runtime/);
  controller.freeze();
  assert.equal(controller.frameAt(0),true);
  assert.equal(controller.frameAt(1000/60),true);
  assert.deepEqual(calls,[0,1000/60]);
  assert.throws(()=>controller.frameAt(Number.NaN),/finite and nonnegative/);
  assert.equal(calls.length,2);

  const missing=runtimeWith();
  delete missing.runtime.core.sdl_presentation_audit_frame_at_v1;
  assert.throws(()=>new LabController(missing.runtime,{runtimeEpoch:10}),/missing sdl_presentation_audit_frame_at_v1/);
  const rejected=runtimeWith(0),rejectedController=new LabController(rejected.runtime,{runtimeEpoch:11});
  rejectedController.freeze();
  assert.equal(rejectedController.frameAt(10),false);
});
test('gate-disabled scene remains explicitly gate-disabled',()=>{assert.equal(analyze(record(10),record(14),a=>record(10+4*a),{gate:false}).gate,false);});
test('non-finite values are not counted as static success',()=>{assert.equal(find(analyze(record(10),record(14),a=>record(a===.5?NaN:10+4*a)),'position'),'nonfinite');});

test('mixed logical ticks invalidate a frozen sweep',()=>{const r=analyze(record(10),record(14),a=>record(10+4*a),{samples:alphas.map((a,i)=>({tick:i===3?3:2,alpha:a,records:[record(10+4*a)]}))});assert.equal(r.valid,false);});
test('misaligned or non-integer WASM pointers are rejected',()=>{for(const p of [1,2.5,NaN,Infinity])assert.throws(()=>decodeRecords(new ArrayBuffer(4096),p,1));});
test('3D CPU projection does not pretend to observe GPU UV/fog/opacity',()=>{const r=analyze(record(10,{flags:8}),record(14,{flags:8}),a=>record(10+4*a,{flags:8}));assert.equal(find(r,'submitted-uv'),'unobserved');assert.equal(find(r,'submitted-opacity'),'unobserved');});
test('3D near-plane divide is projection-clip rather than a render NaN',()=>{
  const clipped=()=>{const r=record(10,{owner:11,flags:8|(1<<22)});r.values.fill(NaN,21,27);for(let i=0;i<r.vertices.length;i+=9){r.vertices[i]=NaN;r.vertices[i+1]=NaN;}return r;};
  const r=analyze(record(10,{owner:11,flags:8}),clipped(),clipped);
  assert.equal(find(r,'screen-position'),'projection-clip');assert.equal(find(r,'screen-shape'),'projection-clip');assert.notEqual(r.objects[0].severity,6);
});
test('disappeared objects remain explicit lifecycle evidence',()=>{const r=analyzeWindow({previous:{tick:1,records:[record(10)]},current:{tick:2,records:[]},samples:alphas.map(alpha=>({alpha,records:[]}))});assert.equal(r.disappeared.length,1);assert.equal(r.objects.length,0);});
test('particle instances and time windows are grouped without losing counts',()=>{const a=analyze(record(10),record(14),()=>record(14));const b=structuredClone(a);b.tick=3;b.objects[0].key='another-instance';const g=groupFindings([a,b]).find(g=>g.field==='position');assert.equal(g.instanceCount,2);assert.equal(g.windowCount,2);assert.equal(g.observations,2);});
test('retained history has an explicit detail and image bound',()=>{const a=analyze(record(10),record(14),()=>record(14));a.images=[{png:'example'}];const r=compactReport(a,{maxObjects:0,images:false});assert.equal(r.totalObjects,1);assert.equal(r.objects.length,0);assert.equal(r.detailsTruncated,true);assert.equal(r.images,undefined);});
test('scan groups preserve full counts after object-detail truncation',()=>{const a=analyze(record(10),record(14),()=>record(14));const retained=compactReport(a,{maxObjects:0});const g=mergeIssueGroups([retained,retained]).find(g=>g.field==='position');assert.equal(g.windowCount,2);assert.equal(g.observations,2);assert.equal(g.peakInstances,1);assert.equal(g.instanceCount,undefined);});

test('recorded screen shake does not falsely count as object translation',()=>{
  const shaken=(shake,x=10)=>{const r=record(x,{screenX:x+shake,flags:(1<<16)|(1<<8)});r.values[27]=shake;r.values[28]=0;return r;};
  const r=analyze(shaken(1),shaken(-2),()=>shaken(-2));assert.equal(find(r,'screen-position'),'static');
  const missing=analyze(shaken(1,10),shaken(-2,14),()=>shaken(-2,14));assert.equal(find(missing,'screen-position'),'missing-interpolation');
});
test('custom ring raw ANM radius is not treated as sprite position',()=>{
  const raw=(x,screenX)=>{const r=record(x,{owner:7,screenX,flags:1<<8});r.meta[4]=1;return r;};
  const r=analyze(raw(10,100),raw(20,104),a=>raw(20,100+4*a));
  assert.equal(find(r,'position'),'upstream-parameter');assert.equal(find(r,'screen-position'),'interpolated');
});
test('actual consumed ring radius still detects missing interpolation',()=>{
  const ring=scale=>{const r=record(100,{owner:7,scale,flags:1<<9});r.meta[4]=5;return r;};
  const missing=analyze(ring(1),ring(2),()=>ring(2));assert.equal(find(missing,'scale'),'missing-interpolation');
  assert.equal(find(analyze(ring(1),ring(2),a=>ring(1+a)),'scale'),'interpolated');
});
test('inactive secondary color is not promoted to a visible fade',()=>{
  const sample=a=>{const r=record(100,{flags:(1<<19)|(1<<15)});r.values[20]=a;return r;};
  assert.equal(find(analyze(sample(.2),sample(.8),()=>sample(.8)),'opacity2'),'inactive-channel');
  const selected=a=>{const r=sample(a);r.meta[11]|=1<<18;return r;};
  assert.equal(find(analyze(selected(.2),selected(.8),()=>selected(.8)),'opacity2'),'missing-interpolation');
});

test('one visible alpha step is response, not a held downstream value',()=>{
  const p=record(100,{opacity:10/255,flags:1<<13}),c=record(100,{opacity:14/255,flags:1<<13});
  const r=analyze(p,c,a=>record(100,{opacity:(a<.5?13:14)/255,flags:1<<13}));
  assert.equal(find(r,'opacity'),'responsive');assert.equal(find(r,'submitted-opacity'),'responsive');
});
test('small measured geometry response is not reported as no interpolation',()=>{
  const r=analyze(record(10),record(10.06),a=>record(10.04+.02*a));
  assert.ok(['responsive','interpolated'].includes(find(r,'screen-position')));
});

test('direct loadedSprite changes are discrete even if activeSpriteIndex is stale',()=>{
  const p=record(10),c=record(14);p.spriteIdentity=101;c.spriteIdentity=102;
  const r=analyze(p,c,()=>c);assert.equal(find(r,'submitted-uv'),'sprite-change');
  assert.equal(find(r,'position'),'missing-interpolation');
});
test('actual sprite pointer decodes as an opaque integer, not a float',()=>{
  const b=new ArrayBuffer(760);new Uint32Array(b,0,14)[11]=1<<20;new Uint32Array(b,56+29*4,1)[0]=123456789;
  assert.equal(decodeRecords(b,0,1)[0].spriteIdentity,123456789);
});

function nativeState(){return {coverageVersion:'th08/state/3',groups:STATE_GROUPS.map(id=>({id,digest:17,includedFields:STATE_FIELD_MAP[id]})),missingGroups:[],authority:STATE_AUTHORITY};}
function nativeWords(value){
  const buffer=new ArrayBuffer(760);
  new Uint32Array(buffer,0,14).set(value.meta);new Float32Array(buffer,56,32).set(value.values);
  new Float32Array(buffer,184,144).set(value.vertices);
  if(value.meta[11]&(1<<20))new Uint32Array(buffer,56+29*4,1)[0]=value.spriteIdentity;
  return Array.from(new Uint32Array(buffer));
}
function wireFrame(records,tick,count,alpha){
  const words=records.flatMap(nativeWords),decoded=[];
  for(let index=0;index<words.length;index+=190)decoded.push(decodeNativeVisualRecord(words.slice(index,index+190)));
  return {tick,dropped:0,...(alpha===undefined?{}:{alpha}),records:decoded,
    native:{schema:'th08/presentation-record-buffer/1',stride:760,count:decoded.length,runtimeEpoch:4,words,
      lifecycle:{count,next:count,dropped:0,epoch:1}}};
}
function rebuildNativeReport(report){
  const reads=report.readVerification.receipts.map(receipt=>receipt.reads),frames=report.nativeFrames;
  return {...report,...analyzeWindow({...frames,stateBefore:reads[0][0],statesAfter:[reads[0][1],...reads.slice(1).flat()],
    worldFrozen:report.worldFrozen,gate:report.gate,build:report.build,label:report.label,negativeControl:report.negativeControl,
    lifecycle:report.nativeLifecycle})};
}
function nativeWindow({reuse=false,foreign=false}={}){
  const born={owner:5,object:100,generation:1,tick:1,operation:1};
  const retired={...born,tick:2,operation:2},next={...born,tick:2,generation:2};
  const events=reuse?[born,retired,...(foreign?[{...born,owner:9,object:100}]:[]),next]:[born],count=events.length;
  const current=record(14,{generation:reuse?2:1}),frames={schema:'th08/presentation-native-frames/1',
    previous:wireFrame([record(10)],1,1),current:wireFrame([current],2,count),
    samples:alphas.map(alpha=>wireFrame([reuse?current:record(10+4*alpha)],2,count,alpha))};
  const report={schema:'th08/presentation-audit/1',label:reuse?'display05-pool-reuse-boundary':'stage1-motion',
    build:{game:'th08',nativeAbi:TH08_PRESENTATION_LAB_ABI,runtimeEpoch:4},worldFrozen:false,gate:true,negativeControl:false,
    nativeFrames:frames,nativeLifecycle:{available:true,capacity:16384,count,next:count,dropped:0,epoch:1,runtimeEpoch:4,events,
      native:{schema:'th08/presentation-lifecycle-buffer/1',stride:20,words:events.flatMap(event=>[event.owner,event.object,event.generation,event.tick,event.operation])}},
    readVerification:{schema:'eagler-presentation/sweep-reads/1',receipts:[null,...alphas].map(alpha=>({phase:alpha===null?'before':'after-draw',alpha,reads:[nativeState(),nativeState()]}))}};
  return {report:rebuildNativeReport(report),event:{retired,born:next}};
}

test('native visual evidence redecodes 190-word frames and all 18 actual authority reads',()=>{
  const {report}=nativeWindow(),window=assertNativeVisualEvidence(report,TH08_VISUAL_CONTRACT);
  assert.equal(window.samples.length,8);assert.equal(window.statesAfter.length,17);
  assert.equal(window.recomputed.purityStatus,'pass');
  assert.equal(assertContinuousVisualEvidence(report,TH08_VISUAL_CONTRACT).status,'passed');
  assert.doesNotThrow(()=>readNativeWindow(JSON.parse(JSON.stringify(compactReport(report)))));
});
test('dropping native words or changing decoded/derived observations cannot pass the raw gate',()=>{
  for(const mutation of [
    report=>{delete report.nativeFrames;},
    report=>{delete report.nativeLifecycle.native;},
    report=>{report.nativeFrames.current.records[0].values[0]+=1;},
    report=>{report.objects[0].fields.find(field=>field.id==='position').current[0]+=1;},
    report=>{report.nativeFrames.samples[0].native.words.pop();},
  ]){const {report}=nativeWindow();mutation(report);assert.throws(()=>readNativeWindow(report));}
});
test('raw float bits retain negative zero and NaN-shaped opaque identities across JSON serialization',()=>{
  const {report}=nativeWindow();
  for(const frame of [report.nativeFrames.previous,report.nativeFrames.current,...report.nativeFrames.samples]){
    const value=frame.records[0];value.values[2]=-0;value.meta[11]|=1<<20;value.spriteIdentity=0x7fc01234;
    const changed=wireFrame([value],frame.tick,frame.native.lifecycle.count,frame.alpha);Object.assign(frame,changed);
    assert.equal(frame.native.words[16],0x80000000);assert.equal(frame.native.words[43],0x7fc01234);
  }
  const rebuilt=rebuildNativeReport(report),serialized=JSON.parse(JSON.stringify(rebuilt));
  assert.equal(serialized.nativeFrames.current.records[0].values[29],null);
  assert.doesNotThrow(()=>readNativeWindow(serialized));
  assert.equal(serialized.nativeFrames.current.native.words[43],0x7fc01234);
});
test('a changed second read, unknown authority name, stale cursor, or other Runtime epoch is rejected',()=>{
  for(const mutation of [
    report=>{report.readVerification.receipts[4].reads[1].groups[0].digest++;},
    report=>{report.readVerification.receipts[0].reads[0].groups[0].id='unregistered-domain';},
    report=>{report.readVerification.receipts.pop();},
    report=>{report.nativeFrames.samples[2].native.lifecycle.count=0;report.nativeFrames.samples[2].native.lifecycle.next=0;},
    report=>{report.nativeLifecycle.dropped=1;},
    report=>{report.nativeFrames.current.native.runtimeEpoch=5;},
  ]){const {report}=nativeWindow();mutation(report);assert.throws(()=>readNativeWindow(report));}
});
test('a missing alpha object cannot turn an observed continuous path into unobserved success',()=>{
  const {report}=nativeWindow();report.nativeFrames.samples[3]=wireFrame([],2,1,.75);
  const rebuilt=rebuildNativeReport(report);
  assert.equal(rebuilt.objects[0].status,'unobserved');assert.doesNotThrow(()=>readNativeWindow(rebuilt));
  assert.throws(()=>assertContinuousVisualEvidence(rebuilt,TH08_VISUAL_CONTRACT),/missing an alpha submission/);
});
test('pool reuse binds one adjacent interval, full disappearance and every newborn raw submission',()=>{
  const capture=nativeWindow({reuse:true}),result=assertPoolReuseBoundary(capture,TH08_VISUAL_CONTRACT);
  assert.equal(result.oldSubmissions,1);assert.equal(result.newSubmissions,1);assert.equal(result.samples,8);
  assert.equal(capture.report.disappeared[0].object,100);
  const incomplete=structuredClone(capture);delete incomplete.report.disappeared[0].object;
  assert.throws(()=>assertPoolReuseBoundary(incomplete,TH08_VISUAL_CONTRACT),/derived disappeared/);
  const stale=structuredClone(capture);stale.report.nativeFrames.previous.native.lifecycle={...stale.report.nativeFrames.current.native.lifecycle};
  assert.throws(()=>assertPoolReuseBoundary(stale,TH08_VISUAL_CONTRACT),/not alive|adjacent reference interval/);
});
test('newborn scale/rotation/UV/color/vertices cannot borrow an unobserved lifecycle verdict',()=>{
  for(const change of [value=>{value.values[6]+=1;},value=>{value.values[10]+=.2;},value=>{value.values[11]+=.2;},
    value=>{value.values[16]-=.2;},value=>{value.vertices[0]+=1;}]){
    const capture=nativeWindow({reuse:true}),frame=capture.report.nativeFrames.samples[2],value=frame.records[0];change(value);
    capture.report.nativeFrames.samples[2]=wireFrame([value],frame.tick,frame.native.lifecycle.count,frame.alpha);
    capture.report=rebuildNativeReport(capture.report);
    assert.doesNotThrow(()=>readNativeWindow(capture.report));
    assert.throws(()=>assertPoolReuseBoundary(capture,TH08_VISUAL_CONTRACT),/newborn submission/);
  }
});
test('pool owner namespace distinguishes another owner with the same numeric object address',()=>{
  const capture=nativeWindow({reuse:true,foreign:true});
  assert.equal(assertPoolReuseBoundary(capture,TH08_VISUAL_CONTRACT).status,'passed');
  capture.event.born={...capture.event.born,owner:9};
  assert.throws(()=>assertPoolReuseBoundary(capture,TH08_VISUAL_CONTRACT),/same registered physical owner/);
});

function pauseWindows({recoveryWait=1}={}){
  const scene=(world,control,paused)=>({nativeScene:[2,1,0,0,0,world,control,0,0,0,0,0,0,paused?1:0,0,0]});
  const player=x=>record(x,{owner:13,object:200}),menu=()=>record(20,{owner:3,object:300});
  const window=(label,tick,frozen,previous,current,sampled)=>{
    const {report}=nativeWindow();report.label=label;report.worldFrozen=frozen;
    report.nativeFrames={schema:'th08/presentation-native-frames/1',previous:wireFrame(previous,tick-1,1),
      current:wireFrame(current,tick,1),samples:alphas.map(alpha=>wireFrame(sampled(alpha),tick,1,alpha))};
    return rebuildNativeReport(report);
  };
  const pause=window('pause-menu',13,true,[player(10),menu()],[player(10),menu()],()=>[player(10),menu()]);
  pause.pauseProgress={schema:'th08/pause-progress/1',runtimeEpoch:4,waited:1,steps:12,complete:true,
    before:{tick:1,worldFrozen:true,scene:scene(20,100,true),reference:wireFrame([player(10),menu()],1,1)},
    after:{tick:13,worldFrozen:true,scene:scene(20,112,true),reference:pause.nativeFrames.current},
    samples:Array.from({length:12},(_,index)=>({step:index+1,tick:index+2,worldFrozen:true,nativeScene:scene(20,101+index,true).nativeScene}))};
  const movementTick=16+recoveryWait,resumed=window('pause-resumed',movementTick+5,false,[player(14)],[player(18)],alpha=>[player(14+4*alpha)]);
  resumed.pauseRecovery={schema:'th08/pause-recovery/1',runtimeEpoch:4,
    before:structuredClone(pause.pauseProgress.after),
    movementBefore:{tick:movementTick,worldFrozen:false,scene:scene(21,115+recoveryWait,false),reference:wireFrame([player(10)],movementTick,1)},
    after:{tick:movementTick+5,worldFrozen:false,scene:scene(26,120+recoveryWait,false),reference:resumed.nativeFrames.current},
    samples:Array.from({length:recoveryWait},(_,index)=>({step:index+1,tick:17+index,worldFrozen:index+1<recoveryWait,
      nativeScene:scene(index+1<recoveryWait?20:21,116+index,index+1<recoveryWait).nativeScene})),
    input:[{code:'Escape',down:true,ticks:3},{code:'Escape',down:false},{code:'ArrowRight',down:true,ticks:4},{code:'ArrowRight',down:false}],
    keysReleased:true,playerMoved:true,complete:true};
  return {pause,resumed};
}

test('pause/recovery binds native world and control clocks, released input and real player submissions',()=>{
  const {pause,resumed}=pauseWindows();
  assert.equal(assertContinuousVisualEvidence(pause,TH08_VISUAL_CONTRACT).status,'passed');
  assert.equal(assertContinuousVisualEvidence(resumed,TH08_VISUAL_CONTRACT).status,'passed');
  const result=assertPauseRecoveryEvidence(pause,resumed);
  assert.equal(result.pausedSteps,12);assert.equal(result.worldFramesBefore,20);assert.equal(result.worldFramesAfter,26);
  assert.equal(result.machineComplete,true);assert.equal(result.complete,false);assert.equal(result.humanReview,'pending');
});

test('pause metadata cannot hide one tick, stopped control clock, absent steps, held input or unmoved player',()=>{
  for(const change of [
    ({pause})=>{pause.pauseProgress.steps=1;},
    ({pause})=>{pause.pauseProgress.after.scene.nativeScene[6]=100;},
    ({pause})=>{pause.pauseProgress.after.scene.nativeScene[5]++;},
    ({pause})=>{pause.pauseProgress.samples[4].nativeScene[5]++;},
    ({pause})=>{pause.pauseProgress.samples.pop();},
    ({resumed})=>{resumed.pauseRecovery.samples=[];},
    ({resumed})=>{resumed.pauseRecovery.samples[0].tick++;},
    ({resumed})=>{resumed.pauseRecovery.keysReleased=false;},
    ({resumed})=>{resumed.pauseRecovery.input.pop();},
    ({resumed})=>{resumed.pauseRecovery.movementBefore.reference=wireFrame([record(18,{owner:13,object:200})],17,1);},
    ({resumed})=>{resumed.pauseRecovery.before.scene.nativeScene[6]--;},
    ({resumed})=>{resumed.pauseRecovery.runtimeEpoch=5;},
  ]){const windows=pauseWindows();change(windows);assert.throws(()=>assertPauseRecoveryEvidence(windows.pause,windows.resumed));}
});

test('a still-paused recovery step cannot advance the native world before thawing',()=>{
  const {pause,resumed}=pauseWindows({recoveryWait:2});
  assert.equal(assertPauseRecoveryEvidence(pause,resumed).status,'passed');
  resumed.pauseRecovery.samples[0].nativeScene[5]++;
  assert.throws(()=>assertPauseRecoveryEvidence(pause,resumed),/pause flag was still set/);
});

const pixelPNG='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR4nGP4z8DwHwAFAAH/iZk9HQAAAABJRU5ErkJggg==';
function visualMaterial(){
  const {report}=nativeWindow(),material=structuredClone(report);
  material.label=`${report.label}/visual-window`;
  material.images=material.nativeFrames.samples.slice(0,5).map(sample=>({alpha:sample.alpha,png:pixelPNG,
    boxes:sample.records.map(record=>({key:keyOf(record),bounds:record.values.slice(21,25)}))}));
  return {report,material:rebuildNativeReport(material)};
}

test('PNG material uses all five actual native alpha records and producer-scoped pending review',()=>{
  const {report,material}=visualMaterial();
  const result=assertVisualMaterialEvidence(compactReport(material),{sourceReport:report});
  assert.equal(result.images,5);assert.equal(result.humanReview,'pending');assert.equal(result.physicalRefreshVerified,false);
});

test('material rejects missing raw, forged boxes, wrong alpha/source and an absent PNG',()=>{
  for(const change of [
    material=>{delete material.nativeFrames;},
    material=>{delete material.nativeLifecycle.native;},
    material=>{material.images[1].boxes[0].bounds[0]++;},
    material=>{material.images[1].boxes[0].key='5:unrelated';},
    material=>{material.images[1].alpha=.75;},
    material=>{material.images[1].png=null;},
    material=>{material.images.pop();},
    material=>{material.label='unrelated/visual-window';},
    material=>{material.nativeFrames.current.native.runtimeEpoch=8;},
  ]){const {report,material}=visualMaterial();change(material);assert.throws(()=>assertVisualMaterialEvidence(material,{sourceReport:report}));}
});
