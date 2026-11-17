import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
const out=resolve(import.meta.dirname,'../../../archive/th19-aura-mobile-20261107');mkdirSync(out,{recursive:true});
const b=await puppeteer.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
try{const p=await b.newPage();await p.setViewport({width:844,height:390,isMobile:true,hasTouch:true,deviceScaleFactor:1});
await p.goto('http://127.0.0.1:8140/tests/harness.html?autolaunch=0');await p.waitForFunction(()=>window.__TH19_HARNESS?.configured,{timeout:180000});
await p.evaluate(async()=>{await __TH19_HARNESS.command('launch');window.c=__TH19_HARNESS.frame.contentWindow.Module;c._th19_loop_stop();});
const client=await p.createCDPSession();await client.send('Emulation.setCPUThrottlingRate',{rate:4});
const report=await p.evaluate(()=>{
 const result=[];for(const present of [0,1]){
 c._th19_network_end();c._th19_network_select_begin(1234,2,18,10);
 for(let i=0;i<60;i++)c._th19_network_step(i,0,0,0,0,0,0,0,0,1);
 const samples=[];for(let trial=0;trial<3;trial++){const t=performance.now();for(let i=0;i<60;i++)c._th19_network_step(60+trial*60+i,0,0,0,0,0,0,0,0,present);samples.push(performance.now()-t);}
 const q=c._th19_network_select_info()/4;result.push({present,samples,hash:c._th19_network_hash(),selection:Array.from(c.HEAP32.subarray(q,q+5))});
 }return result;});
writeFileSync(resolve(out,`perf-${process.argv[2]||'after'}.json`),JSON.stringify(report,null,2));console.log(JSON.stringify(report));
}finally{await b.close();}
