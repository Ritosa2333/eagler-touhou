import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,root,report} from './helpers.mjs';

test('TH11 scene draw order matches original registrations and propagates draw failures',async()=>{
 const c=await core(),registrations=[];
 const layers=new Map([[0x455140,0],[0x455150,1],[0x455160,2],[0x455170,3],...Array.from({length:15},(_,i)=>[0x4551f0+i*16,i+4]),[0x455320,19],[0x455350,20],[0x455310,21],[0x455300,22],[0x4552f0,23],[0x4552e0,24],[0x4553f0,30],[0x4553a0,29]]);
 for(const [file,kind,callback] of [['004526f0',0,0],['0042f8a0',1,0x431c60],['00423270',2,0x4240d0],['00424970',3,0x424dd0],['00408450',4,0x408ef0],['0040bd00',5,0x40c640],['004024c0',14,0x403910],['004024c0',15,0x403920],['0041a3c0',16,0x41cfd0],['0041a3c0',17,0x41cfc0],['00401080',18,0x4014d0],['00401080',19,0x4014c0],['00437d50',20,0x438430]]){
  let priority=null,fn=null;
  for(const line of readFileSync(resolve(root,`artifacts/cpp/analysis/jp/functions/${file}.asm`),'utf8').split(/\r?\n/)){
   let m=line.match(/MOV EBX,0x([0-9a-f]+)/);if(m)priority=parseInt(m[1],16);
   m=line.match(/MOV dword ptr \[(?:EAX|ESI) \+ 0x8\],0x([0-9a-f]+)/);if(m)fn=parseInt(m[1],16);
   if(line.includes('CALL 0x00456c10')&&(kind?fn===callback:layers.has(fn)))registrations.push([priority,kind,kind?0:layers.get(fn)]);
  }
 }
 const composites=new Map([[0x4290e0,[6,0]],[0x429220,[7,0]],[0x4293d0,[8,27]],[0x4292b0,[9,0]],[0x429420,[10,28]],[0x429340,[11,0]],[0x429470,[12,0]],[0x4293a0,[13,0]]]);
 let priority=1,fn=0;
 for(const line of readFileSync(resolve(root,'artifacts/cpp/analysis/jp/functions/004298c0.asm'),'utf8').split(/\r?\n/)){
  let m=line.match(/MOV EBX,0x([0-9a-f]+)/);if(m)priority=parseInt(m[1],16);
  m=line.match(/MOV dword ptr \[(?:EAX|ESI) \+ 0x8\],0x([0-9a-f]+)/);if(m)fn=parseInt(m[1],16);
  if(line.includes('CALL 0x00456c10')&&composites.has(fn))registrations.push([priority,...composites.get(fn)]);
 }
 registrations.sort((a,b)=>a[0]-b[0]);
 const actual=Array.from({length:c.scene_draw_count()},(_,i)=>[0,1,2].map(k=>c.scene_draw_value(i,k)));
 assert.deepEqual(actual,registrations,'native callback priority order');
 const raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(raw.length),source=c.resources_create(),session=c.game_session_create();
 try{
  memory(c,data,raw.length).set(raw);assert.equal(c.resources_open(source,data,raw.length),1);assert.equal(c.game_session_begin(session,source,1,0,0,1,0),1);
  assert.equal(c.game_session_layer_probe(session,0),1);
  const drawn=new Uint32Array(c.memory.buffer,c.game_session_drawn_layers(),c.game_session_drawn_count());
  assert.deepEqual([...drawn],registrations.filter(p=>p[1]===0||p[1]===8||p[1]===10).map(p=>p[2]));
  assert.equal(c.game_session_layer_probe(session,1),0,'invalid sprite fails session draw');
  report('scene-schedule',{passed:true,registrations,scope:'Original registered stage/ANM/player/item/laser/bullet/spell/compositor priorities, title layer traversal and renderer failure propagation. Full gameplay and HUD require separate integration coverage.'});
 }finally{c.game_session_delete(session);c.resources_delete(source);c.release(data);}
});
