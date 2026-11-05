import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {firstDifference} from './shot-oracle.mjs';
test('TH11 laser manager matches original linked lifecycle, capacity and cutting',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),{original,source}=o.load('bullet.anm');
 const data=c.allocate(source.length),args=c.allocate(0x204),center=c.allocate(12),size=c.allocate(12),na=m.allocate(0x204),nc=m.allocate(12),ns=m.allocate(12),manager=m.allocate(0x480),player=m.allocate(0x900),bm=m.allocate(0x46d680),sound=m.allocate(0x100),em=m.allocate(0x100);
 memory(c,data,source.length).set(source);m.u32(0x4a8eb4,player);m.u32(0x4a8d68,bm);m.u32(bm+0x46d674,original);m.u32(0x4a8e88,sound);m.u32(0x4a8d7c,em);m.view(em,0x100).fill(0);
 const enemyFixture=c.enemy_fixture_create(),enemy=c.enemy_fixture_data(enemyFixture,0),estate=enemy+0x103c,eworld=c.enemy_fixture_data(enemyFixture,2),cc=c.allocate(0x1024),ci=c.allocate(128),ne=m.allocate(0x2678),nec=m.allocate(0x1024),nei=m.allocate(128),special=m.allocate(0x140);
 let f=0,events=[],collision=0,checks=0,states=0,frames=0,spawns=0,cuts=0,drawChecks=0,grazes=0,commandChecks=0,warningChecks=0;
 const cv=()=>new DataView(c.memory.buffer),read=p=>cv().getUint32(p,true),position=p=>[m.f32(p),m.f32(p+4),m.f32(p+8)];
 m.replace(0x44a260,'manager positional sound',()=>{events.push([1,m.reg('EDI')|0,m.f32(m.reg('ESP')+4),0,0]);return 0;},1);
 m.replace(0x44a1e0,'manager centered sound',()=>{events.push([0,m.reg('ESI')|0,0,0,0]);return 0;});
 m.replace(0x432070,'manager collision boundary',()=>{const sp=m.reg('ESP'),p=m.reg('EAX');events.push([6,collision,m.f32(p),m.f32(p+4),0],[7,0,m.f32(sp+4),m.f32(sp+8),m.f32(sp+12)]);return collision;},3);
 m.replace(0x455b10,'manager effect boundary',()=>{const id=m.i32(m.reg('ESP')+12);events.push([id===156?10:4,id,...position(m.reg('EAX'))]);return 0;},4);
 m.replace(0x40baa0,'manager graze reward boundary',()=>{events.push([11,0,0,0,0]);return 0;},1);
 m.replace(0x40bb10,'manager graze count boundary',()=>{m.i32(0x4a5754,m.i32(0x4a5754)+1);return 0;});
 m.replace(0x424230,'manager cancel reward boundary',()=>{events.push([5,8,...position(m.reg('EAX'))]);return 0;},3);
 m.replace(0x433f90,'manager cancel player shot boundary',()=>{events.push([13,0,...position(m.reg('ECX'))],[14,0,m.f32(0x4a3a64),0,0]);return 0;},2);
 // The original probe reads x/y only; its unused stack z accumulates stale values.
 m.replace(0x4322f0,'manager warning collision boundary',()=>{const sp=m.reg('ESP'),p=m.reg('EAX');events.push([15,collision,m.f32(p),m.f32(p+4),0],[16,0,m.f32(sp+4),m.f32(sp+8),m.f32(sp+12)]);return collision;},3);
 const camera=m.allocate(0x100),device=m.allocate(4),vtable=m.allocate(0x180),batch=m.allocate(1024*1024);let draws=[];const safeHeap=m.heap;
 m.u32(0x4c3288,device);m.u32(device,vtable);m.u32(0x4c37cc,camera);m.u32(camera+0xd4,640);m.u32(camera+0xd8,480);
 const arg=n=>m.u32(m.reg('ESP')+n*4),hook=(off,name,argc,handler)=>m.u32(vtable+off,m.registerImport({dll:'laser-manager-render',name,argc,handler}));
 for(const [off,argc]of [[0xe4,3],[0x104,3],[0x10c,4],[0x164,2],[0x114,4]])hook(off,'material',argc,()=>0);
 hook(0x14c,'draw',5,()=>{assert.equal(arg(2),4);assert.equal(arg(5),28);draws.push(Buffer.from(m.bytes(arg(4),arg(3)*3*28)));return 0;});
 const list=(head,get)=>{const result=[];for(let p=head;p;p=get(p+8)){assert.ok(result.length<257,'list cycle/capacity');result.push(p);}return result;};
 const normalize=(bytes,base,get,nodes)=>{const b=Buffer.from(bytes);b.writeUInt32LE(0,0);
  for(const off of[4,8]){const value=b.readUInt32LE(off);b.writeUInt32LE(value?nodes.indexOf(value)+2:0,off);}
  for(const t of[0x14,0x28,...Array.from({length:18},(_,i)=>0x70+i*52),0x424])b.writeUInt32LE(b.readUInt32LE(t+12)?1:0,t+12);
  for(const v of b.readInt32LE(16)===0?[0x624,0xa58]:[0x648,0xa7c]){
   for(const off of[4,16,0x1c]){const x=b.readUInt32LE(v+off);if(x)b.writeUInt32LE((x-base)>>>0,v+off);}
   for(const off of[0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0,0x3ac,0x42c,0x430])b.writeUInt32LE(b.readUInt32LE(v+off)?1:0,v+off);
   const start=get(base+v+0x3a4);for(const off of[0x394,0x3a4,0x3a8]){const x=b.readUInt32LE(v+off);if(x)b.writeUInt32LE((x-start)>>>0,v+off);}
  }return b;
 };
 const begin=()=>{events=[];c.laser_fixture_count(f,2);};
 const compare=label=>{
  const a=list(c.lm_data(f,0),read),b=list(m.u32(manager+0x18),p=>m.u32(p));assert.equal(a.length,b.length,label+' length');assert.equal(c.lm_count(f),m.u32(manager+0x454),label+' count');assert.equal(c.lm_count(f),a.length,label+' stored count');assert.equal(c.lm_data(f,1),a.at(-1)??0,label+' tail');assert.equal(m.u32(manager+0x450),b.at(-1)??manager+16,label+' original tail');
  assert.equal(read(c.lm_data(f,2)),m.u32(manager+0x458),label+' id');
  for(let i=0;i<a.length;++i){const n=read(a[i]+16)===0?0xe8c:0xeb0,aa=normalize(memory(c,a[i],n),a[i],read,a),bb=normalize(m.bytes(b[i],n),b[i],p=>m.u32(p),b);assert.ok(aa.equals(bb),label+` slot ${i} id ${read(a[i]+108)} `+firstDifference(aa,bb));++states;}
  for(const [k,off]of[[3,0x45c],[4,0x468]])assert.deepEqual(Buffer.from(memory(c,c.lm_data(f,k),12)),Buffer.from(m.bytes(manager+off,12)),label+' last cancellation');
  const ep=c.laser_fixture_data(f,3),n=c.laser_fixture_count(f,0),v=cv(),actual=Array.from({length:n},(_,i)=>[v.getInt32(ep+i*20,true),v.getInt32(ep+i*20+4,true),...Array.from({length:3},(_,j)=>v.getFloat32(ep+i*20+8+j*4,true))]);
  grazes+=actual.filter(e=>e[0]===12).length;assert.equal(m.i32(0x4a5754),grazes,label+' graze count');assert.deepEqual(actual.filter(e=>e[0]!==12),events,label+' events');++checks;
 };
 const reset=(rate=1)=>{if(f)c.lm_delete(f);f=c.lm_create();assert.equal(c.ll_load(f,data,source.length),1);o.reset();m.heap=safeHeap;m.view(manager,0x480).fill(0);m.u32(0x4a8e94,manager);m.u32(manager+0x450,manager+16);m.u32(manager+0x458,0x10000);m.u32(manager+0x474,original);m.i32(0x4a5754,0);grazes=0;
  c.ll_configure(f,rate,0);m.f32(0x4a7948,rate);for(const visual of[0,1]){const r=c.ll_rng(f,visual);memory(c,r,8).fill(0);cv().setUint16(r,12345,true);}
  const pos=Buffer.alloc(12);pos.writeFloatLE(20);pos.writeFloatLE(120,4);memory(c,c.laser_fixture_data(f,2),12).set(pos);m.write(player+0x87c,pos);begin();
 };
 const parameters=(type,index)=>{const b=Buffer.alloc(type?0x204:0x1e4);b.writeFloatLE(index%4*20-40);b.writeFloatLE(120,4);b.writeFloatLE(.125,8);
  if(type){b.writeFloatLE(.375,24);b.writeFloatLE(.01,28);b.writeFloatLE(160,32);b.writeFloatLE(96,36);b.writeFloatLE(32,40);b.writeFloatLE(3.5,44);for(const [i,v]of[3,5,11,7].entries())b.writeInt32LE(v,48+i*4);b.writeInt32LE(-1,64);b.writeInt32LE(-1,68);b.writeInt32LE(800+index,72);b.writeInt16LE(4,76);b.writeInt16LE(index%16,78);b.writeUInt32LE(index%16,80);}
  else{b.writeFloatLE(.375,12);b.writeFloatLE(160,16);b.writeFloatLE(96,20);b.writeFloatLE(400,24);b.writeFloatLE(32,28);b.writeFloatLE(3.5,32);b.writeInt16LE(4,36);b.writeInt16LE(index%16,38);b.writeUInt32LE(index%2,40);b.writeInt32LE(-1,0x1dc);b.writeInt32LE(-1,0x1e0);}return b;
 };
 const spawn=(type,params,label)=>{begin();m.write(na,params);memory(c,args,params.length).set(params);m.reg('EDI',na);assert.equal(c.lm_spawn(f,args,type),m.call(0x424df0,{args:[type]})|0,label+' result');++spawns;compare(label);};
 const step=(rate,hit=0,paused=0,frozen=0)=>{begin();collision=hit;c.ll_configure(f,rate,hit);m.f32(0x4a7948,rate);m.u32(sound+0x60,paused?4:frozen?2:0);assert.equal(c.lm_update(f,paused,frozen),m.call(0x424d70,{ecx:manager}));++frames;compare(`step ${frames}`);};
 const cancel=(kind,x,y,radius,rewards=1,skip=1)=>{begin();const pos=Buffer.alloc(12),sz=Buffer.alloc(12);pos.writeFloatLE(x);pos.writeFloatLE(y,4);sz.writeFloatLE(radius*2);sz.writeFloatLE(radius*2,4);memory(c,center,12).set(pos);memory(c,size,12).set(sz);m.write(nc,pos);m.write(ns,sz);let expected;
  if(kind===0){m.reg('EBX',rewards);m.reg('EDI',skip);expected=m.call(0x425040);}
  else if(kind===1){m.reg('EDI',nc);m.reg('ESI',ns);expected=m.call(0x424f30,{args:[rewards]});}
  else{const bits=Buffer.alloc(4);bits.writeFloatLE(radius);m.reg('ESI',nc);expected=m.call(0x424fe0,{args:[bits.readUInt32LE(),rewards,skip]});}
  const actual=c.lm_cancel(f,kind,center,size,radius,rewards,skip);assert.ok(actual>=0,'cancel failure');if(kind===0)assert.equal(actual,expected);++cuts;compare(`cancel ${cuts}`);
 };
 const draw=(paused=0)=>{begin();draws=[];m.u32(sound+0x60,paused?4:0);m.u32(o.manager+0x435620,0);m.u32(o.manager+0x7b5624,batch);m.u32(o.manager+0x7b5628,batch);m.u32(o.manager+0x4355bc,0);for(const off of[0x4355c0,0x4355c2,0x4355c6])m.view(o.manager+off,1)[0]=255;
  for(let i=0;i<4;++i)m.f32(0x4c91d8+i*28+12,1);m.call(0x424dd0,{ecx:manager});m.reg('ESI',o.manager);m.call(0x44fd10);assert.equal(c.lm_draw(f,paused),1);const render=c.ll_render(f),a=Buffer.from(memory(c,c.render_data(render),c.render_size(render))),b=Buffer.concat(draws);assert.equal(a.length,b.length,'draw length');assert.ok(a.equals(b),'draw '+firstDifference(a,b));++drawChecks;compare(`draw ${drawChecks}`);
 };
 try{
  for(const rate of[1,.5,1.01]){
   reset(rate);for(let i=0;i<12;++i)spawn(i%2,parameters(i%2,i),`initial ${rate}/${i}`);
   cancel(1,20,120,16);step(rate,0,1);step(rate,0,0,1);draw(1);draw();
   for(let frame=0;frame<65;++frame){step(rate,frame%5===0?1:frame%3===0?2:0);if(frame===5)cancel(2,60,140,24,3,1);if(frame===12)cancel(1,70,170,16);if(frame===25)cancel(0,0,0,0,1,1);if(frame%13===0)draw();}
  }
  reset();cv().setUint32(c.lm_data(f,2),0x7ffffffe,true);m.u32(manager+0x458,0x7ffffffe);
  for(let i=0;i<260;++i)spawn(i%2,parameters(i%2,i),`capacity ${i}`);
  const aa=list(c.lm_data(f,0),read),bb=list(m.u32(manager+0x18),p=>m.u32(p));for(let i=0;i<aa.length;++i){const mark=[0,1,254,255][i%4];memory(c,aa[i]+0x68,1)[0]=mark;m.view(bb[i]+0x68,1)[0]=mark;if(i%7===0){cv().setInt32(aa[i]+12,1,true);m.i32(bb[i]+12,1);}}
  begin();c.lm_mark(f);m.call(0x424fb0);compare('mark mixed');step(1);draw();begin();c.lm_mark(f);m.call(0x424fb0);compare('mark survivors');step(1);assert.equal(c.lm_count(f),0);
  for(let round=0;round<8;++round){reset();
   for(let i=0;i<4;++i){const p=parameters(0,i);p.writeFloatLE(i%2?185:-185);p.writeFloatLE(i%2?445:3,4);p.writeFloatLE(i%2?.4:-2.7,12);p.writeFloatLE(80,16);p.writeFloatLE(80,20);p.writeFloatLE(6,44);p.writeInt32LE(3,52);p.writeInt32LE(15,56);p.writeUInt32LE(0x100,60);spawn(0,p,`reflection ${round}/${i}`);}
   for(let frame=0;frame<30;++frame){step([1,.5,1.01][round%3]);if(frame===8)cancel(round%2?1:2,round%2?170:-170,round%2?440:10,40,1,0);}draw();
  }
  reset();for(let i=0;i<6;++i)spawn(i%2,parameters(i%2,i),`warning initial ${i}`);
  for(let example=0;example<1024;++example){
   const a=list(c.lm_data(f,0),read),b=list(m.u32(manager+0x18),p=>m.u32(p));for(let j=0;j<a.length;++j){
    const type=read(a[j]+16),state=[1,2,3,3][(example+j)%4],angle=Math.fround((example%101-50)/7),target=Math.fround((example%93-46)/6),frames=[0,59,60,61,120,2147483647,-2147483648][example%7];
    for(const [off,value]of[[0x54,angle],...(type?[[0x458,target]]:[])]){cv().setFloat32(a[j]+off,value,true);m.f32(b[j]+off,value);}
    for(const [off,value]of[[12,state],[0x18,example%125],...(type?[[0x470,frames],[0x644,255]]:[])]){cv().setInt32(a[j]+off,value,true);m.i32(b[j]+off,value);}
   }
   begin();collision=example%3;c.ll_configure(f,1,collision);const delta=Math.fround([0,.01,-.25,4,-6,.04908738657832146][example%6]);m.f32(special+0x134,delta);m.call(0x428610,{ecx:special});assert.equal(c.lm_warning(f,delta),1);compare(`warning ${example}`);++warningChecks;
  }
  for(const op of[0x19c,0x19d,0x1ac,0x1ad,0x1af,0x1b0,0x1b1,0x1b2])for(let example=0;example<36;++example){
   reset([1,.5,1.01][example%3]);const b=Buffer.alloc(0x163c),infinite=[0x19d,0x1ad,0x1b0,0x1b2].includes(op);
   for(const [off,value]of[[0x34,16],[0x38,90],[0x3c,.125],[0x1414,3.25],[0x1418,-.75],[0x141c,.25],[0x1474,32],[0x1478,200],[0x147c,[0,.9,1][example%3]]])b.writeFloatLE(value,off);
   b.writeInt32LE(7,0x578);b.writeInt32LE(24,0x57c);b.writeInt32LE(777,0x124);for(let j=0;j<18;++j){const t=0x398+j*24;b.writeFloatLE(j*.05,t);b.writeFloatLE(j*.1,t+4);b.writeInt32LE(j,t+8);b.writeInt32LE(j%3,t+12);b.writeInt32LE([0,0x200,0x1000][j%3],t+16);b.writeInt32LE(j%2,t+20);}
   b.writeUInt32LE(enemy,0x1614);memory(c,estate,b.length).set(b);b.writeUInt32LE(ne,0x1614);m.write(ne+0x103c,b);m.u32(ne,0x494074);m.u32(ne+4,nec);m.u32(nec+4,nei);m.u32(nec+0x1014,ne);memory(c,cc,0x1024).fill(0);cv().setUint32(cc+4,ci,true);
   const ins=Buffer.alloc(128);ins.writeUInt16LE(op,4);ins.writeUInt16LE(128,6);ins[10]=255;ins[11]=12;
   if(infinite){ins.writeUInt16LE(1,8);ins.writeInt32LE(-9985,16);ins.writeInt32LE(4,20);ins.writeInt32LE(example%16,24);ins.writeFloatLE((example-18)*.8,28);ins.writeFloatLE(32,32);ins.writeFloatLE(64,36);for(const[j,value]of[120,10,90,15].entries())ins.writeInt32LE(value,40+j*4);ins.writeFloatLE(160,56);ins.writeInt32LE(example%16,60);}
   else{ins.writeInt32LE(4,16);ins.writeInt32LE(example%16,20);for(const[j,value]of[(example-18)*.8,3.5,64,160,400,32].entries())ins.writeFloatLE(value,24+j*4);}
   const command=(label)=>{begin();memory(c,ci,128).set(ins);m.write(nei,ins);m.resetThreadFPU();assert.equal(c.lm_enemy_command(f,enemyFixture,cc),m.call(0x412e30,{ecx:ne+0x103c})|0,label+' result');assert.equal(cv().getInt32(estate+0x124,true),m.i32(ne+0x103c+0x124),label+' destination');++commandChecks;compare(label);};
   command(`create command ${op.toString(16)}/${example}`);
   const node=c.lm_data(f,0),id=cv().getInt32(node+0x6c,true);for(const mutation of[0x19e,0x19f,0x1a0,0x1a1,0x1a2,0x1a3]){
    ins.fill(0);ins.writeUInt16LE(mutation,4);ins.writeUInt16LE(128,6);ins[10]=255;ins[11]=3;ins.writeInt32LE(example%4===0?0:id,16);ins.writeFloatLE(.75,20);ins.writeFloatLE(1.25,24);command(`mutate ${mutation.toString(16)}/${example}`);
   }
   ins.writeUInt16LE(0x1c0,4);ins.writeInt32LE(id,16);command(`cancel id ${op.toString(16)}/${example}`);step(1);assert.equal(c.lm_count(f),id===0?1:0);
  }
  report('laser-manager',{passed:true,checks,states,frames,spawns,cuts,drawChecks,commandChecks,warningChecks,scope:'Original 424df0 creation and real line/infinite initialization; 424c10/424d70 linked updates, pause/freeze, capacity 256, signed ID wrap, override IDs, mixed deletion marks including 255 wrap, ordered reflected/split creation, 424f30/424fe0/425040 cancellation and 424d40/424dd0 rendered vertices. Original 428610 warning tracking and ECL 412e30 laser creation, mutation, cancellation commands including additive/non-additive flags, normalization and ID destinations. Real ANM interpreter; collision result, effect/item/player-shot/audio/boss systems remain callbacks. Partial-cut return values with original undefined EAX are not compared.'});
 }finally{if(f)c.lm_delete(f);c.enemy_fixture_delete(enemyFixture);for(const p of[data,args,center,size,cc,ci])c.release(p);m.close();}
});
