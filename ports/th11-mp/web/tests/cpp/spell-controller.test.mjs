import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';

test('TH11 spell lifecycle and tick match original state, counters and boundaries',async()=>{
 const c=await core(),m=await oracle(),f=c.spell_create(),name=Buffer.from('Spell test\0'),cn=c.allocate(64),nn=m.allocate(64);
 memory(c,cn,name.length).set(name);m.write(nn,name);
 const spell=m.allocate(0x920),stage=m.allocate(0x3000),boss=m.allocate(0x2700),enemies=m.allocate(0x60),player=m.allocate(0x900),bomb=m.allocate(0x50),replay=m.allocate(0x20),scores=m.allocate(0x30000),ascii=m.allocate(0x18500),bullets=m.allocate(0x46d680);
 for(const [address,p] of [[0x4a8d6c,spell],[0x4a8d60,stage],[0x4a8d7c,enemies],[0x4a8eb4,player],[0x4a8d64,bomb],[0x4a8eb8,replay],[0x4a8ebc,scores],[0x4a8d58,ascii],[0x4a8d68,bullets]])m.u32(address,p);
 m.u32(enemies+0x1c,boss);
 const vms=Array.from({length:6},()=>m.allocate(0x434));let created=0,nativeEvents=[];
 // Presentation is isolated here; separate ANM tests exercise actual scripts.
 m.replace(0x455a00,'record spell animation create',()=>{const p=vms[Math.min(created++,3)],id=vms.indexOf(p)+1;m.u32(p,id);m.u32(m.reg('ESI'),id);return m.reg('ESI');},3);
 m.replace(0x4561e0,'spell animation lookup',()=>vms[m.u32(m.reg('ESP')+4)-1]??0,1);
 m.replace(0x454d00,'spell text pixels',()=>0);
 m.replace(0x44a1e0,'spell sound',()=>{nativeEvents.push([5,m.reg('ESI'),0]);return 0;});
 m.replace(0x44acd0,'bind spell background',()=>0);
 m.replace(0x405b10,'bind initialized background',()=>0);
 m.replace(0x44b4b0,'update spell background',()=>0,1);
 m.replace(0x4563a0,'position circle',()=>0,1);
 m.replace(0x4562e0,'end circle',()=>{nativeEvents.push([3,0,0]);return 0;},1);
 m.replace(0x41f010,'spell result',()=>{nativeEvents.push([4,m.reg('EAX'),m.i32(m.reg('ESP')+4)]);return 0;},1);
 const view=()=>new DataView(c.memory.buffer),ptr=k=>c.spell_data(f,k),put=(p,n)=>view().setInt32(p,n,true),get=p=>view().getInt32(p,true),float=(p,n)=>view().setFloat32(p,n,true);
 function resetVms(){for(const p of vms){m.view(p,0x434).fill(0);m.u32(p+16,p);}m.u32(vms[3]+20,vms[4]+16);m.u32(vms[4]+16,vms[4]);m.u32(vms[4]+20,vms[5]+16);m.u32(vms[5]+16,vms[5]);m.view(vms[4]+0x3a2,2).set([137,0]);m.view(vms[5]+0x3a2,2).set([138,0]);created=0;}
 function equalState(label){assert.equal(get(ptr(2)),m.i32(spell+0x8e0),label+' flags');assert.equal(get(ptr(3)),m.i32(spell+0x8e4),label+' bonus');assert.equal(get(ptr(4)),m.i32(spell+0x8e8),label+' initial bonus');assert.equal(c.spell_value(f,0),m.i32(spell+0x8dc),label+' id');assert.equal(c.spell_value(f,2),m.i32(spell+0x8ec),label+' timeout');assert.equal(c.spell_value(f,3),m.i32(spell+0x8f4),label+' frame count');assert.deepEqual(Buffer.from(memory(c,ptr(1),12)),Buffer.from(m.bytes(spell+0x888,12)),label+' timer');assert.deepEqual(Buffer.from(memory(c,ptr(5),12)),Buffer.from(m.bytes(spell+0x910,12)),label+' position');assert.equal(get(ptr(8)),m.i32(stage+0x2ff0),label+' stage flags');}
 let starts=0,ticks=0,ends=0;
 try{
  for(let selection=0;selection<6;++selection)for(let difficulty=0;difficulty<5;++difficulty)for(let st=1;st<=7;++st)for(const playingReplay of [0,1]){
   resetVms();const id=(st*19+difficulty*2)%175,flags=0x7c,record=scores+selection*0x68d4+id*0x90+0x66c,aggregate=scores+id*0x90+0x27b64;
   m.view(spell,0x920).fill(0);m.u32(spell+0x8e0,flags);put(ptr(2),flags);put(ptr(8),1);m.u32(stage+0x2ff0,1);
   for(const [cp,np] of [[c.spell_record(f,selection,id),record],[c.spell_record(f,6,id),aggregate]]){memory(c,cp,72).fill(0);m.view(np,0x90).fill(0);put(cp+64,selection===5?99999:123);put(cp+68,difficulty===4?99999:456);m.i32(np+0x80,get(cp+64));m.i32(np+0x84,get(cp+68));}
   c.spell_select(f,selection,st,difficulty,playingReplay);m.i32(0x4a5710,Math.floor(selection/3));m.i32(0x4a5714,selection%3);m.i32(0x4a5720,difficulty);m.i32(0x4a5728,st);m.i32(0x4a5730,st%2?23:24);m.i32(replay+16,playingReplay);m.i32(bomb+60,1);m.f32(0x4a7948,1);float(ptr(0),1);
   const pos=[-33.25,47.125,12.5];pos.forEach((n,i)=>m.f32(boss+0x1070+i*4,n));m.reg('EAX',nn);m.call(0x40c650,{args:[id,1800]});assert.equal(c.spell_begin(f,id,1800,cn,1,...pos),1);equalState(`begin ${selection}/${difficulty}/${st}/${playingReplay}`);++starts;
   for(const [cp,np] of [[c.spell_record(f,selection,id),record],[c.spell_record(f,6,id),aggregate]]){assert.equal(get(cp+68),m.i32(np+0x84));assert.deepEqual(Buffer.from(memory(c,cp,64)),Buffer.from(m.bytes(np,64)));}
   if(st%2){c.spell_hide_circle(f);m.call(0x4107b0);equalState("hide circle");}
   // Alternate successful captures and failures, including capped counters.
   if(difficulty%2){put(ptr(2),get(ptr(2))&~2);m.u32(spell+0x8e0,m.u32(spell+0x8e0)&~2);}
   put(ptr(6),difficulty===4?999999999:1000);m.i32(0x4a56e4,get(ptr(6)));nativeEvents=[];
   assert.equal(c.spell_end(f),1);m.call(0x40cd60);equalState('end');assert.equal(get(ptr(6)),m.i32(0x4a56e4));
   for(const [cp,np] of [[c.spell_record(f,selection,id),record],[c.spell_record(f,6,id),aggregate]])assert.equal(get(cp+64),m.i32(np+0x80));
   const ev=Array.from({length:c.spell_value(f,4)/3},(_,i)=>[0,1,2].map(j=>c.spell_event(f,i*3+j))).filter(e=>e[0]!==2);assert.deepEqual(ev,nativeEvents);++ends;
  }
  c.spell_select(f,0,1,1,0);m.i32(0x4a5710,0);m.i32(0x4a5714,0);m.i32(0x4a5720,1);m.i32(0x4a5728,1);
  for(const speed of [1,.5,.99,1.01,1.25])for(const initial of [0,59,60,118,119,120,299,300,1800])for(const flags of [0,3,7,0x23,0x2b]){
   resetVms();put(ptr(2),flags);m.u32(spell+0x8e0,flags);float(ptr(0),speed);m.f32(0x4a7948,speed);
   const timer=ptr(1);put(timer,initial-1);put(timer+4,initial);float(timer+8,initial);put(timer+12,ptr(0));put(timer+16,1);
   m.i32(spell+0x888,initial-1);m.i32(spell+0x88c,initial);m.f32(spell+0x890,initial);m.u32(spell+0x894,0x4a7948);m.u32(spell+0x898,1);
   put(ptr(3),2000000);m.i32(spell+0x8e4,2000000);put(ptr(4),2000000);m.i32(spell+0x8e8,2000000);m.i32(spell+0x8ec,1800);
   m.i32(spell+0x8f4,c.spell_value(f,3));m.i32(spell+0x8dc,c.spell_value(f,0));put(ptr(8),3);m.u32(stage+0x2ff0,3);
   for(let j=0;j<3;++j){m.u32(spell+0x878+j*4,j+1);float(ptr(5)+j*4,0);m.f32(spell+0x910+j*4,0);}
   for(let frame=0;frame<16;++frame){const y=[95,96,128,129][frame%4],bombActive=frame%2,target=[Math.fround(Math.sin(frame)*137.2),Math.fround(80+frame*1.7),Math.fround(frame*.13)];m.f32(player+0x880,y);m.i32(bomb+60,bombActive);target.forEach((n,j)=>m.f32(boss+0x1070+j*4,n));
    assert.equal(c.spell_tick(f,y,...target,bombActive),1);m.reg('EDI',spell);m.call(0x40c0a0);equalState(`tick ${speed}/${initial}/${flags}/${frame}`);++ticks;
   }
  }
  report('spell-controller',{passed:true,starts,ticks,ends,scope:'Original 40c650/40c0a0/40cd60 numeric state, timer, records, replay suppression, score, title thresholds, bomb flags and circle smoothing. Presentation calls isolated; full replay and persisted score.dat are separate gates.'});
 }finally{c.release(cn);c.spell_delete(f);m.close();}
});
