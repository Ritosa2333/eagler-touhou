import {test} from 'node:test';
import assert from 'node:assert/strict';
import {analyzeWindow,decodeRecords,groupFindings,compactReport,mergeIssueGroups} from './analyzer.mjs';
import {decodeTimingRing} from './timing.mjs';
function record(x=0,{owner=5,scale=1,angle=0,opacity=1,sprite=1,age=10,flags=1<<8,screenX=x,z=0}={}){
  const r={meta:[owner,100,0,0,0,1,1,sprite,age,4,4,flags],values:Array(32).fill(0),vertices:[]};
  r.values[0]=x;r.values[2]=z;r.values[6]=r.values[7]=scale;r.values[10]=angle;r.values[16]=opacity;
  const c=Math.cos(angle),s=Math.sin(angle);
  for(const [px,py] of [[-2,-2],[2,-2],[-2,2],[2,2]])r.vertices.push(screenX+(px*c-py*s)*scale,100+(px*s+py*c)*scale,0,0,0,1,1,1,opacity);
  const xs=r.vertices.filter((_,i)=>i%9===0),ys=r.vertices.filter((_,i)=>i%9===1);
  r.values.splice(21,6,Math.min(...xs),Math.min(...ys),Math.max(...xs),Math.max(...ys),screenX,100);r.values[30]=opacity;r.values[31]=1;return r;
}
const alphas=[0,.25,.5,.75,1,.5,.5,1];
const state=digest=>({coverageVersion:'th08/state/test',groups:[{id:'RNG/资源数值',digest:digest??1}],missingGroups:[]});
test('timing ring decodes wrapped samples in chronological order',()=>{
  const buffer=new ArrayBuffer(512*128),view=new DataView(buffer),put=(slot,time,frame)=>{const o=slot*128;view.setFloat64(o,time,true);view.setFloat32(o+8,6.1,true);view.setFloat32(o+12,.5,true);view.setUint32(o+16,0xff,true);view.setUint32(o+20,frame,true);view.setFloat32(o+24,frame+.25,true);};
  put(511,100,7);put(0,106,8);const ring=decodeTimingRing(buffer,{base:0,capacity:512,count:2,next:1,stride:128});assert.deepEqual(ring.rows.map(r=>r.simulationFrame),[7,8]);assert.equal(ring.durationMs,6);assert.equal(ring.rows[0].camera.previous[0],7.25);assert.equal(ring.rows[0].flags.interpolate,true);assert.equal(ring.rows[0].flags.cameraRebased,true);
});
function analyze(previous,current,maker,extra={}){return analyzeWindow({previous:{tick:1,records:[previous]},current:{tick:2,records:[current]},samples:alphas.map(alpha=>({alpha,records:[maker(alpha)]})),stateBefore:state(),statesAfter:alphas.map(()=>state()),...extra});}
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
  const r=analyzeWindow({previous:{tick:1,records:[previous]},current:{tick:2,records:[current]},samples:alphas.map(alpha=>({alpha,records:[current]})),stateBefore:state(),statesAfter:alphas.map(()=>state())});
  assert.equal(r.objects[0].status,'unobserved');assert.equal(r.objects[0].lifecycleState,2);assert.equal(r.objects[0].part,0);assert.equal(r.disappeared.length,1);
});
test('script/lifecycle switch snaps rather than blends unrelated VMs',()=>{const current=record(14);current.meta[6]=2;assert.equal(analyze(record(10),current,()=>current).objects[0].status,'lifecycle');});
test('unscoped stack addresses cannot become object identities',()=>{assert.equal(analyze(record(10,{owner:0}),record(14,{owner:0}),()=>record(14,{owner:0})).objects[0].status,'unscoped');});
test('absent/cull samples are unobserved rather than passed',()=>{const r=analyze(record(10),record(14),a=>record(10+4*a),{samples:alphas.map(a=>({alpha:a,records:a===0?[]:[record(10+4*a)]}))});assert.equal(r.objects[0].status,'unobserved');});
test('duplicate semantic keys fail closed',()=>{const r=analyze(record(10),record(14),a=>record(10+4*a),{current:{tick:2,records:[record(14),record(14)]}});assert.equal(r.objects[0].status,'ambiguous');});
test('nonconsecutive references invalidate window',()=>{const r=analyze(record(10),record(14),a=>record(10+4*a),{previous:{tick:0,records:[record(10)]}});assert.equal(r.valid,false);assert.equal(r.objects[0].status,'unobserved');});
test('bounded capture overflow never counts as complete',()=>{const r=analyze(record(10),record(14),a=>record(10+4*a),{current:{tick:2,records:[record(14)],dropped:1}});assert.equal(r.valid,false);});
test('logic state mutation is identified by owner group',()=>{const r=analyze(record(10),record(14),a=>record(10+4*a),{statesAfter:[state(2)]});assert.equal(r.purity,false);assert.equal(r.purityStatus,'fail');assert.equal(r.stateChanges[0].group,'RNG/资源数值');});
test('incomplete state coverage remains unknown rather than purity pass',()=>{const incomplete={...state(),missingGroups:['audio']};const r=analyze(record(10),record(14),a=>record(10+4*a),{stateBefore:incomplete,statesAfter:alphas.map(()=>incomplete)});assert.equal(r.purity,false);assert.equal(r.purityStatus,'unknown');});
test('angle wrap follows shortest arc',()=>{const r=analyze(record(10,{angle:Math.PI-.1}),record(10,{angle:-Math.PI+.1}),a=>record(10,{angle:Math.PI-.1+.2*a}));assert.equal(find(r,'rotation'),'interpolated');});
test('visibility is distinct from final geometry evidence',()=>{assert.equal(analyze(record(1000),record(1004),a=>record(1000+4*a)).objects[0].status,'offscreen');});
test('invalid record buffer does not read WASM memory out of bounds',()=>{assert.throws(()=>decodeRecords(new ArrayBuffer(8),0,1));assert.throws(()=>decodeRecords(new ArrayBuffer(752),0,1,750));});
test('wire layout roundtrip preserves metadata and subpixels',()=>{const b=new ArrayBuffer(752);new Uint32Array(b,0,12).set([5,100,1,0,0,3,8,2,9,4,4,256]);new Float32Array(b,48,32)[0]=10.25;const r=decodeRecords(b,0,1)[0];assert.equal(r.values[0],10.25);assert.equal(r.vertices.length,36);});
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
  const b=new ArrayBuffer(752);new Uint32Array(b,0,12)[11]=1<<20;new Uint32Array(b,48+29*4,1)[0]=123456789;
  assert.equal(decodeRecords(b,0,1)[0].spriteIdentity,123456789);
});
