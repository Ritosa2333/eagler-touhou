import test from 'node:test';
import assert from 'node:assert/strict';
import createCore from '../site/runtime/core.mjs';

const word=value=>{const b=Buffer.alloc(4);b.writeInt32LE(value);return b;};
const float=value=>{const b=Buffer.alloc(4);b.writeFloatLE(value);return b;};
const str=value=>{const n=Buffer.byteLength(value)+1,b=Buffer.alloc((n+3)&~3);b.write(value);return Buffer.concat([word(b.length),b]);};
function instruction(op,args=[],time=0,refs=0,count=args.length){
  const payload=Buffer.concat(args),b=Buffer.alloc(16+payload.length);
  b.writeInt32LE(time);b.writeUInt16LE(op,4);b.writeUInt16LE(b.length,6);b.writeUInt16LE(refs,8);b[10]=63;b[11]=count;payload.copy(b,16);return b;
}
function program(routines){
  const entries=Object.entries(routines),names=Buffer.concat(entries.map(([name])=>Buffer.from(name+'\0')));
  const start=(36+entries.length*4+names.length+3)&~3;let offset=start;
  const chunks=entries.map(([_,code])=>{const h=Buffer.alloc(16);h.write('ECLH');return Buffer.concat([h,...code,instruction(10)]);});
  const b=Buffer.alloc(start+chunks.reduce((n,c)=>n+c.length,0));b.write('SCPT');b.writeUInt16LE(1,4);b.writeUInt16LE(entries.length,16);names.copy(b,36+entries.length*4);
  for(let i=0;i<entries.length;i++){b.writeUInt32LE(offset,36+i*4);chunks[i].copy(b,offset);offset+=chunks[i].length;}return b;
}
async function ready(routines,character=0){const c=await createCore(),b=program(routines),p=c._malloc(b.length);c.HEAPU8.set(b,p);assert.equal(c._th12_load_ecl(p,b.length),1);c._free(p);c._th12_start(character,0,1,1234);return c;}
function sprites(c,kind){const n=c._th12_draw(),p=c._th12_draw_ptr(),stride=c._th12_draw_stride(),view=new DataView(c.HEAPU8.buffer);const out=[];for(let i=0;i<n;i++){const at=p+i*stride;if(view.getInt32(at+24,true)!==kind)continue;out.push({x:view.getFloat32(at,true)-224,y:view.getFloat32(at+4,true)-16,angle:view.getFloat32(at+8,true)-Math.fround(Math.PI/2),bank:view.getInt32(at+16,true),animation:view.getInt32(at+20,true),age:view.getInt32(at+32,true)});}return out;}
const hold=()=>instruction(83,[word(100000)]);
const spawn=(op,name,x,y,refs=0)=>instruction(op,[str(name),float(x),float(y),word(100),word(0),word(0)],0,refs,6);
const bind=animation=>instruction(259,[word(0),word(animation)]);
const near=(actual,expected,label)=>assert.ok(Math.abs(actual-expected)<.000002,`${label}: ${actual} != ${expected}`);

test('spawn uses logical reference bits after a variable-length name and adds the parent position',async()=>{
  const c=await ready({main:[instruction(300,[float(100),float(200)]),instruction(44,[float(75)]),spawn(256,'EnemyWithLongName',-1,30,2),hold()],EnemyWithLongName:[hold()]});
  assert.deepEqual(sprites(c,0).map(s=>[s.x,s.y]),[[175,230]]);
});
test('absolute and relative mirrored spawns reflect movement once, without moving the birth position',async()=>{
  const c=await ready({main:[instruction(300,[float(100),float(200)]),spawn(260,'Relative',10,20),spawn(261,'Absolute',10,20),hold()],Relative:[instruction(304,[float(0),float(2)]),hold()],Absolute:[instruction(304,[float(0),float(2)]),hold()]});
  assert.deepEqual(sprites(c,0).map(s=>[s.x,s.y]),[[110,220],[10,20]]);
  c._th12_tick(0,0);const values=sprites(c,0);near(values[0].x,108,'relative mirrored movement');near(values[1].x,8,'absolute mirrored movement');
});
test('enemy variables are copied at birth and remain independent across enemies',async()=>{
  const c=await ready({main:[instruction(44,[float(12)]),instruction(45,[float(-9981)]),spawn(257,'Child',1,20),instruction(44,[float(-12)]),instruction(45,[float(-9981)]),spawn(257,'Child',2,30),instruction(44,[float(0)]),instruction(45,[float(-9981)]),hold()],Child:[instruction(300,[float(-9981),float(-999999)],5,1),hold()]});
  for(let i=0;i<5;i++)c._th12_tick(0,0);
  assert.deepEqual(sprites(c,0).map(s=>[s.x,s.y]),[[12,20],[-12,30]]);
});
test('angle/speed interpolation advances while a position interpolation owns displacement',async()=>{
  const c=await ready({main:[spawn(257,'Child',0,100),hold()],Child:[instruction(304,[float(0),float(2)]),instruction(305,[word(2),word(0),float(0),float(6)]),instruction(301,[word(2),word(0),float(20),float(100)]),hold()]});
  c._th12_tick(0,0);near(sprites(c,0)[0].x,10,'first interpolated position');c._th12_tick(0,0);near(sprites(c,0)[0].x,20,'last interpolated position');c._th12_tick(0,0);near(sprites(c,0)[0].x,26,'completed speed interpolation');
});
test('health interrupts preserve their slots and timeout scripts do not replace health scripts',async()=>{
  const c=await ready({main:[spawn(257,'TestBoss',0,100),hold()],TestBoss:[instruction(414,[word(0),word(50),word(100),str('Health')],0,0,4),instruction(414,[word(1),word(0),word(200),str('Death')],0,0,4),instruction(421,[word(0),str('Timeout')],0,0,2),instruction(411,[word(40)],3),hold()],Health:[bind(31),hold()],Death:[bind(32),hold()],Timeout:[bind(33),hold()]});
  for(let i=0;i<4;i++)c._th12_tick(0,0);assert.equal(sprites(c,0)[0].animation,31);
  const d=await ready({main:[spawn(257,'TestBoss',0,100),hold()],TestBoss:[instruction(414,[word(0),word(50),word(3),str('Health')],0,0,4),instruction(421,[word(0),str('Timeout')],0,0,2),hold()],Health:[bind(31),hold()],Timeout:[bind(33),hold()]});
  for(let i=0;i<3;i++)d._th12_tick(0,0);assert.equal(sprites(d,0)[0].animation,33);
});
test('TH12 emitter modes reproduce original launch selection evidence',async()=>{
  // Explicit original inspection: 0x40a34d..0x40a617, seed 1234, count 4,
  // angle .25, spread -.5, speeds 4/1. Runtime tests never execute the EXE.
  const first=[1.5707963705062866,0,1.8207963705062866,.25,2.606194496154785,1.035398244857788,-.3665192127227783,.25,-.3665192127227783];
  for(let mode=0;mode<9;mode++){
    const c=await ready({main:[instruction(500,[word(0)]),instruction(507,[word(0),word(mode)]),instruction(504,[word(0),float(.25),float(-.5)]),instruction(505,[word(0),float(4),float(1)]),instruction(506,[word(0),word(4),word(2)]),instruction(501,[word(0)]),hold()]});
    const emitted=sprites(c,1);assert.equal(emitted.length,8);near(emitted[0].angle,first[mode],'mode '+mode+' first angle');
    if(mode<6)near(emitted[4].angle,first[mode]-(mode>=2?.5:0),'mode '+mode+' layer angle');
    const firstSpeed=mode===7?1.5339231491088867:mode===8?1.275545597076416:4;
    c._th12_tick(0,0);const moved=sprites(c,1)[0];assert.ok(Math.abs(Math.hypot(moved.x,moved.y)-firstSpeed)<.00003,'mode '+mode+' original speed');
    const hud=new Float32Array(c.HEAPU8.buffer,c._th12_hud(),20);assert.equal(hud[9],mode<6?0:mode===8?32:16);
  }
});
test('zero count emits no bullets and emitter polar offset / radius / fixed origin retain their semantics',async()=>{
  const c=await ready({main:[instruction(506,[word(0),word(0),word(1)]),instruction(501,[word(0)]),hold()]});assert.equal(sprites(c,1).length,0);
  const d=await ready({main:[instruction(507,[word(0),word(1)]),instruction(504,[word(0),float(0),float(0)]),instruction(523,[word(0),float(Math.PI/2),float(10)]),instruction(524,[word(0),float(20)]),instruction(525,[word(0),float(30),float(40)]),instruction(501,[word(0)]),hold()]});
  const b=sprites(d,1)[0];near(b.x,50,'radial origin x');near(b.y,50,'polar offset y');near(b.angle,0,'radius must not overwrite angle');
});
test('animation clock resets on binding and selected bank does not change the bound animation',async()=>{
  const c=await ready({main:[spawn(257,'Child',0,100),hold()],Child:[instruction(258,[word(2)]),bind(0),instruction(258,[word(1)],3),instruction(259,[word(0),word(45)],5),hold()]});
  for(let i=0;i<3;i++)c._th12_tick(0,0);assert.equal(sprites(c,0)[0].bank,2);
  for(let i=0;i<2;i++)c._th12_tick(0,0);const bound=sprites(c,0)[0];assert.equal(bound.bank,1);assert.equal(bound.animation,45);assert.equal(bound.age,1);
});

test('items keep tracking after the player leaves the point-of-collection line',async()=>{
  const c=await ready({main:[spawn(257,'Dropper',-180,100),hold()],Dropper:[instruction(410,[word(1)]),instruction(411,[word(0)]),hold()]});
  c._th12_set_initial(0,100*128,100,2,2,0);c._th12_tick(0,0);
  const above=sprites(c,3)[0];assert.ok(above);assert.ok(above.x>-180);
  // Move far below the line and outside the proximity attraction rectangle.
  c._th12_set_initial(150*128,400*128,100,2,2,0);c._th12_tick(0,0);
  const below=sprites(c,3)[0];assert.ok(below.x>above.x,'already attracted item must continue moving toward the player');assert.ok(below.y>above.y,'item must follow the descending player instead of resuming its upward/falling trajectory');
});

test('ordinary native pickup rectangles collect immediately and focused near attraction starts after movement',async()=>{
  for(const [character,x,collected] of [[0,29,true],[0,31,false],[1,34,true],[1,36,false],[2,29,true],[2,31,false]]){
    const c=await ready({main:[instruction(300,[float(0),float(200)]),instruction(410,[word(1)]),instruction(409),hold()]},character);
    c._th12_set_initial(x*128,200*128,100,2,2,0);c._th12_tick(0,0);
    assert.equal(sprites(c,3).length===0,collected);assert.equal(new Float32Array(c.HEAPU8.buffer,c._th12_hud(),20)[4],collected?101:100);
  }
  for(const [character,x,attracted] of [[0,49,true],[0,51,false],[1,58,true],[1,60,false],[2,49,true]]){
    const c=await ready({main:[instruction(300,[float(0),float(200)]),instruction(410,[word(1)]),instruction(409),hold()]},character);
    c._th12_set_initial(x*128,200*128,100,2,2,0);c._th12_tick(8,0);c._th12_tick(8,0);const item=sprites(c,3)[0];assert.ok(item);assert.equal(item.x>0,attracted);
  }
});

test('ordinary drops preserve primary types, count batches, native scatter and RNG consumption',async()=>{
  const empty=await ready({main:[instruction(409),hold()]});assert.equal(sprites(empty,3).length,0);
  assert.equal(new Float32Array(empty.HEAPU8.buffer,empty._th12_hud(),20)[9],2);
  const c=await ready({main:[instruction(300,[float(20),float(100)]),instruction(410,[word(2)]),instruction(407,[word(1),word(3)]),instruction(407,[word(2),word(3)]),instruction(408,[float(32),float(48)]),instruction(409),instruction(409,[],3),hold()]});
  const dropped=sprites(c,3);assert.deepEqual(dropped.map(s=>s.animation),[167,166,166,166,167,167,167]);
  // Read-only original 412100/412140 inspection, seed 1234 and origin (20,100).
  const native=[[20,100],[12.36124324798584,76.43350219726562],[42.64081573486328,96.34001922607422],[31.94236946105957,129.64321899414062],[-6.468851089477539,104.20158386230469],[11.174394607543945,65.99765014648438],[40.0731201171875,87.22154235839844]];
  for(let i=0;i<dropped.length;i++){assert.ok(Math.abs(dropped[i].x-native[i][0])<.00003,'scatter x '+i);assert.ok(Math.abs(dropped[i].y-native[i][1])<.00003,'scatter y '+i);}
  const hud=new Float32Array(c.HEAPU8.buffer,c._th12_hud(),20);assert.equal(hud[8],58727);assert.equal(hud[9],26);
  for(let i=0;i<3;i++)c._th12_tick(0,0);
  assert.equal(sprites(c,3).length,7,'immediate drop clears primary and counts');
  assert.equal(new Float32Array(c.HEAPU8.buffer,c._th12_hud(),20)[9],28);
});
test('clearing scatter counts retains the primary and UFO drops preserve native token types',async()=>{
  const c=await ready({main:[instruction(410,[word(1)]),instruction(407,[word(2),word(3)]),instruction(406),instruction(409),hold()]});
  assert.deepEqual(sprites(c,3).map(s=>s.animation),[166]);
  const d=await ready({main:[instruction(300,[float(0),float(200)]),instruction(410,[word(10)]),instruction(407,[word(14),word(2)]),instruction(409),hold()]});assert.deepEqual(sprites(d,3).map(s=>s.animation),[175,179,179]);
  const missing=new Uint32Array(d.HEAPU8.buffer,d._th12_unsupported(),1024);assert.equal(missing[410],0);assert.equal(missing[407],0);
});
