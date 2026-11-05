import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {firstDifference} from './shot-oracle.mjs';
test('TH11 line laser animation initialization, frame behavior and draw match original',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),{original,source}=o.load('bullet.anm');
 const data=c.allocate(source.length),args=c.allocate(0x1e4),na=m.allocate(0x1e4),q=m.allocate(0xe8c),manager=m.allocate(0x480),player=m.allocate(0x900),bm=m.allocate(0x46d680);
 memory(c,data,source.length).set(source);m.u32(0x4a8e94,manager);m.u32(manager+0x474,original);m.u32(0x4a8eb4,player);m.u32(0x4a8d68,bm);m.u32(bm+0x46d674,original);
 let f=0,p=0,events=[],collision=0,checks=0,frames=0,initializations=0,drawChecks=0;
 const cv=()=>new DataView(c.memory.buffer),read=p=>cv().getUint32(p,true),position=p=>[m.f32(p),m.f32(p+4),m.f32(p+8)];
 m.replace(0x44a260,'laser positional sound',()=>{events.push([1,m.reg('EDI')|0,m.f32(m.reg('ESP')+4),0,0]);return 0;},1);
 m.replace(0x44a1e0,'laser centered sound',()=>{events.push([0,m.reg('ESI')|0,0,0,0]);return 0;});
 // 432070 reads only x/y; the caller's unused local z retains stack bytes.
 m.replace(0x432070,'laser player collision boundary',()=>{const sp=m.reg('ESP'),p=m.reg('EAX');events.push([6,collision,m.f32(p),m.f32(p+4),0],[7,0,m.f32(sp+4),m.f32(sp+8),m.f32(sp+12)]);return collision;},3);
 m.replace(0x426230,'laser cut boundary',()=>{const sp=m.reg('ESP');events.push([8,m.u32(sp+12)|(m.u32(sp+16)<<1),...position(m.u32(sp+4))],[9,0,...position(m.u32(sp+8))]);return 0;},4);
 m.replace(0x455b10,'laser graze effect',()=>{events.push([10,m.i32(m.reg('ESP')+12),...position(m.reg('EAX'))]);return 0;},4);
 m.replace(0x40baa0,'laser graze reward',()=>{events.push([11,0,0,0,0]);return 0;},1);
 m.replace(0x40bb10,'laser graze count',()=>{events.push([12,0,0,0,0]);return 0;});
 const camera=m.allocate(0x100),device=m.allocate(4),vtable=m.allocate(0x180),batch=m.allocate(4096);let draws=[];
 m.u32(0x4c3288,device);m.u32(device,vtable);m.u32(0x4c37cc,camera);m.u32(camera+0xd4,640);m.u32(camera+0xd8,480);
 const arg=n=>m.u32(m.reg('ESP')+n*4),hook=(off,name,argc,handler)=>m.u32(vtable+off,m.registerImport({dll:'laser-render-oracle',name,argc,handler}));
 for(const [off,argc]of [[0xe4,3],[0x104,3],[0x10c,4],[0x164,2],[0x114,4]])hook(off,'material',argc,()=>0);
 hook(0x14c,'draw',5,()=>{assert.equal(arg(2),4);assert.equal(arg(5),28);draws.push(Buffer.from(m.bytes(arg(4),arg(3)*3*28)));return 0;});
 const normalize=(bytes,base,get)=>{const b=Buffer.from(bytes);b.writeUInt32LE(0,0);
  for(const t of[0x14,0x28,...Array.from({length:18},(_,i)=>0x70+i*52),0x424])b.writeUInt32LE(b.readUInt32LE(t+12)?1:0,t+12);
  for(const v of[0x624,0xa58]){
   for(const off of[4,16,0x1c]){const x=b.readUInt32LE(v+off);if(x)b.writeUInt32LE((x-base)>>>0,v+off);}
   for(const off of[0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0,0x3ac,0x42c,0x430])b.writeUInt32LE(b.readUInt32LE(v+off)?1:0,v+off);
   const start=get(base+v+0x3a4);for(const off of[0x394,0x3a4,0x3a8]){const x=b.readUInt32LE(v+off);if(x)b.writeUInt32LE((x-start)>>>0,v+off);}
  }return b;
 };
 const compare=label=>{const a=normalize(memory(c,p,0xe8c),p,read),b=normalize(m.bytes(q,0xe8c),q,x=>m.u32(x));assert.ok(a.equals(b),label+' '+firstDifference(a,b));++checks;
  const ep=c.laser_fixture_data(f,3),n=c.laser_fixture_count(f,0),v=cv();assert.deepEqual(Array.from({length:n},(_,i)=>[v.getInt32(ep+i*20,true),v.getInt32(ep+i*20+4,true),...Array.from({length:3},(_,j)=>v.getFloat32(ep+i*20+8+j*4,true))]),events,label+' events');
 };
 const reset=(params,rate)=>{if(f)c.ll_delete(f);f=c.ll_create();p=c.laser_fixture_data(f,0);assert.equal(c.ll_load(f,data,source.length),1);o.reset();c.ll_configure(f,rate,0);m.f32(0x4a7948,rate);
  for(const visual of[0,1]){const r=c.ll_rng(f,visual);memory(c,r,8).fill(0);cv().setUint16(r,12345,true);}
  const pos=Buffer.alloc(12);pos.writeFloatLE(20);pos.writeFloatLE(300,4);memory(c,c.laser_fixture_data(f,2),12).set(pos);m.write(player+0x87c,pos);
  events=[];m.view(q,0xe8c).fill(0);m.reg('EAX',q);m.call(0x424ea0);m.write(na,params);memory(c,args,params.length).set(params);m.call(0x4250b0,{ecx:q,args:[na]});assert.equal(c.ll_initialize(f,args),1);++initializations;compare(`initialize ${params.readInt16LE(36)}/${params.readInt16LE(38)}`);
 };
 const draw=label=>{draws=[];m.u32(o.manager+0x435620,0);m.u32(o.manager+0x7b5624,batch);m.u32(o.manager+0x7b5628,batch);m.u32(o.manager+0x4355bc,0);for(const off of[0x4355c0,0x4355c2,0x4355c6])m.view(o.manager+off,1)[0]=255;
  for(let i=0;i<4;++i)m.f32(0x4c91d8+i*28+12,1);m.call(0x426160,{ecx:q});m.reg('ESI',o.manager);m.call(0x44fd10);assert.equal(c.ll_draw(f),1);
  const render=c.ll_render(f),actual=Buffer.from(memory(c,c.render_data(render),c.render_size(render))),expected=Buffer.concat(draws);assert.equal(actual.length,expected.length,label+' vertices');assert.ok(actual.equals(expected),label+' '+firstDifference(actual,expected));++drawChecks;compare(label+' post draw');
 };
 const params=(type,color)=>{const b=Buffer.alloc(0x1e4);b.writeFloatLE(16.25);b.writeFloatLE(90.75,4);b.writeFloatLE(.1,8);b.writeFloatLE(.375,12);b.writeFloatLE(100,16);b.writeFloatLE(color%2?32:0,20);b.writeFloatLE(400,24);b.writeFloatLE([2,4,8,32,64][color%5],28);b.writeFloatLE(3.5,32);b.writeInt16LE(type,36);b.writeInt16LE(color,38);b.writeUInt32LE(color%2,40);b.writeInt32LE(-1,0x1dc);b.writeInt32LE(-1,0x1e0);return b;};
 try{
  for(let type=0;type<29;++type){const count=[17,28].includes(type)?4:type===23?3:[12,13,14,15,16,19,20,21,22,27].includes(type)?8:16;
   for(let color=0;color<count;++color){const rate=[1,.5,1.01][color%3];reset(params(type,color),rate);
    for(let frame=0;frame<12;++frame){events=[];c.laser_fixture_count(f,2);collision=frame%3;c.ll_configure(f,rate,collision);const expected=m.call(0x425cc0,{ecx:q})|0;assert.equal(c.ll_update(f),expected,`${type}/${color}/${frame} status`);compare(`frame ${type}/${color}/${frame}`);++frames;if(frame===3||frame===11)draw(`draw ${type}/${color}/${frame}`);if(expected)break;}
   }
  }
  for(let sample=0;sample<60;++sample){const b=params(4,sample%16),rate=[1,.5,1.01][sample%3];b.writeFloatLE(sample%2?199:-199,0);b.writeFloatLE(sample%3===0?-40:460,4);b.writeFloatLE((sample%7-3)*.85,12);b.writeFloatLE(sample%2?0:120,16);b.writeFloatLE(sample%2?80:0,20);b.writeFloatLE(sample%4?250:0,24);b.writeInt32LE(sample%5?7:-1,0x1dc);
   if(sample%6){const t=44;b.writeFloatLE(.3,t);b.writeFloatLE(.02,t+4);b.writeInt32LE(8,t+8);b.writeInt32LE(3,t+12);b.writeUInt32LE([0,4,8,0x10,0x1000,0x800][sample%6],t+16);if(sample%6===5){b.writeInt32LE(4,t+8);b.writeInt32LE(0,t+12);}}
   reset(b,rate);for(let frame=0;frame<100;++frame){events=[];c.laser_fixture_count(f,2);collision=frame%4===0?2:0;c.ll_configure(f,rate,collision);const expected=m.call(0x425cc0,{ecx:q})|0;assert.equal(c.ll_update(f),expected);compare(`extended ${sample}/${frame}`);++frames;if(expected)break;}
  }
  report('laser-line',{passed:true,initializations,frames,checks,drawChecks,scope:'Original 4250b0 initialization, 425cc0 update and 426160 screen vertices with real bullet.anm interpreter and all legal sprite/color combinations, three rates, growth, movement, clipping, retirement and appearance transforms. Collision result, partial cut, graze rewards/effects/audio and reflected creation remain ordered integration boundaries.'});
 }finally{if(f)c.ll_delete(f);c.release(data);c.release(args);m.close();}
});
