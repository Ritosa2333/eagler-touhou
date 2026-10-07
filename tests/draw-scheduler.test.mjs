import test from 'node:test';import assert from 'node:assert/strict';
import {animationLayerPriority,readCoreDrawSchedule,orderPresentation} from '../src/draw-scheduler.mjs';
test('native manager callbacks interleave lasers, enemy bullets, title and the two font draws',()=>{
  // Priorities:4095f9(Bullet31),435be3(Player24),4259d4(Item27),
  // Laser29;45dfa0(layer table);401154/4011a9(Font69/45).
  const schedule=new Map([[1,{drawPriority:31,order:0,clipped:true}],[2,{drawPriority:24,order:0,clipped:true}]]);
  const poses=[{label:'bullet',logical:true,id:1,a:{layer:0}},{label:'backdrop',a:{layer:3}},
    {label:'player',logical:true,id:2,a:{layer:0}},{label:'laser',mesh:true,renderPass:1,drawPriority:29,a:{layer:0}},
    {label:'ring',mesh:true,renderPass:0,drawPriority:0,a:{layer:14}},
    {label:'title',a:{layer:20}},{label:'bonus',drawPriority:45},{label:'frame',a:{layer:21}},
    {label:'ufo-panel',a:{layer:23}},{label:'ufo-counts',drawPriority:60},{label:'hud-number',drawPriority:69}];
  const ordered=orderPresentation(poses,schedule);
  assert.deepEqual(ordered.map(p=>p.label),['backdrop','player','ring','laser','bullet','title','bonus','frame','ufo-panel','ufo-counts','hud-number']);
  assert.equal(ordered.find(p=>p.label==='bullet').clipped,true);
  assert.equal(ordered.find(p=>p.label==='ufo-panel').clipped,false);
  assert.equal(animationLayerPriority(31),65);
  assert.deepEqual(orderPresentation([{a:{layer:26}},{a:{layer:27}}]),[]);
});
test('registered head insertion and secondary draw priority survive the separate schedule ABI',()=>{
  const HEAPU8=new Uint8Array(128),view=new DataView(HEAPU8.buffer);
  for(const [i,values]of [[0,[30,41,1,1]],[1,[31,41,0,1]],[2,[32,66,2,0]]])values.forEach((n,j)=>view.setInt32(16+i*16+j*4,n,true));
  const core={HEAPU8,_th12_anm_schedule_count:()=>3,_th12_anm_schedule_ptr:()=>16,_th12_anm_schedule_stride:()=>16};
  const schedule=readCoreDrawSchedule(core),before=HEAPU8.slice();
  const poses=[30,31,32].map(id=>({id,logical:true,a:{layer:19}}));
  assert.deepEqual(orderPresentation(poses,schedule).map(p=>p.id),[31,30,32]);assert.deepEqual(HEAPU8,before);
  assert.deepEqual(readCoreDrawSchedule({}),new Map());
  core._th12_anm_schedule_count=()=>20;assert.throws(()=>readCoreDrawSchedule(core),/顺序记录/);
  core._th12_anm_schedule_count=()=>3;view.setUint32(44,3,true);assert.throws(()=>readCoreDrawSchedule(core),/顺序字段/);
});
