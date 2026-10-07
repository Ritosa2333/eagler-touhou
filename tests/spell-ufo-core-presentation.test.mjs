// Uses the production WASM, not a rebuilt test-only C++ harness. The fixture
// ECL supplies predictable actors; ANM and SHT files are the retail resources.
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';
import {readCoreAnimationPoses} from '../src/renderer.mjs';
import {readCoreDrawSchedule,orderPresentation} from '../src/draw-scheduler.mjs';
import {readCoreSpell,decodeSpellTitle,spellTextDescriptors} from '../src/spell-presentation.mjs';
import {readCoreUfo,ufoTextPoses} from '../src/ufo-presentation.mjs';

const word=value=>{const b=Buffer.alloc(4);b.writeInt32LE(value);return b;};
const float=value=>{const b=Buffer.alloc(4);b.writeFloatLE(value);return b;};
const str=value=>{const raw=Buffer.from(value+'\0'),b=Buffer.alloc((raw.length+3)&~3);raw.copy(b);return Buffer.concat([word(b.length),b]);};
function instruction(op,args=[],time=0){
  const payload=Buffer.concat(args),b=Buffer.alloc(16+payload.length);
  b.writeInt32LE(time);b.writeUInt16LE(op,4);b.writeUInt16LE(b.length,6);b[10]=63;b[11]=args.length;
  payload.copy(b,16);return b;
}
function program(routines){
  const entries=Object.entries(routines),names=Buffer.concat(entries.map(([name])=>Buffer.from(name+'\0'))),
    start=(36+entries.length*4+names.length+3)&~3;
  const chunks=entries.map(([,commands])=>{const h=Buffer.alloc(16);h.write('ECLH');return Buffer.concat([h,...commands,instruction(10)]);});
  const b=Buffer.alloc(start+chunks.reduce((sum,c)=>sum+c.length,0));b.write('SCPT');b.writeUInt16LE(1,4);
  b.writeUInt16LE(entries.length,16);names.copy(b,36+entries.length*4);let offset=start;
  for(let i=0;i<chunks.length;i++){b.writeUInt32LE(offset,36+i*4);chunks[i].copy(b,offset);offset+=chunks[i].length;}
  return b;
}
const hold=()=>instruction(83,[word(100000)]);
const spawn=(name,x,y)=>instruction(257,[str(name),float(x),float(y),word(100000),word(0),word(0)]);
const titleHex='90af98409144'; // CP932 星蓮船; no generated substitute artwork.
function encryptedTitle(){
  const plain=Buffer.from(titleHex+'00','hex'),b=Buffer.alloc((plain.length+3)&~3);plain.copy(b);
  let key=0x77,step=7;for(let i=0;i<b.length;i++){b[i]^=key;key=(key+step)&255;step=(step+16)&255;}
  return Buffer.concat([word(b.length),b]);
}
function load(core,bytes,method,...leading){
  const ptr=core._malloc(bytes.length);core.HEAPU8.set(bytes,ptr);
  try{assert.equal(core[method](...leading,ptr,bytes.length),1,method+' accepted fixture resource');}
  finally{core._free(ptr);}
}
function rawState(core){
  const ptr=core._th12_state(),size=core._th12_state_size();
  return new TextDecoder().decode(core.HEAPU8.subarray(ptr,ptr+size));
}
async function ready(){
  const core=await createCore();core._th12_select_stage(1);
  for(const[bank,name]of[[0,'bullet'],[1,'enemy'],[2,'stgenm01'],[4,'pl00'],[7,'front'],[8,'text'],[11,'ascii']])
    load(core,fs.readFileSync(new URL(`../site/assets/${name}.anm`,import.meta.url)),'_th12_load_anm',bank);
  load(core,fs.readFileSync(new URL('../site/assets/pl00a.sht',import.meta.url)),'_th12_load_sht',0);
  const ecl=program({
    main:[spawn('SpellBoss',0,100),spawn('Emitter',100,100),hold()],
    SpellBoss:[instruction(258,[word(2)]),instruction(259,[word(0),word(1)]),
      instruction(412,[word(0)]),instruction(411,[word(100000)]),
      instruction(422,[word(3),word(2400),word(0),encryptedTitle()]),
      instruction(423,[],160),hold()],
    Emitter:[instruction(258,[word(1)]),instruction(259,[word(0),word(0)]),
      instruction(500,[word(0)]),instruction(502,[word(0),word(2),word(3)]),
      instruction(507,[word(0),word(1)]),instruction(504,[word(0),float(Math.PI/2),float(0)]),
      instruction(505,[word(0),float(.4),float(.4)]),instruction(506,[word(0),word(1),word(1)]),
      instruction(501,[word(0)]),hold()]
  });
  load(core,ecl,'_th12_load_ecl');core._th12_start(0,0,1,0x1234);
  core._th12_set_initial(0,400*128,400,2,2,0);
  core._th12_set_replay_economy(10000,0,0,1,2,0,0);
  return core;
}
function presentation(core){
  const before=rawState(core),beforeState=JSON.parse(before),spell=readCoreSpell(core),ufo=readCoreUfo(core),
    poses=readCoreAnimationPoses(core),schedule=readCoreDrawSchedule(core),
    text=spellTextDescriptors(spell,poses),ufoText=ufoTextPoses(ufo),ordered=orderPresentation(poses,schedule);
  // Exercise repeated reads and ordering too: these getters use output
  // buffers, but may not change registered VMs, simulation state or RNG.
  assert.deepEqual(readCoreSpell(core),spell);assert.deepEqual(readCoreUfo(core),ufo);
  assert.deepEqual(readCoreAnimationPoses(core),poses);assert.deepEqual(readCoreDrawSchedule(core),schedule);
  assert.deepEqual(orderPresentation(poses,schedule),ordered);
  assert.equal(rawState(core),before,'all presentation getters leave semantic state unchanged');
  assert.deepEqual(JSON.parse(rawState(core)).rng,beforeState.rng);
  assert.deepEqual(spell,beforeState.spellState);
  const d=beforeState.economy.ufoDisplay;
  assert.deepEqual([ufo.displayFrames,ufo.multiplier,ufo.score,ufo.rewardX,ufo.rewardY],d);
  const nativeUfo=beforeState.economy.ufo;
  assert.deepEqual([ufo.enemyId,ufo.age,ufo.duration,ufo.fill,ufo.bucketA,ufo.bucketB],
    [nativeUfo[1],nativeUfo[3],nativeUfo[4],nativeUfo[6],nativeUfo[7],nativeUfo[8]]);
  assert.equal(ufo.enemy,null);assert.deepEqual(ufoText,[],'inactive UFO does not fabricate text');
  for(const p of poses){const metadata=schedule.get(p.id);assert.ok(metadata,'each captured VM has schedule metadata');
    assert.ok(Number.isInteger(metadata.drawPriority));assert.equal(typeof metadata.clipped,'boolean');
    assert.ok([0,1,2].includes(metadata.clip));assert.equal(metadata.clipped,metadata.clip!==0);}
  return {spell,ufo,poses,schedule,text,ordered,state:beforeState};
}

test('production core provides decoded spell title, bonus/history and manager draw order with read-only getters',async()=>{
  const core=await ready();let observed;
  for(let tick=0;tick<=125;tick++){
    if([0,1,60,90,120,125].includes(tick))observed=presentation(core);
    if(tick<125)core._th12_tick(1,0);
  }
  const {spell,poses,schedule,text,ordered}=observed;
  assert.equal(spell.flags&3,3);assert.equal(spell.titleHex,titleHex);assert.equal(decodeSpellTitle(spell.titleHex),'星蓮船');
  assert.deepEqual(spell.records,[1,0,1,0],'only current-session counters; persisted save history is outside scope');
  const title=text.find(p=>p.kind==='spell-title'),numbers=text.filter(p=>p.font);
  assert.ok(title);assert.equal(title.text,'星蓮船');assert.equal(title.pose.a.alpha,1);
  assert.deepEqual([title.pose.x,title.pose.y,title.pose.a.sx,title.pose.a.sy],[412,32,1,1]);
  assert.equal(numbers.length,2);assert.equal(numbers[0].text,String(spell.bonusDisplayed).padStart(8,' '));
  assert.equal(numbers[1].text,'00/01');assert.equal(numbers[0].colorARGB,0xffffffff);
  // Friendly and hostile shots share the0x1 owner namespace. Select the
  // hostile retail bank as well; friendly shots use their player ANM banks.
  const body=poses.find(p=>p.owner===0xffff0000),bullet=poses.find(p=>(p.owner>>>28)===1&&p.a.name==='bullet'),
    stock=poses.filter(p=>(p.owner&0xffff0000)>>>0===0xfffe0000);
  assert.ok(body,'real player VM');assert.ok(bullet,'ordinary hostile bullet VM');assert.equal(stock.length,3,'all physical UFO stock VMs');
  assert.equal(schedule.get(body.id).drawPriority,24);assert.equal(schedule.get(bullet.id).drawPriority,31);
  for(const p of stock)assert.equal(schedule.get(p.id).drawPriority,44,'stock embedded layer20 bypasses normal layer callback');
  assert.equal(schedule.get(title.pose.id).drawPriority,42,'text74 layer20 root uses its registered layer callback');
  const index=id=>ordered.findIndex(p=>p.id===id);
  assert.ok(index(body.id)<index(bullet.id));assert.ok(index(bullet.id)<index(title.pose.id));
  assert.ok(index(title.pose.id)<index(stock[0].id));
  const callbacks=orderPresentation([{...title.pose,logical:true},...numbers.map((p,i)=>({...p,drawPriority:45,clipped:false,order:i}))],schedule);
  assert.deepEqual(callbacks.map(p=>p.drawPriority),[42,45,45],'queued spell numbers are Font flag1 priority45');
});

test('production spell end removes queued numbers immediately and leaves the actual text74 outro',async()=>{
  const core=await ready();for(let i=0;i<160;i++)core._th12_tick(0,0);
  const after=presentation(core);assert.equal(after.spell.flags&1,0);
  assert.equal(after.text.filter(p=>p.font).length,0);
  assert.equal(after.text.filter(p=>p.kind==='spell-title').length,1,'title keeps its registered outro VM');
  for(let i=0;i<31;i++)core._th12_tick(0,0);
  assert.deepEqual(presentation(core).text,[],'title disappears only after the native ANM outro ends');
});

test('production compact spell/UFO/schedule readers reject escaped ranges without mutating the core',async()=>{
  const core=await ready(),before=rawState(core);readCoreAnimationPoses(core);
  for(const [reader,method,size]of[[readCoreSpell,'_th12_spell_size',65537],[readCoreUfo,'_th12_ufo_size',4097],
    [readCoreDrawSchedule,'_th12_anm_schedule_stride',36]]){
    const proxy=Object.create(core);proxy[method]=()=>size;assert.throws(()=>reader(proxy));
  }
  assert.equal(rawState(core),before);
});
