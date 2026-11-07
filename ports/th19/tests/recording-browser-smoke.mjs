import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {mkdirSync,writeFileSync} from 'node:fs';
const arg=(name,fallback)=>process.argv.find(x=>x.startsWith('--'+name+'='))?.slice(name.length+3)||fallback;
const output=resolve(arg('output','artifacts/recording'));mkdirSync(output,{recursive:true});
const report={passed:false,errors:[],checks:[]},save=()=>writeFileSync(resolve(output,'recording-report.json'),JSON.stringify(report,null,2)+'\n');save();
const {default:puppeteer}=await import(pathToFileURL(process.env.TH19_PUPPETEER_MODULE||resolve(import.meta.dirname,'../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js')).href);
const browser=await puppeteer.launch({executablePath:process.env.TH19_BROWSER,headless:true,timeout:60000,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
try{
 const page=await browser.newPage();page.on('pageerror',e=>report.errors.push(e.message));page.setDefaultTimeout(180000);
 await page.goto(new URL('tests/harness.html?autolaunch=0',arg('url','http://127.0.0.1:8140/')).href,{waitUntil:'domcontentloaded',timeout:120000});
 await page.waitForFunction(()=>window.__TH19_HARNESS?.configured);
 await page.evaluate(async()=>{await window.__TH19_HARNESS.command('launch');const c=window.__TH19_HARNESS.frame.contentWindow.Module;c._th19_loop_stop();c._th19_loop_pause(1);});
 // Use the actual menu path so every state transition is part of the tape.
 const expected=await page.evaluate(async()=>{
  const c=window.__TH19_HARNESS.frame.contentWindow.Module,read=(fn,n)=>Array.from(c.HEAP32.subarray(fn()/4,fn()/4+n));
  const press=(code,ticks=120)=>{c._th19_key(code,1);c._th19_test_ticks(1);c._th19_key(code,0);c._th19_test_ticks(ticks);};
  c._th19_test_ticks(600);await window.__TH19_HARNESS.command('sync');
  if((await window.__TH19_HARNESS.command('list')).files.some(f=>f.path.startsWith('replay/')))throw Error('menu-only session saved a recording');
  press(29);press(29);press(29);
  if(read(c._th19_title_status,8)[1]!==10)throw Error('expected dialogue before real gameplay');
  await window.__TH19_HARNESS.command('sync');
  if((await window.__TH19_HARNESS.command('list')).files.some(f=>f.path.startsWith('replay/')))throw Error('dialogue-only session saved a recording');
  for(let i=0;i<12&&read(c._th19_title_status,8)[1]===10;i++)c._th19_test_story_pump();
  c._th19_key(29,1);c._th19_key(79,1);c._th19_test_ticks(180);c._th19_keys_clear();
  return {frame:read(c._th19_record_info,8)[1],title:read(c._th19_title_status,8),hash:c._th19_network_hash()>>>0};
 });
 assert.equal(expected.title[1],11);report.singlePlayer=expected;report.checks.push('menu, selection and dialogue alone never save recordings');
 // Return through the real pause menu and choose Quit at the title.
 await page.evaluate(()=>{
  const c=window.__TH19_HARNESS.frame.contentWindow.Module;
  // Save the current session without injecting a state transition into the tape.
  c._th19_loop_stop();
 });
 // Runtime close uses the same persistence path as native Quit.
 await page.evaluate(()=>{const w=window.__TH19_HARNESS.frame.contentWindow;w.Module.onGameExit();});
 await page.waitForFunction(()=>window.__TH19_HARNESS.events.some(e=>e.event==='exit'));
 const files=await page.evaluate(async()=>(await window.__TH19_HARNESS.command('list')).files.filter(f=>f.path.endsWith('.rpy')));assert.equal(files.length,0);
 assert.equal(expected.frame,0,'single player must not allocate an input tape');
 report.checks.push('actual single-player gameplay and exit save no replay');
 await page.reload({waitUntil:'domcontentloaded'});await page.waitForFunction(()=>window.__TH19_HARNESS?.configured);
 assert.equal((await page.evaluate(async()=>(await window.__TH19_HARNESS.command('list')).files.filter(f=>f.path.endsWith('.rpy')))).length,0);
 report.checks.push('no single-player replay persisted after reload');
 assert.deepEqual(report.errors,[]);report.passed=true;save();console.log(JSON.stringify(report));
}catch(error){report.failure=error.stack;save();throw error;}finally{await browser.close();}
