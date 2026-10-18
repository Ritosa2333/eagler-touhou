import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report,bits} from './helpers.mjs';
test('Match scene, results, retry and next-stage transitions match the original',async()=>{
 const m=await oracle(),c=await core(),scene=m.allocate(0x1293c),players=[m.allocate(0x31000),m.allocate(0x31000)],scores=[m.allocate(160),m.allocate(160)],backgrounds=[m.allocate(64),m.allocate(64)],controllers=[m.allocate(256),m.allocate(256)],huds=[m.allocate(16),m.allocate(16)],route=m.allocate(16),config=m.allocate(204),resource=m.allocate(16),position=m.allocate(12);
 let f=0,p=[],events=[],checks=0,draws=0,dv=new DataView(c.memory.buffer);
 const offsets=[...Array.from({length:7},(_,i)=>12+i*0x2a4),0x123f4,0x12698,0x106b4,...Array.from({length:7},(_,i)=>0x10968+i*0x2a4),0x11eac,0x12150];
 const fields=Array.from({length:c.match_scene_field_count()},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(c.match_scene_fields()+(i*3+j)*4,true))),arg=i=>m.u32(m.reg('ESP')+4+i*4),event=(...a)=>events.push([...a,...Array(8-a.length).fill(0)].map(x=>x|0));
 const index=vm=>{const i=offsets.indexOf(vm-scene);assert.ok(i>=0,'animation '+(vm-scene).toString(16));return i;};
 m.replace(0x401660,'front-resource',()=>resource,1);m.u32(resource+4,1);m.u32(0x4d66e0,0x102);
 m.replace(0x4182e0,'messages',()=>0,3);m.replace(0x415c90,'dialogue-init',()=>0);
 m.replace(0x401560,'scene-animation',()=>{const vm=arg(0),script=arg(1);event(0,index(vm),m.reg('ECX')===0x102?1:0,script);m.view(vm,0x2a4).fill(0);m.write(vm+0x21a,[script&255,script>>>8]);m.u32(vm+0x1f8,1);m.u32(vm+0x1f0,0xffffffff);m.f32(vm+0x288,script*3-10);m.f32(vm+0x28c,script*2+1);m.f32(vm+0x290,.25);return 0;},2);
 m.replace(0x436ac0,'sprite',()=>{event(1,index(arg(0)),arg(1));m.write(arg(0)+0x214,[arg(1)&255,arg(1)>>>8]);return 0;},2);
 m.replace(0x436f30,'animation-step',()=>{event(2,index(arg(0)));m.i32(arg(0)+0x100,m.i32(arg(0)+0x100)+1);return 0;},1);
 m.replace(0x43a950,'animation-draw',()=>{event(3,index(arg(0)));return 0;},1);
 m.replace(0x403d90,'view',()=>{event(4,arg(0));m.u32(0x4b3448,0x4b3178+arg(0)*0xf0);return 0;},1);
 m.replace(0x422330,'outline',()=>{event(5,(m.reg('ECX')-scene-0xe86c)/100,m.reg('EDX'));return 0;});
 m.replace(0x434330,'number',()=>{const text=Array.from(m.bytes(arg(2),32)),end=text.indexOf(0),fmt=String.fromCharCode(...text.slice(0,end));if(fmt==='%7d'||fmt==='%8d')event(6,m.u32(arg(1)),m.u32(arg(1)+4),Number(fmt[1]),arg(3));else if(fmt.startsWith('STAGE'))event(7,m.u32(arg(1)),arg(3),arg(4),arg(5));else event(7,m.u32(arg(1)),-1,arg(3),arg(4));return 0;});
 m.replace(0x416590,'dialogue-step',()=>{event(8);return 0;});m.replace(0x417130,'dialogue-draw',()=>{event(9);return 0;});
 m.replace(0x416160,'dialogue-start',()=>{event(10,arg(0),arg(1));m.i32(scene+0xe94c,arg(0));return 0;},2);
 m.replace(0x4162d0,'victory',()=>{event(11,arg(0));m.i32(scene+0xe94c,0);return 0;},1);
 m.replace(0x43e2f0,'sound',()=>{event(12,arg(0),arg(1));return 0;},2);
 m.replace(0x401500,'attack-reset',()=>{event(13,controllers.indexOf(m.reg('ECX')-0x98));assert.equal(arg(0),0);return 0;},1);
 for(const [fn,kind,argc] of [[0x41cfe0,0,5],[0x41d0d0,1,6]])m.replace(fn,'round-clear',()=>{event(14,players.indexOf(m.reg('ECX')),kind);assert.deepEqual(m.readWords(arg(0),3),[0,bits(224),0]);assert.equal(arg(1),bits(500));return 0;},argc);
 m.replace(0x41d7e0,'combo-flush',()=>{event(15,players.indexOf(m.reg('ECX')-0x30410));return 0;});
 m.replace(0x422d20,'fade',()=>{event(16,m.reg('ECX'),m.reg('EDX'),arg(0),arg(4));assert.deepEqual([arg(1),arg(2),arg(3)],[0,0,35]);return 0;},5);
 m.replace(0x415d50,'HUD-fade',()=>{event(17,huds.indexOf(m.reg('ECX')));return 0;});
 m.replace(0x41b5c6,'restart',()=>{event(18);const id=m.i32(0x4a7e90)===0?m.i16(route+6):201;event(10,id,0);m.i32(scene+0xe94c,id);for(const off of [0x1095c,0x10960,0x10964,0x11ea8])m.u32(scene+off,0);return 0;});
 m.replace(0x40e3d0,'defeat-stat',()=>{event(19,arg(0));return 0;},1);
 for(let s=0;s<2;++s){m.u32(0x4a7d90+s*56,backgrounds[s]);m.u32(0x4a7d94+s*56,players[s]);m.u32(0x4a7da4+s*56,controllers[s]);m.u32(0x4a7da8+s*56,huds[s]);m.u32(0x4a7dac+s*56,scores[s]);}
 m.u32(0x4a7e80,route);m.u32(0x4a7e78,config);m.u32(0x4b36d4,0);m.u32(0x4a7e38,scene);
 const progressAddresses=[0x4a7e40,0x4a7e48,0x4a7e50,0x4a7e54,0x4a7e58,0x4a7e5c,0x4a7e60,0x4a8104,0x4a7e8c,0x4a7e90,0x4a80d8];
 function global(part,address,value){dv=new DataView(c.memory.buffer);dv.setInt32(p[part],value,true);m.i32(address,value);}
 function sceneField(off,value){const field=fields.find(a=>a[0]===off);assert.ok(field);dv.setInt32(p[0]+field[1],value,true);m.i32(scene+off,value);}
 function compare(label){
  dv=new DataView(c.memory.buffer);for(const [o,a,n] of fields)assert.deepEqual(memory(c,p[0]+a,n),m.bytes(scene+o,n),label+' scene '+o.toString(16));
  offsets.forEach((o,i)=>assert.deepEqual(memory(c,p[1]+i*0x2a4,0x2a4),m.bytes(scene+o,0x2a4),label+' animation '+i));
  for(let s=0;s<2;++s){assert.deepEqual(memory(c,p[4]+s*20,20),m.bytes(scores[s],20),label+' score');assert.equal(dv.getFloat32(p[13]+s*36+20,true),m.f32(scores[s]+52),label+' losses');assert.deepEqual(memory(c,p[16]+s*8,8),m.bytes(backgrounds[s]+28,8),label+' background');}
  progressAddresses.forEach((a,i)=>assert.equal(dv.getInt32(p[2]+i*4,true),m.i32(a),label+' progress '+i));
  for(const [n,a] of [[5,0x4a7ec4],[11,0x4a7df4],[15,scene+0xe94c],[17,0x4b3690]])assert.equal(dv.getInt32(p[n],true),m.i32(a),label+' part '+n);
  assert.deepEqual(memory(c,p[7],8),m.bytes(0x4a7e98,8));assert.equal(memory(c,p[18],1)[0],m.bytes(0x4a7ecd,1)[0]);assert.equal(memory(c,p[19],1)[0],m.bytes(0x4a7ece,1)[0]);
  assert.equal(c.match_scene_event_count(f),events.length,label+' event count '+JSON.stringify(events));events.forEach((e,i)=>assert.deepEqual(Array.from({length:8},(_,j)=>dv.getInt32(c.match_scene_events(f)+32*i+4*j,true)),e,label+' event '+i));++checks;
 }
 function op(n,side=0,x=0){events=[];const fn=[0x418480,0x4181e0,0x415d70,0x415e50,0x416080,0x417630,0x417c10][n],args=n===2?[side]:n===4?[side,position]:[];m.f32(position,x);m.f32(position+4,81);m.f32(position+8,3);m.call(fn,{ecx:scene,args});c.match_scene_op(f,n,side,x);compare(`op ${n}/${side}/${x}`);if(n===6)++draws;}
 try{for(let scenario=0;scenario<120;++scenario){
  f=c.match_scene_fixture();p=Array.from({length:21},(_,i)=>c.match_scene_part(f,i));dv=new DataView(c.memory.buffer);m.view(scene,0x1293c).fill(0);m.u32(scene+8,resource);for(const q of [...players,...scores,...backgrounds])m.view(q,q===players[0]||q===players[1]?0x31000:q===scores[0]||q===scores[1]?160:64).fill(0);
  global(3,0x4a7ea8,scenario%3);global(5,0x4a7ec4,[0,8,0x4000,0x1800][scenario%4]);global(6,0x4a7e94,scenario%3+1);global(8,config+0xac,scenario%2);global(9,0x4a7de8,scenario%16);global(10,0x4dc690,scenario%18-1);global(11,0x4a7df4,0);global(15,scene+0xe94c,0);global(17,0x4b3690,0);global(20,0x4a7eac,scenario%5);m.u32(0x4a7e98,0);m.u32(0x4a7e9c,0);m.write(0x4a7ecd,[0,0]);m.i32(0x4a7db0,scenario%14);
  const progress=[0,2,0,1800,16,0,scenario*3427,-1,scenario%3===2?9:Math.floor(scenario/3)%9,scenario%6,0];progress.forEach((v,i)=>{dv.setInt32(p[2]+i*4,v,true);m.i32(progressAddresses[i],v);});
  const routeData=new Int16Array([0,0,0,13,48,39,8,0]);m.write(route,new Uint8Array(routeData.buffer));memory(c,p[12],16).set(new Uint8Array(routeData.buffer));
  for(let s=0;s<2;++s){dv.setFloat32(p[4]+s*20,[0,.5,1,3,9.5,10][(scenario+s)%6],true);dv.setUint32(p[4]+s*20+8,1000+scenario*721,true);m.write(scores[s],memory(c,p[4]+s*20,20));const stats=[scenario%11,[0,73,999,1000][scenario%4],[0,66,67,80][(scenario+1)%4],[0,33,34,39][scenario%4],[0,33,34,45][(scenario+2)%4]];for(let i=0;i<5;++i){dv.setInt32(p[13]+s*36+i*4,stats[i],true);m.i32(players[s]+[0xa8,0x30418,0xb0,0xb4,0xb8][i],stats[i]);}dv.setFloat32(p[13]+s*36+24,31.5+s*5,true);m.f32(players[s]+0x1b88,31.5+s*5);
   const at=p[14]+s*20;dv.setUint32(at,32+s*304,true);dv.setUint32(at+4,16,true);dv.setFloat32(at+8,9.375,true);dv.setFloat32(at+12,3.75,true);dv.setFloat32(at+16,288,true);m.u32(0x4b3178+s*0xf0+0xcc,32+s*304);m.u32(0x4b3178+s*0xf0+0xd0,16);
  }m.f32(0x4a80e0,9.375);m.f32(0x4a80e4,3.75);op(0);
  for(let s=0;s<2;++s)for(const x of [-113,-112,-33.5,0,15,31.5,52.75,101,112,113])op(4,s,x);
  for(let t=0;t<12;++t){if(t===2){const field=fields.find(a=>a[0]===0xe93c);for(let s=0;s<2;++s){dv.setInt32(p[0]+field[1]+s*4,5+s,true);m.i32(scene+0xe93c+s*4,5+s);const color=fields.find(a=>a[0]===0xe934);dv.setUint32(p[0]+color[1]+s*4,0x8f345678+s,true);m.u32(scene+0xe934+s*4,0x8f345678+s);}}op(5);}
  op(2,scenario%2);for(let t=0;t<205;++t){op(5);if(t%31===0)op(6);}op(3);for(let t=0;t<24;++t){if(t===21)global(15,scene+0xe94c,-1);op(5);if(t===22)op(6);}op(1);
  c.match_scene_delete(f);f=0;
 }report('match-scene',{checks,draws,scenarios:120,scope:'Original front overlay initialization, boss arrows, round-end freezing/fades, life-based retries, CPU retry policy, result caps and scoring, next stage/game over/ending flags and draw order. Animation execution, dialogue, resource loading and world restart are explicit service boundaries.'});
 }finally{if(f)c.match_scene_delete(f);m.close();}
});
