const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
const fs=require('node:fs'),path=require('node:path');
const root=path.resolve(__dirname,'..'),mode=process.env.PROBE_MODE||'baseline';
let browser,page;
const report={mode,scope:'SM-T500 real device, fixed 30000 cycles, two-player network runtime, confirmed synthetic inputs; excludes network wait',samples:[],errors:[]};
const pause=ms=>new Promise(r=>setTimeout(r,ms));
(async()=>{
 browser=await chromium.connectOverCDP('http://127.0.0.1:9224');page=await browser.contexts()[0].newPage();await page.bringToFront();
 page.on('pageerror',e=>report.errors.push(e.message));
 if(mode==='idle')await page.route('**/wdosbox-x-rollback.wasm',r=>r.fulfill({contentType:'application/wasm',body:fs.readFileSync(path.join(root,'build/core-idle-compatible.wasm'))}));
 if(mode.startsWith('native')){process.env.NATIVE_SCHEDULER='1';process.env.NATIVE_RUNTIME=process.env.NATIVE_RUNTIME||'build/native-scheduler/runtime-cpu-helpers';await require('./native_scheduler_qa.cjs').install(page);}
 await page.route('**/netplay/runtime.js',async r=>{const response=await r.fetch();await r.fulfill({response,body:await response.text()+'\nglobalThis.coreProbe={module:()=>emulator.module};'});});
 await page.goto('http://127.0.0.1:9868/netplay/runtime.html');await page.waitForFunction(()=>!!window.th04Sync);
 console.log('LOADING',mode);report.browser=browser.version();
 await page.evaluate(()=>{th04Sync.configure(0,0,2);globalThis.probeLoad=th04Sync.load({players:2,rollback:false,language:'cn',p1:0,p2:2,difficulty:1,lives:6,bombs:2}).then(()=>{th04Sync.start();globalThis.probeReady=true;}).catch(e=>globalThis.probeError=String(e));});
 for(let i=0;i<180;i++){await pause(1000);const state=await page.evaluate(()=>({ready:globalThis.probeReady,error:globalThis.probeError,stage:document.querySelector('#details')?.textContent}));if(state.error)throw Error(state.error);if(i%15===0)console.log('BOOT',i,state.stage);if(state.ready)break;if(i===179)throw Error('load timeout');}
 await page.evaluate(()=>{
 globalThis.probe={frame:0,costs:[],error:null,running:true};let next=performance.now();
 function pump(){if(!probe.running)return;try{const at=performance.now();th04Sync.step([0,0],{frame:probe.frame++});th04Sync.confirm(probe.frame);th04Sync.present();probe.costs.push(performance.now()-at);if(probe.costs.length>10000)probe.costs.shift();next=Math.max(next+1000/60,performance.now());probe.timer=setTimeout(pump,Math.max(0,next-performance.now()));}catch(e){probe.error=String(e);probe.running=false;}}pump();
 });
 for(let i=0;i<100;i++){await pause(1000);const s=await page.evaluate(()=>({game:th04Sync.snapshot(),error:probe.error}));if(s.error)throw Error(s.error);if(s.game?.gameFrame>60)break;if(i===99)throw Error('game startup timeout');}
 const read=()=>page.evaluate(()=>({time:performance.now(),frame:probe.frame,game:th04Sync.snapshot(),render:th04Sync.renderInfo(),hidden:document.hidden,error:probe.error}));
 await pause(2000);report.start=await read();await page.evaluate(()=>probe.costs=[]);
 for(let i=0;i<15;i++){await pause(1000);const s=await read();report.samples.push(s);if(s.error||s.hidden)throw Error(s.error||'tablet page hidden');if(i%5===4)console.log('SAMPLE',mode,i+1,s.game.gameFrame);}
 report.end=await read();report.costs=await page.evaluate(()=>probe.costs);report.hz=(report.end.game.gameFrame-report.start.game.gameFrame)*1000/(report.end.time-report.start.time);report.syncHz=(report.end.frame-report.start.frame)*1000/(report.end.time-report.start.time);
 if(process.env.PROFILE==='1'){
 const cdp=await page.context().newCDPSession(page);await cdp.send('Profiler.enable');await cdp.send('Profiler.setSamplingInterval',{interval:2000});await cdp.send('Profiler.start');await pause(3000);const {profile}=await cdp.send('Profiler.stop');const hit=new Map();for(const id of profile.samples||[])hit.set(id,(hit.get(id)||0)+1);report.hot=profile.nodes.map(n=>({name:n.callFrame.functionName,url:n.callFrame.url,hits:hit.get(n.id)||0})).sort((a,b)=>b.hits-a.hits).slice(0,25);await cdp.detach();}
 console.log('RESULT',JSON.stringify({mode,hz:report.hz,syncHz:report.syncHz,meanMs:report.costs.reduce((a,b)=>a+b,0)/report.costs.length,hot:report.hot,errors:report.errors}));
})().catch(e=>{report.failure=String(e);console.error(e);process.exitCode=1;}).finally(async()=>{if(page){await page.evaluate(()=>{if(globalThis.probe){probe.running=false;clearTimeout(probe.timer);}window.th04Sync?.stop();}).catch(()=>{});await page.close().catch(()=>{});}if(browser)await browser.close();fs.writeFileSync(path.join(root,`reports/tablet-core-${mode}-20261015.json`),JSON.stringify(report,null,2));});
