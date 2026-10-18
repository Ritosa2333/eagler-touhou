// Desktop scheduling control, not a game/rollback acceptance result.
import {mkdirSync,existsSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createRequire} from 'node:module';
const root=resolve(import.meta.dirname,'../..');
const at=process.argv.indexOf('--output');if(at<0)throw Error('fresh --output required');
const out=resolve(process.argv[at+1]);if(existsSync(resolve(out,'receipt.json')))throw Error('existing receipt');
mkdirSync(out,{recursive:true});const tmp=resolve(out,'browser-temp');mkdirSync(tmp,{recursive:true});
process.env.TEMP=tmp;process.env.TMP=tmp;process.env.TMPDIR=tmp;
const require=createRequire(resolve(root,'../../eagler-touhou/th11/tools/diagnostics/package.json'));
const {chromium}=require('playwright');
const report={schema:'th11-cpu-throttle-control/1',started:new Date().toISOString(),completed:null,
  gameLoaded:false,rollbackAcceptance:false,status:'running',phases:[],errors:[],browsers:[]};
const save=()=>writeFileSync(resolve(out,'receipt.json'),JSON.stringify(report,null,2)+'\n');
save();const browsers=[],pages=[],sessions=[];
const watchdog=setTimeout(()=>{report.errors.push('90s watchdog');for(const b of browsers)void b.close();},90000);
try{
 for(let seat=0;seat<2;++seat){
  const browser=await chromium.launch({headless:true,executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',
   args:['--disable-background-timer-throttling','--disable-backgrounding-occluded-windows','--disable-renderer-backgrounding','--enable-gpu','--use-gl=angle','--use-angle=d3d11']});
  browsers.push(browser);const bcdp=await browser.newBrowserCDPSession();
  report.browsers.push({seat,version:browser.version(),processes:await bcdp.send('SystemInfo.getProcessInfo')});
  const context=await browser.newContext(),page=await context.newPage();pages.push(page);sessions.push(await context.newCDPSession(page));
 }
 for(const rate of [1,4,1]){
  for(const cdp of sessions)await cdp.send('Emulation.setCPUThrottlingRate',{rate});
  const samples=await Promise.all(pages.map(page=>page.evaluate(async()=>{
   function work(){let x=731;const start=performance.now();for(let i=0;i<2000000;++i)x=(Math.imul(x^i,1664525)+1013904223)|0;return {ms:performance.now()-start,x};}
   for(let i=0;i<4;++i)work();const cpu=[];for(let i=0;i<9;++i)cpu.push(work());
   const raf=[];for(let i=0;i<90;++i){const requested=performance.now();raf.push(await new Promise(r=>requestAnimationFrame(stamp=>r({requested,stamp,entry:performance.now()}))));}
   return {timeOrigin:performance.timeOrigin,cpu,raf};
  })));
  report.phases.push({rate,samples});save();
 }
 report.status='completed';
}catch(error){report.status='failed';report.errors.push(error.stack||String(error));process.exitCode=1;}
finally{await Promise.allSettled(browsers.map(b=>b.close()));clearTimeout(watchdog);report.completed=new Date().toISOString();save();
 console.log(JSON.stringify({status:report.status,out,phases:report.phases.map(p=>({rate:p.rate,seats:p.samples.map(s=>({cpu:s.cpu.map(x=>x.ms),rafMedian:s.raf.map(r=>r.entry-r.requested).sort((a,b)=>a-b)[45],rafMax:Math.max(...s.raf.map(r=>r.entry-r.requested))}))}))}));}
