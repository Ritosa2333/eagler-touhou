// Same checkpoint / same binary: full interpreter vs guarded synchronous lane.
const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
process.env.NATIVE_SCHEDULER='1';process.env.NATIVE_CPU='0';
(async()=>{
 const browser=await chromium.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true}),reports=[];
 try{for(const [language,players]of [['cn',2],['jp',3]]){
  const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));
  await require('./native_scheduler_qa.cjs').install(page);
  await page.route('**/netplay/runtime.js',async route=>{const response=await route.fetch();await route.fulfill({response,body:await response.text()+'\nglobalThis.cpuQA={emulator:()=>emulator,data:()=>gameDataBase(),mailbox:()=>mailbox,patch:()=>patch};'});});
  await page.goto((process.env.BASE||'http://127.0.0.1:19870')+'/netplay/runtime.html');await page.waitForFunction(()=>window.th04Sync);
  await page.evaluate(async config=>{const r=th04Sync;r.configure(0,0,config.players);await r.load({...config,p1:0,p2:2,p3:1,difficulty:3,lives:6,bombs:2});r.start();window.qaFrame=0;for(let i=0;i<180;i++){r.step(Array(config.players).fill(0),{frame:qaFrame++});r.confirm(qaFrame);}}, {language,players});
  if(process.env.DENSE==='1')await page.evaluate(({players,stage})=>{
   const r=th04Sync,m=cpuQA.emulator().module,d=cpuQA.data(),at=cpuQA.mailbox(),v=new DataView(m.HEAPU8.buffer),guest=d-v.getUint16(at+20,true)*16,resident=guest+v.getUint16(d+0xba86,true)+v.getUint16(d+0xba88,true)*16;
   m.thDirtyMark?.(resident,256);m.thDirtyMark?.(d,65536);m.HEAPU8[resident+17]=stage-1;m.HEAPU8[resident+19]=48+stage-1;m.HEAPU8[d+0x5392]=2;
   const survive=()=>{m.thDirtyMark?.(d,65536);m.thDirtyMark?.(at,256);m.HEAPU8[d+0x4669]=0;m.HEAPU8[at+71]=0;if(players===3){const p=cpuQA.patch(),third=at-p.mailbox_cs_offset+p.p3_motion_cs_offset+27;m.thDirtyMark?.(third,1);m.HEAPU8[third]=0;}m.HEAPU8[d+0x53a0]=1;};
   let ready=false;for(let i=0;i<1350;i++){survive();r.step(Array(players).fill(0),{frame:qaFrame++});r.confirm(qaFrame);if(m.HEAPU8[d+0x5394]===stage-1&&m.HEAPU8[d+0x5392]===0&&r.snapshot().stageFrame>20){ready=true;break;}}if(!ready)throw Error('dense stage load');
   for(let i=0;i<1200;i++){survive();r.step(Array(players).fill(0),{frame:qaFrame++});r.confirm(qaFrame);}window.cpuSurvive=survive;
  },{players,stage:Number(process.env.STAGE||5)});
  const samples=[];
  for(let window=0;window<Number(process.env.WINDOWS||24);window++){
   const result=await page.evaluate(async({players,window})=>{
    const r=th04Sync,e=cpuQA.emulator(),m=e.module,api=m.nativeExports(),control=api.thCpuState(0),start=qaFrame;
    const hash=async bytes=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))).map(v=>v.toString(16).padStart(2,'0')).join('');
    const slice=(api,field,length)=>Array.from(m.HEAPU8.subarray(api(field),api(field)+length));
    const state=async()=>{
     const audio=[];for(const [frame,blocks]of e.outputs)for(const block of blocks)audio.push([frame,await hash(new Uint8Array(block.buffer,block.byteOffset,block.byteLength))]);
     return {guest:await hash(m.HEAPU8.subarray(api.thCoreState(0),api.thCoreState(0)+api.thCoreState(1))),regs:slice(api.thCoreState,2,api.thCoreState(3)),cpu:slice(api.thCoreState,4,api.thCoreState(5)),lazy:slice(api.thCpuState,1,api.thCpuState(2)),core:slice(api.thCpuState,3,api.thCpuState(4)),segments:slice(api.thCpuState,7,api.thCpuState(8)),prefix:slice(api.thCpuState,9,1),offset:slice(api.thCpuState,10,2),cycles:slice(api.thCpuState,5,4),instructions:slice(api.thCpuState,6,4),game:r.snapshot(),check:r.checksum(),now:m.virtualNow,deadline:m.targetNow,wakeAt:m.wakeAt,audio};
    };
    const set=enabled=>{m.thDirtyMark?.(control,16);m.HEAPU32[control>>>2]=enabled;};
    const input=i=>Array.from({length:players},(_,slot)=>32|((i+slot+window)%4<2?8:4)|((i+window)%5===0?16:0));
    set(0);r.capture(start);const expected=[];
    for(let i=0;i<12;i++){if(i>0)r.capture(start+i);window.cpuSurvive?.();r.step(input(i),{frame:start+i});expected.push(await state());}
    r.restore(start);set(1);const before=Array.from(m.HEAPU32.subarray(control>>>2,(control>>>2)+4)),checks=[];
    for(let i=0;i<12;i++){r.capture(start+i);window.cpuSurvive?.();r.step(input(i),{frame:start+i,replay:true});const actual=await state();checks.push({frame:start+i,equal:JSON.stringify(actual)===JSON.stringify(expected[i]),...(JSON.stringify(actual)!==JSON.stringify(expected[i])?{expected:expected[i],actual}:{})});}
    const after=Array.from(m.HEAPU32.subarray(control>>>2,(control>>>2)+4));r.confirm(start+12);qaFrame+=12;
    return {start,checks,before,after,game:r.snapshot()};
   },{players,window});
   samples.push(result);if(result.checks.some(x=>!x.equal))break;
  }
  reports.push({language,players,samples,errors});
  fs.writeFileSync(path.resolve(__dirname,'..',process.env.OUTPUT||'reports/native-cpu-parity-20261014.json'),JSON.stringify(reports,null,2)+'\n');
  assert.deepEqual(errors,[]);for(const sample of samples)for(const check of sample.checks)assert(check.equal,'native CPU differs at '+check.frame);
  console.log('PASS native CPU',language,players,'windows',samples.length,'fast instructions',samples.reduce((n,x)=>n+x.after[1]-x.before[1],0),'fallbacks',samples.reduce((n,x)=>n+x.after[2]-x.before[2],0));
  await page.evaluate(()=>th04Sync.stop());await page.close();
 }}finally{await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
