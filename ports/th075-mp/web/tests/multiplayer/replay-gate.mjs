import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createRequire} from 'node:module';
const root=resolve(import.meta.dirname,'../../..'),launcher=resolve(process.env.EAGLER_LAUNCHER_ROOT||resolve(root,'../eagler-touhou'));
const require=createRequire(resolve(launcher,'package.json'));
const {default:puppeteer}=await import(pathToFileURL(require.resolve('puppeteer-core')));
const {findChromiumExecutable}=await import(pathToFileURL(resolve(launcher,'lib/chromium-executable.mjs')));
const outputFile=name=>resolve(root,'web/artifacts/multiplayer-browser',name);
import {readFile,writeFile} from 'node:fs/promises';
const recording=JSON.parse(await readFile(outputFile('replays.json'),'utf8'))[0][0];
// Predicted presentation is not an offline Replay oracle. The adapter exposes
// the actor sample from each reconciled/confirmed history entry separately.
const live=JSON.parse(await readFile(outputFile('browser-match.json'),'utf8')).states.map(s=>s[0].s).filter(s=>s[8]===0).map(s=>s[51]?{frame:s[55],actors:s.slice(56,64)}:{frame:s[12],actors:[...s.slice(16,20),...s.slice(32,36)]}).filter(s=>s.frame>180);
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
try{const page=await browser.newPage();await page.goto(process.env.EAGLER_TH075_MP_TEST_URL||'http://127.0.0.1:18375/');await page.waitForFunction(()=>host.events.some(e=>e.event==='ready'),{timeout:120000});
await page.evaluate(async recording=>{await host.request('configure',{language:'ja',music:'none',options:{replayViewer:true}});await host.request('write',{path:'replay/replay000.rep',bytes:recording.bytes});await host.request('launch');},recording);
await page.evaluate(()=>{host.replayHistory=new Map();function poll(){const w=document.querySelector('iframe').contentWindow,c=w.Module;if(c){const at=c._th075_mp_status(),s=Array.from(c.HEAPU32.subarray(at/4,at/4+64));if(c._th075_scene()===0&&s[12]>0)host.replayHistory.set(s[12],s);}requestAnimationFrame(poll);}poll();});
await page.evaluate(()=>host.key('KeyZ',true));await new Promise(r=>setTimeout(r,100));await page.evaluate(()=>host.key('KeyZ',false));
await page.waitForFunction(()=>{const c=document.querySelector('iframe').contentWindow.Module;return c._th075_scene()===0||host.error();},{timeout:60000});
// A short completed recording can return to the menu before 230 RAF samples.
// Keep the exact-frame comparisons below; wait for samples or actual playback EOF.
await page.waitForFunction(()=>host.replayHistory.size>230||(host.replayHistory.size>0&&document.querySelector('iframe').contentWindow.Module._th075_scene()===5)||host.error(),{timeout:60000});
const values=await page.evaluate(()=>({history:[...host.replayHistory],error:host.error(),scene:document.querySelector('iframe').contentWindow.Module._th075_scene()}));assert.equal(values.error,null);
const replay=new Map(values.history);let compared=0;for(const s of live){const t=replay.get(s.frame);if(!t)continue;assert.deepEqual(s.actors,[...t.slice(16,20),...t.slice(32,36)],'Replay differs at simulation frame '+s.frame);++compared;}assert.ok(compared>=5,'insufficient matching replay frames '+compared);await writeFile(outputFile('replay-gate.json'),JSON.stringify({compared,frames:values.history.length,scene:values.scene}));console.log('PASS native offline Replay matches',compared,'recorded combat states');
}finally{await browser.close();}
