import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {writeFileSync,mkdirSync,readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
const build=resolve(process.env.PC_BUILD||'ports/th09/web/artifacts/sdl3');
const host=await presentationServer(0,{artifactDirectory:build});
const browser=await launchBrowser();
try{
 const page=await browser.newPage();const errors=[];page.on('pageerror',e=>errors.push(String(e)));
 await page.goto(host.url);
 const result=await page.evaluate(async()=>{
  await openProbe(0,1,3,1);const c=core;
  const ints=(fn,n)=>Array.from(c.HEAP32.subarray(c[fn]()/4,c[fn]()/4+n));
  const motion=()=>Array.from(new Float32Array(c.HEAPU8.buffer,c._th09_touch_probe(),8));
  const real=n=>new Float32Array(new Int32Array([n]).buffer)[0];
  c._th09_touch_options(1,1,1,0,0);c._th09_touch_controls(1,0,0,0,0);
  let dragging=false,previousStop=false,stoppedSamples=0,resumes=0;
  for(let frame=0;frame<3600;++frame){
   const w=ints('_th09_probe_world',48),state=ints('_th09_touch_state',4),stopped=!!(w[2]&0x1800);
   const before=motion();
   const charging=real(w[11])>=205;c._th09_key(44,charging?+(real(w[10])<202):+(frame%6<3));c._th09_key(29,+(frame<140));
   if(state[1]&&!dragging){c._th09_touch(0,71,.5,.5);dragging=true;}
   if(stopped&&dragging){if(state[1])throw Error('Declaration still accepts movement');c._th09_touch(1,71,frame%2?.9:.1,.8);++stoppedSamples;}
   const resume=previousStop&&!stopped&&state[1]&&dragging;
   if(!c._th09_game_tick(0))throw Error(probeError());
   const after=motion();
   if(resume){if(Math.abs(after[0]-before[0])>.001||Math.abs(after[1]-before[1])>.001)throw Error('Buffered displacement leaked after declaration');++resumes;
    c._th09_touch(1,71,.51,.8);
   }
   previousStop=stopped;
   if(resumes>=3)break;
  }
  return {stoppedSamples,resumes};
 });
 assert.deepEqual(errors,[]);assert.ok(result.stoppedSamples>0&&result.resumes>=3,'Three real declaration recoveries exercised');
 const report={passed:true,scope:'Real game declaration and held touch through native sampling. Death/revival covered separately by controller tests.',wasm:createHash('sha256').update(readFileSync(resolve(build,'th09-presentation.wasm'))).digest('hex'),result};
 const out=resolve('ports/th09/web/artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});writeFileSync(resolve(out,'touch-stop-report.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report));
}finally{await browser.close();host.netplay.close();await new Promise(r=>host.server.close(r));}
