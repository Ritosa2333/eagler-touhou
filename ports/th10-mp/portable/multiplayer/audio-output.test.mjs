// Build first: node portable/build.mjs --multiplayer --multiplayer-fixtures
// Exercises the real AudioHost through diagnostic-only forwarding exports.
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createServer} from 'node:http';
import {resolve} from 'node:path';
import {createRequire} from 'node:module';
const root=resolve(import.meta.dirname,'../..');
const launcher=process.env.EAGLER_LAUNCHER_ROOT||resolve(root,'../../eagler-touhou');
const {chromium}=createRequire(resolve(launcher,'package.json'))(process.env.EAGLER_PLAYWRIGHT_MODULE||'playwright');
const build=resolve(root,'th10_web/artifacts/multiplayer-fixtures');
const server=createServer((req,res)=>{
 const path=new URL(req.url,'http://localhost').pathname;
 if(path==='/'){res.setHeader('Content-Type','text/html');res.end('<canvas id="canvas"></canvas>');return;}
 if(!['/th10-sdl.mjs','/th10-sdl.wasm'].includes(path)){res.writeHead(404).end();return;}
 res.setHeader('Content-Type',path.endsWith('.wasm')?'application/wasm':'text/javascript');
 res.end(readFileSync(resolve(build,path.slice(1))));
});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
let browser;
try{
 browser=await chromium.launch({headless:true,executablePath:process.env.EAGLER_TEST_CHROMIUM||undefined,args:['--autoplay-policy=no-user-gesture-required']});
 const page=await browser.newPage();const errors=[];page.on('pageerror',e=>errors.push(String(e)));
 await page.goto('http://127.0.0.1:'+server.address().port);
 const result=await page.evaluate(async()=>{
  let raw;const {default:create}=await import('/th10-sdl.mjs');
  const c=await create({canvas:document.querySelector('canvas'),instantiateWasm(imports,done){WebAssembly.instantiateStreaming(fetch('/th10-sdl.wasm'),imports).then(({instance,module})=>{raw=instance.exports;done(instance,module);});return {};}});
  const device=raw.sdl_audio_device();if(!device)throw Error('Audio device unavailable');
  const p=(raw.allocate||raw.graphics_allocate)(128),u32=new Uint32Array(raw.memory.buffer),u16=new Uint16Array(raw.memory.buffer);
  // PCM16 stereo, one-second tone. No game assets or network session required.
  u16[(p+64)/2]=1;u16[(p+66)/2]=2;u32[(p+68)/4]=44100;u32[(p+72)/4]=176400;u16[(p+76)/2]=4;u16[(p+78)/2]=16;
  u32[p/4]=20;u32[p/4+1]=0;u32[p/4+2]=176400;u32[p/4+4]=p+64;
  const call=(id,op,args=[])=>{new Uint32Array(raw.memory.buffer,p+32,8).fill(0);new Uint32Array(raw.memory.buffer,p+32,args.length).set(args);return raw.mp_fixture_audio_call(id,op,p+32);};
  call(device,2,[p,p+96]);const handle=new Uint32Array(raw.memory.buffer)[(p+96)/4];
  call(handle,14,[0,176400,p+100,p+104,0,0,2]);
  const pcm=new Int16Array(raw.memory.buffer,new Uint32Array(raw.memory.buffer)[(p+100)/4],88200);pcm.fill(4000);
  call(handle,15);call(handle,10,[0,0,1]);
  const pool=c.eaglerSfxOutput,voice=[...pool.voices.values()][0];if(!voice?.node)throw Error('Independent SFX not active');
  raw.sdl_audio_pause(1);raw.mp_fixture_audio_seek(1);
  const retired=!voice.playing&&!voice.node;
  call(handle,10,[0,0,1]);const suppressed=!voice.playing&&!voice.node;
  raw.mp_fixture_audio_seek(0);
  const pausePreserved=c.eaglerSfxPaused===true;
  raw.sdl_audio_pause(0);const noStaleReplay=!voice.node&&!voice.playing;
  call(handle,10,[0,0,1]);const freshPlayback=!!voice.node;
  raw.sdl_audio_pause(1);const cursor=pool.cursor(voice);
  await new Promise(r=>setTimeout(r,80));const frozen=pool.cursor(voice)===cursor;
  raw.sdl_audio_pause(0);const resumed=!!voice.node;
  call(handle,1);const released=pool.voices.size===0;
  raw.sdl_audio_shutdown();return {retired,suppressed,pausePreserved,noStaleReplay,freshPlayback,frozen,resumed,released};
 });
 assert.deepEqual(errors,[]);for(const [name,passed] of Object.entries(result))assert.equal(passed,true,name);
 console.log(JSON.stringify(result));
}finally{if(browser)await browser.close();await new Promise(r=>server.close(r));}
