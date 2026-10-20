import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';import {createRequire} from 'node:module';import {pathToFileURL} from 'node:url';
const root=resolve(import.meta.dirname,'../../..'),runtime=resolve(process.env.EAGLER_TH075_MP_RUNTIME||resolve(root,'web/artifacts/rollback-harness'));
const launcher=resolve(process.env.EAGLER_LAUNCHER_ROOT||resolve(root,'../eagler-touhou'));const require=createRequire(resolve(launcher,'package.json'));
const {default:puppeteer}=await import(pathToFileURL(require.resolve('puppeteer-core')));
const {findChromiumExecutable}=await import(pathToFileURL(resolve(launcher,'lib/chromium-executable.mjs')));
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
try{const page=await browser.newPage();await page.setRequestInterception(true);page.on('request',r=>r.url().endsWith('/runtime/shell.mjs')?r.respond({status:200,contentType:'text/javascript',body:readFileSync(resolve(runtime,'shell.mjs'),'utf8').replace('multiplayer:true','multiplayer:false').replace('"multiplayer":true','"multiplayer":false')}):r.continue());page.on('pageerror',e=>console.error(e));
 await page.goto(process.env.EAGLER_TH075_MP_TEST_URL||'http://127.0.0.1:18375/');await page.waitForFunction(()=>host.events.some(e=>e.event==='ready')||host.error(),{timeout:120000});
 await page.evaluate(async()=>{await host.request('configure',{language:'ja',music:'none',options:{}});await host.request('launch');document.querySelector('iframe').contentWindow.Module._th075_loop_pause(1);});
 for(let phase=0;phase<4;++phase)for(let character=0;character<11;++character){const result=await page.evaluate(async ({character,phase})=>{const c=document.querySelector('iframe').contentWindow.Module;const prepared=await c.ccall('th075_mp_prepare','number',Array(6).fill('number'),[1947,character,(character+3)%11,8,4,1],{async:true});if(!prepared)return {error:c.UTF8ToString(c._th075_error())};const ok=c._th075_probe_rollback(8|(phase<<8));return {ok,error:c.UTF8ToString(c._th075_error())};},{character,phase});assert.equal(result.ok,1,JSON.stringify({character,...result}));console.log('PASS native snapshot, divergent speculative branch, restore/replay',character,'phase',phase);}
}finally{await browser.close();}
