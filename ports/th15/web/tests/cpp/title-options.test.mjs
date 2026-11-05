import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';import {core,oracle,memory,string,root,report,enableNativeMath} from './helpers.mjs';import {resourceLoader,nativePool,prepareBank,nativeInstances,compareAnm} from './anm-oracle.mjs';
const load=(c,a,name,id)=>{const data=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(data.length);memory(c,p,data.length).set(data);assert.equal(c.anm_manager_load(a,id,p,data.length),1,string(c,c.anm_manager_error(a)));c.release(p);return data;};
test('TH15 options preserve original menu frames, volume digits, audio curve, reset and controller/title exits',async()=>{
 const c=await core(),m=await oracle();enableNativeMath(m);const byte=(p,v)=>v===undefined?m.view(p,1)[0]:(m.view(p,1)[0]=v);const loader=resourceLoader(m),base=m.heap;let frames=0,checks=0,sounds=[],volumes=0;
 m.replace(0x476360,'menu audio output',()=>{sounds.push(m.u32(m.reg('ESP')+4));return 0;},2);
 m.replace(0x476f10,'volume output boundary',()=>{assert.equal(m.u32(m.reg('ESP')+4),8);volumes++;return 0;},3);
 try{for(let scenario=0;scenario<15;scenario++){
  m.heap=base;m.view(m.heap,0x1caf980).fill(0);const a=c.anm_manager_create(),native=nativePool(m),owner=m.allocate(0x5bb0),text=m.allocate(0x19270),f=c.title_menu_create(a),options=c.title_options_create(f);
  try{
   m.u32(0x4e9a40,0);m.u32(0x4e9a44,0);m.view(owner,0x5bb0).fill(0);m.u32(0x4e9a58,text);m.u32(0x4ca620,0x4e73e8);m.f32(0x4e73e8,1);m.i32(owner+0x18,3);m.i32(owner+0x1c,1);m.i32(owner+0xf4,1);m.i32(owner+0x24,6);m.call(0x403b10,{ecx:owner+0x24});c.title_menu_configure(f,6,1,0,0);c.title_menu_push(f);
   // The menu is entered with the main-menu selection on its history stack.
   const cp=c.title_options_settings(options),age=c.title_menu_field(f,2);const v=[0,5,10,45,80,95,100];const bgm=v[scenario%7],se=v[(scenario*3)%7];byte(0x4e79c6,bgm);byte(0x4e79c7,se);byte(0x4e79c8,3);c.title_options_configure(options,bgm,se);new DataView(c.memory.buffer).setUint8(cp+8,3);m.write(owner+0x2b0,Buffer.from(memory(c,age,20)));
   const ascii=load(c,a,'ascii.anm',5),ab=prepareBank(m,loader,'ascii.anm',ascii,native,5);m.u32(text+0x19258,ab);assert.equal(c.anm_manager_fallback(a,5),1);const data=load(c,a,'title.anm',16),bank=prepareBank(m,loader,'title.anm',data,native,16);m.u32(owner+0x10,bank);
   const compareQueue=label=>{for(let alt=0;alt<2;alt++){const list=nativeInstances(m,native,alt);assert.equal(c.anm_manager_count(a,alt),list.length,label+' count');for(let i=0;i<list.length;i++){const h=m.u32(list[i]+0x544);assert.equal(c.anm_manager_order(a,alt,i),h,label+' queue');compareAnm(c,m,c.anm_manager_find(a,h),list[i],label+' script '+(m.u32(list[i]+0x4a8)&65535));checks++;}}};
   const step=(pressed=0,repeated=0)=>{
    const label=scenario+'/'+frames;m.u32(0x4e6d1c,pressed);m.u32(0x4e6d18,repeated);sounds=[];volumes=0;
    assert.equal(m.call(0x462010,{ecx:owner}),1);assert.equal(c.title_options_update(options,pressed,repeated),1,label+': '+string(c,c.title_options_error(options)));
    assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.title_menu_sounds(f),c.title_menu_sound_count(f))),sounds,label+' sounds');assert.equal(c.title_options_volume_calls(options),volumes,label+' volume calls');
    const st=Array.from(new Uint32Array(c.memory.buffer,c.title_menu_state(f),254));for(const[i,addr]of[[0,owner+0x18],[1,owner+0x1c],[2,owner+0x20],[3,owner+0xf4]])assert.equal(st[i],m.u32(addr),label+' state '+i);
    assert.deepEqual(Buffer.from(memory(c,c.title_menu_cursor(f),0xd8)),Buffer.from(m.bytes(owner+0x24,0xd8)),label+' menu');for(let i=0;i<248;i++)assert.equal(st[6+i],m.u32(owner+0x2c4+i*4),label+' handle '+i);
    const dv=new DataView(c.memory.buffer);assert.equal(dv.getInt32(cp,true),byte(0x4e79c6),label+' BGM');assert.equal(dv.getInt32(cp+4,true),byte(0x4e79c7),label+' SE');assert.equal(dv.getUint8(cp+8),byte(0x4e79c8),label+' controller');
    if(volumes)assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.title_options_volume(options),3)),[m.i32(0x521350),m.i32(0x521354),m.i32(0x521358)],label+' volume curve');
    assert.deepEqual(Buffer.from(memory(c,age,20)),Buffer.from(m.bytes(owner+0x2b0,20)),label+' timer');compareQueue(label+' pre-update');m.i32(owner+0x2b0,m.i32(owner+0x2b4));m.i32(owner+0x2b4,m.i32(owner+0x2b4)+1);m.f32(owner+0x2b8,Math.fround(m.f32(owner+0x2b8)+1));c.title_menu_tick(f,1);
    for(let alt=0;alt<2;alt++){assert.equal(c.anm_manager_update(a,alt,1),1,string(c,c.anm_manager_error(a)));m.call(alt?0x487890:0x487740,{ecx:native});}compareQueue(label+' post-update');frames++;return st[0];
   };
   for(let i=0;i<10;i++)step();
   for(let i=0;i<24;i++)step(128);for(let i=0;i<24;i++)step(0,64);step(32);
   for(let i=0;i<125;i++)step(i<24?128:0,i>=90?64:0);
   step(32);step(32);step(0x80001);step(16);step(32);step(32);step(16);
   if(scenario%3===0){step(16);step(0x80001);for(let i=0;i<14;i++){if(step()!==3)break;}}
   else{step(0x102);step(scenario%2?0x80001:0x102);for(let i=0;i<14;i++){if(step()!==3)break;}}
  }finally{c.title_options_delete(options);c.title_menu_delete(f);c.anm_manager_delete(a);}
 }
 report('title-options',{passed:true,scenarios:15,frames,animationChecks:checks,originalFunctions:['0x462010','0x462340','0x463600'],scope:'Actual title animation tree, sprite/visibility digits, menu input/repeat, timing/history and original audio request curve. Hardware audio output is an isolated boundary.'});
 }finally{m.close();}
});
