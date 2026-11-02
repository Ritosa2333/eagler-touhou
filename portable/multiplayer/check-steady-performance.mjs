import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {dirname,resolve} from 'node:path';
import {parseArgs} from 'node:util';
import {launchBrowser} from '../../../../tools/browser/launch.mjs';

const {values}=parseArgs({options:{url:{type:'string'},output:{type:'string'},rounds:{type:'string',default:'8'},'cpu-rate':{type:'string',default:'4'}}});
assert(values.url&&values.output,'Pass --url and --output');
const rounds=Number(values.rounds),rate=Number(values['cpu-rate']);
assert(Number.isInteger(rounds)&&rounds>0&&rounds<=50&&Number.isFinite(rate)&&rate>=1);
const output=resolve(values.output);mkdirSync(dirname(output),{recursive:true});
const report={passed:false,scope:'No-B original normal/Extra simulation; serial A/B/B/A with identical native state and playfield pixels',cpuRate:rate,rounds,cases:[],errors:[]};
const save=()=>writeFileSync(output,JSON.stringify(report,null,2)+'\n');
const browser=await launchBrowser();
try{
 for(const scenario of [{players:2,difficulty:1,bullets:0},{players:3,difficulty:1,bullets:1000},{players:3,difficulty:4,bullets:0},{players:2,difficulty:4,bullets:600},{players:3,difficulty:4,bullets:1200}]){
  const page=await browser.newPage();
  try{
   page.on('pageerror',e=>report.errors.push(e.message));await page.goto(values.url);
   await page.waitForFunction(()=>!!window.multiplayerSmoke,{timeout:120000});
   await page.evaluate(s=>multiplayerSmoke.start(1234,Array.from({length:s.players},(_,i)=>i),0,null,null,s.difficulty),scenario);
   const setup=await page.evaluate(s=>{
    const w=multiplayerSmoke;w.ticks(120);for(let i=0;i<3;++i){w.key('KeyZ',true);w.ticks(2);w.key('KeyZ',false);w.ticks(120);}
    if(s.bullets&&!w.denseBullets(s.bullets))throw Error('Dense field setup failed');
    w.commit(Array(s.players).fill(1));w.ticks(24);
    const gl=document.querySelector('canvas').getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');
    return {status:w.nativeStatus(),identity:w.identity().wasmSha256,renderer:gl.getParameter(ext?ext.UNMASKED_RENDERER_WEBGL:gl.RENDERER)};
   },scenario);
   assert.equal(setup.status[2],0);assert.equal(setup.status[1],scenario.difficulty===4?8:0);
   if(process.env.NATIVE_GPU==='1')assert(!/swiftshader|llvmpipe|software|basic render|warp/i.test(setup.renderer),setup.renderer);
   const session=await page.context().newCDPSession(page);await session.send('Emulation.setCPUThrottlingRate',{rate});
   // Warm both policies before recording their samples.
   await page.evaluate(()=>{const w=multiplayerSmoke;for(const enabled of [false,true]){w.steadyMode(enabled);if(!w.extraProfile(0,2)[0])throw Error('Warmup restore failed');}});
   const samples=[];
   for(const optimized of [false,true,true,false]){
    const sample=await page.evaluate(async o=>{
     const w=multiplayerSmoke;w.steadyMode(o.optimized);const words=w.extraProfile(0,o.rounds);
     const crop=document.createElement('canvas');crop.width=384;crop.height=448;
     crop.getContext('2d').drawImage(document.querySelector('canvas'),32,16,384,448,0,0,384,448);
     const pixels=crop.getContext('2d').getImageData(0,0,384,448).data;
     const pixelHash=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',pixels)),x=>x.toString(16).padStart(2,'0')).join('');
     return {optimized:o.optimized,words,pixelHash};
    },{optimized,rounds});
    assert.equal(sample.words[0],1,'Native checkpoint/restore failed');assert.equal(sample.words[11],0,'No-B workload required');
    if(samples.length){assert.deepEqual(sample.words.slice(12),samples[0].words.slice(12),'Optimization changed native state');assert.equal(sample.pixelHash,samples[0].pixelHash,'Optimization changed playfield pixels');}
    samples.push(sample);
   }
   const summarize=enabled=>{
    const rows=samples.filter(x=>x.optimized===enabled).map(x=>x.words),ticks=rows.reduce((sum,row)=>sum+row[1],0);
    return {captureMsPerTick:rows.reduce((sum,r)=>sum+r[2],0)/ticks,updateDrawMsPerTick:rows.reduce((sum,r)=>sum+r[3],0)/ticks,restoreMsPerTick:rows.reduce((sum,r)=>sum+r[4],0)/ticks,maxCheckpointBytes:Math.max(...rows.map(r=>r[5]))};
   };
   await page.evaluate(()=>multiplayerSmoke.steadyMode(true));
   assert(await page.evaluate(()=>multiplayerSmoke.steadyNumeric()),'Numeric reference mismatch');
   const ownership=await page.evaluate(()=>multiplayerSmoke.liveBulletProbe());assert.deepEqual(ownership.slice(0,3),[1,1,0],'Whole-pool byte ownership failed');
   const row={...scenario,...setup,baseline:summarize(false),optimized:summarize(true),samples,ownership,passed:true};
   report.cases.push(row);save();console.log(JSON.stringify({...scenario,baseline:row.baseline,optimized:row.optimized}));
  }finally{await page.close();}
 }
 assert.deepEqual(report.errors,[]);report.passed=true;
}catch(error){report.error=String(error);throw error;}
finally{save();await browser.close();}
