import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report,enableNativeMath} from './helpers.mjs';
import {resourceLoader,nativePool,nativeInstances,prepareBank,compareAnm,retireOriginalAnimations} from './anm-oracle.mjs';

const fields=[[0,0x61c,8],[1,0x610,12],[2,0x1902c,8],[3,0x18eb8,8],[4,0x18ea0,12],[5,0x18eac,12],[6,0x18e90,16],[7,0x19034,4],[8,0x18dd0,4],[9,0x63c,4],[10,0x18ec0,4],[11,0x18ec4,4],[12,0x18dfc,4],[13,0x18e8c,4],[14,0x19078,4],[15,0x18ec8,4],[16,0x18ecc,12],[17,0x18dbc,20],[18,0x18edc,264]];
const optionFields=[[0,0x660,4],[1,0x6b4,8],[2,0x6bc,8],[3,0x6c4,8],[4,0x6cc,8],[5,0x734,4],[6,0x730,4],[7,0x72c,4]];
test('TH17 nine character/animal combinations preserve power formations and Youmu history following',async()=>{
 const c=await core(),m=await oracle({memoryEnd:0x10000000,heapStart:0x08000000,heapLimit:0x0f000000});
 const manager=c.anm_manager_create(),nativeManager=nativePool(m),loader=resourceLoader(m),owner=m.allocate(0x19100),text=m.allocate(0x19270),effects=m.allocate(0x2020),enemy=m.allocate(0x200);
 enableNativeMath(m);m.u32(0x4b7678,text);m.u32(0x4b7698,effects);m.u32(0x4b76a0,enemy);m.u32(enemy+0x18c,1);m.u32(0x4b77d0,owner);m.u32(0x4a0430,0x4b5918);m.f32(0x4b5918,1);
 let frames=0,checks=0,configurations=0;
 const load=(name,id)=>{const bytes=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.anm_manager_load(manager,id,p,bytes.length),1,string(c,c.anm_manager_error(manager)));c.deallocate(p);return prepareBank(m,loader,name,bytes,nativeManager,id);};
 try{
  const ascii=load('ascii.anm',2);assert.equal(c.anm_manager_fallback(manager,2),1);m.u32(text+0x19248,ascii);m.u32(effects+12,load('effect.anm',8));
  for(let character=0;character<3;character++){
   const bank=load('pl0'+character+'.anm',9+character);
   for(let animal=0;animal<3;animal++){
    const name='pl0'+character+String.fromCharCode(97+animal)+'.sht',source=readFileSync(resolve(root,'reference/assets',name)),sht=c.sht_create(),p=c.allocate(source.length),nativeSht=m.allocate(source.length);
    memory(c,p,source.length).set(source);m.write(nativeSht,source);assert.equal(c.sht_open(sht,p,source.length),1,name);c.deallocate(p);m.view(owner,0x19100).fill(0);m.u32(owner+12,bank);m.u32(owner+0x1903c,nativeSht);
    const fixture=c.player_motion_create(manager,9+character,8),options=c.player_options_create(sht,fixture,9+character,character),header=c.allocate(0xe0);memory(c,header,0xe0).set(source.subarray(0,0xe0));c.player_motion_configure(fixture,header);c.deallocate(header);
    m.call(0x407230,{ecx:bank,args:[owner+16,0]});m.call(0x466de0,{ecx:owner+16});
    for(const [field,no,n]of fields)m.write(owner+no,Buffer.from(memory(c,c.player_motion_field(fixture,field),n)));
    m.i32(0x4b59f4,character);m.i32(0x4b59f8,animal);m.i32(0x4b5a38,100);m.i32(0x4b5a34,400);
    for(let frame=0;frame<160;frame++){
     const rate=[1,.5,1.25][animal],power=[0,100,200,300,400,400,300,200,100,0,400,0,200,400,300,100][Math.floor(frame/10)],input=((frame*19)&255)|(frame%19<9?8:0);
     m.f32(0x4b5918,rate);assert.equal(c.anm_manager_update(manager,1,rate),1);m.call(0x475940,{ecx:nativeManager});
     if(frame%5===0){m.i32(0x4b5a30,power);assert.equal(c.player_options_configure(options,power,100,400),1,string(c,c.player_options_error(options)));m.call(0x449630,{ecx:owner+0x610});assert.equal(new DataView(c.memory.buffer).getInt32(c.player_options_state(options),true),m.i32(owner+0x18ed8));configurations++;}
     new DataView(c.memory.buffer).setInt32(c.player_motion_field(fixture,9),frame,true);m.i32(owner+0x63c,frame);m.u32(0x4b3448,input);assert.equal(c.player_motion_step(fixture,input,1,rate),1,string(c,c.player_motion_error(fixture)));m.call(0x446ff0,{ecx:owner});
     const label=name+'/'+frame;
     for(const[field,no,n]of fields)assert.deepEqual(Buffer.from(memory(c,c.player_motion_field(fixture,field),n)),Buffer.from(m.bytes(owner+no,n)),label+' motion '+field);
     for(let i=0;i<12;i++){
      for(const[field,no,n]of optionFields)assert.deepEqual(Buffer.from(memory(c,c.player_motion_option(fixture,i,field),n)),Buffer.from(m.bytes(owner+no+i*0xe4,n)),label+' option '+i+' field '+field);
      for(let k=0;k<2;k++)assert.equal(new DataView(c.memory.buffer).getUint32(c.player_motion_handles(fixture,2)+(i*2+k)*4,true),m.u32(owner+0x710+i*0xe4+k*4),label+' handle '+i+'/'+k);
     }
     assert.equal(c.anm_manager_update(manager,0,rate),1,string(c,c.anm_manager_error(manager)));m.call(0x475830,{ecx:nativeManager});
     for(let alternate=0;alternate<2;alternate++){
      const list=nativeInstances(m,nativeManager,alternate);assert.equal(c.anm_manager_count(manager,alternate),list.length,label+' count');
      for(let i=0;i<list.length;i++){const h=m.u32(list[i]+0x538);assert.equal(c.anm_manager_order(manager,alternate,i),h,label+' order');const vm=c.anm_manager_find(manager,h);compareAnm(c,m,vm,list[i],label+' ANM '+h);assert.deepEqual(Buffer.from(memory(c,c.anm_vm_translation(vm),12)),Buffer.from(m.bytes(list[i]+0x5e4,12)),label+' translation '+h);checks++;}
     }frames++;checks+=139;
    }
    retireOriginalAnimations(c,m,manager,nativeManager);c.player_options_delete(options);c.player_motion_delete(fixture);c.sht_delete(sht);
   }
  }
  report('player-option-formations',{passed:true,characters:3,animals:3,frames,checks,configurations,originalFunctions:['0x449630','0x449b10','0x446ff0'],scope:'Nine actual SHT variants, power/focus formations, maximum-power glow replacement, original ANM lifecycle/order and Youmu history-based option callbacks. Roaring-only extra options and shooting are separate.'});
 }finally{c.anm_manager_delete(manager);m.close();}
});
