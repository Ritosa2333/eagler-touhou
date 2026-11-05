// Test harness for the shipping BrowserRuntime. It exercises the real assets,
// fonts, input sampler and renderer with a deterministic platform clock.
export async function createProbe({sdl,replay,configuration}){
 const [{BrowserGraphics},{loadSDLModule,SDLRenderer}]=await Promise.all([import('/scripts/cpp/browser-graphics.mjs'),import('/scripts/cpp/sdl-module.mjs')]);
 let core,runtime,graphics,clock=100000,buttons=0;
 const bytes=(p,n)=>new Uint8Array(core.memory.buffer,p,n),encode=new TextEncoder(),decode=new TextDecoder(),cstr=p=>{const b=new Uint8Array(core.memory.buffer,p);return decode.decode(b.subarray(0,b.indexOf(0)));};
 const withBytes=(b,fn)=>{const p=core.allocate(b.length+1);bytes(p,b.length).set(b);try{return fn(p);}finally{core.deallocate(p);}},withName=(s,fn)=>withBytes(encode.encode(s),fn);
 const put=(name,b)=>withName(name,p=>withBytes(b,d=>core.asset(runtime,p,d,b.length))),read=name=>withName(name,p=>{const n=core.file_size(runtime,p),d=core.file(runtime,p);return bytes(d,n).slice();});
 const device=(op,a,b,c)=>{if(op<=12)return graphics.call(op,a,b,c);if(op===22)return clock>>>0;if(op===23)return buttons;if(op===21){bytes(a,6).set(encode.encode('09/13\0'));bytes(b,18).set(encode.encode('26/09/13 18:00:00\0'));}return 0;};
 const imports={th08_device:{call:device},wasi_snapshot_preview1:{clock_time_get(k,p,out){new DataView(core.memory.buffer).setBigUint64(out,1789315200000000000n,true);return 0;},fd_close(){return 8;},fd_seek(){return 8;},fd_write(fd,p,n,out){new DataView(core.memory.buffer).setUint32(out,0,true);return 0;}}};
 const wasm=await(await fetch('/probe-game.wasm')).arrayBuffer(),canvas=document.querySelector('canvas');
 if(sdl)core=(await loadSDLModule('/vendor/th08-sdl.mjs',wasm,canvas,imports)).exports;else{core=(await WebAssembly.instantiate(wasm,imports)).instance.exports;core._initialize();}
 runtime=core.create();graphics=sdl?{gpu:new SDLRenderer(core,canvas)}:new BrowserGraphics(canvas,core,runtime);
 const manifest=await(await fetch('/manifest.json')).json(),archive=new Uint8Array(await(await fetch('/data/th08.dat')).arrayBuffer());if(!withBytes(archive,p=>core.archive(runtime,p,archive.length)))throw Error('Archive');
 put('thbgm.dat',new Uint8Array(await(await fetch('/data/thbgm.dat',{headers:{Range:'bytes=0-15'}})).arrayBuffer()));
 for(const f of manifest.fonts){const r=await fetch('/fonts/'+f.file),b=new Uint8Array(await new Response(r.body.pipeThrough(new DecompressionStream('gzip'))).arrayBuffer());withBytes(b,p=>{if(!core.font(runtime,f.file.startsWith('cp932.')?0:f.file.startsWith('blend.')?1:2,p,b.length))throw Error('Font');});}
 for(const name of manifest.images){const bitmap=await createImageBitmap(new Blob([read(name)]),{premultiplyAlpha:'none',colorSpaceConversion:'none'}),surface=new OffscreenCanvas(bitmap.width,bitmap.height),ctx=surface.getContext('2d',{willReadFrequently:true});ctx.drawImage(bitmap,0,0);const data=ctx.getImageData(0,0,bitmap.width,bitmap.height).data;withName(name,p=>withBytes(data,b=>core.image(runtime,p,bitmap.width,bitmap.height,b,data.length)));bitmap.close();}
 for(const name of manifest.resources)read(name);
 if(replay)put('replay/th8_01.rpy',new Uint8Array(await(await fetch('/probe-replay.rpy')).arrayBuffer()));
 if(configuration)put('th08.cfg',new Uint8Array(configuration));
 if(!core.initialize(runtime))throw Error('Initialize');
 let ticks=0;const step=(input=0)=>{buttons=input;clock=100000+(++ticks)*1000/60;if(!core.step(runtime,true))throw Error('Game exited '+core.status(runtime,0)+'/'+core.status(runtime,4));};
 const wait=n=>{for(let i=0;i<n;i++)step();},key=k=>{for(let i=0;i<3;i++)step(k);wait(42);};
 wait(100);const title=()=>({scene:core.status(runtime,0),screen:core.status(runtime,8),cursor:core.status(runtime,9),diagnostics:core.diagnostics?Array.from(new Int32Array(core.memory.buffer,core.diagnostics(runtime),16)):null});
 const select=cursor=>{for(let i=0;i<10&&title().cursor!==cursor;i++)key(16);if(title().cursor!==cursor)throw Error('Menu cursor');key(1);};
 const trace=()=>{const p=core.trace(runtime),v=new Uint32Array(core.memory.buffer,p,68),f=new Float32Array(core.memory.buffer,p,68);return {frame:v[0],stage:v[1],gameFrames:v[2],controlFrames:v[3],flags:v[4],rng:v[5],rngCalls:v[6],player:[f[7],f[8]],playerState:v[9],keys:v[10],numbers:Array.from(bytes(p+44,228))};};
 function close(){core.destroy(runtime);}
 return {core,runtime,graphics,step,wait,key,title,select,trace,close,read,put};
}
