// ROOT runs: node --test portable/multiplayer/check-live-motion-capture-selection.mjs
// Bounded diagnostic-selection tests only. No browser/native/GL or pixel oracle
// is simulated as a gameplay PASS; the existing analyzer is the structural
// reference and every real capture still needs its original live gates.
import test,{after} from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {analyzeLiveMotion} from './live-motion-analysis.mjs';
import {createLiveMotionCapture,createLiveMotionPixelSchedule,
 inspectLiveMotionPendingPixelEligibility,liveMotionPixelSelectionPolicy} from './live-motion-capture.mjs';

const INVALID=0xffffffff,H=32,R=80,AUX=H+3*R;
const sources=['check-live-motion-capture-selection.mjs','live-motion-capture.mjs',
 'live-motion-analysis.mjs','live-motion-texture-guards.mjs'];
const inventory=()=>Object.fromEntries(sources.map(path=>[path,
 createHash('sha256').update(readFileSync(new URL(path,import.meta.url))).digest('hex')]));
const before=inventory();
after(()=>assert.deepEqual(inventory(),before,'Source bytes changed while this suite was running'));

// Independent literal native ABI fixture; no selector-generated identity,
// flags, endpoints or quad. Pixels are intentionally absent.
function sample({frame=180,index=0,alpha=.25}={}){
 const words=new Uint32Array(352),f=new Float32Array(words.buffer);
 words.set([1,352,80,4,1,1,200+index,100+index,0,2,31,2,
  0x12345678,1,0,0,2,frame,frame-2,0,0,INVALID,2,1000,7,640,480,3]);
 f[28]=alpha;f[29]=alpha;words[30]=3;
 for(let slot=0;slot<3;++slot){
  const b=H+slot*R,x=100+slot*130,y=110+slot*80,cx=x+alpha*4;
  words.set([slot<2?1:2,slot,slot<2?slot:42,slot<2?slot:777,10+slot,
   slot<2?slot:INVALID,1,63|(slot===0?64:0),0,1,17+slot,91,1,2,3,3,
   0xffffffff,0xffffffff,frame,8,9,7,0,0,640,480,frame,0x1000+slot*0x100],b);
  f.set([x-192.5,480-y-16.5,0,x+4-192.5,480-y-16.5,0],b+28);
  f.set([cx-.5,480-y-.5,0],b+34);f[b+46]=f[b+47]=1;
  [[-10,-10,0,0],[10,-10,1,0],[-10,10,0,1],[10,10,1,1]].forEach(([dx,dy,u,v],i)=>{
   const q=b+50+7*i;f.set([cx-.5+dx,480-y-.5+dy,.4,1],q);
   words[q+4]=0xffffffff;f[q+5]=u;f[q+6]=v;
  });
  words[b+78]=1;
 }
 return {index,atMs:index*1000/120,words,presentedWords:null,
  blit:[0,0,640,480,0,480,640,0,0x4000,0x2600],bufferSize:[640,480],
  bindingMatched:true,readbackStatus:'metadata-only',pixels:[]};
}
function commit(s){s.presentedWords=s.words.slice();s.presentedWords[8]=300+s.index;
 s.presentedWords[9]=3;s.presentedWords[10]=s.words[10]|32;return s;}
function analyze(samples){return analyzeLiveMotion({samples,errors:[],dropped:0,
 observerStatus:[1,0,1,1,400,400,samples.length,samples.length,0,0,0,42,777,1,400,0]});}
function parity(s,label){
 commit(s);const selected=inspectLiveMotionPendingPixelEligibility(s),row=analyze([s]).samples[0];
 assert.equal(selected.frameReason,row.reason,label+' frame');
 if(row.status==='eligible')assert.deepEqual(selected.objects.map(({slot,status,reason})=>({slot,status,reason})),
  row.objects.map(({slot,status,reason})=>({slot,status,reason})),label+' objects');
 return selected;
}
function stationary(s,except=-1){for(let slot=0;slot<3;++slot)if(slot!==except){
 const b=H+slot*R;s.words.set(s.words.subarray(b+31,b+34),b+28);}return s;}
function tintAlpha(s,slot,byte,vertex=null){
 for(let i=0;i<4;++i)if(vertex===null||vertex===i)
  s.words[H+slot*R+54+i*7]=((byte<<24)|0xffffff)>>>0;
 return s;
}
function localSeat(s,seat,count=2){
 s.words[15]=seat;s.words[16]=count;
 for(let slot=0;slot<2;++slot)s.words[H+slot*R+7]=
  (s.words[H+slot*R+7]&~64)|(slot===seat?64:0);
 return s;
}
function frontier(s,slot=1){
 const w=s.words,b=H+slot*R,at=AUX+8+slot*16;
 const previous=w.slice(b+28,b+31),current=w.slice(b+31,b+34);
 if(w[AUX]!==0x52465031)w.fill(0,AUX);
 w.set([0x52465031,1,w[AUX+2]|(1<<slot),16,2,0,0,0],AUX);
 w.set([9,w[17]-1,w[17],10,11,1,0,1],at);w.set(previous,at+8);w.set(current,at+11);
 w.set(current,b+28);w[b+7]|=256;return s;
}

test('pending structural qualification matches the unchanged analyzer header/world rules',()=>{
 const mutations=[
  ['baseline',()=>{}],['schema',s=>s.words[0]++],['frame size',s=>s.words[1]--],
  ['record size',s=>s.words[2]++],['capacity',s=>s.words[3]--],
  ['phase',s=>s.words[9]=3],['already committed',s=>s.words[8]=1],
  ['committed flag',s=>s.words[10]|=32],['required flag',s=>s.words[10]&=~4],
  ...[64,128,256,512,1024,2048,4096,8192].map(bit=>['frame bit '+bit,s=>s.words[10]|=bit]),
  ...[4,16,32,64].map(bit=>['world bit '+bit,s=>s.words[20]|=bit]),
  ...[4,5,6,7,24].map(word=>['zero identity '+word,s=>s.words[word]=0]),
  ['invalid frame',s=>s.words[17]=INVALID],['wrong player count',s=>s.words[16]=1],
  ['wrong seat',s=>s.words[15]=2],['empty session',s=>{s.words[12]=s.words[13]=0;}],
  ['wrong owner count',s=>s.words[27]--],['wrong submit count',s=>s.words[30]--],
  ['wrong rejection count',s=>s.words[31]++],['reserved owner',s=>s.words[AUX+7]=1],
  ['recovery pending',s=>s.words[21]=s.words[17]],
  ['prediction limit',s=>s.words[22]=12],['below limit',s=>s.words[22]=11],
  ['wrong buffer',s=>s.bufferSize[0]=639],['wrong blit',s=>s.blit[9]=0x2601],
  ['zero source dimension',s=>s.words[25]=0],['bad index',s=>s.index=-1],
  ['bad timestamp',s=>s.atMs=NaN],
  ...[28,29].flatMap(word=>[NaN,Infinity,-.01,1.01].map(value=>['alpha '+word+'/'+value,
   s=>{new Float32Array(s.words.buffer)[word]=value;}]))
 ];
 for(const [label,mutate] of mutations){const s=sample();mutate(s);parity(s,label);}
});

test('object qualification covers both players and the native bullet slot',()=>{
 for(let slot=0;slot<3;++slot){const b=H+slot*R,q=b+50;
  const mutations=[
   ['owner not seen',w=>{w[b+7]&=~1;--w[27];--w[30];}],
   ['quad not submitted',w=>{w[b+7]&=~2;--w[30];}],
   ['native rejection',w=>{w[b+8]=1;++w[31];}],
   ['wrong kind',w=>w[b]=0],['wrong slot',w=>w[b+1]=3],
   ['wrong object index',w=>w[b+2]=2000],['zero incarnation',w=>w[b+4]=0],
   ['nonzero part',w=>w[b+79]=1],['new incarnation',w=>w[b+7]|=128],
   ['authored draw',w=>w[11]=1],
   ...[4,8,16,32].map(bit=>['required record bit '+bit,w=>w[b+7]&=~bit]),
   ['multiple quads',w=>w[b+9]=2],['state discontinuity',w=>w[b+78]=2],
   ['wrong target frame',w=>--w[b+26]],['missing texture',w=>w[b+19]=0],
   ['wrong surface',w=>++w[b+21]],['zero viewport width',w=>w[b+24]=0],
   ['oversized viewport',w=>++w[b+22]],
   ['nonfinite previous',(_,f)=>f[b+28]=NaN],['nonfinite current',(_,f)=>f[b+31]=Infinity],
   ['nonfinite quad',(_,f)=>f[q+2]=NaN],['nonfinite uv',(_,f)=>f[q+5]=Infinity],
   ['invalid reciprocal w',(_,f)=>f[q+3]=0],['viewport clip',(_,f)=>f[q]=-.75],
   ['transparent quad',w=>{for(let i=0;i<4;++i)w[q+7*i+4]=0x00ffffff;}],
   ['tiny quad',(_,f)=>{for(let i=0;i<4;++i){f[q+7*i]=100+(i%2)*.25;f[q+7*i+1]=100+(i>>1)*.25;}}]
  ];
  if(slot<2)mutations.push(['wrong seat',w=>w[b+5]=2],['wrong object id',w=>w[b+3]=5]);
  else mutations.push(['inactive bullet',w=>w[b+6]=0]);
  for(const [label,mutate] of mutations){const s=sample();mutate(s.words,new Float32Array(s.words.buffer));
   parity(s,'slot '+slot+' '+label);}
 }
});

test('RFP1 held logical targets retain moving display pairs; valid snaps are not sampled',()=>{
 const s=frontier(stationary(sample(),1)),b=H+R,at=AUX+24;
 assert.deepEqual(s.words.slice(b+28,b+31),s.words.slice(b+31,b+34));
 const result=parity(s,'remote frontier');assert.equal(result.eligible,true);
 assert.deepEqual(result.objects.filter(o=>o.moving).map(o=>o.slot),[1]);
 assert.equal(result.objects[1].endpointSource,'completed-display-frontier');
 // RFP1 state is authoritative for display continuity, not raw logical history.
 s.words[b+78]=0;assert.equal(parity(s,'frontier state').eligible,true);
 for(const flags of [3,7]){s.words[at+7]=flags;s.words.set(s.words.subarray(at+11,at+14),at+8);
  const snap=parity(s,'snap '+flags);assert.equal(snap.eligible,false);
  assert.equal(snap.reason,'stationary-display-endpoints');
  assert.equal(createLiveMotionPixelSchedule().select(s).attempted,false);}
});

test('native arena byte offsets and three-seat display-frontier masks are preserved',()=>{
 const s=sample();s.words[16]=3;s.words[15]=2;s.words[H+7]&=~64;
 frontier(frontier(s,0),1);
 const arena=new Uint32Array(400);arena.set(s.words,16);s.words=arena.subarray(16,16+352);
 const bytesBefore=arena.slice(),result=parity(s,'offset arena with two remote seats');
 assert.equal(result.eligible,true);
 assert.deepEqual(result.objects.slice(0,2).map(o=>o.endpointSource),
  ['completed-display-frontier','completed-display-frontier']);
 assert.deepEqual(arena,bytesBefore);
});

test('malformed RFP1 cannot silently select raw logical endpoints',()=>{
 const at=AUX+24;
 const mutations=[w=>w[AUX]++,w=>w[AUX+1]++,w=>w[AUX+2]=1,w=>w[AUX+3]++,
  w=>w[AUX+4]++,w=>w[AUX+7]=1,w=>w[AUX+50]=1,w=>w[AUX+8]=1,
  w=>w[at]=0,w=>w[at+1]=INVALID,w=>--w[at+2],w=>w[at+3]=12,
  w=>w[at+5]=3,w=>w[at+7]=2,w=>w[at+7]=3,w=>w[at+11]^=1,
  w=>w[at+14]=1,w=>w[at+8]=0x7fc00000,w=>w[H+2*R+7]|=256,w=>w[15]=1,
  w=>w[H+R+7]&=~256];
 for(let i=0;i<mutations.length;++i){const s=frontier(sample());mutations[i](s.words);
  const selected=parity(s,'malformed frontier '+i);assert.equal(selected.eligible,false);
  assert.equal(selected.reason,'invalid-completed-frontier-extension');}
});

test('qualification is exact nonzero XY motion, not an alpha or RGB success filter',()=>{
 for(const alpha of [0,.25,1]){
  assert.equal(parity(sample({alpha}),'valid alpha '+alpha).eligible,true);
  const remote=frontier(stationary(sample({alpha}),1));
  assert.equal(createLiveMotionPixelSchedule().select(remote).budgetClass,'remote',
   'The reserve uses existing alpha eligibility, including a real endpoint sample');
 }
 const s=stationary(sample());assert.equal(parity(s,'stationary').eligible,false);
 // Signed zero does not imply screen motion; Z-only displacement also cannot.
 const f=new Float32Array(s.words.buffer);f[H+28]=-0;f[H+31]=0;f[H+30]=1;f[H+33]=2;
 assert.equal(parity(s,'signed zero and z').eligible,false);
 f[H+31]=.00001;assert.equal(parity(s,'nonzero subpixel').eligible,true);
 const tiny=frontier(stationary(sample(),1)),tinyFloats=new Float32Array(tiny.words.buffer);
 tinyFloats[AUX+24+8]=tinyFloats[AUX+24+11]+.00001;
 assert.equal(createLiveMotionPixelSchedule().select(tiny).budgetClass,'remote',
  'Nonzero remote displacement has no new magnitude threshold');
 const unseen=sample();
 for(const name of ['pixels','presentedWords','bindingMatched','readbackStatus','correlation','coverage'])
  Object.defineProperty(unseen,name,{get(){throw Error('Selector read future/pixel field '+name);}});
 assert.equal(createLiveMotionPixelSchedule().select(unseen).attempted,true);
});

test('pending eligibility does not bypass actual presented binding in final analysis',()=>{
 const s=commit(sample());s.bindingMatched=false;
 assert.equal(inspectLiveMotionPendingPixelEligibility(s).eligible,true);
 assert.equal(analyze([s]).samples[0].reason,'default-binding-unmatched');
 s.bindingMatched=true;s.presentedWords[H+31]^=1;
 const result=analyze([s]);assert.equal(result.captureComplete,false);
 assert.equal(result.samples[0].reason,'unmatched-commit-content');
});

test('fixed sections include old endpoints, preserve 8/32/96 caps, and never reset on rewind',()=>{
 const schedule=createLiveMotionPixelSchedule();
 assert.deepEqual(schedule.windows.map(w=>w.sections.map(s=>[s.first,s.last])),[
  [[180,199],[200,219],[220,239],[240,260]],
  [[700,719],[720,739],[740,759],[760,780]],
  [[1100,1119],[1120,1139],[1140,1159],[1160,1180]]]);
 let index=0,attempts=0;
 // Revisit all twelve sections in reverse order repeatedly; no forward-only
 // assumption, epoch reset or re-entering an exhausted bucket is permitted.
 for(let pass=0;pass<12;++pass)for(let wi=2;wi>=0;--wi)for(let si=3;si>=0;--si){
  const section=schedule.windows[wi].sections[si],frame=pass%2?section.first:section.last;
  const selected=schedule.select(sample({frame,index:index++}));
  assert.equal(selected.window,wi);assert.equal(selected.section,si);
  assert.equal(selected.attempted,pass<8);
  if(selected.attempted){++attempts;
   assert.equal(selected.budgetClass,pass<4?'remote':'general');}
  assert.equal(section.attempted,Math.min(pass+1,8));
  assert.equal(section.remoteAttempted,Math.min(pass+1,4));
  assert.equal(section.generalAttempted,Math.min(Math.max(pass-3,0),4));
 }
 assert.equal(attempts,96);assert.deepEqual(schedule.windows.map(w=>w.attempted),[32,32,32]);
 for(const frame of [0,179,261,699,781,1099,1181,INVALID])
  assert.equal(schedule.select(sample({frame})).reason,'outside-fixed-window');
 assert.equal(liveMotionPixelSelectionPolicy.maximumAttempts,96);
 assert.equal(liveMotionPixelSelectionPolicy.generalAttemptsPerSection,4);
 assert.equal(liveMotionPixelSelectionPolicy.remoteAttemptsPerSection,4);
 assert.equal(liveMotionPixelSelectionPolicy.failedAttemptsReplenished,false);
 assert.equal(liveMotionPixelSelectionPolicy.rewindResetsBudget,false);
});

test('early local-only attempts cannot starve later visible RFP1 remote motion in the same section',()=>{
 const schedule=createLiveMotionPixelSchedule();let index=0;
 for(let frame=180;frame<184;++frame){
  const selected=schedule.select(stationary(sample({frame,index:index++}),0));
  assert.equal(selected.attempted,true);assert.equal(selected.budgetClass,'general');
 }
 for(let frame=184;frame<190;++frame){
  const selected=schedule.select(stationary(sample({frame,index:index++}),0));
  assert.equal(selected.attempted,false);
  assert.equal(selected.reason,'general-budget-exhausted-remote-reserved');
  assert.equal(selected.remoteReserveReason,'remote-stationary-display-endpoints');
 }
 const section=schedule.windows[0].sections[0];
 assert.equal(section.generalAttempted,4);assert.equal(section.remoteAttempted,0);
 for(let frame=190;frame<194;++frame){
  const s=frontier(stationary(sample({frame,index:index++}),1));
  const bytes=s.words.slice(),selected=schedule.select(s);
  assert.equal(selected.attempted,true);assert.equal(selected.budgetClass,'remote');
  assert.deepEqual(selected.qualifiedRemoteSlots,[1]);assert.deepEqual(s.words,bytes);
 }
 assert.equal(section.attempted,8);assert.equal(section.remoteAttempted,4);
 assert.equal(section.remoteReserveSkipReasons['remote-stationary-display-endpoints'],10);
 const reentered=frontier(stationary(sample({frame:180,index:index++}),1));
 reentered.words[5]=2;reentered.words[14]=3;
 assert.equal(schedule.select(reentered).attempted,false,'Identity/generation and frame rewind do not reset the section');
 assert.equal(section.attempted,8);
});

test('low opacity can use general attempts but cannot spend the four remote reservations',()=>{
 const schedule=createLiveMotionPixelSchedule();let index=0;
 for(const byte of [51,101,51,101]){
  const s=tintAlpha(frontier(stationary(sample({index:index++}),1)),1,byte);
  const selected=schedule.select(s);
  assert.equal(selected.attempted,true);assert.equal(selected.budgetClass,'general');
  assert.equal(selected.remoteReserveReason,'remote-foreground-opacity-insufficient');
 }
 const section=schedule.windows[0].sections[0];
 assert.equal(section.remoteAttempted,0);assert.equal(section.generalAttempted,4);
 for(let vertex=0;vertex<4;++vertex){
  const s=tintAlpha(frontier(stationary(sample({frame:185,index:index++}),1)),1,102);
  tintAlpha(s,1,101,vertex);
  const selected=schedule.select(s);
  assert.equal(selected.attempted,false,'Each submitted vertex participates in the opacity prerequisite');
  assert.equal(selected.remoteReserveReason,'remote-foreground-opacity-insufficient');
  assert.equal(section.remoteAttempted,0);
 }
 for(let i=0;i<4;++i){
  const s=tintAlpha(frontier(stationary(sample({frame:190,index:index++}),1)),1,102);
  assert.equal(schedule.select(s).budgetClass,'remote','102/255 meets the existing 0.4 minimum exactly');
 }
 assert.equal(section.attempted,8);
 assert.equal(section.remoteReserveSkipReasons['remote-foreground-opacity-insufficient'],8);
});

test('the reserved class follows the actual local seat and never treats the bullet as a remote player',()=>{
 for(const [count,local,remote] of [[2,0,1],[2,1,0],[3,2,0],[3,2,1]]){
  const schedule=createLiveMotionPixelSchedule();
  const movingBullet=localSeat(stationary(sample(),2),local,count);
  assert.equal(schedule.select(movingBullet).budgetClass,'general');
  const s=frontier(localSeat(stationary(sample({index:1}),remote),local,count),remote);
  const result=schedule.select(s);
  assert.equal(result.budgetClass,'remote');assert.deepEqual(result.qualifiedRemoteSlots,[remote]);
  if(local<2){
   const localOnly=localSeat(stationary(sample({index:2}),local),local,count);
   assert.equal(schedule.select(localOnly).budgetClass,'general');
  }
  assert.equal(schedule.windows[0].sections[0].remoteAttempted,1);
 }
 const both=frontier(frontier(localSeat(stationary(sample({index:3}),0),2,3),0),1);
 // Give the second observed remote its own nonzero, valid completed frontier.
 const f=new Float32Array(both.words.buffer);f[AUX+24+8]-=4;
 const schedule=createLiveMotionPixelSchedule(),result=schedule.select(both);
 assert.equal(result.budgetClass,'remote');assert.deepEqual(result.qualifiedRemoteSlots,[0,1]);
 assert.equal(schedule.windows[0].sections[0].remoteAttempted,1,'One PBO attempt consumes one shared remote reservation');
});

test('unused remote reservations remain explicit and cannot turn into extra general attempts',()=>{
 const schedule=createLiveMotionPixelSchedule();let index=0,attempted=0;
 for(const window of schedule.windows)for(const section of window.sections){
  for(let i=0;i<10;++i){
   const selected=schedule.select(stationary(sample({frame:section.first,index:index++}),0));
   if(selected.attempted){++attempted;assert.equal(selected.budgetClass,'general');}
   else assert.equal(selected.reason,'general-budget-exhausted-remote-reserved');
  }
  assert.equal(section.generalAttempted,4);assert.equal(section.remoteAttempted,0);
  assert.equal(section.scheduled,0,'Reservation alone never claims an actual GPU readback');
  assert.equal(section.remoteReserveSkipReasons['remote-stationary-display-endpoints'],10);
 }
 assert.equal(attempted,48);assert.deepEqual(schedule.windows.map(w=>w.attempted),[16,16,16]);
 assert.deepEqual(schedule.windows.map(w=>w.remoteAttempted),[0,0,0]);
});

test('unsafe/nonmoving frames consume no quota; attempted ROI/getter/queue failures never replenish it',()=>{
 const schedule=createLiveMotionPixelSchedule();let index=0;
 for(let i=0;i<32;++i){const s=sample({index:index++});s.words[10]|=8192;
  assert.equal(schedule.select(s).attempted,false);}
 for(let i=0;i<32;++i)assert.equal(schedule.select(stationary(sample({index:index++}))).attempted,false);
 assert.equal(schedule.windows[0].attempted,0);
 const failures=['no-supported-roi','failed','dropped','pending'];
 for(let i=0;i<8;++i){const s=sample({index:index++});
  const selected=schedule.select(s);assert.equal(selected.attempted,true);
  assert.equal(selected.budgetClass,i<4?'remote':'general');
  assert.equal(schedule.windows[0].sections[0].attempted,i+1,'Reservation precedes the attempted operation');
  s.readbackStatus=failures[i%failures.length];
 }
 assert.equal(schedule.select(sample({frame:199,index:index++})).attempted,false);
 assert.equal(schedule.select(sample({frame:200,index:index++})).attempted,true);
 assert.equal(schedule.select(sample({frame:180,index:index++})).attempted,false);
 assert.equal(schedule.windows[0].attempted,9);
 assert.equal(schedule.windows[0].sections[0].remoteAttempted,4);
 assert.equal(schedule.windows[0].sections[0].generalAttempted,4);
 // Margin failure is deliberately after reservation, not an adaptive search
 // for a successful ROI. The analyzer permits this whole on-viewport quad.
 const edge=sample({frame:220,index:index++}),f=new Float32Array(edge.words.buffer);
 for(let slot=0;slot<3;++slot)for(let i=0;i<4;++i)f[H+slot*R+50+i*7]=(i%2)*20;
 assert.equal(parity(edge,'ROI halo edge').eligible,true);
 assert.equal(schedule.select(edge).attempted,true);
 edge.readbackStatus='no-supported-roi';assert.equal(schedule.windows[0].sections[2].attempted,1);
});

test('selection preserves all metadata bytes and rejection boundaries; failures still fail final capture',()=>{
 const samples=[commit(sample({index:0})),commit(sample({index:1})),commit(sample({index:2}))];
 samples[1].words[10]|=8192;commit(samples[1]);
 const wordsBefore=samples.map(s=>s.words.slice()),baseline=analyze(samples),schedule=createLiveMotionPixelSchedule();
 for(const s of samples){const selected=schedule.select(s);s.pixelSelection={attempted:selected.attempted,reason:selected.reason};}
 assert.equal(samples.length,3);samples.forEach((s,i)=>assert.deepEqual(s.words,wordsBefore[i]));
 const afterSelection=analyze(samples);
 assert.deepEqual(afterSelection.accounting,baseline.accounting);
 assert.deepEqual(afterSelection.geometry,baseline.geometry);
 assert.equal(afterSelection.samples[1].reason,'frozen-replay-or-open-frame');
 assert.equal(afterSelection.coverage.met,false,'Metadata qualification is never a pixel PASS');
 for(const s of samples)if(s.pixelSelection.attempted)s.readbackStatus='failed';
 const failures=analyze(samples);assert.equal(failures.accounting.samples,3);
 assert.equal(failures.accounting.readbacks.failed,2);assert.equal(failures.captureComplete,false);
 assert.equal(failures.samples[1].reason,'frozen-replay-or-open-frame');
});

test('default-disabled capture does not touch core or GL; source inventory is explicit',t=>{
 const poison=new Proxy({},{get(){throw Error('Disabled observer touched a runtime dependency');}});
 const capture=createLiveMotionCapture({core:poison,app:1,gl:poison});
 capture.afterBlit([]);capture.afterCallback();assert.deepEqual(capture.finish(),{enabled:false});
 t.diagnostic(JSON.stringify({scope:'metadata qualification and quota only; no native/WebGL performance assertion',sources:before}));
});
