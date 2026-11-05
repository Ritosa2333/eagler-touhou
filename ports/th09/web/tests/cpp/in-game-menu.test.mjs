import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root} from './helpers.mjs';
import {installAnm,normalizedAnm} from './anm-oracle.mjs';
test('pause, continue and match-end menus preserve original animation, navigation and transition timing',async()=>{
 const m=await oracle(),c=await core(),raw=readFileSync(resolve(root,'reference/assets/ascii.anm')),data=c.allocate(raw.length);memory(c,data,raw.length).set(raw);const anm=installAnm(m,raw,1),native=m.allocate(5500),score=m.allocate(160),opponent=m.allocate(160),config=m.allocate(256),arg=i=>m.u32(m.reg('ESP')+4+i*4);let f=0,events=[],checks=0,scenarios=0;
 const u8=(a,v)=>v===undefined?m.view(a,1)[0]:(m.view(a,1)[0]=v),u16=(a,v)=>new DataView(m.view(a,2).buffer,m.view(a,2).byteOffset,2).setUint16(0,v,true);
 m.u32(0x4d66e0,anm.file);m.u32(0x4a7dac,score);m.u32(0x4a7de4,opponent);m.u32(0x4a7e78,config);u8(config+0xac,0);m.f32(0x4b36b8,1);
 m.replace(0x43e2f0,'menu-sound',()=>{events.push(200+arg(0));return 0;},2);
 m.replace(0x423498,'menu-resume',()=>{events.push(300);return 0;});
 for(const e of m.importMap.values())if(e.name==='timeGetTime'){e.handler=()=>10000;e.argc=0;}
 m.replace(0x41b5c6,'menu-continue',()=>{events.push(305);return 0;});
 for(const a of [0x42fd30,0x42fcf0])m.replace(a,'menu-replay',()=>0);
 const functions=[0x434740,0x434e90,0x4355f0],counts=[8,6,4];
 function compare(type,label){const cp=c.overlay_part(f,5+type);assert.deepEqual(memory(c,cp,8),m.bytes(native,8),label+' state');for(let n=0;n<counts[type];++n)assert.deepEqual(normalizedAnm(memory(c,cp+8+n*676,676),c.overlay_resource(f,0),c.overlay_resource(f,1),c.overlay_resource(f,2)),normalizedAnm(m.bytes(native+8+n*676,676),anm.file,anm.source,anm.sprites),label+' animation'+n);assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.overlay_events(f),c.overlay_event_count(f))),events,label+' events');++checks;}
 try{for(let type=0;type<3;++type)for(const mode of [0,1,2])for(const initial of type===0?[0,1,2,3,4,5,6,7,8,9]:type===1?[0,1,2,3,4]:[0,1,2,3,4,5,6]){
  if(f)c.overlay_delete(f);f=c.overlay_fixture();assert.equal(c.overlay_initialize(f,data,raw.length),1);m.view(native,5500).fill(0);m.view(score,160).fill(0);m.view(opponent,160).fill(0);m.u32(0x4a7ea8,mode);m.u32(0x4a7eac,2);m.u32(0x4a7ec4,0);m.u32(0x4b36d4,2);c.overlay_configure(f,mode,2,0,0,1);
  for(let frame=0;frame<72;++frame){
   if(frame===5){new DataView(c.memory.buffer).setInt32(c.overlay_part(f,5+type),initial,true);m.u32(native,initial);}
   const keys=frame%17===8?0x10:frame%17===10?0x20:frame%23===13?0x1001:frame===57&&initial%2?8:frame===60&&initial%3===0?0x200:0;
   u16(0x4acf3a,keys);m.u32(0x4b3690,0);u8(0x4a7ecc,1);u8(0x4a7ecd,1);u8(0x4a7ece,1);events=[];
   const actual=c.overlay_menu_step(f,type,keys),expected=m.call(functions[type],{ecx:native});assert.equal(actual,expected,`${type}/${mode}/${initial}/${frame} return`);
   const transition=m.u32(0x4b3690);if(transition)events.push(transition===1?301:transition===10?302:transition===11?303:transition===6?304:transition===13?306:1000+transition);
   compare(type,`${type}/${mode}/${initial}/${frame}`);assert.equal(c.overlay_value(f,2),u8(score+20),'continues');
  }++scenarios;
 }
 for(const [type,flags,continued,difficulty] of [[0,1,0,4],[1,8,0,1],[1,0,3,1],[2,8,0,1]]){
  if(f)c.overlay_delete(f);f=c.overlay_fixture();assert.equal(c.overlay_initialize(f,data,raw.length),1);m.view(native,5500).fill(0);m.u32(0x4a7ec4,flags);m.u32(0x4a7eac,difficulty);u8(score+20,continued);c.overlay_configure(f,0,difficulty,continued,flags,0);m.u32(0x4b36d4,0);if(type===0){new DataView(c.memory.buffer).setInt32(c.overlay_part(f,5),9,true);new DataView(c.memory.buffer).setInt32(c.overlay_part(f,5)+4,20,true);m.u32(native,9);m.u32(native+4,20);}u16(0x4acf3a,0);m.u32(0x4b3690,0);events=[];assert.equal(c.overlay_menu_step(f,type,0),m.call(functions[type],{ecx:native}));const t=m.u32(0x4b3690);if(t)events.push(t===1?301:t===11?303:t===6?304:1000+t);compare(type,'replay/continue boundary');++scenarios;
 }
 report('in-game-menu',{checks,scenarios,scope:'Original ascii.anm, all pause/continue/match-end states, menu colors and positions, interrupts, input delay, sound requests and outgoing transitions; platform captures and the requested session transitions are separate boundaries.'});
 }finally{if(f)c.overlay_delete(f);c.release(data);m.close();}
});
