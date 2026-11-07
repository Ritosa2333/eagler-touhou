import assert from 'node:assert/strict';
import fs from 'node:fs';
import {createPracticeEngine} from '../web/practice-engine.js';
import {PRACTICE_DEFAULTS,PRACTICE_SETUP,PRACTICE_VERSION,normalizePractice,createPracticeClock} from '../web/practice-config.js';
const patch=JSON.parse(fs.readFileSync(new URL('../web/practice-assets.json',import.meta.url))).languages.jp.patch;
const entries=patch.practice.entries,cs=0x1aaf,ds=0x4000,base=0x10000,cb=base+cs*16,d=base+ds*16,r=base+0x90000;
let checks=0;
function instance(mode='target',setup={},options={},nativeResources=false){
  const mod={HEAPU8:new Uint8Array(0x120000)},h=mod.HEAPU8,v=new DataView(h.buffer);
  h.set(new TextEncoder().encode(patch.signature),cb+patch.mailbox_cs_offset);h.set(new TextEncoder().encode('HUMAConfig\0'),r);
  v.setUint16(d+0xba86,0,true);v.setUint16(d+0xba88,0x9000,true);v.setUint16(d+0xbcd0,0x11c0,true);v.setUint16(d+0x46b6,0x11c0,true);
  h[r+11]=4;h[r+13]=3;h[d+0x4664]=1;
  h[d+0x4676]=7;v.setUint16(d+0xbccc,1280,true);
  const config=normalizePractice({version:PRACTICE_VERSION,mode,setup:{...PRACTICE_SETUP,...setup},options:{...PRACTICE_DEFAULTS,...options}}),clock={speed:1,setSpeed(s){this.speed=s;}};
  const fixturePatch={...patch,practice:{...patch.practice,entries:{...entries}}};
  if(!nativeResources)delete fixturePatch.practice.entries.initialPower;
  delete fixturePatch.practice.entries.initialEntry; // JS-only fixture covers the legacy compatibility path.
  delete fixturePatch.practice.entries.arrivalPending;
  delete fixturePatch.practice.entries.targetReached;
  delete fixturePatch.practice.entries.initialHp;
  delete fixturePatch.practice.entries.targetAdvance;
  delete fixturePatch.practice.entries.targetOutcome;
  const engine=createPracticeEngine({mod,patch:fixturePatch,config,clock}),signal=port=>engine.signal(port,ds<<16|cs),b=(at,n)=>{h[d+at]=n;},w=(at,n)=>v.setUint16(d+at,n&65535,true);
  return {mod,h,v,engine,signal,b,w,clock};
}
{
  const t=instance('full');t.signal(0x7fe0);
  const mailbox=cb+patch.mailbox_cs_offset;t.h[mailbox+22]=1;
  t.h[cb+entries.resultVisible]=1;t.h[cb+entries.resultRequest]=2;
  t.h[cb+entries.pauseOpen]=1;t.h[cb+entries.pauseRequest]=3;
  assert.equal(t.engine.status.nativeResult.visible,true);assert.equal(t.engine.status.nativePause.request,3);
  for(const sequence of [0xfd,0xfe,0xff]){
    t.h[r+48]=sequence;
    assert.deepEqual(t.engine.status.nativeResult,{visible:false,request:0});
    assert.deepEqual(t.engine.status.nativePause,{visible:false,choice:0,request:0});
  }
  t.h[r+48]=0;t.h[mailbox]=0;
  assert.equal(t.engine.status.nativeResult.visible,false);assert.equal(t.engine.status.nativePause.visible,false);checks++;
}
{
  const t=instance('target',{power:64,rank:16,lives:2,bombs:3},{},true);t.signal(0x7fe0);
  assert.equal(t.h[cb+entries.targetPreparing],1);assert.equal(t.h[cb+entries.initialPower],64);
  assert.equal(t.h[cb+entries.initialLives],2);assert.equal(t.h[cb+entries.initialBombs],3);
  assert.equal(t.h[r+11],4,'host leaves resident stock to guest');assert.equal(t.h[d+0x4664],1);checks++;
}
{
  const t=instance('target',{}, {lockBombs:true});t.signal(0x7fe0);t.signal(0x7fe1);t.signal(0x7fe2);
  assert.equal(t.h[cb+entries.assistFlags],4,'initial settings seed guest flags');
  t.h[cb+entries.assistFlags]=5;t.b(0x4669,1);t.signal(0x7fe4);
  assert.equal(t.engine.status.options.invincible,true);assert.equal(t.engine.status.outcome,null);
  assert.equal(t.engine.status.playerHit,0);assert.equal(t.engine.status.misses,1);
  t.engine.configure({lockBombs:false});assert.equal(t.h[cb+entries.assistFlags],1,'host setting updates guest flags');checks++;
}
for(const stage of [5,6]){
  const t=instance('target',{stage,entry:'stage'});t.signal(0x7fe0);t.signal(0x7fe1);t.signal(0x7fe2);
  t.w(0xbcd0,0x6013);t.b(0x53d9,2);t.signal(0x7fe5);
  if(stage===6){t.b(0x53d9,254);t.signal(0x7fe5);assert.equal(t.engine.status.outcome,null,'Mugetsu hands over to Gengetsu');t.w(0xbcd0,0xc7da);t.b(0x53d9,2);t.signal(0x7fe5);}
  t.b(0x53d9,254);t.b(0x4669,1);t.signal(0x7fe5);assert.equal(t.engine.status.outcome,null,'a simultaneous hit must resolve before a clear');
  t.b(0x4669,0);t.signal(0x7fe5);assert.equal(t.engine.status.outcome,'clear','final Boss defeat completes practice');checks++;
}
{
  const t=instance('full',{stage:5,power:128,rank:24});t.signal(0x7fe0);t.signal(0x7fe1);
  assert.equal(t.h[r+73],1);assert.equal(t.h[d+0x53a0],1,'full play uses native Turbo mode');
  assert.equal(t.h[r+11],4);assert.equal(t.h[r+13],3);assert.equal(t.engine.status.power,1);assert.equal(t.engine.status.stage,0);checks++;
  assert.equal(t.engine.status.dream,12800);assert.equal(t.engine.status.dreamItems,7);assert.equal(t.h[cb+entries.resourceDirty],0);checks++;
  t.b(0x4669,1);t.signal(0x7fe4);assert.equal(t.engine.status.misses,1);t.signal(0x7fe2);assert.equal(t.engine.status.misses,1);checks++;
}
{
  const t=instance();t.h.fill(7,d+0x4349,d+0x4351);t.h.fill(9,d+0x435a,d+0x4362);t.b(0x1a66,3);t.signal(0x7fe0);t.signal(0x7fe1);
  assert.equal(t.h[r+73],1);assert.equal(t.h[d+0x53a0],1,'target practice uses native Turbo mode');
  assert(t.h.slice(d+0x4349,d+0x4351).every(n=>n===0));assert(t.h.slice(d+0x435a,d+0x4362).every(n=>n===0));assert.equal(t.h[d+0x1a66],0);
  assert(t.engine.status.ready&&t.engine.status.waiting);assert.equal(t.h[cb+entries.entryPause],1);assert.equal(t.h[r+11],1);assert.equal(t.h[r+13],0);
  assert.equal(t.engine.status.invincibility,0);assert.equal(t.engine.status.power,128);checks++;
  assert.equal(t.engine.status.dream,0);assert.equal(t.engine.status.dreamItems,0);assert.equal(t.h[cb+entries.resourceDirty],3);checks++;
  t.signal(0x7fe2);assert.equal(t.engine.status.waiting,false);
  t.engine.configure({invincible:true});t.b(0x4669,1);t.signal(0x7fe4);assert.equal(t.engine.status.misses,1);assert.equal(t.engine.status.playerHit,0);assert.equal(t.engine.status.invincibility,0);
  t.signal(0x7fe2);assert.equal(t.engine.status.misses,1);assert.equal(t.engine.status.outcome,null);checks++;
  t.engine.configure({lockPower:true});t.b(0x4664,64);t.signal(0x7fe3);assert.equal(t.engine.status.power,128);
  assert(t.h[cb+entries.resourceDirty]&1,'F4 must refresh the native shot function and HUD');
  t.engine.configure({lockPower:false});t.b(0x4664,64);t.signal(0x7fe3);assert.equal(t.engine.status.power,64);
  t.engine.configure({lockPower:true});t.b(0x4664,80);t.signal(0x7fe3);assert.equal(t.engine.status.power,64);checks++;
  t.engine.configure({lockBombs:true,lockLives:true,enemyInvincible:true});t.signal(0x7fe1);
  assert.equal(t.h[cb+entries.unlimitedBombs],1);assert.equal(t.h[cb+entries.lockLives],1);assert.equal(t.h[cb+entries.enemyLock],1);
  t.b(0x4664,16);t.engine.configure({lockPower:false});t.b(0x466a,20);t.signal(0x7fe3);assert.equal(t.engine.status.outcome,null);assert.equal(t.engine.status.power,16);checks++;
  t.h[r+50]=2;t.signal(0x7fe4);assert.equal(t.engine.status.bombsUsed,2);checks++;
}
// Skip only audited Extra entrance waits. Native frame-zero initialization and
// the selected attack's first frame must still run with their original clocks.
for(const [gengetsu,phase,deadline]of [[false,0,129],[false,1,64],[false,3,64],[false,5,128],[true,0,129],[true,1,64]]){
  const t=instance('target',{stage:6,entry:'boss',branch:gengetsu?'gengetsu':'auto',phase:gengetsu?8:6});
  t.signal(0x7fe0);t.w(0xbcd0,gengetsu?0xc7da:0x6013);t.b(0x53d9,phase);
  t.signal(0x7fe1);assert.equal(t.engine.status.phaseFrame,0,'native initializer keeps frame zero');
  t.w(0x53da,1);t.signal(0x7fe1);assert.equal(t.engine.status.phaseFrame,deadline-1);
  assert.equal(t.engine.status.phase,phase,'original update performs the transition');checks++;
}
for(const phase of [0,1]){
  const t=instance('target',{stage:5,entry:'boss',phase:16});
  t.signal(0x7fe0);t.w(0xbcd0,0x6013);t.b(0x53d9,phase);t.w(0x53da,1);t.signal(0x7fe1);
  assert.equal(t.engine.status.phaseFrame,phase===0?128:63);assert.equal(t.engine.status.phase,phase);checks++;
}
for(const frame of [2,20,63]){
  const t=instance('target',{stage:6,entry:'boss',branch:'gengetsu',phase:8});
  t.signal(0x7fe0);t.w(0xbcd0,0xc7da);t.b(0x53d9,3);t.b(0x53dd,255);
  t.w(0x53ca,3000);t.w(0x53d2,-12);t.w(0x53da,frame);t.signal(0x7fe1);
  assert.equal(t.v.getInt16(d+0x53ca,true),3000-(64-frame)*12);
  assert.equal(t.engine.status.phaseFrame,64);assert.equal(t.engine.status.phase,3);checks++;
}
for(const power of [1,16,64,128]){
  const t=instance('target',{entry:'boss',phase:2,power});t.signal(0x7fe0);t.signal(0x7fe1);
  assert.equal(t.engine.status.ready,false);assert.equal(t.engine.status.power,power);assert.equal(t.engine.status.dream,0);
  // A Dream pickup during the seek must not leak into the actual entry.
  t.b(0x4676,5);t.w(0xbccc,800);t.w(0xbcd0,0x6013);t.b(0x53d9,2);t.w(0x53d6,3050);t.signal(0x7fe1);
  assert(t.engine.status.ready);assert.equal(t.engine.status.power,power);assert.equal(t.engine.status.dream,0);assert.equal(t.engine.status.dreamItems,0);
  assert.equal(t.h[cb+entries.resourceDirty],3);
  t.b(0x4664,Math.min(128,power+1));t.b(0x4676,1);t.w(0xbccc,100);t.signal(0x7fe4);
  assert.equal(t.engine.status.power,Math.min(128,power+1));assert.equal(t.engine.status.dream,1000,'Dream must grow naturally after entry');checks++;
}
{
  const t=instance('target',{entry:'boss',phase:2,lives:2,bombs:3});t.signal(0x7fe0);t.w(0xbcd0,0x6013);t.b(0x53d9,2);t.w(0x53d6,3050);t.w(0x53e0,1950);t.signal(0x7fe1);
  assert.equal(t.h[r+11],3);assert.equal(t.h[r+13],3);
  t.b(0x4669,1);t.signal(0x7fe4);assert.equal(t.engine.status.outcome,null,'contact cannot end the attempt');
  t.b(0x4669,0);t.b(0x466a,40);t.signal(0x7fe4);assert.equal(t.engine.status.outcome,null,'deathbomb window remains open');
  t.b(0x466a,0);t.signal(0x7fe4);assert.equal(t.engine.status.outcome,null,'successful deathbomb continues');
  t.b(0x466a,25);t.signal(0x7fe4);t.h[r+11]=2;t.b(0x466a,0);t.signal(0x7fe4);assert.equal(t.engine.status.outcome,null,'spare lives allow a respawn');
  t.h[r+11]=1;t.b(0x466a,25);t.signal(0x7fe4);assert.equal(t.engine.status.outcome,null);t.b(0x466a,0);t.signal(0x7fe4);assert.equal(t.engine.status.outcome,'failed','final death finishes after its animation');checks++;
}
// Fixed deadlines and actual native loop counters: attack frames continue.
for(const [stage,phase,counter,limit,frameCap]of [[0,2,0xbced,16,0],[1,2,0x53df,11,0],[2,3,0x53df,8,0],[3,2,0x53df,52,0],[4,2,0x53df,4,0],[5,14,0x53df,18,0],[6,2,0x46ae,36,0],[0,3,0,0,1501],[6,4,0,0,192]]){
  const t=instance('target',{stage,entry:'boss',phase},{lockTime:true});t.signal(0x7fe0);t.w(0xbcd0,0x6013);t.b(0x53d9,phase);t.w(0x53d6,3050);t.signal(0x7fe1);
  t.w(0x53da,frameCap?frameCap-1:57);if(counter)t.b(counter,limit-1);t.signal(0x7fe2);
  if(counter){assert.equal(t.h[d+counter],0);assert.equal(t.engine.status.phaseFrame,57);}else assert.equal(t.engine.status.phaseFrame,0);checks++;
}
{
  const t=instance('target',{entry:'mid'},{lockTime:true});t.signal(0x7fe0);t.w(0x46b6,0x6013);t.b(0x53c5,3);t.w(0x53c2,800);t.signal(0x7fe1);t.b(0x4277,4);t.signal(0x7fe2);
  assert.equal(t.h[d+0x4277],2);checks++;
}
{
  const t=instance('target',{stage:5,entry:'boss',phase:16});t.signal(0x7fe0);t.w(0xbcd0,0x6013);t.b(0x53d9,14);t.w(0x53d6,1200);t.w(0x53e0,0);t.signal(0x7fe1);
  assert.equal(t.engine.status.bossHp,0);t.b(0x53d9,16);t.signal(0x7fe1);assert(t.engine.status.ready);assert.equal(t.engine.status.bossHp,1200);checks++;
}
// Arrival is synchronous after the native Boss/midboss state change, before
// delayed emitters update. Clear live flags once, retaining native templates
// and Boss entities needed by the selected attack.
const pools=[['bullets',0x5a22,26,440],['lasers',0x42f0,24,2],['gather',0x9292,42,16],['items',0xaf34,20,32],['circles',0x9594,10,16],['custom',0xb204,26,32]];
for(const setup of [
  {stage:0,entry:'boss',phase:3}, {stage:1,entry:'boss',phase:5},
  {stage:3,entry:'boss',phase:2,character:0}, {stage:3,entry:'boss',phase:4,character:2},
  {stage:5,entry:'boss',phase:4}, {stage:5,entry:'boss',phase:8},
  {stage:5,entry:'boss',phase:16}, {stage:6,entry:'boss',phase:7,branch:'gengetsu'},
  {stage:0,entry:'mid',phase:3}
]){
  const t=instance('target',setup);t.signal(0x7fe0);t.signal(0x7fe5);assert.equal(t.engine.debug.entryCleanup,null);
  t.signal(0x7fe1);
  for(const [,offset,stride,count]of pools)for(let i=0;i<count;i++){t.b(offset+i*stride,1+i%3);t.b(offset+i*stride+1,0x5a);}
  t.b(0xbcb9,12);t.b(0xbcba,45);t.b(0x42d8,0x6b);t.b(0x5a08,0x7c);
  t.w(setup.entry==='mid'?0x46b6:0xbcd0,setup.branch==='gengetsu'?0xc7da:0x6013);
  t.b(setup.entry==='mid'?0x53c5:0x53d9,setup.phase);t.w(setup.entry==='mid'?0x53c2:0x53d6,1200);
  t.signal(0x7fe5);assert(t.engine.status.ready&&t.engine.status.waiting);assert.equal(t.engine.status.outcome,null);
  assert.equal(t.engine.status[setup.entry==='mid'?'midHp':'bossHp'],1200);
  assert.equal(t.h[cb+entries.cleanupPending],1,'guest cleanup must run before the next native render');
  assert.equal(t.h[cb+entries.cleanupKeepBits],Number(setup.stage===3&&setup.entry==='boss'&&setup.character===0));
  for(const [,offset,stride,count]of pools)for(let i=0;i<count;i++){
    assert.equal(t.h[d+offset+i*stride],1+i%3,'host must leave native entity cleanup to guest');
    assert.equal(t.h[d+offset+i*stride+1],0x5a);
  }
  t.b(0x5a22,1);t.b(0x9292,1);
  for(let frame=0;frame<3;frame++){
    t.h.fill(0,d+0x4700,d+0x4d40);t.signal(0x7fe1);
    assert(t.h.subarray(d+0x4700,d+0x4d40).every(n=>n===0),'host must not invalidate the guest background');
    t.signal(0x7fe2);t.signal(0x7fe5);t.signal(0x7fe4);
    assert.equal(t.h[d+0x5a22],1);assert.equal(t.h[d+0x9292],1,'New attack emitters must survive');
  }
  checks++;
}
for(const mode of ['full','target']){
  const t=instance(mode,{entry:'stage'});t.signal(0x7fe0);t.b(0x5a22,1);t.signal(0x7fe1);t.signal(0x7fe5);
  assert.equal(t.h[d+0x5a22],1);assert.equal(t.engine.debug.entryCleanup,null);checks++;
}
assert.throws(()=>normalizePractice({version:PRACTICE_VERSION,mode:'target',setup:{entry:'mid',stage:6}}));
{
  const t=instance('target',{entry:'boss'});t.signal(0x7fe0);t.signal(0x7fe1);
  t.w(0x538a,3100);t.w(0x53c0,3100);t.signal(0x7fe1);assert.equal(t.v.getUint16(d+0x53c0,true),3099);checks++;
  t.engine.observeBoundary();assert.equal(t.engine.status.error,null);
  t.mod._emGetRunningProgram=()=>1;t.mod.UTF8ToString=()=>'PMD86';t.engine.observeBoundary();
  assert.match(t.engine.status.error,/MAIN 已退出/);assert.equal(t.engine.status.preparing,false);checks++;
}
const clock=createPracticeClock(1),a=clock.now();clock.setSpeed(.5);assert(clock.now()>=a);assert.equal(clock.speed,.5);clock.setSpeed(2);assert.equal(clock.speed,2);assert.throws(()=>clock.setSpeed(0));checks++;
console.log(`PASS ${checks} TH04 engine checks: full/target isolation, stocks, hits, F1/F2/F4, bomb statistics, clear priority, native timers, final survival HP and clock`);
