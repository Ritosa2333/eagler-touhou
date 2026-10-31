import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report,enableNativeMath} from './helpers.mjs';
import {resourceLoader,nativePool,nativeInstances,prepareBank,compareAnm,retireOriginalAnimations} from './anm-oracle.mjs';

const callbackTables=[
 [0,0x44b7c0,0x44c5e0,0x44d510,0x44bb20,0x44d6b0],
 [0,0x44b7d0,0x44c660,0x44d260,0x44d850,0x44bba0,0x44d870],
 [0,0],[0,0x44b9c0,0x44cc40,0x44d280,0x44d880,0x44c140,0x44d390]
];
export function originalSht(m,source){
 const p=m.allocate(source.length);m.write(p,source);
 for(let group=0;group<source.readUInt16LE(2);group++){
  let at=0x130+source.readUInt32LE(0xe0+group*4);m.u32(p+0xe0+group*4,p+at);
  while(source.readInt8(at)>=0){for(let k=0;k<4;k++)m.u32(p+at+0x28+k*4,callbackTables[k][source.readUInt32LE(at+0x28+k*4)]);at+=0x58;}
 }return p;
}
export async function verifyPlayerShots({simulate=false,withTargets=false,dynamicAngles=false,viewport=false}={}){
 const c=await core(),m=await oracle({memoryEnd:0x10000000,heapStart:0x08000000,heapLimit:0x0f000000});
 const manager=c.anm_manager_create(),nativeManager=nativePool(m),loader=resourceLoader(m),owner=m.allocate(0x19100),text=m.allocate(0x19270),effects=m.allocate(0x2020),enemies=m.allocate(0x200),bomb=m.allocate(0x200);
 enableNativeMath(m);m.u32(0x4b77d0,owner);m.u32(0x4b7678,text);m.u32(0x4b7698,effects);m.u32(0x4a0430,0x4b5918);m.f32(0x4b5918,1);m.u32(0x4b76ac,bomb);
 const audio=m.allocate(4),audioVtable=m.allocate(0x44);let sounds=[],frames=0,activeShots=0,checks=0,specifications=0;
 m.u32(audio,audioVtable);m.u32(0x526454,audio);m.u32(audioVtable+0x40,m.registerImport({dll:'fixture',name:'laser pan',argc:2,handler:()=>{sounds.push([20,2]);return 0;}}));
 m.replace(0x4654f0,'shot audio request',()=>{sounds.push([m.i32(m.reg('ESP')+4),0]);return 0;},1);
 m.replace(0x4655d0,'shot audio stop',()=>{sounds.push([m.i32(m.reg('ESP')+4),1]);return 0;},1);
 const nodes=m.allocate(8*12),targets=Array.from({length:8},()=>m.allocate(0x5770));
 for(let i=0;i<8;i++){m.u32(nodes+i*12,targets[i]);m.u32(nodes+i*12+4,i<7?nodes+(i+1)*12:0);}m.u32(enemies+0x180,nodes);
 const equalBytes=(a,b,label)=>{assert.equal(a.length,b.length,label+' size');for(let i=0;i<a.length;i++)if(a[i]!==b[i]){const base=Math.max(0,i-8);assert.fail(label+' byte '+i.toString(16)+' actual='+a.subarray(base,i+16).toString('hex')+' original='+b.subarray(base,i+16).toString('hex'));}};
 const load=(name,id)=>{const bytes=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.anm_manager_load(manager,id,p,bytes.length),1,name+': '+string(c,c.anm_manager_error(manager)));c.deallocate(p);return prepareBank(m,loader,name,bytes,nativeManager,id);};
 if(viewport){m.i32(0x52471c,128);m.i32(0x524720,16);for(const [i,v]of [320,16,320,16].entries())m.i32(0x52472c+i*4,v);new Int32Array(c.memory.buffer,c.anm_environment_offsets(c.anm_manager_environment(manager)),4).set([320,16,320,16]);}
 // Execute the original damage-pool update, with its original instructions.
 const tickCall=m.allocate(64),setup=Buffer.from([0x55,0x89,0xe5,0x83,0xec,0x40,0x53,0x56,0x57,0x89,0xcf,0xb9,0,1,0,0,0xe9,0,0,0,0,0x5f,0x5e,0x5b,0x89,0xec,0x5d,0xc3]);setup.writeInt32LE(0x447ef3-(tickCall+21),17);m.write(tickCall,setup);const leave=Buffer.from([0xe9,0,0,0,0]);leave.writeInt32LE(tickCall+21-0x448047-5,1);m.write(0x448047,leave);
 // Keep iteration in the guest so each active shot still executes the exact
 // native routine without a host round trip for every individual projectile.
 const shotUpdate=m.allocate(64),loop=Buffer.from([0x53,0x56,0x89,0xce,0x81,0xc6,0x10,0x11,0,0,0xbb,0,1,0,0,0x83,0xbe,0x8c,0,0,0,0,0x74,7,0x89,0xf1,0xe8,0,0,0,0,0x81,0xc6,0xe0,0,0,0,0x4b,0x75,0xe7,0x5e,0x5b,0xc3]);loop.writeInt32LE(0x44ad70-shotUpdate-31,27);m.write(shotUpdate,loop);
 try{
  const ascii=load('ascii.anm',2);assert.equal(c.anm_manager_fallback(manager,2),1);m.u32(text+0x19248,ascii);m.u32(effects+12,load('effect.anm',8));
  for(let character=0;character<3;character++){
   const bank=load('pl0'+character+'.anm',9+character);
   for(let animal=0;animal<3;animal++){
    const name='pl0'+character+String.fromCharCode(97+animal),source=Buffer.from(readFileSync(resolve(root,'reference/assets',name+'.sht'))),sht=c.sht_create();
    if(dynamicAngles)for(let group=0;group<20;group++){let at=0x130+source.readUInt32LE(0xe0+group*4);while(source.readInt8(at)>=0){if(source.readInt8(at+32)&15)source.writeFloatLE(group%2?995:1000,at+20);at+=0x58;}}
    const nativeSht=originalSht(m,source),bytes=c.allocate(source.length);memory(c,bytes,source.length).set(source);assert.equal(c.sht_open(sht,bytes,source.length),1);c.deallocate(bytes);
    for(let group=0;group<20;group++)for(let rateIndex=0;rateIndex<2;rateIndex++){
     m.view(owner,0x19100).fill(0);m.u32(owner+0x1903c,nativeSht);m.u32(owner+12,bank);
     const f=c.player_shots_create(sht,manager,9+character),data=c.player_shots_data(f),damage=c.player_shots_damage(f),focus=group>=5&&group<10?1:0,power=(group%5)*100,rate=[1,.5][rateIndex],flags=group>=10?2:0,kind=group>=15?3:1;
     specifications+=c.sht_count(sht,group);m.write(owner+0x1110,Buffer.from(memory(c,data,256*0xe0)));m.write(owner+0xf114,Buffer.from(memory(c,damage,256*0x9c)));
     c.player_shots_options(f,focus,power,100,rate);c.player_shots_roaring(f,flags,kind);m.i32(owner+0x18dd0,focus);m.i32(0x4b5a30,power);m.i32(0x4b5a38,100);m.f32(0x4b5918,rate);m.u32(0x4b5ac4,flags);m.i32(0x4b5abc,kind);
     for(const [p,seed]of [[c.player_shots_random(f),0x5144],[c.player_shots_visual_random(f),0]]){new DataView(c.memory.buffer).setUint32(p,seed,true);new DataView(c.memory.buffer).setUint32(p+4,0,true);}m.u32(0x4b7668,0x5144);m.u32(0x4b766c,0);m.u32(0x4b7660,0);m.u32(0x4b7664,0);sounds=[];
     if(withTargets)c.player_shots_targets(f,8);
     for(let frame=0;frame<(simulate?96:24);frame++){
      let dv=new DataView(c.memory.buffer);const px=(frame-12)*.25,py=360+group*.5;
      for(let k=0;k<3;k++){dv.setFloat32(c.player_shots_position(f)+k*4,[px,py,0][k],true);m.f32(owner+0x610+k*4,[px,py,0][k]);}
      for(let i=0;i<12;i++){
       const x=(i-3)*3100+frame*3,y=360*128-i*1800;dv.setInt32(c.player_shots_option(f,i),x,true);dv.setInt32(c.player_shots_option(f,i)+4,y,true);m.i32(owner+0x6bc+i*0xe4,x);m.i32(owner+0x6c0+i*0xe4,y);
       const angle=Math.fround(-1.5707964+(i-3)*.0625);dv.setFloat32(c.player_shots_angles(f)+i*4,angle,true);m.f32(owner+0x708+i*0xe4,angle);
      }
      c.player_shots_context(f,frame,1,0,viewport?128:0,viewport?16:0);m.i32(owner+0x18dd8,frame);m.u32(0x4b76a0,enemies);dv.setInt32(c.player_shots_option_count(f),group%5,true);m.i32(owner+0x18dfc,group%5);
      if(withTargets)for(let i=0;i<8;i++){
       const pos=[Math.fround((i-4)*30+(frame%23-11)*2),Math.fround(350+i*4-(frame%17)),0],bits=frame%19<4&&i%2?0x21:frame%13<4&&i%3===0?0xc000000:0,id=i+1+(frame>=64?10:0);
       for(let k=0;k<3;k++){dv.setFloat32(c.player_shots_target(f,i,0)+k*4,pos[k],true);m.f32(targets[i]+0x1250+k*4,pos[k]);}dv.setUint32(c.player_shots_target(f,i,1),bits,true);dv.setUint32(c.player_shots_target(f,i,2),id,true);m.u32(targets[i]+0x528c,bits);m.u32(targets[i]+0x5760,id);
      }
      if(!withTargets)m.u32(enemies+0x180,0);else m.u32(enemies+0x180,nodes);
      m.call(0x44a710,{ecx:owner,args:[frame,frame+3]});assert.equal(c.player_shots_fire(f,frame,frame+3),1,string(c,c.player_shots_error(f)));
      if(simulate){
       m.call(shotUpdate,{ecx:owner});assert.equal(c.player_shots_update(f),1,string(c,c.player_shots_error(f)));
       m.call(tickCall,{ecx:owner});c.damage_sources_tick_for_shots(f,rate);
       for(let alternate=0;alternate<2;alternate++){m.call(alternate?0x475940:0x475830,{ecx:nativeManager});assert.equal(c.anm_manager_update(manager,alternate,rate),1,string(c,c.anm_manager_error(manager)));}
      }
      const label=name+'/'+group+'/'+rate+'/'+frame;equalBytes(Buffer.from(memory(c,data,256*0xe0)),Buffer.from(m.bytes(owner+0x1110,256*0xe0)),label+' shot pool');equalBytes(Buffer.from(memory(c,damage,256*0x9c)),Buffer.from(m.bytes(owner+0xf114,256*0x9c)),label+' damage pool');
      assert.deepEqual(Buffer.from(memory(c,c.player_shots_locks(f),52)),Buffer.from(m.bytes(owner+0x18e00,52)),label+' laser locks');assert.deepEqual(Buffer.from(memory(c,c.player_shots_random(f),8)),Buffer.from(m.bytes(0x4b7668,8)),label+' game random');assert.deepEqual(Buffer.from(memory(c,c.player_shots_visual_random(f),8)),Buffer.from(m.bytes(0x4b7660,8)),label+' visual random');assert.equal(new DataView(c.memory.buffer).getInt32(c.player_shots_cursor(f),true),m.i32(owner+0xf110),label+' damage cursor');
      assert.equal(c.player_shots_sound_count(f),sounds.length,label+' sound count');for(let i=0;i<sounds.length;i++){dv=new DataView(c.memory.buffer);assert.deepEqual([dv.getInt32(c.player_shots_sound(f,i),true),dv.getInt32(c.player_shots_sound(f,i)+4,true)],sounds[i],label+' audio '+i);}
      for(let alternate=0;alternate<2;alternate++){
       const list=nativeInstances(m,nativeManager,alternate);assert.equal(c.anm_manager_count(manager,alternate),list.length,label+' queue '+alternate);
       for(let i=0;i<list.length;i++){const h=m.u32(list[i]+0x538);assert.equal(c.anm_manager_order(manager,alternate,i),h,label+' order '+i);const vm=c.anm_manager_find(manager,h);compareAnm(c,m,vm,list[i],label+' ANM '+h);assert.deepEqual(Buffer.from(memory(c,c.anm_vm_translation(vm),12)),Buffer.from(m.bytes(list[i]+0x5e4,12)),label+' translation '+h);checks++;}
      }frames++;checks+=7;
     }
     activeShots+=Array.from({length:256},(_,i)=>new DataView(c.memory.buffer).getInt32(data+i*0xe0+0x8c,true)).filter(Boolean).length;retireOriginalAnimations(c,m,manager,nativeManager);c.player_shots_delete(f);
    }c.sht_delete(sht);
   }
  }
  const name=viewport?'player-shot-viewport':dynamicAngles?'player-shot-dynamic-angles':withTargets?'player-shot-targets':simulate?'player-shot-frame':'player-shot-generation';
  report(name,{passed:true,characters:3,animals:3,groups:180,frames,activeShots,checks,specifications:specifications/2,originalFunctions:['0x44a710','0x44a180','0x44b7c0','0x44c5e0','0x44bb20','0x44d510','0x44d6b0',...(simulate?['0x44ad70','0x44b7d0','0x44c660','0x44d260','0x44bba0','0x44d870','0x447ef3..0x448047']:[])],scope:'Nine actual SHT and three ANM sets, twenty power/focus/roaring groups, first-free shot allocation, delayed overlapping slash damage, laser locks, animation generation/order and game/visual random streams.'+(simulate?' Includes original per-shot behavior, motion, bounds retirement, damage lifetime and ANM updates.':' Spawn and firing schedules only.')+' Enemy damage callbacks and complete game sessions are separate integration.'});
 }finally{c.anm_manager_delete(manager);m.close();}
}
