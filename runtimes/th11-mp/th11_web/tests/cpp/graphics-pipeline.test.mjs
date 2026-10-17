import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,report} from './helpers.mjs';

test('TH11 initial graphics pipeline matches original 447270 device states',async()=>{
 const c=await core(),m=await oracle(),f=c.render_create(),device=m.allocate(4),table=m.allocate(0x180);
 const states=new Map(),combiners=new Map(),samplers=new Map();
 const arg=n=>m.u32(m.reg('ESP')+n*4);
 const hook=(offset,name,count,handler)=>m.u32(table+offset,m.registerImport({dll:'pipeline-oracle',name,argc:count,handler}));
 m.u32(device,table);m.u32(0x4c3288,device);m.u32(0x4c3268,0);
 hook(0xe4,'state',3,()=>{states.set(arg(2),arg(3));return 0;});
 hook(0x10c,'combiner',4,()=>{assert.equal(arg(2),0);combiners.set(arg(3),arg(4));return 0;});
 hook(0x114,'sampler',4,()=>{assert.equal(arg(2),0);samplers.set(arg(3),arg(4));return 0;});
 try{
  m.call(0x447270);c.render_initialize_pipeline(f);
  const s=(k,fallback)=>states.get(k)??fallback,t=k=>combiners.get(k),p=k=>samplers.get(k);
  // Translate the captured original states into the shared renderer's semantic
  // enums. Defaults below are device creation defaults, not C++ output values.
  const expected=[s(7),s(14,1),s(27),s(15),s(28),s(48,0),s(26,0),s(23)-1,s(25)-1,s(19)-1,s(20)-1,s(171,1)-1,s(22)-1,s(168,15),s(24),s(34),s(140),s(36),s(37),s(38),t(1)-1,t(4)-1,t(2)-1,t(5)-1,t(3)-1,t(6)-1,Number(t(24)!==0),p(6)-1,p(5)-1,p(1)-1,p(2)-1];
  const actual=expected.map((_,i)=>c.render_pipeline_value(f,i)>>>0);
  assert.deepEqual(actual,expected,'native fixed-function initialization');
  assert.equal(s(137),0);assert.equal(s(9),2);assert.equal(s(35),0);assert.equal(s(161),0);assert.equal(t(11),0);assert.equal(p(7),0);assert.equal(p(3),3);
  report('graphics-pipeline',{passed:true,properties:expected.length,renderStates:[...states],textureStageStates:[...combiners],samplerStates:[...samplers],scope:'Actual native 447270 initialization mapped to shared SDL pipeline; covers blend, alpha testing, combiners, depth, fog, UV transformation and sampling. Does not assert final raster pixels.'});
 }finally{c.render_delete(f);m.close();}
});
