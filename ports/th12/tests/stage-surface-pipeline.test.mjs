import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {Renderer} from '../src/renderer.mjs';
const original=JSON.parse(fs.readFileSync(new URL('fixtures/stage-surface-pipeline-native.json',import.meta.url)));

test('original surface bootstrap aliases text entries2/3 and draws background Main81 before all sixteen STD strips',()=>{
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const aliases=original.aliases,records=original.records;
 assert.deepEqual(aliases.mainScripts,[81,82,81]);assert.deepEqual(aliases.mainSprites,[75,76,75]);
 assert.deepEqual(records.filter(r=>r.kind==='get-surface').map(r=>[r.textureEntry,r.destination]),[[2,'0x4cea94'],[3,'0x4cea98']]);
 assert.deepEqual(records.filter(r=>r.kind==='set-target').map(r=>[r.phase,r.surface]),
  [['draw-priority-0',aliases.surface2],['draw-priority-12',aliases.surface3],
   ['draw-priority-37',aliases.surface2],['draw-priority-67',aliases.backbuffer]]);
 const draws=records.filter(r=>r.phase==='draw-priority-14'&&r.kind==='draw-vm');
 assert.equal(draws.length,17);assert.equal(draws[0].id,0);
 assert.deepEqual(draws.slice(1).map(r=>r.id),Array.from({length:16},(_,i)=>i+1));
 assert.deepEqual([draws[0].position,draws[0].engine,draws[0].scale,draws[0].dimensions,
  (draws[0].flags>>>5)&7,draws[0].primary],[[32,16,0],[0,0,0],[1,1],[384,448],3,0xffffffff]);
 assert.deepEqual(original.rngBeforeDraw,original.rngAfterDraw,'draw selection consumes no game RNG');
});
test('renderer snapshots only the current background pass, copies Main81 once and reuses its texture for every strip',()=>{
 const events=[],renderer=Object.create(Renderer.prototype);
 renderer.textures=new Map();renderer.current={};renderer.point=true;
 renderer.stageSurfaceCopied=renderer.stageSurfaceComposited=false;
 renderer.gl={TEXTURE_2D:1,RGB:2,RGBA:9,TEXTURE_MIN_FILTER:3,TEXTURE_MAG_FILTER:4,
  TEXTURE_WRAP_S:5,TEXTURE_WRAP_T:6,REPEAT:7,LINEAR:8,
  createTexture:()=>{events.push(['allocate']);return 'native-background';},
  bindTexture:(...args)=>events.push(['bind',...args]),copyTexImage2D:(...args)=>events.push(['copy',...args]),
  enable:()=>{},disable:()=>{},scissor:(...args)=>events.push(['scissor',...args]),
  clearColor:(...args)=>events.push(['clear-color',...args]),clear:()=>events.push(['clear-destination']),
  texParameteri:()=>{}};
 renderer.flush=()=>events.push(['flush']);renderer.region=(...args)=>events.push(['main81',...args]);
 for(const priority of [4,6,8,9,11]){renderer.stageSurface(priority);events.push(['background-layer',priority]);}
 assert.ok(!events.some(e=>e[0]==='copy'));
 renderer.stageSurface(13);events.push(['next-target-layer',13]);renderer.stageSurface(14);
 for(let i=0;i<16;i++){renderer.stageSurface(14);events.push(['strip',i]);}
 for(const priority of [16,18,24,31,44]){renderer.stageSurface(priority);events.push(['foreground',priority]);}
 assert.equal(events.filter(e=>e[0]==='copy').length,1);assert.equal(events.filter(e=>e[0]==='main81').length,1);
 const copyAt=events.findIndex(e=>e[0]==='copy'),mainAt=events.findIndex(e=>e[0]==='main81');
 assert.ok(copyAt>events.findIndex(e=>e[0]==='background-layer'&&e[1]===11));
 assert.ok(copyAt<events.findIndex(e=>e[0]==='next-target-layer'));
 assert.ok(copyAt<events.findIndex(e=>e[0]==='clear-destination'));
 assert.ok(events.findIndex(e=>e[0]==='clear-destination')<events.findIndex(e=>e[0]==='next-target-layer'));
 assert.ok(mainAt<events.findIndex(e=>e[0]==='strip'));
 assert.deepEqual(events[copyAt].slice(1),[1,0,2,0,0,640,480,0]);
 assert.deepEqual(events[mainAt].slice(1,14),['text',2,32,16,384,448,32,16,384,448,0,1,[1,1,1]]);
 assert.equal(events[mainAt][14],3,'native main81 uses ONE/ZERO copy blend');
 assert.deepEqual(events.filter(e=>e[0]==='scissor').map(e=>e.slice(1)),[[13,0,422,480],[32,16,384,448]],
  'original destination viewport clears before the saved draw clip is restored');
 assert.deepEqual(renderer.textures.get('text:2'),{texture:'native-background',w:640,h:480,defaultWrap:7,flipY:true,available:true,surfaceInitialized:true});
});
test('copied background strips invert framebuffer V once while ordinary image atlases preserve their V',()=>{
 const renderer=Object.create(Renderer.prototype),capture={texture:{},w:640,h:480,flipY:true};
 renderer.textures=new Map([['text:2',capture],['bullet:0',{texture:{},w:640,h:480}]]);
 renderer.gl={TRIANGLES:4,LINE_STRIP:3};renderer.material=renderer.texture=renderer.primitive=()=>{};renderer.batch=[];
 const vertices=[{x:32,y:16,z:0,rhw:1,color:0xc0ffffff,u:.05,v:16/480},
  {x:56,y:16,z:0,rhw:1,color:0xc0ffffff,u:.0875,v:16/480},
  {x:32,y:37.333335876464844,z:0,rhw:1,color:0xc0ffffff,u:.05,v:37.333335876464844/480}];
 renderer.mesh({a:{name:'text',entry:2,blend:0},primitive:5,vertices});
 assert.equal(renderer.batch[3],1-vertices[0].v);
 renderer.batch=[];renderer.mesh({a:{name:'bullet',entry:0,blend:0},primitive:5,vertices});
 assert.equal(renderer.batch[3],vertices[0].v);
});
test('native Main81 normal copy keeps real mode0 camera rounding while its strips retain raw coordinates',()=>{
 const main=JSON.parse(fs.readFileSync(new URL('fixtures/background-main-quad-native.json',import.meta.url)));
 assert.equal(main.originalExeSha256,original.originalExeSha256);
 const normal=main.records.filter(r=>r.interrupt===0);assert.equal(normal.length,3);
 for(const record of normal){
  const n=record.samples[0],renderer=Object.create(Renderer.prototype);renderer.gl={TRIANGLES:4};renderer.batch=[];
  renderer.stageSurfaceCopied=true;renderer.stageSurfaceComposited=false;renderer.stageSurfaceAvailable=true;
  renderer.textures=new Map([['text:2',{texture:{},w:640,h:480,flipY:true}]]);
  renderer.material=renderer.texture=renderer.primitive=renderer.flush=()=>{};
  renderer.stageSurface(14,record.offset);
  const native=[0,1,5,2].map(i=>n.vertices[i].slice(0,2).map(v=>v+.5));
  const actual=[0,1,2,5].map(i=>renderer.batch.slice(i*10,i*10+2));
  assert.deepEqual(actual,native,'real script81 source copy uses native mode0 corners');
  assert.equal(Math.fround(renderer.batch[3]),Math.fround(1-Math.fround(16/480)),
   'Float32 WebGL upload preserves the original normalized source coordinate');
 }
});
test('opaque RGB drawing buffer rejects RGBA capture while failed copies never clear or overwrite the original background',()=>{
 for(const forceFailure of [false,true]){
  const events=[],renderer=Object.create(Renderer.prototype);renderer.textures=new Map();
  renderer.stageSurfaceCopied=renderer.stageSurfaceComposited=renderer.stageSurfaceAvailable=false;
  let error=0;
  renderer.gl={TEXTURE_2D:1,RGB:2,RGBA:3,NO_ERROR:0,INVALID_OPERATION:1282,
   TEXTURE_MIN_FILTER:4,TEXTURE_MAG_FILTER:5,TEXTURE_WRAP_S:6,TEXTURE_WRAP_T:7,REPEAT:8,LINEAR:9,
   createTexture:()=>({}),bindTexture:()=>{},texParameteri:()=>{},enable:()=>{},disable:()=>{},scissor:()=>{},
   clearColor:()=>{},clear:()=>events.push('clear'),getError:()=>error,
   copyTexImage2D:(_target,_level,format)=>{events.push('copy');error=forceFailure||format!==2?1282:0;}};
  renderer.flush=()=>{};renderer.region=()=>events.push('main');renderer.stageSurface(14);
  if(forceFailure){
   assert.deepEqual(events,['copy'],'copy failure preserves the visible background and skips the opaque Main copy');
   assert.equal(renderer.surfaceCopyError,1282);assert.equal(renderer.stageSurfaceAvailable,false);
   renderer.stageSurface(14);assert.deepEqual(events,['copy'],'one failed attempt per frame');
   renderer.material=()=>assert.fail('failed texture must not be sampled');
   renderer.mesh({a:{name:'text',entry:2},vertices:[],primitive:5});
  }else{
   assert.deepEqual(events,['copy','clear','main']);assert.equal(renderer.surfaceCopyError,0);
   assert.equal(renderer.stageSurfaceAvailable,true,'native STD17 texture remains available after a compatible RGB copy');
  }
 }
});
test('subsequent background frames reuse verified RGB storage without synchronous error queries',()=>{
 const renderer=Object.create(Renderer.prototype),calls=[];renderer.textures=new Map();
 renderer.gl={TEXTURE_2D:1,RGB:2,NO_ERROR:0,createTexture:()=>({}),bindTexture:()=>{},texParameteri:()=>{},enable:()=>{},disable:()=>{},scissor:()=>{},clearColor:()=>{},clear:()=>{},
  getError:()=>{calls.push('error');return 0;},copyTexImage2D:()=>calls.push('allocate'),copyTexSubImage2D:(...args)=>calls.push(args)};
 renderer.flush=()=>{};renderer.region=()=>{};
 for(let i=0;i<2;i++){renderer.stageSurfaceCopied=renderer.stageSurfaceComposited=renderer.stageSurfaceAvailable=false;renderer.stageSurface(14);}
 assert.deepEqual(calls,['allocate','error',[1,0,0,0,0,0,640,480]]);
 assert.equal(renderer.stageSurfaceAvailable,true);
});
