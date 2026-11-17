import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {writeFileSync} from 'node:fs';
const {default:puppeteer}=await import(pathToFileURL(resolve('launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js')).href);
const browser=await puppeteer.launch({executablePath:process.env.TH19_BROWSER,headless:true,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
try {
 const page=await browser.newPage();const errors=[];page.on('pageerror',e=>errors.push(e.message));
 await page.goto(`${process.env.TH19_TEST_URL||'http://127.0.0.1:8140/'}tests/harness.html?autolaunch=0`);
 await page.waitForFunction(()=>window.__TH19_HARNESS?.configured,{timeout:180000});
 await page.evaluate(()=>window.__TH19_HARNESS.command('launch'));
 const result=await page.evaluate(()=>{
  const c=window.__TH19_HARNESS.frame.contentWindow.__th19Runtime.core;c._th19_loop_stop();c._th19_loop_pause(1);
  const read=(action=0)=>{const p=c._th19_test_gameplay_parity(action)/4;return Array.from(c.HEAP32.subarray(p,p+20));};
  const ranks=[];for(let d=0;d<4;d++){c._th19_network_begin(19019,d,0,1);ranks.push(read().slice(0,4));c._th19_network_end();}
  c._th19_network_begin(19019,1,15,0);c._th19_test_ticks(400);read(1);read(2);
  const visuals=[];for(let i=0;i<24;i++){c._th19_test_ticks(1);visuals.push(read().slice(4,6));}
  const checkpoint=c._th19_checkpoint_save();if(!checkpoint)throw Error("checkpoint unavailable");c._th19_test_ticks(8);const forward=read();c._th19_checkpoint_load(checkpoint);c._th19_test_ticks(8);const replayed=read();c._th19_checkpoint_release(checkpoint);
  read(1);const before=read();read(4);c._th19_test_ticks(1);const extra=read();read(4);c._th19_test_ticks(65);const boss=read();
  const traps=[];for(const ticks of [75,125]) {
   c._th19_network_end();c._th19_test_character(15);c._th19_test_ticks(400);read(1);c._th19_key(27,1);c._th19_test_ticks(ticks);c._th19_key(27,0);c._th19_test_ticks(1);
   const before=read(3);c._th19_test_ticks(1);traps.push({ticks,before,after:read()});
  }
  return {ranks,visuals,forward,replayed,before,extra,boss,traps};
 });
 writeFileSync(process.env.TH19_REPORT||'../th19-gameplay-browser.json',JSON.stringify({result,errors},null,2));
 assert.deepEqual(errors,[]);assert.deepEqual(result.ranks,[[0,0,0,0],[0,0,0,0],[2,2,2,2],[4,4,4,4]]);
 assert(new Set(result.visuals.map(x=>x[0])).size>1,'energy warning must animate');
 assert(new Set(result.visuals.map(x=>x[1])).size>1,'last heart must animate');
 assert(result.visuals.every(x=>(x[1]>>>24)===255),'last heart must remain opaque throughout its animation');
 assert.deepEqual(result.forward,result.replayed,'checkpoint must restore visual and gameplay state');
 assert.equal(result.extra[6],result.before[6]+1);assert.equal(result.boss[7],result.before[7]+1);
 assert.equal(result.extra[8],result.before[8]);assert.equal(result.extra[9],result.before[9]);
 for(const {before,after} of result.traps){assert(before[10]>0,'real charge must produce traps');assert(after[16]<100000,'trap must damage enemy');assert.equal(after[10],before[10],'collision must not consume trap');}
 console.log('Gameplay parity browser assertions PASS');
} finally {await browser.close();}
