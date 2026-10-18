import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../',import.meta.url)),game=process.argv.includes('--th08')?'th08':'th10',version=game==='th08'?8:10,synthetic=process.argv.includes('--synthetic');
const files=game==='th10'?['title','menu','stage1','bomb','pause','resume'].map(n=>`th10_web/artifacts/browser-quality-chs/pixels/${n}.json`):synthetic?['th08_web/artifacts/native/d3d-pixels/cases.json']:['th08_web/artifacts/native/game-pixels/cases.json'];
const fixtures=files.flatMap(n=>JSON.parse(readFileSync(resolve(root,n)))),browser=await launchBrowser(),results={};
try{for(const backend of ['baseline','sdl']){
 const page=await browser.newPage();await page.route('**/runtime/**',r=>{const name=new URL(r.request().url()).pathname.slice('/runtime/'.length);if(name.includes('..'))throw Error('Invalid fixture module');return r.fulfill({contentType:'text/javascript',body:readFileSync(resolve(root,'th10_web/runtime',name))});});await page.route('**/test.html',r=>r.fulfill({contentType:'text/html',body:'<!doctype html><canvas id="screen" width="640" height="480"></canvas>'}));
 await page.route('**/fixture.mjs',r=>r.fulfill({contentType:'text/javascript',body:readFileSync(resolve(root,'th10_web/artifacts/sdl3/fixture.mjs'))}));await page.route('**/fixture.wasm',r=>r.fulfill({contentType:'application/wasm',body:readFileSync(resolve(root,'th10_web/artifacts/sdl3/fixture.wasm'))}));
 await page.route('**/baseline.mjs',r=>r.fulfill({contentType:'text/javascript',body:readFileSync(resolve(root,game==='th08'?'th08_web/artifacts/sdl3/baseline/webgl.mjs':'th10_web/artifacts/sdl3/baseline/runtime/webgl.mjs'))}));
 await page.goto('http://127.0.0.1:8093/test.html');results[backend]=await page.evaluate(async({fixtures,version,backend})=>{
  const decode=b=>Uint8Array.from(atob(b),c=>c.charCodeAt(0)),rows=[];
  let c,canvas=document.querySelector('canvas');if(backend==='sdl'){const{loadSDLModule}=await import('/runtime/sdl-module.mjs');c=(await loadSDLModule('/fixture.mjs',await(await fetch('/fixture.wasm')).arrayBuffer(),canvas)).exports;}
  for(const f of fixtures){
   let render,host,device,m,textures,known;const viewport=new Uint8Array(new Uint32Array([0,0,f.width,f.height,0,0x3f800000]).buffer);
   if(backend==='baseline'){
    const {NativeGraphics}=await import('/runtime/native-graphics.mjs'),classes=await import('/baseline.mjs');let cursor=4096;const memory=new WebAssembly.Memory({initial:1024});host=new NativeGraphics().bind({memory,graphics_allocate:n=>{let p=cursor;cursor=(cursor+n+15)&~15;return p;},graphics_free(){},graphics_configure_arithmetic(){}});m=host.m;const params=host.alloc(56);m.write(params,new Uint8Array(new Uint32Array([f.width,f.height,f.format,1,0,0,1,0,1,1,80,0,0,0]).buffer));device=host.d3d.createDevice(params);canvas=document.createElement('canvas');canvas.width=f.width;canvas.height=f.height;document.body.append(canvas);render=new classes[version===8?'WebGLD3D8':'WebGLD3D9'](canvas,host.d3d);render.warmPrograms();textures=f.textures.map(t=>{const o=host.d3d.texture(t.width,t.height,1,0,t.format,1);m.write(o.surface.data,decode(t.pixels));return o.address;});
   }else{if(!c.fixture_create(version,f.width,f.height,f.format))throw Error('SDL fixture initialization');textures=f.textures.map(t=>{const id=c.fixture_texture(t.width,t.height,t.format),b=decode(t.pixels);new Uint8Array(c.memory.buffer,c.fixture_pixels(id),b.length).set(b);c.fixture_changed(id);return id;});}
   const withBytes=(bytes,fn)=>{const p=c.allocate(bytes.length);new Uint8Array(c.memory.buffer,p,bytes.length).set(bytes);try{return fn(p);}finally{c.deallocate(p);}},vp=b=>withBytes(b,p=>c.fixture_viewport(p)),hashes=[];
   const clear=op=>{if(backend==='baseline')render.clear({...op,target:device.back.address,depthTarget:device.depth.address,viewport:op.viewport?decode(op.viewport):viewport});else{vp(op.viewport?decode(op.viewport):viewport);if(op.rects)withBytes(new Uint8Array(new Int32Array(op.rects).buffer),p=>c.fixture_clear(op.flags,op.color,op.depth,op.stencil??0,p,op.rects.length/4));else c.fixture_clear(op.flags,op.color,op.depth,op.stencil??0,0,0);}};
   for(let repeat=0;repeat<3;repeat++){
    if(f.initialPixels){const b=decode(f.initialPixels);if(backend==='baseline'){m.write(device.back.data,b);device.back.version++;}else{new Uint8Array(c.memory.buffer,c.fixture_pixels(1),b.length).set(b);c.fixture_changed(1);}}
    if(!f.ops?.length)clear({flags:3,color:f.clear,depth:1});
    for(let i=0;i<=f.draws.length;i++){
     for(const op of f.ops??[])if(op.before===i){if(op.type==='clear')clear(op);else if(op.type==='copy'){if(backend==='baseline'){render.flush();const s=host.d3d.surface(op.width,op.height,op.format,2);m.write(s.data,decode(op.pixels));render.copy(s,op.rect,device.back,op.point);device.back.version++;}else{const id=c.fixture_texture(op.width,op.height,op.format),b=decode(op.pixels);new Uint8Array(c.memory.buffer,c.fixture_pixels(id),b.length).set(b);c.fixture_changed(id);withBytes(new Uint8Array(new Int32Array([...op.rect,...op.point]).buffer),p=>c.fixture_copy(id,p,p+16));}}else throw Error('Unknown recorded operation');}
     if(i===f.draws.length)break;const d=f.draws[i];if(backend==='baseline')render.queue({...d,texture:textures[d.texture]??0,target:device.back.address,depthTarget:device.depth.address,vertices:decode(d.vertices),indices:d.indices?decode(d.indices):null,viewport:d.viewport?decode(d.viewport):viewport,states:new Map(d.states),stages:new Map(d.stages),transforms:new Map(d.transforms.map(([k,v])=>[k,decode(v)]))});
     else {c.fixture_state(d.fvf,textures[d.texture]??0);for(const [k,v]of d.states)c.fixture_render(k,v);for(const[k,v]of d.stages)c.fixture_stage(k,v);for(const[k,v]of d.transforms)withBytes(decode(v),p=>c.fixture_transform(k,p));vp(d.viewport?decode(d.viewport):viewport);withBytes(decode(d.vertices),p=>{if(d.indices)withBytes(decode(d.indices),ix=>c.fixture_draw(d.primitive,d.count,p,d.stride,ix,d.indexFormat));else c.fixture_draw(d.primitive,d.count,p,d.stride,0,0);});}
    }
    let pixels;if(backend==='baseline'){render.flush();render.readSurface(device.back);pixels=m.bytes(device.back.data,device.back.size);if(render.gl.getError())throw Error('Baseline GL error');}else pixels=new Uint8Array(c.memory.buffer,c.fixture_read(),f.width*f.height*(f.format>=23&&f.format<=26?2:4)).slice();
    hashes.push(Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',pixels)),b=>b.toString(16).padStart(2,'0')).join(''));
   }
   rows.push({name:f.name,hashes});if(backend==='baseline'){render.gl.getExtension('WEBGL_lose_context')?.loseContext();canvas.remove();}else c.fixture_destroy();
  }return rows;
 },{fixtures,version,backend});await page.close();}
 const passed=JSON.stringify(results.baseline)===JSON.stringify(results.sdl);writeFileSync(resolve(root,game+'_web/artifacts/sdl3/render'+(synthetic?'-synthetic':'')+'.json'),JSON.stringify({passed,frames:fixtures.length*3,...results},null,2));assert(passed,'SDL pixels differ; inspect report');console.log('PASS '+game+' '+fixtures.length+' scenes / '+fixtures.length*3+' frames');
}finally{await browser.close();}
