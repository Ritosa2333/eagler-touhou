import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
const word=n=>n>>>0,float=v=>{const b=Buffer.alloc(4);b.writeFloatLE(v);return b.readUInt32LE();};
function ins(op,args=[],time=0,refs=0){const b=Buffer.alloc(8+args.length*4);b.writeInt16LE(op);b.writeUInt16LE(b.length,2);b.writeInt16LE(time,4);b.writeUInt16LE(refs,6);args.forEach((v,i)=>b.writeUInt32LE(word(v),8+i*4));return b;}
test('TH11 ANM arithmetic, animation, interrupts and state instructions match original frames',async()=>{
 const c=await core(),m=await oracle(),vm=c.allocate(0x434),code=c.allocate(4096),env=c.anm_env_create(),q=m.allocate(0x434),nativeCode=m.allocate(4096);let checks=0;
 m.replace(0x46a720,'memset',()=>{const sp=m.reg('ESP'),p=m.u32(sp+4);m.view(p,m.u32(sp+12)).fill(m.u32(sp+8)&255);return p;});
 function normalized(b,base){const v=Buffer.from(b);for(const off of [4,16,0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c])v.writeUInt32LE(0,off);for(const off of [0x394,0x3a4,0x3a8])if(v.readUInt32LE(off))v.writeUInt32LE(v.readUInt32LE(off)-base,off);return v;}
 const cases=[];
 for(let op=6;op<=27;++op){const f=op&1,three=op>=18;cases.push({name:'arithmetic '+op,code:[ins(op,[f?float(10004):10000,f?float(3.25):13,...(three?[f?float(1.75):5]:[])],0,1)]});}
 for(let op=28;op<=39;++op){const f=op&1;for(const a of [1,3,5])cases.push({name:`branch ${op} ${a}`,code:[ins(op,[f?float(a):a,f?float(3):3,40,0]),ins(6,[10000,111],0,1),ins(6,[10001,222],0,1)]});}
 for(const op of [40,41,42,43,44,45,46,47])cases.push({name:'numeric '+op,code:[ins(op,op===47?[float(10004)]:[op===40?10000:float(10004),op===40?42:float(0.37)],0,1)]});
 const ordinary={48:[float(32.75),float(-17.125),float(0.37)],49:[float(0.125),float(0.375),float(-0.75)],50:[float(0.33),float(1.37)],51:[157],52:[23,71,199],53:[float(0.075),float(-0.02),float(0.093)],54:[float(0.03),float(-0.07)],55:[87,17],56:[17,4,float(111.71),float(-82.97),float(0.36)],57:[17,4,19,67,189],58:[17,3,5],59:[17,4,float(0.01),float(-0.5),float(0.36)],60:[17,4,float(0.71),float(1.97)],61:[],62:[],63:[],65:[0x00010002],66:[5],67:[2],68:[13],69:[],70:[float(0.03)],71:[float(-0.07)],72:[0],73:[1],74:[1],75:[7],76:[23,71,199],77:[93],78:[17,4,19,67,189],79:[17,3,5],80:[1],82:[1],83:[],85:[1],86:[1],87:[1],89:[1],93:[17,5,float(0.97)],94:[17,5,float(-0.78)],99:[0],100:[17,float(0.01),float(0.2),float(-0.31),float(70.91),float(80.81),float(0.717),float(0.91),float(-0.78),float(0.16)]};
 for(const [op,args] of Object.entries(ordinary))cases.push({name:'state '+op,code:[ins(+op,args)]});
 cases.push({name:'counted loop',code:[ins(6,[10000,5],0,1),ins(8,[10001,3],0,1),ins(5,[10000,16,0],0,1)]});
 cases.push({name:'interrupt return',code:[ins(63),ins(64,[7]),ins(51,[99]),ins(81)],interrupt:7});
 try{for(const fixture of cases)for(const speed of [1,0.375]){
  const bytes=Buffer.concat([...fixture.code,ins(0,[],32760),ins(-1)]);memory(c,code,4096).fill(0);memory(c,code,bytes.length).set(bytes);m.view(nativeCode,4096).fill(0);m.write(nativeCode,bytes);
  memory(c,vm,0x434).fill(0);m.view(q,0x434).fill(0);c.anm_vm_init(vm);m.reg('ESI',q);m.call(0x401fd0);
  const d=()=>new DataView(c.memory.buffer);for(const off of [0x3a4,0x3a8]){d().setUint32(vm+off,code,true);m.u32(q+off,nativeCode);}
  d().setFloat32(c.anm_env_rate(env),speed,true);m.f32(0x4a7948,speed);c.timer_set(vm+0x5c,0,c.anm_env_rate(env));m.reg('EAX',q+0x5c);m.call(0x406100,{args:[0]});
  for(const [off,v] of [[0x3b4,27],[0x3b8,11],[0x3bC,77]]){d().setInt32(vm+off,v,true);m.i32(q+off,v);}
  for(const [off,v] of [[0x3c4,2.713],[0x3c8,-0.7],[0x3e8,7.7],[0x3ec,8.1]]){d().setFloat32(vm+off,v,true);m.f32(q+off,v);}
  for(const visual of [0,1]){const r=c.anm_env_rng(env,visual),n=visual?0x4c2ef8:0x4c2f00;memory(c,r,8).fill(0);d().setUint16(r,12345+visual,true);m.view(n,8).fill(0);m.u32(n,12345+visual);}
  for(let frame=0;frame<48;++frame){if(frame===3&&fixture.interrupt){d().setInt16(vm+0x37c,fixture.interrupt,true);m.write(q+0x37c,Buffer.from([fixture.interrupt,0]));}
   const actual=c.anm_vm_update(vm,env),expected=m.call(0x44b4b0,{args:[q]});assert.equal(actual,expected,fixture.name+' return');
   const a=normalized(memory(c,vm,0x434),code),b=normalized(m.bytes(q,0x434),nativeCode);let first=-1;for(let j=0;j<a.length;j++)if(a[j]!==b[j]){first=j;break;}
   assert.equal(first,-1,`${fixture.name} rate=${speed} frame=${frame} offset=0x${first.toString(16)} actual=${first<0?'':a.subarray(first&~3,(first&~3)+4).toString('hex')} expected=${first<0?'':b.subarray(first&~3,(first&~3)+4).toString('hex')}`);
   for(const visual of [0,1])assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(env,visual),8)),Buffer.from(m.bytes(visual?0x4c2ef8:0x4c2f00,8)),fixture.name+' RNG');
   ++checks;if(actual)break;
  }
 }report('anm-interpreter',{passed:true,checks,scenarios:cases.length,scope:'isolated CPU instructions; manager/renderer callbacks require separate integration'});
 }finally{m.close();c.anm_env_delete(env);c.release(vm);c.release(code);}
});
