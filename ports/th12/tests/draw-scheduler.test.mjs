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
test('owned capture wrappers reuse storage while published endpoints remain independent',()=>{
  const source=[{id:1,logical:true,order:8,a:{layer:0}},{a:{layer:26}},{a:{layer:21}}];
  const schedule=new Map([[1,{drawPriority:31,clip:2}]]),before=structuredClone(source);
  const first=orderPresentation(source,schedule);
  assert.deepEqual(source,before,'ordinary callers retain their input');
  const fresh=structuredClone(source),second=orderPresentation(fresh,schedule,{reusePoses:true});
  assert.deepEqual(second,first);assert.equal(second[0],fresh[0]);
  second[0].order=100;assert.equal(first[0].order,8);
  assert.deepEqual(source,before);
});
test('dense layers retain native priority, insertion order and stable ties',()=>{
  const poses=Array.from({length:1200},(_,index)=>({id:index,drawPriority:[24,31,69,41,24][index%5],order:index%17===0?1200-index:index}));
  const expected=poses.map((pose,index)=>({...pose,index})).sort((a,b)=>a.drawPriority-b.drawPriority||a.order-b.order||a.index-b.index);
  assert.deepEqual(orderPresentation(poses).map(p=>p.id),expected.map(p=>p.id));
  poses[601].drawPriority=101;
  const fallback=poses.map((pose,index)=>({...pose,index})).sort((a,b)=>a.drawPriority-b.drawPriority||a.order-b.order||a.index-b.index);
  assert.deepEqual(orderPresentation(poses).map(p=>p.id),fallback.map(p=>p.id));
});


test('native scheduling on fresh wrappers preserves endpoints across VM births, retirement and changing clips',()=>{
  const HEAPU8=new Uint8Array(256),view=new DataView(HEAPU8.buffer);let count=0;
  const core={HEAPU8,_th12_anm_schedule_count:()=>count,_th12_anm_schedule_ptr:()=>16,_th12_anm_schedule_stride:()=>16};
  const write=rows=>{count=rows.length;for(const [i,row]of rows.entries())row.forEach((n,j)=>view.setInt32(16+i*16+j*4,n,true));};
  const buckets=[],make=ids=>ids.map(id=>({id,logical:true,a:{layer:0}}));
  write([[1,31,10,2],[2,24,0,1]]);
  const poses=make([1,2]),first=orderPresentation(poses,readCoreDrawSchedule(core,poses),{buckets,reusePoses:true});
  const saved=structuredClone(first);
  write([[1,69,0,0],[3,24,0,2]]);
  const fresh=make([1,3]),next=orderPresentation(fresh,readCoreDrawSchedule(core,fresh),{buckets,reusePoses:true});
  assert.deepEqual(next.map(p=>[p.id,p.clip]),[[3,2],[1,0]]);
  assert.deepEqual(first,saved,'new captures and reused buckets cannot mutate published endpoints');
  assert.deepEqual(next,orderPresentation(make([1,3]),readCoreDrawSchedule(core)));
  // Schedule-only custom geometry and nonmatching producer order still use explicit ID lookup.
  write([[2,31,9,2],[9,29,1,1],[1,24,0,0]]);
  const mixed=make([1,2]);mixed.push({id:9,mesh:true,renderPass:0,a:{layer:0}});
  const expected=orderPresentation(structuredClone(mixed),readCoreDrawSchedule(core));
  assert.deepEqual(orderPresentation(mixed,readCoreDrawSchedule(core,mixed),{buckets,reusePoses:true}),expected);
  write([[1,69,0,0],[1,24,0,2]]);assert.throws(()=>readCoreDrawSchedule(core,make([1])),/顺序字段/);
});

test('reused draw buckets clear both prior sort flags and fallback contents',()=>{
  const buckets=[],run=poses=>orderPresentation(poses,new Map(),{buckets});
  const old=run([{id:1,drawPriority:31,order:2},{id:2,drawPriority:31,order:1}]);
  assert.deepEqual(old.map(p=>p.id),[2,1]);
  assert.deepEqual(run([{id:3,drawPriority:31,order:0},{id:4,drawPriority:101,order:0}]).map(p=>p.id),[3,4]);
  assert.deepEqual(run([{id:5,drawPriority:24,order:0}]).map(p=>p.id),[5]);
  assert.deepEqual(run([]),[]);assert.deepEqual(old.map(p=>p.id),[2,1]);
});
