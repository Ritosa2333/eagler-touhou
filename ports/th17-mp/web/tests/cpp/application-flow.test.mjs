import test from'node:test';import assert from'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
test('TH17 scene dispatcher preserves native title/game/replay/restart/next-stage/ending/results destinations and release order',async()=>{
 const c=await core(),m=await oracle(),owner=m.allocate(0xa50),game=m.allocate(0xb0),f=c.app_flow_create();let events=[],available=true,cases=0;
 for(const[at,id]of[[0x441670,2],[0x431310,4],[0x451420,5],[0x41c370,6],[0x451210,8],[0x41c310,9]])m.replace(at,'scene lifetime platform boundary',()=>{events.push(id);return 0;});
 m.replace(0x440a30,'initial loading owner',()=>{events.push(1);return available?0x22334455:0;});m.replace(0x404e20,'platform shutdown delay',()=>{events.push(3,m.i32(m.reg('ESP')+4));return 0;},1);m.replace(0x431240,'game scene construction boundary',()=>{events.push(7,m.reg('ECX'));return game;});
 try{for(const current of[-2,0,1,2,3,4,7,15,16])for(const pending of[0,1,2,3,4,7,10,11,12,13,14,15,16,17,19,22])for(let mode=0;mode<4;mode++){
  const fresh=mode%2,retry=mode>>1,stage=6,initial=mode%2?7:1,replay=mode>>1,reason=mode;available=mode!==3;
  m.view(owner,0xa50).fill(0);m.i32(owner+0x6f0,current);m.i32(owner+0x6f4,pending);m.i32(owner+0x6f8,-2);m.i32(owner+0x700,fresh);m.i32(owner+0x704,retry);m.i32(0x4b59dc,stage);m.i32(0x4b59e0,initial);m.i32(0x4b77ec,reason);m.u32(0x4b76b0,game);m.i32(game+0xa8,replay);c.app_flow_configure(f,current,pending,fresh,retry,stage,initial,replay,available?1:0,reason);
  events=[];const result=m.call(0x4427d0,{ecx:owner}),label=current+'→'+pending+'/'+mode;assert.equal(c.app_flow_update(f),result,label+' result');assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.app_flow_events(f),c.app_flow_count(f))),events,label+' lifecycle');
  const values=[m.i32(owner+0x6f0),m.i32(owner+0x6f4),m.i32(owner+0x6f8),m.i32(owner+0x700),m.i32(owner+0x704),m.i32(owner+0xa40),m.i32(0x4b59dc),m.i32(0x4b59e0),m.i32(0x4b77ec)];assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.app_flow_values(f),9)),values,label+' state');cases++;
 }
 report('application-flow',{passed:true,cases,originalFunctions:['0x4427d0'],scope:'All original dispatcher branches, same-destination no-op, loading success/failure, resource release order, fresh/retained runs, replay retry, initial-stage restore, Extra game-over, ending and result return reason. Scene factories/destructors, startup loading and shutdown are explicit platform boundaries; game logic within those owners is verified separately.'});
 }finally{c.app_flow_delete(f);m.close();}
});
