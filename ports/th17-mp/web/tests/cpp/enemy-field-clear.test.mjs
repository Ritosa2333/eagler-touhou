import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,string,report,enableNativeMath} from './helpers.mjs';
import {ins,resource} from './ecl-fixtures.mjs';

test('TH17 field clearing preserves protected enemies, ordered death callbacks and animation retirement',async()=>{
 const c=await core(),m=await oracle();enableNativeMath(m);
 const {bytes}=resource({main:Buffer.concat([ins(40,[0]),ins(10,[],0,100000)])});
 const program=c.program_create(),cp=c.allocate(bytes.length),np=m.allocate(bytes.length),nativeProgram=m.allocate(0x90),definitions=m.allocate(8),manager=m.allocate(0x200),name=c.allocate(8),nativeName=m.allocate(8),req=c.allocate(84),nativeReq=m.allocate(84),vms=Array.from({length:16},()=>m.allocate(0x608));
 memory(c,cp,bytes.length).set(bytes);m.write(np,bytes);assert.equal(c.program_attach(program,cp,bytes.length),0);
 memory(c,name,5).set(Buffer.from('main\0'));m.write(nativeName,Buffer.from('main\0'));m.u32(nativeProgram+8,1);m.u32(nativeProgram+0x8c,definitions);m.u32(definitions,nativeName);m.u32(definitions+4,np+bytes.readUInt32LE(36));
 m.u32(0x4b76a0,manager);m.u32(0x52ae30,1);m.u32(0x4a0430,0x4b5918);m.u32(0x509a20,manager);
 m.u32(0x4b76b0,m.allocate(0x100));const player=m.allocate(0x190a0);m.u32(0x4b77d0,player);m.f32(player+0x1907c,1);
 m.replace(0x41f5e0,'tested damage boundary',()=>0);m.replace(0x420110,'tested distortion boundary',()=>0);
 m.replace(0x47b250,'oracle allocation',()=>m.allocate(m.u32(m.reg('ESP')+4)));m.replace(0x47b280,'oracle deallocation',()=>0);
 m.replace(0x476470,'animation handle lookup',()=>{const id=m.u32(m.reg('ESP')+4);return id>0&&id<=16?vms[id-1]:0;},1);
 let nativeDeaths=[],cases=0,checks=0,affected=0;
 m.replace(0x420a40,'separately verified death execution boundary',()=>{nativeDeaths.push(m.u32(m.reg('ECX')+0x5760));return 1;});
 const maskBits=[32,128,1024,0x400000,0x800000,256];
 try{for(let sample=0;sample<128;sample++){
  const fixture=c.enemy_manager_create(program),rate=[.5,.995,1,1.25][sample%4],suppress=sample%2;
  new DataView(c.memory.buffer).setFloat32(c.enemy_manager_rate(fixture),rate,true);m.f32(0x4b5918,rate);
  m.view(manager,0x200).fill(0);m.u32(manager+0x17c,nativeProgram);m.u32(manager+0x8c,1000);m.u32(manager+0x90,1);m.call(0x405bc0,{ecx:manager+0x98,args:[0]});m.u32(0x4b59e4,0);m.u32(0x4b5a00,0);
  const owners=[];
  for(let index=0;index<8;index++){
   const request=Buffer.alloc(84);request.writeFloatLE(index*9-31);request.writeFloatLE(96+index*3,4);request.writeInt32LE(100,20);memory(c,req,84).set(request);m.write(nativeReq,request);
   assert.equal(c.enemy_manager_spawn(fixture,name,req),1,string(c,c.enemy_manager_error(fixture)));owners.push(m.call(0x41e140,{ecx:manager,args:[nativeName,nativeReq]}));
  }
  for(let index=0;index<16;index++){
   const value=(index%3===0?0x4000000:0)|(index%2?64:0)|0x88000;
   const vm=c.enemy_manager_visual_vm(fixture,index);new DataView(c.memory.buffer).setUint32(c.anm_vm_visual(vm)+4,value,true);m.u32(vms[index]+0x534,value);
  }
  for(let index=0;index<8;index++){
   const entity=owners[index]+0x120c,f=c.enemy_manager_projection(fixture,index),dv=()=>new DataView(c.memory.buffer);
   const write=(ptr,at,value)=>{dv().setUint32(ptr,value>>>0,true);m.u32(at,value);};
   let flags=0x40000;for(let bit=0;bit<6;bit++)if((sample+index)&1<<bit)flags|=maskBits[bit];
   write(c.enemy_fixture_field(f,0,9,2),entity+0x4080,flags);write(c.enemy_attribute_field(f,6),entity+0x3f90,17+index);
   const drops=Buffer.alloc(144,0x31+index);memory(c,c.enemy_attribute_field(f,7),136).set(drops.subarray(0,136));m.write(entity+0x3f94,drops);dv().setFloat32(c.enemy_attribute_field(f,8),17,true);dv().setFloat32(c.enemy_attribute_field(f,8)+4,31,true);m.f32(entity+0x401c,17);m.f32(entity+0x4020,31);
   const pos=Buffer.alloc(12);[5+index,27+index,3.25].forEach((v,i)=>pos.writeFloatLE(v,i*4));memory(c,c.enemy_damage_field(f,6),12).set(pos);m.write(entity+0x280,pos);
   write(c.enemy_attribute_field(f,12)+4,entity+0x454c,7+index);
   for(let h=0;h<16;h++)write(c.enemy_attribute_field(f,4)+h*4,entity+0x124+h*4,(h+index)%17);
   c.enemy_set_death_script(f,name);m.write(entity+0x44f8,Buffer.from('main\0'));c.enemy_manager_apply_projection(fixture,index);
  }
  nativeDeaths=[];m.call(suppress?0x420f60:0x420bf0);assert.equal(c.enemy_manager_clear_field(fixture,suppress),1,string(c,c.enemy_manager_error(fixture)));
  assert.deepEqual(Array.from(new Uint32Array(c.memory.buffer,c.enemy_manager_scene_deaths(fixture),c.enemy_manager_scene_death_count(fixture))),nativeDeaths,'ordered deaths '+sample);affected+=nativeDeaths.length;
  assert.deepEqual(Buffer.from(memory(c,c.enemy_manager_timer(fixture),20)),Buffer.from(m.bytes(manager+0x98,20)),'timer '+sample);
  for(let index=0;index<8;index++){
   const entity=owners[index]+0x120c,f=c.enemy_manager_projection(fixture,index);
   for(const [ptr,at,n]of [[c.enemy_fixture_field(f,0,9,2),0x4080,4],[c.enemy_attribute_field(f,6),0x3f90,4],[c.enemy_attribute_field(f,7),0x3f94,136],[c.enemy_attribute_field(f,8),0x401c,8],[c.enemy_damage_field(f,6),0x280,12],[c.enemy_attribute_field(f,12)+4,0x454c,4],[c.enemy_attribute_field(f,4),0x124,64]])assert.deepEqual(Buffer.from(memory(c,ptr,n)),Buffer.from(m.bytes(entity+at,n)),sample+'/'+index+' state '+at.toString(16));
   assert.equal(string(c,c.enemy_death_script(f)),m.string(entity+0x44f8),sample+'/'+index+' death routine');checks+=8;
  }
  for(let index=0;index<16;index++)assert.equal(new DataView(c.memory.buffer).getUint32(c.anm_vm_visual(c.enemy_manager_visual_vm(fixture,index))+4,true),m.u32(vms[index]+0x534),'retired animation '+sample+'/'+index);
  checks+=18;cases++;c.enemy_manager_delete(fixture);
 }
 report('enemy-field-clearing',{passed:true,cases,checks,affected,originalFunctions:['0x420bf0','0x420f60'],scope:'Actual original list traversal and protection flags, drop suppression, last-hit position, phase contribution, death-routine suppression, retirement of sixteen animation owners, persistent animation policy and manager timer. EnemyDeath execution is a separately verified boundary.'});
 }finally{c.program_delete(program);c.deallocate(cp);c.deallocate(name);c.deallocate(req);m.close();}
});
