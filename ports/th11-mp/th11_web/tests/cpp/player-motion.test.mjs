import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
import {normalizeAnimation,firstDifference} from './shot-oracle.mjs';
test('TH11 player movement, focus, warp and option following match original machine code',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m);let frames=0,states=0,animations=0,options=0;let events=[];
 m.replace(0x44a1e0,'player sound sink',()=>{events.push(m.reg('ESI')|0);return 0;});
 m.replace(0x432cc0,'separately tested option rebuild boundary',()=>{events.push(100);return 0;});
 m.replace(0x4245e0,'item attraction boundary',()=>{events.push(200);return 0;});
 const dv=()=>new DataView(c.memory.buffer),put=(p,v)=>dv().setInt32(p,v,true),flt=(p,v)=>dv().setFloat32(p,v,true),read=p=>dv().getUint32(p,true);
 const fields=[[0,0x87c,12],[12,0x888,8],[20,0x890,16],[36,0x8a0,12],[48,0x8ac,12],[60,0x8b8,8],[68,0x914,8],[76,0x91c,12],[88,0x8d20,4],[92,0x95c,4],[96,0x8ba0,16],[112,0x8bc8,4],[116,0x8bc4,4],[120,0x7c90,4],[124,0x8c14,4]];
 try{for(const [combination,name]of ['pl00a','pl00b','pl00c','pl01a','pl01b','pl01c'].entries()){
  const player=o.load(name.slice(0,4)+'.anm'),bullet=o.load('bullet.anm',true);
  const anms=[];for(const r of [player,bullet]){const p=c.anm_create(),data=c.allocate(r.source.length);memory(c,data,r.source.length).set(r.source);assert.equal(c.anm_open(p,data,r.source.length),1);anms.push([p,data]);}
  const sht=c.sht_create(),shtBytes=readFileSync(resolve(root,'reference/assets',name+'.sht')),sd=c.allocate(shtBytes.length);memory(c,sd,shtBytes.length).set(shtBytes);assert.equal(c.sht_open(sht,sd,shtBytes.length),1);
  const np=m.allocate(0x8d40),nem=m.allocate(0x100),nb=m.allocate(0x46d680),hud=m.allocate(0x4460),heap=m.heap;
  for(const speed of [.5,1,1.5]){
   m.heap=heap;m.view(heap,4000000).fill(0);for(const [p,n]of [[np,0x8d40],[nem,0x100],[nb,0x46d680],[hud,0x4460]])m.view(p,n).fill(0);o.reset();
   m.u32(0x4a8eb4,np);m.u32(np+0x10,player.original);m.u32(0x4a8d7c,nem);m.u32(0x4a8d68,nb);m.u32(nb+0x46d674,bullet.original);m.u32(0x4a8d84,hud);m.i32(0x4a5710,Math.floor(combination/3));m.i32(0x4a5714,combination%3);
   const manager=c.anm_manager_create(),f=c.pm_create(anms[0][0],anms[1][0],manager),s=c.pm_state(f),body=c.pm_body(f),rate=c.anm_env_rate(manager);
   flt(rate,speed);m.f32(0x4a7948,speed);for(const vis of [0,1]){memory(c,c.anm_env_rng(manager,vis),8).fill(0);put(c.anm_env_rng(manager,vis),12345);}
   c.pm_speeds(f,sht);put(s+12,-0x5bf0);put(s+16,300*128);flt(s,-183.875);flt(s+4,300);put(s+120,4);
   for(const [offset,native,n]of fields)m.write(np+native,memory(c,s+offset,n));
   m.reg('EAX',np+0x14);m.reg('EDI',player.original);m.reg('EBX',0);m.call(0x44acd0);assert.equal(c.anm_vm_bind(body,anms[0][0],0,manager),1);assert.ok(c.anm_vm_update(body,manager)>=0);
   for(let i=0;i<8;++i){const cp=c.pm_option(f,i);memory(c,cp,0xe4).fill(0);put(cp,i<4?2:0);flt(cp+0x1c,-1.2+i*.8);flt(cp+0x20,40+i*3);flt(cp+0x24,-1.4+i*.5);flt(cp+0x28,20+i*2);
    put(cp+0x5c,-2000+i*1500);put(cp+0x60,320*128);put(cp+0x64,(i-2)*2000);put(cp+0x68,-1000-i*400);put(cp+0x6c,(i-2)*700);put(cp+0x70,-1500-i*300);
    for(let k=0;k<5;++k){put(cp+0x74+k*8,(i-2)*1200+k*350);put(cp+0x78+k*8,-k*900-i*300);}
    flt(cp+0xa8,-1.57+i*1.57);put(cp+0xd0,i);put(cp+0xd4,1);put(cp+0xdc,combination===0?1:combination===2?2:0);
    for(const off of [0xb0,0xb4]){const cv=c.anm_manager_spawn(manager,anms[0][0],0,11,0),id=read(cv);assert.equal(o.spawn(player.original,0,11),id);put(cp+off,id);}
    const b=Buffer.from(memory(c,cp,0xe4));b.writeUInt32LE(combination===0?0x433690:combination===2?0x4337a0:0,0xdc);m.write(np+0x7570+i*0xe4,b);
   }
   function compare(label){
    for(const [off,native,n]of fields){assert.equal(firstDifference(Buffer.from(memory(c,s+off,n)),Buffer.from(m.bytes(np+native,n))),'',label+` player0x${native.toString(16)}`);++states;}
    assert.equal(c.pm_value(f,0),m.u32(np+0x756c),label+' focus handle');
    for(let i=0;i<8;++i){const b=Buffer.from(memory(c,c.pm_option(f,i),0xe4)),a=Buffer.from(m.bytes(np+0x7570+i*0xe4,0xe4));a.writeUInt32LE(0,0xdc);b.writeUInt32LE(0,0xdc);assert.equal(firstDifference(b,a),'',label+` option${i}`);++options;}
    const norm=(b,r,p)=>normalizeAnimation(b,r,p);const b=norm(memory(c,body,0x434),read,read(body+0x3a4)),a=norm(m.bytes(np+0x14,0x434),p=>m.u32(p),m.u32(np+0x3b8));assert.equal(firstDifference(b,a),'',label+' body');
    const native=o.states();assert.equal(c.anm_manager_count(manager),native.length,label+' animation count');for(const item of native){const cp=c.anm_manager_find(manager,item.id);assert.ok(cp);assert.equal(firstDifference(norm(memory(c,cp,0x434),read,read(cp+0x3a4)),norm(item.bytes,p=>m.u32(p),m.u32(item.p+0x3a4))),'',label+` animation${item.id}`);++animations;}
    const ep=c.pm_events(f);assert.deepEqual(Array.from({length:c.pm_value(f,2)},(_,i)=>dv().getInt32(ep+i*4,true)),events,label+' integration events');
    for(const vis of [0,1])assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(manager,vis),8)),Buffer.from(m.bytes(vis?0x4c2ef8:0x4c2f00,8)),label+' RNG');
   }
   try{for(let frame=0;frame<280;++frame){
    const phase=Math.floor(frame/10),keys=[0x40,0,0x40,0x48,0x80,0,0x10,0x90,0xa0,0x50,0x60,0xf0,0,0,0,0x81,0x88,0x10,0x40,0x80,0x50,0x60,0x90,0xa0,0,0,0,0x18];
    const held=keys[phase],pressed=frame%17===0?0x400:0,managerPresent=!(frame>=70&&frame<80),enemyPresent=!(frame>=60&&frame<70),bomb=frame>=140&&frame<150;
    if(frame===90||frame===130){const warp=frame===90?99:100;put(s+80,warp);m.i32(np+0x920,warp);put(s+84,0);m.i32(np+0x924,0);put(s+12,warp===99?-0x67f0:0x67f0);m.i32(np+0x888,warp===99?-0x67f0:0x67f0);}
    if(frame===170){put(s+116,2);m.u32(np+0x8bc4,2);}if(frame===200){put(s+116,0);m.u32(np+0x8bc4,0);}if(frame===240){put(s+116,8);m.u32(np+0x8bc4,8);}
    const lerp=[29,30,75,100][Math.floor(frame/23)%4];put(s+96,lerp);m.i32(np+0x8ba0,lerp);put(s+92,frame<3?frame:4);m.i32(np+0x95c,frame<3?frame:4);
    c.pm_input(f,held,pressed,combination,managerPresent?1:0,enemyPresent?1:0,bomb?1:0);m.u32(0x4c93c0,held);m.u32(0x4c93cc,pressed);m.u32(0x4a8d7c,managerPresent?nem:0);m.i32(nem+0x70,enemyPresent?1:0);m.i32(hud+0x4438,bomb?1:0);events=[];
    m.call(0x430290,{args:[np]});assert.equal(c.pm_update(f,0),1,`error ${c.pm_value(f,1)}`);compare(`${name} rate${speed} frame${frame}`);++frames;
    for(const overlay of [0,1]){o.update(overlay);assert.equal(c.anm_manager_update(manager,overlay),1);}
   }}finally{c.pm_delete(f);c.anm_manager_delete(manager);}
  }
  for(const [a,d]of anms){c.anm_delete(a);c.release(d);}c.sht_delete(sht);c.release(sd);console.log(name+' movement verified');
 }report('player-motion',{passed:true,frames,states,options,animations,scope:'Original 430290 and 430e50 with real player/bullet ANM resources and interpreter, six SHT movement tables, direction precedence, focus, speeds, clamping, warp states, orbit/directional options, weapon-mode selection and recall. Option reconstruction, sound output and item attraction are recorded integration boundaries; full player lifecycle is separate.'});
 }finally{m.close();}
});
