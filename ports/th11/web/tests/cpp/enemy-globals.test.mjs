import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
const SIZE=0x2678,STATE=0x103c;
const u32=n=>{const b=Buffer.alloc(4);b.writeUInt32LE(n>>>0);return b;};
function wrapper(m,fn,enemy,id,out){const b=Buffer.concat([Buffer.from([0xff,0x35]),u32(id),Buffer.from([0xb9]),u32(enemy),Buffer.from([0xb8]),u32(fn),Buffer.from([0xff,0xd0,0xdd,0x1d]),u32(out),Buffer.from([0xc3])]);const p=m.allocate(b.length);m.write(p,b);return p;}
function normalized(b,base,rate){b=Buffer.from(b);for(const o of [0x119c,0x25a0,0x25b4])if(b.readUInt32LE(o)===rate)b.writeUInt32LE(1,o);for(const o of [0x11a4,0x2650])if(b.readUInt32LE(o)===base)b.writeUInt32LE(2,o);return b;}
test('TH11 enemy construction and script globals preserve original values, precision and references',async()=>{
 const c=await core(),m=await oracle(),f=c.enemy_fixture_create(),p=c.enemy_fixture_data(f,0),boss=c.enemy_fixture_data(f,1),world=c.enemy_fixture_data(f,2),sprite=c.enemy_fixture_data(f,3),n=m.allocate(SIZE),nb=m.allocate(SIZE),nm=m.allocate(0x100),np=m.allocate(0x900),vm=m.allocate(0x440),idp=m.allocate(4),out=m.allocate(8);let checks=0,refChecks=0,precisionChecks=0,seed=4751;
 const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed;};
 try{
 m.write(0x4a8d7c,u32(nm));m.write(0x4a8eb4,u32(np));m.write(nm+0x1c,u32(nb));m.replace(0x456520,'animation sprite',()=>vm);m.replace(0x45db10,'constructor lookup',()=>0,1);
 m.write(n,Buffer.alloc(SIZE));m.reg('EDI',n);m.call(0x4111c0,{args:[0]});
 const cb=normalized(Buffer.from(memory(c,p,SIZE)),p,world+8),ob=normalized(m.bytes(n,SIZE),n,0x4a7948);
 assert.deepEqual(cb.subarray(STATE),ob.subarray(STATE),'constructor state');++checks;
 const wf=wrapper(m,0x4174b0,n,idp,out);
 const floatOffsets=[0x1070,0x1074,0x107c,0x1080,0x10a4,0x10a8,0x10bc,0x10c0,0x10c4,0x10d8,0x10dc,0x10f0,0x10f4,0x10f8,0x1170,0x1174,0x1178,0x117c,0x1180,0x1184,0x1188,0x118c,0x1198];
 const intOffsets=[0x1160,0x1164,0x1168,0x116c,0x1194,0x252c,0x25bc];
 for(let example=0;example<96;++example){
  const data=Buffer.alloc(SIZE),bd=Buffer.alloc(SIZE),w=Buffer.alloc(0x44);
  for(const target of[data,bd]){for(const o of floatOffsets)target.writeFloatLE((random()/4294967296-.5)*800,o);for(const o of intOffsets)target.writeUInt32LE(random(),o);}
  // Coincident and near-coincident aim need their original special case.
  const x=example%8===0?data.readFloatLE(0x1070):Math.fround((random()/4294967296-.5)*300),y=example%8===0?data.readFloatLE(0x1074):Math.fround(random()/4294967296*448);
  w.writeUInt16LE(random()&65535,0);w.writeFloatLE(1,8);w.writeFloatLE(x,12);w.writeFloatLE(y,16);
  for(const o of [0x24,0x28,0x2c,0x30,0x34,0x38,0x3c,0x40])w.writeUInt32LE(random(),o);if(example<8)w.writeInt32LE(example,0x28);
  memory(c,p,SIZE).set(data);memory(c,boss,SIZE).set(bd);memory(c,world,w.length).set(w);m.write(n,data);m.write(nb,bd);m.write(0x4c2f00,w.subarray(0,8));m.write(np+0x87c,w.subarray(12,24));
  for(const [dest,offset]of [[0x4a5744,0x24],[0x4a5720,0x28],[0x4a5710,0x2c],[0x4a5714,0x30]])m.write(dest,w.subarray(offset,offset+4));m.write(nm+0x10,w.subarray(0x34,0x40));m.write(nm+0x70,w.subarray(0x40,0x44));
  const si=(random()<<16)>>16;memory(c,sprite,4).set(u32(si));const sb=Buffer.alloc(2);sb.writeInt16LE(si);m.write(vm+0x3a2,sb);
  for(let id=-10002;id<=-9928;++id){
   m.write(idp,u32(id));m.resetThreadFPU();m.call(wf);const expected=Buffer.from(m.bytes(out,8)).readDoubleLE(),actual=c.enemy_variable(f,id,1);assert.equal(actual,expected,`float ${example}/${id}`);++checks;
   const ni=m.call(0x416ee0,{ecx:n,args:[id]})|0;assert.equal(c.enemy_variable(f,id,0),ni,`integer ${example}/${id}`);++checks;
   for(const kind of[2,3]){const nr=m.call(kind===2?0x4173a0:0x4179b0,{ecx:n,args:[id]}),cr=c.enemy_variable(f,id,kind);let expected=0;if(nr>=n&&nr<n+SIZE)expected=p+nr-n;else if(nr>=nb&&nr<nb+SIZE)expected=boss+nr-nb;else if(nr>=nm+0x10&&nr<nm+0x1c)expected=world+0x34+nr-(nm+0x10);assert.equal(cr,expected,`reference ${kind}/${id}`);++refChecks;}
   assert.deepEqual(Buffer.from(memory(c,world,8)),Buffer.from(m.bytes(0x4c2f00,8)),`rng ${example}/${id}`);
  }
 }
 // Native float argument getters must retain integer globals above 2^24.
 const cc=c.allocate(0x1024),ci=c.allocate(20),nc=m.allocate(0x1024),ni=m.allocate(20),vtable=m.allocate(24);
 const vt=Buffer.alloc(24);vt.writeUInt32LE(0x4174b0,12);m.write(vtable,vt);m.write(n,u32(vtable));
 const context=Buffer.alloc(0x1024);context.writeUInt32LE(n,0x1014);context.writeUInt32LE(ni,4);m.write(nc,context);context.writeUInt32LE(ci,4);memory(c,cc,context.length).set(context);
 const code=Buffer.concat([Buffer.from([0x31,0xc9,0xb8]),u32(nc),Buffer.from([0xba]),u32(0x45d550),Buffer.from([0xff,0xd2,0xdd,0x1d]),u32(out),Buffer.from([0xc3])]),cw=m.allocate(code.length);m.write(cw,code);
 for(const value of[16777217,16777219,2147483647,-2147483647,123456789]){memory(c,p+0x1160,4).set(u32(value));m.write(n+0x1160,u32(value));const ins=Buffer.alloc(20);ins.writeUInt16LE(1,8);ins.writeFloatLE(-9985,16);memory(c,ci,20).set(ins);m.write(ni,ins);m.call(cw);assert.equal(c.enemy_float_argument(cc,f,0,0),Buffer.from(m.bytes(out,8)).readDoubleLE());++precisionChecks;}
 c.release(cc);c.release(ci);
 report('enemy-globals',{passed:true,checks,refChecks,precisionChecks,scope:'constructor state; script globals, RNG consumption, typed writable references and extended argument precision'});
 }finally{c.enemy_fixture_delete(f);m.close();}
});