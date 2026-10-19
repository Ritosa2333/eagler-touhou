import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,root,report} from './helpers.mjs';

test('TH11 live session advances and renders all seven STD backgrounds with shared cameras',async()=>{
 const c=await core(),raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
 const data=c.allocate(raw.length),source=c.resources_create(),session=c.game_session_create(),render=c.comp_create();
 const view=()=>new DataView(c.memory.buffer),get=p=>view().getUint32(p,true),bytes=(p,n)=>Buffer.from(memory(c,p,n));
 const error=()=>{const b=memory(c,c.game_session_error(session),512);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
 const draw=()=>assert.equal(c.game_session_render(session,render),1,error());
 const step=()=>assert.equal(c.game_session_update(session,0,0,0,0,0,0),1,error());
 let frames=0,submissions=0;
 memory(c,data,raw.length).set(raw);assert.equal(c.resources_open(source,data,raw.length),1);
 try{
  for(let number=1;number<=7;++number){
   assert.equal(c.game_session_begin(session,source,number,0,0,number===7?4:1,0),1,error());
   const s=c.game_session_stage_data(session,0),camera=c.game_session_stage_data(session,2),play=c.game_session_stage_data(session,1);
   assert.deepEqual(Array.from({length:4},(_,i)=>get(play+0xcc+i*4)),[32,16,384,448]);
   assert.deepEqual(Array.from({length:4},(_,i)=>get(camera+0xcc+i*4)),[13,0,422,480]);
   assert.equal(get(s+0x3008),number);
   let visible=0;
   for(let frame=0;frame<120;++frame){
    step();++frames;
    assert.equal(get(s+0x300c),frame+1,'stage updates at live frame boundary');
    assert.deepEqual(bytes(camera,0x118),bytes(s+0x3040,0x118),'stage publishes camera before enemies and ANM');
    for(const [kind,offset,owner]of[[3,0,camera],[4,0xf0,camera],[5,0,play],[6,0xf0,play]])assert.deepEqual(bytes(c.game_session_stage_data(session,kind),12),bytes(owner+offset,12));
    assert.deepEqual(bytes(c.game_session_stage_data(session,3)+12,12),bytes(camera+0x24,12));
    draw();visible+=get(s+0x2fec);submissions+=c.comp_size(render,2);
    assert.equal(get(camera+0xe8),0,'frame end clears camera shake');
    assert.equal(get(camera+0xec),0);
   }
   assert.ok(visible>0,`stage ${number} submits background primitives`);
   const saved=bytes(s,0x3158);assert.equal(c.game_session_pause(session),1);step();assert.deepEqual(bytes(s,0x3158),saved,'pause freezes stage update');assert.equal(c.game_session_resume(session),1);
   assert.equal(c.game_session_stage_action(session,0,0),1);draw();assert.equal(get(s+0x2fec),0,'spell background hides STD objects');
   assert.equal(c.game_session_stage_action(session,0,1),1);assert.equal(c.game_session_stage_action(session,1,0xff506070),1);assert.equal(get(s+0x2328),0xff506070,'Bomb writes live stage tint');
   step();assert.equal(get(s+0x2328),0x00808080,'next stage tick resets transient tint');draw();
   assert.equal(c.game_session_stage_action(session,3,0),1);
   for(let frame=0;frame<30;++frame){step();draw();++frames;}
   assert.ok(get(s+0x2ff0)&8,'outgoing background ends after its draw timer');
   assert.equal(c.game_session_title(session),1);assert.equal(c.game_session_stage_data(session,0),0);draw();
  }
  report('stage-integration',{passed:true,stages:7,frames,submissions,scope:'Live archive-backed session: STD ownership, update/draw scheduling, published camera and enemy/ANM inputs, original viewport roles, pause, spell visibility, Bomb tint, outgoing fade and title teardown. Not an original full-game replay or GPU pixel comparison.'});
 }finally{c.comp_delete(render);c.game_session_delete(session);c.resources_delete(source);c.release(data);}
});
