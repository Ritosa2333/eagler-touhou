import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
import {firstDifference} from './shot-oracle.mjs';
test('TH11 damage-area creation, movement, lifetime, geometry and damage budgets match original code',async()=>{
 const c=await core(),m=await oracle();animationOracle(m);
 const sht=c.sht_create(),a=c.anm_create(),manager=c.anm_manager_create(),f=c.sm_create(sht,a,manager),cp=c.sm_areas(f),rate=c.anm_env_rate(manager),economy=c.sm_economy(f),ct=c.allocate(20);
 const np=m.allocate(0x8d40),special=m.allocate(0x100),target=m.allocate(12),size=m.allocate(8);
 m.view(np,0x8d40).fill(0);m.view(special,0x100).fill(0);m.u32(0x4a8eb4,np);m.u32(0x4a8d64,special);m.u32(np+0x948,1);
 m.replace(0x431658,'return at the original damage-area update boundary',()=>0);
 const dv=()=>new DataView(c.memory.buffer),put=(p,v)=>dv().setInt32(p,v,true),flt=(p,v)=>dv().setFloat32(p,v,true),read=p=>dv().getUint32(p,true);
 const bits=n=>{const b=Buffer.alloc(4);b.writeFloatLE(n);return b.readUInt32LE();};
 let states=0,frames=0,spawns=0,hits=0;
 function compare(label){for(let i=0;i<32;++i){const a=Buffer.from(m.bytes(np+0x7c9c+i*0x74,0x74)),b=Buffer.from(memory(c,cp+i*0x74,0x74));a.writeUInt32LE(0,0x58);b.writeUInt32LE(0,0x58);assert.equal(firstDifference(b,a),'',label+` area${i}`);++states;}}
 function hit(x,y,w,h,label){flt(ct,x);flt(ct+4,y);flt(ct+8,0);flt(ct+12,w);flt(ct+16,h);m.write(target,memory(c,ct,12));m.write(size,memory(c,ct+12,8));const expected=m.call(0x4347f0,{args:[target,size]})|0;assert.equal(c.sm_damage(f,ct,ct+12,1),expected,label);assert.equal(read(economy),m.u32(0x4a56e4),label+' score');compare(label);++hits;}
 try{
  for(const speed of [0,.25,.5,.99,1,1.001,1.01,1.5,2]){
   memory(c,cp,32*0x74).fill(0);m.view(np+0x7c9c,32*0x74).fill(0);flt(rate,speed);m.f32(0x4a7948,speed);put(economy,987654);m.i32(0x4a56e4,987654);
   for(let i=0;i<33;++i){flt(ct,(i%8)*21.375-70);flt(ct+4,110+Math.floor(i/8)*40.125);flt(ct+8,.1);m.write(target,memory(c,ct,12));m.reg('EAX',np);
    const native=m.call(0x433a90,{args:[target,bits(5+i),bits((i%5-2)*.75),13+i,i*3+1]});const actual=c.sm_area_circle(f,ct,5+i,(i%5-2)*.75,13+i,i*3+1,rate);
    assert.equal(actual?actual-cp:32*0x74,native-(np+0x7c9c));++spawns;
    if(i===32)break;const q=cp+i*0x74;
    // Circular, axis-aligned and rotated rectangles, linear and orbital motion.
    put(q+0x70,i%3===0?3:1);flt(q+8,i%3===2?.7853981852531433:0);flt(q+12,i%4===0?.03125:0);flt(q+16,19+i);flt(q+20,12+i*.5);
    flt(q+0x30,.3+i*.07);flt(q+0x34,-1.2+i*.025);flt(q+0x38,3+i*.5);flt(q+0x3c,.125);flt(q+0x40,.25);flt(q+0x44,.75);put(q+0x48,i%4===1?1:i%4===2?3:0);
    put(q+0x68,40+i);put(q+0x6c,1+i%7);const b=Buffer.from(memory(c,q,0x74));b.writeUInt32LE(0x4a7948,0x58);m.write(np+0x7c9c+i*0x74,b);
   }compare('spawn '+speed);
   for(let frame=0;frame<80;++frame){m.resetThreadFPU();m.reg('ESI',np);m.call(0x43155b);c.sm_areas_update(f);compare(`rate${speed} frame${frame}`);++frames;
    hit(Math.sin(frame*.1)*70,110+frame*.9,16,24,`moving target ${speed}/${frame}`);
   }
  }
  // Single-area edge cases isolate inclusive geometry and timer/budget gates.
  for(const shape of [0,1,2])for(const clock of [0,1,2,3,4,8])for(const equal of [false,true]){
   memory(c,cp,32*0x74).fill(0);m.view(np+0x7c9c,32*0x74).fill(0);
   flt(cp,32);flt(cp+8,shape===2?Math.PI/3:0);flt(cp+16,40);flt(cp+20,20);flt(cp+0x18,12.5);flt(cp+0x1c,90.25);put(cp+0x4c,equal?clock:clock-1);put(cp+0x50,clock);put(cp+0x60,103);put(cp+0x68,100000);put(cp+0x6c,4);put(cp+0x70,shape===0?3:1);m.write(np+0x7c9c,memory(c,cp,0x74));
   for(const [x,y,w,h]of [[12.5,90.25,0,0],[44.5,90.25,0,0],[44.50001,90.25,0,0],[32.5,100.25,0,0],[42.5,110.25,20,20],[-17.5,70.25,20,20],[500,500,1000,1000]])hit(x,y,w,h,`shape${shape} clock${clock} equal${equal} point${x}/${y}`);
  }
  report('damage-area',{passed:true,spawns,frames,hits,states,scope:'Original 433a90 creation and original 43155b..431658 update instructions; all 32 slots, capacity overflow, nine clock rates, linear/orbital/elliptical motion, expanding circles, rotated/unrotated rectangles, inclusive boundaries, periodic gate, damage budgets and score. Original 4347f0 performs the collision oracle.'});
 }finally{m.close();c.release(ct);c.sm_delete(f);c.anm_manager_delete(manager);c.sht_delete(sht);c.anm_delete(a);}
});
