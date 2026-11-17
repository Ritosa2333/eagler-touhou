import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report,enableNativeMath} from './helpers.mjs';
import {resourceLoader,nativePool,nativeInstances,prepareBank,compareAnm,retireOriginalAnimations} from './anm-oracle.mjs';
test('TH17 hit, deathbomb cancellation and death preserve native life/bomb/roaring state and animations',async()=>{
 const c=await core(),m=await oracle({memoryEnd:0x10000000,heapStart:0x08000000,heapLimit:0x0f000000}),manager=c.anm_manager_create(),nativeManager=nativePool(m),loader=resourceLoader(m);
 const owner=m.allocate(0x19100),effects=m.allocate(0x2020),spell=m.allocate(0x90),bomb=m.allocate(0x40),enemies=m.allocate(0x200),text=m.allocate(0x19270),hud=m.allocate(0x200),replay=m.allocate(32),out=m.allocate(4);
 enableNativeMath(m);for(const [address,p]of [[0x4b77d0,owner],[0x4b7698,effects],[0x4b7690,spell],[0x4b7688,bomb],[0x4b76a0,enemies],[0x4b7678,text],[0x4b76ac,hud],[0x4b77d8,replay],[0x4a0430,0x4b5918]])m.u32(address,p);m.f32(0x4b5918,1);
 let events=[],transitions=0,frames=0,checks=0;
 m.replace(0x465450,'hit sound request',()=>{events.push(1,m.i32(m.reg('ESP')+4),0);return 0;},2);
 for(const [address,kind]of [[0x42fc60,2],[0x42fd50,3]])m.replace(address,'life/bomb HUD',()=>{events.push(kind,m.i32(m.reg('ESP')+4),m.i32(m.reg('ESP')+8));return 0;},2);
 const load=(name,id)=>{const bytes=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.anm_manager_load(manager,id,p,bytes.length),1,string(c,c.anm_manager_error(manager)));c.deallocate(p);return prepareBank(m,loader,name,bytes,nativeManager,id);};
 const sessionOffsets=[0x4b5a40,0x4b5a44,0x4b5a18,enemies+0x3c,enemies+0x44,bomb+0x30,0x4b5a30,0x4b5a38,0x4b5a4c,0x4b5a50,0x4b5a54,0x4b59f8,replay+12];
 try{
  const ascii=load('ascii.anm',2);assert.equal(c.anm_manager_fallback(manager,2),1);m.u32(text+0x19248,ascii);const effectBank=load('effect.anm',8);m.u32(effects+12,effectBank);
  for(let character=0;character<3;character++){
   const bank=load('pl0'+character+'.anm',9+character);
   for(let scenario=0;scenario<24;scenario++){
    m.view(owner,0x19100).fill(0);m.view(effects+0x18,0x1004).fill(0);m.u32(owner+12,bank);m.call(0x407230,{ecx:bank,args:[owner+16,0]});m.call(0x466de0,{ecx:owner+16});
    const f=c.player_life_create(manager,9+character),rootVm=c.player_life_root(f);events=[];for(const [field,offset]of [[1,0x624],[2,0x18e78]])m.write(owner+offset,Buffer.from(memory(c,c.player_life_field(f,field),20)));
    const dv=()=>new DataView(c.memory.buffer),values=[[2,0,8,-1][scenario%4],scenario*17,scenario%2?999999:19,scenario+2,17,scenario%2,100,100,1,scenario,[-1,0,3,9][scenario%4],scenario%3,scenario%2];
    for(let i=0;i<13;i++){dv().setInt32(c.player_life_field(f,3)+i*4,values[i],true);m.i32(sessionOffsets[i],values[i]);}
    const spellValues=[scenario%3?0x23:0,[0,59,60,99][scenario%4],123456,100];for(let i=0;i<4;i++){dv().setInt32(c.player_life_field(f,4)+i*4,spellValues[i],true);m.i32(spell+[0x78,0x24,0x7c,0x74][i],spellValues[i]);}
    dv().setUint32(c.player_life_field(f,5),scenario%2?8:0,true);m.u32(owner+0x18e8c,scenario%2?8:0);const flags=[0,2,6,14][scenario%4];dv().setUint32(c.player_life_field(f,11),flags,true);m.u32(0x4b5ac4,flags);m.write(0x4b5aa4,Buffer.from(memory(c,c.player_life_field(f,10),20)));
    for(let i=0;i<3;i++){dv().setFloat32(c.player_life_field(f,6)+i*4,(i-1)*73.25,true);m.f32(owner+0x610+i*4,(i-1)*73.25);}dv().setInt32(c.player_life_field(f,9),12,true);m.i32(owner+0x18dfc,12);
    for(let i=0;i<12;i++){
     dv().setInt32(c.player_life_option_active(f,i),2,true);m.i32(owner+0x660+i*0xe4,2);
     for(let k=0;k<2;k++){const h=c.anm_manager_spawn(manager,8,26+k,14,0);m.call(0x405a70,{ecx:effectBank,args:[out,26+k,14,0]});assert.equal(h,m.u32(out));dv().setUint32(c.player_life_option_handles(f)+(i*2+k)*4,h,true);m.u32(owner+0x710+i*0xe4+k*4,h);}
    }
    for(const kind of [0,2,0,1]){
     assert.equal(c.player_life_transition(f,kind),1,string(c,c.player_life_error(f)));m.call([0x449420,0x4491b0,0x4495e0][kind],{ecx:owner});const label=character+'/'+scenario+'/'+kind;
     assert.equal(dv().getInt32(c.player_life_field(f,0),true),m.i32(owner+0x18db0),label+' state');for(const[field,no]of [[1,0x624],[2,0x18e78]])assert.deepEqual(Buffer.from(memory(c,c.player_life_field(f,field),20)),Buffer.from(m.bytes(owner+no,20)),label+' timer');
     for(let i=0;i<13;i++)assert.equal(dv().getInt32(c.player_life_field(f,3)+i*4,true),m.i32(sessionOffsets[i]),label+' session '+i);for(let i=0;i<4;i++)assert.equal(dv().getInt32(c.player_life_field(f,4)+i*4,true),m.i32(spell+[0x78,0x24,0x7c,0x74][i]),label+' spell '+i);
     assert.deepEqual(Buffer.from(memory(c,c.player_life_field(f,10),20)),Buffer.from(m.bytes(0x4b5aa4,20)),label+' roaring countdown');assert.equal(dv().getInt32(c.player_life_field(f,9),true),m.i32(owner+0x18dfc));for(let i=0;i<12;i++)assert.equal(dv().getInt32(c.player_life_option_active(f,i),true),m.i32(owner+0x660+i*0xe4));
     assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.player_life_events(f),c.player_life_count(f)*3)),events,label+' HUD/audio');assert.deepEqual(Buffer.from(memory(c,c.player_life_field(f,7),4096)),Buffer.from(m.bytes(effects+0x1c,4096)),label+' tracked effects');assert.equal(dv().getInt32(c.player_life_field(f,8),true),m.i32(effects+0x18));compareAnm(c,m,rootVm,owner+16,label+' root');
     for(let alternate=0;alternate<2;alternate++){const list=nativeInstances(m,nativeManager,alternate);assert.equal(c.anm_manager_count(manager,alternate),list.length,label+' queue');for(let i=0;i<list.length;i++){const h=m.u32(list[i]+0x538);assert.equal(c.anm_manager_order(manager,alternate,i),h,label+' order');compareAnm(c,m,c.anm_manager_find(manager,h),list[i],label+' effect '+h);checks++;}}
     transitions++;checks+=35;
    }
    for(let frame=0;frame<60;frame++){
     const rate=[1,.5,.25,1.25][scenario%4];m.f32(0x4b5918,rate);for(let alternate=0;alternate<2;alternate++){assert.equal(c.anm_manager_update(manager,alternate,rate),1,string(c,c.anm_manager_error(manager)));m.call(alternate?0x475940:0x475830,{ecx:nativeManager});const list=nativeInstances(m,nativeManager,alternate);assert.equal(c.anm_manager_count(manager,alternate),list.length);for(let i=0;i<list.length;i++){const h=m.u32(list[i]+0x538);assert.equal(c.anm_manager_order(manager,alternate,i),h);compareAnm(c,m,c.anm_manager_find(manager,h),list[i],'tick '+character+'/'+scenario+'/'+frame+'/'+h);checks++;}}frames++;
    }
    retireOriginalAnimations(c,m,manager,nativeManager);c.player_life_delete(f);
   }
  }
  report('player-life-transitions',{passed:true,characters:3,transitions,frames,checks,originalFunctions:['0x449420','0x4491b0','0x4495e0','0x41dea0'],scope:'Native hit, deathbomb cancellation and committed death; spell failure, death count, life and bomb reset/HUD order, four base options versus eight roaring options, roaring countdown, real effect tracking and subsequent ANM updates. Complete lifecycle/world services are separate integration.'});
 }finally{c.anm_manager_delete(manager);m.close();}
});
