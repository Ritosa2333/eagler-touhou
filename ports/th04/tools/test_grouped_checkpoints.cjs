const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
(async()=>{
 const browser=await chromium.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true}),reports=[];
 try{
  for(const [language,players,fallback,pauseCase=false]of [['cn',2,false],['cn',3,false],['jp',2,false],['jp',3,false],['cn',3,true],['jp',3,false,true]].filter((_,i)=>!process.env.CHECKPOINT_CASES||JSON.parse(process.env.CHECKPOINT_CASES).includes(i))){
   const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack||e.message));
   if(process.env.QA_RUNTIME_DIR)for(const name of ['dosbox-snapshots.js','dosbox-memory-diff.js','snapshot-diff.wasm','dosbox-rollback.mjs'])await page.route('**/'+name,route=>route.fulfill({contentType:name.endsWith('.wasm')?'application/wasm':'text/javascript',body:fs.readFileSync(path.resolve(__dirname,'..',process.env.QA_RUNTIME_DIR,name))}));
   if(process.env.CANDIDATE)await page.route('**/wdosbox-x-rollback.wasm',route=>route.fulfill({contentType:'application/wasm',body:fs.readFileSync(path.resolve(__dirname,'..',process.env.CANDIDATE))}));
   if(process.env.TRACKED_CANDIDATE)await page.route('**/wdosbox-x-rollback-tracked.wasm',route=>route.fulfill({contentType:'application/wasm',body:fs.readFileSync(path.resolve(__dirname,'..',process.env.TRACKED_CANDIDATE))}));
   if(process.env.CANDIDATE_GLUE)await page.route('**/dosbox-rollback.mjs',route=>route.fulfill({contentType:'text/javascript',body:fs.readFileSync(path.resolve(__dirname,'..',process.env.CANDIDATE_GLUE))}));
   if(process.env.UNTRACKED==='1')await page.route('**/dosbox-adapter.js',async route=>{const response=await route.fetch();await route.fulfill({response,body:(await response.text()).replace('const tracked=net&&','const tracked=false&&net&&')});});
   await page.route('**/netplay/runtime.js',async route=>{const response=await route.fetch();const source=process.env.QA_RUNTIME_DIR?fs.readFileSync(path.resolve(__dirname,'..',process.env.QA_RUNTIME_DIR,'runtime.js'),'utf8'):await response.text();await route.fulfill({response,body:source+'\nglobalThis.checkpointQA={emulator:()=>emulator};'});});
   if(fallback)await page.route('**/dosbox-memory-diff.js',route=>route.fulfill({contentType:'text/javascript',body:'export async function makeMemoryDiff(){return null;}'}));
   await require('./native_scheduler_qa.cjs').install(page);
   await page.goto((process.env.BASE||'http://127.0.0.1:9867')+'/netplay/runtime.html');try{await page.waitForFunction(()=>window.th04Sync);}catch(error){console.error('CHECKPOINT IMPORT',language,players,fallback,pauseCase,errors,await page.locator('body').innerText());throw error;}
   await page.evaluate(async({language,players})=>{const r=th04Sync;r.configure(1,0,players);await r.load({language,players,p1:0,p2:2,p3:1,difficulty:1,lives:6,bombs:2});r.start();for(let i=0;i<60;i++){r.step(Array(players).fill(0),{frame:i});r.confirm(i+1);}}, {language,players});
   const result=await page.evaluate(async({players,fallback,pauseCase})=>{
    const r=th04Sync,e=checkpointQA.emulator(),m=e.module,FS=m.FS,filename='/home/web_user/checkpoint-undo.txt';
    FS.writeFile(filename,new TextEncoder().encode('initial'));
    const advance=m.advanceNative;
    m.advanceNative=()=>{FS.writeFile(filename,new TextEncoder().encode('tick:'+e.tick));advance();};
    let audioDelivered=0,audioFlushes=0;const push=e.audio.push.bind(e.audio),flush=e.audio.flush.bind(e.audio);e.audio.push=channels=>{audioDelivered+=channels[0].length;push(channels);};e.audio.flush=()=>{audioFlushes++;flush();};
    const hash=async()=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',m.HEAPU8))).map(v=>v.toString(16).padStart(2,'0')).join('');
    const state=async()=>({heap:await hash(),check:r.checksum(),game:r.snapshot(),file:FS.readFile(filename,{encoding:'utf8'})});
    const start=r.checksum().tick,length=fallback?4:12,expected=[],samples=[];
    const {packTouch}=await import('../touch-input.js');
    const inputs=index=>{
     const pair=Array.from({length:players},(_,slot)=>32|((index+slot)%4===0?8:(index+slot)%4===1?4:0));
     if(!pauseCase)pair[0]=packTouch(pair[0],{active:true,unlimited:false,x:index%2?16:-16,y:0});
     else if(index===2)return Array.from({length:players},(_,slot)=>slot===0?128:0);
     else if(index===4)return Array.from({length:players},(_,slot)=>slot===1?2:0);
     else if(index===5)return Array.from({length:players},(_,slot)=>slot===0?256:0);
     return pair;
    };
    const mask=index=>players===3&&index>=9?4:0;
    async function advanceWindow(){for(let i=0;i<length;i++){r.capture(start+i);expected[i]=await state();r.step(inputs(i),{frame:start+i,offlineMask:mask(i)});}expected[length]=await state();}
    await advanceWindow();const peak=r.rollbackInfo();
    // Restore both members of several pairs, then replay the remaining tail.
    for(const offset of (fallback?[1,0]:[1,3,9,11,0])){
     r.restore(start+offset);const restored=await state();
     const matches=JSON.stringify(restored)===JSON.stringify(expected[offset]);
     for(let i=offset;i<length;i++){r.capture(start+i);r.step(inputs(i),{frame:start+i,replay:true,offlineMask:mask(i)});}
     samples.push({offset,matches,replayed:JSON.stringify(await state())===JSON.stringify(expected[length])});
    }
    // Confirm just the first tick. Its checkpoint must survive for its second
    // member; rebuilding that confirmed tick must never re-submit audio.
    const splitOffset=pauseCase?3:1;r.confirm(start+splitOffset);const delivered=audioDelivered,flushed=audioFlushes;
    r.restore(start+splitOffset);const split=await state();
    const splitResult={matches:JSON.stringify(split)===JSON.stringify(expected[splitOffset]),audioUnchanged:audioDelivered===delivered,flushUnchanged:audioFlushes===flushed,noConfirmedOutput:[...e.outputs.keys()].every(frame=>frame>=start+splitOffset)};
    for(let i=splitOffset;i<length;i++){r.capture(start+i);r.step(inputs(i),{frame:start+i,replay:true,offlineMask:mask(i)});}
    const final=await state();r.confirm(start+length);const info=r.rollbackInfo();m.advanceNative=advance;
    return {start,peak,samples,split:splitResult,finalMatches:JSON.stringify(final)===JSON.stringify(expected[length]),info};
   },{players,fallback,pauseCase});
   for(const sample of result.samples){assert(sample.matches,`exact restore ${language} ${players}P ${sample.offset}`);assert(sample.replayed,'exact tail replay');}
   assert.equal(result.peak.snapshots,fallback?2:6);assert.equal(result.info.snapshots,0);assert(result.split.matches);assert(result.split.audioUnchanged);assert(result.split.flushUnchanged);assert(result.split.noConfirmedOutput);assert(result.finalMatches);assert.deepEqual(errors,[]);
   reports.push({language,players,fallback,pauseCase,result,errors});console.log('PASS grouped checkpoints',language,players,'fallback',fallback,'pause',pauseCase);
   await page.evaluate(()=>th04Sync.stop());await page.close();
  }
  fs.writeFileSync(path.resolve(__dirname,process.env.OUTPUT||'../reports/grouped-checkpoints-2026-10-14.json'),JSON.stringify(reports,null,2)+'\n');
 }finally{await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
