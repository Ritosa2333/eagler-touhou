import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
import {normalizeAnimation,firstDifference} from './shot-oracle.mjs';
test('TH11 player shots resolve enemy hits, effects, damage areas and scoring exactly',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m);let states=0,animations=0,cases=0,spawns=0,areas=0;
 let sounds=[];m.replace(0x44a260,'shot sound sink',()=>{sounds.push([m.reg('EDI')|0,m.f32(m.reg('ESP')+4)]);return 0;},1);
 const dv=()=>new DataView(c.memory.buffer),put=(p,v)=>dv().setInt32(p,v,true),flt=(p,v)=>dv().setFloat32(p,v,true),read=p=>dv().getUint32(p,true);
 const spawnCallbacks=[0,0x434e20,0x435210,0x435250],updateCallbacks=[0,0x434e30,0x4352a0,0x435330];
 try{for(const [combination,name]of ['pl00a','pl00b','pl00c','pl01a','pl01b','pl01c'].entries()){
  const {source,original}=o.load(name.slice(0,4)+'.anm'),a=c.anm_create(),ad=c.allocate(source.length);memory(c,ad,source.length).set(source);assert.equal(c.anm_open(a,ad,source.length),1);
  const bytes=readFileSync(resolve(root,'reference/assets',name+'.sht')),sht=c.sht_create(),data=c.allocate(bytes.length);memory(c,data,bytes.length).set(bytes);assert.equal(c.sht_open(sht,data,bytes.length),1);
  const ns=m.allocate(bytes.length);m.write(ns,bytes);const groups=bytes.readUInt16LE(2);
  for(let g=0;g<groups;++g){let off=bytes.readUInt32LE(0x268+g*8);m.u32(ns+0x268+g*8,ns+off);while(bytes.readInt8(off)>=0){m.u32(ns+off+0x24,spawnCallbacks[bytes.readUInt32LE(off+0x24)]);m.u32(ns+off+0x28,updateCallbacks[bytes.readUInt32LE(off+0x28)]);off+=0x34;}}
  const np=m.allocate(0x8d40),special=m.allocate(0x100),target=m.allocate(12),size=m.allocate(8),heap=m.heap;
  for(let g=0;g<groups;++g)for(let mode=0;mode<(g===0&&combination===0?8:4);++mode){
   m.heap=heap;m.view(heap,4000000).fill(0);m.view(np,0x8d40).fill(0);m.view(special,0x100).fill(0);o.reset();sounds=[];
   m.u32(0x4a8eb4,np);m.u32(np+0x10,original);m.u32(np+0x92c,ns);m.u32(0x4a8d64,special);m.u32(0x4a5710,Math.floor(combination/3));m.u32(0x4a5714,combination%3);
   const manager=c.anm_manager_create(),f=c.sm_create(sht,a,manager),p=c.sm_player(f),rate=c.anm_env_rate(manager),ct=c.allocate(20),economy=c.sm_economy(f);
   for(const vis of [0,1]){memory(c,c.anm_env_rng(manager,vis),8).fill(0);put(c.anm_env_rng(manager,vis),12345);}
   put(p+160,Math.floor(combination/3));put(p+164,combination%3);put(p+140,8);put(p+184,1);m.i32(np+0x7c90,8);
   const boosted=combination===5&&mode===3;put(p+188,boosted?1:0);m.i32(special+0x3c,boosted?1:0);
   const initialScore=mode===2?999999998:98765;put(economy,initialScore);m.i32(0x4a56e4,initialScore);
   for(let i=0;i<8;++i){const option=c.anm_manager_spawn(manager,a,0,22,0),id=read(option);assert.equal(o.spawn(original,0,22),id);
    const native=o.states().find(s=>s.id===id).p;flt(option+0x3c4,-1.7);m.f32(native+0x3c4,-1.7);
    put(p+12+i*16,0);put(p+16+i*16,240*128);flt(p+20+i*16,-1.57);put(p+24+i*16,id);
    m.i32(np+(i+1)*0xe4+0x74ec,240*128);m.f32(np+(i+1)*0xe4+0x7534,-1.57);m.u32(np+(i+1)*0xe4+0x753c,id);
   }
   flt(p,0);flt(p+4,240);flt(p+8,0);m.f32(np+0x87c,0);m.f32(np+0x880,240);
   function compare(label){
    for(let i=0;i<256;++i){const b=Buffer.from(memory(c,c.sm_shot(f,i),0x6c)),a=Buffer.from(m.bytes(np+0x96c+i*0x6c,0x6c));if(!a.readUInt32LE(0x68)&&!b.readUInt32LE(0x68))continue;for(const v of [a,b]){v.writeUInt32LE(0,12);for(const off of [0x54,0x68])v.writeUInt32LE(v.readUInt32LE(off)?1:0,off);}assert.equal(firstDifference(b,a),'',label+` shot${i}`);++states;}
    for(let i=0;i<32;++i){const a=Buffer.from(m.bytes(np+0x7c9c+i*0x74,0x74)),b=Buffer.from(memory(c,c.sm_areas(f)+i*0x74,0x74));if(!a.readUInt32LE(0x70)&&!b.readUInt32LE(0x70))continue;a.writeUInt32LE(0,0x58);b.writeUInt32LE(0,0x58);assert.equal(firstDifference(b,a),'',label+` area${i}`);++areas;}
    const native=o.states();assert.equal(c.anm_manager_count(manager),native.length,label+' animation count');
    for(const item of native){const cp=c.anm_manager_find(manager,item.id);assert.ok(cp);const b=normalizeAnimation(memory(c,cp,0x434),read,read(cp+0x3a4)),a=normalizeAnimation(item.bytes,p=>m.u32(p),m.u32(item.p+0x3a4));assert.equal(firstDifference(b,a),'',label+` animation${item.id}`);++animations;}
    for(const vis of [0,1])assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(manager,vis),8)),Buffer.from(m.bytes(vis?0x4c2ef8:0x4c2f00,8)),label+' RNG');
    assert.equal(read(economy),m.u32(0x4a56e4),label+' score');
    const soundBase=c.sm_sounds(f);assert.deepEqual(Array.from({length:c.sm_sound_count(f)},(_,i)=>[dv().getInt32(soundBase+i*8,true),dv().getFloat32(soundBase+i*8+4,true)]),sounds,label+' sounds');
   }
   try{
    const start=bytes.readUInt32LE(0x268+g*8),count=mode>=4?1:c.sht_count(sht,g);
    if(mode>=4){const type=mode===4?3:4;memory(c,c.sht_shots(sht,g)+29,1)[0]=type;m.view(ns+start+29,1)[0]=type;}
    for(let i=0;i<count;++i){m.reg('EAX',ns+start+i*0x34);m.call(0x433f90,{ecx:np+0x87c,args:[np,0]});assert.notEqual(c.sm_spawn(f,c.sht_shots(sht,g)+i*0x34,0,p),-2);++spawns;
     const cp=c.sm_shot(f,i),native=np+0x96c+i*0x6c;const x=mode===1?200:0,y=mode===2?-8:180;
     flt(cp+0x14,x);flt(cp+0x18,y);m.f32(native+0x14,x);m.f32(native+0x18,y);
    }
    flt(ct,0);flt(ct+4,mode===2?0:180);flt(ct+8,0);flt(ct+12,24);flt(ct+16,24);m.write(target,memory(c,ct,12));m.write(size,memory(c,ct+12,8));
    for(let hit=0;hit<9;++hit){const advanced=hit!==0; m.i32(np+0x944,hit);m.i32(np+0x948,hit+(advanced?1:0));
     for(let i=0;i<count;++i){const cp=c.sm_shot(f,i),native=np+0x96c+i*0x6c;c.timer_set(cp,hit,rate);m.reg('EAX',native);m.call(0x406100,{args:[hit]});if(hit===8){put(cp,hit);m.i32(native,hit);}}
     if(mode>=6){const shot=c.sm_shot(f,0),native=np+0x96c,amount=[0,1,29,30,95,96,97,400,1000][hit];put(shot+0x60,amount);m.i32(native+0x60,amount);put(shot,hit*4-1);put(shot+4,hit*4);m.i32(native,hit*4-1);m.i32(native+4,hit*4);if(mode===7){flt(ct+4,-12.000001);m.f32(target+4,-12.000001);flt(shot+0x18,-8);m.f32(native+0x18,-8);}}
     const expected=m.call(0x4347f0,{args:[target,size]})|0,actual=c.sm_damage(f,ct,ct+12,advanced?1:0);
     const label=`${name} group${g} mode${mode} hit${hit}`;assert.equal(actual,expected,label+` damage error ${c.sm_error(f)}`);compare(label);++cases;
    }
   }finally{if(mode>=4){const start=bytes.readUInt32LE(0x268+g*8);memory(c,c.sht_shots(sht,g)+29,1)[0]=bytes[start+29];m.view(ns+start+29,1)[0]=bytes[start+29];}c.release(ct);c.sm_delete(f);c.anm_manager_delete(manager);}
  }
  c.anm_delete(a);c.release(ad);c.sht_delete(sht);c.release(data);console.log(name+' hit resolution verified');
 }report('shot-damage',{passed:true,cases,spawns,states,animations,areas,scope:'All six shipped SHT resources (types 0/1/2), plus synthetic types 3/4 exercising original explosion creation and laser periodic gates. Native original enemy-hit resolution, exact shot and ANM state, damage decay/compression, boosted type-2 hit, clipping, paused player tick and score saturation. Special bomb damage and full enemy/player integration remain separate.'});
 }finally{m.close();}
});
