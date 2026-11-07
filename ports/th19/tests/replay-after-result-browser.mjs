import assert from 'node:assert/strict';
import {writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
const browser=await puppeteer.launch({executablePath:process.env.TH19_BROWSER,headless:true,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
try {
 const page=await browser.newPage();
 await page.goto(process.env.TH19_TEST_URL||'http://127.0.0.1:8148/tests/harness.html');
 await page.waitForFunction(()=>window.__TH19_HARNESS?.frame?.contentWindow?.__th19Runtime,{timeout:120000});
 const report=await page.evaluate(async()=>{
  const c=window.__TH19_HARNESS.frame.contentWindow.Module;c._th19_loop_stop();
  const {ConfirmedTimeline,VersusReplayRunner}=await import('/runtime/replay-session.mjs');
  const {th19StreamFrame,encodeTh19Replay,decodeTh19Replay,TH19_FRAME_BYTES}=await import('/runtime/th19-recording.mjs');
  const {RollbackSession}=await import('/runtime/rollback-session.mjs');
  const {nativeRollbackCore}=await import('/runtime/native-adapter.mjs');
  const build=(await(await fetch('/runtime/manifest.json')).json()).buildId;
  const config={rules:[0,1,1,4,4,1,1,0,0],protocol:1,build,seed:190001,difficulty:3,characters:[0,1],delay:0,rollback:true};
  const tape=new ConfirmedTimeline(config),start=new VersusReplayRunner(c,tape);start.reset();
  const a=f=>({keys:f%31<20?3:65,direct:0,x:0,y:0}),b=f=>({keys:f%17<8?129:1,direct:0,x:0,y:0});
  const session=new RollbackSession(nativeRollbackCore(c),{commit:(f,e)=>tape.append(th19StreamFrame(f,e.left,e.right,e))});
  for(let f=0;f<1200;f++){
   if(f>=3)session.receive(f-3,b(f-3));
   if(!session.advance(a(f)))throw Error('Prediction blocked at '+f);
  }
  for(let f=1197;f<1200;f++)session.receive(f,b(f));session.reconcile();
  const rollbacks=session.rollbacks;session.close();tape.end();
  const file=encodeTh19Replay({kind:'versus',build,config,seed:config.seed,frames:tape.length,startedAt:Date.now(),partial:true},tape.payload());
  const decoded=decodeTh19Replay(file),loaded=new ConfirmedTimeline(decoded.meta.config);
  for(let f=0;f<decoded.meta.frames;f++)loaded.append(decoded.payload.subarray(f*TH19_FRAME_BYTES,(f+1)*TH19_FRAME_BYTES));loaded.end();
  const results=[];
  for(const winner of [1,0,1]){
   // Exercise the actual result transition, then replay on the same live core.
   c._th19_test_vs_rules(2,winner===1?1:0);
   const ptr=c._th19_test_vs_rules(-1,0)/4;
   const priorWinner=c.HEAP32[ptr+4];
   if(priorWinner!==winner)throw Error('Result fixture did not set winner');
   const runner=new VersusReplayRunner(c,loaded);runner.reset();
   for(let f=0;f<loaded.length;f++)runner.step(f%60===0,{silent:true});
   results.push({priorWinner,frames:runner.frame,hash:c._th19_network_hash()>>>0});
  }
  return {build,rollbacks,results};
 });
 assert.ok(report.rollbacks>0);assert.equal(new Set(report.results.map(r=>r.hash)).size,1);
 const output=resolve(process.env.TH19_TEST_OUTPUT||'outputs/th19-replay-reset');mkdirSync(output,{recursive:true});
 writeFileSync(resolve(output,'report.json'),JSON.stringify({passed:true,...report},null,2)+'\n');
 await page.screenshot({path:resolve(output,'replay-after-result.png')});
 console.log(JSON.stringify({passed:true,...report}));
}finally{await browser.close();}
