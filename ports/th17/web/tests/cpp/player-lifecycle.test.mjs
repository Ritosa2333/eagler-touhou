import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report,enableNativeMath} from './helpers.mjs';
import {resourceLoader,nativePool,prepareBank,retireOriginalAnimations} from './anm-oracle.mjs';
test('TH17 lifecycle preserves native respawn trajectory, animal drops, deathbomb and roaring branches',async()=>{
 const c=await core(),m=await oracle({memoryEnd:0x10000000,heapStart:0x08000000,heapLimit:0x0f000000}),manager=c.anm_manager_create(),nativeManager=nativePool(m),loader=resourceLoader(m);
 const owner=m.allocate(0x19100),effects=m.allocate(0x2020),spell=m.allocate(0x90),bomb=m.allocate(0x40),enemies=m.allocate(0x200),text=m.allocate(0x19270),hud=m.allocate(0x200),replay=m.allocate(0x20),scratch=m.allocate(8);
 enableNativeMath(m);for(const [address,p]of [[0x4b77d0,owner],[0x4b7698,effects],[0x4b7690,spell],[0x4b7688,bomb],[0x4b76a0,enemies],[0x4b7678,text],[0x4b76ac,hud],[0x4b77d8,replay],[0x4a0430,0x4b5918]])m.u32(address,p);
 let events=[],lifeEvents=[],can=false,cases=0,checks=0;const push=(...v)=>{events.push(...v.map(x=>x>>>0));return 0;},pos=p=>Array.from({length:3},(_,i)=>m.u32(p+i*4));
 m.replace(0x465450,'hit audio',()=>{lifeEvents.push(1,m.i32(m.reg('ESP')+4),0);return 0;},2);
 for(const [address,kind]of [[0x42fc60,2],[0x42fd50,3]])m.replace(address,'life/bomb HUD',()=>{lifeEvents.push(kind,m.i32(m.reg('ESP')+4),m.i32(m.reg('ESP')+8));return 0;},2);
 m.replace(0x411d40,'bomb availability boundary',()=>{push(1);return can?1:0;});m.replace(0x411c30,'begin bomb boundary',()=>push(2));m.replace(0x446ff0,'movement boundary',()=>push(3));m.replace(0x449630,'option update boundary',()=>push(8));m.replace(0x4447e0,'game over boundary',()=>push(9));m.replace(0x40f6d0,'end roaring boundary',()=>push(12,m.u32(m.reg('ESP')+4)),1);m.replace(0x435b30,'all laser cancellation boundary',()=>push(6,m.u32(m.reg('ESP')+4),0),2);
 m.replace(0x434740,'death item boundary',()=>{const sp=m.reg('ESP');return push(7,m.u32(sp+4),...pos(m.u32(sp+8)),m.u32(sp+16),m.u32(sp+20));},8);
 const observeFloat=(address,argc,fn)=>{
  const entry=m.registerImport({dll:'fixture',name:'float service '+address.toString(16),argc,handler:fn}),cave=m.allocate(32),code=Buffer.from([0xf3,0x0f,0x11,0x15,0,0,0,0,0xf3,0x0f,0x11,0x1d,0,0,0,0,0xe9,0,0,0,0]);code.writeUInt32LE(scratch,4);code.writeUInt32LE(scratch+4,12);code.writeInt32LE(entry-cave-code.length,17);m.write(cave,code);const jump=Buffer.from([0xe9,0,0,0,0]);jump.writeInt32LE(cave-address-5,1);m.write(address,jump);
 };
 observeFloat(0x435ab0,3,()=>{const sp=m.reg('ESP');return push(4,...pos(m.u32(sp+4)),m.u32(scratch),m.u32(sp+8),m.u32(sp+12));});
 for(const [address,all]of [[0x419df0,0],[0x419cf0,1]])observeFloat(address,2,()=>{const sp=m.reg('ESP');return push(5,...pos(m.u32(sp+4)),m.u32(scratch),m.u32(sp+8),all);});
 observeFloat(0x449c80,3,()=>{const sp=m.reg('ESP');return push(10,...pos(m.u32(sp+4)),m.u32(scratch),m.u32(scratch+4),m.u32(sp+8),m.u32(sp+12));});
 // Exit after the unchanged native lifecycle switch. Damage, animation,
 // collision and shooting frame passes have independent oracle coverage.
 const exit=m.allocate(16);m.write(exit,Buffer.from([0x5f,0x5e,0x89,0xec,0x5d,0xc3]));const jump=Buffer.from([0xe9,0,0,0,0]);jump.writeInt32LE(exit-0x447ef3-5,1);m.write(0x447ef3,jump);
 const load=(name,id)=>{const b=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(b.length);memory(c,p,b.length).set(b);assert.equal(c.anm_manager_load(manager,id,p,b.length),1,string(c,c.anm_manager_error(manager)));c.deallocate(p);return prepareBank(m,loader,name,b,nativeManager,id);};
 const nativeSession=[0x4b5a40,0x4b5a44,0x4b5a18,enemies+0x3c,enemies+0x44,bomb+0x30,0x4b5a30,0x4b5a38,0x4b5a4c,0x4b5a50,0x4b5a54,0x4b59f8,replay+12];
 try{
  m.u32(text+0x19248,load('ascii.anm',2));assert.equal(c.anm_manager_fallback(manager,2),1);m.u32(effects+12,load('effect.anm',8));
  for(let character=0;character<3;character++){
   const bank=load('pl0'+character+'.anm',9+character);
   for(let state=0;state<6;state++)for(let scenario=0;scenario<144;scenario++){
    const age=[-1,0,3,7,8,15,29,30,31,59,60,61][scenario%12],rate=[1,.5,.25,1.25][Math.floor(scenario/12)%4],pressed=scenario%3?2:0;
    m.f32(0x4b5918,rate);m.view(owner,0x19100).fill(0);m.view(effects+0x18,0x1004).fill(0);m.u32(owner+12,bank);m.call(0x407230,{ecx:bank,args:[owner+16,0]});m.call(0x466de0,{ecx:owner+16});const f=c.player_life_create(manager,9+character),dv=()=>new DataView(c.memory.buffer);events=[];lifeEvents=[];can=scenario%4!==0;
    dv().setInt32(c.player_life_field(f,0),state,true);m.i32(owner+0x18db0,state);const timer=Buffer.alloc(20);timer.writeInt32LE(age-1);timer.writeInt32LE(age,4);timer.writeFloatLE(age,8);timer.writeUInt32LE(scenario%2,16);memory(c,c.player_life_field(f,1),20).set(timer);m.write(owner+0x624,timer);m.write(owner+0x18e78,Buffer.from(memory(c,c.player_life_field(f,2),20)));
    const values=[scenario%3-1,19,17,3,5,1,100+(scenario%4)*100,100,scenario%4-1,2,[-1,0,3,9][Math.floor(scenario/4)%4],scenario%3,scenario%2];for(let i=0;i<13;i++){dv().setInt32(c.player_life_field(f,3)+i*4,values[i],true);m.i32(nativeSession[i],values[i]);}
    for(let i=0;i<4;i++){const value=[0x23,scenario%2?59:60,700,scenario%3?0:100][i];dv().setInt32(c.player_life_field(f,4)+i*4,value,true);m.i32(spell+[0x78,0x24,0x7c,0x74][i],value);}
    const flags=[0,2,6,14][Math.floor(scenario/12)%4];dv().setUint32(c.player_life_field(f,11),flags,true);m.u32(0x4b5ac4,flags);m.write(0x4b5aa4,Buffer.from(memory(c,c.player_life_field(f,10),20)));
    for(let k=0;k<3;k++){const value=[Math.fround((scenario-72)*4.125),Math.fround(300+(scenario%8)*.25),0][k];dv().setFloat32(c.player_life_field(f,6)+k*4,value,true);m.f32(owner+0x610+k*4,value);dv().setFloat32(c.player_lifecycle_field(f,0)+k*4,[10,200,0][k],true);m.f32(owner+0x19020+k*4,[10,200,0][k]);}
    for(let k=0;k<2;k++){dv().setInt32(c.player_lifecycle_field(f,1)+k*4,[7000,40000][k],true);m.i32(owner+0x61c+k*4,[7000,40000][k]);}m.u32(0x4b3454,pressed);
    assert.equal(c.player_lifecycle_step(f,pressed,rate,1,can),1,string(c,c.player_life_error(f)));m.call(0x447970,{ecx:owner});const label=character+'/'+state+'/'+scenario;
    assert.equal(dv().getInt32(c.player_life_field(f,0),true),m.i32(owner+0x18db0),label+' state');for(const[field,no]of [[1,0x624],[2,0x18e78]])assert.deepEqual(Buffer.from(memory(c,c.player_life_field(f,field),20)),Buffer.from(m.bytes(owner+no,20)),label+' timer '+field);
    for(let i=0;i<13;i++)assert.equal(dv().getInt32(c.player_life_field(f,3)+i*4,true),m.i32(nativeSession[i]),label+' session '+i);for(let i=0;i<4;i++)assert.equal(dv().getInt32(c.player_life_field(f,4)+i*4,true),m.i32(spell+[0x78,0x24,0x7c,0x74][i]),label+' spell '+i);
    for(const[cp,np,n]of [[c.player_life_field(f,6),owner+0x610,12],[c.player_lifecycle_field(f,0),owner+0x19020,12],[c.player_lifecycle_field(f,1),owner+0x61c,8],[c.player_lifecycle_field(f,3),owner+0x18edc,264],[c.player_life_field(f,10),0x4b5aa4,20]])assert.deepEqual(Buffer.from(memory(c,cp,n)),Buffer.from(m.bytes(np,n)),label+' field '+np.toString(16));
    assert.equal(dv().getFloat32(c.player_lifecycle_field(f,2),true),m.f32(0x4b5918),label+' rate');assert.deepEqual(Array.from(new Uint32Array(c.memory.buffer,c.player_lifecycle_events(f),c.player_lifecycle_count(f))),events,label+' world requests');assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.player_life_events(f),c.player_life_count(f)*3)),lifeEvents,label+' life requests');
    for(let i=0;i<12;i++){assert.equal(dv().getInt32(c.player_life_option_active(f,i),true),m.i32(owner+0x660+i*0xe4),label+' option '+i);assert.equal(dv().getInt32(c.player_life_option_snap(f,i),true),m.i32(owner+0x734+i*0xe4),label+' snap '+i);}
    retireOriginalAnimations(c,m,manager,nativeManager);c.player_life_delete(f);cases++;checks+=50;
   }
  }
  report('player-lifecycle-state',{passed:true,characters:3,cases,checks,originalFunctions:['0x447970..0x447ef3','0x4491b0','0x4495e0','0x446280','0x4436b0'],scope:'Unchanged native lifecycle switch: respawn trajectory and history, spell-100 clear, laser/bullet cancellation requests, bomb and roaring death saves, death and power/animal drops, game-over/replay and bomb reset. Actual world services are observed boundaries, with native life transitions/animation construction executed. Full PlayerFrame and animal/bomb manager integration remain separate.'});
 }finally{c.anm_manager_delete(manager);m.close();}
});
