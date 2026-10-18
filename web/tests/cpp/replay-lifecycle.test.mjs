import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,string,report}from './helpers.mjs';
test('TH15 one run recorder preserves every stage snapshot, live RNG reset timing, configuration and old input streams across stage factories',async()=>{
 const c=await core(),m=await oracle(),manager=m.allocate(0x31c),owner=m.allocate(0x2c100),scene=m.allocate(0x100),timing=m.allocate(0x100),base=m.heap;let checks=0,stages=0,ticks=0;
 m.replace(0x401390,'update callback registration boundary',()=>1,2);m.replace(0x401440,'draw callback registration boundary',()=>1,2);m.replace(0x422160,'precomputed live input edge boundary',()=>0);
 const mapping=[[0,0,0],[10,0,4],[0,4,8],[0,8,12],[0,12,16],[0,16,20],[0,20,24],[2,0,28],[0,24,32],[0,28,36],[3,0,40],[2,48,44],[2,52,48],[0,32,52],[1,8,60],[2,12,64],[2,4,68],[0,36,72],[2,8,76],[1,28,80],[2,56,84],[1,32,88],[0,112,92],[1,0,96],[1,4,100],[2,40,104],[1,36,108],[1,40,112],[2,16,116],[2,20,124],[2,24,128],[2,28,132],[2,32,136],[2,36,140],[0,120,144],[4,0,148],[4,4,152],...[...Array(9)].map((_,i)=>[0,52+i*4,420+i*4]),[0,88,456]];
 try{for(let scenario=0;scenario<12;scenario++){
  const run=c.replay_lifecycle_create(),f=c.replay_lifecycle_model(run),saved=new Map(),expectedInputs=new Map(),set=(kind,offset,value)=>new DataView(c.memory.buffer).setUint32(c.replay_snapshot_field(f,kind)+offset,value>>>0,true);
  try{
   m.heap=base;m.view(base,0x50000).fill(0);m.view(manager,0x31c).fill(0);m.view(owner,0x2c100).fill(0);m.view(scene,0x100).fill(0);m.view(timing,0x100).fill(0);m.u32(0x4e9bc4,manager);m.u32(0x4e9bb8,owner);m.u32(0x4e9a94,scene);m.u32(0x4e9a88,timing);
   const config=Buffer.from(Array.from({length:0x6c},(_,i)=>(scenario*13+i*7)&255));memory(c,c.replay_lifecycle_config(run),config.length).set(config);m.write(scene+0x24,config);
   const sequence=scenario>=10?[7]:Array.from({length:6-scenario%3},(_,i)=>1+scenario%3+i);
   const configure=(stage,phase,first)=>{m.view(0x4e73f0,0x1cc).fill(0);for(let i=0;i<mapping.length;i++){const[k,o,n]=mapping[i],v=scenario*50000+stage*8000+phase*100+i*31;set(k,o,v);m.u32(0x4e73f0+n,v);}
    for(const[k,o,n,v]of[[0,0,0,stage],[10,0,4,sequence[0]],[0,16,20,scenario%4],[0,20,24,0],[0,24,32,scenario%5],[0,32,52,scenario%3===0?17:-1]]){set(k,o,v);m.i32(0x4e73f0+n,v);}
    const flags=(scenario%4)*16;set(1,24,flags);m.u32(0x4e7794,flags);const music='th15_'+String(stage*2).padStart(2,'0'),p=c.allocate(64);memory(c,p,64).fill(0);memory(c,p,music.length+1).set(Buffer.from(music+'\0'));c.replay_snapshot_configure(f,p,first);c.release(p);m.write(0x4e748c,Buffer.from(music+'\0'));m.u32(0x4e7ed8,first?1:0);
   };
   let first=true;for(const stage of sequence){
    configure(stage,0,first);set(5,0,0xabcd0000|(scenario*4567+stage*543)&65535);set(5,4,15000+stage);set(6,0,0xfedc0000|(scenario*3321+stage*789)&65535);set(6,4,25000+stage);m.write(0x4e9a48,Buffer.from(memory(c,c.replay_snapshot_field(f,5),8)));m.write(0x4e9a40,Buffer.from(memory(c,c.replay_snapshot_field(f,6),8)));
    if(first){assert.equal(m.call(0x45b630,{ecx:manager,args:[0,0]}),0);assert.equal(c.replay_lifecycle_begin(run),1,string(c,c.replay_lifecycle_error(run)));assert.deepEqual(Buffer.from(memory(c,c.replay_lifecycle_description(run),0xa4)),Buffer.from(m.bytes(m.u32(manager+0x18),0xa4)),'run metadata '+scenario);checks++;}
    else {m.call(0x45d080);assert.equal(c.replay_lifecycle_prepare(run),1,string(c,c.replay_lifecycle_error(run)));}
    const native=m.u32(manager+0x1c+stage*4);assert.deepEqual(Buffer.from(memory(c,c.replay_lifecycle_header(run,stage),0x238)),Buffer.from(m.bytes(native,0x238)),'prepared stage '+scenario+':'+stage);assert.equal(c.replay_lifecycle_clock(run),first?-1:expectedInputs.get(sequence[sequence.indexOf(stage)-1]).clock);checks+=2;
    if(!first){assert.deepEqual(Buffer.from(memory(c,c.replay_snapshot_field(f,5),8)),Buffer.from(m.bytes(0x4e9a48,8)));assert.deepEqual(Buffer.from(memory(c,c.replay_snapshot_field(f,6),8)),Buffer.from(m.bytes(0x4e9a40,8)));checks+=2;}
    configure(stage,1,first);set(7,0,(stage-4)*16731);set(7,4,(stage+9)*47831);set(9,0,(scenario+stage)%2);m.write(owner+0x624,Buffer.from(memory(c,c.replay_snapshot_field(f,7),8)));m.u32(owner+0x16240,(scenario+stage)%2);
    m.call(0x45cf20);assert.equal(c.replay_lifecycle_activate(run),1,string(c,c.replay_lifecycle_error(run)));const header=Buffer.from(m.bytes(native,0x238));assert.deepEqual(Buffer.from(memory(c,c.replay_lifecycle_header(run,stage),0x238)),header,'active stage '+scenario+':'+stage);assert.equal(c.replay_lifecycle_clock(run),m.i32(manager+0x20c));saved.set(stage,header);checks+=2;
    const samples=[];for(let frame=0;frame<961+stage*7;frame++){
     const held=(frame*37+stage*977+scenario*13)&65535,pressed=(frame*53+scenario)&65535,released=(frame*67+stage)&65535,paused=frame%17===3;const fps=[60,59.4,59.5,256,30][Math.floor(frame/30)%5];m.u32(0x4e6d10,held);m.u32(0x4e6f34,pressed);m.u32(0x4e6f38,released);m.u32(0x4e7794,paused?0x300:0);m.f32(timing+0x30,fps);assert.equal(m.call(0x45c040,{ecx:manager}),1);assert.equal(c.replay_lifecycle_tick(run,held,pressed,released,fps,paused),1,string(c,c.replay_lifecycle_error(run)));assert.equal(c.replay_lifecycle_clock(run),m.i32(manager+0x20c));if(!paused){const b=Buffer.alloc(6);b.writeUInt16LE(held);b.writeUInt16LE(pressed,2);b.writeUInt16LE(released,4);samples.push(b);}ticks++;
    }
    expectedInputs.set(stage,{bytes:Buffer.concat(samples),clock:c.replay_lifecycle_clock(run)});
    for(const[n,h]of saved){assert.deepEqual(Buffer.from(memory(c,c.replay_lifecycle_header(run,n),0x238)),h,'previous stage header retained '+n);const input=expectedInputs.get(n).bytes;assert.equal(c.replay_lifecycle_input_count(run,n),input.length/6);assert.deepEqual(Buffer.from(memory(c,c.replay_lifecycle_inputs(run,n),input.length)),input,'previous input stream retained '+n);checks+=3;}
    first=false;stages++;
   }
  }finally{c.replay_lifecycle_delete(run);}
 }
 report('replay-lifecycle',{passed:true,scenarios:12,stages,ticks,checks,originalFunctions:['0x45b630','0x45d080','0x45cf20','0x45c040'],scope:'One live recorder across run/practice/Extra stage sequences, complete config/metadata and snapshot bytes, subsequent blank-header creation versus initial diagnostic words, snapshot timing, initial/new-run versus retained progress, player fixed position and focus, both RNG seeds/counters, activation clocks, suspended sampling, 900-frame boundary and retention of every prior stage input/header. Callback registration and hardware input are isolated; independent whole-game replay remains outstanding.'});
 }finally{m.close();}
});
