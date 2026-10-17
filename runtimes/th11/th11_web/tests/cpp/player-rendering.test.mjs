import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {normalizeAnimation,firstDifference} from './shot-oracle.mjs';
test('TH11 player draw loop matches original screen vertices and death visibility',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m);let checks=0,vertices=0,draws=[];
 const camera=m.allocate(0x100),device=m.allocate(4),vtable=m.allocate(0x180),batch=m.allocate(4096*168),np=m.allocate(0x8d40);
 m.u32(0x4c3288,device);m.u32(device,vtable);m.u32(0x4c37cc,camera);m.u32(camera+0xd4,640);m.u32(camera+0xd8,480);
 const arg=n=>m.u32(m.reg('ESP')+n*4),hook=(off,argc,handler)=>m.u32(vtable+off,m.registerImport({dll:'player-render-oracle',name:'GPU boundary '+off,argc,handler}));
 for(const [off,argc]of [[0xe4,3],[0x104,3],[0x10c,4],[0x164,2],[0x114,4]])hook(off,argc,()=>0);
 hook(0x14c,5,()=>{assert.equal(arg(2),4);assert.equal(arg(5),28);draws.push(Buffer.from(m.bytes(arg(4),arg(3)*3*arg(5))));return 0;});
 const dv=()=>new DataView(c.memory.buffer),read=p=>dv().getUint32(p,true),put=(p,v)=>dv().setUint32(p,v,true),flt=(p,v)=>dv().setFloat32(p,v,true);
 try{for(const name of ['pl00.anm','pl01.anm']){
  const {source,original}=o.load(name,true),ad=c.allocate(source.length),a=c.anm_create();memory(c,ad,source.length).set(source);assert.equal(c.anm_open(a,ad,source.length),1);
  const sht=c.sht_create(),manager=c.anm_manager_create(),f=c.pf_create(sht,a,a,manager),render=c.render_create(),s=c.pf_pointer(f,0),state=c.pf_pointer(f,1),body=c.pf_pointer(f,2);
  try{for(const script of [0,1,2,3,4])for(const life of [0,1,2,3,4])for(const flash of [0,1])for(const [x,y]of [[0,400],[-191.875,12.125],[183.875,432],[-260,-30]]){
   m.view(np,0x8d40).fill(0);m.reg('ESI',np+0x14);m.call(0x401fd0);m.reg('EAX',np+0x14);m.reg('EDI',original);m.reg('EBX',script);m.call(0x44acd0);
   memory(c,body,0x434).fill(0);c.anm_vm_init(body);assert.equal(c.anm_vm_bind(body,a,script,manager),1);assert.ok(c.anm_vm_update(body,manager)>=0);
   flt(s,x);flt(s+4,y);m.f32(np+0x87c,x);m.f32(np+0x880,y);put(state,life);m.i32(np+0x928,life);
   const flag=(read(body+0x404)&~0x8000)|(flash?0x8000:0);put(body+0x404,flag);m.u32(np+0x418,flag);put(body+0x378,0xff0000ff);m.u32(np+0x38c,0xff0000ff);
   draws=[];m.u32(o.manager+0x435620,0);m.u32(o.manager+0x7b5624,batch);m.u32(o.manager+0x7b5628,batch);m.u32(o.manager+0x4355bc,0);for(const off of [0x4355c0,0x4355c2,0x4355c6])m.view(o.manager+off,1)[0]=255;
   for(let i=0;i<4;++i)m.f32(0x4c91d8+i*28+12,1);m.reg('EAX',np);assert.equal(m.call(0x431bc0),1);m.reg('ESI',o.manager);m.call(0x44fd10);
   c.render_reset(render);assert.equal(c.pf_draw(f,render),1);c.render_flush(render);
   const actual=Buffer.from(memory(c,c.render_data(render),c.render_size(render)));assert.deepEqual(actual,Buffer.concat(draws),`${name} script${script} state${life} flash${flash} ${x}/${y} vertices`);vertices+=actual.length/28;
   assert.equal(firstDifference(normalizeAnimation(memory(c,body,0x434),read,read(body+0x3a4)),normalizeAnimation(m.bytes(np+0x14,0x434),p=>m.u32(p),m.u32(np+0x3b8))),'','body draw mutation');++checks;
  }}finally{c.render_delete(render);c.pf_delete(f);c.anm_manager_delete(manager);c.sht_delete(sht);c.anm_delete(a);c.release(ad);}
 }report('player-rendering',{passed:true,checks,vertices,scope:'Original 431bc0 and real ANM screen renderer; Reimu/Marisa body scripts 0..4, all five life states, invincibility tint and clipping. Exact ordered vertex bytes and animation draw mutations. Shipped option draw callbacks are unset; separate option animations use the ANM manager.'});
 }finally{m.close();}
});
