import assert from 'node:assert/strict';
import {resolve} from 'node:path';import {pathToFileURL} from 'node:url';import {writeFileSync} from 'node:fs';
const {default:p}=await import(pathToFileURL(resolve('launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js')));
const browser=await p.launch({executablePath:process.env.TH19_BROWSER,headless:true,protocolTimeout:240000,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
const report={passed:false,errors:[],menus:[],returns:[]};
try{const page=await browser.newPage();page.on('pageerror',e=>report.errors.push(e.message));
async function load(vs){await page.goto('http://127.0.0.1:8140/tests/harness.html?autolaunch=0');await page.waitForFunction(()=>window.__TH19_HARNESS?.configured,{timeout:180000});await page.evaluate(async vs=>{await window.__TH19_HARNESS.command('launch');window.c=window.__TH19_HARNESS.frame.contentWindow.Module;c._th19_loop_stop();window.state=()=>{const p=c._th19_achievement_status()/4;return Array.from(c.HEAP32.subarray(p,p+2));};window.press=k=>{c._th19_key(k,1);c._th19_test_ticks(1);c._th19_key(k,0);c._th19_test_ticks(120);};c._th19_test_ticks(600);if(vs)press(81);press(29);},vs);}
await load(true);report.vsOptions=await page.evaluate(()=>{const first=state();press(82);const firstUp=state();press(80);const firstLeft=state();for(let i=0;i<4;i++)press(81);const fifth=state();press(81);const fifthDown=state();press(79);return {first,firstUp,firstLeft,fifth,fifthDown,fifthRight:state()};});
for(const key of ['first','firstUp','firstLeft'])assert.equal(report.vsOptions[key][1],0);for(const key of ['fifth','fifthDown','fifthRight'])assert.equal(report.vsOptions[key][1],4);
for(const vs of [false,true]){await load(vs);if(vs)await page.evaluate(()=>press(29));
 const bounds=await page.evaluate(()=>{for(let i=0;i<5;i++)press(82);const easy=state();press(82);press(80);const easyBoundary=state();for(let i=0;i<5;i++)press(81);const lunatic=state();press(81);press(79);return {easy,easyBoundary,lunatic,lunaticBoundary:state()};});
 assert.equal(bounds.easyBoundary[1],0);assert.equal(bounds.lunaticBoundary[1],3);report.menus.push({vs,...bounds});
 for(let rank=0;rank<4;rank++){const row=await page.evaluate(rank=>{for(let i=0;i<4;i++)press(82);for(let i=0;i<rank;i++)press(81);const before=state();press(29);const character=state();c._th19_key(27,1);c._th19_test_ticks(1);c._th19_key(27,0);let elapsed=1;while(state()[0]!==before[0]&&elapsed<40){c._th19_test_ticks(1);elapsed++;}const after=state();c._th19_test_ticks(120);return {before,character,after,elapsed};},rank);assert.deepEqual(row.after,row.before);assert(row.elapsed<=7);report.returns.push({vs,rank,...row});}
}
assert.deepEqual(report.errors,[]);report.passed=true;
}finally{writeFileSync('../th19-menu-report-20261106.json',JSON.stringify(report,null,2));await browser.close();}
console.log(JSON.stringify(report));
