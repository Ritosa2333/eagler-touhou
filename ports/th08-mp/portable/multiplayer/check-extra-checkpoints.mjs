import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {dirname,resolve} from 'node:path';
import {parseArgs} from 'node:util';
import {launchBrowser} from '../../../../tools/browser/launch.mjs';

const {values}=parseArgs({options:{url:{type:'string'},output:{type:'string'},rounds:{type:'string',default:'12'}}});
assert(values.url&&values.output,'Pass --url and --output');
const rounds=Number(values.rounds);assert(Number.isInteger(rounds)&&rounds>0&&rounds<=50);
const report={passed:false,scope:'Original Extra Update/Draw with dense projectiles; same-world baseline versus batched checkpoints and conservative barrier rejection, exact undo and replay hashes',cases:[],errors:[]};
const output=resolve(values.output);mkdirSync(dirname(output),{recursive:true});
const save=()=>writeFileSync(output,JSON.stringify(report,null,2)+'\n');
const browser=await launchBrowser();
try{
 for(const loadouts of [[0,1],[2,3,10]])for(const focus of [false,true]){
  const page=await browser.newPage();
  try{
   page.on('pageerror',e=>report.errors.push(e.message));
   await page.goto(values.url);await page.waitForFunction(()=>!!window.multiplayerSmoke,{timeout:120000});
   await page.evaluate(chars=>multiplayerSmoke.start(1234,chars,0,null,null,4),loadouts);
   const setup=await page.evaluate(()=>{const w=multiplayerSmoke;w.ticks(120);for(let i=0;i<3;++i){w.key('KeyZ',true);w.ticks(2);w.key('KeyZ',false);w.ticks(120);}return {status:w.nativeStatus(),identity:w.identity()};});
   assert.equal(setup.status[1],8,'Original Extra stage required');assert.equal(setup.status[2],0);
   const renderer=await page.evaluate(()=>{const gl=document.querySelector('canvas').getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');return gl.getParameter(ext?ext.UNMASKED_RENDERER_WEBGL:gl.RENDERER);});
   if(process.env.NATIVE_GPU==='1')assert(!/swiftshader|llvmpipe|software|basic render|warp/i.test(renderer),renderer);
   const sample=async label=>{
    const samples=[];
    // A/B/B/A alternation uses identical restored native state on both paths.
    for(const batched of [false,true,true,false]){
     const row=await page.evaluate(({batched,rounds})=>{const w=multiplayerSmoke;w.batchCheckpointCopies(batched);w.barrierBroadphase(batched);return w.extraProfile(0,rounds);},{batched,rounds});
     assert.equal(row[0],1,'Checkpoint or native restore diverged');
     if(samples.length)assert.deepEqual(row.slice(12),samples[0].words.slice(12),'Checkpoint policy changed native state');
     samples.push({batched,words:row});
    }
    const summarize=batched=>{const rows=samples.filter(x=>x.batched===batched).map(x=>x.words),ticks=rows.reduce((s,r)=>s+r[1],0);return {captureMsPerTick:rows.reduce((s,r)=>s+r[2],0)/ticks,updateDrawMsPerTick:rows.reduce((s,r)=>s+r[3],0)/ticks,restoreMsPerSixTicks:rows.reduce((s,r)=>s+r[4],0)/(rows.length*rounds),maxCheckpointBytes:Math.max(...rows.map(r=>r[5])),bombActive:Math.max(...rows.map(r=>r[11]))};};
    const result={loadouts,focus,label,identity:setup.identity.wasmSha256,renderer,baseline:summarize(false),optimized:summarize(true),samples};
    report.cases.push(result);save();console.log(JSON.stringify({loadouts,focus,label,baseline:result.baseline,optimized:result.optimized}));return result;
   };
   assert(await page.evaluate(()=>multiplayerSmoke.denseBullets(1000)));
   await sample('dense-extra');
   // Establish real Bomb objects with the normal committed input path.
   await page.evaluate(({count,focus})=>{const w=multiplayerSmoke;w.commit(Array(count).fill(3|(focus?4:0)));w.ticks(1);w.commit(Array(count).fill(1|(focus?4:0)));w.ticks(24);},{count:loadouts.length,focus});
   const active=await sample('active-bomb');assert.equal(active.optimized.bombActive,1,'Bomb must be active in the measured window');
   await page.evaluate(()=>multiplayerSmoke.ticks(60));
   await sample('bomb-tail');
   assert(await page.evaluate(()=>multiplayerSmoke.collisionProbe()),'Conservative broadphase rejects an exact hit');
   assert(await page.evaluate(()=>multiplayerSmoke.eclAimingProbe()),'ECL targeting must select live peers across all stages');
  }finally{await page.close();}
 }
 assert.deepEqual(report.errors,[]);report.passed=true;
}catch(error){report.error=String(error);throw error;}
finally{save();await browser.close();}
