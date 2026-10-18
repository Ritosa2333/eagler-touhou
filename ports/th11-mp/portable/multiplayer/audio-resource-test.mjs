import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createServer} from 'node:http';
import {createRequire} from 'node:module';
// Uses the real C++ diagnostic build and resource-write bridge, never a JS mixer.
// Build first: node portable/multiplayer/native-ui-build.mjs --mp-only --phase audio-late
const root=resolve(import.meta.dirname,'../..'),workspace=resolve(root,'../..');
const argument=(name,fallback)=>{const i=process.argv.indexOf(name);return i<0?fallback:resolve(process.argv[i+1]);};
const launcher=resolve(process.env.EAGLER_LAUNCHER_ROOT||resolve(workspace,'eagler-touhou'));
const {chromium}=createRequire(resolve(launcher,'package.json'))('playwright');
const diag=argument('--diagnostic',resolve(root,'artifacts/multiplayer-ui/20261009/audio-late'));
const archive=argument('--archive',process.env.TH11_TEST_ARCHIVE||resolve(workspace,'th11-eagler/[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
const music=argument('--music',resolve(workspace,'games/web-content/th11/music'));
const output=argument('--output',resolve(diag,'native-audio-late.json'));
const files=new Map([['/multiplayer.mjs',resolve(diag,'multiplayer.mjs')],['/multiplayer.wasm',resolve(diag,'multiplayer.wasm')],['/host.mjs',resolve(root,'th11_web/sdl-runtime/eagler-host.mjs')],['/th11.dat',archive]]);
for(const n of ['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'])files.set('/fonts/'+n,resolve(root,'build-eagler-multiplayer/fonts',n));
for(const n of ['th11_00.ogg','th11_15.ogg'])files.set('/music/'+n,resolve(music,n));
const server=createServer((req,res)=>{const p=new URL(req.url,'http://localhost').pathname;if(p==='/'){res.setHeader('Content-Type','text/html');res.end('<canvas id="canvas" width="640" height="480"></canvas>');return;}if(!files.has(p)){res.writeHead(404).end();return;}res.setHeader('Content-Type',p.endsWith('.mjs')?'text/javascript':p.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(readFileSync(files.get(p)));});
await new Promise(r=>server.listen(0,'127.0.0.1',r));const browser=await chromium.launch({channel:'msedge',headless:true,args:['--autoplay-policy=no-user-gesture-required']});const page=await browser.newPage();let report;
try{await page.goto('http://127.0.0.1:'+server.address().port);report=await page.evaluate(async()=>{
 const {default:create}=await import('/multiplayer.mjs');let raw;const wasm=await(await fetch('/multiplayer.wasm')).arrayBuffer();const c=await create({canvas:document.querySelector('canvas'),instantiateWasm(imports,done){WebAssembly.instantiate(wasm,imports).then(({instance})=>{raw=instance.exports;done(instance);});return {};}});
 const put=async p=>c.FS.writeFile(p,new Uint8Array(await(await fetch(p)).arrayBuffer()));await put('/th11.dat');c.FS.mkdir('/fonts');for(const n of ['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'])await put('/fonts/'+n);c.FS.mkdir('/music');await put('/music/th11_00.ogg');if(!c._th11_initialize())throw Error('native initialize failed');
 const {observeMusicWrites}=await import('/host.mjs');observeMusicWrites(c,raw,'th11');
 const probe=(n,v=0)=>{if(!c._th11_audio_probe(n,v,0))throw Error('audio probe failed '+n);};const stats=()=>Array.from(new Uint32Array(c.HEAPU8.buffer,c._th11_audio_statistics(),9));
 const energy=()=>{let sum=0;for(let j=0;j<48;j++){const p=c._th11_audio_samples();if(!p)throw Error('mix failed');for(const v of new Float32Array(c.HEAPU8.buffer,p,2048)){if(!Number.isFinite(v))throw Error('invalid PCM');sum+=v*v;}}return sum;};
 probe(2);probe(0,13);const waiting=stats();const silent=energy();if(waiting[5]!==13||silent>1e-8)throw Error('missing Extra track must be silent and nonfatal');
 await put('/music/th11_15.ogg');probe(4);const restored=stats(),played=energy();if(restored[7]===0||played===0)throw Error('written Extra track did not restore native decoder');
 probe(2);c.FS.unlink('/music/th11_15.ogg');probe(0,13);c._mp_fixture_audio_pause(1);await put('/music/th11_15.ogg');probe(4);const paused=energy();if(paused>1e-8||stats()[8]!==1)throw Error('late music bypassed pause');c._mp_fixture_audio_pause(0);const resumed=energy();if(resumed===0)throw Error('native resume failed');
 probe(3,120);for(let j=0;j<120;j++)probe(4);const faded=energy();if(faded>1e-8)throw Error('native fade failed');
 probe(2);c.FS.unlink('/music/th11_15.ogg');probe(0,13);probe(2);await put('/music/th11_15.ogg');probe(4);const cancelled=energy();if(cancelled>1e-8)throw Error('cancelled pending music restarted');
 c._th11_music_enabled(0);probe(0,13);probe(1,10);const sfx=energy();if(sfx===0)throw Error('music none disabled SFX');
 return {passed:true,diagnostic:true,originalExecutable:false,waiting,restored,silent,played,paused,resumed,faded,cancelled,sfx};
 });console.log(JSON.stringify(report));}catch(e){report={passed:false,error:e.stack};console.log(JSON.stringify(report));process.exitCode=1;}finally{writeFileSync(output,JSON.stringify(report,null,2));await browser.close();await new Promise(r=>server.close(r));}
