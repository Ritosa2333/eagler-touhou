import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
import {animationOracle} from './anm-oracle.mjs';
import {firstDifference} from './shot-oracle.mjs';

test('TH11 circular and full-screen cancellation preserve original filtering, boundaries and event order',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),{source,original}=o.load('bullet.anm');
 const data=c.allocate(source.length),center=c.allocate(12),f=c.bm_create();memory(c,data,source.length).set(source);assert.equal(c.bm_load(f,data,source.length),1);
 const manager=m.allocate(0x46d680),spell=m.allocate(0x900),origin=m.allocate(12);m.view(manager,0x46d680).fill(0);m.view(spell,0x900).fill(0);
 m.u32(0x4a8d68,manager);m.u32(0x4a8d6c,spell);m.u32(manager+0x46d674,original);
 let events=[],checks=0,cases=0,rewards=0,effects=0;
 const arg=n=>m.u32(m.reg('ESP')+n*4),position=p=>[m.f32(p),m.f32(p+4),m.f32(p+8)];
 m.replace(0x455b10,'cancel animation integration boundary',()=>{events.push([2,arg(3)|0,...position(m.reg('EAX')),0,0,0]);m.u32(arg(2),0);++effects;return 0;},4);
 m.replace(0x424230,'cancel point-item integration boundary',()=>{events.push([5,m.reg('ECX')|0,...position(m.reg('EAX')),arg(1),m.f32(m.reg('ESP')+8),m.f32(m.reg('ESP')+12)]);++rewards;return 0;},3);
 m.replace(0x412990,'cancel destructible-enemy integration boundary',()=>{events.push([6,arg(3)|0,...position(arg(1)),0,m.f32(m.reg('ESP')+8),0]);return 0;},3);
 m.replace(0x412b10,'beam destructible-enemy integration boundary',()=>{events.push([9,arg(3)|0,...position(arg(1)),0,m.f32(m.reg('ESP')+8),0]);return 0;},3);
 const view=()=>new DataView(c.memory.buffer),put=(p,v)=>view().setUint32(p,v,true);
 const bits=v=>{const b=Buffer.alloc(4);b.writeFloatLE(v);return b.readUInt32LE();};
 const next=v=>{const b=Buffer.alloc(4);b.writeUInt32LE(bits(v)+1);return b.readFloatLE();};
 const normalize=input=>{const b=Buffer.from(input);b.writeUInt32LE(b.readUInt32LE(0x470)?1:0,0x470);return b;};
 try{
  for(const mode of ['circle','all','beam'])for(const flags of [0,1,3])for(const id of [157,158,159,160,161,162])for(const skip of [false,true])for(const reward of [false,true]){
   const all=mode==='all',beam=mode==='beam';
   const sample=cases++,radius=[0,2.1,32,96,512,1.00001][sample%6],pos=[[0,224,0],[-192,0,2],[192,448,-2],[.125,112.125,0]][sample%4];
   const cb=Buffer.alloc(12);pos.forEach((v,i)=>cb.writeFloatLE(v,i*4));memory(c,center,12).set(cb);m.write(origin,cb);
   m.u32(spell+0x8e0,flags);m.i32(spell+0x8dc,id);const effective=(flags&1)&&id>=158&&id<=161?Math.fround(radius/3):Math.fround(radius);
   for(let i=0;i<2000;++i){const b=Buffer.alloc(0x910),r=Math.fround(effective+2),j=i%20;
    const coords=[[0,224],[pos[0],pos[1]],[pos[0]+r,pos[1]],[pos[0]+next(r),pos[1]],[pos[0],pos[1]-r],[-194,224],[-193.999,224],[194,224],[193.999,224],[0,-2],[0,-1.999],[0,450],[0,449.999],[pos[0]+r*.6,pos[1]+r*.8],[-200,-8],[-199.999,-7.999],[200,456],[199.999,455.999],[.37,112.28],[120,-90]][j];
    if(beam&&j<5){coords[0]=pos[0]+[0,effective,-effective,next(effective),-next(effective)][j];coords[1]=pos[1]-(i%3);}
    b.writeUInt32LE(i%8);b.writeInt32LE([0,0,1,-1][i%4],4);coords.forEach((v,k)=>b.writeFloatLE(v,0x43c+k*4));b.writeFloatLE(pos[2],0x444);
    b.writeFloatLE(4,0x45c);b.writeFloatLE(4,0x460);b.writeInt16LE([1,2,0,3,4][Math.floor(i/20)%5],0x4b2);b.writeInt32LE(i%3?14:-1,0x4a4);
    b.writeInt32LE(26,0x464);b.writeInt32LE(27,0x468);b.writeFloatLE(27.5,0x46c);b.writeUInt32LE(i%2,0x474);
    const p=c.bm_bullet(f,i);memory(c,p,0x910).set(b);if(i%2)put(p+0x470,c.bm_data(f,0));b.writeUInt32LE(i%2?0x4a7948:0,0x470);m.write(manager+0x64+i*0x910,b);
   }
   events=[];if(all){m.reg('EBX',+skip);m.call(0x40b5c0);}else if(beam){m.call(0x40b0e0,{args:[origin,bits(radius),+reward]});}else{m.reg('EBX',origin);m.call(0x40af90,{args:[bits(radius),+reward,+skip]});}
   assert.equal(beam?c.bm_cancel_beam(f,center,radius,+reward,flags,id):c.bm_cancel_area(f,all?0:center,radius,+reward,+skip,flags,id),1);
   for(let i=0;i<2000;++i){const a=normalize(memory(c,c.bm_bullet(f,i),0x910)),b=normalize(m.bytes(manager+0x64+i*0x910,0x910));assert.equal(firstDifference(a,b),'',`case ${sample} bullet ${i}`);++checks;}
   const ep=c.bm_cancel_events(f),dv=view(),actual=Array.from({length:c.bm_cancel_event_count(f)},(_,i)=>{const p=ep+i*32;return[dv.getInt32(p,true),dv.getInt32(p+4,true),...Array.from({length:3},(_,j)=>dv.getFloat32(p+8+j*4,true)),dv.getUint32(p+20,true),dv.getFloat32(p+24,true),dv.getFloat32(p+28,true)];});
   assert.deepEqual(actual,events,`case ${sample} ordered callbacks`);
  }
  report('bullet-cancellation',{passed:true,cases,checks,rewards,effects,scope:'Original 40af90/40b5c0/40b0e0/40ae90 over all 2,000 slots; radius, beam and playfield boundaries, protected spell IDs 158..161, delayed bullets, inactive/cancelling states, timer changes and exact ordered effect/item/enemy callbacks. Item creation and destructible-enemy damage remain explicit external boundaries in this test.'});
 }finally{c.bm_delete(f);c.release(data);c.release(center);m.close();}
});
