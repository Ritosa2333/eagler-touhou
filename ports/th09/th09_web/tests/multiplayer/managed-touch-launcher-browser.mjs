// Real React / Package Store / hosted development Runtime integration.
// The browser source is frozen separately; this driver never invents a provider.
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {existsSync,mkdirSync,readFileSync,realpathSync,writeFileSync} from 'node:fs';
import {basename,resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
import {performance} from 'node:perf_hooks';
import {setTimeout as delay} from 'node:timers/promises';
import {chromium} from 'playwright';
import {startManagedTouchLauncherServer} from './managed-touch-launcher-server.mjs';
import {installManagedTouchLauncherProbe} from './managed-touch-launcher-probe.mjs';

const root=process.cwd(),label=process.env.RUN_LABEL;
assert.match(label||'',/^[a-z0-9_-]+$/);
const platform=process.env.TH09_HOSTED_PLATFORM||'desktop';
assert.ok(['desktop','android'].includes(platform));
const movement=process.env.TH09_HOSTED_MOVEMENT||'touch';
assert.ok(['touch','joystick-free'].includes(movement));
const settingActivation=process.env.TH09_HOSTED_SETTING_ACTIVATION||'cdp-touch';
assert.ok(['cdp-touch','keyboard-space'].includes(settingActivation));
assert.ok(process.env.TH09_HOSTED_HEADLESS===undefined||['0','1'].includes(process.env.TH09_HOSTED_HEADLESS));
const desktopHeadless=process.env.TH09_HOSTED_HEADLESS==='1';
const desktopBrowserProfile=process.env.TH09_HOSTED_BROWSER_PROFILE||'installed-channel';
assert.ok(['installed-channel','pinned-native'].includes(desktopBrowserProfile));
assert.ok(desktopBrowserProfile!=='pinned-native'||platform==='desktop'&&!desktopHeadless,'pinned native control uses the existing normal headed desktop browser');
const traceCheckboxWrites=process.env.TH09_HOSTED_TRACE_CHECKBOX==='1';
const dispatchWindow=Number(process.env.TH09_HOSTED_DISPATCH_WINDOW||'1');
assert.ok(Number.isInteger(dispatchWindow)&&dispatchWindow>=1&&dispatchWindow<=8,'bounded real CDP MOVE window');
const rate=Number(process.env.TH09_HOSTED_CPU_RATE||(platform==='desktop'?'4':'1'));
assert.ok([1,4].includes(rate));
assert.ok(platform!=='android'||rate===1,'Android integration uses actual device, not an asserted CPU4 equivalent');
const base=resolve(root,'th09_web/artifacts/multiplayer-tests',label),reportPath=base+'-report.json';
assert.equal(existsSync(base),false,'exclusive run directory');assert.equal(existsSync(reportPath),false);
mkdirSync(base);
const sha=b=>createHash('sha256').update(b).digest('hex');
const identity=p=>{const b=readFileSync(p);return {path:p,bytes:b.length,sha256:sha(b)};};
const scriptPaths=['managed-touch-launcher-browser.mjs','managed-touch-launcher-server.mjs','managed-touch-launcher-probe.mjs'].map(n=>resolve(root,'th09_web/tests/multiplayer',n));
const report={schema:'th09-managed-touch-hosted-integration/v1',runLabel:label,startedAt:new Date().toISOString(),
 scope:'Ordinary React, Package Store and single real native Runtime on validated web-development Host; not immutable publication, physical-finger latency, or multiplayer performance acceptance',
 platform,movement,settingActivation,traceCheckboxWrites,desktopHeadless:platform==='desktop'?desktopHeadless:null,cpuRate:rate,audio:'none BGM resolved and checked in real Game settings; record actual SFX AudioContext separately',
 sourceBefore:scriptPaths.map(identity),stages:[],pageErrors:[],consoleErrors:[],requestsFailed:[],deviceCommands:[],injections:[],gestureAttempts:[],
 dispatchWindow,dispatchScope:'Ordered actual CDP sends with bounded pending completion slots; the slot peak can include fulfilled promises. Planned time starts before DOWN; the 20s guard precedes each MOVE, not final drain/UP. A timed-out underlying CDP request may remain unresolved. Browser coalescing and actual DOM/native receipt are measured separately.',goals:{}};
const stage=(name,fields={})=>{const row={name,nodeAt:performance.now(),...fields};report.stages.push(row);console.log(JSON.stringify({stage:name,...fields}));};
let server,browser,context,page,session,ownPage=false,forwardPort=null,reversePort=null,remote=false,probeInstalled=false;
let closing=false,ownedPagePromise=null,inputOutcomeUnknown=false;
let adbPath,deviceId;
function bounded(promise,ms,name){
 let timer;
 const limit=new Promise((_,reject)=>{timer=setTimeout(()=>reject(Error('Bounded operation timed out; outcome unknown: '+name)),ms);});
 return Promise.race([promise,limit]).finally(()=>clearTimeout(timer));
}
function adb(args,{allowFailure=false,recordOutput=true}={}){
 const result=spawnSync(adbPath,['-s',deviceId,...args],{encoding:'utf8',windowsHide:true,timeout:20000,maxBuffer:1024*1024});
 const row={args,status:result.status,signal:result.signal,error:result.error?.message||null,
  stdout:recordOutput?(result.stdout||'').trim():null,stderr:(result.stderr||'').trim()};
 report.deviceCommands.push(row);
 if(!allowFailure){assert.equal(result.error,undefined,'ADB process error');assert.equal(result.status,0,'ADB failed '+args.join(' ')+': '+row.stderr);}
 return (result.stdout||'').trim();
}
async function requestText(url,options={},name='HTTP request'){
 const lifetime=new AbortController(),timer=setTimeout(()=>lifetime.abort(Error('HTTP deadline exceeded: '+name)),10000);
 try{const response=await fetch(url,{...options,signal:lifetime.signal});return {ok:response.ok,status:response.status,text:await response.text()};}
 finally{clearTimeout(timer);}
}
async function control(action,body){
 const options=body===undefined?{}:{method:'POST',headers:{'content-type':'application/json'},body:JSON.stringify(body)};
 const response=await requestText(server.controlUrl+'/'+action,options,'control '+action);
 if(!response.ok)throw Error('control '+action+' '+response.status+': '+response.text);
 return JSON.parse(response.text);
}
async function until(check,timeout,label){
 const began=performance.now();let last;
 while(performance.now()-began<timeout){last=await bounded(check(),Math.min(30000,timeout),'condition '+label);if(last)return last;await delay(75);}
 throw Error('Timed out: '+label);
}
async function currentState(){
 const calledAt=performance.now();
 const state=await bounded(page.evaluate(()=>{
  const sampledAt=performance.now(),timeOrigin=performance.timeOrigin;
  const fs=document.querySelectorAll('[data-runtime-host] iframe');
  if(fs.length!==1)return {frames:fs.length,sampledAt,timeOrigin};
  const f=fs[0],w=f.contentWindow;
  try {
   const href=w.location.href,r=w.__th09Runtime,c=r?.core||w.Module;
   const ints=(name,n)=>{if(typeof c?.[name]!=='function')return null;const p=c[name]();return Array.from(new Int32Array(c.HEAPU8.buffer,p,n));};
   const floats=(name,n)=>{if(typeof c?.[name]!=='function')return null;const p=c[name]();return Array.from(new Float32Array(c.HEAPU8.buffer,p,n));};
   return {frames:fs.length,href,sampledAt,timeOrigin,owned:f.isConnected&&f.contentDocument===w.document,
    visible:document.querySelector('[data-runtime-host]')?.getAttribute('aria-hidden')==='false',
    documentVisibility:w.document.visibilityState,documentFocus:w.document.hasFocus(),
    runtimeSampledAt:w.performance.now(),runtimeTimeOrigin:w.performance.timeOrigin,
    startupMarks:w.performance.getEntriesByType('mark').filter(x=>['eagler-startup-image','eagler-startup-menu-ready'].includes(x.name)).map(x=>({name:x.name,startTime:x.startTime})),
    commandAudit:w.__th09HostedCommandAudit||null,
    runtime:!!r,epoch:Number(new URL(href).searchParams.get('runtimeEpoch')),
    title:ints('_th09_title_status',8),session:ints('_th09_session_status',8),touch:ints('_th09_touch_state',4),
    position:floats('_th09_touch_probe',8),rollback:ints('_th09_rollback_info',8),
    options:c?.eaglerOptions||null,audioState:c?.SDL3?.audioContext?.state||null,
    error:w.document.querySelector('#error')?.textContent||'',
    overlay:!!document.querySelector('.touch-runtime'),bridge:typeof w.__eaglerDirectInputBridge};
  }catch(error){return {frames:fs.length,error:String(error)};}
 }),30000,'current Runtime state');
 return {...state,calledAt,returnedAt:performance.now()};
}
async function acceptLaunchWarningsUntil(check,timeout=180000){
 const allowed=new Set(['touch.disabledInputWarning','music.noneLaunchWarning','music.midiLaunchWarning']);
 // launchInputWarnings emits each kind at most once for this one Start intent.
 // The exit animation retains the old data-launch-warning after acceptance.
 const acknowledged=new Set();
 return until(async()=>{
  if(await check())return true;
  // Read the exit-animation list and its kinds in one DOM operation. A
  // count/getAttribute pair can lose its element and auto-wait for 30 seconds
  // while the Runtime has already launched successfully.
  const warningKinds=await page.locator('[data-launch-warning]:visible').evaluateAll(elements=>elements.map(element=>element.getAttribute('data-launch-warning')));
  if(warningKinds.length){
   const kind=warningKinds[0];
   assert.ok(allowed.has(kind),'unrelated dialog must not be accepted: '+kind);
   if(acknowledged.has(kind)){
    report.warningExitObservations??={};report.warningExitObservations[kind]=(report.warningExitObservations[kind]||0)+1;
    return false;
   }
   report.warningClickAttempts??=[];
   const attempt={kind,calledAt:performance.now(),returnedAt:null,error:null};report.warningClickAttempts.push(attempt);
   try{
    const warning=page.locator('[data-launch-warning="'+kind+'"]:visible').first();
    await warning.getByRole('button',{name:'Start anyway',exact:true}).click({timeout:10000});
    acknowledged.add(kind);report.acceptedWarnings??=[];report.acceptedWarnings.push(kind);
   }
   catch(error){attempt.error=String(error);throw error;}
   finally{attempt.returnedAt=performance.now();}
  }
  return false;
 },timeout,'documented launch warning / actual launch boundary');
}
async function tapKey(code,holdMs=80){
 await page.keyboard.down(code);await delay(holdMs);await page.keyboard.up(code);await delay(140);
}
async function chooseSetting(scope,suffix,value){
 const select=scope.locator('select[id$="-'+suffix+'"]');await select.waitFor({state:'attached',timeout:60000});
 assert.equal(await select.count(),1,'one real preference '+suffix);
 const before=await select.inputValue();
 report.preferenceChecks??=[];const row={suffix,before,requested:value,disabled:await select.isDisabled(),changed:false};report.preferenceChecks.push(row);
 if(before!==value){
  assert.equal(row.disabled,false,'requested preference must be supported');
  await select.locator('..').locator('.mizuki-select-trigger').click();
  await page.locator('.mizuki-select-menu:visible [role="option"][data-value="'+value+'"]').click();
  if(suffix==='movement'&&value==='joystick-free'){
   const prompt=page.getByRole('dialog',{name:'Confirm',exact:true});await prompt.waitFor();
   const description=await prompt.textContent();assert.ok(description.includes('The unrestricted-direction stick records replays in a new format'));
   report.acceptedTouchModeWarning=description;await prompt.getByRole('button',{name:'Enable',exact:true}).click();
  }
  await until(async()=>await select.inputValue()===value,10000,'real preference applied '+suffix);row.changed=true;
 }
 row.after=await select.inputValue();assert.equal(row.after,value);
}
async function input(type,points,plannedAt){
 assert.equal(inputOutcomeUnknown,false,'no further Input after uncertain delivery');
 const row={seq:report.injections.length,type,points:points.map(x=>({...x})),plannedAt,
  calledAt:performance.now(),returnedAt:null,error:null};
 report.injections.push(row);
 try{await bounded(session.send('Input.dispatchTouchEvent',{type,touchPoints:points}),10000,'one touch dispatch');}
 catch(error){inputOutcomeUnknown=true;row.error=String(error);row.deliveryOutcome='unknown; do not retry this event or send subsequent Input';throw error;}
 finally{row.returnedAt=performance.now();}
}
async function trajectory(rect,durationMs=2200,hz=60){
 const steps=Math.ceil(durationMs*hz/1000),start=performance.now();
 const attempt={plannedHz:hz,plannedDurationMs:durationMs,steps,dispatchWindow,startNodeMs:start,
  firstInjection:report.injections.length,movesIssued:0,pendingWindowPeak:0,backpressureWaits:0,
  moveErrors:[],completed:false,releaseAttempted:false,releaseConfirmed:false};
 report.gestureAttempts.push(attempt);
 const cx=rect.x+rect.width*.50,cy=rect.y+rect.height*.48;
 const sx=movement==='touch'?Math.min(55,rect.width*.14):rect.width*.29;
 const sy=movement==='touch'?Math.min(35,rect.height*.08):rect.height*.29;
 const point=(x,y)=>({id:1,x,y,radiusX:4,radiusY:4,force:.8});
 const pending=[];let failure=null,downConfirmed=false;
 try{
  await input('touchStart',[point(cx,cy)],start);downConfirmed=true;
  for(let n=1;n<=steps;n++){
   const planned=start+n*1000/hz;
   if(performance.now()<planned)await delay(Math.max(0,planned-performance.now()));
   if(pending.length>=dispatchWindow){
    attempt.backpressureWaits++;
    const error=await pending.shift();if(error)throw error;
   }
   assert.equal(inputOutcomeUnknown,false,'stop scheduling after an uncertain MOVE');
   assert.ok(performance.now()-start<20000,'bounded gesture dispatch duration');
   // Four linear legs preserve reversals; no resampling or skipped sent MOVE.
   const u=n/steps*4,leg=Math.min(3,Math.floor(u)),v=u-leg;
   const vertices=[[0,0],[1,-1],[-1,-1],[-1,1],[0,0]];
   const a=vertices[leg],b=vertices[leg+1],x=cx+sx*(a[0]+(b[0]-a[0])*v),y=cy+sy*(a[1]+(b[1]-a[1])*v);
   // Observe rejection immediately; every issued request is drained even on failure.
   // Never skip a late source MOVE. Browser input coalescing remains observable.
   const completion=input('touchMove',[point(x,y)],planned).then(()=>null,error=>{
    attempt.moveErrors.push(String(error));return error;
   });
   pending.push(completion);attempt.movesIssued++;
   attempt.pendingWindowPeak=Math.max(attempt.pendingWindowPeak,pending.length);
  }
 }catch(error){failure=error;attempt.failure=String(error);}
 finally{
  const remaining=await Promise.all(pending);
  if(!failure)failure=remaining.find(Boolean)||null;
  if(downConfirmed&&!inputOutcomeUnknown){
   attempt.releaseAttempted=true;
   try{await input('touchEnd',[],performance.now());attempt.releaseConfirmed=true;}
   catch(error){attempt.releaseError=String(error);if(!failure)failure=error;}
  }else if(downConfirmed)attempt.releaseSkipped='prior Input outcome unknown; close owned page instead of another Input';
  attempt.afterLastInjection=report.injections.length;attempt.finishedNodeMs=performance.now();
  attempt.actualNodeDispatchDurationMs=attempt.finishedNodeMs-start;
  attempt.completed=!failure&&attempt.movesIssued===steps&&attempt.releaseConfirmed;
  if(failure&&!attempt.failure)attempt.failure=String(failure);
 }
 if(failure)throw failure;
 return {plannedHz:hz,plannedDurationMs:durationMs,steps,actualNodeDispatchDurationMs:performance.now()-start,
  dispatchWindow,movesIssued:attempt.movesIssued,
  meaning:'CDP-injected source plan; count actual DOM and native receipt/coalescing separately, not physical input rate'};
}
async function tapHud(selector,holdMs){
 const rect=await page.locator(selector).boundingBox();assert.ok(rect,'real HUD control geometry');
 await input('touchStart',[{id:2,x:rect.x+rect.width/2,y:rect.y+rect.height/2,radiusX:4,radiusY:4,force:.8}],performance.now());
 await delay(holdMs);await input('touchEnd',[],performance.now());await delay(120);
}
async function probePhase(name,meta={}){
 const result=await bounded(page.evaluate(({name,meta})=>window.__th09ManagedTouchIntegrationProbeV1.phase(name,meta),{name,meta}),10000,'probe phase');
 assert.equal(result.accepted,true,'phase must retain original Runtime owner');
}
async function snapshot(includeRecords=true){return bounded(page.evaluate(includeRecords=>window.__th09ManagedTouchIntegrationProbeV1.snapshot({includeRecords}),includeRecords),10000,'probe snapshot');}
async function disposeProbe(name){
 report.probe=await bounded(page.evaluate(()=>window.__th09ManagedTouchIntegrationProbeV1.dispose()),20000,name);
 probeInstalled=false;
 const {records:disposedRecords,...disposedMetadata}=report.probe;
 report.disposed={...disposedMetadata,coverage:{...disposedMetadata.coverage,includedRecords:false,rawComplete:false},
  scopeNote:'Metadata from the final disposed snapshot; complete raw is retained once in report.probe'};
 report.probeRestored=report.probe.hooksAtDisposal.every(row=>row.stillOurs)&&report.probe.hooks.every(row=>row.restored&&!row.restoreError);
 assert.equal(report.probeRestored,true,'restore only captured original probe properties');
}
function inputEvidence(value,name,throughSeq=Infinity){
 assert.equal(value.coverage.rawComplete,true,'complete bounded probe raw');
 assert.equal(value.disposed?value.hooksAtDisposal?.every(row=>row.stillOurs===true):value.coverage.allHooksCurrent,true,'original probe hook bindings at observation end');
 assert.equal(value.owner.valid,true);assert.equal(value.owner.invalidation,null);assert.equal(value.gettersCurrent,true);
 const phase=value.phases.find(row=>row.name===name);assert.ok(phase,'recorded input phase '+name);
 const rows=value.records.filter(row=>row.phaseId===phase.id&&row.seq<=throughSeq);
 const posts=rows.filter(row=>row.kind==='postMessage-entry');
 const deliveries=rows.filter(row=>row.kind==='message-observed');
 const natives=rows.filter(row=>row.kind==='native-entry');
 assert.ok(rows.some(row=>row.kind==='pointer'&&row.type==='pointermove'),'actual overlay MOVE was dispatched');
 const command=movement==='touch'?'direct-touch':'touch-controls';
 const submitted=posts.filter(row=>row.command===command),received=deliveries.filter(row=>row.command===command);
 assert.ok(submitted.length>=10,'active host input submissions, not an empty positive');
 assert.deepEqual(received.map(row=>row.message),submitted.map(row=>row.message),'every observed submitted message delivered in order');
 if(movement==='touch'){
  const calls=natives.filter(row=>row.name==='_th09_touch');
  assert.deepEqual(calls.map(row=>row.args.slice(0,2)),submitted.map(row=>[({down:0,move:1,up:2,cancel:2})[row.message.type],row.message.id]),
   'every original direct message reaches native once in order');
  assert.ok(calls.every(row=>row.args.every(Number.isFinite)),'finite native direct parameters');
 }else{
  const calls=natives.filter(row=>row.name==='_th09_touch_controls'||row.name==='_th09_touch_stick');
  assert.deepEqual(calls.map(row=>row.name),submitted.flatMap(()=>['_th09_touch_controls','_th09_touch_stick']),
   'each real joystick message retains controls then stick');
  assert.ok(calls.some(row=>row.name==='_th09_touch_stick'&&row.args.some(x=>x!==0)),'real nonzero joystick axes');
 }
 const samples=rows.filter(row=>row.kind==='frame-after-original'&&row.state),first=samples[0],last=samples.at(-1);
 assert.ok(samples.length>=2,'existing native game callbacks continue');
 assert.ok(last.state.session[1]>first.state.session[1],'new logical game progress during input');
 assert.ok(last.state.gameMetrics[9]>first.state.gameMetrics[9],'native presentations advance during input');
 const positionRanges=[0,1].map(axis=>{const values=samples.map(row=>row.state.touchProbe.values[axis]);assert.ok(values.every(Number.isFinite));return {min:Math.min(...values),max:Math.max(...values)};});
 assert.ok(positionRanges.some(axis=>axis.max>axis.min),'actual player position changes during movement');
 return {phase:name,throughSeq:Number.isFinite(throughSeq)?throughSeq:null,
  counts:Number.isFinite(throughSeq)?null:value.byPhase.find(row=>row.id===phase.id),submitted:submitted.length,delivered:received.length,
  nativeCalls:natives.length,frameSamples:samples.length,positionRanges,first: {atMs:first.sampledAtMs,session:first.state.session,gameMetrics:first.state.gameMetrics},
  last:{atMs:last.sampledAtMs,session:last.state.session,gameMetrics:last.state.gameMetrics},
  scope:'Complete observed single-owner phase, original native input and new progress; not physical latency or a no-hitch performance gate'};
}
try{
 const options=JSON.parse(process.env.TH09_HOSTED_SERVER_OPTIONS||'{}');
 assert.equal(Object.hasOwn(options,'artifactDir'),false,'driver owns exclusive server artifact directory');
 server=await startManagedTouchLauncherServer({...options,artifactDir:resolve(base,'server')});
 report.server={url:server.url,controlUrl:server.controlUrl,identity:server.identity};
 const hostResponse=await requestText(new URL('host-manifest.json',server.url),{},'host manifest'),host=JSON.parse(hostResponse.text);
 assert.equal(hostResponse.status,200);assert.equal(host.profile,'web-development');assert.equal(host.shared?.resourceMode,'hosted');
 assert.equal(host.shared?.testBuild,true);assert.equal(host.shared?.runtimeManifest,undefined);
 report.host=host;
 for(const path of ['ui-publication.json','__unlisted_probe__.json','app-shell-sw.js']){
  const response=await requestText(new URL(path,server.url),{},'absent '+path);assert.equal(response.status,404,path+' must be absent');
 }
 stage('host-preflight',{profile:host.profile,engineRebuilt:false});
 if(platform==='android'){
  remote=true;adbPath=process.env.TH09_ADB_PATH;deviceId=process.env.TH09_ANDROID_SERIAL;
  assert.ok(adbPath&&deviceId,'explicit existing ADB and Android device required');
  const existingForward=adb(['forward','--list']),existingReverse=adb(['reverse','--list']);
  assert.ok(!existingForward.split(/\r?\n/).some(line=>line.trim().split(/\s+/)[0]===deviceId&&line.includes('chrome_devtools_remote')),
   'another Chrome DevTools forwarding owner is present');
  report.android={model:adb(['shell','getprop','ro.product.model']),android:adb(['shell','getprop','ro.build.version.release']),
   sdk:adb(['shell','getprop','ro.build.version.sdk']),size:adb(['shell','wm','size']),
   battery:adb(['shell','dumpsys','battery']),package:adb(['shell','pm','list','packages','org.chromium.chrome']),
   beforeForward:existingForward,beforeReverse:existingReverse};
  assert.match(report.android.package,/package:org\.chromium\.chrome/);
  const activity=adb(['shell','cmd','package','resolve-activity','--brief','org.chromium.chrome']).split(/\r?\n/).find(s=>/^org\.chromium\.chrome\/[A-Za-z0-9_.$]+$/.test(s));
  assert.ok(activity,'resolve installed Chromium launcher activity');report.android.activity=activity;
  adb(['shell','am','start','-W','-a','android.intent.action.MAIN','-c','android.intent.category.LAUNCHER','-n',activity]);
  stage('android-chromium-foreground',{model:report.android.model,activity});
  forwardPort=Number(adb(['forward','--no-rebind','tcp:0','localabstract:chrome_devtools_remote']));
  assert.ok(Number.isInteger(forwardPort)&&forwardPort>0);
  const hostPort=Number(new URL(server.url).port);assert.ok(hostPort>0);
  assert.ok(!existingReverse.split(/\r?\n/).some(line=>line.split(/\s+/).includes('tcp:'+hostPort)),'reverse mapping already owned');
  adb(['reverse','--no-rebind','tcp:'+hostPort,'tcp:'+hostPort]);reversePort=hostPort;
  const endpoint='http://127.0.0.1:'+forwardPort;
  stage('android-owned-mappings-ready',{forwardPort,reversePort});
  const versionResponse=await requestText(endpoint+'/json/version',{},'Android DevTools version');
  assert.equal(versionResponse.status,200);report.android.devtoolsVersion=JSON.parse(versionResponse.text);
  const debuggerUrl=new URL(report.android.devtoolsVersion.webSocketDebuggerUrl);
  assert.equal(debuggerUrl.protocol,'ws:');assert.ok(debuggerUrl.pathname==='/devtools/browser'||debuggerUrl.pathname.startsWith('/devtools/browser/'));
  debuggerUrl.host=new URL(endpoint).host;
  stage('android-devtools-version-ready',{browser:report.android.devtoolsVersion.Browser});
  browser=await chromium.connectOverCDP(debuggerUrl.href,{timeout:30000,noDefaults:true});
  context=browser.contexts()[0];assert.ok(context,'existing Android browser context');
  report.android.existingPageCount=context.pages().length;
  report.android.pageCreation={outcome:'pending',requestedAt:performance.now()};
  ownedPagePromise=context.newPage().then(async created=>{
   report.android.pageCreation={...report.android.pageCreation,outcome:'created',returnedAt:performance.now()};
   if(closing){
    try{await bounded(created.close({runBeforeUnload:false}),5000,'late owned Android page close');report.android.pageCreation.latePageClosed=true;}
    catch(error){report.latePageCloseError=String(error);}
    return null;
   }
   page=created;ownPage=true;return created;
  },error=>{report.android.pageCreation={...report.android.pageCreation,outcome:'API rejected; target creation outcome unknown',error:String(error)};throw error;});
  page=await bounded(ownedPagePromise,30000,'new owned Android page');assert.ok(page);
  await bounded(page.bringToFront(),10000,'owned Android page foreground');
  stage('android-owned-page-ready');
 }else{
  let launchOptions={channel:'chrome',headless:desktopHeadless};
  report.desktopLaunch={profile:desktopBrowserProfile,headless:desktopHeadless,scope:'Desktop touch emulation; browser launch identity is not a physical phone equivalence'};
  if(desktopBrowserProfile==='pinned-native'){
   const executablePath=realpathSync(chromium.executablePath());
   assert.equal(process.platform,'win32');assert.equal(basename(executablePath).toLowerCase(),'chrome.exe');
   if(process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE)assert.equal(realpathSync(process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE).toLowerCase(),executablePath.toLowerCase(),'one pinned existing executable');
   const args=['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required',
    '--hide-scrollbars','--mute-audio','--blink-settings=primaryHoverType=2,availableHoverTypes=2,primaryPointerType=4,availablePointerTypes=4'];
   const {path:ignoredPath,...executableIdentity}=identity(executablePath);
   report.desktopLaunch={...report.desktopLaunch,executable:{basename:basename(executablePath),...executableIdentity},explicitArgs:args,requestedBackend:'ANGLE / D3D11'};
   launchOptions={executablePath,headless:false,args};
  }
  browser=await chromium.launch(launchOptions);
  const version=browser.version();
  context=await browser.newContext({viewport:{width:412,height:844},deviceScaleFactor:1,isMobile:true,hasTouch:true,
   userAgent:'Mozilla/5.0 (Linux; Android 11; TH09 desktop emulation) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/'+version+' Mobile Safari/537.36'});
  page=await context.newPage();ownPage=true;await page.bringToFront();
 }
 page.setDefaultTimeout(30000);page.setDefaultNavigationTimeout(60000);
 page.on('pageerror',error=>{if(report.pageErrors.length<50)report.pageErrors.push(String(error));});
 page.on('console',message=>{if(message.type()==='error'&&report.consoleErrors.length<100)report.consoleErrors.push(message.text().slice(0,1000));});
 page.on('requestfailed',request=>{if(report.requestsFailed.length<100)report.requestsFailed.push({url:request.url(),failure:request.failure(),nodeAt:performance.now(),timing:request.timing()});});
 report.mainFrameNavigations=[];
 page.on('framenavigated',frame=>{if(frame===page.mainFrame()&&report.mainFrameNavigations.length<50)report.mainFrameNavigations.push({url:frame.url(),nodeAt:performance.now()});});
 const origin=new URL(server.url).origin;
 await page.addInitScript(({origin})=>{
  if(location.origin!==origin)return;
  localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');
  localStorage.setItem('eagler-touhou-site-notice-enabled-v1','0');
  localStorage.setItem('eagler-touch-help-seen-v8','1');
  if(window!==window.top){
   // Observe only protocol control commands to this owned Runtime document.
   // Input MOVE and the original listener remain untouched.
   const commands=new Set(['configure','launch','stop','resources','network-open','network-cancel']);
   window.__th09HostedCommandAudit={events:[],overflow:false};
   addEventListener('message',event=>{
    const m=event.data;
    if(event.source!==parent||event.origin!==origin||m?.protocol!=='eagler-touhou/1'||m.game!=='th09'||!commands.has(m.command))return;
    const epoch=Number(new URL(location.href).searchParams.get('runtimeEpoch'));
    if(!Number.isSafeInteger(epoch)||epoch<1||m.epoch!==epoch)return;
    const audit=window.__th09HostedCommandAudit;
    if(audit.events.length>=250){audit.overflow=true;return;}
    audit.events.push({at:performance.now(),timeOrigin:performance.timeOrigin,epoch,command:m.command,request:m.request??null,
     netplayMode:m.options?.netplayMode??null});
   });
   return;
  }
  window.__th09HostedEvents=[];
  addEventListener('message',event=>{
   const f=document.querySelector('[data-runtime-host] iframe'),m=event.data;
   if(!f||event.source!==f.contentWindow||event.origin!==location.origin||m?.protocol!=='eagler-touhou/1'||m.game!=='th09')return;
   let epoch;try{epoch=Number(new URL(f.contentWindow.location.href).searchParams.get('runtimeEpoch'));}catch{return;}
   if(!Number.isSafeInteger(epoch)||epoch<1||m.epoch!==epoch)return;
   if(window.__th09HostedEvents.length<2000)window.__th09HostedEvents.push({at:performance.now(),timeOrigin:performance.timeOrigin,
    epoch,event:m.event,request:m.request,ok:m.ok,error:m.error,loaded:m.loaded,total:m.total,path:m.path});
  });
 },{origin});
 session=await context.newCDPSession(page);
 if(platform==='desktop')await session.send('Emulation.setCPUThrottlingRate',{rate});
 await page.goto(new URL('play/th09?uiLocale=en',server.url).href,{waitUntil:'load'});
 await page.locator('html[data-ui-locale="en"]').waitFor();
 assert.equal(await page.locator('[data-runtime-host] iframe').count(),1);
 const form=page.getByRole('form',{name:'Game settings',exact:true});await form.waitFor();
 await chooseSetting(form,'music','none');
 await page.evaluate(()=>{
  window.__th09SettingsInputAudit=[];const lifetime=new AbortController();window.__th09SettingsInputAuditStop=()=>lifetime.abort();
  for(const type of ['pointerdown','pointerup','mousedown','mouseup','click','input','change','keydown','keyup'])document.addEventListener(type,event=>{
   const target=event.target;if(!(target instanceof Element)||window.__th09SettingsInputAudit.length>=300)return;
   const row={type:event.type,at:performance.now(),trusted:event.isTrusted,target:target.tagName,id:target.id,inputType:target.type,checked:target.checked,
    isConnected:target.isConnected,url:location.href,settingsPresent:!!document.querySelector('[data-game-settings]'),
    pointerType:event.pointerType,pointerId:event.pointerId,x:event.clientX,y:event.clientY,key:event.key,code:event.code,detail:event.detail,
    defaultPrevented:event.defaultPrevented,focused:document.hasFocus(),path:event.composedPath().filter(x=>x instanceof Element).slice(0,12).map(x=>({
     tag:x.tagName,id:x.id,href:x.getAttribute('href'),product:x.getAttribute('data-library-product'),settings:x.hasAttribute('data-game-settings')}))};
   window.__th09SettingsInputAudit.push(row);
   queueMicrotask(()=>{row.microtask={at:performance.now(),defaultPrevented:event.defaultPrevented,checked:target.checked,isConnected:target.isConnected};});
   setTimeout(()=>{row.nextTask={at:performance.now(),defaultPrevented:event.defaultPrevented,checked:target.checked,isConnected:target.isConnected,url:location.href};},0);
  },{capture:true,passive:true,signal:lifetime.signal});
 });
 const touchEnabled=form.getByRole('switch',{name:'Enable touch controls',exact:true});
 report.touchPreferenceBefore=await touchEnabled.isChecked();
 report.touchPreferenceElement=await touchEnabled.evaluate(element=>({id:element.id,type:element.type,checked:element.checked,disabled:element.disabled,
  matchingIdCount:[...document.querySelectorAll('[id]')].filter(x=>x.id===element.id).length,
  preferences:[...document.querySelectorAll('[data-game-settings] select')].map(x=>({id:x.id,value:x.value,disabled:x.disabled})),
  viewport:{width:innerWidth,height:innerHeight,scale:visualViewport?.scale,offsetLeft:visualViewport?.offsetLeft,offsetTop:visualViewport?.offsetTop},
  labels:[...element.labels].map(x=>({for:x.htmlFor,text:x.textContent})),
  ancestry:(()=>{const result=[];for(let p=element;p&&result.length<12;p=p.parentElement)result.push({tag:p.tagName,id:p.id,className:p.className,href:p.getAttribute('href')});return result;})()}));
 if(traceCheckboxWrites)await touchEnabled.evaluate(element=>{
  const own=Object.getOwnPropertyDescriptor(element,'checked'),native=Object.getOwnPropertyDescriptor(HTMLInputElement.prototype,'checked'),original=own||native;
  if(!original?.get||!original?.set||own&&!own.configurable)throw Error('Checkbox accessor cannot be observed without changing its contract');
  const lifetime=new AbortController(),trace={id:element.id,hadOwnAccessor:!!own,writes:[],windowEvents:[],overflow:false,restored:null};
  const storage=()=>Object.fromEntries(['eagler-touhou-game-options-v1-th09','eagler-touhou-touch-options-v1'].map(key=>[key,localStorage.getItem(key)]));
  trace.storageBefore=storage();
  const observedSet=function(value){
   const row={at:performance.now(),requested:value,nativeBefore:Reflect.apply(native.get,this,[]),stack:new Error('observed checkbox write').stack};
   if(trace.writes.length<100)trace.writes.push(row);else trace.overflow=true;
   try{return Reflect.apply(original.set,this,[value]);}finally{row.nativeAfter=Reflect.apply(native.get,this,[]);}
  };
  Object.defineProperty(element,'checked',{...original,set:observedSet});
  for(const type of ['pointerdown','pointerup','click','input','change','keydown','keyup'])window.addEventListener(type,event=>{
   if(event.target!==element)return;
   if(trace.windowEvents.length>=100){trace.overflow=true;return;}
   trace.windowEvents.push({type:event.type,at:performance.now(),trusted:event.isTrusted,checked:element.checked,nativeChecked:Reflect.apply(native.get,element,[]),defaultPrevented:event.defaultPrevented});
  },{capture:true,passive:true,signal:lifetime.signal});
  let stopped=false;
  window.__th09CheckboxWriteTraceStop=()=>{
   if(!stopped){stopped=true;lifetime.abort();const current=Object.getOwnPropertyDescriptor(element,'checked');
    if(current?.set===observedSet){if(own)Object.defineProperty(element,'checked',own);else delete element.checked;trace.restored=true;}
    else trace.restored=false;trace.storageAfter=storage();trace.finalNativeChecked=Reflect.apply(native.get,element,[]);
   }
   return trace;
  };
 });
 if(!report.touchPreferenceBefore){
  if(settingActivation==='keyboard-space'){await touchEnabled.focus();await touchEnabled.press('Space');}
  else{
   await touchEnabled.scrollIntoViewIfNeeded();const r=await touchEnabled.boundingBox();assert.ok(r);
   report.touchPreferenceTap={rect:r,hit:await page.evaluate(({x,y})=>{const e=document.elementFromPoint(x,y);return e?{tag:e.tagName,id:e.id,type:e.type}:null;},{x:r.x+r.width/2,y:r.y+r.height/2})};
   await input('touchStart',[{id:11,x:r.x+r.width/2,y:r.y+r.height/2,radiusX:4,radiusY:4,force:.8}],performance.now());
   await delay(80);await input('touchEnd',[],performance.now());
  }
 }
 await until(async()=>await touchEnabled.isChecked(),10000,'real touch switch enabled');
 report.touchPreferenceAfter=await touchEnabled.isChecked();
 await form.getByRole('button',{name:'Button layout & touch settings',exact:true}).click();
 const editor=page.locator('[data-touch-editor-scene]');await editor.waitFor();
 const collapse=editor.locator('[data-touch-workbench-collapse]');
 if(await collapse.getAttribute('aria-expanded')==='false')await collapse.click();
 await chooseSetting(editor,'movement',movement);await chooseSetting(editor,'focus','hold-button');
 await editor.locator('.layout-workbench-actions').getByRole('button',{name:'Exit',exact:true}).click();
 await editor.waitFor({state:'detached'});
 report.settingsInputAudit=await page.evaluate(()=>{window.__th09SettingsInputAuditStop?.();return window.__th09SettingsInputAudit;});
 if(traceCheckboxWrites)report.checkboxWriteTrace=await page.evaluate(()=>window.__th09CheckboxWriteTraceStop?.());
 report.deviceBrowser=await page.evaluate(()=>({ua:navigator.userAgent,maxTouchPoints:navigator.maxTouchPoints,devicePixelRatio,
  screen:{width:screen.width,height:screen.height,orientation:screen.orientation?.type},
  viewport:{width:innerWidth,height:innerHeight,visualWidth:visualViewport?.width,visualHeight:visualViewport?.height},
  timeOrigin:performance.timeOrigin,visibility:document.visibilityState,focus:document.hasFocus()}));
 report.browserVersion=browser.version();
 stage('real-ui-ready',{platform,movement,cpuRate:rate});
 // The frozen ProductSettings chunk uses controller.launch: warnings precede
 // preparation and startPreparedRuntime runs automatically. Do not race its
 // transient Prepared panel with a second launch.
 const primary=page.getByRole('region',{name:'TH09 game launch',exact:true}).locator('[data-launch-primary]');
 await primary.waitFor({timeout:180000});
 report.launchFlow={name:'current-settings-start-game-auto-prepare',primaryLabel:(await primary.textContent()).trim()};
 assert.equal(report.launchFlow.primaryLabel,'Start game');
 await primary.click({timeout:180000});stage('real-product-start-requested');
 report.launchStateSamples=[];
 await acceptLaunchWarningsUntil(async()=>{
  const s=await currentState();
  if(report.launchStateSamples.length<2000)report.launchStateSamples.push(s);else report.launchStateSamplesOverflow=true;
  return s.runtime&&s.visible;
 });
 await until(async()=>await page.locator('.touch-runtime .layout-placed').count()>0,30000,'real loaded touch controller');
 report.launched=await currentState();assert.equal(report.launched.frames,1);assert.equal(report.launched.owned,true);assert.equal(report.launched.error,'');
 stage('real-native-launched',{epoch:report.launched.epoch});
 // Use the player's real touch path: a retained Launcher button can own
 // keyboard focus. Title/demo frames alone do not qualify human gameplay.
 const gameStart=performance.now();report.menuNavigation=[];
 while(performance.now()-gameStart<60000){
  const s=await currentState();if(s.touch?.[0]===1&&s.touch?.[1]===1&&s.title?.[1]===0)break;
  assert.equal(s.error,'');assert.notEqual(s.touch?.[0],3,'replay/demo is not a playable input target');
  if(s.title?.[1]===1&&s.title?.[3]===1&&s.title?.[5]>=10||s.touch?.[0]===2){
   if(s.title?.[1]===1)assert.ok([1,2,3].includes(s.title[2]),'expected Story main/difficulty/character menu');
   assert.ok(report.menuNavigation.length<40,'bounded real title/dialogue taps');
   const geometry=await page.evaluate(movement=>{
    const frame=document.querySelector('[data-runtime-host] iframe'),w=frame.contentWindow;
    const direct=document.querySelector('.touch-runtime > div[aria-hidden="true"].touch-none');
    let b;
    if(movement==='touch'&&direct)b=direct.getBoundingClientRect();
    else {const f=frame.getBoundingClientRect(),c=w.document.querySelector('#canvas').getBoundingClientRect();
     b={x:f.x+c.x*f.width/w.innerWidth,y:f.y+c.y*f.height/w.innerHeight,width:c.width*f.width/w.innerWidth,height:c.height*f.height/w.innerHeight};}
    const point={x:b.x+b.width*.5,y:b.y+b.height*.48},hit=document.elementFromPoint(point.x,point.y);
    return {rect:{x:b.x,y:b.y,width:b.width,height:b.height},point,
     targetOwned:movement==='touch'?hit===direct:hit===frame,
     target:{tag:hit?.tagName,className:typeof hit?.className==='string'?hit.className:null},
     viewport:{width:innerWidth,height:innerHeight,visualWidth:visualViewport?.width,visualHeight:visualViewport?.height,scale:visualViewport?.scale},
     focus:{parent:document.hasFocus(),child:w.document.hasFocus(),parentActive:document.activeElement?.tagName,childActive:w.document.activeElement?.tagName}};
   },movement);
   assert.equal(geometry.targetOwned,true,'menu tap must hit the real input surface, not a HUD control');
   const at=performance.now(),row={before:s,geometry,firstInjection:report.injections.length};report.menuNavigation.push(row);
   await input('touchStart',[{id:3,...geometry.point,radiusX:4,radiusY:4,force:.8}],at);
   try{await delay(90);}finally{await input('touchEnd',[],at+90);}
   row.afterLastInjection=report.injections.length;
   await delay(240);
  }else await delay(100);
 }
 report.gameReady=await currentState();
 assert.equal(report.gameReady.touch?.[0],1,'real playable touch context');assert.equal(report.gameReady.touch?.[1],1);
 report.gameGpu=await bounded(page.evaluate(({expectedEpoch,expectedHref})=>{
  const observedAt=performance.now(),frames=document.querySelectorAll('[data-runtime-host] iframe'),f=frames[0],w=f?.contentWindow;
  try{
   const runtime=w?.__th09Runtime,core=runtime?.core,actualEpoch=Number(new URL(w.location.href).searchParams.get('runtimeEpoch'));
   if(frames.length!==1||!f?.isConnected||f.contentDocument!==w?.document||!core||w.Module!==core||actualEpoch!==expectedEpoch||w.location.href!==expectedHref)
    return {observedAt,available:false,reason:'exact current game Runtime identity unavailable'};
   const canvas=w.document.querySelector('#canvas');
   if(!canvas||core.canvas!==canvas)return {observedAt,available:false,reason:'actual core canvas identity unavailable'};
   const gl=canvas.getContext('webgl2');
   if(!gl)return {observedAt,available:false,reason:'existing game WebGL2 context unavailable'};
   if(gl.canvas!==canvas)return {observedAt,available:false,reason:'game context canvas identity mismatch'};
   const ext=gl.getExtension('WEBGL_debug_renderer_info');
   const renderer=gl.getParameter(ext?ext.UNMASKED_RENDERER_WEBGL:gl.RENDERER),vendor=gl.getParameter(ext?ext.UNMASKED_VENDOR_WEBGL:gl.VENDOR);
   const software=/swiftshader|llvmpipe|microsoft basic render/i.test(renderer+' '+vendor);
   const nativeD3D11Qualified=!!ext&&!software&&/Direct3D11|D3D11/i.test(renderer)&&/Intel|NVIDIA|AMD|ATI|Qualcomm/i.test(renderer+' '+vendor);
   return {observedAt,available:true,expectedEpoch,actualEpoch,frameCount:frames.length,coreCanvasMatches:true,contextCanvasMatches:true,
    debugRendererAvailable:!!ext,renderer,vendor,software,nativeD3D11Qualified,
    scope:'The existing actual game canvas before observation; flags alone do not qualify the backend'};
  }catch(error){return {observedAt,available:false,reason:String(error)};}
 },{expectedEpoch:report.launched.epoch,expectedHref:report.launched.href}),10000,'actual game GPU identity');
 if(desktopBrowserProfile==='pinned-native')assert.equal(report.gameGpu.nativeD3D11Qualified,true,'actual hardware D3D11 evidence required for pinned native control');
 await bounded(page.evaluate('window.__th09ManagedTouchIntegrationProbeV1=('+installManagedTouchLauncherProbe.toString()+')({capacity:30000,label:'+JSON.stringify(label)+',expectedEpoch:'+report.launched.epoch+'}); null'),10000,'probe installation');
 probeInstalled=true;stage('actual-gameplay-observation-start');
 const surface=movement==='touch'?page.locator('.touch-runtime > div[aria-hidden="true"].touch-none'):page.locator('.touch-runtime .layout-joystick.layout-placed');
 report.geometrySamples=[];
 async function captureGeometry(phase){
  const rect=await surface.boundingBox();assert.ok(rect&&rect.width>20&&rect.height>20,'actual touch target geometry');
  const viewport=await page.evaluate(()=>({width:innerWidth,height:innerHeight,visualWidth:visualViewport?.width,visualHeight:visualViewport?.height,
   offsetLeft:visualViewport?.offsetLeft,offsetTop:visualViewport?.offsetTop,scale:visualViewport?.scale,orientation:screen.orientation?.type,at:performance.now()}));
  report.geometrySamples.push({phase,nodeAt:performance.now(),rect,viewport});return rect;
 }
 let rect=await captureGeometry('before-baseline');
 report.surface=rect;
 await probePhase('baseline-gesture');
 report.baselineGesture=await trajectory(rect,1800,60);
 await captureGeometry('after-baseline');
 await delay(250);
 await probePhase('hud-focus');await tapHud('.touch-runtime .layout-focus.layout-placed',250);
 await probePhase('hud-fire');await tapHud('.touch-runtime .layout-fire.layout-placed',450);
 const arm=await control('arm',{label:'active-native-resource-backlog',maxWaitMs:30000});
 const gateId=arm.probe.gateId;report.resourceProbe={arm,request:'managed-touch-resource-'+label};
 await probePhase('queue-request');
 await page.evaluate(({resource,request})=>{
  const f=document.querySelector('[data-runtime-host] iframe'),w=f.contentWindow;
  const epoch=Number(new URL(w.location.href).searchParams.get('runtimeEpoch'));
  w.postMessage({protocol:'eagler-touhou/1',game:'th09',epoch,request,command:'resources',resources:[resource]},location.origin);
 },{resource:arm.resource,request:report.resourceProbe.request});
 try{
  await until(async()=>{const s=await control('status');return s.active?.requests?.length?s:false;},10000,'controlled real font request waiting');
  report.resourceProbe.pendingState=await control('status');
  rect=await captureGeometry('before-queue-held');
  await probePhase('queue-held');
  report.queueGesture=await trajectory(rect,2200,120);
  await captureGeometry('after-queue-held');
  await delay(200);
  report.resourceProbe.beforeReleaseState=await currentState();
  report.resourceProbe.beforeReleaseEvents=await page.evaluate(()=>window.__th09HostedEvents);
  // Capture only the sequence boundary/counters while the game continues.
  // The matching immutable raw prefix is validated after observation ends.
  report.resourceProbe.beforeReleaseProbe=await snapshot(false);
  report.resourceProbe.beforeReleaseThroughSeq=report.resourceProbe.beforeReleaseProbe.coverage.lastSeq;
  assert.ok(Number.isSafeInteger(report.resourceProbe.beforeReleaseThroughSeq),'retained pre-release input sequence boundary');
  assert.equal(report.resourceProbe.beforeReleaseEvents.some(x=>x.request===report.resourceProbe.request),false,'resource command remains pending while input runs');
 }finally{report.resourceProbe.release=await control('release',{gateId});}
 await until(async()=>page.evaluate(request=>window.__th09HostedEvents.some(x=>x.request===request),report.resourceProbe.request),10000,'resource ACK after release');
 rect=await captureGeometry('before-after-resource');
 await probePhase('fresh-after-resource');
 report.afterResourceGesture=await trajectory(rect,900,60);await delay(300);
 await captureGeometry('after-after-resource');
 report.finalState=await currentState();
 // Dispose before the single full export so copying/serialization cannot
 // insert a large diagnostic pause into the observed gameplay interval.
 await disposeProbe('final disposed probe export');
 report.resourceProbe.inputEvidence=inputEvidence(report.probe,'queue-held',report.resourceProbe.beforeReleaseThroughSeq);
 report.inputEvidence=['baseline-gesture','queue-held','fresh-after-resource'].map(name=>inputEvidence(report.probe,name));
 report.runtimeEvents=await page.evaluate(()=>window.__th09HostedEvents);
 report.resourceProbe.finalStatus=await control('status');
 await page.screenshot({path:resolve(base,'gameplay.png'),fullPage:false,timeout:10000});
 report.screenshot=identity(resolve(base,'gameplay.png'));
 report.goals={singleCurrentNativeRuntime:report.finalState.frames===1&&report.finalState.owned&&report.finalState.runtime&&
   report.finalState.epoch===report.launched.epoch&&report.finalState.href===report.launched.href,
  originalOwnerAndHooksPreserved:report.probe.owner.valid&&!report.probe.owner.invalidation&&report.probe.hooksAtDisposal.every(row=>row.stillOurs)&&report.probe.coverage.rawComplete,
  originalInputReachesNativeDuringResourceWait:!!report.resourceProbe.inputEvidence,
  realGameStillRunning:report.finalState.title?.[1]===0&&!report.finalState.error,
  resourcesAwaitedUntilExplicitRelease:!report.resourceProbe.beforeReleaseEvents.some(x=>x.request===report.resourceProbe.request),
  resourceEventuallyAcknowledged:report.runtimeEvents.some(x=>x.request===report.resourceProbe.request&&x.ok===true),
  noUnhandledPageError:report.pageErrors.length===0};
 assert.ok(Object.values(report.goals).every(Boolean),'host integration goals');
 // Cadence distributions remain diagnostic evidence, not a claimed 60 FPS gate.
 report.passed=true;stage('capture-complete',{platform,movement});
}catch(error){
 report.passed=false;report.failure={message:String(error),stack:error?.stack||null,cause:error?.cause?String(error.cause):null};stage('retained-failure',{error:String(error)});
 if(page&&ownPage){
  // Freeze the observation before failure-state reads, screenshots or raw
  // export can add their own long pauses to the retained gameplay interval.
  if(probeInstalled)try{await disposeProbe('failed observation disposed probe export');}catch(e){report.failureProbeError=String(e);}
  try{report.runtimeEvents=await bounded(page.evaluate(()=>window.__th09HostedEvents||null),10000,'retained Runtime protocol events');}catch(e){report.runtimeEventsError=String(e);}
  try{report.failureState=await currentState();}catch(e){report.failureStateError=String(e);}
  try{report.failureUi=await bounded(page.evaluate(()=>({url:location.href,alerts:[...document.querySelectorAll('[role="alert"]')].map(x=>x.textContent),
   status:[...document.querySelectorAll('[role="status"]')].map(x=>x.textContent),buttons:[...document.querySelectorAll('button')].filter(x=>x.getBoundingClientRect().width>0).map(x=>x.textContent).slice(0,60),
   settingsInputAudit:window.__th09SettingsInputAudit,
   switches:[...document.querySelectorAll('input[type="checkbox"]')].slice(0,30).map(x=>({id:x.id,checked:x.checked,ariaChecked:x.getAttribute('aria-checked')})),
   selects:[...document.querySelectorAll('select')].slice(0,30).map(x=>({id:x.id,value:x.value,disabled:x.disabled,options:[...x.options].map(o=>({value:o.value,text:o.textContent}))}))})),10000,'failure UI snapshot');}catch(e){report.failureUiError=String(e);}
  try{report.failureScreenshotTiming={calledAt:performance.now()};await page.screenshot({path:resolve(base,'failure.png'),timeout:10000});report.failureScreenshotTiming.returnedAt=performance.now();report.failureScreenshot=identity(resolve(base,'failure.png'));}catch(e){report.screenshotError=String(e);}
 }
}finally{
 closing=true;
 if(traceCheckboxWrites&&page&&ownPage)try{report.checkboxWriteTrace=await bounded(page.evaluate(()=>window.__th09CheckboxWriteTraceStop?.()),5000,'checkbox observation cleanup');
  if(report.checkboxWriteTrace?.restored===false)report.checkboxTraceCleanupError='Captured checked accessor was replaced; did not overwrite the new owner';
 }catch(error){report.checkboxTraceCleanupError=String(error);}
 if(probeInstalled&&page)try{
  await disposeProbe('final cleanup probe disposal');
 }catch(e){report.disposeError=String(e);}
 if(page&&ownPage)try{await bounded(page.close({runBeforeUnload:false}),5000,'owned page close');}catch(e){report.pageCloseError=String(e);}
 if(browser)try{await bounded(browser.close(),5000,'browser close or Android CDP disconnect');}catch(e){report.browserCloseError=String(e);}
 if(ownedPagePromise)try{await bounded(ownedPagePromise.catch(()=>null),5000,'owned Android page request settled after disconnect');}catch(e){report.pageCreationSettleError=String(e);}
 if(report.android?.pageCreation&&report.android.pageCreation.outcome!=='created')report.pageCreationUncertain=report.android.pageCreation.outcome;
 if(reversePort!==null){
  const current=adb(['reverse','--list'],{allowFailure:true});
  if(current.split(/\r?\n/).some(line=>{const fields=line.trim().split(/\s+/);return fields.at(-2)==='tcp:'+reversePort&&fields.at(-1)==='tcp:'+reversePort;}))
   adb(['reverse','--remove','tcp:'+reversePort],{allowFailure:true});
  else report.reverseCleanupSkipped='exact owned tuple no longer present';
 }
 if(forwardPort!==null){
  const current=adb(['forward','--list'],{allowFailure:true});
  if(current.split(/\r?\n/).some(line=>{const fields=line.trim().split(/\s+/);return fields[0]===deviceId&&fields[1]==='tcp:'+forwardPort&&fields[2]==='localabstract:chrome_devtools_remote';}))
   adb(['forward','--remove','tcp:'+forwardPort],{allowFailure:true});
  else report.forwardCleanupSkipped='exact owned tuple no longer present';
 }
 if(remote&&adbPath&&deviceId){
  report.androidMappingsAfter={forward:adb(['forward','--list'],{allowFailure:true}),reverse:adb(['reverse','--list'],{allowFailure:true})};
  report.androidMappingCleanup={
   forwardRemoved:forwardPort===null||!report.androidMappingsAfter.forward.split(/\r?\n/).some(line=>{const p=line.trim().split(/\s+/);return p[0]===deviceId&&p[1]==='tcp:'+forwardPort&&p[2]==='localabstract:chrome_devtools_remote';}),
   reverseRemoved:reversePort===null||!report.androidMappingsAfter.reverse.split(/\r?\n/).some(line=>{const p=line.trim().split(/\s+/);return p.at(-2)==='tcp:'+reversePort&&p.at(-1)==='tcp:'+reversePort;}),
   commandsSucceeded:report.deviceCommands.every(row=>row.status===0&&!row.error)
  };
 }
 if(server)try{report.serverFinal=await bounded(server.stop(),30000,'server stop and input audit');}catch(e){report.serverStopError=String(e);}
 const cleanupErrors=['disposeError','pageCloseError','browserCloseError','serverStopError','latePageCloseError','pageCreationSettleError','pageCreationUncertain','checkboxTraceCleanupError'].filter(key=>report[key]);
 report.cleanupErrors=cleanupErrors;
 if(cleanupErrors.length||report.serverFinal?.clean===false||report.androidMappingCleanup&&!Object.values(report.androidMappingCleanup).every(Boolean))report.passed=false;
 report.sourceAfter=scriptPaths.map(identity);report.sourceStable=JSON.stringify(report.sourceBefore)===JSON.stringify(report.sourceAfter);
 report.endedAt=new Date().toISOString();
 writeFileSync(reportPath,JSON.stringify(report,null,2)+'\n',{flag:'wx'});
 console.log(JSON.stringify({report:identity(reportPath),passed:report.passed,sourceStable:report.sourceStable,failure:report.failure?.message||null}));
 if(!report.passed||!report.sourceStable)process.exitCode=1;
}
