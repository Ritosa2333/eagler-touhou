import assert from 'node:assert/strict';import {resolve} from 'node:path';import {pathToFileURL} from 'node:url';import {mkdirSync,writeFileSync} from 'node:fs';
const {default:p}=await import(pathToFileURL(resolve('launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js')).href);
const out=resolve('../../outputs/th19-hit-effects-20261106');mkdirSync(out,{recursive:true});const report={passed:false,characters:[],errors:[]};
const b=await p.launch({executablePath:process.env.TH19_BROWSER,headless:true,protocolTimeout:240000,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
try{const page=await b.newPage();await page.setViewport({width:1280,height:960});page.on('pageerror',e=>report.errors.push(e.message));await page.goto('http://127.0.0.1:8140/tests/harness.html?autolaunch=0');await page.waitForFunction(()=>window.__TH19_HARNESS?.configured,{timeout:180000});await page.evaluate(async()=>{await window.__TH19_HARNESS.command('launch');window.c=window.__TH19_HARNESS.frame.contentWindow.Module;c._th19_loop_stop();});
for(let ch=0;ch<19;ch++){
 const row=await page.evaluate(ch=>{c._th19_test_character(ch);c._th19_test_ticks(400);c._th19_test_shot_impact(1);c._th19_key(29,1);const read=()=>{let p=c._th19_test_shot_impact(0)/4;return Array.from(c.HEAP32.subarray(p,p+16));};let peak=read(),maxEnding=0,maxEffects=0,mask=0;for(let i=0;i<80;i++){c._th19_test_ticks(1);let r=read();maxEnding=Math.max(maxEnding,r[2]);maxEffects=Math.max(maxEffects,r[9]);mask|=r[6];if(r[3])throw Error('ending shot still damages');if(r[2]+r[9]>=peak[2]+peak[9])peak=r;}return {ch,peak,maxEnding,maxEffects,mask,state:read()};},ch);
 assert(row.state[0]<100000,`character ${ch} collision`);assert(row.maxEnding>0,`character ${ch} impact animation`);report.characters.push(row);
 if([0,1,2,6,14,16].includes(ch))await page.screenshot({path:resolve(out,`character-${ch}-normal.png`)});
 const tail=await page.evaluate(()=>{c._th19_key(29,0);c._th19_test_shot_impact(2);c._th19_test_ticks(180);let p=c._th19_test_shot_impact(0)/4;return Array.from(c.HEAP32.subarray(p,p+16));});assert.equal(tail[2],0,`character ${ch} retires`);row.tail=tail;console.log(JSON.stringify(row));
}
assert.deepEqual(report.errors,[]);report.passed=true;
}finally{writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2));await b.close();}
