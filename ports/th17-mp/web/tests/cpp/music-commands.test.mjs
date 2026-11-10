import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report} from './helpers.mjs';
test('TH17 original music request insertion, normal streaming, pause, fade and release lifecycle',async()=>{
 const c=await core(),m=await oracle(),layout=readFileSync(resolve(root,'reference/assets/thbgm.fmt'));
 const bytes=c.allocate(layout.length),cs=c.allocate(256),ns=m.allocate(256),table=m.allocate(layout.length+52),engine=m.allocate(0x100),metadata=m.allocate(0x100),buffers=m.allocate(4),buffer=m.allocate(16),vtable=m.allocate(0x60),engineVtable=m.allocate(4);
 let events=[],waiting=false,fill=0,cases=0,steps=0,checks=0;
 memory(c,bytes,layout.length).set(layout);m.write(table,layout);m.u32(0x526170,table);m.view(0x509b8e,1)[0]=0;m.view(0x4b5cd2,1)[0]=0;m.u32(0x5247fc,1);
 m.u32(engine,engineVtable);m.u32(engine+8,buffers);m.u32(buffers,buffer);m.u32(buffer,vtable);m.u32(engine+0x10,metadata);m.u32(engine+0x14,1);m.u32(metadata+0x90,table);
 const sp=()=>m.reg('ESP'),index=p=>(p-table)/52,hash=value=>{let n=2166136261;for(const b of Buffer.from(value)){n^=b;n=Math.imul(n,16777619)>>>0;}return n|0;};
 const imported=(at,argc,handler)=>{const stub=m.importMap.get(m.u32(at));stub.argc=argc;stub.handler=handler;};
 const virtual=(at,name,argc,handler)=>m.u32(at,m.registerImport({dll:'fixture',name,argc,handler}));
 virtual(engineVtable,'close music engine',1,()=>{events.push(13);return 0;});
 virtual(vtable+0x48,'pause music buffer',1,()=>{events.push(7,1);return 0;});
 virtual(vtable+0x30,'resume music buffer',4,()=>{events.push(7,0);return 0;});
 virtual(vtable+0x3c,'refresh music volume',2,()=>{events.push(6);return 0;});
 m.replace(0x4651e0,'prepare file boundary',()=>{const slot=m.i32(sp()+4),file=m.string(m.u32(sp()+8));events.push(11,slot,hash(file));m.write(0x528d88+slot*256,Buffer.from(file+'\0'));return 0;},2);
 m.replace(0x477d20,'stop stream boundary',()=>{events.push(1,m.i32(sp()+4)?1:0);m.i32(engine+0x18,0);m.i32(engine+0x1c,0);m.i32(engine+0x20,0);return 0;},1);
 m.replace(0x4775f0,'open stream boundary',()=>{events.push(2,index(m.u32(sp()+4)));return 0;},1);
 m.replace(0x4784b0,'select stream header boundary',()=>{const p=m.u32(sp()+4);m.u32(metadata+0x90,p);if(m.i32(0x526c08)!==7)events.push(3,index(p));return 0;},2);
 m.replace(0x478140,'rewind buffer boundary',()=>0,1);
 m.replace(0x477920,'fill buffer boundary',()=>{events.push(4,0,1);return fill;},3);
 m.replace(0x477b40,'start buffer boundary',()=>{events.push(5);m.i32(engine+0x58,1);return 0;},3);
 m.replace(0x478550,'timed switch begin boundary',()=>0);
 m.replace(0x478610,'timed switch commit boundary',()=>{events.push(8,index(m.u32(metadata+0x90)));return 0;});
 m.replace(0x462710,'wall clock boundary',()=>0);
 imported(0x49a1e0,4,()=>{events.push(12);return 1;});imported(0x49a0a8,2,()=>waiting?1:0);imported(0x49a08c,1,()=>1);imported(0x49a0a4,4,()=>m.u32(m.u32(metadata+0x90)+0x10));
 try{for(let scenario=0;scenario<48;scenario++){
  const f=c.music_commands_create();assert.equal(c.music_commands_layout(f,bytes,layout.length),1);
  let ready=scenario%5!==0,release=scenario%3===1;waiting=scenario%4===0;fill=scenario%7===0?-1:0;const mode=scenario%3;
  c.music_commands_configure(f,false,mode,ready,false,release,waiting,false,fill);
  m.view(0x526c08,32*268).fill(0);m.view(0x528d88,4*256).fill(0);m.view(0x526b08,256).fill(0);
  for(const at of [0x18,0x1c,0x20,0x5c,0xa4])m.i32(engine+at,0);m.i32(engine+0x58,1);m.u32(metadata+0x90,table);
  m.u32(0x529e88,ready?engine:0);m.u32(0x524804,release?123:0);m.u32(0x529e90,456);m.u32(0x524800,99);m.u32(0x4b5cdc,0);m.view(0x4b5cd1,1)[0]=mode;
  const enqueue=(code,value,file='')=>{memory(c,cs,file.length+1).set(Buffer.from(file+'\0'));m.write(ns,Buffer.from(file+'\0'));m.call(0x4662e0,{ecx:0x5247f0,args:[code,value,ns]});assert.equal(c.music_commands_enqueue(f,code,value,cs),1,string(c,c.music_commands_error(f)));assert.deepEqual(Buffer.from(memory(c,c.music_commands_queue(f),32*268)),Buffer.from(m.bytes(0x526c08,32*268)),'enqueue '+scenario+'/'+code);checks++;};
  try{
   if(scenario>=40){for(let n=0;n<36;n++)enqueue(5,n%3,'fade-'+n);}
   else{enqueue(1,0,'th17_03.wav');enqueue(1,1,'th17_04.wav');enqueue(2,scenario%4===0?-1:0,scenario%4===0?'th17_05.wav':'');enqueue(6,0,'Pause');enqueue(7,0,'UnPause');enqueue(5,scenario%9-2);enqueue(8,0);enqueue(9,0,'th128_08.wav');enqueue(3,0);enqueue(4,0);enqueue(8,0);}
   for(let tick=0;tick<80;tick++){
    const busy=tick===3||tick===4;ready=m.u32(0x529e88)!==0;release=m.u32(0x524804)!==0;if(tick===25)waiting=false;
    c.music_commands_configure(f,false,mode,ready,busy,release,waiting,false,fill);m.i32(engine+0xa4,busy?1:0);events=[];
    m.call(0x465660,{limit:1000000});assert.equal(c.music_commands_process(f),1,string(c,c.music_commands_error(f)));
    const label=scenario+'/'+tick;assert.deepEqual(Buffer.from(memory(c,c.music_commands_queue(f),32*268)),Buffer.from(m.bytes(0x526c08,32*268)),label+' request bytes');
    assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.music_commands_events(f),c.music_commands_count(f))),events,label+' platform operations');
    assert.equal(string(c,c.music_commands_wave(f)),m.string(0x526b08),label+' current wave');
    for(let slot=0;slot<4;slot++)assert.equal(string(c,c.music_commands_prepared(f,slot)),m.string(0x528d88+slot*256),label+' prepared '+slot);
    assert.equal(c.music_commands_ready(f)!==0,m.u32(0x529e88)!==0,label+' engine');assert.equal(c.music_commands_release(f)!==0,m.u32(0x524804)!==0,label+' release');assert.equal(c.music_commands_fade(f),m.i32(engine+0x18),label+' fade');steps++;checks+=10;if(!m.i32(0x526c08))break;
   }
   assert.equal(m.i32(0x526c08),0,'drained '+scenario);cases++;
  }finally{c.music_commands_delete(f);}
 }
 report('music-commands',{passed:true,cases,steps,checks,originalFunctions:['0x4662e0','0x465660'],scope:'Native insertion and normal PCM command processor, complete 32-row queue bytes and 31-request capacity, prepared/current names, streaming stages, busy gates, fade, pause/resume and release wait; explicit file/decoder/OS output boundaries. Legacy alternate Windows cached streaming is not covered or used in the SDL production path.'});
 }finally{c.deallocate(bytes);c.deallocate(cs);m.close();}
});
