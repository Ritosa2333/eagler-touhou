import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {bits} from './ecl-bytecode.mjs';
test('TH11 rotated laser hit, graze and non-damaging probe match original',async()=>{
 const c=await core(),m=await oracle(),state=c.allocate(44),origin=c.allocate(8),half=c.allocate(8),player=m.allocate(0x8d40),hud=m.allocate(0x4500),np=m.allocate(8);
 m.u32(0x4a8eb4,player);m.u32(0x4a8d84,hud);let hits=0,checks=0;const v=()=>new DataView(c.memory.buffer),flt=(p,x)=>v().setFloat32(p,x,true),put=(p,x)=>v().setInt32(p,x,true);
 m.replace(0x432a90,'laser hit transition boundary',()=>{++hits;return 0;});
 const run=(px,py,x,y,hx,hy,angle,width,length,probe,mode=1,inv=0,flags=0,bomb=0)=>{
  memory(c,state,44).fill(0);flt(state,px);flt(state+4,py);put(state+28,mode);put(state+32,inv);put(state+36,flags);memory(c,state+40,1)[0]=bomb;flt(origin,x);flt(origin+4,y);flt(half,hx);flt(half+4,hy);
  for(const [off,z]of [[0x87c,px],[0x880,py],[0x8e4,hx],[0x8e8,hy]])m.f32(player+off,z);m.i32(player+0x928,mode);m.i32(player+0x8bb4,inv);m.u32(player+0x8bc4,flags);m.u32(hud+0x4438,bomb);m.f32(np,x);m.f32(np+4,y);
  hits=0;m.reg('EAX',np);const expected=m.call(probe?0x4322f0:0x432070,{args:[bits(angle),bits(width),bits(length)]})|(hits?4:0),actual=c.player_laser_collision(state,origin,half,angle,width,length,probe);
  assert.equal(actual,expected,JSON.stringify({px,py,x,y,hx,hy,angle,width,length,probe,mode,inv,flags,bomb}));++checks;
 };
 try{
  for(const probe of [0,1])for(const h of [1,1.75])for(const width of [0,2,5,32,64])for(const length of [0,16,100,500])for(const angle of [0,Math.PI/2,-Math.PI/2,Math.PI,.375])for(const scale of [1,16])for(const epsilon of [-.0001,0,.0001])for(const edge of [0,1,2,3]){
   const localX=edge===0?-h*scale+epsilon:edge===1?length+h*scale+epsilon:length/2,localY=edge===2?width/2+h*scale+epsilon:edge===3?-width/2-h*scale+epsilon:0;
   run(10+localX*Math.cos(angle)-localY*Math.sin(angle),20+localX*Math.sin(angle)+localY*Math.cos(angle),10,20,h,h,angle,width,length,probe);
  }
  for(const probe of [0,1])for(const mode of [0,1,2,3,4,5])for(const inv of [-1,0,1,120])for(const flags of [0,1,2,3])for(const bomb of [0,1])for(const y of [0,10,40])run(50,y,0,0,1.75,1.75,0,12,100,probe,mode,inv,flags,bomb);
  let seed=173;const random=()=>((seed=(Math.imul(seed,1664525)+1013904223)>>>0)/4294967296);
  for(let i=0;i<10000;++i)run(random()*384-192,random()*448,random()*384-192,random()*448,.5+random()*2,.5+random()*2,(random()-.5)*6.283185,random()*64,random()*600,i%2);
  report('player-laser-collision',{passed:true,checks,scope:'Original 432070 damaging beam and 4322f0 non-damaging probe: rotated coordinates, inclusive inner/graze boundaries, Reimu/Marisa half hitboxes, state/Bomb/flag immunity, invincibility and hit callback. Laser object lifetime and rendering remain separate.'});
 }finally{c.release(state);c.release(origin);c.release(half);m.close();}
});
