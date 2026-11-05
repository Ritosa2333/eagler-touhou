// Real Android Chrome, owned target, ordinary React / Package Store / native Runtime.
// No Playwright initialization, synthetic DOM input, native stepping, or font wait.
// Run from this worktree. Required: RUN_LABEL, TH09_HOSTED_SERVER_OPTIONS,
// TH09_ADB_PATH, TH09_ANDROID_SERIAL, TH09_ANDROID_DEPENDENCIES_ROOT.
// TH09_HOSTED_MOVEMENT is touch (default) or joystick-free; one startup profile.
// The planned CDP gesture rates are not physical sampling rates or a FPS claim.
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {existsSync,mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {isAbsolute,resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {performance} from 'node:perf_hooks';
import {setTimeout as delay} from 'node:timers/promises';
import {startManagedTouchLauncherServer} from './managed-touch-launcher-server.mjs';
import {installManagedTouchLauncherProbe} from './managed-touch-launcher-probe.mjs';
import {openOwnedAndroidSession} from './managed-touch-owned-android-session.mjs';

const root=process.cwd(),label=process.env.RUN_LABEL;
assert.match(label||'',/^[a-z0-9_-]+$/,'explicit exclusive RUN_LABEL');
const base=resolve(root,'web/artifacts/multiplayer-tests',label),reportPath=base+'-report.json';
assert.equal(existsSync(base),false,'run directory must be new');
assert.equal(existsSync(reportPath),false,'report must be new');
mkdirSync(base);
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const identity=path=>{const bytes=readFileSync(path);return {path,bytes:bytes.length,sha256:sha(bytes)};};
const scriptPaths=['managed-touch-launcher-android.mjs','managed-touch-launcher-server.mjs',
 'managed-touch-launcher-probe.mjs','managed-touch-owned-android-session.mjs']
 .map(name=>resolve(root,'web/tests/multiplayer',name));
const report={schema:'th09-managed-touch-launcher-android/v1',runLabel:label,startedAt:new Date().toISOString(),
 scope:'Owned real Android Chrome target, normal single-player React hosted-development Runtime. Not immutable publication, multiplayer, CPU4, physical-finger latency, or no-hitch performance acceptance.',
 cpuRate:1,clock:{nodeTimeOrigin:performance.timeOrigin,dispatch:'Node performance.now; page and child clocks retain their own origins'},
 sourceBefore:[],stages:[],injections:[],uiActions:[],preferenceChecks:[],menuNavigation:[],launchStateSamples:[],
 geometrySamples:[],phaseChecks:[],gestures:[],cleanupErrors:[],passed:false};
let server,session,probeInstallAttempted=false,probeDisposed=false,inputOutcomeUnknown=false;
let movement,activePhase='launcher-ui',optionsPath;
const sourcePaths=[...scriptPaths],acknowledgedWarnings=new Set(),dismissedNotices=new Set();
const errorRecord=error=>({message:String(error),stack:error?.stack||null,cause:error?.cause?String(error.cause):null});
function stage(name,extra={}){const row={name,nodeAt:performance.now(),...extra};report.stages.push(row);console.log(JSON.stringify({stage:name,...extra}));}
async function evaluateExpression(expression,timeoutMs=10000){
 const response=await session.send('Runtime.evaluate',{expression,awaitPromise:true,returnByValue:true},timeoutMs);
 if(response.exceptionDetails)throw Error('Owned Runtime.evaluate exception: '+JSON.stringify(response.exceptionDetails));
 if(response.result?.subtype==='error')throw Error('Owned Runtime.evaluate error: '+response.result.description);
 assert.ok(response.result,'CDP evaluation must return its actual remote result');
 return response.result.value;
}
async function evaluate(fn,arg,timeoutMs=10000){
 return evaluateExpression('('+fn.toString()+')('+JSON.stringify(arg===undefined?null:arg)+')',timeoutMs);
}
async function until(check,timeoutMs,name,pollMs=250){
 const start=performance.now();
 while(performance.now()-start<timeoutMs){const result=await check();if(result)return result;await delay(pollMs);}
 throw Error('Timed out: '+name);
}
async function requestJSON(url){
 const response=await fetch(url,{signal:AbortSignal.timeout(10000)});
 assert.equal(response.status,200,'real Host manifest HTTP status');
 return response.json();
}
function installAudit({origin}){
 if(location.origin!==origin)return;
 const audit={url:location.href,timeOrigin:performance.timeOrigin,protocol:[],ui:[],errors:[],lifecycle:[],overflow:false};
 window.__th09OwnedAndroidAuditV1=audit;
 const add=(kind,row,limit)=>{if(audit[kind].length>=limit){audit.overflow=true;return;}audit[kind].push({at:performance.now(),...row});};
 for(const type of ['error','unhandledrejection'])addEventListener(type,event=>{
  add('errors',{type,message:String(event.message??event.reason??'unknown').slice(0,4096),
   filename:event.filename??null,line:event.lineno??null,column:event.colno??null},100);
 });
 for(const type of ['blur','focus','pagehide','pageshow','visibilitychange'])addEventListener(type,event=>{
  const windowEvent=event.target===window,documentEvent=event.target===document;
  if(!windowEvent&&!documentEvent)return;
  add('lifecycle',{type,windowEvent,documentEvent,visible:document.visibilityState,focused:document.hasFocus()},200);
 },{capture:true,passive:true});
 if(window!==top){
  const commands=new Set(['configure','launch','stop','resources','network-open','network-cancel']);
  addEventListener('message',event=>{
   const m=event.data,epoch=Number(new URL(location.href).searchParams.get('runtimeEpoch'));
   if(event.source!==parent||event.origin!==origin||m?.protocol!=='eagler-touhou/1'||m.game!=='th09'||
      !Number.isSafeInteger(epoch)||epoch<1||m.epoch!==epoch||!commands.has(m.command))return;
   add('protocol',{epoch,command:m.command,request:m.request??null,netplayMode:m.options?.netplayMode??null},500);
  });
  return;
 }
 addEventListener('message',event=>{
  const frames=document.querySelectorAll('[data-runtime-host] iframe'),m=event.data;
  if(frames.length!==1||event.source!==frames[0].contentWindow||event.origin!==origin||
     m?.protocol!=='eagler-touhou/1'||m.game!=='th09')return;
  const epoch=Number(new URL(frames[0].contentWindow.location.href).searchParams.get('runtimeEpoch'));
  if(!Number.isSafeInteger(epoch)||epoch<1||m.epoch!==epoch)return;
  add('protocol',{epoch,event:m.event??null,request:m.request??null,ok:m.ok??null,
   error:m.error==null?null:String(m.error).slice(0,4096),loaded:m.loaded??null,total:m.total??null,path:m.path??null},2000);
 });
 // No MOVE observer or asynchronous follow-up work here; the actual probe owns MOVE evidence.
 for(const type of ['pointerdown','pointerup','pointercancel','click','input','change','keydown','keyup'])
  document.addEventListener(type,event=>{
   const element=event.target;
   add('ui',{type,trusted:event.isTrusted,eventTimeStamp:event.timeStamp,tag:element?.tagName??null,id:element?.id??null,
    pointerType:event.pointerType??null,pointerId:event.pointerId??null,x:event.clientX??null,y:event.clientY??null,
    checked:typeof element?.checked==='boolean'?element.checked:null,value:typeof element?.value==='string'?element.value.slice(0,120):null,
    key:event.key??null,code:event.code??null,defaultPrevented:event.defaultPrevented},1200);
  },{capture:true,passive:true});
}
function readUI(spec){
 const normalize=value=>String(value??'').replace(/\s+/g,' ').trim();
 const named=element=>[element.getAttribute('aria-label'),
  (element.getAttribute('aria-labelledby')||'').split(/\s+/).map(id=>document.getElementById(id)?.textContent||'').join(' '),
  ...Array.from(element.labels||[],label=>label.textContent),
  ...Array.from(element.labels||[],label=>label.querySelector('.game-settings-option-copy > span')?.textContent),element.textContent].map(normalize);
 const scopes=spec.scope?Array.from(document.querySelectorAll(spec.scope)):[document];
 const visible=element=>{
  const rect=element.getBoundingClientRect(),style=getComputedStyle(element);
  return rect.width>0&&rect.height>0&&style.display!=='none'&&style.visibility!=='hidden'&&
   !element.closest('[hidden],[inert],[aria-hidden="true"],[data-state="closed"]');
 };
 const elements=scopes.flatMap(scope=>Array.from(scope.querySelectorAll(spec.selector)))
  .filter(element=>(spec.attached||visible(element))&&(!spec.name||named(element).includes(spec.name)));
 const rows=elements.map(element=>{
  let target=spec.trigger?element.parentElement?.querySelector('.mizuki-select-trigger'):element;
  if(!target)throw Error('Real select trigger missing');
  let rect=target.getBoundingClientRect();
  if(!spec.trigger&&element instanceof HTMLInputElement&&rect.width<2&&element.labels?.length===1){
   target=element.labels[0];rect=target.getBoundingClientRect();
  }
  const viewport={width:innerWidth,height:innerHeight,visualWidth:visualViewport?.width??null,
   visualHeight:visualViewport?.height??null,offsetLeft:visualViewport?.offsetLeft??null,
   offsetTop:visualViewport?.offsetTop??null,scale:visualViewport?.scale??null};
  let clip={left:0,top:0,right:Math.min(innerWidth,visualViewport?.width??innerWidth),
   bottom:Math.min(innerHeight,visualViewport?.height??innerHeight)},scroll=null;
  for(let parent=target.parentElement;parent;parent=parent.parentElement){
   const style=getComputedStyle(parent),box=parent.getBoundingClientRect();
   if(/auto|scroll|hidden|clip/.test(style.overflowY)){
    clip.top=Math.max(clip.top,box.top);clip.bottom=Math.min(clip.bottom,box.bottom);
    if(!scroll&&/auto|scroll/.test(style.overflowY)&&parent.scrollHeight>parent.clientHeight+2)
     scroll={rect:{x:box.x,y:box.y,width:box.width,height:box.height},top:parent.scrollTop,max:parent.scrollHeight-parent.clientHeight};
   }
   if(/auto|scroll|hidden|clip/.test(style.overflowX)){
    clip.left=Math.max(clip.left,box.left);clip.right=Math.min(clip.right,box.right);
   }
  }
  if(!scroll&&document.scrollingElement?.scrollHeight>innerHeight+2)
   scroll={rect:{x:0,y:0,width:innerWidth,height:innerHeight},top:scrollY,max:document.scrollingElement.scrollHeight-innerHeight};
  const left=Math.max(rect.left,clip.left),right=Math.min(rect.right,clip.right),
   top=Math.max(rect.top,clip.top),bottom=Math.min(rect.bottom,clip.bottom);
  const point=right-left>=2&&bottom-top>=2?{x:(left+right)/2,y:(top+bottom)/2}:null;
  const hit=point?document.elementFromPoint(point.x,point.y):null;
  return {tag:element.tagName,id:element.id,name:named(element),text:normalize(element.textContent),
   checked:typeof element.checked==='boolean'?element.checked:null,value:typeof element.value==='string'?element.value:null,
   disabled:!!element.disabled||element.getAttribute('aria-disabled')==='true',
   expanded:element.getAttribute('aria-expanded'),pressed:element.getAttribute('aria-pressed'),
   warning:element.getAttribute('data-launch-warning'),rect:{x:rect.x,y:rect.y,width:rect.width,height:rect.height},
   clip,scroll,point,hitOwned:!!hit&&(hit===target||target.contains(hit)),
   hit:hit?{tag:hit.tagName,id:hit.id,className:typeof hit.className==='string'?hit.className:null}:null,
   viewport,at:performance.now()};
 });
 return {count:rows.length,rows,locale:document.documentElement.getAttribute('data-ui-locale')};
}

async function input(type,points,plannedAt,reason){
 assert.equal(inputOutcomeUnknown,false,'no Input command after an uncertain delivery');
 assert.ok(points.every(point=>Number.isFinite(point.x)&&Number.isFinite(point.y)));
 const row={seq:report.injections.length,phase:activePhase,reason,type,points:points.map(point=>({...point})),
  plannedAt,calledAt:performance.now(),returnedAt:null,outcome:null,error:null};
 report.injections.push(row);
 try{await session.send('Input.dispatchTouchEvent',{type,touchPoints:points},10000);row.outcome='CDP replied; DOM/native receipt evaluated separately';}
 catch(error){inputOutcomeUnknown=true;row.error=errorRecord(error);row.outcome='unknown; no retry or subsequent Input';throw error;}
 finally{row.returnedAt=performance.now();}
}
const point=(x,y,id=1)=>({id,x,y,radiusX:4,radiusY:4,force:.8});
async function tapPoint(location,reason,holdMs=90){
 const start=performance.now();
 await input('touchStart',[point(location.x,location.y,3)],start,reason);
 await delay(holdMs);
 await input('touchEnd',[],start+holdMs,reason);
 await delay(140);
}
async function dismissKnownNotices(){
 const choices=[
  {kind:'first-use-info',selector:'.notice-swipe-close'},
  {kind:'site-info',scope:'[data-site-notice]',selector:'button',name:'Close'},
  {kind:'touch-entry-help',scope:'.touch-help-bar',selector:'button[title="Close"]'}
 ];
 for(const spec of choices){
  if(dismissedNotices.has(spec.kind))continue;
  const found=await evaluate(readUI,spec);
  assert.ok(found.count<=1,'unique known informational notice');
  if(found.count===1&&found.rows[0].hitOwned&&!found.rows[0].disabled){
   const row={kind:'dismiss-known-info',spec,target:found.rows[0],firstInjection:report.injections.length};
   report.uiActions.push(row);await tapPoint(row.target.point,spec.kind);row.afterLastInjection=report.injections.length;
   dismissedNotices.add(spec.kind);return true;
  }
 }
 return false;
}
async function swipeToReveal(target,name){
 const scroll=target.scroll;assert.ok(scroll,'off-screen UI target requires an observed scroll container: '+name);
 const viewport=target.viewport,b=scroll.rect;
 const left=Math.max(0,b.x),right=Math.min(viewport.visualWidth??viewport.width,b.x+b.width),
  top=Math.max(0,b.y),bottom=Math.min(viewport.visualHeight??viewport.height,b.y+b.height);
 assert.ok(right-left>30&&bottom-top>80,'usable observed UI scrolling region');
 const upward=target.rect.y>=target.clip.bottom,x=left+(right-left)*.65;
 const y0=top+(bottom-top)*(upward ? .78 : .25),y1=top+(bottom-top)*(upward ? .25 : .78),start=performance.now();
 await input('touchStart',[point(x,y0,12)],start,'real UI scroll '+name);
 for(let n=1;n<=10;n++){
  const planned=start+n*30;if(performance.now()<planned)await delay(planned-performance.now());
  await input('touchMove',[point(x,y0+(y1-y0)*n/10,12)],planned,'real UI scroll '+name);
 }
 await input('touchEnd',[],performance.now(),'real UI scroll '+name);await delay(200);
}
async function uiTarget(spec,timeoutMs=15000){
 let scrolls=0;
 return until(async()=>{
  const found=await evaluate(readUI,spec);
  assert.ok(found.count<=1,'ambiguous real UI target '+JSON.stringify(spec));
  if(found.count===0){await dismissKnownNotices();return false;}
  const target=found.rows[0];
  if(target.hitOwned&&!target.disabled)return target;
  if(await dismissKnownNotices())return false;
  if(!target.point&&target.scroll&&scrolls<8){scrolls++;await swipeToReveal(target,spec.name||spec.selector);}
  return false;
 },timeoutMs,'real visible input target '+JSON.stringify(spec));
}
async function tapUI(spec,reason,holdMs=90){
 const target=await uiTarget(spec);
 const row={kind:'trusted-CDP-touch',reason,spec,target,firstInjection:report.injections.length};report.uiActions.push(row);
 await tapPoint(target.point,reason,holdMs);row.afterLastInjection=report.injections.length;
 return target;
}
async function attachedUI(spec){
 const found=await evaluate(readUI,{...spec,attached:true});assert.equal(found.count,1,'one attached real control');
 return found.rows[0];
}
async function chooseSetting(scope,suffix,value){
 const spec={scope,selector:'select[id$="-'+suffix+'"]',attached:true};
 const before=await attachedUI(spec),row={scope,suffix,before:before.value,requested:value,disabled:before.disabled,changed:false};
 report.preferenceChecks.push(row);
 if(before.value!==value){
  assert.equal(before.disabled,false,'requested real preference must be enabled');
  await tapUI({...spec,trigger:true},'open '+suffix);
  await tapUI({selector:'.mizuki-select-menu [role="option"][data-value="'+value+'"]'},'choose '+suffix+' '+value);
  if(suffix==='movement'&&value==='joystick-free'){
   const dialog={selector:'[role="dialog"]',name:'Confirm'};
   await until(async()=>{
    const found=await evaluate(readUI,dialog);
    if(found.count===0)return false;
    assert.equal(found.count,1);assert.ok(found.rows[0].text.includes('The unrestricted-direction stick records replays in a new format'));
    report.touchModeWarning=found.rows[0].text;return true;
   },10000,'real free-stick replay-format warning');
   await tapUI({scope:'[role="dialog"]',selector:'button',name:'Enable'},'accept documented free-stick warning');
  }
  await until(async()=>((await attachedUI(spec)).value===value),10000,'preference committed '+suffix);
  row.changed=true;
 }
 row.after=(await attachedUI(spec)).value;assert.equal(row.after,value);
}
function runtimeState(){
 const sampledAt=performance.now(),timeOrigin=performance.timeOrigin,frames=document.querySelectorAll('[data-runtime-host] iframe');
 if(frames.length!==1)return {frames:frames.length,sampledAt,timeOrigin};
 const frame=frames[0],child=frame.contentWindow,href=child.location.href,runtime=child.__th09Runtime,core=runtime?.core||child.Module;
 const ints=(name,n)=>{if(typeof core?.[name]!=='function'||!core.HEAPU8?.buffer)return null;const pointer=core[name]();return Array.from(new Int32Array(core.HEAPU8.buffer,pointer,n));};
 return {frames:frames.length,sampledAt,timeOrigin,href,owned:frame.isConnected&&frame.contentDocument===child.document,
  visible:frame.closest('[data-runtime-host]')?.getAttribute('aria-hidden')==='false',
  parentVisibility:document.visibilityState,parentFocus:document.hasFocus(),parentActive:document.activeElement?.tagName,
  documentVisibility:child.document.visibilityState,documentFocus:child.document.hasFocus(),childActive:child.document.activeElement?.tagName,
  runtimeSampledAt:child.performance.now(),runtimeTimeOrigin:child.performance.timeOrigin,
  startupMarks:child.performance.getEntriesByType('mark').filter(entry=>['eagler-startup-image','eagler-startup-menu-ready'].includes(entry.name))
   .map(entry=>({name:entry.name,startTime:entry.startTime})),
  runtime:!!runtime,moduleMatchesRuntime:!!runtime&&child.Module===runtime.core,
  epoch:Number(new URL(href).searchParams.get('runtimeEpoch')),title:ints('_th09_title_status',8),
  session:ints('_th09_session_status',8),touch:ints('_th09_touch_state',4),
  options:core?.eaglerOptions??null,audioState:core?.SDL3?.audioContext?.state??null,
  error:child.document.querySelector('#error')?.textContent||'',overlay:!!document.querySelector('.touch-runtime')};
}
async function currentState(){
 const calledAt=performance.now(),state=await evaluate(runtimeState,null,15000);
 return {...state,calledAt,returnedAt:performance.now()};
}
function geometry({movement,menu=false}){
 const frames=document.querySelectorAll('[data-runtime-host] iframe');if(frames.length!==1)throw Error('one current Runtime iframe required');
 const frame=frames[0],child=frame.contentWindow;
 if(!frame.isConnected||frame.contentDocument!==child.document)throw Error('Runtime document owner changed');
 const selector=movement==='touch'?'.touch-runtime > div[aria-hidden="true"].touch-none':'.touch-runtime .layout-joystick.layout-placed';
 const surface=document.querySelector(selector);let rect,target;
 if(menu&&movement==='joystick-free'){
  const canvas=child.document.querySelector('#canvas');if(!canvas)throw Error('real Runtime canvas missing');
  const f=frame.getBoundingClientRect(),c=canvas.getBoundingClientRect();
  rect={x:f.x+c.x*f.width/child.innerWidth,y:f.y+c.y*f.height/child.innerHeight,
   width:c.width*f.width/child.innerWidth,height:c.height*f.height/child.innerHeight};target=frame;
 }else{
  if(!surface)throw Error('actual touch surface missing: '+selector);
  const b=surface.getBoundingClientRect();rect={x:b.x,y:b.y,width:b.width,height:b.height};target=surface;
 }
 const location={x:rect.x+rect.width*.5,y:rect.y+rect.height*.48},hit=document.elementFromPoint(location.x,location.y);
 return {rect,point:location,targetOwned:hit===target||(!menu&&target.contains(hit)),selector,menu,
  hit:hit?{tag:hit.tagName,className:typeof hit.className==='string'?hit.className:null}:null,
  viewport:{width:innerWidth,height:innerHeight,visualWidth:visualViewport?.width??null,visualHeight:visualViewport?.height??null,
   offsetLeft:visualViewport?.offsetLeft??null,offsetTop:visualViewport?.offsetTop??null,scale:visualViewport?.scale??null,
   orientation:screen.orientation?.type??null,devicePixelRatio},
  focus:{parent:document.hasFocus(),child:child.document.hasFocus(),parentActive:document.activeElement?.tagName,childActive:child.document.activeElement?.tagName},
  hostAt:performance.now(),hostTimeOrigin:performance.timeOrigin};
}
function assertPlayable(state){
 assert.equal(state.frames,1);assert.equal(state.owned,true);assert.equal(state.runtime,true);assert.equal(state.moduleMatchesRuntime,true);
 assert.equal(state.visible,true);assert.equal(state.error,'');assert.equal(state.touch?.[0],1,'human playable touch context');
 assert.equal(state.touch?.[1],1,'human input ready');assert.equal(state.title?.[1],0);assert.equal(state.session?.[6],0,'isReplay must be zero');
 assert.equal(state.options?.netplayMode,undefined,'ordinary single-player variant, no disguised multiplayer mode');
 assert.equal(state.options?.touchEnabled,true);assert.equal(state.options?.touchMovementMode,movement);
 assert.equal(state.options?.touchFocusMode,'hold-button','actual startup focus profile remains selected');
}
async function captureGeometry(name,menu=false){
 const calledAt=performance.now(),value=await evaluate(geometry,{movement,menu});
 const result={name,calledAt,returnedAt:performance.now(),...value};report.geometrySamples.push(result);
 assert.ok(value.rect.width>20&&value.rect.height>20,'real input geometry has usable size');
 assert.equal(value.targetOwned,true,'touch point must hit the current real input surface');
 return value;
}
async function probePhase(name,metadata={}){
 activePhase=name;
 const result=await evaluate(({name,metadata})=>window.__th09ManagedTouchIntegrationProbeV1.phase(name,metadata),{name,metadata});
 assert.equal(result.accepted,true,'retain current Runtime owner at phase boundary');
 return result;
}
async function lightProbe(name){
 const calledAt=performance.now();
 const snapshot=await evaluate(()=>window.__th09ManagedTouchIntegrationProbeV1.snapshot({includeRecords:false}));
 report.phaseChecks.push({name,calledAt,returnedAt:performance.now(),snapshot});
 assert.equal(snapshot.coverage.includedRecords,false);assert.equal(snapshot.records.length,0,'no full raw copy during gestures');
 assert.equal(snapshot.owner.valid,true);assert.equal(snapshot.owner.invalidation,null);
 assert.equal(snapshot.coverage.allHooksCurrent,true);assert.equal(snapshot.gettersCurrent,true);
 assert.equal(snapshot.counters.overwritten,0);assert.equal(snapshot.counters.observationErrors,0);
 assert.equal(snapshot.counters.payloadTruncations,0);
}

async function trajectory(name,durationMs,hz){
 assertPlayable(await currentState());
 const before=await captureGeometry('before-'+name);
 await probePhase(name,{plannedDurationMs:durationMs,plannedCDPHz:hz});
 const rect=before.rect,cx=rect.x+rect.width*.5,cy=rect.y+rect.height*.48;
 const sx=movement==='touch'?Math.min(55,rect.width*.14):rect.width*.29,
  sy=movement==='touch'?Math.min(35,rect.height*.08):rect.height*.29;
 const steps=Math.ceil(durationMs*hz/1000),start=performance.now(),firstInjection=report.injections.length;
 const row={name,plannedDurationMs:durationMs,plannedCDPHz:hz,plannedMoves:steps,firstInjection,start,
  rateMeaning:'Requested sequential CDP dispatch plan. Actual call/reply intervals and observed DOM/native events are retained separately.'};
 report.gestures.push(row);
 await input('touchStart',[point(cx,cy)],start,name);
 const vertices=[[0,0],[1,-1],[-1,-1],[-1,1],[0,0]];
 for(let n=1;n<=steps;n++){
  const planned=start+n*1000/hz;if(performance.now()<planned)await delay(planned-performance.now());
  assert.ok(performance.now()-start<20000,'bounded gesture dispatch; never skip or retry a MOVE');
  const u=n/steps*4,leg=Math.min(3,Math.floor(u)),v=u-leg,a=vertices[leg],b=vertices[leg+1];
  await input('touchMove',[point(cx+sx*(a[0]+(b[0]-a[0])*v),cy+sy*(a[1]+(b[1]-a[1])*v))],planned,name);
 }
 await input('touchEnd',[],performance.now(),name);
 row.finished=performance.now();row.afterLastInjection=report.injections.length;
 await captureGeometry('after-'+name);
 await delay(300);await lightProbe('after-'+name);
}
function inputEvidence(probe,name){
 const phase=probe.phases.find(row=>row.name===name);assert.ok(phase,'recorded real gesture phase');
 const rows=probe.records.filter(row=>row.phaseId===phase.id),command=movement==='touch'?'direct-touch':'touch-controls';
 const submitted=rows.filter(row=>row.kind==='postMessage-entry'&&row.command===command),
  delivered=rows.filter(row=>row.kind==='message-observed'&&row.command===command),
  native=rows.filter(row=>row.kind==='native-entry');
 assert.ok(rows.some(row=>row.kind==='pointer'&&row.type==='pointermove'&&row.isTrusted),'actual trusted touch MOVE observed');
 assert.ok(submitted.length>=10,'nonempty active input path');
 assert.deepEqual(delivered.map(row=>row.message),submitted.map(row=>row.message),'full submitted messages delivered once, in order');
 if(movement==='touch'){
  const calls=native.filter(row=>row.name==='_th09_touch');
  assert.deepEqual(calls.map(row=>row.args.slice(0,2)),submitted.map(row=>[
   ({down:0,move:1,up:2,cancel:2})[row.message.type],row.message.id]),'original native direct action/id order');
  assert.ok(calls.every(row=>row.args.every(Number.isFinite)),'finite original native direct arguments');
 }else{
  const calls=native.filter(row=>row.name==='_th09_touch_controls'||row.name==='_th09_touch_stick');
  assert.deepEqual(calls.map(row=>row.name),submitted.flatMap(()=>['_th09_touch_controls','_th09_touch_stick']),
   'each actual free-stick message retains original controls then stick order');
  assert.ok(calls.some(row=>row.name==='_th09_touch_stick'&&row.args.some(value=>value!==0)),'nonzero real free-stick axis reaches native');
 }
 const frames=rows.filter(row=>row.kind==='frame-after-original'&&row.state),first=frames[0],last=frames.at(-1);
 assert.ok(frames.length>=2,'existing native callbacks continue');
 assert.ok(frames.every(row=>row.state.session[6]===0&&row.state.touchState[0]!==3),'no attract/demo frame qualifies');
 assert.ok(last.state.session[1]>first.state.session[1],'new logical game frames during gesture');
 assert.ok(last.state.gameMetrics[9]>first.state.gameMetrics[9],'new native presentations during gesture');
 const positionRanges=[0,1].map(axis=>{
  const values=frames.map(row=>row.state.touchProbe.values[axis]);assert.ok(values.every(Number.isFinite));
  return {min:Math.min(...values),max:Math.max(...values)};
 });
 assert.ok(positionRanges.some(range=>range.max>range.min),'real player coordinates move');
 return {phase:name,phaseId:phase.id,counts:probe.byPhase.find(row=>row.id===phase.id),
  submitted:submitted.length,delivered:delivered.length,nativeCalls:native.length,frameSamples:frames.length,positionRanges,
  first:{atMs:first.sampledAtMs,session:first.state.session,gameMetrics:first.state.gameMetrics},
  last:{atMs:last.sampledAtMs,session:last.state.session,gameMetrics:last.state.gameMetrics},
  scope:'Observed host/native route and game progress, not a physical latency or uninterrupted cadence gate.'};
}
function captureAudit(){
 const frames=document.querySelectorAll('[data-runtime-host] iframe'),child=frames.length===1?frames[0].contentWindow:null;
 return {host:window.__th09OwnedAndroidAuditV1??null,child:child?.__th09OwnedAndroidAuditV1??null,
  ui:{url:location.href,alerts:Array.from(document.querySelectorAll('[role="alert"]'),node=>node.textContent),
   statuses:Array.from(document.querySelectorAll('[role="status"]'),node=>node.textContent),
   buttons:Array.from(document.querySelectorAll('button')).filter(node=>node.getBoundingClientRect().width>0).slice(0,80).map(node=>node.textContent),
   switches:Array.from(document.querySelectorAll('input[type="checkbox"]')).slice(0,30).map(node=>({id:node.id,checked:node.checked,ariaChecked:node.getAttribute('aria-checked')})),
   selects:Array.from(document.querySelectorAll('select')).slice(0,30).map(node=>({id:node.id,value:node.value,disabled:node.disabled})),
   preferences:Object.fromEntries(['eagler-touhou-game-options-v1-th09','eagler-touhou-touch-options-v1'].map(key=>[key,localStorage.getItem(key)]))}};
}
async function saveScreenshot(name){
 const calledAt=performance.now(),response=await session.send('Page.captureScreenshot',{format:'png',captureBeyondViewport:false},15000);
 assert.equal(typeof response.data,'string','actual owned-page screenshot payload');
 const path=resolve(base,name+'.png');writeFileSync(path,Buffer.from(response.data,'base64'),{flag:'wx'});
 return {calledAt,returnedAt:performance.now(),file:identity(path)};
}

try{
 assert.equal(fileURLToPath(import.meta.url),scriptPaths[0],'run from the matching managed worktree');
 movement=process.env.TH09_HOSTED_MOVEMENT||'touch';assert.ok(['touch','joystick-free'].includes(movement));
 assert.ok(process.env.TH09_HOSTED_CPU_RATE===undefined||process.env.TH09_HOSTED_CPU_RATE==='1','Android uses actual hardware, no CPU4 emulation');
 optionsPath=process.env.TH09_HOSTED_SERVER_OPTIONS;
 const adbPath=process.env.TH09_ADB_PATH,serial=process.env.TH09_ANDROID_SERIAL,dependenciesRoot=process.env.TH09_ANDROID_DEPENDENCIES_ROOT;
 for(const [name,value] of Object.entries({optionsPath,adbPath,dependenciesRoot}))assert.ok(value&&isAbsolute(value),'explicit absolute '+name);
 assert.ok(serial,'explicit existing Android serial');
 sourcePaths.push(optionsPath);report.sourceBefore=sourcePaths.map(identity);
 report.movement=movement;report.settingsActivation='real CDP touch';report.artifactDirectory=base;
 const options=JSON.parse(readFileSync(optionsPath,'utf8'));
 assert.equal(Object.hasOwn(options,'artifactDir'),false,'driver owns its new server artifact directory');
 server=await startManagedTouchLauncherServer({...options,artifactDir:resolve(base,'server')});
 report.server={url:server.url,identity:server.identity,started:server.started};
 const url=new URL(server.url);assert.equal(url.hostname,'127.0.0.1','explicit loopback Android reverse origin');
 report.host=await requestJSON(new URL('host-manifest.json',url));
 assert.equal(report.host.profile,'web-development');assert.equal(report.host.shared?.resourceMode,'hosted');
 assert.equal(report.host.shared?.testBuild,true);assert.equal(report.host.shared?.runtimeManifest,undefined);
 stage('real-host-preflight',{profile:report.host.profile,movement,cpuRate:1});
 session=await openOwnedAndroidSession({adbPath,serial,dependenciesRoot,reversePort:Number(url.port),commandTimeoutMs:30000});
 report.ownedSessionOpened=session.snapshot();
 await session.send('Page.enable');
 const expression='('+installAudit.toString()+')('+JSON.stringify({origin:url.origin})+')';
 report.auditInstallation=await session.send('Page.addScriptToEvaluateOnNewDocument',{source:expression});
 report.navigation=await session.send('Page.navigate',{url:new URL('play/th09?uiLocale=en',url).href});
 assert.equal(report.navigation.errorText,undefined,'actual owned page navigation');
 await until(async()=>{
  const found=await evaluate(readUI,{selector:'[data-game-settings]'});
  await dismissKnownNotices();return found.locale==='en'&&found.count===1;
 },60000,'actual English Game settings');
 await chooseSetting('[data-game-settings]','music','none');
 const switchSpec={scope:'[data-game-settings]',selector:'input[role="switch"]',name:'Enable touch controls',attached:true};
 report.touchPreferenceBefore=await attachedUI(switchSpec);
 if(!report.touchPreferenceBefore.checked)await tapUI(switchSpec,'enable real touch preference');
 await until(async()=>(await attachedUI(switchSpec)).checked===true,10000,'real touch preference persisted');
 report.touchPreferenceAfter=await attachedUI(switchSpec);
 await tapUI({scope:'[data-game-settings]',selector:'button',name:'Button layout & touch settings'},'open real touch layout settings');
 const collapseSpec={scope:'[data-touch-editor-scene]',selector:'[data-touch-workbench-collapse]'};
 const collapse=await uiTarget(collapseSpec);
 if(collapse.expanded==='false')await tapUI(collapseSpec,'expand real layout controls');
 await chooseSetting('[data-touch-editor-scene]','movement',movement);
 await chooseSetting('[data-touch-editor-scene]','focus','hold-button');
 await tapUI({scope:'[data-touch-editor-scene] .layout-workbench-actions',selector:'button',name:'Exit'},'exit real layout editor');
 await until(async()=>(await evaluate(()=>document.querySelectorAll('[data-touch-editor-scene]').length))===0,10000,'layout editor closed');
 report.deviceBrowser=await evaluate(()=>({userAgent:navigator.userAgent,maxTouchPoints:navigator.maxTouchPoints,
  devicePixelRatio,screen:{width:screen.width,height:screen.height,orientation:screen.orientation?.type??null},
  viewport:{width:innerWidth,height:innerHeight,visualWidth:visualViewport?.width??null,visualHeight:visualViewport?.height??null,
   offsetLeft:visualViewport?.offsetLeft??null,offsetTop:visualViewport?.offsetTop??null,scale:visualViewport?.scale??null},
  visibility:document.visibilityState,focus:document.hasFocus(),timeOrigin:performance.timeOrigin}));
 assert.ok(report.deviceBrowser.maxTouchPoints>0,'actual Android touch-capable browser');
 const primarySpec={scope:'[role="region"][aria-label="TH09 game launch"]',selector:'[data-launch-primary]'};
 const primary=await uiTarget(primarySpec,180000);assert.equal(primary.text,'Start game');
 report.launchIntent={kind:'one-real-Start-game-touch',primary};
 await tapUI(primarySpec,'single real Start game intent');stage('real-start-requested');
 await until(async()=>{
  await dismissKnownNotices();
  const state=await currentState();report.launchStateSamples.push(state);
  assert.ok(report.launchStateSamples.length<=1500,'bounded launch state history');
  assert.equal(state.error||'','');
  assert.notEqual(state.touch?.[0],3,'do not recover or qualify attract/demo');
  if(state.runtime&&state.visible)return true;
  const warnings=await evaluate(readUI,{selector:'[data-launch-warning]'});
  assert.ok(warnings.count<=1,'one actual launch warning');
  if(warnings.count){
   const kind=warnings.rows[0].warning;
   assert.ok(['touch.disabledInputWarning','music.noneLaunchWarning','music.midiLaunchWarning'].includes(kind),'unrelated warning cannot be accepted');
   if(!acknowledgedWarnings.has(kind)){
    await tapUI({scope:'[data-launch-warning="'+kind+'"]',selector:'button',name:'Start anyway'},'accept launch warning '+kind);
    acknowledgedWarnings.add(kind);report.acceptedWarnings=[...acknowledgedWarnings];
   }else{report.warningExitObservations??={};report.warningExitObservations[kind]=(report.warningExitObservations[kind]||0)+1;}
  }
  return false;
 },180000,'real launch ACK / __th09Runtime publication',400);
 await until(async()=>(await evaluate(()=>document.querySelectorAll('.touch-runtime .layout-placed').length))>0,30000,'actual touch overlay controls');
 report.launched=await currentState();assert.equal(report.launched.owned,true);assert.equal(report.launched.frames,1);
 assert.equal(report.launched.moduleMatchesRuntime,true);assert.equal(report.launched.error,'');
 stage('real-native-launched',{epoch:report.launched.epoch});
 await until(async()=>{
  await dismissKnownNotices();
  const state=await currentState();
  assert.equal(state.error,'');assert.notEqual(state.touch?.[0],3,'replay/attract is not human gameplay');
  if(state.touch?.[0]===1&&state.touch?.[1]===1&&state.title?.[1]===0){assertPlayable(state);return true;}
  const menu=state.title?.[1]===1&&state.title?.[3]===1&&state.title?.[5]>=10;
  if(menu||state.touch?.[0]===2){
   assert.ok(report.menuNavigation.length<40,'bounded real menu taps');
   if(menu){
    assert.ok([1,2,3].includes(state.title[2]),'Story main/difficulty/character only');
    if(state.title[2]===1)assert.equal(state.title[4],0,'real default Story selection; do not blindly choose another menu');
   }
   const target=await captureGeometry('menu-'+report.menuNavigation.length,true);
   const row={before:state,geometry:target,firstInjection:report.injections.length};report.menuNavigation.push(row);
   await tapPoint(target.point,'real single-finger menu confirm',90);row.afterLastInjection=report.injections.length;
   await delay(140);
  }
  return false;
 },60000,'human Story gameplay through real menu touch',120);
 report.gameReady=await currentState();assertPlayable(report.gameReady);
 probeInstallAttempted=true;
 await evaluateExpression('window.__th09ManagedTouchIntegrationProbeV1=('+installManagedTouchLauncherProbe.toString()+')('+
  JSON.stringify({capacity:30000,label,expectedEpoch:report.launched.epoch})+');({installed:true})');
 stage('actual-android-gameplay-observation-start',{movement,cpuRate:1});
 await trajectory('gesture-60-a',1800,60);
 await trajectory('gesture-120',2200,120);
 await trajectory('gesture-60-b',900,60);
 await probePhase('hud-focus');
 const focusSpec={selector:'.touch-runtime .layout-focus.layout-placed'};
 report.hudFocus={before:await attachedUI(focusSpec)};
 await tapUI(focusSpec,'real HUD focus hold',250);
 report.hudFocus.after=await attachedUI(focusSpec);assert.equal(report.hudFocus.after.pressed,'false');
 await delay(200);await lightProbe('after-hud-focus');
 await probePhase('hud-fire');
 const fireSpec={selector:'.touch-runtime .layout-fire.layout-placed'},fireBefore=await attachedUI(fireSpec);
 assert.ok(['true','false'].includes(fireBefore.pressed),'actual TH09 toggle-fire state');
 report.hudFire={before:fireBefore,checks:[]};
 await tapUI(fireSpec,'real HUD fire toggle',120);
 await until(async()=>{
  const state=await attachedUI(fireSpec);report.hudFire.checks.push(state);return state.pressed!==fireBefore.pressed;
 },5000,'real fire toggle state');
 await tapUI(fireSpec,'restore fire through real HUD toggle',120);
 await until(async()=>(await attachedUI(fireSpec)).pressed===fireBefore.pressed,5000,'real fire preference restored');
 report.hudFire.after=await attachedUI(fireSpec);
 await delay(200);await lightProbe('after-hud-fire');
 report.finalState=await currentState();assertPlayable(report.finalState);
 assert.equal(report.finalState.epoch,report.launched.epoch);assert.equal(report.finalState.href,report.launched.href);
 await probePhase('end-of-input',{noFurtherInput:true});
 report.functionalWorkComplete=true;
}catch(error){
 report.failure=errorRecord(error);
 if(error?.ownedAndroidSession)report.failedSessionOpen=error.ownedAndroidSession;
 else if(error?.diagnostics)report.failedSessionOpen=error.diagnostics;
 stage('retained-failure',{message:String(error)});
}finally{
 // Restore first and export once. Never copy the full raw during an input phase.
 if(session&&probeInstallAttempted){
  report.disposalTiming={calledAt:performance.now(),returnedAt:null};
  try{
   const result=await evaluate(()=>{
    const probe=window.__th09ManagedTouchIntegrationProbeV1;
    if(!probe)return {probeAbsent:true};
    return probe.dispose();
   },null,30000);
   if(result.probeAbsent)throw Error('Attempted probe installation left no observable probe to dispose');
   report.probe=result;probeDisposed=result.disposed===true;
   report.disposed={disposed:result.disposed,observing:result.observing,hooksAtDisposal:result.hooksAtDisposal,
    hooks:result.hooks,gettersCurrent:result.gettersCurrent,snapshotWork:result.snapshotWork};
  }catch(error){report.disposeError=errorRecord(error);report.cleanupErrors.push('probe disposal/export');}
  finally{report.disposalTiming.returnedAt=performance.now();}
 }
 if(session){
  try{report.observations=await evaluate(captureAudit);report.retainedState=await currentState();}
  catch(error){report.observationError=errorRecord(error);report.cleanupErrors.push('final read-only observations');}
  try{report.screenshot=await saveScreenshot(report.failure?'failure':'gameplay');}
  catch(error){report.screenshotError=errorRecord(error);report.cleanupErrors.push('owned screenshot');}
 }
 try{
  if(report.functionalWorkComplete){
   const probe=report.probe;assert.ok(probeDisposed&&probe,'one final disposed probe export required');
   assert.equal(probe.observing,false);assert.equal(probe.coverage.rawComplete,true);
   assert.equal(probe.owner.valid,true);assert.equal(probe.owner.invalidation,null);assert.equal(probe.gettersCurrent,true);
   assert.ok(probe.hooksAtDisposal.every(hook=>hook.stillOurs===true),'original hook ownership before disposal');
   assert.ok(probe.hooks.every(hook=>hook.restored&&!hook.restoreSkipped&&!hook.restoreError),'captured original hooks restored');
   assert.equal(probe.errors.length,0);
   report.inputEvidence=['gesture-60-a','gesture-120','gesture-60-b'].map(name=>inputEvidence(probe,name));
   const focusPhase=probe.phases.find(row=>row.name==='hud-focus');
   const focusCalls=probe.records.filter(row=>row.phaseId===focusPhase.id&&row.kind==='native-entry'&&row.name==='_th09_touch_controls');
   assert.ok(focusCalls.some(row=>row.args[2]===1)&&focusCalls.some(row=>row.args[2]===0),'real held focus reaches native and releases');
   const firePhase=probe.phases.find(row=>row.name==='hud-fire');
   const fireCalls=probe.records.filter(row=>row.phaseId===firePhase.id&&row.kind==='native-entry'&&row.name==='_th09_touch_controls');
   assert.ok(fireCalls.some(row=>row.args[1]===1)&&fireCalls.some(row=>row.args[1]===0),'real fire on/off reaches native');
   report.hudNativeEvidence={focusCalls:focusCalls.map(row=>({seq:row.seq,args:row.args})),fireCalls:fireCalls.map(row=>({seq:row.seq,args:row.args}))};
   report.continuity={boundaries:probe.records.filter(row=>row.kind==='lifecycle'||row.kind==='owner-invalidated'),
    meaning:'Retained boundaries require separate cadence analysis; functional PASS does not certify uninterrupted frames.'};
   for(const audit of [report.observations.host,report.observations.child]){
    assert.ok(audit,'both actual documents retain protocol/error observations');
    assert.equal(audit.overflow,false);assert.equal(audit.errors.length,0,'no unhandled page initialization/runtime error');
   }
   report.goals={realHumanSinglePlayer:true,realSettingsAndTrustedInput:true,allGestureRoutesAndNewStates:true,
    originalProbeHooksRestored:true,oneFinalFullExport:true,noArtificialResourceWait:true};
   report.passed=true;
  }
 }catch(error){report.validationFailure=errorRecord(error);report.passed=false;}
 if(session){
  try{report.ownedSessionFinal=await session.close();}
  catch(error){report.sessionCloseError=errorRecord(error);report.ownedSessionFinal=error?.ownedAndroidSession??session.snapshot();report.cleanupErrors.push('owned session close');}
  if(report.ownedSessionFinal?.cleanup?.qualified!==true||report.ownedSessionFinal?.metadataComplete!==true||
     report.ownedSessionFinal?.sourceStable!==true)report.cleanupErrors.push('owned session cleanup/provenance qualification');
 }else if(report.failedSessionOpen?.cleanup?.qualified!==true&&report.failedSessionOpen)report.cleanupErrors.push('failed-open cleanup qualification');
 if(server){
  try{report.serverFinal=await server.stop();if(report.serverFinal.clean!==true)report.cleanupErrors.push('server input audit/cleanup');}
  catch(error){report.serverStopError=errorRecord(error);report.cleanupErrors.push('owned server stop');}
 }
 try{
  report.sourceAfter=sourcePaths.map(identity);
  report.sourceStable=report.sourceBefore.length===sourcePaths.length&&JSON.stringify(report.sourceBefore)===JSON.stringify(report.sourceAfter);
 }catch(error){report.sourceAuditError=errorRecord(error);report.sourceStable=false;}
 report.inputOutcomeUnknown=inputOutcomeUnknown;
 if(report.failure||report.validationFailure||report.cleanupErrors.length||!report.sourceStable)report.passed=false;
 report.endedAt=new Date().toISOString();
 writeFileSync(reportPath,JSON.stringify(report,null,2)+'\n',{flag:'wx'});
 console.log(JSON.stringify({report:identity(reportPath),passed:report.passed,sourceStable:report.sourceStable,
  movement,cpuRate:1,inputEvidence:report.inputEvidence?.map(row=>({phase:row.phase,submitted:row.submitted,frameSamples:row.frameSamples})),
  failure:report.failure?.message??report.validationFailure?.message??null,cleanupErrors:report.cleanupErrors}));
 if(!report.passed)process.exitCode=1;
}
