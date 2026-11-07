import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {mkdirSync,writeFileSync} from 'node:fs';
const option=(name,fallback)=>process.argv.find(arg=>arg.startsWith(`--${name}=`))?.slice(name.length+3)||fallback;
const output=resolve(option('output','artifacts/audio-browser'));mkdirSync(output,{recursive:true});
const executablePath=process.env.TH19_BROWSER;if(!executablePath)throw Error('Set TH19_BROWSER.');
const modulePath=process.env.TH19_PUPPETEER_MODULE||resolve(import.meta.dirname,'../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js');
const {default:puppeteer}=await import(pathToFileURL(modulePath).href);
const report={passed:false,errors:[],samples:[]};const save=()=>writeFileSync(resolve(output,'sfx-report.json'),JSON.stringify(report,null,2)+'\n');save();
const browser=await puppeteer.launch({executablePath,headless:true,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required'],timeout:60000});
try{
 const page=await browser.newPage();page.on('pageerror',e=>report.errors.push(e.message));
 await page.goto(new URL('tests/harness.html',option('url','http://127.0.0.1:8139/')).href,{waitUntil:'domcontentloaded'});
 await page.waitForFunction(()=>window.__TH19_HARNESS?.frame?.contentWindow?.__th19Runtime?.status().title[0]>0,{timeout:120000});
 await page.evaluate(async()=>{
  const w=window.__TH19_HARNESS.frame.contentWindow,c=w.__th19Runtime.core;c._th19_loop_pause(1);c._th19_test_ticks(600);
  const pool=c.eaglerSfxOutput;await pool.context.resume();
  const analyser=pool.context.createAnalyser();analyser.fftSize=2048;const mute=pool.context.createGain();mute.gain.value=0;analyser.connect(mute);mute.connect(pool.context.destination);
  for(const v of pool.voices.values())v.merge.connect(analyser);
  const start=pool.start;pool.starts=[];pool.start=v=>{start(v);if(v.node)v.merge.connect(analyser);pool.starts.push({hasNode:!!v.node,offset:v.offset,volume:v.volume,time:pool.context.currentTime});};
  window.__TH19_AUDIO={analyser};
 });
 for(let repeat=0;repeat<4;repeat++){
  report.samples.push(await page.evaluate(async()=>{
   const w=window.__TH19_HARNESS.frame.contentWindow,c=w.__th19Runtime.core,pool=c.eaglerSfxOutput;
   const before=pool.starts.length;
   w.dispatchEvent(new w.KeyboardEvent('keydown',{code:'ArrowDown',key:'ArrowDown',bubbles:true}));c._th19_test_ticks(1);
   w.dispatchEvent(new w.KeyboardEvent('keyup',{code:'ArrowDown',key:'ArrowDown',bubbles:true}));c._th19_test_ticks(30);
   const samples=new Float32Array(2048);let peak=0;const end=performance.now()+500;
   while(performance.now()<end){window.__TH19_AUDIO.analyser.getFloatTimeDomainData(samples);for(const value of samples)peak=Math.max(peak,Math.abs(value));await new Promise(r=>setTimeout(r,10));}
   return {peak,context:pool.context.state,starts:pool.starts.slice(before)};
  }));save();
 }
 assert.deepEqual(report.errors,[]);
 assert(report.samples.every(s=>s.context==='running'&&s.starts.some(v=>v.hasNode)&&s.peak>0.0001),'every repeat of native menu SFX must produce nonzero PCM');
 report.passed=true;save();console.log(JSON.stringify(report));
}catch(error){report.failure=error.stack;save();throw error;}finally{await browser.close();}
