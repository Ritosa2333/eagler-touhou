import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
const option=(n,f)=>process.argv.find(a=>a.startsWith(`--${n}=`))?.slice(n.length+3)||f;
const output=resolve(option('output','artifacts/replay-tool'));mkdirSync(output,{recursive:true});
const root=resolve(option('root','../..'));
const file=resolve(option('file','verified-match.rpy'));
const {startReplayViewer}=await import(pathToFileURL(resolve(root,'tools/replay/viewer-server.mjs')).href);
const {server,url}=await startReplayViewer({root});
const {default:puppeteer}=await import(pathToFileURL(resolve(import.meta.dirname,'../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js')).href);
const report={passed:false,errors:[],checks:[]};
const save=()=>writeFileSync(resolve(output,'tool-report.json'),JSON.stringify(report,null,2)+'\n');
const browser=await puppeteer.launch({executablePath:process.env.TH19_BROWSER,headless:true,defaultViewport:{width:1100,height:950},args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
try{
 const page=await browser.newPage();page.setDefaultTimeout(180000);page.on('pageerror',e=>report.errors.push(e.message));
 await page.goto(url,{waitUntil:'domcontentloaded'});
 await page.waitForFunction(()=>document.querySelector('#status').textContent==='请选择录像');
 const picker=await page.$('#file');await picker.uploadFile(file);
 await page.waitForFunction(()=>!document.querySelector('#pause').disabled);
 await page.click('#pause');
 await page.waitForFunction(()=>document.querySelector('iframe').contentWindow.__th19Runtime.player.state().paused);
 await page.select('#rate','4');
 await page.click('#play');
 await page.waitForFunction(()=>document.querySelector('iframe').contentWindow.__th19Runtime.player.state().frame>=360);
 await page.click('#pause');
 await page.evaluate(()=>{const s=document.querySelector('#seek');s.value='1200';s.dispatchEvent(new Event('change'));});
 await page.waitForFunction(()=>document.querySelector('iframe').contentWindow.__th19Runtime.player.state().frame===1200);
 const original=JSON.parse(readFileSync(resolve(output,'spectator-report.json'),'utf8'));
 const hash=await page.evaluate(()=>document.querySelector('iframe').contentWindow.Module._th19_network_hash()>>>0);
 report.hash=hash;report.frames=1200;
 // Compare the stored replay's final verification hash, not a newly invented expectation.
 const {decodeTh19Replay,parseTh19StreamPacket}=await import('../web/sdl-runtime/th19-recording.mjs');
 const tape=decodeTh19Replay(new Uint8Array(readFileSync(file)));
 const row=parseTh19StreamPacket(tape.payload.subarray(1199*44,1200*44));assert.equal(hash,row.hash);assert.equal(original.passed,true);
 await page.screenshot({path:resolve(output,'03-tool-replay.png')});
 await page.click('#restart');await page.waitForFunction(()=>document.querySelector('iframe').contentWindow.__th19Runtime.player.state().frame<360);
 await page.click('#pause');
 await page.evaluate(()=>{const s=document.querySelector('#seek');s.value='1200';s.dispatchEvent(new Event('change'));});
 await page.waitForFunction(()=>document.querySelector('iframe').contentWindow.__th19Runtime.player.state().frame===1200);
 assert.equal(await page.evaluate(()=>document.querySelector('iframe').contentWindow.Module._th19_network_hash()>>>0),hash);
 report.checks.push('file picker, play, pause, 4x, end seek, restart and repeat end hash');
 // Exercise the authorized one-time old-recording migration in real IndexedDB.
 await page.evaluate(async()=>{
  const w=document.querySelector('iframe').contentWindow,c=w.Module,fs=c.FS;
  const {decodeTh19Replay,encodeTh19Replay}=await import(new URL('runtime/th19-recording.mjs',location.href));
  const r=decodeTh19Replay(new Uint8Array(fs.readFile('/savesth19/replay/th19_ud0000.rpy')));
  r.meta.build='0'.repeat(64);r.meta.config.build=r.meta.build;
  fs.writeFile('/savesth19/replay/th19_udold0.rpy',encodeTh19Replay(r.meta,r.payload));
  fs.writeFile('/savesth19/keep-cleanup-test.txt','keep');
  fs.unlink('/savesth19/th19-replay-cutover-20261106.txt');
  await new Promise((ok,no)=>fs.syncfs(false,e=>e?no(e):ok()));
 });
 await page.reload({waitUntil:'domcontentloaded'});await page.waitForFunction(()=>document.querySelector('#status').textContent==='请选择录像');
 const cleaned=await page.evaluate(()=>{const fs=document.querySelector('iframe').contentWindow.Module.FS;return {old:fs.analyzePath('/savesth19/replay/th19_udold0.rpy').exists,current:fs.analyzePath('/savesth19/replay/th19_ud0000.rpy').exists,kept:fs.readFile('/savesth19/keep-cleanup-test.txt',{encoding:'utf8'})};});
 assert.deepEqual(cleaned,{old:false,current:true,kept:'keep'});report.checks.push('real IndexedDB old replay deleted; current recording and unrelated save retained');
 assert.deepEqual(report.errors,[]);report.passed=true;save();console.log(JSON.stringify(report));
}catch(e){report.failure=e.stack;save();throw e;}finally{await browser.close();await new Promise(ok=>server.close(ok));}
