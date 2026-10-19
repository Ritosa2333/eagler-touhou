import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,root,report} from './helpers.mjs';

test('TH11 internal scene layer traversal propagates invalid sprite failures',async()=>{
 const c=await core(),actual=Array.from({length:c.scene_draw_count()},(_,i)=>[0,1,2].map(k=>c.scene_draw_value(i,k)));
 assert.ok(actual.length>0,'scene draw schedule is populated');
 let lastPriority=-1;
 for(const [priority,kind,layer] of actual){
  assert.ok(Number.isInteger(priority)&&priority>lastPriority,'scene draw priorities are strictly increasing');
  assert.ok(Number.isInteger(kind)&&kind>=0&&kind<=20,'scene draw kind is recognized');
  assert.ok(Number.isInteger(layer)&&layer>=0&&layer<=30,'scene layer index is valid');
  lastPriority=priority;
 }
 const raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(raw.length),source=c.resources_create(),session=c.game_session_create();
 try{
  memory(c,data,raw.length).set(raw);assert.equal(c.resources_open(source,data,raw.length),1);assert.equal(c.game_session_begin(session,source,1,0,0,1,0),1);
  assert.equal(c.game_session_layer_probe(session,0),1);
  const drawn=new Uint32Array(c.memory.buffer,c.game_session_drawn_layers(),c.game_session_drawn_count()),drawnLayers=[...drawn];
  const scheduledLayers=actual.filter(([,kind])=>kind===0||kind===8||kind===10).map(([,kind,layer])=>layer);
  assert.deepEqual(drawnLayers,scheduledLayers,'session traversal follows its registered animation and composite layers');
  assert.equal(c.game_session_layer_probe(session,1),0,'invalid sprite fails session draw');
  report('scene-schedule-internal',{passed:true,drawnLayers,scope:'DAT-backed internal scene layer traversal and invalid-sprite failure propagation.'});
 }finally{c.game_session_delete(session);c.resources_delete(source);c.release(data);}
});
