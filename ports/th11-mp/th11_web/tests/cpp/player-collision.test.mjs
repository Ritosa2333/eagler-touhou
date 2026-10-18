import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
test('TH11 player hit and graze boundaries match original',async()=>{
 const c=await core(),m=await oracle(),state=c.allocate(44),pos=c.allocate(8),size=c.allocate(8),player=m.allocate(0x8c00),shot=m.allocate(16),bomb=m.allocate(0x4500),np=m.allocate(8),ns=m.allocate(8);
 m.u32(0x4a8eb4,player);m.u32(player+0x92c,shot);m.u32(0x4a8d84,bomb);
 let hits=0,checks=0;const view=()=>new DataView(c.memory.buffer);const bits=n=>{const b=Buffer.alloc(4);b.writeFloatLE(n);return b.readUInt32LE();};
 m.replace(0x432a90,'player hit transition boundary',()=>{++hits;return 0;});
 const run=(shape,x,y,sx,sy,px,py,r,mode,invincible,flags,bombActive)=>{
  const f=Math.fround;px=f(px);py=f(py);r=f(r);
  const values=[px,py,f(px-r),f(py-r),f(px+r),f(py+r),r];
  memory(c,state,44).fill(0);values.forEach((v,i)=>view().setFloat32(state+i*4,v,true));
  view().setInt32(state+28,mode,true);view().setInt32(state+32,invincible,true);view().setUint32(state+36,flags,true);memory(c,state+40,1)[0]=+bombActive;
  for(const [off,v]of [[0x87c,values[0]],[0x880,values[1]],[0x8cc,values[2]],[0x8d0,values[3]],[0x8d8,values[4]],[0x8dc,values[5]]])m.f32(player+off,v);
  m.f32(shot+4,r);m.u32(player+0x928,mode);m.u32(player+0x8bb4,invincible);m.u32(player+0x8bc4,flags);m.u32(bomb+0x4438,+bombActive);
  for(const [p,q,a,b]of [[pos,np,x,y],[size,ns,sx,sy]]){view().setFloat32(p,a,true);view().setFloat32(p+4,b,true);m.f32(q,a);m.f32(q+4,b);}
  hits=0;m.reg('EAX',shape?np:ns);const expected=m.call(shape?0x431f70:0x431e00,{edx:np,args:shape?[bits(sx)]:[]})|(hits?4:0);
  assert.equal(c.player_collision(state,pos,size,shape),expected,JSON.stringify({shape,x,y,sx,sy,px,py,r,mode,invincible,flags,bombActive}));++checks;
 };
 try{
  for(const shape of [0,1])for(const r of [2,2.5,3,4])for(const radius of [0,4,8,14,40,120])for(const p of [[0,400],[16.25,200.75],[-184,431.9]]){
   const distances=shape?[Math.sqrt(r*r+radius*radius),Math.sqrt((r+Math.max(40,Math.fround(radius/2.5)))**2+radius*radius)]:[r+radius/2,r+24];
   for(const d of distances)for(const epsilon of [-1e-4,0,1e-4])for(const axis of [0,1,2,3]){
    const x=p[0]+(axis===0?d+epsilon:axis===2?-d-epsilon:0),y=p[1]+(axis===1?d+epsilon:axis===3?-d-epsilon:0);
    run(shape,x,y,radius,radius,p[0],p[1],r,0,0,0,false);
   }
  }
  for(const shape of [0,1])for(let mode=0;mode<6;++mode)for(const invincible of [-1,0,1,120])for(const flags of [0,1,2,3,0x80])for(const active of [false,true])for(const d of [0,12,28,60])run(shape,d,400,8,6,0,400,2.5,mode,invincible,flags,active);
  let seed=12345;const random=()=>((seed=(Math.imul(seed,1664525)+1013904223)>>>0)/4294967296);
  for(let n=0;n<10000;++n){const px=(random()-.5)*384,py=random()*448;run(n&1,px+(random()-.5)*140,py+(random()-.5)*140,random()*96,random()*32,px,py,2+random()*2,0,0,0,false);}
  report('player-collision',{passed:true,checks,scope:'Rectangular and circular projectile hit/graze geometry, rectangular inclusive and circular strict boundaries, state/bomb/flag immunity and invincibility hit callback. Player death/bomb state machine remains a separate integration boundary.'});
 }finally{c.release(state);c.release(pos);c.release(size);m.close();}
});
