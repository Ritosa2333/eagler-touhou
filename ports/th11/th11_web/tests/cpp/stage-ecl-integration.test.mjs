import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,root,report} from './helpers.mjs';
import {graphicsOracle} from './graphics-oracle.mjs';
import {instruction,resource,bits} from './ecl-bytecode.mjs';

test('TH11 live ECL stage interrupts and camera-dependent spawns match original dispatcher',async()=>{
 const c=await core(),m=await oracle();graphicsOracle(m);
 const archive=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
 const raw=c.allocate(archive.length),source=c.resources_create(),session=c.game_session_create(),context=c.allocate(0x1024),command=c.allocate(128);
 const owner=m.allocate(0x2678),state=owner+0x103c,nc=m.allocate(0x1024),ni=m.allocate(128),ns=m.allocate(0x3158),manager=m.allocate(128),child=m.allocate(0x2678);
 const device=m.allocate(4),table=m.allocate(0x180);m.u32(0x4c3288,device);m.u32(device,table);m.u32(0x4c3268,0);
 m.u32(table+0xb0,m.registerImport({dll:'stage-ecl',name:'transform',argc:3,handler:()=>0}));
 m.u32(table+0xbc,m.registerImport({dll:'stage-ecl',name:'viewport',argc:2,handler:()=>0}));
 m.u32(0x4a8d7c,manager);m.u32(owner,0x494074);m.u32(owner+4,nc);m.u32(state+0x1614,owner);m.u32(nc+4,ni);m.u32(nc+0x1014,owner);
 const identity=Buffer.alloc(64);for(let i=0;i<4;++i)identity.writeFloatLE(1,i*20);m.write(0x4a5900,identity);
 let nativeSpawn=null,checks=0,interrupts=0,spawns=0;
 m.replace(0x4108f0,'spawn record boundary',()=>{nativeSpawn=Buffer.from(m.bytes(m.reg('EBX'),80));return child;},1);
 const dv=()=>new DataView(c.memory.buffer),put=(p,v)=>dv().setUint32(p,v>>>0,true),get=p=>dv().getUint32(p,true),bytes=(p,n)=>Buffer.from(memory(c,p,n));
 const error=()=>{const b=memory(c,c.game_session_error(session),512);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
 const invoke=ins=>{memory(c,command,128).fill(0);memory(c,command,ins.length).set(ins);m.view(ni,128).fill(0);m.write(ni,ins);put(context+4,command);m.resetThreadFPU();const expected=m.call(0x412e30,{ecx:state})|0;assert.equal(c.game_session_ecl_command(session,context),expected,error());};
 memory(c,raw,archive.length).set(archive);assert.equal(c.resources_open(source,raw,archive.length),1);
 try{
  for(let number=1;number<=7;++number){
   assert.equal(c.game_session_begin(session,source,number,0,0,number===7?4:1,0),1,error());
   const s=c.game_session_stage_data(session,0),play=c.game_session_stage_data(session,1),world=c.game_session_stage_data(session,2);
   assert.deepEqual(bytes(c.game_session_stage_data(session,5),12),bytes(play,12),'initial enemy camera 0');
   assert.deepEqual(bytes(c.game_session_stage_data(session,6),12),bytes(play+0xf0,12),'initial enemy camera delta');
   const std=readFileSync(resolve(root,`reference/assets/stage0${number}.std`)),file=m.allocate(std.length),base=file+std.readUInt32LE(8),labels=[];m.write(file,std);
   for(let off=std.readUInt32LE(8);off+8<=std.length;){const op=std.readInt16LE(off+4),length=std.readInt16LE(off+6);if(length<8)break;if(op===16)labels.push(std.readInt32LE(off+8));off+=length;}
   m.write(ns,bytes(s,0x3158));m.u32(ns+0x1c,base);m.u32(ns+0x4c,base+get(s+0x4c));m.u32(ns+0x44,0x4a7948);m.u32(0x4a8d60,ns);m.f32(0x4a7948,1);
   for(const id of [...labels,-1234567,...labels.toReversed()]){
    invoke(instruction(0x1b9,[id]));assert.equal(get(s+0x4c),m.u32(ns+0x4c)-base,'interrupt target');
    assert.deepEqual(bytes(s+0x38,12),Buffer.from(m.bytes(ns+0x38,12)),'interrupt timer');++interrupts;checks+=2;
   }
   // Reset after jumping through labels, then deliberately distinguish both
   // camera slots. Native ECL and movement use camera 0, ANM uses camera 2.
   assert.equal(c.game_session_begin(session,source,number,0,0,number===7?4:1,0),1,error());
   const play0=c.game_session_stage_data(session,1),world2=c.game_session_stage_data(session,2);
   for(const [offset,value]of[[0,37.25],[4,-18.5],[0xf0,.375],[0xf4,-.625],[0xf8,.25]])dv().setFloat32(play0+offset,value,true);
   assert.equal(c.game_session_update(session,0,0,0,0,0,0),1,error());
   assert.deepEqual(bytes(c.game_session_stage_data(session,5),12),bytes(play0,12));assert.deepEqual(bytes(c.game_session_stage_data(session,6),12),bytes(play0+0xf0,12));
   assert.deepEqual(bytes(c.game_session_stage_data(session,4),12),bytes(world2+0xf0,12));checks+=3;
   memory(c,play0+0xf0,12).fill(0);assert.equal(c.game_session_update(session,0,0,0,0,0,0),1,error());
   const leaf='__stage_oracle',code=resource({[leaf]:[instruction(10,[],{time:100000})]}),codePtr=c.allocate(code.length);memory(c,codePtr,code.length).set(code);assert.ok(c.ecl_attach(c.game_session_ecl_program(session),codePtr,code.length)>=0);c.release(codePtr);
   for(let sample=0;sample<16;++sample){
    const e=c.game_session_enemy_state(session,0),position=[sample*11.25-77,sample*7.5+20,sample*19.5-450];
    position.forEach((v,i)=>dv().setFloat32(e+0x34+i*4,v,true));put(e+0x1580,get(e+0x1580)|0x400000);m.write(state,bytes(e,0x163c));m.u32(state+0x1614,owner);
    m.write(0x4c3484,bytes(play0,0x118));m.write(0x4c36b4,bytes(world2,0x118));
    const op=[0x100,0x109,0x10e,0x10f][sample%4],worldSpace=op>=0x10e,args=[bits(12.25),bits(-9.5),...(worldSpace?[bits(sample*23-600)]:[]),10,0,0xffffffff];
    const name=Buffer.from(leaf+'\0'),length=(name.length+3)&~3,ins=Buffer.alloc(20+length+args.length*4);instruction(op,[],{count:args.length+1}).copy(ins);ins.writeUInt16LE(ins.length,6);ins.writeUInt32LE(length,16);name.copy(ins,20);args.forEach((v,i)=>ins.writeUInt32LE(v>>>0,20+length+i*4));
    nativeSpawn=null;invoke(ins);assert.ok(nativeSpawn,'native spawn reached');const result=c.game_session_enemy_state(session,1);
    assert.deepEqual(bytes(result+0x68,12),nativeSpawn.subarray(0,12),`stage ${number} opcode ${op.toString(16)} sample ${sample} position`);
    assert.deepEqual(bytes(play0,0x118),Buffer.from(m.bytes(0x4c3484,0x118)),'projection camera side effects');++spawns;checks+=2;
   }
  }
  report('stage-ecl-integration',{passed:true,checks,interrupts,spawns,stages:7,scope:'Live GameBattle ECL dispatcher, STD labels/timers and camera-0 inputs. Native 412e30 executes argument evaluation, 401ea0 camera selection and D3DX projection; native enemy creation is intercepted at its 80-byte spawn record, C++ creates a real passive child. This is not a full-game replay.'});
 }finally{c.game_session_delete(session);c.resources_delete(source);c.release(raw);c.release(context);c.release(command);m.close();}
});
