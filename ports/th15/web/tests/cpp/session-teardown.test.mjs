import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,string,report}from './helpers.mjs';
test('TH15 original scene teardown preserves run managers only on a normal stage transition and matches retry, continue, ending and audio ordering',async()=>{
 const c=await core(),m=await oracle(),f=c.session_teardown_create(),scene=m.allocate(0x100),effect=m.allocate(0x20),checkpointData=m.allocate(0x40),objects=Array.from({length:14},()=>m.allocate(0x240)),callbacks=Array.from({length:9},()=>m.allocate(0x30));
 const globals=[0x4e9bc4,0x4e9a64,0x4e9a60,0x4e9a5c,0x4e9bb4,0x4e9a8c,0x4e9bb8,0x4e9a6c,0x4e9a9c,0x4e9ba0,0x4e9bd0,0x4e9a80,0x4e9a68,0x4e9a70];let events=[],cases=0,checks=0;
 const record=id=>{events.push(id);return 0;},sp=()=>m.reg('ESP');
 m.replace(0x403f30,'checkpoint worker join boundary',()=>record(1));m.replace(0x45df50,'record serialization boundary',()=>record(2));m.replace(0x424670,'transition effect boundary',()=>record(3));
 for(const[at,kind]of[[0x45bb60,0],[0x450120,4],[0x434f90,5],[0x453e30,6],[0x419090,7],[0x43f610,8],[0x441670,9],[0x45e830,10],[0x426820,11],[0x4143b0,12],[0x41fb70,13]])m.replace(at,'object destructor boundary '+kind,()=>{m.u32(globals[kind],0);return record(100+kind);});
 m.replace(0x40e1c0,'background destructor boundary',()=>{const kind=m.reg('ECX')===objects[2]?2:3;m.u32(globals[kind],0);return record(100+kind);});
 m.replace(0x4903f0,'deallocation boundary',()=>m.u32(sp()+4)===objects[1]?record(101):0);m.replace(0x4904c4,'checkpoint buffer deallocation boundary',()=>m.u32(sp()+4)===checkpointData?record(4):0);
 m.replace(0x434ef0,'retained GUI reset boundary',()=>record(5));m.replace(0x421230,'retained item pool reset boundary',()=>record(6));m.replace(0x421390,'retained enemy clear boundary',()=>record(7));
 m.replace(0x4889a0,'stage animation bank retirement boundary',()=>record(m.u32(sp()+4)===101?8:9),1);m.replace(0x4018a0,'scene callback removal boundary',()=>record(m.u32(sp()+4)===callbacks[6]?10:11),1);
 m.replace(0x476f10,'audio command queue boundary',()=>record(20+m.u32(sp()+4)),3);m.replace(0x43e440,'audio channel reset boundary',()=>record(12));
 try{for(const destination of [2,4,10,11,12,14,15,16,0,9,13])for(let variant=0;variant<128;variant++){
  const stage=variant%7+1,starting=variant%2?stage:1,continues=[0,7,9,10,-1,2147483647][variant%6],flags=[0,3,8,0x10,0x20,0x30,0x40,0x100,0x200,0x300,0x118,0x220,0x240,0x330,0x308,0x341][variant%16],restart=variant%3===0,callbackFlags=[0,1,2,3,7,0xdeadbeef][variant%6];
  let owners=0x3fff;if(variant&16)owners&=~((1<<2)|(1<<5)|(1<<9));if(variant&32)owners&=~((1<<3)|(1<<6)|(1<<10));if(variant&64)owners&=~((1<<1)|(1<<11)|(1<<12)|(1<<13));
  for(let i=0;i<objects.length;i++){m.view(objects[i],0x240).fill(0);m.u32(globals[i],owners&(1<<i)?objects[i]:0);}for(const cb of callbacks){m.view(cb,0x30).fill(0);m.u32(cb+4,callbackFlags);}m.view(scene,0x100).fill(0);m.u32(scene+4,callbacks[6]);m.u32(scene+8,callbacks[7]);
  m.u32(objects[1],checkpointData);m.u32(objects[4]+4,callbacks[0]);m.u32(objects[4]+8,callbacks[1]);m.u32(objects[7]+4,callbacks[2]);m.u32(objects[7]+8,callbacks[3]);m.u32(objects[0]+0x210,callbacks[4]);m.u32(objects[0]+8,callbacks[5]);m.u32(objects[0]+4,callbacks[8]);
  m.u32(0x4e9a78,effect);m.u32(effect+12,101);m.u32(effect+16,102);m.i32(0x4e73f0,stage);m.i32(0x4e73f4,starting);m.i32(0x4e7414,continues);m.u32(0x4e7794,flags);m.f32(0x4e73e8,0.25);m.i32(0x4e7ecc,destination);m.u32(0x4e79cc,restart?0x10:0);m.u32(0x4e9a94,scene);m.u32(0x4e9ba8,123);m.u32(0x4e9bac,456);m.view(0x503d86,1)[0]=0;
  c.session_teardown_configure(f,stage,starting,continues,flags,owners,callbackFlags);events=[];m.call(0x43c6a0,{ecx:scene});assert.equal(c.session_teardown_run(f,destination,restart),1,string(c,c.session_teardown_error(f)));
  const label='destination '+destination+' variant '+variant,dv=new DataView(c.memory.buffer),state=c.session_teardown_state(f),presentation=c.session_teardown_presentation(f);
  assert.equal(c.session_teardown_flags(f),m.u32(0x4e7794),label+' mode');assert.equal(dv.getInt32(state+28,true),m.i32(0x4e7414),label+' continues');assert.equal(dv.getUint32(state+92,true),m.u32(0x4e73e8),label+' speed');
  assert.equal(dv.getUint32(presentation,true),m.u32(0x503c10),label+' frame skip');assert.equal(dv.getUint32(presentation+4,true),m.u32(0x4e8214),label+' clear color');
  let retained=0;for(let i=0;i<globals.length;i++)if(m.u32(globals[i]))retained|=1<<i;assert.equal(c.session_teardown_owned(f),retained,label+' exact run ownership');
  for(let i=0;i<3;i++){assert.equal(c.session_teardown_callback_flags(f,i)>>>0,m.u32(callbacks[i*2]+4),label+' callback group '+i);assert.equal(m.u32(callbacks[i*2]+4),m.u32(callbacks[i*2+1]+4));}assert.equal(m.u32(callbacks[8]+4),callbackFlags,label+' primary replay callback preserved');
  assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.session_teardown_events(f),c.session_teardown_event_count(f))),events,label+' operation order');assert.equal(m.u32(0x4e9ba8)|m.u32(0x4e9bac)|m.u32(0x4e9a94),0);checks+=15;cases++;
 }
 report('session-teardown',{passed:true,cases,checks,originalFunctions:['0x43c6a0'],scope:'Full original teardown control flow, nullable ownership, stage retry/continue/selection/demo/normal and Extra ending, worker join count, record save ordering, persistent versus released managers, previous background identity, all retained update/draw callback flags, enemy retention, animation retirement order, music command and output renderer state. Destructors, record serialization, audio channel operations and resource retirement are independently isolated platform/object boundaries; this is not full-game application verification.'});
 }finally{c.session_teardown_delete(f);m.close();}
});
