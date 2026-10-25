import assert from 'node:assert/strict';
import {runNativeForegroundOperation} from './runtime-desktop-lifecycle.mjs';
import {isDeepStrictEqual} from 'node:util';
import {MUSIC_PCM_ANALYSIS_FRAMES,TH08_MIDI_PCM_ROLE,runtimePcmTargetIdentity} from './music-browser-helpers.mjs';

export function midiOutputResumeReady(sample,settled){
  const current=sample?.midiHost?.pcmTarget,paused=settled?.midiHost?.pcmTarget,
    now=runtimePcmTargetIdentity(current),before=runtimePcmTargetIdentity(paused);
  return now.ok&&before.ok&&isDeepStrictEqual(now.identity,before.identity)&&current.role===TH08_MIDI_PCM_ROLE&&
    sample.midiHost.contextState==='running'&&current.state==='running'&&
    current.currentTime>=paused.currentTime+MUSIC_PCM_ANALYSIS_FRAMES/current.sampleRate;
}
export function assertMidiOutputResumeBoundary(e,output){
  assert.equal(e.analysisWindowFrames,MUSIC_PCM_ANALYSIS_FRAMES);assert.ok(e.samples.length>0);
  const paused=e.settled.midiHost.pcmTarget;assert.equal(e.settled.midiHost.contextState,'suspended');assert.equal(paused.state,'suspended');
  const ready=e.samples.at(-1);assert.equal(midiOutputResumeReady(ready,e.settled),true);
  for(const sample of e.samples.slice(0,-1))assert.equal(midiOutputResumeReady(sample,e.settled),false);
  const target=ready.midiHost.pcmTarget;assert.equal(target.stateChangeDrops,0);assert.equal(paused.stateChangeDrops,0);
  assert.ok(target.stateChangeCount>paused.stateChangeCount);assert.deepEqual(target.stateChanges.slice(0,paused.stateChanges.length),paused.stateChanges);
  assert.ok(output.pcm.windows.length>0);const first=output.pcm.windows[0].target;
  assert.equal(first.state,'running');assert.ok(first.currentTime>=target.currentTime);
  assert.deepEqual(runtimePcmTargetIdentity(first).identity,runtimePcmTargetIdentity(target).identity);
}

function readDocument(){return{url:location.href,timeOrigin:performance.timeOrigin,hidden:document.hidden,visibilityState:document.visibilityState,
  hasFocus:document.hasFocus(),shadowed:['hidden','visibilityState'].filter(key=>Object.hasOwn(document,key)),events:window.__nativeTabMusicEvents??[]};}
function armDocument(){
  window.__nativeTabMusicEvents=[];
  const handler=event=>window.__nativeTabMusicEvents.push({type:event.type,trusted:event.isTrusted,hidden:document.hidden,
    visibilityState:document.visibilityState,at:performance.now()});
  document.addEventListener('visibilitychange',handler);window.__nativeTabMusicDisarm=()=>document.removeEventListener('visibilitychange',handler);
}
async function identity(page){
  const session=await page.context().newCDPSession(page);
  try{const {targetInfo}=await session.send('Target.getTargetInfo'),window=await session.send('Browser.getWindowForTarget',{targetId:targetInfo.targetId});
    return{targetId:targetInfo.targetId,url:targetInfo.url,windowId:window.windowId};}finally{await session.detach();}
}

/** Reuse the desktop startup guard, limited to the selected native tab. */
export async function runMusicTabStartup({page,frame,controller,operation,evidence}){
  evidence.before=await foregroundOwnedMusicPage(page,controller,frame);evidence.samples=[];evidence.restores=[];
  const owner=evidence.before.before;
  return runNativeForegroundOperation({operation,evidence,readState:async()=>{
    const current=await identity(page),host=await page.evaluate(readDocument),runtime=await frame.evaluate(readDocument);
    const native=await frame.evaluate(()=>({app:window.__th08Runtime?.app??null,marks:performance.getEntriesByType('mark').map(mark=>({name:mark.name,at:mark.startTime}))}));
    return{owned:current.targetId===owner.targetId&&current.windowId===owner.windowId&&current.url===owner.url,
      ready:!host.hidden&&!runtime.hidden&&host.hasFocus&&runtime.hasFocus,current,host,runtime,native};
  },restore:async()=>({ready:true,...await foregroundOwnedMusicPage(page,controller,frame)}),pause:()=>page.waitForTimeout(100)});
}
export async function foregroundOwnedMusicPage(page,controller,frame=null){
  const session=await page.context().newCDPSession(page);
  try{
    const {targetInfo}=await session.send('Target.getTargetInfo'),before=await identity(page);
    await controller.browserCdp.send('Target.activateTarget',{targetId:targetInfo.targetId});await page.bringToFront();
    if(frame)await frame.locator('canvas').focus();else await page.evaluate(()=>document.querySelector('iframe')?.focus());
    await page.waitForFunction(()=>!document.hidden&&document.hasFocus(),null,{timeout:5000,polling:100});
    const after=await identity(page);assert.deepEqual(after,before);
    return{kind:'native-selected-tab',processId:controller.metadata.processId,before,after,host:await page.evaluate(readDocument)};
  }finally{await session.detach();}
}

/** Real same-window tab activation, with browser-generated events in both documents. */
export async function captureNativeMusicSuspension({page,frame,controller,whileHidden,afterRestore}){
  const evidence={schema:'eagler-touhou/native-tab-suspension/1',kind:'same-window-tab',cleanupErrors:[]};let peer;
  const snapshot=async()=>({host:await page.evaluate(readDocument),runtime:await frame.evaluate(readDocument),owner:await identity(page)});
  try{
    evidence.activation=await foregroundOwnedMusicPage(page,controller,frame);await frame.locator('canvas').focus();
    await page.evaluate(armDocument);await frame.evaluate(armDocument);evidence.before=await snapshot();
    peer=await page.context().newPage();await peer.goto('about:blank');evidence.peer=await identity(peer);
    assert.equal(evidence.peer.windowId,evidence.before.owner.windowId,'Background peer is in another Chrome window');
    assert.notEqual(evidence.peer.targetId,evidence.before.owner.targetId);
    await peer.bringToFront();
    for(const document of [page,frame])await document.waitForFunction(()=>document.hidden&&document.visibilityState==='hidden',null,{timeout:5000});
    evidence.hidden=await snapshot();const hiddenResult=await whileHidden(evidence);evidence.hiddenObservation=hiddenResult;
    await page.bringToFront();
    for(const document of [page,frame])await document.waitForFunction(()=>!document.hidden&&document.visibilityState==='visible',null,{timeout:5000});
    await frame.locator('canvas').focus();evidence.restored=await snapshot();const resumedResult=await afterRestore(hiddenResult,evidence);
    return{evidence,hiddenResult,resumedResult};
  }catch(error){error.evidence=evidence;throw error;}
  finally{
    if(peer){try{await page.bringToFront();await peer.close();}catch(error){evidence.cleanupErrors.push(String(error));}}
    for(const document of [page,frame])try{await document.evaluate(()=>window.__nativeTabMusicDisarm?.());}catch(error){evidence.cleanupErrors.push(String(error));}
    if(evidence.cleanupErrors.length)throw Object.assign(Error('Native tab lifecycle cleanup failed'),{evidence});
  }
}

export function assertNativeMusicSuspension(e){
  assert.equal(e.schema,'eagler-touhou/native-tab-suspension/1');assert.equal(e.kind,'same-window-tab');assert.deepEqual(e.cleanupErrors,[]);
  assert.equal(e.peer.windowId,e.before.owner.windowId);assert.notEqual(e.peer.targetId,e.before.owner.targetId);
  assert.equal(e.before.host.url,e.before.owner.url);
  assert.equal(new URL(e.before.runtime.url).origin,new URL(e.before.host.url).origin);
  for(const boundary of ['hidden','restored'])assert.deepEqual(e[boundary].owner,e.before.owner,'The original Chrome page/window changed');
  for(const document of ['host','runtime']){
    const before=e.before[document],hidden=e.hidden[document],restored=e.restored[document];
    for(const row of [before,hidden,restored]){assert.deepEqual(row.shadowed,[]);assert.equal(row.url,before.url);assert.equal(row.timeOrigin,before.timeOrigin);}
    assert.equal(before.hidden,false);assert.equal(before.visibilityState,'visible');assert.equal(before.hasFocus,true);
    assert.equal(hidden.hidden,true);assert.equal(hidden.visibilityState,'hidden');assert.equal(hidden.hasFocus,false);
    assert.equal(restored.hidden,false);assert.equal(restored.visibilityState,'visible');assert.equal(restored.hasFocus,true);
    const events=restored.events;assert.ok(events.length>=2);assert.ok(events.every(event=>event.trusted===true));
    assert.deepEqual(before.events,[]);assert.ok(hidden.events.length>0&&hidden.events.at(-1).hidden===true);
    assert.deepEqual(events.slice(0,hidden.events.length),hidden.events);
    const start=events.findIndex(event=>event.hidden===true&&event.visibilityState==='hidden');
    assert.ok(start>=0&&events.slice(start+1).some(event=>event.hidden===false&&event.visibilityState==='visible'));
  }
}
