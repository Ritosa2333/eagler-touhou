import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';

const root=fileURLToPath(new URL('../../',import.meta.url)),out=resolve(root,'artifacts/sdl3/browser/audio');mkdirSync(out,{recursive:true});
const wasmSha256=createHash('sha256').update(readFileSync(resolve(root,'artifacts/sdl3/th11-harness.wasm'))).digest('hex');
const tracks=JSON.parse(readFileSync(resolve(root,'assets/sdl-native/music-verification.json'),'utf8'));
const files=new Map([['/th11.mjs',resolve(root,'artifacts/sdl3/th11-harness.mjs')],['/th11-harness.wasm',resolve(root,'artifacts/sdl3/th11-harness.wasm')],['/th11.dat',resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')]]);
for(const t of tracks)files.set('/music/'+t.file,resolve(root,'assets/sdl-native/music',t.file));
for(const name of ['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'])files.set('/fonts/'+name,resolve(root,'assets/sdl-native/fonts',name));
const server=createServer((req,res)=>{const name=new URL(req.url,'http://localhost').pathname;res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');if(name==='/'){res.setHeader('Content-Type','text/html');res.end('<!doctype html><title>TH11 audio test</title><canvas id="canvas" width="640" height="480"></canvas>');return;}if(!files.has(name)){res.writeHead(404).end();return;}res.setHeader('Content-Type',name.endsWith('.mjs')?'text/javascript':name.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(readFileSync(files.get(name)));});
await new Promise(r=>server.listen(0,'127.0.0.1',r));const url='http://127.0.0.1:'+server.address().port;
let browser;const errors=[];
try{
 browser=await launchBrowser({args:[...(process.env.NATIVE_GPU==='1'?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-unsafe-swiftshader']),'--autoplay-policy=no-user-gesture-required']});const page=await browser.newPage({viewport:{width:680,height:520}});
 page.on('pageerror',e=>errors.push(e.stack));await page.goto(url);
 await page.evaluate(async tracks=>{const {default:create}=await import('/th11.mjs');const c=window.core=await create({canvas:document.querySelector('canvas')});c.FS.writeFile('/th11.dat',new Uint8Array(await fetch('/th11.dat').then(r=>r.arrayBuffer())));c.FS.mkdir('/music');await Promise.all(tracks.map(async t=>c.FS.writeFile('/music/'+t.file,new Uint8Array(await fetch('/music/'+t.file).then(r=>r.arrayBuffer())))));c.FS.mkdir('/fonts');for(const name of ['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'])c.FS.writeFile('/fonts/'+name,new Uint8Array(await fetch('/fonts/'+name).then(r=>r.arrayBuffer())));window.error=()=>{const p=c._th11_error();return new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));};if(!c._th11_initialize())throw Error(error());},tracks);
 const result=await page.evaluate(()=>{
  const c=core,stats=()=>Array.from(c.HEAPU32.subarray(c._th11_audio_statistics()/4,c._th11_audio_statistics()/4+9));
  function energy(blocks=12){let l=0,r=0,peak=0;for(let j=0;j<blocks;++j){const p=c._th11_audio_samples();if(!p)throw Error('Mixer failure');const pcm=new Float32Array(c.HEAPU8.buffer,p,2048);for(let i=0;i<2048;i+=2){if(!Number.isFinite(pcm[i])||!Number.isFinite(pcm[i+1]))throw Error('Non-finite PCM');l+=pcm[i]*pcm[i];r+=pcm[i+1]*pcm[i+1];peak=Math.max(peak,Math.abs(pcm[i]),Math.abs(pcm[i+1]));}}return {l,r,peak};}
  const sounds=[];c._th11_audio_probe(2,0,0);
  for(let id=0;id<56;++id){c._th11_audio_probe(2,0,0);c._th11_audio_probe(1,id,0);sounds.push({id,...energy()});}
  const songs=[];for(let id=0;id<18;++id){c._th11_audio_probe(2,0,0);if(!c._th11_audio_probe(0,id,0))throw Error('Music '+id+' decode failed');songs.push({id,...energy(48)});}
  c._th11_audio_probe(2,0,0);c._th11_audio_probe(1,10,-192);const left=energy();c._th11_audio_probe(2,0,0);c._th11_audio_probe(1,10,192);const right=energy();
  c._th11_audio_probe(2,0,0);c._th11_audio_probe(0,1,0);c._th11_audio_probe(3,120,0);
  for(let i=0;i<120;++i)c._th11_audio_probe(4,0,0);const faded=energy();
  c._th11_music_enabled(0);c._th11_audio_probe(0,1,0);const muted=energy();c._th11_music_enabled(1);
  if(!c._th11_probe_stage(1,0,0,1))throw Error(error());for(let frame=0;frame<300;++frame)if(!c._th11_tick(1))throw Error(error());
  const battle=stats();if(!c._th11_return_title())throw Error(error());for(let i=0;i<5;++i)if(!c._th11_tick(0))throw Error(error());const title=stats();if(!c._th11_restart())throw Error(error());const restarted=stats();
  for(let i=0;i<40;++i)if(!c._th11_tick(0))throw Error(error());if(!c._th11_pause())throw Error('pause rejected');const paused=stats();if(!c._th11_resume())throw Error('resume rejected');const resumed=stats();
  if(!c._th11_probe_stage(6,0,0,1))throw Error(error());const stage6Start=stats();for(let i=0;i<40;++i)if(!c._th11_tick(0))throw Error(error());
  if(!c._th11_pause()||!c._th11_resume())throw Error('stage-six pause rejected');const stage6Resume=stats();
  while(c._th11_frame()<300)if(!c._th11_tick(0))throw Error(error());const stage6Before=stats();if(!c._th11_tick(0))throw Error(error());const stage6After=stats();
  if(!c._th11_probe_stage(5,0,0,1)||!c._th11_probe_complete())throw Error(error());
  for(let i=0;i<301;++i)if(!c._th11_tick(0))throw Error(error());if(!c._th11_pause()||!c._th11_resume())throw Error('cross-stage pause rejected');const crossStageResume=stats();
  for(let i=0;i<151;++i)if(!c._th11_tick(0))throw Error(error());const crossStageAfter=stats();
  return {sounds,songs,left,right,faded,muted,battle,title,restarted,paused,resumed,stage6Start,stage6Resume,stage6Before,stage6After,crossStageResume,crossStageAfter};
 });
 for(const s of [...result.sounds,...result.songs])assert.ok(s.l+s.r>1e-8,'Audible original sample '+s.id);
 assert.ok(result.left.l>result.left.r*5&&result.right.r>result.right.l*5,'original stereo positioning');
 assert.equal(result.faded.l+result.faded.r,0);assert.equal(result.muted.l+result.muted.r,0);
 assert.equal(result.battle[3],0);assert.ok(result.battle[2]>0);assert.equal(result.title[5],0);assert.equal(result.restarted[5],1);assert.deepEqual(errors,[]);
 assert.equal(result.paused[8],1);assert.equal(result.resumed[8],0);
 for(const state of[result.stage6Start,result.stage6Resume,result.stage6Before])assert.equal(state[8],1,'stage six holds music before frame 300');assert.equal(result.stage6After[8],0,'stage-six music resumes at native frame 300');
 assert.equal(result.crossStageResume[8],1,'cross-stage pause uses the activated stage clock, excluding the 150-frame handoff');assert.equal(result.crossStageAfter[8],0);
 await page.screenshot({path:resolve(out,'game.png')});
 const dialogue=[];
 for(let stage=1;stage<=7;++stage){for(let shot=0;shot<6;++shot){
  const entry=await page.evaluate(({stage,shot})=>{const c=core;if(!c._th11_probe_stage(stage,Math.floor(shot/3),shot%3,stage===7?4:1))throw Error(error());const before=c._th11_text_writes();if(!c._th11_probe_dialogue(0))throw Error('Dialogue entry unavailable '+stage+'/'+shot);for(let n=0;n<45;++n)if(!c._th11_tick(0))throw Error(error());return {stage,shot,writes:c._th11_text_writes()-before};},{stage,shot});
  assert.ok(entry.writes>0,`Dialogue pixels ${stage}/${shot}`);dialogue.push(entry);
  if(shot===0)await page.screenshot({path:resolve(out,'dialogue-stage'+stage+'.png')});
 }}
 assert.deepEqual(errors,[]);writeFileSync(resolve(out,'report.json'),JSON.stringify({passed:true,physicalDevice:false,...result,dialogue,errors},null,2));console.log(JSON.stringify({passed:true,sounds:56,music:18,dialogues:dialogue.length,stereo:true,fade:true,mute:true,titleMusic:true}));
 if(process.env.TH11_REALTIME_PROFILE){
  const transition=process.env.TH11_PROFILE_TRANSITION==='1',bytes=Array.from(readFileSync(resolve(root,transition?'reference/replays/th11_ud0189.rpy':'reference/replays/th11_ud0170.rpy')));
  await page.evaluate(bytes=>{const c=core;c.FS.writeFile('/save/replay/th11_01.rpy',Uint8Array.from(bytes));if(!c._th11_return_title())throw Error(error());const tick=(held,n=1)=>{for(let i=0;i<n;++i)if(!c._th11_tick(held))throw Error(error());};const press=held=>{tick(held);tick(0);};tick(0,100);press(32);press(32);press(1);tick(0,40);press(1);tick(0,16);press(1);tick(0,35);if(c._th11_phase()!==1)throw Error('Profile replay did not start');},bytes);
  for(let n=0;n<(transition?73:100);++n)await page.evaluate(()=>{for(let i=0;i<120;++i)if(!core._th11_tick(0))throw Error(error());});
  const cdp=await page.context().newCDPSession(page),rate=Number(process.env.TH11_REALTIME_PROFILE);await cdp.send('Emulation.setCPUThrottlingRate',{rate});
  await page.evaluate(()=>{window.frameSamples=[];window.stageChanges=0;window.lastProfileFrame=core._th11_frame();window.profileStart=performance.now();core.onGameFrame=(ok,ms,ticks)=>{if(!ok)throw Error(error());frameSamples.push({ms,ticks});const frame=core._th11_frame();if(frame<lastProfileFrame)++stageChanges;lastProfileFrame=frame;};window.profileFrame=core._th11_frame();core._th11_loop_start();});
  await page.waitForTimeout(20000);
  const perf=await page.evaluate(()=>{core._th11_loop_stop();const durationMs=performance.now()-profileStart,frames=frameSamples.reduce((n,s)=>n+s.ticks,0),ms=frameSamples.map(s=>s.ms).sort((a,b)=>a-b),gl=document.querySelector('canvas').getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');return {durationMs,frames,stageChanges,fps:frames*1000/durationMs,samples:ms.length,p50:ms[Math.floor(ms.length*.5)],p95:ms[Math.floor(ms.length*.95)],p99:ms[Math.floor(ms.length*.99)],max:ms.at(-1),renderer:ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER),phase:core._th11_phase()};});
  assert.equal(perf.phase,1);assert.ok(perf.frames>0);if(transition)assert.equal(perf.stageChanges,1);assert.deepEqual(errors,[]);writeFileSync(resolve(out,transition?'transition-profile.json':'realtime-profile.json'),JSON.stringify({physicalDevice:false,cpuThrottlingRate:rate,wasmSha256,...perf},null,2));console.log(JSON.stringify({realtime:perf}));await cdp.send('Emulation.setCPUThrottlingRate',{rate:1});
 }
 if(['1','extra'].includes(process.env.TH11_GPU_CAMPAIGN)){
  const extra=process.env.TH11_GPU_CAMPAIGN==='extra',replay=Array.from(readFileSync(resolve(root,extra?'reference/replays/th11_ud0170.rpy':'reference/replays/th11_ud0189.rpy')));
  await page.evaluate(bytes=>{const c=core;c.FS.writeFile('/save/replay/th11_01.rpy',Uint8Array.from(bytes));if(!c._th11_return_title())throw Error(error());const tick=(held,n=1)=>{for(let i=0;i<n;++i)if(!c._th11_tick(held))throw Error(error());};const press=held=>{tick(held);tick(0);};tick(0,100);press(32);press(32);press(1);tick(0,40);press(1);tick(0,16);press(1);tick(0,35);if(c._th11_phase()!==1)throw Error('Original replay menu did not start gameplay');},replay);
  let ticks=0,lastFrame=0,stage=extra?7:1;const visits=[stage],timings=[];
  while(ticks<110000){const state=await page.evaluate(()=>{const c=core,start=performance.now();let ticks=0;for(;ticks<120&&c._th11_phase()===1;++ticks)if(!c._th11_tick(0))throw Error(error());return {ticks,frame:c._th11_frame(),phase:c._th11_phase(),milliseconds:performance.now()-start};});ticks+=state.ticks;timings.push(state.milliseconds);if(state.phase===1&&state.frame<lastFrame){visits.push(++stage);console.log('GPU stage',stage,'at',ticks);await page.screenshot({path:resolve(out,'campaign-stage'+stage+'.png')});}lastFrame=state.frame;if(state.phase!==1)break;if(ticks%6000===0)console.log('GPU campaign frames',ticks);}
  assert.deepEqual(visits,extra?[7]:[1,2,3,4,5,6]);assert.ok(extra?ticks>43000&&ticks<44000:ticks>99000&&ticks<102000,'original full campaign plays without injected lives or exits');assert.deepEqual(errors,[]);const gpu={passed:true,physicalDevice:false,wasmSha256,ticks,visits,maxBatchMs:Math.max(...timings),scope:'External original clear replay selected through native menu, real SDL/WebGL2 draw/audio/text for every tick. Desktop Chromium; accelerated stepping is not a phone FPS measurement.'};writeFileSync(resolve(out,extra?'extra.json':'campaign.json'),JSON.stringify(gpu,null,2));console.log(JSON.stringify(gpu));
 }
}catch(e){writeFileSync(resolve(out,'failure.json'),JSON.stringify({error:e.stack,errors},null,2));throw e;}
finally{if(browser)await browser.close();await new Promise(r=>server.close(r));}
