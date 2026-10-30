import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report,enableNativeMath,enableOriginalVectorMath} from './helpers.mjs';import {nativePool,resourceLoader,prepareBank,nativeInstances,compareAnm,retireOriginalAnimations} from './anm-oracle.mjs';
test('TH17 actual gathering-effect scripts preserve all particles, random streams and curve state',async()=>{
 const c=await core(),m=await oracle({memoryEnd:0x10000000,heapStart:0x08000000,heapLimit:0x0f000000}),a=c.anm_manager_create(),nm=nativePool(m),loader=resourceLoader(m);enableNativeMath(m);enableOriginalVectorMath(m);let effects=0,cases=0,frames=0,checks=0;
 try{
  const font=readFileSync(resolve(root,'reference/assets/ascii.anm')),fp=c.allocate(font.length);memory(c,fp,font.length).set(font);assert.equal(c.anm_manager_load(a,2,fp,font.length),1,string(c,c.anm_manager_error(a)));c.deallocate(fp);const nf=prepareBank(m,loader,'ascii.anm',font,nm,2),nt=m.allocate(0x19270);m.u32(0x4b7678,nt);m.u32(nt+0x19248,nf);assert.equal(c.anm_manager_fallback(a,2),1);
  const bytes=readFileSync(resolve(root,'reference/assets/effect.anm')),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.anm_manager_load(a,8,p,bytes.length),1,string(c,c.anm_manager_error(a)));c.deallocate(p);const bank=prepareBank(m,loader,'effect.anm',bytes,nm,8),owner=m.allocate(0x2020),out=m.allocate(4);m.u32(0x4b7698,owner);m.u32(owner+12,bank);m.u32(0x4a0430,0x4b5918);m.f32(0x4b5918,1);effects=c.anm_scene_effects_create(a,8);
  const all=JSON.parse(readFileSync(resolve(root,'artifacts/cpp/anm-effect-usage.json'),'utf8'))['1'];
  const selected=process.env.TH17_EFFECT_ONE?[Number(process.env.TH17_EFFECT_ONE)]:all.map(row=>row[1]);
  const same=(cp,np,n,label)=>{const got=Buffer.from(memory(c,cp,n)),want=Buffer.from(m.bytes(np,n));if(!got.equals(want)){let off=0;while(off<n&&got[off]===want[off])off++;assert.fail(label+' byte '+off.toString(16)+' got='+got.subarray(Math.max(0,off-4),off+12).toString('hex')+' original='+want.subarray(Math.max(0,off-4),off+12).toString('hex'));}checks++;};
  for(const script of selected){
   const rate=[1,.5,1.25][script%3];m.f32(0x4b5918,1);m.f32(0x524718,1);const h=c.anm_manager_spawn(a,8,script,-1,0);m.call(0x405a70,{ecx:bank,args:[out,script,0xffffffff,0]});assert.equal(h,m.u32(out));assert.ok(h,string(c,c.anm_manager_error(a)));m.f32(0x4b5918,rate);
   for(let frame=0;frame<200;frame++){
    const label=`script ${script} frame ${frame}`;
    same(c.anm_manager_random(a),0x4b7660,8,label+' visual RNG');same(c.anm_manager_environment(a),0x4b7668,8,label+' game RNG');
    for(let alt=0;alt<2;alt++){
     const list=nativeInstances(m,nm,alt);assert.equal(c.anm_manager_count(a,alt),list.length,label+' count');
     for(const np of list){const handle=m.u32(np+0x538),vm=c.anm_manager_find(a,handle);assert.ok(vm,label+' handle '+handle);compareAnm(c,m,vm,np,label+' ANM '+handle);const gather=c.anm_scene_effects_gather(vm);if(gather)same(gather,m.u32(np+0x5bc),0x193c,label+' gather');}
     assert.equal(c.anm_manager_update(a,alt,rate),1,label+' '+string(c,c.anm_manager_error(a)));m.call(alt?0x475940:0x475830,{ecx:nm,limit:30000000});
    }frames++;
   }
   retireOriginalAnimations(c,m,a,nm);cases++;
  }
  report('anm-gathering-effects',{passed:true,cases,frames,checks,originalFunctions:['0x41b810','0x404e50','0x404ec0','0x405790','0x475830','0x475940'],scope:'Actual TH17 ANM opcode-508 gathering scripts, complete particle curves, handles, native vector normalization, game/visual RNG streams, live animation order and lifetime. Other scene-effect descriptors and final GPU pixels require separate validation.'});
 }finally{if(effects)c.anm_scene_effects_delete(effects);c.anm_manager_delete(a);m.close();}
});
