// Short serial, real-device measurements. Served sources receive observation
// hooks in memory; repository game code, CPU budget and game rules are unchanged.
const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
const root=path.resolve(__dirname,'..'),base=process.env.BASE||'http://192.168.1.102:9868',seconds=Number(process.env.SECONDS||25);
const output=path.resolve(root,process.env.OUTPUT||'reports/k70-usb-room-20261015.json');
const pause=ms=>new Promise(r=>setTimeout(r,ms));
let phone,desktop,phonePage,host;const results=[],errors=[];
const roomProbe=`
const usbWait={samples:0,waiting:0,hidden:0};
const usbTimer=setInterval(()=>{if(running&&!stopped){usbWait.samples++;if(waitingSince)usbWait.waiting++;if(document.hidden)usbWait.hidden++;}},50);
globalThis.usbRoomQA={
 read:()=>({time:performance.now(),running,stopped,stopReason,frame:queue?.frame,confirmed:queue?.confirmed,active:queue?.active,game:runtime?.snapshot(),hz,averageCost,peakCost,inputAge,rollbacks:rollbackCount,replayed:resimulated,info:runtime?.rollbackInfo(),render:runtime?.renderInfo(),costs:{...measuredCosts},wait:{...usbWait},hashes:[...hashes].slice(-8),details:$('diagnostics').textContent,bootStage,bootStep,visibility:document.visibilityState}),
 begin:()=>{usbWait.samples=usbWait.waiting=usbWait.hidden=0;iframe.contentWindow.usbRuntimeQA.reset();},
 probe:()=>iframe.contentWindow.usbRuntimeQA.read(),
 paths:async()=>{const out=[];for(const peer of peers.values()){const stats=await peer.pc.getStats();for(const row of stats.values())if(row.type==='candidate-pair'&&row.state==='succeeded'&&row.nominated){out.push({local:stats.get(row.localCandidateId)?.candidateType,remote:stats.get(row.remoteCandidateId)?.candidateType,rtt:row.currentRoundTripTime});}}return out;},
 stop:()=>{stopped=true;running=false;clearInterval(usbTimer);clearTimers();runtime?.stop();disconnect();}
};`;
const runtimeProbe=`
let usbMetrics={};function usbReset(){usbMetrics={step:[],capture:[],confirm:[],present:[],restore:[]};}
usbReset();for(const name of Object.keys(usbMetrics)){const original=th04Sync[name];th04Sync[name]=function(...args){const at=performance.now();try{return original.apply(this,args);}finally{if(usbMetrics[name].length<12000)usbMetrics[name].push(performance.now()-at);}};}
globalThis.usbRuntimeQA={reset:usbReset,read:()=>({metrics:usbMetrics,heap:emulator?.module.HEAPU8.length,environment:emulator?.module.mobileRuntime,audio:emulator?.module.netInfo(),ua:navigator.userAgent,cores:navigator.hardwareConcurrency})};`;
function stats(values){const sorted=[...values].sort((a,b)=>a-b);return {count:values.length,mean:values.length?values.reduce((a,b)=>a+b,0)/values.length:0,p95:sorted[Math.max(0,Math.ceil(sorted.length*.95)-1)]??0,p99:sorted[Math.max(0,Math.ceil(sorted.length*.99)-1)]??0,max:sorted.at(-1)??0};}
async function state(page){return page.evaluate(()=>usbRoomQA.read());}
async function observe(page,label){
 page.on('pageerror',e=>errors.push({label,message:e.message}));
 await page.route('**/netplay/room.js',async r=>{const response=await r.fetch();const source=process.env.ROOM_SOURCE?fs.readFileSync(path.resolve(root,process.env.ROOM_SOURCE),'utf8'):await response.text();await r.fulfill({response,body:source+roomProbe});});
 await page.route('**/netplay/runtime.js',async r=>{const response=await r.fetch();await r.fulfill({response,body:await response.text()+runtimeProbe});});
}
async function press(page,key){await page.keyboard.press(key,{delay:60});}
async function run(rollback){
 const pages=[host,phonePage];
 for(const p of pages){await p.goto(base+'/lan.html?network=lan&rollback='+(rollback?'on':'off'));await p.waitForFunction(()=>!!window.usbRoomQA);}
 await phonePage.bringToFront();
 await host.click('#create');await host.locator('#lobby').waitFor({state:'visible'});
 await host.selectOption('#lives','6');await host.waitForTimeout(600);
 if(rollback)await host.locator('#rollback').check();else await host.locator('#rollback').uncheck();
 const code=(await host.locator('#room-info').textContent()).match(/房间 (\d{4})/)[1];
 await phonePage.fill('#room',code);await phonePage.click('#join');await phonePage.locator('#lobby').waitFor({state:'visible'});
 await host.click('#seat0');await phonePage.click('#seat1');
 for(const p of pages)await p.waitForFunction(()=>document.querySelector('#seats').textContent.includes('P2：客机 1'));
 for(const p of pages){await p.click('#ready');await p.waitForFunction(()=>document.querySelector('#ready').textContent==='取消准备');}
 await host.waitForFunction(()=>!document.querySelector('#start').disabled);await host.click('#start');
 console.log('START',rollback?'rollback':'lockstep',code);
 const bootAt=Date.now();
 for(let i=0;i<180;i++){
  await pause(1000);const states=await Promise.all(pages.map(state));
  for(const s of states)assert(!s.stopped,s.stopReason);
  if(i%10===0)console.log('BOOT',Math.round((Date.now()-bootAt)/1000),states.map(s=>({step:s.bootStep,frame:s.frame,game:s.game?.gameFrame,active:s.active})));
  if(states.every(s=>s.running&&s.game?.gameFrame>100&&(!rollback||s.active)))break;
  if(i===179)throw Error('USB room did not complete boot');
 }
 const bootSeconds=(Date.now()-bootAt)/1000;
 for(const p of pages){await p.locator('#game .player-stage').click({position:{x:200,y:160}});await p.keyboard.down('KeyZ');}
 await pause(3000);for(const p of pages)await p.evaluate(()=>usbRoomQA.begin());
 const start=await Promise.all(pages.map(state)),samples=[];
 for(let i=0;i<seconds;i++){
  if(process.env.BOTH_MOVE==='1')for(const p of pages){await p.keyboard.up('ArrowLeft');await p.keyboard.up('ArrowRight');await p.keyboard.down(i%2?'ArrowLeft':'ArrowRight');}
  // Ordinary short direction changes exercise actual input prediction. No
  // injected packet delays, skipped stages, invulnerability or CPU throttling.
  if(process.env.BOTH_MOVE!=='1'&&i%5===1)await phonePage.keyboard.down(i%10<5?'ArrowRight':'ArrowLeft');
  if(process.env.BOTH_MOVE!=='1'&&i%5===2){await phonePage.keyboard.up('ArrowRight');await phonePage.keyboard.up('ArrowLeft');}
  await pause(1000);const values=await Promise.all(pages.map(state));
  for(const s of values)assert(s.running&&!s.stopped,s.stopReason);
  samples.push(values);if(i%5===4)console.log('SAMPLE',rollback,i+1,values.map(s=>({stepHz:s.hz,ms:s.averageCost,frame:s.frame,game:s.game.gameFrame,rollbacks:s.rollbacks})));
 }
 for(const p of pages)await p.keyboard.up('KeyZ');
 const end=await Promise.all(pages.map(state)),probes=await Promise.all(pages.map(p=>p.evaluate(()=>usbRoomQA.probe()))),paths=await phonePage.evaluate(()=>usbRoomQA.paths());
 const common=end[0].hashes.filter(([tick])=>end[1].hashes.some(([other])=>other===tick));assert(common.length,'common confirmed checksums');
 for(const [tick,hash]of common)assert.deepEqual(end[1].hashes.find(([other])=>other===tick)[1],hash,'desktop and phone confirmed state');
 const summary=pages.map((p,i)=>{const elapsed=(end[i].time-start[i].time)/1000;return {device:i?(process.env.DEVICE||'Redmi K70, real USB-connected Edge'):'desktop Edge headless',seconds:elapsed,syncHz:(end[i].frame-start[i].frame)/elapsed,gameHz:(end[i].game.gameFrame-start[i].game.gameFrame)/elapsed,drawHz:(end[i].render.drawn-start[i].render.drawn)/elapsed,canvasHz:(end[i].render.canvasSubmitted-start[i].render.canvasSubmitted)/elapsed,rollbacks:end[i].rollbacks-start[i].rollbacks,replayed:end[i].replayed-start[i].replayed,waitingSampleFraction:end[i].wait.waiting/Math.max(1,end[i].wait.samples),hiddenSamples:end[i].wait.hidden,metrics:Object.fromEntries(Object.entries(probes[i].metrics).map(([key,v])=>[key,stats(v)])),heap:probes[i].heap,engine:probes[i].environment,audio:probes[i].audio,render:end[i].render,snapshot:end[i].info,rttMeanMs:samples.reduce((sum,v)=>sum+(Number(/往返 ([\d.]+) ms/.exec(v[i].details)?.[1])||0),0)/samples.length};});
 await phonePage.screenshot({path:path.join(root,`reports/${process.env.DEVICE_TAG||'k70'}-room-${rollback?'rollback':'lockstep'}.png`)});
 const result={rollback,transport:'WebRTC over local LAN; page served over LAN; USB carries debugging',language:'cn',difficulty:'Normal',players:2,stage:process.env.BOTH_MOVE==='1'?'ordinary beginning of stage 1, both firing and changing directions':'ordinary beginning of stage 1, both firing, phone changing directions',bootSeconds,summary,start,end,samples,commonChecksums:common,paths,errors:[...errors]};
 results.push(result);fs.writeFileSync(output,JSON.stringify({schema:'th04-real-usb-phone-room/1',commit:process.env.REVISION||'1a55f37',browser:phone.version(),recordedAt:new Date().toISOString(),results,limitations:'Short stage-1 test; does not establish dense-stage, full-session, other-device or physical-screen refresh performance.'},null,2));
 console.log('RESULT',JSON.stringify({rollback,summary,paths}));
 for(const p of [...pages].reverse())await p.evaluate(()=>usbRoomQA.stop());await pause(500);
}
(async()=>{
 phone=await chromium.connectOverCDP('http://127.0.0.1:9224');
 const context=phone.contexts()[0];phonePage=await context.newPage();
 desktop=await chromium.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true});host=await desktop.newPage();
 await observe(host,'desktop');await observe(phonePage,'phone');
 console.log('CONNECTED',phone.version());if(process.env.ONLY_ROLLBACK!=='1')await run(false);if(process.env.ONLY_LOCKSTEP!=='1')await run(true);assert.deepEqual(errors,[]);console.log('PASS real device LAN measurements');
})().catch(async error=>{console.error(error.stack);for(const p of [host,phonePage])if(p)console.error('FAIL STATE',await state(p).catch(()=>null));fs.writeFileSync(output,JSON.stringify({failure:String(error),results,errors},null,2));process.exitCode=1;}).finally(async()=>{
 for(const p of [host,phonePage])if(p&&!p.isClosed()){await p.evaluate(()=>globalThis.usbRoomQA?.stop()).catch(()=>{});await p.unrouteAll({behavior:'ignoreErrors'}).catch(()=>{});}
 if(desktop)await desktop.close();if(phonePage&&!phonePage.isClosed())await phonePage.close();
 if(phone)await phone.close();
});
