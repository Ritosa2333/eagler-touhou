import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';

test('TH11 stage awards, clear records, mode branches and ending clock match original MSG/HUD code',async()=>{
 const c=await core(),m=await oracle();
 const hud=m.allocate(0x4460),control=m.allocate(0x80),replay=m.allocate(0x20),header=m.allocate(0x20),records=m.allocate(0x28000),msg=m.allocate(0xac),code=m.allocate(8),animation=m.allocate(4);
 for(const [a,p]of [[0x4a8d84,hud],[0x4a8e88,control],[0x4a8eb8,replay],[0x4a8ebc,records]])m.u32(a,p);
 m.u32(replay+0x18,header);m.write(code,[0,0,21,0,255,255,0,0]);m.u32(animation,123);
 let events=[];
 m.replace(0x455a00,'record stage-clear ANM creation',()=>{const sp=m.reg('ESP');assert.equal(m.u32(sp+8),72);assert.equal(m.u32(sp+12),22);events.push(1);return animation;},3);
 m.replace(0x41a0f0,'record option recall boundary',()=>{events.push(2);return 0;});
 m.replace(0x42d6b0,'record result-menu request boundary',()=>{events.push(12);return 0;});
 m.replace(0x42c760,'record replay-end request boundary',()=>{events.push(13);return 0;});
 m.replace(0x40dea0,'record next-stage request boundary',()=>{events.push(m.u32(0x4c3810)&0x2000?15:11);return 0;});
 m.replace(0x45fce4,'ending effect allocation',()=>m.allocate(0x44));
 m.replace(0x46a720,'memset',()=>{const sp=m.reg('ESP'),p=m.u32(sp+4);m.view(p,m.u32(sp+12)).fill(m.u32(sp+8)&255);return p;});
 m.replace(0x448a90,'record ending-fade request boundary',()=>{const sp=m.reg('ESP');assert.equal(m.u32(sp+8),5);assert.equal(m.u32(sp+12),200);assert.equal(m.u32(sp+24),67);events.push(3);return 0;},6);
 m.replace(0x41b436,'stop after original ending clock',()=>{m.reg('ESP',m.stack-8);return 0;});
 const map=[0x4a56e4,0x4a56e8,0x4a56f0,0x4a56f4,0x4a5718,0x4a571c,0x4a5720,0x4a5744,0x4a5748,0x4a574c,0x4a5754,0x4a576c];
 const view=()=>new DataView(c.memory.buffer),get=p=>view().getInt32(p,true),put=(p,v)=>view().setInt32(p,v,true);
 let cases=0,checks=0,frames=0;
 try{
  for(let st=1;st<=7;++st)for(let selection=0;selection<6;++selection)for(let difficulty=0;difficulty<5;++difficulty)for(let mode=0;mode<8;++mode){
   const f=c.completion_create(),e=c.completion_data(f,0),s=c.completion_data(f,1),r=c.completion_data(f,2),rate=c.completion_data(f,3);
   const controlMode=mode>=4?1:0,replayMode=mode===4?1:0,flags=(mode===1?1:0)|(mode===5?2:0)|((mode===3||mode===7)?4:0);
   const speed=[1,.375,1.005][selection%3],score=[0,999999998,999999999][selection%3],lives=[0,2,8,20,100,-1][selection],power=[0,100,400,99,10000,2147483647][selection];
   const count=[0,99998,99999,100000,-1,2147483647][selection];
   m.view(hud,0x4460).fill(0);m.view(records,0x28000).fill(0);m.view(msg,0xac).fill(0);
   const economy=[score,power,5001234,10000,lives,0,difficulty,0,400,100,0,10000000];
   for(let i=0;i<map.length;++i){put(e+i*4,economy[i]);m.i32(map[i],economy[i]);}
   m.i32(0x4a5710,Math.floor(selection/3));m.i32(0x4a5714,selection%3);m.i32(0x4a5728,st);m.u32(0x4a5758,flags&1?0x10:0);
   m.i32(control+0x74,controlMode);m.i32(replay+0x10,replayMode);m.view(header+10,1)[0]=flags&2?1:0;m.u32(0x4c3810,flags&4?0x2000:0);m.u32(0x4c37d8,0);
   m.f32(0x4a7948,speed);view().setFloat32(rate,speed,true);
   m.u32(msg+0x64,code);m.u32(msg+0x24,0x4a7948);m.u32(msg+0x28,1);m.u32(0x4c93c0,0);m.u32(0x4c93cc,0);
   m.i32(records+selection*0x68d4+0x598+difficulty*4,count);put(r+(selection*5+difficulty)*4,count);
   c.completion_configure(f,st,selection,difficulty,controlMode,replayMode,flags);
   function compare(label){
    assert.deepEqual(map.map((a,i)=>get(e+i*4)),map.map(a=>m.i32(a)),label+' economy');
    assert.equal(get(s)>>>0,m.u32(hud+0x4420),label+' HUD flags');
    assert.deepEqual([get(s+4),get(s+8),get(s+12),get(s+20)],[m.i32(hud+0x4424),m.i32(hud+0x4428),m.i32(hud+0x442c),m.i32(hud+0x4434)],label+' timer');
    assert.equal(get(s+24),m.i32(hud+0x4450),label+' displayed bonus');assert.equal(get(s+28),m.i32(hud+0x4454),label+' ending frame');
    assert.equal(get(r+(selection*5+difficulty)*4),m.i32(records+selection*0x68d4+0x598+difficulty*4),label+' clears');
    const i=difficulty*6+st;
    assert.deepEqual([...memory(c,r+120+(selection*32+i)*2,2)],[...m.bytes(records+selection*0x68d4+0x5a8+i*8,2)],label+' stage record');
    assert.deepEqual(Array.from({length:c.completion_events(f)},(_,i)=>get(c.completion_data(f,4)+i*4)),events,label+' ordered effects');checks+=8;
   }
   try{
    events=[];m.call(0x41d380,{args:[msg]});assert.equal(c.completion_finish(f),1);compare(`${st}/${selection}/${difficulty}/${mode}`);++cases;
    if(st===6&&difficulty===selection%5&&(mode===0||mode===3||mode===6)){
     for(let frame=0;frame<381;++frame){m.u32(0x4c37d8,0);m.call(0x41b380,{args:[hud]});if(m.u32(0x4c37d8))events.push(m.u32(0x4c37d8)===2?15:14);c.completion_tick(f);compare(`ending ${selection}/${mode}/${frame}`);++frames;}
    }
   }finally{c.completion_delete(f);}
  }
  report('stage-completion',{passed:true,cases,checks,frames,scope:'Original MSG 21 and award helpers: signed score overflow/cap, all stage/selection/difficulty branches, practice/replay modes, clear records; original HUD ending clock and fade request at normal/slow rates. ANM allocation, option recall and scene/menu requests are recorded integration boundaries.'});
 }finally{m.close();}
});
