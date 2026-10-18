import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const out='th10_web/artifacts/architecture-candidate/validation';mkdirSync(out,{recursive:true});
const language=process.argv.includes('--jp')?'jp':'chs',browser=await launchBrowser(),context=await browser.newContext({viewport:{width:844,height:634}}),page=await context.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));
try{
 await page.addInitScript(()=>{Date.now=()=>1788840000000;});await page.goto('http://127.0.0.1:8093/runtime/th10/th10.html?language='+ (language==='chs'?'lang_zh-hans':'ja'));
 await page.waitForFunction(()=>window.__th10Runtime,null,{timeout:120000});
 await page.evaluate(language=>{const r=__th10Runtime,c=r.core;r.Module.runtimePrepare=()=>0;const app=c.sdl_game_open(language==='chs',Date.now()&65535);const mem=()=>new DataView(c.memory.buffer),name=c.graphics_allocate(100);window.probe={r,c,app,keys:new Set(),tick(keys=[]){const wanted=new Set(keys);for(const key of new Set([...this.keys,...wanted]))if(this.keys.has(key)!==wanted.has(key)){new Uint8Array(c.memory.buffer,name,100).set(new TextEncoder().encode(key+'\0'));c.sdl_key(name,wanted.has(key));}this.keys=wanted;const result=c.sdl_loop_tick(app,1/60,17);if(result)throw Error('C++ tick failed '+result);},wait(n,keys=[]){for(let i=0;i<n;i++)this.tick(keys);}};probe.wait(330);for(let i=0;i<5;i++){probe.wait(3,['KeyZ']);probe.wait(42);}probe.wait(100);if(mem().getInt32(c.application_state(app)+0x38c,true)!==7)throw Error('Gameplay did not start');},language);
 const cdp=await context.newCDPSession(page);await cdp.send('Profiler.enable');await cdp.send('Profiler.start');
 const report=await page.evaluate(async language=>{const {c,app}=probe,start=Array.from(new Uint32Array(c.memory.buffer,c.sdl_stats(),6)),times=[],spikes=[];
  for(let i=0;i<14000;i++){const p=c.world_actor(c.application_world(app),1),v=new DataView(c.memory.buffer);if(p){v.setInt32(p+0x430c,120,true);v.setInt32(p+0x4310,121,true);v.setFloat32(p+0x4314,121,true);}const begin=performance.now();probe.tick(['KeyZ','ShiftLeft',...(language==='chs'?['ControlLeft']:[])]);const ms=performance.now()-begin;times.push(ms);if(ms>5)spikes.push({i,ms,state:Array.from(new Int32Array(c.memory.buffer,c.sdl_game_status(),10))});if(i%120===119)await new Promise(r=>setTimeout(r,0));}
  const end=Array.from(new Uint32Array(c.memory.buffer,c.sdl_stats(),6));return {language,state:Array.from(new Int32Array(c.memory.buffer,c.sdl_game_status(),10)),stats:end.map((n,i)=>n-start[i]),fontErrors:c.sdl_fonts_errors(),times,spikes};
 },language);
 const {profile}=await cdp.send('Profiler.stop');writeFileSync(out+'/behavior-'+language+'.cpuprofile',JSON.stringify(profile));await page.screenshot({path:out+'/behavior-'+language+'.png'});
 const baseline=JSON.parse(readFileSync('th10_web/artifacts/sdl3/performance/architecture-2.3-'+(language==='chs'?'behavior':'jp')+'/report.json'));
 assert.equal(report.state[1],baseline.screen.stage);assert.equal(report.state[2],0);assert.equal(report.state[3],baseline.screen.lives);assert.equal(report.state[4],baseline.screen.power);assert.equal(report.fontErrors,0);assert.equal(errors.length,0);
 report.baseline={screen:baseline.screen,stats:baseline.stats};report.errors=errors;report.passed=true;writeFileSync(out+'/behavior-'+language+'.json',JSON.stringify(report,null,2));console.log(JSON.stringify({...report,times:undefined,spikes:report.spikes.filter(s=>s.ms>15)}));
}catch(e){writeFileSync(out+'/behavior-failure.json',JSON.stringify({error:e.stack,errors},null,2));throw e;}finally{await browser.close();}
