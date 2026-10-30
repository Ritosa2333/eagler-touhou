import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report,enableNativeMath} from './helpers.mjs';
import {resourceLoader,nativePool,nativeInstances,prepareBank,compareAnm,retireOriginalAnimations} from './anm-oracle.mjs';
import {originalSht} from './player-shot-oracle.mjs';

test('TH17 shot contacts preserve native damage, slash overlap callbacks, laser length and roaring rewards',async()=>{
 const c=await core(),m=await oracle({memoryEnd:0x10000000,heapStart:0x08000000,heapLimit:0x0f000000}),manager=c.anm_manager_create(),nativeManager=nativePool(m),loader=resourceLoader(m);
 const owner=m.allocate(0x19100),text=m.allocate(0x19270),effects=m.allocate(0x2020),itemManager=m.allocate(0xe4ba00),itemNode=m.allocate(0xc90),query=c.damage_query_create(),position=m.allocate(12),size=m.allocate(8);
 enableNativeMath(m);m.u32(0x4b77d0,owner);m.u32(0x4b7678,text);m.u32(0x4b7698,effects);m.u32(0x4b76b8,itemManager);m.u32(itemManager+0xe4b968,itemNode);m.u32(0x4a0430,0x4b5918);m.f32(0x4b5918,1);
 let items=[],sounds=[],checks=0,hits=0,cases=0;const kinds=new Set;
 m.replace(0x4654f0,'shot sound',()=>{sounds.push([m.i32(m.reg('ESP')+4),0]);return 0;},1);
 // Observe the item request, then execute the original vector constructor.
 // All reward decisions, random draws and item setup stay in native code.
 const observe=m.registerImport({dll:'fixture',name:'observe item request',argc:0,handler:()=>{const sp=m.reg('ESP'),out=Buffer.alloc(24);out.writeInt32LE(9);Buffer.from(m.bytes(m.reg('ECX')-12,12)).copy(out,4);out.writeUInt32LE(m.u32(sp+44),16);out.writeUInt32LE(m.u32(sp+48),20);items.push(out);return 0;}}),cave=m.allocate(64),original=Buffer.from(m.bytes(0x434cf0,7));
 const code=Buffer.concat([Buffer.from([0x9c,0x60,0xb8,0,0,0,0,0xff,0xd0,0x61,0x9d]),original,Buffer.from([0xe9,0,0,0,0])]);code.writeUInt32LE(observe,3);code.writeInt32LE(0x434cf7-cave-code.length,code.length-4);m.write(cave,code);const jump=Buffer.from([0xe9,0,0,0,0,0x90,0x90]);jump.writeInt32LE(cave-0x434cf0-5,1);m.write(0x434cf0,jump);
 const load=(name,id)=>{const b=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(b.length);memory(c,p,b.length).set(b);assert.equal(c.anm_manager_load(manager,id,p,b.length),1,string(c,c.anm_manager_error(manager)));c.deallocate(p);return prepareBank(m,loader,name,b,nativeManager,id);};
 const bits=v=>{const b=Buffer.alloc(4);b.writeFloatLE(v);return b.readUInt32LE();},equalBytes=(a,b,label)=>{for(let i=0;i<a.length;i++)if(a[i]!==b[i])assert.fail(label+' byte '+i.toString(16)+' actual='+a.subarray(Math.max(0,i-8),i+16).toString('hex')+' original='+b.subarray(Math.max(0,i-8),i+16).toString('hex'));};
 try{
  m.u32(text+0x19248,load('ascii.anm',2));assert.equal(c.anm_manager_fallback(manager,2),1);m.u32(effects+12,load('effect.anm',8));
  for(let character=0;character<3;character++){
   const bank=load('pl0'+character+'.anm',9+character);
   for(let animal=0;animal<3;animal++){
    const name='pl0'+character+String.fromCharCode(97+animal),bytes=readFileSync(resolve(root,'reference/assets',name+'.sht')),nativeSht=originalSht(m,bytes),sht=c.sht_create(),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.sht_open(sht,p,bytes.length),1);c.deallocate(p);
    for(let group=0;group<20;group++)for(let si=0;si<c.sht_count(sht,group);si++){
     const spec=c.sht_shot(sht,group,si),kind=new DataView(c.memory.buffer).getUint32(spec+0x34,true);kinds.add(kind);
     for(let sample=0;sample<4;sample++){
      const f=c.player_shots_create(sht,manager,9+character),data=c.player_shots_data(f),damage=c.player_shots_damage(f),focus=group>=5&&group<10?1:0,flags=sample%2?2:0;
      m.view(owner,0x19100).fill(0);m.u32(owner+12,bank);m.u32(owner+0x1903c,nativeSht);m.write(owner+0x1110,Buffer.from(memory(c,data,256*0xe0)));m.write(owner+0xf114,Buffer.from(memory(c,damage,256*0x9c)));
      c.player_shots_options(f,focus,(group%5)*100,100,1);c.player_shots_roaring(f,flags,group>=15?3:1);m.i32(owner+0x18dd0,focus);m.i32(0x4b5a30,(group%5)*100);m.i32(0x4b5a38,100);m.u32(0x4b5ac4,flags);
      for(const [cp,np,seed]of [[c.player_shots_random(f),0x4b7668,0x5144+sample],[c.player_shots_visual_random(f),0x4b7660,7+sample]]){new DataView(c.memory.buffer).setUint32(cp,seed,true);new DataView(c.memory.buffer).setUint32(cp+4,0,true);m.u32(np,seed);m.u32(np+4,0);}
      const dv=()=>new DataView(c.memory.buffer);for(let k=0;k<3;k++){dv().setFloat32(c.player_shots_position(f)+k*4,[12,300,0][k],true);m.f32(owner+0x610+k*4,[12,300,0][k]);}
      for(let i=0;i<12;i++){const x=(i-3)*3000,y=360*128-i*1800;dv().setInt32(c.player_shots_option(f,i),x,true);dv().setInt32(c.player_shots_option(f,i)+4,y,true);m.i32(owner+0x6bc+i*0xe4,x);m.i32(owner+0x6c0+i*0xe4,y);dv().setFloat32(c.player_shots_angles(f)+i*4,-1.5707964,true);m.f32(owner+0x708+i*0xe4,-1.5707964);}
      sounds=[];items=[];const id=(group<<8)|si;m.call(0x44a180,{ecx:owner+0x1110,args:[id,0,owner+0x610]});assert.equal(c.player_shots_spawn(f,id),0,string(c,c.player_shots_error(f)));
      const counter=sample%2?207+sample:0;dv().setInt32(c.player_shots_rewards(f),counter,true);m.i32(owner+0x190ec,counter);
      // Four shapes/ages cover both laser clipping variants and the even-frame
      // impact effect, while each real SHT entry retains its actual callback.
      const age=[0,1,4,8][sample];dv().setInt32(data+0xc,age-1,true);dv().setInt32(data+0x10,age,true);m.i32(owner+0x111c,age-1);m.i32(owner+0x1120,age);
      const rectangle=sample>=2,pos=[m.f32(owner+0x1158)+(sample-1)*8,m.f32(owner+0x115c)-24-sample*8,0],shape=[18+sample,24+sample],angle=Math.fround((sample-1)*.25),radius=3.25;
      for(let k=0;k<3;k++){m.f32(position+k*4,pos[k]);dv().setFloat32(c.damage_query_field(query,0)+k*4,pos[k],true);}for(let k=0;k<2;k++){m.f32(size+k*4,shape[k]);dv().setFloat32(c.damage_query_field(query,1)+k*4,shape[k],true);}dv().setFloat32(c.damage_query_field(query,2),radius,true);dv().setFloat32(c.damage_query_field(query,3),angle,true);
      const sourceIndex=m.i32(owner+0x11c0)-1,nativeSource=owner+0xf114+sourceIndex*0x9c;
      const expected=m.call(0x44b650,{ecx:nativeSource,edx:position,args:[rectangle?size:0,bits(angle),bits(radius)]})|0,actual=c.player_shots_hit(f,sourceIndex,query,rectangle);
      const label=name+'/'+group+'/'+si+'/'+sample;assert.equal(string(c,c.player_shots_error(f)),'',label);assert.equal(actual,expected,label+' damage');hits++;
      equalBytes(Buffer.from(memory(c,data,256*0xe0)),Buffer.from(m.bytes(owner+0x1110,256*0xe0)),label+' shots');equalBytes(Buffer.from(memory(c,damage,256*0x9c)),Buffer.from(m.bytes(owner+0xf114,256*0x9c)),label+' sources');
      assert.equal(dv().getInt32(c.player_shots_rewards(f),true),m.i32(owner+0x190ec),label+' reward counter');assert.equal(c.player_shots_item_count(f),items.length,label+' item count');if(items.length)equalBytes(Buffer.from(memory(c,c.player_shots_items(f),items.length*24)),Buffer.concat(items),label+' items');
      assert.deepEqual(Buffer.from(memory(c,c.player_shots_random(f),8)),Buffer.from(m.bytes(0x4b7668,8)),label+' game random');assert.deepEqual(Buffer.from(memory(c,c.player_shots_visual_random(f),8)),Buffer.from(m.bytes(0x4b7660,8)),label+' visual random');
      assert.equal(c.player_shots_sound_count(f),sounds.length,label+' sounds');for(let i=0;i<sounds.length;i++)assert.deepEqual([dv().getInt32(c.player_shots_sound(f,i),true),dv().getInt32(c.player_shots_sound(f,i)+4,true)],sounds[i],label+' sound '+i);
      for(let alternate=0;alternate<2;alternate++){
       const list=nativeInstances(m,nativeManager,alternate);assert.equal(c.anm_manager_count(manager,alternate),list.length,label+' queue');for(let i=0;i<list.length;i++){const h=m.u32(list[i]+0x538);assert.equal(c.anm_manager_order(manager,alternate,i),h,label+' order');const vm=c.anm_manager_find(manager,h);compareAnm(c,m,vm,list[i],label+' ANM '+h);assert.deepEqual(Buffer.from(memory(c,c.anm_vm_translation(vm),12)),Buffer.from(m.bytes(list[i]+0x5e4,12)),label+' translation');checks++;}
      }
      checks+=8;cases++;retireOriginalAnimations(c,m,manager,nativeManager);c.player_shots_delete(f);
     }
    }c.sht_delete(sht);
   }
  }
  report('player-shot-hits',{passed:true,cases,hits,checks,collisionKinds:[...kinds].sort(),originalFunctions:['0x44a180','0x44b650','0x44b710','0x44b9c0','0x44cc40','0x44d280','0x44d880','0x44c140','0x44d390','0x44d8a0'],scope:'All actual shot specs with circle/rectangle contacts, root-only hit interruption, laser clipping and timed impact animations, two explosion variants, slash callback damage, roaring reward counts/item requests and exact random advancement. Full item lifetime and enemy/roaring scene ownership remain separate integration.'});
 }finally{c.damage_query_delete(query);c.anm_manager_delete(manager);m.close();}
});
