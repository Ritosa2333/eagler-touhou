import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root,bits} from './helpers.mjs';
import {installAnm,normalizedAnm} from './anm-oracle.mjs';
test('HUD uses original animation resources for score, life, charge, notices, portraits and round wipes',async()=>{
 const m=await oracle(),c=await core(),raw=readFileSync(resolve(root,'reference/assets/front.anm')),data=c.allocate(raw.length),input=c.allocate(56);memory(c,data,raw.length).set(raw);
 const original=installAnm(m,raw,10),hud=m.allocate(0xac00),player=m.allocate(0x31000),score=m.allocate(32),device=m.allocate(4),vt=m.allocate(0x160),arg=i=>m.u32(m.reg('ESP')+4+i*4);
 let f=0,checks=0,draws=[],vertices=[];
 m.replace(0x401660,'hud-resource',()=>original.file,1);
 m.replace(0x403d90,'hud-view',()=>{m.u32(0x4b3448,0x4b3178+arg(0)*0xf0);return 0;},1);
 const index=p=>p>=hud+0xa684?63+(p-hud-0xa684)/676:(p-hud)/676;
 for(const [a,rotated] of [[0x43ab50,1],[0x43a950,0]])m.replace(a,'hud-sprite',()=>{draws.push([index(arg(0)),rotated]);return 0;},1);
 for(const a of [0x4396a0,0x40e380,0x415d00,0x40e370,0x415d10])m.replace(a,'hud-state',()=>0);
 m.replace(0x42ff40,'hud-depth',()=>0,2);m.u32(0x4b3108,device);m.u32(device,vt);
 for(const [off,argc] of [[0xfc,4],[0x130,2]])m.u32(vt+off,m.registerImport({dll:'hud',name:'state',argc,handler:()=>0}));
 m.u32(vt+0x120,m.registerImport({dll:'hud',name:'triangle',argc:5,handler:()=>{vertices.push(...m.bytes(arg(3),60));return 0;}}));
 const operation=(type,value=0)=>{c.hud_operation(f,type,value);if(type===10)m.call(0x401560,{ecx:original.file,args:[hud+0xa684+(value&1)*676,7+value]});else {const a=[0x41a210,0x41a270,0x41a2d0,0x41a320,0x41a380,0x41a3e0,0x418950,0x41a3c0,0x415d50,0x41a230][type];m.call(a,{ecx:hud,args:[3,4,5,6].includes(type)?[value]:[]});}};
 function compare(label){const base=c.hud_animation(f,0),file=c.hud_resource(f,0),source=c.hud_resource(f,1),sprites=c.hud_resource(f,2);
  for(let n=0;n<65;++n){const p=n<63?hud+n*676:hud+0xa684+(n-63)*676;assert.deepEqual(normalizedAnm(memory(c,c.hud_animation(f,n),676),file,source,sprites),normalizedAnm(m.bytes(p,676),original.file,original.source,original.sprites),label+' animation'+n);}
  assert.equal(c.hud_value(f,0),m.i32(hud+0xa65c),label+' charge flag');assert.equal(c.hud_value(f,1),m.i32(hud+0xabcc),label+' wipe flag');assert.deepEqual(memory(c,c.hud_part(f,0),12),m.bytes(hud+0xabd0,12),label+' wipe timer');assert.deepEqual(memory(c,c.hud_part(f,1),12),m.bytes(hud+0xabdc,12),label+' blink timer');assert.deepEqual(memory(c,c.hud_part(f,2),8),m.bytes(0x4ace0c,8),label+' random');++checks;
 }
 try{for(const versus of [0,1])for(let side=0;side<2;++side){
  if(f)c.hud_delete(f);f=c.hud_fixture();m.view(hud,0xac00).fill(0);m.view(player,0x31000).fill(0);m.view(score,32).fill(0);const own=0x4a7d90+side*0x38;m.u32(own+4,player);m.u32(own+28,score);m.u32(own+32,0);m.u32(hud+0xa66c,side);m.u32(hud+0xa670,own);m.u32(0x4a7ea8,versus?2:0);m.u32(0x4a7e8c,0);m.u32(0x4b36d4,0);m.f32(0x4b36b8,1);m.f32(0x4a80e0,-144);m.f32(0x4a80e4,0);m.u32(0x4b3178+side*0xf0+0xcc,16+side*320);m.u32(0x4b3178+side*0xf0+0xd0,16);m.u32(0x4ace0c,0x7513);m.u32(0x4ace10,0);
  assert.equal(c.hud_initialize(f,data,raw.length,side,versus),1);assert.equal(m.call(0x41a000,{ecx:hud}),0);compare('init');
  for(let frame=0;frame<150;++frame){
   const v=[frame%11,(frame*17)%1300,(frame*197)%999999,(frame*1357901)%999999999,frame%8,frame%17,(frame*3)%17,bits((frame*13)%420),bits((frame*7)%430),bits((frame%7)+.5),frame%110,bits(frame%106),(bits(frame%100-50.25)),bits(300.125+frame%80)];memory(c,input,56).set(new Uint8Array(new Uint32Array(v).buffer));c.hud_input(f,input);
   for(const [off,n] of [[0xa8,0],[0x30414,1],[0x30420,2],[0xa0,5],[0xa4,6],[0x30384,7],[0x30388,8],[0x1b88,12],[0x1b8c,13]])m.u32(player+off,v[n]);m.u32(score+4,v[3]);m.u32(score,v[9]);m.u32(0x4a7e98+side*4,v[4]);m.call(0x401500,{ecx:player+0x30430,args:[v[10]]});m.u32(player+0x30428,v[11]);
   if(frame%30===0)operation(0);if(frame%40===3)operation(1);if(frame%40===20)operation(2);if(frame%29===5)operation(3,1+frame%4);if(frame%37===10)operation(4,frame%2);if(frame===8)operation(5,12345);if(frame>=9&&frame<28)operation(6,12345-frame*150);if(frame===29)operation(6,-1);if(frame===35)operation(7);if(frame===50)operation(8);if(frame===76)operation(9);if(frame%33===4)operation(10,frame%2);
   c.hud_update(f);m.call(0x418a90,{ecx:hud});compare(`${versus}/${side}/${frame}`);draws=[];vertices=[];c.hud_draw(f);m.call(0x4193e0,{ecx:hud});compare('draw '+frame);
   const values=new Int32Array(c.memory.buffer,c.hud_draws(f),c.hud_draw_count(f)*2);assert.deepEqual(Array.from({length:values.length/2},(_,n)=>[values[n*2],values[n*2+1]]),draws,'ordered HUD sprites');assert.deepEqual(memory(c,c.hud_vertices(f),c.hud_vertex_count(f)*20),Uint8Array.from(vertices),'wipe geometry');
  }
 }report('hud',{checks,frames:600,scope:'Original front.anm execution, all HUD animation slots, charge/score/life/survival notices, both portraits, draws and four round-wipe triangles. Final rasterization is a platform boundary.'});
 }finally{if(f)c.hud_delete(f);c.release(input);c.release(data);m.close();}
});
