import test, {before} from 'node:test';
import assert from 'node:assert/strict';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {readFile} from 'node:fs/promises';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
let createHarness,nativeLifecycle;
before(async()=>{
  const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),emcc=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py'),out=path.join(root,'artifacts/ufo-harness.mjs');
const names=['reset','spawn','set_player','item_tick','ufo_tick','token','absorb','fixture_ufo','finish','fixture_item','fixture_state','fixture_ufo_age','fixture_fragments','death_drops','collect_aggregate','item','state','nth_id','ufo_tick_dialogue','apply_enemy_death','removed','direct_reset','enemy_alive','death_requested','effects','reward_frames','fixture_inventory','hud_rotation','hud_event_count','hud_event','visual_event_count','visual_event','clear_events','item_event_count','item_event_kind','fixture_private','animation_count','animation_visit'];
  const result=spawnSync(python,[emcc,'tests/native/ufo_harness.cpp','src/game/Items.cpp','src/game/Ufo.cpp','-std=c++20','-O2','-o',out,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(n=>'_'+n)),'-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPF32'])],{cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python}});
  assert.equal(result.status,0,result.stdout+result.stderr);createHarness=(await import(pathToFileURL(out))).default;
  nativeLifecycle=JSON.parse(await readFile(path.join(root,'tests/fixtures/ufo-lifecycle-native.json'),'utf8'));
  assert.equal(nativeLifecycle.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
});
const state=h=>Array.from(new Float32Array(h.HEAPF32.buffer,h._state(),24));
const item=(h,id)=>Array.from(new Float32Array(h.HEAPF32.buffer,h._item(id),21));
const near=(a,b,tolerance=.00002)=>assert.ok(Math.abs(a-b)<tolerance,`${a} != ${b}`);
async function ready(power=100){const h=await createHarness();h._reset(1,power);return h;}
test('native token spawn alternates sides, uses its own pool and retains retail type/script',async()=>{
  const h=await ready(),a=h._spawn(13,-200,-4),b=h._spawn(14,0,128);const first=item(h,a),second=item(h,b);
  assert.deepEqual(first.slice(0,5),[a,13,6,-192,-4]);near(first[5],-1.0606601238250732);near(first[6],1.0606601238250732);near(second[5],1.0606601238250732);assert.equal(first[9],178);
  for(let i=2;i<16;i++)assert.ok(h._spawn(10,0,200));assert.equal(h._spawn(10,0,200),0);assert.equal(state(h)[23],1);
  h._set_player(0,-192,-4,0,0);h._item_tick();const replacement=h._spawn(10,0,200);assert.ok(item(h,replacement)[5]>0,'a failed native token birth still advances the side counter');
});
test('native changing token freezes at 60 pixels, resumes outside, and cycles at 150',async()=>{
  for(const [distance,expected] of [[59.999,30],[60,30],[60.001,31]]){
    const h=await ready(),id=h._spawn(13,distance,0);h._fixture_item(id,100,30,0,0);h._set_player(0,0,0,0,0);h._item_tick();assert.equal(item(h,id)[8],expected);
  }
  const h=await ready(),id=h._spawn(13,100,200);h._fixture_item(id,100,150,0,0);h._set_player(0,-180,400,0,0);h._item_tick();assert.equal(item(h,id)[1],14);assert.equal(item(h,id)[8],1);assert.equal(item(h,id)[9],179);
});
test('native token boundary reflects after movement and stops reflecting after 1200',async()=>{
  const h=await ready(),id=h._spawn(10,175,200);h._fixture_item(id,100,0,2,0);h._item_tick();const e=item(h,id);near(e[3],177);near(e[5],-2);
  h._fixture_item(id,1200,0,40,0);h._item_tick();assert.equal(item(h,id)[0],0);assert.equal(state(h)[20],-4);
});
test('all 27 native inventory combinations preserve matching triples or the newest pair',async()=>{
  for(let a=0;a<3;a++)for(let b=0;b<3;b++)for(let c=0;c<3;c++){
    const h=await ready();h._token(a);h._token(b);h._token(c);const s=state(h),unique=new Set([a,b,c]).size;
    assert.equal(s[4],unique===2?2:3);assert.deepEqual(s.slice(1,4),unique===2?[b+1,c+1,0]:[a+1,b+1,c+1]);if(unique!==2)assert.equal(s[6],unique===1?a+1:-1);
  }
});
test('native UFO spawn uses power health and private-variable escape signals without synthetic movement',async()=>{
  const h=await ready(300);for(let i=0;i<3;i++)h._token(0);h._ufo_tick(0);assert.equal(state(h)[21],2300);assert.equal(state(h)[8],1);
  h._ufo_tick(0);assert.equal(state(h)[22],-999);h._ufo_tick(1);assert.equal(state(h)[22],999);
});
test('native fill rate, rainbow bucket exchange and full bonuses preserve reward timing',async()=>{
  const h=await ready();h._fixture_ufo(-1,0,0,0,100,3);h._absorb(1,1);near(state(h)[10],.030000001192092896);assert.deepEqual(state(h).slice(11,13),[0,1]);
  const r=await ready();r._fixture_ufo(1,.99,0,0,100,1);r._absorb(2,1);assert.equal(state(r)[10],1);const life=item(r,r._nth_id(0));assert.equal(life[1],4);assert.equal(state(r)[16],2,'full bonus remains a pickup until collection');
});
test('native burst multipliers and extras match original reward selection samples',async()=>{
  for(const [kind,fill,power,multiplier,extra] of [[-1,.25,100,2.5,[15]],[1,0,100,1,[4,13]],[1,1,400,2,[4,13]],[2,.25,100,4,[14]],[2,.5,100,6,[14]],[2,1,100,8,[14]],[3,1,100,2,[5,15]]]){
    const h=await ready();h._fixture_ufo(kind,fill,3,5,power,3);h._finish();const all=Array.from({length:state(h)[0]},(_,i)=>item(h,h._nth_id(i))),aggregate=all.find(e=>e[1]>=16);assert.equal(aggregate[1],18);near(aggregate[16],multiplier);assert.deepEqual(all.filter(e=>e[1]<16).map(e=>e[1]),extra);
    assert.equal(state(h)[14],0,'burst score must wait for aggregate collection');h._finish();assert.equal(state(h)[0],1+extra.length,'death reward is issued once');
  }
});
test('native aggregate collection conserves power and uses scoreStored units',async()=>{
  for(const kind of [-1,1,2,3])for(const power of [100,400]){
    const h=await ready();h._collect_aggregate(kind,3,5,2,power);const s=state(h);assert.equal(s[13],power===400?400:103);assert.equal(s[14],(kind===1?11000:10000)+(power===400?3:0));
  }
});
test('native ordinary rectangles, sticky attraction and type9 delayed lifecycle retain resource rules',async()=>{
  const h=await ready(),id=h._spawn(1,-180,100);h._set_player(0,0,100,0,0);h._item_tick();const above=item(h,id);assert.equal(above[2],3);
  h._set_player(0,150,400,0,0);h._item_tick();const below=item(h,id);assert.equal(below[2],3);assert.ok(below[3]>above[3]);assert.ok(below[4]>above[4]);
  const d=await ready(),point=d._spawn(9,0,400);assert.equal(item(d,point)[2],5);d._item_tick();assert.equal(item(d,point)[2],2);d._item_tick();assert.equal(item(d,point)[0],0);assert.equal(state(d)[15],1000010);assert.equal(state(d)[14],1);
});
test('complete native item tick samples retain attraction, death-window cancellation and aggregate delays',async()=>{
  const h=await ready(),id=h._spawn(1,-180,100);h._set_player(0,0,100,0,0);h._item_tick();h._set_player(0,150,400,0,0);h._item_tick();let e=item(h,id);
  near(e[3],-171.17901611328125);near(e[4],103.52705383300781);near(e[5],3.8209776878356934);near(e[6],3.5270564556121826);near(e[19],5.399999618530273);assert.equal(e[7],2);
  const d=await ready(),fall=d._spawn(1,-180,100);d._fixture_state(fall,3,5,0);d._set_player(0,150,400,0,4);d._item_tick();e=item(d,fall);assert.equal(e[2],1);near(e[3],-176.30029296875);near(e[4],103.36336517333984);assert.equal(e[5],0);assert.equal(e[6],0);
  const a=await ready(),aggregate=a._spawn(18,0,128);a._fixture_item(aggregate,59,0,0,0);a._set_player(0,150,400,0,0);for(let i=0;i<3;i++)a._item_tick();e=item(a,aggregate);near(e[3],.09658142179250717);near(e[4],128.17514038085938);near(e[20],.4000000059604645);assert.equal(e[7],62);
});
test('native absorption acceleration and pickup rectangles agree with retail complete tick inspection',async()=>{
  const h=await ready();h._fixture_ufo(1,0,0,0,100,1);h._fixture_ufo_age(100);const id=h._spawn(1,100,200);h._item_tick();const e=item(h,id);assert.equal(e[2],7);near(e[3],100);near(e[4],200);near(e[19],.028333332389593124);
  const rectangle=await ready(),token=rectangle._spawn(10,29,429);rectangle._fixture_item(token,0,0,0,0);rectangle._item_tick();assert.equal(item(rectangle,token)[0],0);assert.equal(state(rectangle)[4],1,'both rectangle axes, rather than a 30 pixel circle, decide pickup');
  const focused=await ready(),power=focused._spawn(1,40,440);focused._set_player(0,0,400,1,0);focused._item_tick();assert.equal(item(focused,power)[2],4);
});
test('all native ordinary reward types preserve stored power, score, PIV and fragment counters',async()=>{
  const expectations={
    1:[[101,0,1000000,2,0,2,0,0],[400,1,1000000,2,0,2,0,0]],
    2:[[100,296,1000000,2,0,2,0,0],[400,296,1000000,2,0,2,0,0]],
    3:[[200,0,1000000,2,0,2,0,24],[400,0,1020000,2,0,2,0,0]],
    4:[[100,0,1000000,2,2,2,0,256],[400,0,1000000,2,2,2,0,256]],
    5:[[100,0,1000000,2,0,2,2,256],[400,0,1000000,2,0,2,2,256]],
    6:[[100,0,1000000,3,0,2,0,256],[400,0,1000000,3,0,2,0,256]],
    7:[[100,0,1000000,2,0,3,0,256],[400,0,1000000,2,0,3,0,256]],
    8:[[400,0,1000000,2,0,2,0,12],[400,0,1010000,2,0,2,0,0]],
    9:[[100,1,1000010,2,0,2,0,0],[400,1,1000010,2,0,2,0,0]],
  };
  for(const [type,expected] of Object.entries(expectations))for(const [i,power] of [100,400].entries()){
    const h=await ready(power);h._spawn(Number(type),0,400);h._item_tick();if(Number(type)===9)h._item_tick();assert.deepEqual(state(h).slice(13,21),expected[i]);
  }
  for(const [type,life,bomb,expected] of [[4,4,0,[3,0,2,0]],[5,0,4,[2,0,3,0]]]){
    const h=await ready();h._fixture_fragments(life,bomb);h._spawn(type,0,400);h._item_tick();assert.deepEqual(state(h).slice(16,20),expected);
  }
});
test('native death power loss retains its one-level floor and emits seven deterministic small power items',async()=>{
  const angles=[-1.9634954929351807,-1.8512957096099854,-1.7390960454940796,-1.6268962621688843,-1.514696478843689,-1.4024966955184937,-1.290297031402588];
  for(const [power,after] of [[100,100],[400,300]]){
    const h=await ready(power);h._death_drops(0,400);assert.equal(state(h)[13],after);assert.equal(state(h)[0],7);
    const first=item(h,h._nth_id(0)),last=item(h,h._nth_id(6));assert.equal(first[1],1);near(Math.hypot(first[5],first[6]),3);assert.ok(first[5]<0&&last[5]>0);assert.ok(first[6]<0&&last[6]<0);
    for(let i=0;i<7;i++){const drop=item(h,h._nth_id(i));near(Math.atan2(drop[6],drop[5]),angles[i],.000001);}
  }
});

test('restored C++ token hints match all18 independent original completion cases and keep their owner',async()=>{
  for(const fixture of nativeLifecycle.hintCases){
    const h=await ready();h._fixture_inventory(...fixture.colors,0,2);const id=h._spawn(fixture.type,0,200);
    h._item_tick();const first=Array.from(new Float32Array(h.HEAPF32.buffer,h._item(id),24));
    assert.equal(first[21],Number(fixture.hintInterrupt!==0));assert.equal(first[22],fixture.hintInterrupt);
    near(first[3],fixture.positionAfterTick[0]);near(first[4],fixture.positionAfterTick[1]);
    h._item_tick();const second=Array.from(new Float32Array(h.HEAPF32.buffer,h._item(id),24));assert.equal(second[23],first[23]);
    assert.equal(h._item_event_count(),0,'a persistent hint must not reallocate or rebind every tick');
    h._fixture_inventory(1,1,1,3);h._item_tick();const removed=Array.from(new Float32Array(h.HEAPF32.buffer,h._item(id),24));assert.equal(removed[21],0);
    assert.equal(h._item_event_count(),fixture.hintInterrupt?1:0);
    if(fixture.hintInterrupt)assert.equal(h._item_event_kind(0),8,'full inventory explicitly removes the211 owner');
  }
});

test('token pickup queues the native211 exit after collecting and supports slot reuse',async()=>{
  const h=await ready();h._fixture_inventory(1,1,0,2);h._set_player(0,0,200,0,0);const id=h._spawn(10,0,200);
  h._item_tick();assert.equal(item(h,id)[0],0);assert.equal(state(h)[4],3);
  assert.deepEqual(Array.from({length:h._item_event_count()},(_,i)=>h._item_event_kind(i)),[7,1,9],'create hint, collect token, then queue hintIRQ1; no immediate deletion');
  const replacement=h._spawn(11,0,300);assert.ok(replacement>id);assert.equal(Array.from(new Float32Array(h.HEAPF32.buffer,h._item(replacement),24))[21],0);
});

test('restored inventory separates logical colors from original physical HUD rotation and overlays',async()=>{
  const h=await ready();
  for(let i=0;i<nativeLifecycle.appendColors.length;i++){
    h._token(nativeLifecycle.appendColors[i]-1);const observed=state(h),expected=nativeLifecycle.appendStates[i];
    assert.deepEqual(observed.slice(1,4),expected.slots);assert.equal(observed[4],expected.count);assert.equal(h._hud_rotation(),expected.rotation);
  }
  const events=Array.from({length:h._hud_event_count()},(_,i)=>Array.from(new Float32Array(h.HEAPF32.buffer,h._hud_event(i),10)));
  assert.deepEqual(events.filter(e=>e[1]===0).slice(0,4).map(e=>[e[4],e[7]]),[[1,11],[1,17],[2,10],[3,10]]);
  const overlays=events.filter(e=>e[1]===1);assert.equal(overlays.length,10);
  for(let i=0;i<overlays.length;i+=2){assert.equal(overlays[i][5],81);assert.equal(overlays[i][6],21);assert.equal(overlays[i][8],overlays[i+1][8]);assert.equal(overlays[i][9],1);}
  assert.ok(events.some((e,i)=>e[7]===9&&events[i+1]?.[7]===7&&events[i+2]?.[7]===8),'mismatch retains the9/7/8 animation sequence');
});

test('restored UFO escape action respects original equality420, later persistence and boss signal',async()=>{
  for(const [age,expected] of Object.entries(nativeLifecycle.escapeSignals)){
    const h=await ready();h._fixture_ufo(1,0,0,0,100,1);h._fixture_ufo_age(Number(age));h._fixture_private(123);h._ufo_tick(0);assert.equal(state(h)[22],expected);
  }
  const h=await ready();h._fixture_ufo(1,0,0,0,100,1);h._fixture_ufo_age(100);h._ufo_tick(1);assert.equal(state(h)[22],999);
});

test('dialogue asks for native Enemy death and awards only when its deferred callback executes',async()=>{
  const h=await ready();for(let i=0;i<3;i++)h._token(0);h._ufo_tick(0);h._clear_events();
  h._ufo_tick_dialogue(0,1);assert.equal(h._enemy_alive(),1);assert.equal(h._death_requested(),2);assert.equal(state(h)[0],0);assert.equal(state(h)[4],3);
  h._ufo_tick_dialogue(0,1);assert.equal(state(h)[0],0,'UFO19 never bypasses an Enemy creation/update guard');
  h._apply_enemy_death();assert.equal(h._enemy_alive(),0);assert.equal(h._effects(),1);
  const rewards=Array.from({length:state(h)[0]},(_,i)=>item(h,h._nth_id(i))[1]);assert.deepEqual(rewards,nativeLifecycle.dialogueRewardTypes);
  h._ufo_tick(0);assert.equal(state(h)[4],0);assert.equal(state(h)[7],0);assert.equal(h._effects(),1);
});

test('natural removal and native direct reset do not call the defeated reward path',async()=>{
  const exit=await ready();exit._fixture_ufo(1,.75,3,5,100,1);exit._removed(1);assert.equal(state(exit)[4],3,'Enemy18 removal leaves cleanup toUFO19');exit._ufo_tick(0);
  assert.equal(state(exit)[4],0);assert.equal(state(exit)[0],nativeLifecycle.naturalEscapeRewardTypes.length);assert.equal(exit._effects(),0);
  const reset=await ready();reset._fixture_ufo(1,.75,3,5,100,1);reset._fixture_ufo_age(31);const before=state(reset);reset._direct_reset();const after=state(reset);
  assert.equal(reset._enemy_alive(),1);assert.equal(after[7],0);assert.equal(after[4],0);assert.deepEqual(after.slice(8,13),before.slice(8,13));assert.equal(reset._effects(),0);
  const visual=Array.from(new Float32Array(reset.HEAPF32.buffer,reset._visual_event(reset._visual_event_count()-1),16));assert.equal(visual[1],6);assert.equal(visual[11],0);assert.equal(visual[15],54);
});

test('UFO death spawns the native ECL effect once and retains120 display ticks on cleanup',async()=>{
  const h=await ready();h._fixture_ufo(1,.75,3,5,100,1);h._finish();assert.equal(h._effects(),1);assert.equal(h._reward_frames(),120);assert.equal(state(h)[14],0);
  h._ufo_tick(0);assert.equal(h._reward_frames(),120,'native44a387 cleanup returns before the display decrement');
  h._ufo_tick(0);assert.equal(h._reward_frames(),119);h._finish();assert.equal(h._effects(),1);assert.equal(state(h)[0],3);
});

test('embedded Item22 animation runs only at the native surviving tail before its timer advances',async()=>{
  const h=await ready(),power=h._spawn(1,100,300),picked=h._spawn(10,0,400),delayed=h._spawn(9,0,400),expired=h._spawn(10,192,200);
  h._fixture_item(expired,1200,0,40,0);h._item_tick();assert.equal(h._animation_count(),1);
  const visit=Array.from(new Float32Array(h.HEAPF32.buffer,h._animation_visit(0),2));assert.deepEqual(visit,[power,0]);assert.equal(item(h,power)[7],1);
  assert.equal(item(h,picked)[0],0);assert.equal(item(h,expired)[0],0);assert.equal(item(h,delayed)[2],2);assert.equal(item(h,delayed)[7],0);
  assert.ok(Array.from({length:h._item_event_count()},(_,i)=>h._item_event_kind(i)).includes(6),'type9 activation binds its body once without a tail tick');
  h._item_tick();assert.equal(h._animation_count(),1);assert.equal(item(h,delayed)[0],0);
});
