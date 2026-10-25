import assert from 'node:assert/strict';
import {readRuntimeObservation,runtimeObservationRequest} from './runtime-observations.mjs';
import {resolveTitleFlow} from '../../tests/adaptation/adapters/title-flow.mjs';
import {captureNativeMusicSuspension,assertNativeMusicSuspension} from './music-native-lifecycle.mjs';
import {summarizeRuntimePcmWindow} from './music-browser-helpers.mjs';

export function readLifecycleMixer({runtime,contract}){
  let context=window[runtime.global]?.[runtime.module];
  for(const key of contract.audioContextPath)context=context?.[key];
  const probe=window.__eaglerMusicPcmProbe;
  return{target:probe?.describeTarget(context,contract.audioRole),captures:probe?.capture()??[]};
}

/** Observe live production listeners without replacing their handlers. */
export async function readInputListeners(context,page,frame){
  const result={},documents={};
  for(const [name,target] of [['host',page],['runtime',frame]]){
    let cdp;
    try{cdp=await context.newCDPSession(target);}
    catch(error){if(target===page||!String(error.message).includes('does not have a separate CDP session'))throw error;cdp=await context.newCDPSession(page);}
    try{
      const contexts=[];cdp.on('Runtime.executionContextCreated',event=>contexts.push(event.context));
      await cdp.send('Runtime.enable');const {frameTree}=await cdp.send('Page.getFrameTree'),frames=[];
      const visit=node=>{frames.push(node.frame);for(const child of node.childFrames??[])visit(child);};visit(frameTree);
      const matches=frames.filter(row=>row.url===target.url());assert.equal(matches.length,1,'Listener observation needs the exact live document');
      const defaults=contexts.filter(row=>row.auxData?.isDefault&&row.auxData?.frameId===matches[0].id);
      assert.equal(defaults.length,1,'Listener observation needs the selected document default world');
      const contextId=defaults[0].id;
      documents[name]={url:matches[0].url,frameId:matches[0].id,executionContextId:contextId};
      result[name]={};
      for(const expression of ['window','document']){
        const evaluated=await cdp.send('Runtime.evaluate',{expression,contextId,objectGroup:'input-listeners'});
        const {listeners}=await cdp.send('DOMDebugger.getEventListeners',{objectId:evaluated.result.objectId});
        result[name][expression]=listeners.filter(row=>/^(key|pointer|touch|blur|focus|visibility)/.test(row.type))
          .map(row=>({type:row.type,capture:row.useCapture,passive:row.passive,once:row.once,line:row.lineNumber,column:row.columnNumber}))
          .sort((a,b)=>JSON.stringify(a).localeCompare(JSON.stringify(b)));
      }
    }finally{await cdp.send('Runtime.releaseObjectGroup',{objectGroup:'input-listeners'});await cdp.detach();}
  }
  return{listeners:result,documents};
}

/** Retain read-only references to the outgoing owner before its document retires. */
export function retainOutgoingInput({runtime}){
  const win=document.getElementById('gameFrame').contentWindow,owner=win[runtime.global],bridge=win.__eaglerKeyboardInputBridge;
  const before={url:win.location.href,timeOrigin:win.performance.timeOrigin,epoch:bridge?.epoch};
  window.__readOutgoingInput=()=>({identity:before,active:bridge?.active(),held:['ArrowRight','KeyZ','ShiftLeft'].map(code=>({code,down:bridge?.keyDown(code)})),
    app:owner?.[runtime.app],url:win.location.href,mappingAttached:!!win.__eaglerKeyboardMapping});
  return window.__readOutgoingInput();
}

function readInputOwner({runtime}){
  const owner=window[runtime.global],bridge=window.__eaglerKeyboardInputBridge;
  return{url:location.href,timeOrigin:performance.timeOrigin,epoch:bridge?.epoch,game:bridge?.game,protocol:bridge?.protocol,app:owner?.[runtime.app],active:bridge?.active(),
    touchEnabled:owner?.[runtime.module]?.eaglerOptions?.touchEnabled,touchFocusMode:owner?.[runtime.module]?.eaglerOptions?.touchFocusMode,
    held:['ArrowRight','KeyZ','ShiftLeft'].map(code=>({code,down:bridge?.keyDown(code)}))};
}

export function assertLauncherInputLifecycle(e,{directionMask,focusMask}){
  assert.equal(e.schema,'eagler-touhou/launcher-input-lifecycle/1');assertNativeMusicSuspension(e.background.nativeLifecycle);
  assert.equal(typeof e.touchHelp.wasVisible,'boolean');assert.equal(e.touchHelp.visibleAfter,false);
  assert.ok(['true','false'].includes(e.autoFireSetup.before));assert.equal(e.autoFireSetup.after,'false');
  assert.equal(e.autoFireSetup.operation,e.autoFireSetup.before==='true'?'production-button-pointer-click':'already-disabled');
  for(const row of [e.initial,e.afterBackground,e.reopened,e.singleInput.released]){
    assert.equal(row.observation.input.value,0);assert.ok(row.owner.app>0);assert.equal(row.owner.active,true);
    assert.equal(row.owner.touchEnabled,true);assert.equal(row.owner.touchFocusMode,'hold-button');
    assert.ok(row.owner.held.every(key=>key.down===false));
  }
  const held=e.beforeBackground;assert.ok(held.observation.input.value&directionMask);assert.ok(held.observation.input.value&focusMask);
  assert.ok(held.observation.position.x>e.initial.observation.position.x);
  assert.equal(e.background.hidden.observation.input.value,0);assert.ok(e.background.hidden.owner.held.every(key=>key.down===false));
  assert.equal(e.afterBackground.owner.timeOrigin,e.initial.owner.timeOrigin);assert.equal(e.afterBackground.owner.epoch,e.initial.owner.epoch);
  assert.ok(e.afterBackground.observation.render.presentationFrame>held.observation.render.presentationFrame);
  const audio=e.background.audio;assert.equal(audio.status,'complete');assert.equal(audio.outputObserved,true);
  const samples=audio.windows.map(row=>{const target={...row.target};if(target.sourceIdentity===null)delete target.sourceIdentity;
    return{target,captures:row.taps.map(tap=>({...tap,contextIndex:row.target.contextIndex}))};});
  assert.deepEqual(summarizeRuntimePcmWindow(samples,{role:audio.role,minimumWindows:8}),audio);
  assert.equal(e.beforeExit.observation.input.value&directionMask,directionMask);assert.equal(e.beforeExit.observation.input.value&focusMask,focusMask);
  assert.ok(e.outgoingBefore.held.some(key=>key.code==='ArrowRight'&&key.down===true));
  assert.ok(e.outgoingAfter.held.every(key=>key.down===false));assert.equal(e.outgoingAfter.active,false);
  assert.equal(e.exit.source,'Launcher browser history back');assert.equal(e.exit.hostBefore.timeOrigin,e.exit.hostAfter.timeOrigin);
  assert.equal(e.exit.hostBefore.origin,e.exit.hostAfter.origin);
  assert.equal(e.outgoingAfter.mappingAttached,false);assert.equal(e.outgoingAfter.url,'about:blank');
  assert.deepEqual(e.outgoingAfter.identity,e.outgoingBefore.identity);
  assert.ok(e.reopened.owner.epoch>e.initial.owner.epoch);assert.notEqual(e.reopened.owner.timeOrigin,e.initial.owner.timeOrigin);
  assert.deepEqual(e.reopened.listeners,e.initial.listeners,'Input listener ownership changed across real Launcher retirement/relaunch');
  for(const sample of [e.initial,e.reopened]){
    assert.equal(sample.listenerDocuments.runtime.url,sample.owner.url);
    assert.notEqual(sample.listenerDocuments.host.frameId,sample.listenerDocuments.runtime.frameId);
    assert.notEqual(sample.listenerDocuments.host.executionContextId,sample.listenerDocuments.runtime.executionContextId);
  }
  const listeners=[...e.initial.listeners.host.window,...e.initial.listeners.host.document];
  for(const type of ['keydown','keyup'])assert.ok(listeners.some(row=>row.type===type),'Live Host keyboard listener was not observed');
  assert.equal(e.singleInput.held.observation.input.value,directionMask);
  assert.ok(e.singleInput.held.observation.position.x>e.reopened.observation.position.x);
  assert.equal(e.singleInput.released.observation.input.value,0);assert.deepEqual(e.cleanupErrors,[]);
}

/** One real Launcher session: native tab background, held-input exit, fresh epoch. */
export async function runLauncherInputLifecycle({page,context,frame,target,adapter,controller,exit,relaunch,readAudio,save}){
  const flow=resolveTitleFlow(target),request=runtimeObservationRequest(target),e={schema:'eagler-touhou/launcher-input-lifecycle/1',cleanupErrors:[]};
  const observe=()=>frame.evaluate(readRuntimeObservation,request),wait=ms=>page.waitForTimeout(ms);
  const until=async(label,predicate)=>{const deadline=Date.now()+20000;let state;do{state=await observe();if(predicate(state))return state;await wait(50);}while(Date.now()<deadline);throw Error(label+': '+JSON.stringify(state));};
  const focus=async()=>{if(!frame.isDetached()&&frame.url()!=='about:blank')await frame.locator('canvas').focus();
    else await page.evaluate(()=>{document.body.tabIndex=-1;document.body.focus();});};
  const key=async(code,down)=>{await focus();await page.keyboard[down?'down':'up'](code);};
  const press=async code=>{await key(code,true);await wait(90);await key(code,false);await wait(140);};
  const ready=()=>flow.enterGameplay({observe,waitUntil:until,press,lane:'desktop'});
  const sample=async()=>({observation:await observe(),owner:await frame.evaluate(readInputOwner,{runtime:adapter.runtime})});
  const pointer=async type=>{const button=page.locator('#touchFocus');
    if(type==='pointercancel'&&await button.count()===0)return{ownerUnmounted:true};
    if(type==='pointerdown')assert.equal(await button.isVisible(),true,'Touch focus must be visible before a simulated Pointer input');
    return button.evaluate((button,type)=>{
    const box=button.getBoundingClientRect();button.dispatchEvent(new PointerEvent(type,{bubbles:true,cancelable:true,pointerType:'touch',pointerId:9851,
      clientX:box.x+box.width/2,clientY:box.y+box.height/2,buttons:type==='pointerdown'?1:0}));
  },type);};
  const directionMask=adapter.movement.directionMasks.ArrowRight,focusMask=adapter.actions.inputMasks.focus;
  const hold=async()=>{await pointer('pointerdown');await key('ArrowRight',true);await until('native direction and touch focus held',s=>(s.input.value&directionMask)&&(s.input.value&focusMask));return sample();};
  let failure;
  try{
    const help=page.locator('#touchHelp');e.touchHelp={wasVisible:await help.isVisible()};
    if(e.touchHelp.wasVisible)await page.locator('#touchHelpClose').click();e.touchHelp.visibleAfter=await help.isVisible();await save(e);
    await ready();const fire=page.locator('#touchFire');e.autoFireSetup={before:await fire.getAttribute('aria-pressed')};
    if(e.autoFireSetup.before==='true'){await fire.click();e.autoFireSetup.operation='production-button-pointer-click';}
    else e.autoFireSetup.operation='already-disabled';
    await page.waitForFunction(()=>document.getElementById('touchFire')?.getAttribute('aria-pressed')==='false');
    e.autoFireSetup.after=await fire.getAttribute('aria-pressed');await until('native neutral after automatic fire disabled',s=>s.input.value===0);
    e.initial=await sample();{const read=await readInputListeners(context,page,frame);e.initial.listeners=read.listeners;e.initial.listenerDocuments=read.documents;}await save(e);
    e.beforeBackground=await hold();await save(e);
    const suspended=await captureNativeMusicSuspension({page,frame,controller,whileHidden:async()=>{
      await until('background input released',s=>s.input.value===0);return sample();
    },afterRestore:async()=>{
      await until('foreground world resumed',s=>flow.isGameplayReady(s,'desktop')&&s.render.presentationFrame>e.beforeBackground.observation.render.presentationFrame);
      return sample();
    }});
    e.background={nativeLifecycle:suspended.evidence,hidden:suspended.hiddenResult};e.afterBackground=suspended.resumedResult;
    await key('ArrowRight',false);await pointer('pointercancel');
    await key('KeyZ',true);await until('resumed native firing',s=>(s.input.value&adapter.actions.inputMasks.fire)!==0);
    try{e.background.audio=await readAudio(frame);}finally{await key('KeyZ',false);}
    await until('fire released',s=>s.input.value===0);e.afterBackground=await sample();await save(e);
    e.beforeExit=await hold();e.outgoingBefore=await page.evaluate(retainOutgoingInput,{runtime:adapter.runtime});await save(e);
    e.exit=await exit();
    await page.waitForFunction(()=>document.getElementById('gameFrame')?.contentWindow?.location.href==='about:blank',null,{timeout:30000});
    e.outgoingAfter=await page.evaluate(()=>window.__readOutgoingInput());await save(e);
    await key('ArrowRight',false);await pointer('pointercancel');
    frame=await relaunch();await ready();e.reopened=await sample();{const read=await readInputListeners(context,page,frame);e.reopened.listeners=read.listeners;e.reopened.listenerDocuments=read.documents;}await save(e);
    await key('ArrowRight',true);await until('single new-epoch key reaches native player',s=>s.input.value===directionMask&&s.position.x>e.reopened.observation.position.x);
    e.singleInput={held:await sample()};await key('ArrowRight',false);await until('single new-epoch key released',s=>s.input.value===0);
    e.singleInput.released=await sample();assertLauncherInputLifecycle(e,{directionMask,focusMask});return e;
  }catch(error){failure=error;error.evidence=e;throw error;}
  finally{for(const code of ['ArrowRight','KeyZ'])try{await key(code,false);}catch(error){e.cleanupErrors.push(String(error));}
    try{await pointer('pointercancel');}catch(error){e.cleanupErrors.push(String(error));}await save(e);
    if(e.cleanupErrors.length){if(failure)failure.cleanupErrors=e.cleanupErrors;else throw Object.assign(Error('Launcher lifecycle input cleanup failed'),{evidence:e});}}
}
