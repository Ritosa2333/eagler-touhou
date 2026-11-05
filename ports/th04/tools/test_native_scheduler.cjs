// Replay the same captured machine with one-ms host resumes vs native batching.
// Both use the same binary, input boundaries, and full rollback implementation.
const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
process.env.NATIVE_SCHEDULER='1';process.env.NATIVE_BATCH='0';
(async()=>{
 const browser=await chromium.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true}),reports=[];
 try{for(const [language,players]of [['cn',2],['jp',3]]){
  const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));
  await require('./native_scheduler_qa.cjs').install(page);
  await page.route('**/netplay/runtime.js',async route=>{const response=await route.fetch();await route.fulfill({response,body:await response.text()+'\nglobalThis.schedulerQA={emulator:()=>emulator};'});});
  await page.goto((process.env.BASE||'http://127.0.0.1:19870')+'/netplay/runtime.html');await page.waitForFunction(()=>window.th04Sync);
  await page.evaluate(async config=>{const r=th04Sync;r.configure(0,0,config.players);await r.load({...config,p1:0,p2:2,p3:1,difficulty:3,lives:6,bombs:2});r.start();window.qaFrame=0;for(let i=0;i<180;i++){r.step(Array(config.players).fill(0),{frame:qaFrame++});r.confirm(qaFrame);}}, {language,players});
  const samples=[];
  for(let window=0;window<Number(process.env.WINDOWS||12);window++){
   const result=await page.evaluate(async({players,window})=>{
    const r=th04Sync,e=schedulerQA.emulator(),m=e.module,clock=m.nativeScheduler.pointer,v=()=>new DataView(m.HEAPU8.buffer),api=m.nativeExports(),start=qaFrame;
    const hash=async bytes=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))).map(v=>v.toString(16).padStart(2,'0')).join('');
    const state=async()=>{
     const guest=api.thCoreState(0),length=api.thCoreState(1),reg=api.thCoreState(2),cpu=api.thCoreState(4);
     const audio=[];for(const [frame,blocks]of e.outputs)for(const block of blocks)audio.push([frame,await hash(new Uint8Array(block.buffer,block.byteOffset,block.byteLength))]);
     return {guest:await hash(m.HEAPU8.subarray(guest,guest+length)),regs:Array.from(m.HEAPU8.subarray(reg,reg+api.thCoreState(3))),cpu:Array.from(m.HEAPU8.subarray(cpu,cpu+api.thCoreState(5))),game:r.snapshot(),check:r.checksum(),now:m.virtualNow,deadline:m.targetNow,wakeAt:m.wakeAt,audio};
    };
    const input=i=>Array.from({length:players},(_,slot)=>32|((i+slot+window)%4<2?8:4)|((i+window)%5===0?16:0));
    const advance=m.advanceNative;let resumes=0;
    m.advanceNative=()=>{resumes++;advance();};
    v().setFloat64(clock+16,1,true);r.capture(start);const expected=[];
    for(let i=0;i<12;i++){if(i>0)r.capture(start+i);r.step(input(i),{frame:start+i});expected.push(await state());}
    const oldResumes=resumes;r.restore(start);v().setFloat64(clock+16,2,true);resumes=0;
    const checks=[];
    for(let i=0;i<12;i++){r.capture(start+i);r.step(input(i),{frame:start+i,replay:true});const actual=await state();checks.push({frame:start+i,equal:JSON.stringify(actual)===JSON.stringify(expected[i]),...(JSON.stringify(actual)!==JSON.stringify(expected[i])?{expected:expected[i],actual}:{})});}
    const newResumes=resumes;r.confirm(start+12);qaFrame+=12;m.advanceNative=advance;
    return {start,oldResumes,newResumes,checks,game:r.snapshot(),mode:m.nativeScheduler.mode()};
   },{players,window});
   samples.push(result);
   if(result.checks.some(x=>!x.equal))break;
  }
  reports.push({language,players,samples,errors});
  fs.writeFileSync(path.resolve(__dirname,'../reports/native-scheduler-parity-20261014.json'),JSON.stringify(reports,null,2)+'\n');
  assert.deepEqual(errors,[]);for(const sample of samples)for(const check of sample.checks)assert(check.equal,'native schedule differs at '+check.frame);
  console.log('PASS native scheduler',language,players,'windows',samples.length,'resumes',samples.reduce((n,x)=>n+x.oldResumes,0),'->',samples.reduce((n,x)=>n+x.newResumes,0));
  await page.evaluate(()=>th04Sync.stop());await page.close();
 }}finally{await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
