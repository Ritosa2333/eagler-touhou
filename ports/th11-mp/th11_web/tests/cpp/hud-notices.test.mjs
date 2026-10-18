import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
test('TH11 spell and power notices use the original ANM digits and lifecycle',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),f=c.hud_create(),front=o.load('front.anm'),ascii=o.load('ascii.anm',true),hud=m.allocate(0x4460),am=m.allocate(0x18500);
 m.u32(front.original,5);m.u32(ascii.original,2);m.u32(hud+0x444c,front.original);m.u32(am+0x184ac,ascii.original);m.u32(0x4a8d58,am);o.reset();
 for(const [n,file]of [front,ascii].entries()){const p=c.allocate(file.source.length);memory(c,p,file.source.length).set(file.source);assert.equal(c.hud_load(f,n,p,file.source.length),1);c.release(p);}
 const manager=c.hud_data(f,12),dv=()=>new DataView(c.memory.buffer);for(let n=0;n<2;++n){memory(c,c.anm_env_rng(manager,n),8).fill(0);dv().setUint16(c.anm_env_rng(manager,n),12345,true);}
 let checks=0;
 try{for(let sample=0;sample<210;++sample){const kind=sample%7,score=[0,1,10,12340,23000000,99999999][Math.floor(sample/7)%6];
  assert.equal(c.hud_notice(f,kind,score),1);m.reg('EAX',kind);m.reg('EBX',hud);m.call(0x41f010,{args:[score]});
  for(let i=0;i<12;++i){const address=i<8?hud+0x4350+i*4:i===8?hud+0x4370:i===9?hud+0x4374:i===10?hud+0x43bc:hud+0x43c0;assert.equal(c.hud_notice_handle(f,i),m.u32(address),`${sample} handle ${i}`);}
  const compare=()=>{const nodes=o.states();assert.equal(c.anm_manager_count(manager),nodes.length);for(const {id,p}of nodes){const cp=c.anm_manager_find(manager,id);assert.ok(cp);for(const off of [0x20,0x60,0x374,0x378,0x39c,0x3a0,0x404,0x408])assert.equal(dv().getUint32(cp+off,true),m.u32(p+off),`${sample} vm${id} +${off.toString(16)}`);++checks;}};
  compare();for(let i=0;i<3;++i){assert.equal(c.anm_manager_update(manager,0),1);o.update(false);compare();}
 }
 report('hud-notices',{passed:true,checks,requests:210,scope:'Native 41f010 seven notice cases, real front/ascii ANM, zero/leading digits, interruption and timed retirement; spell elapsed text is separate.'});
 }finally{c.hud_delete(f);m.close();}
});
