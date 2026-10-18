// Adjacent frozen-build CPU diagnostics, alternating AB/BA for each 8-tick block.
// One checkpoint spans eight ticks here. This is not the live per-tick rollback
// workload; no RollbackSession network pump or network pacing method is invoked.
import assert from 'node:assert/strict';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {performance} from 'node:perf_hooks';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
import {startHostLoadProbe} from './host-load-probe.mjs';

const root=fileURLToPath(new URL('../../',import.meta.url));
const label=process.env.RUN_LABEL||'pc-paired';
assert.match(label,/^[\w-]+$/);
for(const name of ['PC_REFERENCE','PC_BUILD'])
 assert.ok(process.env[name]?.trim(),name+' must name an explicit frozen diagnostic build directory');
const cpuRate=Number(process.env.CPU_RATE??1);
assert.ok(cpuRate===1||cpuRate===4,'CPU_RATE must be 1 or 4');
const checkpointBlocksOption=process.env.PC_CHECKPOINT_BLOCKS??'200';
assert.match(checkpointBlocksOption,/^(0|80|200)$/,'PC_CHECKPOINT_BLOCKS must be 0, 80 or 200');
const checkpointBlocks=Number(checkpointBlocksOption);
const activatePage=process.env.PC_ACTIVATE_PAGE==='1';
const hardware=process.env.NATIVE_GPU==='1',hostLoad=process.env.HOST_LOAD==='1';
const definitions=[
 {id:'A',role:'reference',artifactDirectory:resolve(process.env.PC_REFERENCE)},
 {id:'B',role:'candidate',artifactDirectory:resolve(process.env.PC_BUILD)}
];
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const shaBytes=bytes=>createHash('sha256').update(bytes).digest('hex');
const shaFile=path=>shaBytes(readFileSync(path));
const identityFiles={
 harness:fileURLToPath(import.meta.url),
 presentationHarness:resolve(root,'tests/browser/presentation.html'),
 presentationServer:resolve(root,'scripts/presentation-server.mjs'),
 browserLauncher:fileURLToPath(new URL('../../../th10_web/scripts/native/browser-launch.mjs',import.meta.url)),
 hostLoadProbe:fileURLToPath(new URL('./host-load-probe.mjs',import.meta.url))
};
for(const build of definitions)for(const [key,name] of [
 ['wasm','th09-presentation.wasm'],['loader','th09-presentation.mjs'],['manifest','build.json']
])identityFiles[build.id+'.'+key]=resolve(build.artifactDirectory,name);
const identity=Object.fromEntries(Object.entries(identityFiles).map(([key,path])=>[key,shaFile(path)]));
const builds=Object.fromEntries(definitions.map(build=>[build.id,{
 ...build,wasm:identity[build.id+'.wasm'],loader:identity[build.id+'.loader'],manifest:identity[build.id+'.manifest']
}]));
const parameters={
 cpuRate,hardwareRequested:hardware,hostLoad,activatePage,warmupFrames:180,baseBlocks:40,checkpointBlocks,
 workload:checkpointBlocks?'single-checkpoint':'base-only-control',
 ticksPerBlock:8,firstBaseFrame:180,firstCheckpointFrame:checkpointBlocks?500:null,finalFrameExclusive:500+checkpointBlocks*8,
 order:'AB for even measured block indices; BA for odd indices; no concurrent game work',
 inputPolicy:'Unchanged pc-cost-browser deterministic input sequence, openProbe(0,1,2,3)',
 renderPolicy:'Semantic Draw every tick; submit the eighth tick; unchanged probe geometry policy',
 checkpointPolicy:checkpointBlocks?'One sparse checkpoint per eight-tick block, restore once, resimulate the same eight ticks':'No checkpoint capture, restore or resimulation; base-only control'
};
const scope='Same Chromium browser, two isolated frozen diagnostic-build pages; '+(checkpointBlocks?'180 warmup ticks, 40 base blocks without checkpoints, then '+checkpointBlocks+' single-checkpoint eight-tick restore/resimulation blocks per build, ending at frame '+parameters.finalFrameExclusive+'.':'base-only control with 180 warmup ticks and 40 eight-tick base blocks without checkpoints per build, ending at frame 500; no capture, restore or resimulation is exercised.')+(activatePage?' Each page is brought to the front before every warmup and measured block, outside timers.':' Explicit page activation before blocks is disabled.')+' Timed call durations exclude dense hashes, audits, PNG readback, cross-build comparison and page orchestration. This is not live multiplayer, a per-frame checkpoint benchmark, input latency, or physical presentation cadence; network pacing is not exercised.';
const measurementNotes=[
 'PC_ACTIVATE_PAGE=1 awaits page.bringToFront before each warmup and measured evaluate, before Node evaluation and in-page cost timers. Actual visibilityState and hasFocus are sampled at block entry and retained without filtering or a focus-based gate; activation does not guarantee symmetric scheduling or GPU conditions.',
 'PC_CHECKPOINT_BLOCKS=0 is a base-only control. Its gates require 40 base blocks and frame 500, with the unchanged cross-build network hash, 48 audit integers and PNG comparisons; checkpoint restore correctness is not exercised. The bounded 80-block mode ends at frame 1140 and covers frames 847 and 918 with unchanged restore gates. The default 200 retains the complete workload ending at frame 2100.',
 'Every measured block is retained. AB/BA order alternates without a performance threshold, retry, outlier filter or host-load-based exclusion.',
 'performance.now measures elapsed JS/Wasm call time, including synchronous driver waits; it is not process CPU seconds and does not isolate CPU execution from GPU synchronization.',
 'The P diagnostic/profile build retains development-only VM/journal overlap assertions. The final P release with TH09_DEVELOPMENT_HARNESS=0 omits them. These paired timings describe the loaded diagnostic identities only and cannot replace final release performance evidence.',
 'Each build executes a complete synchronous block before the adjacent build. Its forward and resimulation PNGs are read immediately after their timers, before the drawing buffer can be cleared at an asynchronous boundary.',
 'Oracle/readback work is outside timers but can affect the environment before the adjacent block. Alternating order and separate AB/BA summaries expose that asymmetry; pairing cannot cancel all host or GPU contention.',
 'Dense state hashes contain process/build addresses and are compared only within each build before restore versus after resimulation. Cross-build gates compare network hash, all 48 audit integers, and complete PNG strings exactly.',
 'PNG equality is checked on the complete strings in memory; reports retain SHA-256 of decoded PNG bytes and byte lengths. Failure reports retain the current raw pair including PNG strings.',
 'correction is capture + restore + resimEightTicks, matching pc-cost-browser; it excludes the original forwardEightTicks. totalMeasured includes all four timers.',
 'checkpointBytesAtCapture is the checkpoint API return immediately after Save, not peak first-write payload or retained page capacity.',
 'Times in timedIntervals use the owning page performance clock; add page timeOriginMs to align with host UTC samples. Node evaluation intervals include IPC, validation and readback and are not CPU cost.',
 'passed reports correctness and frozen identity only. Host probe validity is reported independently and does not silently remove game measurements.'
];
const distribution=values=>{
 const sorted=values.slice().sort((a,b)=>a-b),at=p=>sorted[Math.floor(sorted.length*p)]??null;
 return {count:values.length,total:values.reduce((a,b)=>a+b,0),min:sorted[0]??null,p50:at(.5),p95:at(.95),p99:at(.99),max:sorted.at(-1)??null};
};
const pngEvidence=png=>{
 assert.ok(typeof png==='string'&&png.startsWith('data:image/png;base64,'),'Missing PNG readback');
 const bytes=Buffer.from(png.slice(png.indexOf(',')+1),'base64');
 return {sha256:shaBytes(bytes),bytes:bytes.length};
};
const compactRow=row=>{
 const oracle=value=>{
  if(!value)return null;
  const {png,...rest}=value;return {...rest,png:pngEvidence(png)};
 };
 return {...row,forwardOracle:oracle(row.forwardOracle),resimOracle:oracle(row.resimOracle)};
};
const makeSummary=blocks=>{
 const metrics=[...new Set(blocks.flatMap(block=>Object.keys(block.pairedDeltaMs||{})))];
 const select=(list,read)=>Object.fromEntries(metrics.map(metric=>[metric,
  distribution(list.filter(block=>Object.hasOwn(block.pairedDeltaMs||{},metric)).map(block=>read(block,metric)))
 ]));
 return {
  builds:Object.fromEntries(['A','B'].map(id=>[id,select(blocks,(block,key)=>block.builds[id].costMs[key])])),
  pairedDeltaMs:select(blocks,(block,key)=>block.pairedDeltaMs[key]),
  pairedDeltaByOrder:Object.fromEntries(['AB','BA'].map(order=>[order,
   select(blocks.filter(block=>block.order.join('')===order),(block,key)=>block.pairedDeltaMs[key])
  ])),
  deltaDefinition:'B minus A milliseconds, negative means candidate costs less; unfiltered adjacent pairs'
 };
};

// Installed separately in each page. No free variables from the Node process.
function installPairedProbe(){
 const c=window.core,canvas=document.querySelector('canvas');
 for(const name of ['_th09_probe_rollback_step','_th09_probe_checkpoint','_th09_probe_restore',
  '_th09_probe_state_hash','_th09_probe_world','_th09_network_hash'])
  if(typeof c[name]!=='function')throw Error('Diagnostic build export missing: '+name);
 const gl=canvas.getContext('webgl2')||canvas.getContext('webgl');
 if(!gl)throw Error('WebGL context missing');
 const ext=gl.getExtension('WEBGL_debug_renderer_info');
 const renderer=ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER);
 let cursor=0;
 const step=(frame,render)=>{
  const left=(sessionStatus()[0]===1&&frame%8<4?1:0)|(frame<180?256:0);
  const right=(sessionStatus()[0]===1?1:0)|((Math.floor(frame/80)%2)?64:128);
  if(!c._th09_probe_rollback_step(left,right,+render))throw Error(probeError());
 };
 const observe=()=>{
  const denseStateHash=c._th09_probe_state_hash()>>>0;
  if(!denseStateHash)throw Error('Dense state hash failed: '+probeError());
  const networkHash=c._th09_network_hash()>>>0,at=c._th09_probe_world()/4;
  const audit=Array.from(c.HEAP32.subarray(at,at+48)),png=canvas.toDataURL('image/png');
  if(audit.length!==48||!png.startsWith('data:image/png;base64,'))throw Error('Invalid oracle output');
  return {denseStateHash,networkHash,audit,png};
 };
 const sameAudit=(a,b)=>a.length===48&&b.length===48&&a.every((value,index)=>value===b[index]);
 const probe=window.pcPairedProbe={
  lastBlock:null,
  metadata:{renderer,timeOriginMs:performance.timeOrigin,initialMemoryBytes:c.HEAPU8.length,
   initialPageCondition:{visibilityState:document.visibilityState,hasFocus:document.hasFocus()}},
  warm(firstFrame,count){
   if(cursor!==firstFrame||count<1||count>8||firstFrame+count>180)throw Error('Invalid warmup frontier');
   for(let n=0;n<count;++n)step(firstFrame+n,(firstFrame+n)%8===7);
   cursor+=count;return cursor;
  },
  runBlock({index,kind,firstFrame}){
   if(cursor!==firstFrame||(kind!=='base'&&kind!=='checkpoint'))throw Error('Invalid paired block frontier');
   const row=probe.lastBlock={index,kind,firstFrame,endFrameExclusive:firstFrame+8,
    startedAtMs:performance.now(),endedAtMs:null,costMs:{},timedIntervals:{},
    pageCondition:{visibilityState:document.visibilityState,hasFocus:document.hasFocus()},
    checkpointBytesAtCapture:null,forwardOracle:null,resimOracle:null,restoreChecks:null};
   const timed=(name,fn)=>{
    const start=performance.now(),value=fn(),end=performance.now();
    row.costMs[name]=end-start;row.timedIntervals[name]={startedAtMs:start,endedAtMs:end};
    return value;
   };
   const eightTicks=()=>{for(let n=0;n<8;++n)step(firstFrame+n,n===7);};
   if(kind==='base'){
    timed('baseEightTicks',eightTicks);
    row.forwardOracle=observe();
   }else{
    row.checkpointBytesAtCapture=timed('capture',()=>c._th09_probe_checkpoint(0));
    if(!row.checkpointBytesAtCapture)throw Error('Checkpoint failed: '+probeError());
    timed('forwardEightTicks',eightTicks);
    row.forwardOracle=observe();
    const restored=timed('restore',()=>c._th09_probe_restore());
    if(!restored)throw Error('Restore failed: '+probeError());
    timed('resimEightTicks',eightTicks);
    row.resimOracle=observe();
    const before=row.forwardOracle,after=row.resimOracle;
    row.restoreChecks={denseStateHash:before.denseStateHash===after.denseStateHash,
     png:before.png===after.png,networkHash:before.networkHash===after.networkHash,audit:sameAudit(before.audit,after.audit)};
    row.costMs.correction=row.costMs.capture+row.costMs.restore+row.costMs.resimEightTicks;
    row.costMs.totalMeasured=row.costMs.correction+row.costMs.forwardEightTicks;
   }
   cursor+=8;row.endedAtMs=performance.now();row.memoryBytes=c.HEAPU8.length;
   return row;
  },
  status(){return {cursor,memoryBytes:c.HEAPU8.length,testErrors:window.testErrors?.slice()||[]};}
 };
 return probe.metadata;
}

const hosts=[],pages={},pageMetadata={},errors=[],blocks=[],warmupBlocks=[];
let browser=null,context=null,hostProbe=null,hostLoadSnapshot=null,pendingPair=null;
const startedAtUtcMs=Date.now();
try{
 hostProbe=hostLoad?startHostLoadProbe():null;hostProbe?.mark('paired-setup-start');
 for(const build of definitions)hosts.push(await presentationServer(0,{artifactDirectory:build.artifactDirectory}));
 browser=await launchBrowser({args:[
  ...(hardware?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-unsafe-swiftshader']),
  '--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling',
  '--disable-renderer-backgrounding','--disable-backgrounding-occluded-windows'
 ]});
 context=await browser.newContext();
 for(let i=0;i<definitions.length;++i){
  const id=definitions[i].id,page=await context.newPage();pages[id]=page;
  page.on('pageerror',error=>errors.push({build:id,error:error.stack}));
  await page.goto(hosts[i].url);await page.evaluate(()=>openProbe(0,1,2,3));
  const cdp=await context.newCDPSession(page);
  await cdp.send('Emulation.setCPUThrottlingRate',{rate:cpuRate});
  pageMetadata[id]=await page.evaluate(installPairedProbe);
  pageMetadata[id].activatePageBeforeBlocks=activatePage;
  if(hardware)assert.doesNotMatch(pageMetadata[id].renderer,/swiftshader|llvmpipe|microsoft basic render/i,id+' hardware lane fell back to software');
  hostProbe?.mark('paired-page-'+id+'-ready');
 }
 hostProbe?.mark('paired-warmup-start');
 for(let firstFrame=0,index=0;firstFrame<180;firstFrame+=8,++index){
  const count=Math.min(8,180-firstFrame),order=index%2?['B','A']:['A','B'];
  const row={index,firstFrame,count,order,startedAtUtcMs:Date.now(),endedAtUtcMs:null,frontiers:{},pageConditions:{}};warmupBlocks.push(row);
  for(const id of order){
   if(activatePage)await pages[id].bringToFront();
   const result=await pages[id].evaluate(({firstFrame,count})=>{
    const pageCondition={visibilityState:document.visibilityState,hasFocus:document.hasFocus()};
    return {frontier:window.pcPairedProbe.warm(firstFrame,count),pageCondition};
   },{firstFrame,count});
   row.frontiers[id]=result.frontier;row.pageConditions[id]=result.pageCondition;
  }
  row.endedAtUtcMs=Date.now();
 }
 hostProbe?.mark('paired-base-start');
 for(let index=0;index<40+checkpointBlocks;++index){
  const kind=index<40?'base':'checkpoint',firstFrame=180+index*8,order=index%2?['B','A']:['A','B'];
  if(index===40)hostProbe?.mark('paired-checkpoint-start');
  const block={index,kind,firstFrame,endFrameExclusive:firstFrame+8,order,startedAtUtcMs:Date.now(),
   endedAtUtcMs:null,builds:{},evaluationIntervals:{},pairedChecks:null,pairedDeltaMs:null};
  blocks.push(block);pendingPair={index,kind,firstFrame,order,raw:{}};
  for(const id of order){
   if(activatePage)await pages[id].bringToFront();
   const began=performance.now(),beganUtcMs=Date.now();
   const raw=await pages[id].evaluate(request=>window.pcPairedProbe.runBlock(request),{index,kind,firstFrame});
   const ended=performance.now();
   block.evaluationIntervals[id]={startedAtUtcMs:beganUtcMs,endedAtUtcMs:Date.now(),elapsedMs:ended-began};
   pendingPair.raw[id]=raw;
  }
  const a=pendingPair.raw.A,b=pendingPair.raw.B;
  // Hash/report compaction waits until both adjacent executions have finished.
  block.builds={A:compactRow(a),B:compactRow(b)};
  const cross=(left,right)=>({networkHash:left.networkHash===right.networkHash,
   audit:left.audit.length===48&&right.audit.length===48&&left.audit.every((value,i)=>value===right.audit[i]),
   png:left.png===right.png});
  block.pairedChecks={forward:cross(a.forwardOracle,b.forwardOracle),
   resim:kind==='checkpoint'?cross(a.resimOracle,b.resimOracle):null,
   restored:kind==='checkpoint'?{A:a.restoreChecks,B:b.restoreChecks}:null};
  block.pairedDeltaMs=Object.fromEntries(Object.keys(a.costMs).map(key=>[key,b.costMs[key]-a.costMs[key]]));
  block.endedAtUtcMs=Date.now();
  for(const [stage,checks] of [['forward',block.pairedChecks.forward],['resim',block.pairedChecks.resim]])
   if(checks)for(const [name,equal] of Object.entries(checks))assert.ok(equal,'Cross-build '+stage+' '+name+' differs at block '+index+' frame '+firstFrame);
  if(block.pairedChecks.restored)for(const [id,checks] of Object.entries(block.pairedChecks.restored))
   for(const [name,equal] of Object.entries(checks))assert.ok(equal,id+' restore '+name+' differs at block '+index+' frame '+firstFrame);
  assert.equal(errors.length,0,JSON.stringify(errors));pendingPair=null;
 }
 hostProbe?.mark('paired-workload-complete');
 const finalStatus={};
 for(const id of ['A','B']){
  finalStatus[id]=await pages[id].evaluate(()=>window.pcPairedProbe.status());
  assert.equal(finalStatus[id].cursor,parameters.finalFrameExclusive,id+' incomplete '+parameters.workload+' workload');
 }
 const finalIdentity=Object.fromEntries(Object.entries(identityFiles).map(([key,path])=>[key,shaFile(path)]));
 assert.deepEqual(finalIdentity,identity,'Measured build or harness files changed during run');
 assert.equal(blocks.filter(block=>block.kind==='base').length,40);
 assert.equal(blocks.filter(block=>block.kind==='checkpoint').length,checkpointBlocks);
 hostProbe?.mark('paired-success-report-ready');hostLoadSnapshot=await hostProbe?.stop()??null;
 const summary=makeSummary(blocks),report={passed:true,correctnessPassed:true,performanceGate:null,scope,measurementNotes,
  startedAtUtcMs,endedAtUtcMs:Date.now(),parameters,identity,finalIdentity,builds,browser:browser.version(),
  pageMetadata,warmupBlocks,blocks,summary,finalStatus,errors,hostLoad:hostLoadSnapshot};
 writeFileSync(resolve(out,label+'-report.json'),JSON.stringify(report,null,2));
 console.log(JSON.stringify({passed:true,scope,parameters,builds,browser:report.browser,pageMetadata,summary,hostLoadValid:hostLoadSnapshot?.valid??null}));
}catch(error){
 hostProbe?.mark('paired-failure-observed');hostLoadSnapshot=await hostProbe?.stop()??null;
 const partial={};
 for(const [id,page] of Object.entries(pages)){
  try{partial[id]=await page.evaluate(()=>({status:window.pcPairedProbe?.status(),lastBlock:window.pcPairedProbe?.lastBlock}));}
  catch(reason){partial[id]={error:String(reason)};}
 }
 writeFileSync(resolve(out,label+'-failure.json'),JSON.stringify({passed:false,error:error.stack,scope,measurementNotes,
  startedAtUtcMs,endedAtUtcMs:Date.now(),parameters,identity,builds,browser:browser?.version()??null,
  pageMetadata,warmupBlocks,blocks,pendingPair,partial,errors,hostLoad:hostLoadSnapshot},null,2));
 throw error;
}finally{
 await hostProbe?.stop();
 if(browser)await browser.close();
 for(const {server,netplay} of hosts){netplay.close();await new Promise(done=>server.close(done));}
}
