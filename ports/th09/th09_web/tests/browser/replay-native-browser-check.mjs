import {writeFileSync,readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
import {core,memory,root} from '../cpp/helpers.mjs';
const run=await presentationServer(),browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']}),results=[];
try {
 for(const replay of [0,1,2]){
 const c=await core(),f=c.session_create(),name=c.allocate(128),data=c.allocate(16000000);
 for(const n of readdirSync(resolve(root,'reference/assets')).filter(n=>/\.(anm|sht|ecl|msg|std|bmp|png)$/.test(n))){const b=readFileSync(resolve(root,'reference/assets',n));memory(c,name,128).fill(0);memory(c,name,128).set(new TextEncoder().encode(n));memory(c,data,b.length).set(b);c.session_file(f,name,data,b.length);}
 const raw=readFileSync(resolve(root,`reference/assets/demorpy${replay}.rpy`));memory(c,data,raw.length).set(raw);if(!c.session_play(f,data,raw.length,9))throw Error('fixture replay failed');
 const context=await browser.newContext(),page=await context.newPage();await page.goto(run.url+'/app/th09.html');await page.locator('#start').click();await page.waitForFunction(()=>window.__th09Runtime,null,{timeout:120000});
 await page.evaluate(raw=>{const c=__th09Runtime.core;c._th09_loop_pause(1);const p=c._th09_import_buffer(raw.length);c.HEAPU8.set(raw,p);if(!c._th09_probe_replay(raw.length,9))throw Error('browser replay failed');},Array.from(raw));
 const expected=[],frames=c.session_value(f,3),before=Array.from(new Int32Array(c.memory.buffer,c.session_snapshot(f),48));
 for(let n=0;n<frames;++n){if(!c.session_step(f,0,0,0))throw Error('fixture step '+n);expected.push(Array.from(new Int32Array(c.memory.buffer,c.session_snapshot(f),48)));if(c.session_value(f,0)!==1)break;}
 const report=await page.evaluate(async({expected,before})=>{const c=__th09Runtime.core,snapshot=()=>Array.from(c.HEAP32.subarray(c._th09_probe_world()/4,c._th09_probe_world()/4+48)),initial=snapshot();const diffs=[];let frames=0;for(let n=0;n<expected.length;++n){if(!c._th09_probe_tick(0))throw Error('browser step '+n);const actual=snapshot();const diff=actual.flatMap((value,j)=>j!==1&&value!==expected[n][j]?[{field:j,actual:value,expected:expected[n][j]}]:[]);frames=n+1;if(diff.length){diffs.push({frame:n,diff,actual,expected:expected[n]});break;}if(n%100===99)await new Promise(requestAnimationFrame);}return {initial,before,frames,diffs};},{expected,before});
 results.push({replay,...report});console.log(JSON.stringify({replay,frames:report.frames,difference:report.diffs[0]}));await context.close();c.session_delete(f);c.release(name);c.release(data);
 }
 writeFileSync(resolve(root,'artifacts/bugfix-20260920/browser-replay-comparison.json'),JSON.stringify(results,null,2));
}finally{await browser.close();run.netplay.close();await new Promise(r=>run.server.close(r));}
