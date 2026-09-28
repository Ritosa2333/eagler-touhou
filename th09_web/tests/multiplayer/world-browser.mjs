import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
const dense=process.argv.includes('--dense'),roundReset=process.argv.includes('--round-reset'),label=process.env.RUN_LABEL||'world-browser';
const characters=(process.env.CHARACTERS||'0,1').split(',').map(Number);
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const {server,netplay,url}=await presentationServer();const browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
const page=await browser.newPage();const errors=[];page.on('pageerror',e=>errors.push(e.stack));
try{
 await page.goto(url);await page.evaluate(chars=>openProbe(chars[0],chars[1],2,3),characters);
 const result=await page.evaluate(({dense,roundReset})=>{
   const c=core,fail=()=>{throw Error(probeError());};
   const keys=f=>[sessionStatus()[0]===1&&f%8<4?1:0,(sessionStatus()[0]===1?1:0)|((Math.floor(f/80)%2)?64:128)];
   let checkpoints=0,bytes=0,captureMs=0,restoreMs=0,stepMs=0;
   for(let f=0;f<1800;){
     if(roundReset&&f===600)c._th09_probe_end_round(0);
     if(f<180){const [a,b]=keys(f++);if(!c._th09_probe_rollback_step(a|256,b,0))fail();continue;}
     let start=performance.now();bytes=Math.max(bytes,c._th09_probe_checkpoint(+dense));captureMs+=performance.now()-start;if(!bytes)fail();
     const readParts=()=>{const p=c._th09_probe_hash_parts();return new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p))).split("\n");};
     const checkpointHash=c._th09_probe_state_hash(),checkpointParts=readParts();
     for(let n=0;n<6;++n){const [a,b]=keys(f+n);start=performance.now();if(!c._th09_probe_rollback_step(a,b,n===5))fail();stepMs+=performance.now()-start;}
     const parts=()=>{const at=c._th09_probe_hash_parts(),end=c.HEAPU8.indexOf(0,at);return new TextDecoder().decode(c.HEAPU8.subarray(at,end)).split("\n");};
     const text=at=>new TextDecoder().decode(c.HEAPU8.subarray(at,c.HEAPU8.indexOf(0,at)));
     const detail=text(c._th09_probe_enemy_detail(0,4));
     const expected=c._th09_probe_state_hash(),before=parts();const image=document.querySelector('canvas').toDataURL();
     start=performance.now();if(!c._th09_probe_restore())fail();restoreMs+=performance.now()-start;
     if(c._th09_probe_state_hash()!==checkpointHash){const actualParts=readParts(),at=checkpointParts.findIndex((p,i)=>p!==actualParts[i]);throw Error(`initial state restore mismatch at ${f}: part ${at}: ${checkpointParts[at]} / ${actualParts[at]}`);}
     for(let n=0;n<6;++n){const [a,b]=keys(f+n);if(!c._th09_probe_rollback_step(a,b,n===5))fail();}
     const actual=c._th09_probe_state_hash();if(expected!==actual){const after=parts(),index=before.findIndex((p,i)=>p!==after[i]);throw Error(`world mismatch frame ${f+6}: ${expected} != ${actual}; part ${index}: ${before[index]} / ${after[index]}; details ${detail} / ${text(c._th09_probe_enemy_detail(0,4))}; preceding ${before.slice(20,38).join(";")}`);}
     if(image!==document.querySelector('canvas').toDataURL())throw Error(`framebuffer mismatch frame ${f+6}`);
     ++checkpoints;f+=6;
   }
   const poolBoundary=c._th09_probe_pool_boundary();if(!poolBoundary)throw Error("owning pool clear/reuse/overflow mismatch");
   const recordingBoundary=c._th09_probe_recording_boundary();if(!recordingBoundary)throw Error("recording chunk/motion rewind mismatch");
   return {checkpoints,frames:1800,poolBoundary,recordingBoundary,bytes,captureMs,restoreMs,stepMs,status:probeStatus(),session:sessionStatus()};
 },{dense,roundReset});
 assert.equal(errors.length,0,errors.join('\n'));const build=JSON.parse(readFileSync(resolve(root,'artifacts/sdl3/build.json')));writeFileSync(resolve(out,label+'-report.json'),JSON.stringify({passed:true,dense,characters,wasm:build.sha256,scope:'Same live TH09 world restore/resimulation with state inventory and framebuffer comparison; desktop Chromium',result,errors},null,2));console.log(JSON.stringify(result));
}catch(e){writeFileSync(resolve(out,'world-browser-failure.json'),JSON.stringify({error:e.stack,errors},null,2));throw e;}
finally{await browser.close();netplay.close();await new Promise(r=>server.close(r));}
