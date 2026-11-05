import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
import {normalizeAnimation,firstDifference} from './shot-oracle.mjs';
import {graphicsOracle} from './graphics-oracle.mjs';
test('TH11 reconstructed Bomb controllers match original machine-code lifecycle with real ANM scripts',async()=>{
 const c=await core(),m=await oracle();graphicsOracle(m);const o=animationOracle(m);let checks=0,frames=0,scenarios=0,events=[],pending=null;
 const dv=()=>new DataView(c.memory.buffer),put=(p,v)=>dv().setUint32(p,v>>>0,true),flt=(p,v)=>dv().setFloat32(p,v,true),read=p=>dv().getUint32(p,true);
 const stack=n=>m.u32(m.reg('ESP')+n*4),vec=p=>[m.u32(p),m.u32(p+4),m.u32(p+8)];
 m.replace(0x44a260,'positional sound output',()=>{events.push(1,m.reg('EDI'),stack(1),1);return 0;},1);
 m.replace(0x44a1e0,'nonpositional sound output',()=>{events.push(1,m.reg('ESI'),0,0);return 0;});
 m.replace(0x44a300,'stop Bomb loop sound',()=>{events.push(2,49);return 0;});
 m.replace(0x420a00,'power refund output',()=>{events.push(4,m.reg('EAX'));return 0;});
 m.replace(0x432cc0,'option rebuild boundary',()=>{events.push(5);return 0;});
 m.replace(0x406f90,'beam cancellation boundary',()=>{const p=m.reg('EAX');events.push(6,...vec(p+0x484),(~m.u32(m.u32(0x4a8d6c)+0x8e0))&1);return 0;});
 m.replace(0x40af90,'normal bullet cancellation boundary',()=>{pending=[...vec(m.reg('EBX')),stack(1),stack(2),0];assert.equal(stack(3),1);return 0;},3);
 m.replace(0x40b210,'converted bullet cancellation boundary',()=>{pending=[...vec(stack(1)),stack(2),m.reg('EBX'),1];return 0;},2);
 m.replace(0x424fe0,'laser cancellation boundary',()=>{assert.ok(pending);assert.deepEqual([...vec(m.reg('ESI')),stack(1)],pending.slice(0,4));assert.equal(stack(3),1);assert.equal(stack(2)&1,pending[4]);events.push(3,...pending.slice(0,4),stack(2),pending[5]);pending=null;return 0;},3);
 const selections=process.env.TH11_BOMB_SELECTIONS?process.env.TH11_BOMB_SELECTIONS.split(',').map(Number):[0,1,2,3,4,5];
 try{for(const selection of selections){
  const name=['pl00a','pl00b','pl00c','pl01a','pl01b','pl01c'][selection],a=o.load(name.slice(0,4)+'.anm'),text=o.load('text.anm',true),an=c.anm_create(),ad=c.allocate(a.source.length);memory(c,ad,a.source.length).set(a.source);assert.equal(c.anm_open(an,ad,a.source.length),1);
  m.u32(0x4c3808,text.original);m.u32(0x4c342c,1);
  const sb=readFileSync(resolve(root,'reference/assets',name+'.sht')),sht=c.sht_create(),sd=c.allocate(sb.length);memory(c,sd,sb.length).set(sb);assert.equal(c.sht_open(sht,sd,sb.length),1);
  const np=m.allocate(0x8d40),ns=m.allocate(0x4a0),spell=m.allocate(0x900),nem=m.allocate(0x100),target=m.allocate(24),stage=m.allocate(0x2330),heap=m.heap;
  for(const speed of [.5,1,1.5])for(const spellAge of [-1,59,60])for(const hitAt of selection===5?[-1,0,59]:[-1]){
   m.heap=heap;m.view(heap,8000000).fill(0);for(const[p,n]of [[np,0x8d40],[ns,0x4a0],[spell,0x900],[nem,0x100]])m.view(p,n).fill(0);o.reset();events=[];pending=null;
   for(const[addr,p]of [[0x4a8eb4,np],[0x4a8d64,ns],[0x4a8d6c,spell],[0x4a8d7c,nem]])m.u32(addr,p);m.u32(np+0x10,a.original);m.i32(0x4a5710,Math.floor(selection/3));m.i32(0x4a5714,selection%3);
   m.u32(0x4a8d60,stage);m.u32(stage+0x2328,0);
   const manager=c.anm_manager_create(),f=c.bomb_create(sht,an,an,manager),player=c.bomb_player(f),s=c.pf_pointer(player,0),state=c.pf_pointer(player,1),body=c.pf_pointer(player,2),sp=c.pf_pointer(player,3),bs=c.bomb_state(f),rate=c.anm_env_rate(manager),point=c.allocate(24);
   c.bomb_select(f,selection);flt(rate,speed);m.f32(0x4a7948,speed);for(const vis of [0,1]){memory(c,c.anm_env_rng(manager,vis),8).fill(0);put(c.anm_env_rng(manager,vis),12345);}
   const textData=c.allocate(text.source.length);memory(c,textData,text.source.length).set(text.source);assert.equal(c.anm_open(c.bomb_text(f),textData,text.source.length),1);c.release(textData);
   for(const[off,val]of [[0,-13.125],[4,311.75],[8,.5]]){flt(s+off,val);m.f32(np+0x87c+off,val);}put(s+68,123);put(s+72,456);m.i32(np+0x914,123);m.i32(np+0x918,456);
   for(const[co,no,n]of [[0,0x14,20],[20,0x28,20]]){m.write(ns+no,memory(c,bs+co,n));m.u32(ns+no+12,0);}
   m.write(np+0x8bb0,memory(c,state+44,20));m.u32(np+0x8bbc,0x4a7948);
   m.write(np+0x7c9c,memory(c,c.pf_pointer(player,6),32*0x74));
   const flags=spellAge<0?0:0x23;put(sp,flags);put(sp+4,Math.max(0,spellAge));put(sp+8,99999);m.u32(spell+0x8e0,flags);m.i32(spell+0x88c,Math.max(0,spellAge));m.i32(spell+0x8e4,99999);
   c.anm_vm_bind(body,an,0,manager);assert.equal(c.anm_vm_update(body,manager),0);m.reg('ESI',np+0x14);m.call(0x401fd0);m.reg('EAX',np+0x14);m.reg('EDI',a.original);m.reg('EBX',0);m.call(0x44acd0);
   const equal=(cp,nat,len,label)=>{assert.equal(firstDifference(Buffer.from(memory(c,cp,len)),Buffer.from(m.bytes(nat,len))),'',label);++checks;};
   const animation=(cp,nat,label)=>{const actual=normalizeAnimation(memory(c,cp,0x434),read,read(cp+0x3a4)),expected=normalizeAnimation(m.bytes(nat,0x434),p=>m.u32(p),m.u32(nat+0x3a4));actual.writeUInt32LE(0,0x418);expected.writeUInt32LE(0,0x418);assert.equal(firstDifference(actual,expected),'',label);++checks;};
   const compare=label=>{
    for(const[co,no,len]of [[0,0x14,12],[16,0x24,4],[20,0x28,12],[36,0x38,4],[40,0x3c,12],[52,0x484,24]])equal(bs+co,ns+no,len,label+' Bomb '+no.toString(16));
    equal(s+68,np+0x914,8,label+' movement');equal(s+116,np+0x8bc4,4,label+' player flags');equal(state+44,np+0x8bb0,12,label+' invincibility');equal(state+60,np+0x8bc0,4,label+' inv flags');
    assert.equal(c.bomb_value(f,3)>>>0,m.u32(stage+0x2328),label+' background tint');equal(state,np+0x928,4,label+' player lifecycle');
    for(const[co,no]of [[0,0x8e0],[4,0x88c],[8,0x8e4]])equal(sp+co,spell+no,4,label+' spell');assert.equal(c.bomb_value(f,1),m.i32(nem+0x14));
    animation(body,np+0x14,label+' body');const native=o.states();assert.equal(c.anm_manager_count(manager),native.length,label+' count');for(const vm of native){const cp=c.anm_manager_find(manager,vm.id);assert.ok(cp);animation(cp,vm.p,label+' ANM '+vm.id);}
    for(let i=0;i<32;++i){const b=Buffer.from(memory(c,c.pf_pointer(player,6)+i*0x74,0x74)),n=Buffer.from(m.bytes(np+0x7c9c+i*0x74,0x74));for(const x of[b,n])x.writeUInt32LE(0,0x58);assert.equal(firstDifference(b,n),'',label+' area '+i);++checks;}
    const ep=c.bomb_events(f);assert.deepEqual(Array.from({length:c.bomb_value(f,0)},(_,i)=>read(ep+i*4)),events.map(v=>v>>>0),label+' callbacks');assert.equal(pending,null);
    for(const vis of[0,1])assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(manager,vis),8)),Buffer.from(m.bytes(vis?0x4c2ef8:0x4c2f00,8)),label+' RNG');
    const mesh=m.u32(ns+0x480),cm=c.bomb_mesh(f,0);assert.equal(!!cm,!!mesh,label+' mesh lifetime');
    if(mesh){equal(cm,mesh,8,label+' mesh size');const columns=m.u32(mesh),rows=m.u32(mesh+4);equal(c.bomb_mesh(f,1),m.u32(mesh+16),columns*rows*28,label+' mesh vertices');equal(c.bomb_mesh(f,2),m.u32(mesh+20),columns*rows*12,label+' mesh base positions');}
    for(const v of native){const cp=c.anm_manager_find(manager,v.id),g=m.u32(v.p+0x400);if(!g)continue;const rows=m.i32(v.p+0x3b4);if(rows>1&&rows<256)equal(read(cp+0x400),g,rows*2*28,label+' animation geometry '+v.id);}
   };
   try{
    assert.equal(m.call(0x406510),0);assert.equal(c.bomb_action(f,0),0);compare(name+' start');
    events=[];assert.equal(m.call(0x406510)|0,-1);assert.equal(c.bomb_action(f,0),-1);compare(name+' duplicate');
    let ended=false;
    for(let frame=0;frame<2100;++frame){const label=name+' rate'+speed+' age'+spellAge+' hit'+hitAt+' frame'+frame;events=[];
     if(selection===5){const x=Math.fround(-13.125+frame*.125),y=Math.fround(311.75-frame*.0625);flt(s,x);flt(s+4,y);m.f32(np+0x87c,x);m.f32(np+0x880,y);if(frame===hitAt){put(state,4);m.i32(np+0x928,4);}}
     assert.equal(m.call(0x4064d0,{ecx:ns,limit:3000000}),1);assert.equal(c.bomb_action(f,1),1,label+' error '+c.bomb_value(f,2));compare(label);++frames;
     if([2,3,4].includes(selection)&&m.u32(ns+0x3c))for(const[x,y]of [[-13.125,311.75],[0,0],[192,448],[-40,200],[70,150]]){flt(point,x);flt(point+4,y);m.f32(target,x);m.f32(target+4,y);m.reg('EBX',ns);m.reg('EAX',ns);m.reg('ESI',target+12);m.u32(target+12,0);m.u32(target+16,0);const damage=selection===4?m.call(0x407220,{args:[target]}):m.call(selection===2?0x407fc0:0x406e90,{ecx:target,edx:ns});assert.equal(c.bomb_damage(f,point),damage,label+' special damage');++checks;}
     if(selection===3&&frame%13===0&&m.u32(ns+0x3c))for(const size of [[0,0],[8,16],[32,48],[1.1,1.3]])for(const [dx,dy]of [[-16,0],[16,-448],[16.001,-224],[-16.001,-224],[0,-448.001],[0,48],[47.999,0],[48,0]]){
      const position=[m.f32(ns+0x484)+dx,m.f32(ns+0x488)+dy];position.forEach((v,i)=>{flt(point+i*4,v);m.f32(target+i*4,v);});size.forEach((v,i)=>{flt(point+12+i*4,v);m.f32(target+12+i*4,v);});m.reg('ESI',target+12);
      const damage=m.call(0x406e90,{ecx:target,edx:ns});assert.equal(c.bomb_damage_box(f,point,point+12),damage,label+' beam hitbox '+JSON.stringify([size,dx,dy]));++checks;
     }
     if(!m.u32(ns+0x3c)){ended=true;break;}
     if(m.u32(ns+0x480)){c.bomb_mesh_copy(f);m.reg('EBX',m.u32(ns+0x480));m.call(0x40e890);compare(label+' draw strips');}
     o.update(false);assert.equal(c.anm_manager_update(manager,0),1);compare(label+' ANM');
    }
    // Native 407ac0 only interrupts at elapsed == 200. A 1.5 rate skips
    // that exact integer; both implementations remain active. Normal/slow
    // gameplay must terminate, while this synthetic rate must match native.
    assert.equal(ended,!(selection===1&&speed===1.5),name+' native termination');++scenarios;
   }finally{c.release(point);c.bomb_delete(f);c.anm_manager_delete(manager);}
  }c.release(sd);c.sht_delete(sht);c.release(ad);c.anm_delete(an);
 }
 report('bomb-controller'+(process.env.TH11_BOMB_SELECTIONS?'-partial':''),{passed:true,scenarios,frames,checks,selections,scope:'Original activation/update and real ANM execution; normal/slow lifetimes; synthetic fast-rate behavior including native skipped frame200; shield natural expiry and hit-triggered cancellation; duplicate activation; spell boundaries59/60; exact cancellation/sound/refund/rebuild calls, background tint, player flags, damage-area creation, deformation meshes and special damage. Cancellation, power refund and option rebuild are observed boundaries. Not a full-game validation.'});
 }finally{m.close();}
});
