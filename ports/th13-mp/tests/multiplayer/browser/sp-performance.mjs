// Real single-player Launcher replay, ordinary RAF only. Evidence is append-only.
import {createRequire} from 'node:module';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';
import {mkdir, readFile, writeFile, appendFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {evaluateFrameWork} from './frame-work.mjs';
import {assertHardwareIdentity} from './hardware-identity.mjs';
import {boundedOperation} from './bounded-operation.mjs';
import assert from 'node:assert/strict';
const require=createRequire(import.meta.url);
const {chromium}=require(process.env.PLAYWRIGHT_MODULE || '/opt/shot/node_modules/playwright-core');
const origin=process.env.TH13_ORIGIN || 'http://127.0.0.1:18764';
const dir=resolve(process.env.TH13_SP_PERF_DIR || '/work/artifacts/sp-performance-'+Date.now());
const replay='/work/games/replays/th13_udN01.rpy';
const build=process.env.TH13_SP_BUILD || '/work/artifacts/work-observer-sp-20261112';
await mkdir(dir); // no overwrite of prior evidence
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const replayBytes=await readFile(replay);
const py=spawnSync('python3',['-c',`import sys,json,hashlib
from oracle.replayfmt import ReplayFile
r=ReplayFile();r.decode(open(sys.argv[1],'rb').read())
print(json.dumps([dict(number=s.number,seed=s.stamp,frames=s.frame_count,inputBytes=len(s.input),inputSha256=hashlib.sha256(s.input).hexdigest()) for s in r.stages]))`,replay],{cwd:'/work',encoding:'utf8'});
if(py.status!==0) throw Error('Replay decode failed: '+py.stderr);
const stages=JSON.parse(py.stdout);
const expectedFiles=await Promise.all(['th13-sdl.mjs','th13-sdl.wasm'].map(async name=>{const b=await readFile(resolve(build,name));return {name,bytes:b.length,sha256:hash(b)}}));
const report={started:new Date().toISOString(),origin,rate:4,replay:{path:replay,sha256:hash(replayBytes),bytes:replayBytes.length,stages},expectedFiles,delivery:[],errors:[],progress:[],result:'INCOMPLETE',scope:'Recorded replay only; not live human play. No independent original framebuffer comparison.',limitations:['Synchronous replay catalog import is timed separately; other shell JavaScript preparation is not included in C++ work scopes.']};
for(const name of ['sp-performance.mjs','frame-work.mjs','hardware-identity.mjs','bounded-operation.mjs']) { const bytes=await readFile(new URL(name,import.meta.url)); await writeFile(resolve(dir,name),bytes); (report.helperHashes ||= {})[name]=hash(bytes); }
await writeFile(resolve(dir,'report.json'),JSON.stringify(report,null,2));
const browser=await chromium.launch({channel:'chromium',headless:true,args:['--no-sandbox','--disable-dev-shm-usage','--ignore-gpu-blocklist','--disable-gpu-sandbox','--use-angle=gl-egl','--disable-software-rasterizer','--enable-gpu','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-backgrounding-occluded-windows','--disable-renderer-backgrounding']});
const context=await browser.newContext({viewport:{width:1280,height:900}});
await context.addInitScript(({base64,size})=>{
 localStorage.setItem('th13.replays',JSON.stringify([{name:'th13_01.rpy',size,ts:0}]));
 localStorage.setItem('th13.replay.th13_01.rpy',base64);
 localStorage.setItem('eagler-touhou-game-options-v1-th13',JSON.stringify({options:{renderScale:1,thpracEnabled:false}}));
 const work=globalThis.__frameWork={samples:[],capture:{first:null,last:null,count:0},overflow:0};
 let moduleValue;
 Object.defineProperty(globalThis,'Module',{configurable:true,get:()=>moduleValue,set:value=>{
   moduleValue=value;if(!value||typeof value!=='object')return;
   const previous=value.onGameWorkFrame;
   if(typeof value._th13_replay_import==='function'){const original=value._th13_replay_import;value._th13_replay_import=function(...args){const begin=performance.now();try{return original.apply(this,args)}finally{(work.importTimes ||= []).push({begin,end:performance.now(),bytes:args[1]})}};}
   work.observer=value.onGameWorkFrame=row=>{if(work.samples.length>=100000){work.overflow++;}else work.samples.push({...row});if(!work.capture.count)work.capture.first=row.sequence;work.capture.last=row.sequence;work.capture.count++;previous?.(row)};
 }});
},{base64:replayBytes.toString('base64'),size:replayBytes.length});
const page=await context.newPage();
const cpu=await context.newCDPSession(page);await cpu.send('Emulation.setCPUThrottlingRate',{rate:4});
const id=await browser.newBrowserCDPSession();report.browser={version:await id.send('Browser.getVersion'),gpu:(await id.send('SystemInfo.getInfo')).gpu};await id.detach();
page.on('pageerror',e=>report.errors.push({type:'pageerror',message:e.stack}));
page.on('requestfailed',r=>report.errors.push({type:'requestfailed',url:r.url(),error:r.failure()?.errorText}));
page.on('response',async r=>{if(r.status()>=400)report.errors.push({type:'http',url:r.url(),status:r.status()});if(/th13-sdl\.(mjs|wasm)(\?|$)/.test(r.url()))try{const b=await r.body();report.delivery.push({url:r.url(),bytes:b.length,sha256:hash(b)})}catch(e){report.errors.push({type:'delivery',message:String(e)})}});
const restoreLogs=[];page.on('console',m=>{const line=m.text();if(line.includes('R3-RESTORE'))restoreLogs.push({at:new Date().toISOString(),type:m.type(),line});else if(m.type()==='error')report.errors.push({type:'console',message:line})});
const all=[];let lastProgress=0;
async function drain(){
 const sample=await page.evaluate(()=>{const w=document.querySelector('#gameFrame')?.contentWindow,m=w?.Module,c=w?.__frameWork;if(!c||!m)return null;const read=n=>{if(!m[n])return null;const raw=m.UTF8ToString(m[n]());try{return raw?JSON.parse(raw):{readError:'empty',raw}}catch(e){return {readError:String(e),raw}}};const progress=read('_th13_debug_replay_progress'),pending=read('_th13_debug_pending');let firstStageCore=null;if(progress?.active&&progress.slot_index>=0&&progress.current_stage===progress.slot_number&&c.lastCoreSlot!==progress.slot_index){firstStageCore=read('_th13_debug_replay_core');c.lastCoreSlot=progress.slot_index}return {firstStageCore,importTimes:[...(c.importTimes||[])],rows:c.samples.slice(),capture:{...c.capture},overflow:c.overflow,attached:m.onGameWorkFrame===c.observer,progress,pending,frame:m._th13_frame(),visible:!w.document.hidden};});
 if(!sample)return null;
 if(sample.rows.length){await appendFile(resolve(dir,'work.ndjson'),sample.rows.map(x=>JSON.stringify(x)).join('\n')+'\n');const first=sample.rows[0].sequence,last=sample.rows.at(-1).sequence,count=sample.rows.length;await page.evaluate(({first,last,count})=>{const c=document.querySelector('#gameFrame')?.contentWindow?.__frameWork;if(!c || c.samples.length<count || c.samples[0].sequence!==first || c.samples[count-1].sequence!==last)throw Error('Work ACK mismatch after durable append');c.samples.splice(0,count)}, {first,last,count});all.push(...sample.rows);}
 const {rows,...snapshot}=sample;report.last=snapshot;report.progress.push({at:new Date().toISOString(),...snapshot});await appendFile(resolve(dir,'progress.ndjson'),JSON.stringify(report.progress.at(-1))+'\n');
 if(!sample.attached||sample.overflow)throw Error('Observer detached or overflowed');
 return snapshot;
}
let timer,draining=Promise.resolve(),failure=null,drainFailed=false;
const runDrain=()=>draining=draining.then(async()=>{if(drainFailed)return;try{await boundedOperation('SP evidence drain',()=>drain(),15000)}catch(e){drainFailed=true;failure ||= e.stack}});
async function waitFrames(n){const f=await page.evaluate(()=>document.querySelector('#gameFrame').contentWindow.Module._th13_frame());await page.waitForFunction(f=>document.querySelector('#gameFrame')?.contentWindow?.Module?._th13_frame()>=f,null===f?null:f+n,{timeout:120000});}
async function key(k){await page.keyboard.press(k,{delay:120});await waitFrames(8)}
try{
 await page.goto(origin+'/?test=th13&renderScale=1');await page.waitForFunction(()=>window.__eaglerBoot?.done===true,null,{timeout:120000});
 await page.locator('#firstUseNoticeClose').click({timeout:3000}).catch(()=>{});
 await page.locator('.book-game-link[data-game=th13]').click();await page.waitForFunction(()=>document.querySelector('#gameId')?.dataset.game==='th13');
 await page.locator('#launch').click();
 page.addLocatorHandler(page.locator('#decisionDialog[open]:not(.closing)'),async()=>page.locator('#decisionConfirm').click());
 timer=setInterval(runDrain,1000);
 await page.waitForFunction(()=>{const w=document.querySelector('#gameFrame')?.contentWindow;return w?.Module?._th13_frame?.()>390},null,{timeout:180000});
 const actualStorage=await page.evaluate(async()=>{const s=localStorage.getItem('th13.replay.th13_01.rpy');if(!s)return null;const bytes=Uint8Array.from(atob(s),c=>c.charCodeAt(0));const digest=await crypto.subtle.digest('SHA-256',bytes);return {bytes:bytes.length,sha256:[...new Uint8Array(digest)].map(x=>x.toString(16).padStart(2,'0')).join('')}});report.importedStorage=actualStorage;assert.equal(actualStorage?.sha256,hash(replayBytes),'Actual persisted replay bytes');
 const postStartId=await browser.newBrowserCDPSession();report.browser.postStartGpu=(await postStartId.send('SystemInfo.getInfo')).gpu;await postStartId.detach();
 const actualContext=await page.evaluate(()=>{const canvas=document.querySelector('#gameFrame').contentDocument.querySelector('canvas'),gl=canvas.getContext('webgl2'),ext=gl?.getExtension('WEBGL_debug_renderer_info');return gl&&{seat:0,version:gl.getParameter(gl.VERSION),renderer:ext&&gl.getParameter(ext.UNMASKED_RENDERER_WEBGL),vendor:ext&&gl.getParameter(ext.UNMASKED_VENDOR_WEBGL)}});report.actualHardware={gpu:report.browser.postStartGpu,seats:[0],contexts:[actualContext]};assertHardwareIdentity(report.actualHardware);await page.frameLocator('#gameFrame').locator('canvas').focus();
 // Main menu: Extra locked, so three down presses select Replay.
 await key('ArrowDown');await key('ArrowDown');await key('ArrowDown');await key('z');await waitFrames(60);
 await key('z');await waitFrames(40);await key('z');
 await page.waitForFunction(()=>{const w=document.querySelector('#gameFrame')?.contentWindow,m=w?.Module;return m?._th13_debug_replay_progress && JSON.parse(m.UTF8ToString(m._th13_debug_replay_progress())).active},null,{timeout:120000});
 console.log('Replay started',dir);
 const deadline=Date.now()+Number(process.env.TH13_SP_MAX_MS || 7200000);
 while(Date.now()<deadline&&!failure){await new Promise(r=>setTimeout(r,1000));await runDrain();const p=report.last?.progress;if(Date.now()-lastProgress>30000){console.log('Replay progress',JSON.stringify(p));lastProgress=Date.now()}if(p?.active&&p.slot_index===5&&p.slot_number===6&&p.overlay===3&&(p.scene_flags&0x4000)!==0&&p.environment_paused){report.clearTerminal=structuredClone(report.last);break;}}
 if(failure)throw Error(failure);
 if(!report.clearTerminal)throw Error('Stage6 clear overlay not observed before deadline');
 // PracticeEnd cursor 1 is title; cursor 4 is retry. Verify rather than guess.
 const pause=await page.evaluate(()=>{const m=document.querySelector('#gameFrame').contentWindow.Module;return JSON.parse(m.UTF8ToString(m._th13_debug_pause_state()))});
 if(pause.screen!==3||pause.phase!==6||pause.cursor!==1)throw Error('Unexpected clear menu; refusing to select retry: '+JSON.stringify(pause));
 report.clearMenu=pause;await key('z');
 await page.waitForFunction(()=>{const m=document.querySelector('#gameFrame')?.contentWindow?.Module;if(!m)return false;const p=JSON.parse(m.UTF8ToString(m._th13_debug_pending()));return p.has_title&&p.title_screen===11&&p.title_substate===2&&!p.replay_active},null,{timeout:120000});
 clearInterval(timer);await draining;if(drainFailed)throw Error(failure);
 report.frozenBoundary=await boundedOperation('SP final freeze',()=>page.evaluate(()=>{const w=document.querySelector('#gameFrame')?.contentWindow,m=w?.Module,c=w?.__frameWork;if(!m?._th13_loop_stop||!c||m.onGameWorkFrame!==c.observer)throw Error('Cannot freeze complete work observation');const p=JSON.parse(m.UTF8ToString(m._th13_debug_pending()));if(!p.has_title||p.title_screen!==11||p.title_substate!==2||p.replay_active)throw Error('Replay return state lost before freeze');m._th13_loop_stop();return {...c.capture,frozenAt:performance.now(),pending:p}}),15000);
 await runDrain();if(drainFailed)throw Error(failure);const boundary=report.frozenBoundary,capture=report.last?.capture;if(!capture||['first','last','count'].some(k=>boundary[k]!==capture[k]))throw Error('Frozen work boundary differs from persisted capture');report.result='OBSERVED_CLEAR_AND_RETURN';
}catch(e){failure=e.stack;report.errors.push({type:'run',message:failure});console.error(failure)}
finally{
 clearInterval(timer);await draining;if(!drainFailed&&report.result!=='OBSERVED_CLEAR_AND_RETURN'){try{report.frozenBoundary=await boundedOperation('SP failure freeze',()=>page.evaluate(()=>{const w=document.querySelector('#gameFrame')?.contentWindow,m=w?.Module,c=w?.__frameWork;if(!m?._th13_loop_stop||!c||m.onGameWorkFrame!==c.observer)throw Error('Cannot freeze complete work observation');m._th13_loop_stop();return {...c.capture,frozenAt:performance.now()}}),15000);await runDrain()}catch(e){report.errors.push({type:'final-freeze-or-drain',message:String(e)})}}
 const capture=report.last?.capture;
 report.evaluation=evaluateFrameWork(all,{expectedStages:[1,2,3,4,5,6],expectedPhases:['loading','menu','active'],expectedFirstSequence:1,requireZero:true,requireCatchup:true,requireRollback:false,capture});
 report.restoreLogs=restoreLogs;await writeFile(resolve(dir,'restore-logs.json'),JSON.stringify(restoreLogs,null,2)+'\n');
 for(const expected of expectedFiles){const got=report.delivery.filter(x=>x.url.includes(expected.name));if(got.length!==1||got[0].sha256!==expected.sha256)report.errors.push({type:'delivery-identity',expected,got});}
 const terminal=report.clearTerminal?.progress;const count=stages.reduce((n,s)=>n+s.frames,0);
 const observed=report.progress.map(x=>x.progress).filter(p=>p?.active&&p.slot_index>=0);const issues=[];let prior=null;const seen=new Set();for(const p of observed){const stage=stages[p.slot_index],offset=stages.slice(0,p.slot_index).reduce((n,s)=>n+s.frames,0);if(!stage||p.slot_number!==stage.number||p.slot_frames!==stage.frames||p.recorded_position!==offset+p.slot_frame)issues.push({type:'cursor-identity',progress:p});if(prior&&(p.slot_index<prior.slot_index||p.recorded_position<prior.recorded_position||p.slot_index>prior.slot_index+1))issues.push({type:'cursor-order',prior,current:p});seen.add(p.slot_index);prior=p;}if(stages.some((_,i)=>!seen.has(i)))issues.push({type:'missing-stage',seen:[...seen]});if(!terminal||terminal.slot_index!==5||terminal.overlay!==3||(terminal.scene_flags&0x4000)===0||!terminal.environment_paused||terminal.recorded_position>count)issues.push({type:'clear-terminal',actual:terminal});if(stages.some(s=>s.inputBytes!==6*s.frames))issues.push({type:'input-width'});report.qualification={passed:!issues.length,issues,seen:[...seen],expectedInputFrames:count,consumedAtClear:terminal?.recorded_position??null,remainingAtClear:terminal?count-terminal.recorded_position:null,firstStageCores:report.progress.filter(x=>x.firstStageCore).map(x=>({at:x.at,progress:x.progress,core:x.firstStageCore})),limitation:'Periodic cursor observations do not independently prove every input byte consumption or original-game equality; core snapshots are first observed in each stage, not frame zero. A clear overlay may precede replay EOF; remaining input is reported.'};
 report.ended=new Date().toISOString();report.complete=report.result==='OBSERVED_CLEAR_AND_RETURN'&&report.evaluation.passed&&report.qualification.passed&&!report.errors.length;
 await writeFile(resolve(dir,'report.json'),JSON.stringify(report,null,2)+'\n');
 await browser.close();
}
if(failure||!report.complete)process.exitCode=1;
