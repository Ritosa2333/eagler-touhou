import {setPriority,constants} from 'node:os';
import puppeteer from '../../../eagler-touhou/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {readFileSync,writeFileSync,mkdirSync,createWriteStream} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
const arg=n=>process.argv[process.argv.indexOf(n)+1];
if(!process.argv.includes('--output')||!process.argv.includes('--replay')||!process.argv.includes('--dat'))throw Error('Required: --replay --dat --output');
const output=resolve(arg('--output')),replay=resolve(arg('--replay'));
const stage=process.argv.includes('--stage')?Number(arg('--stage')):6;
const max=process.argv.includes('--max-ticks')?Number(arg('--max-ticks')):3000;
const rate=process.argv.includes('--rate')?Number(arg('--rate')):60;
const chargeOffset=process.argv.includes('--charge-offset')?Number(arg('--charge-offset')):null;
const chargeStage=process.argv.includes('--charge-stage')?Number(arg('--charge-stage')):2;
const shotOffset=process.argv.includes('--shot-offset')?Number(arg('--shot-offset')):null;
if(shotOffset!==null&&(!Number.isInteger(shotOffset)||Math.abs(shotOffset)>300))throw Error('Invalid shot offset');
if(chargeOffset!==null&&(!Number.isInteger(chargeOffset)||Math.abs(chargeOffset)>300||!Number.isInteger(chargeStage)||chargeStage<1||chargeStage>7))throw Error('Invalid charge diagnostic');
if(!Number.isFinite(rate)||rate<1||rate>240)throw Error('--rate must be between 1 and 240 ticks/second');
if(!Number.isInteger(stage)||stage<1||stage>7||!Number.isInteger(max)||max<1)throw Error('Invalid --stage or --max-ticks');
if(!output.endsWith('.json'))throw Error('--output must end in .json');
const dat=resolve(arg('--dat'));
const sha=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
mkdirSync(dirname(output),{recursive:true});
const rows=output.replace(/\.json$/,'.rows.jsonl'),sink=createWriteStream(rows);
setPriority(process.pid,constants.priority.PRIORITY_BELOW_NORMAL);
const browser=await puppeteer.launch({executablePath:process.env.TH20_CHROME??'C:/Users/w3051/AppData/Local/ms-playwright/chromium-1243/chrome-win64/chrome.exe',headless:true,args:['--use-gl=angle','--use-angle=swiftshader','--enable-webgl','--no-sandbox']});
setPriority(browser.process().pid,constants.priority.PRIORITY_BELOW_NORMAL);
let ticks=0,complete=false,reason='error',loadedWasmHash=null;const errors=[],assetReads=[];
try{
 const page=await browser.newPage();await page.setViewport({width:640,height:480});
 page.on('response',response=>{if(new URL(response.url()).pathname.endsWith('/th20-sdl.wasm'))assetReads.push(response.buffer().then(bytes=>{loadedWasmHash=createHash('sha256').update(bytes).digest('hex');}).catch(e=>errors.push('WASM identity: '+e)));});
 page.on('pageerror',e=>errors.push(String(e.stack||e)));
 page.on('console',m=>{if(m.text().startsWith('MENU'))console.log(m.text());});
 await page.goto(process.argv.includes('--url')?arg('--url'):'http://127.0.0.1:8791/replay-probe.html',{waitUntil:'domcontentloaded'});
 await page.waitForFunction(()=>window.H?.boot,{timeout:120000});
 await page.evaluate(async ({bytes,chargeOffset,chargeStage,shotOffset})=>{
   if(!await H.boot())throw Error('Boot failed');
   Module.FS.writeFile('/savesth20/jp/replay/th20_02.rpy',new Uint8Array(bytes));
   if(!H.core.replay_verifier_trace)throw Error('Build lacks replay_verifier_trace');
   if(chargeOffset!==null)H.core.replay_verifier_charge_offset(chargeStage,chargeOffset);
   if(shotOffset!==null)H.core.replay_verifier_shot_offset(chargeStage,shotOffset);
 },{bytes:Array.from(readFileSync(replay)),chargeOffset,chargeStage,shotOffset});
 const loadedDatHash=await page.evaluate(async()=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',Module.FS.readFile('/game/th20.dat')))).map(x=>x.toString(16).padStart(2,'0')).join(''));
 if(loadedDatHash!==sha(dat))throw Error('Served th20.dat differs from --dat');
 const setup=await page.evaluate(({stage,diagnoseItems,diagnoseFiring})=>{
   const s=window.captureState={rows:[],last:null,started:false,done:false,reason:'bounded-prefix',steps:0};
   const capture=()=>{
     const w=Array.from(new Uint32Array(H.core.memory.buffer,H.core.replay_verifier_trace(),36));
     if(H.core.replay_verifier_finished()){s.done=true;s.reason='replay-finished';}
     if(!w[0]){if(s.started){s.done=true;s.reason='replay-finished';}return;}
     const id=w[1]+':'+w[2];if(id===s.last)return;
     if(s.last){const [a,b]=s.last.split(':').map(Number);if(a===w[1]&&w[2]!==b+1)throw Error('Missing candidate tick '+s.last+' -> '+id);}
     const sample={words:w};
     if(diagnoseFiring&&w[2]<=110){const ptr=H.core.replay_verifier_firing(),n=new Uint32Array(H.core.memory.buffer,ptr,1)[0];sample.firing=Array.from(new Uint32Array(H.core.memory.buffer,ptr+4,n));}
     if(w[2]===1&&H.core.replay_verifier_activation)sample.activation=Array.from(new Uint32Array(H.core.memory.buffer,H.core.replay_verifier_activation(),12));
     if(diagnoseItems&&w[2]>=3600&&w[2]<=4090){const ptr=H.core.replay_verifier_items(),count=new Uint32Array(H.core.memory.buffer,ptr,1)[0],rows=new Uint32Array(H.core.memory.buffer,ptr+4,count*13);sample.items=Array.from({length:count},(_,i)=>Array.from(rows.slice(i*13,(i+1)*13)));}
     s.started=true;s.last=id;s.rows.push(sample);
   };
   window.captureStep=n=>{for(let i=0;i<n&&!s.done;i++){const result=H.core.sdl_loop_tick(1/60,16);if(result<0)throw Error('Game tick failed: '+result);s.steps++;capture();}};
   const press=(key,gap=30)=>{H.key(key,true);captureStep(3);H.key(key,false);captureStep(gap);console.log('MENU '+key+' '+JSON.stringify(H.status()));};
   captureStep(400);console.log('MENU initial '+JSON.stringify(H.status()));for(let i=0;i<3;i++)press('ArrowDown');press('KeyZ',90);press('ArrowDown');press('KeyZ',90);
   for(let i=0;i<stage-1;i++)press('ArrowDown');press('KeyZ');
   for(let i=0;i<600&&!s.started;i++)captureStep(1);
   if(!s.started)throw Error('Real replay did not start '+JSON.stringify(H.status()));
   const rows=s.rows;s.rows=[];return rows;
 },{stage,diagnoseItems:process.argv.includes('--diagnose-items'),diagnoseFiring:process.argv.includes('--diagnose-firing')});
 for(const r of setup){sink.write(JSON.stringify(r)+'\n');ticks++;}
 while(ticks<max&&!complete){
   const batchStarted=Date.now();
   const batch=await page.evaluate(n=>{captureState.rows=[];captureStep(n);return {rows:captureState.rows,done:captureState.done,reason:captureState.reason};},Math.min(300,max-ticks));
   for(const r of batch.rows){sink.write(JSON.stringify(r)+'\n');ticks++;}
   complete=batch.done&&batch.reason==='replay-finished';reason=batch.reason;
   console.log(JSON.stringify({ticks,last:batch.rows.at(-1)?.words.slice(1,3),complete}));
   if(batch.done)break;
   if(!batch.rows.length&&!complete)throw Error('Replay stopped consuming inputs');
   // Wall-clock pacing only; every simulated tick still receives 1/60.
   if(!complete)await new Promise(r=>setTimeout(r,Math.max(0,batch.rows.length*1000/rate-(Date.now()-batchStarted))));
 }
}catch(error){
 errors.push(String(error.stack??error));reason='error';throw error;
}finally{
 await Promise.all(assetReads);
 if(!loadedWasmHash)errors.push('Missing loaded WASM identity');
 await browser.close();sink.end();await new Promise((r,j)=>{sink.on('finish',r);sink.on('error',j);});
 let recordedScoreAudit=null;
 try{
   recordedScoreAudit=JSON.parse(execFileSync(process.env.TH20_PYTHON??'python',[resolve(import.meta.dirname,'audit-recorded-score.py'),'--replay',replay,'--rows',rows],{encoding:'utf8'}));
 }catch(error){errors.push('Recorded score audit failed: '+error.message);}
 writeFileSync(output,JSON.stringify({schema:'th20/replay-capture/v1',provider:'sdl3-fixed-tick',complete,reason,ticks,rows,wallClockRate:rate,diagnosticOverrides:chargeOffset===null&&shotOffset===null?null:{chargeStage,chargeOffset,shotOffset},replaySha256:sha(replay),resourceSha256:sha(dat),executableSha256:'a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897',candidateWasmSha256:loadedWasmHash,recordedScoreAudit,errors},null,2));
}
