import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile,stat} from 'node:fs/promises';
import {join,resolve} from 'node:path';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {dependencySnapshot} from '../testing/dependency-snapshot.mjs';
import {createFixtureServer} from '../browser/fixture-server.mjs';
import {MUSIC_PCM_PROBE_SOURCE} from './music-browser-helpers.mjs';
import {DESKTOP_AUDIO_PROBE_ADAPTER_SOURCE,mapDesktopSfxStartJournal} from './runtime-desktop-audio.mjs';
import {correlateInputSfxWindow} from './runtime-desktop-readiness.mjs';
import {capturePresentationGainCalibration,assertPresentationGainCalibration} from './presentation-gain-calibration.mjs';

const root=resolve(import.meta.dirname,'../..');
const sourcePath='common/include/eagler/audio/WebSfx.hpp';
const existingPcmFixture='common/tests/web-sfx-output.test.mjs';
const oldQueueSamples=Object.freeze({left:0.125,right:-0.125});
const sfxSamples=Object.freeze([[0.25,-0.5],[0.5,-0.25]]);
const expectedStart=[[0.375,-0.625],[0.625,-0.375]];
const expectedQueueTail=[0.125,-0.125];
const expectedSfxAtTail=[[0.25,-0.5],[0.5,-0.25]];

// Ownership is keyed by game, while products and repository locations come
// from the production catalog and workspace layout. th075 deliberately uses
// miniaudio directly, so its two products are tracked as a different output
// path instead of being invented as WebSfx consumers.
export const SFX_AUDIO_OWNERS=Object.freeze({
  th06:Object.freeze({kind:'web-sfx',source:'src/SoundPlayer.cpp'}),
  th07:Object.freeze({kind:'web-sfx',source:'src/SoundPlayer.cpp'}),
  th075:Object.freeze({kind:'other-audio',source:'web/cpp/sdl/AudioDevice.cpp',evidence:/ma_sound_start\(/}),
  th08:Object.freeze({kind:'web-sfx',source:'web/cpp/sdl/AudioHost.cpp'}),
  th09:Object.freeze({kind:'web-sfx',source:'web/cpp/sdl/AudioDevice.cpp'}),
  th10:Object.freeze({kind:'web-sfx',source:'web/cpp/sdl/AudioHost.cpp'}),
  th11:Object.freeze({kind:'web-sfx',source:'web/cpp/sdl/AudioDevice.cpp'}),
  th15:Object.freeze({kind:'web-sfx',source:'web/cpp/sdl/AudioDevice.cpp'}),
  th20:Object.freeze({kind:'web-sfx',source:'web/cpp/sdl/AudioHost.cpp'}),
});

export function buildSfxProductCoverage({productGames,repositories,owners}){
  const consumers=[],otherAudioProducts=[];
  for(const [game,product] of Object.entries(productGames)){
    const owner=owners[game];
    assert.ok(owner,`${game}: missing SFX audio-owner mapping`);
    const repository=repositories[game];
    assert.ok(repository,`${game}: workspace repository mapping is missing`);
    const hasMultiplayerRuntime=Object.hasOwn(product,'multiplayerRuntime');
    const hasMultiplayerContract=Object.hasOwn(product,'multiplayer');
    assert.equal(hasMultiplayerRuntime,hasMultiplayerContract,
      `${game}: multiplayer runtime and product contract must be declared together`);
    const variants=[{variant:'normal',repository,source:owner.source}];
    if(hasMultiplayerRuntime){
      const multiplayerRepository=repositories[`${game}mp`]||repository;
      variants.push({variant:'multiplayer',repository:multiplayerRepository,source:owner.multiplayerSource||owner.source});
    }
    for(const variant of variants){
      const path=join(variant.repository,variant.source).replaceAll('\\','/');
      const row={game,productId:variant.variant==='multiplayer'?`${game}mp`:game,variant:variant.variant,path};
      if(owner.kind==='web-sfx')consumers.push(row);
      else if(owner.kind==='other-audio')otherAudioProducts.push({...row,evidence:owner.evidence});
      else assert.fail(`${game}: unsupported SFX owner kind ${owner.kind}`);
    }
  }
  for(const game of Object.keys(owners))assert.ok(Object.hasOwn(productGames,game),`${game}: stale SFX owner mapping has no product`);
  return {consumers,otherAudioProducts};
}

export function extractWebSfxBody(source){
  const begin='// BEGIN_WEB_SFX_JS',end='// END_WEB_SFX_JS';
  const start=source.indexOf(begin),finish=source.indexOf(end);
  assert.ok(start>=0&&finish>start,'WebSfx production JS markers are missing');
  assert.equal(source.indexOf(begin,start+begin.length),-1,'WebSfx production JS start marker is duplicated');
  assert.equal(source.indexOf(end,finish+end.length),-1,'WebSfx production JS end marker is duplicated');
  return source.slice(start+begin.length,finish);
}

export function makeQueueTailMutation(body){
  const direct='node.start(0, v.offset);';
  assert.equal(body.split(direct).length-1,1,'production direct-start call must be unique for the negative control');
  return body.replace(direct,"node.start(Module['eaglerSfxQueuedFrames'] / ctx.sampleRate, v.offset);");
}

export function assertConsumerCoverage(sources,consumers){
  const rows=[];
  for(const consumer of consumers){
    const {game,variant,path}=consumer,source=sources.get(path);
    assert.ok(source,`${consumer.productId}: missing consumer source ${path}`);
    assert.match(source,/#include\s*<eagler\/audio\/WebSfx\.hpp>/,`${consumer.productId}: missing shared output service include`);
    assert.match(source,/web_sfx::play\s*\(/,`${consumer.productId}: SFX play does not reach the shared output service`);
    rows.push({game,productId:consumer.productId,variant,path});
  }
  return rows;
}

export function assertOtherAudioCoverage(sources,products){
  const rows=[];
  for(const product of products){
    const source=sources.get(product.path);
    assert.ok(source,`${product.productId}: missing alternative audio source ${product.path}`);
    assert.doesNotMatch(source,/#include\s*<eagler\/audio\/WebSfx\.hpp>|web_sfx::play\s*\(/,
      `${product.productId}: source uses WebSfx and must be covered as a consumer`);
    assert.match(source,product.evidence,`${product.productId}: alternative audio path evidence changed`);
    rows.push({game:product.game,productId:product.productId,variant:product.variant,path:product.path,
      scope:'alternative audio owner; shared WebSfx queue contract does not apply'});
  }
  return rows;
}

export function assertRenderedSamples(scenario){
  assert.equal(scenario.renderedFrames,scenario.queuedFrames+2,'render must include the whole prequeued span and the two-frame effect');
  assert.deepEqual(scenario.firstSamples,expectedStart,
    `SFX must be mixed into the first rendered samples while ${scenario.queuedFrames} queued frames remain`);
  assert.deepEqual(scenario.queueTailSample,expectedQueueTail,
    'prequeued PCM must still occupy the queue tail after the SFX starts');
  return true;
}

export function assertMutationWaitsForTail(scenario){
  assert.deepEqual(scenario.firstSamples,[[0.125,-0.125],[0.125,-0.125]],
    'tail-wait negative control must leave the queue prefix without the new SFX');
  assert.deepEqual(scenario.queueTailSamples,expectedSfxAtTail,
    'tail-wait negative control must render the SFX only after the queued PCM');
  assert.throws(()=>assertRenderedSamples(scenario),/first rendered samples/,
    'the normal invariant must reject the delayed-to-tail samples');
  return true;
}

async function sourceEvidence(){
  const workspace=JSON.parse(await readFile(resolve(root,'launcher/config/workspace.json'),'utf8'));
  const productCoverage=buildSfxProductCoverage({productGames:PRODUCT_GAMES,repositories:workspace.repositories,owners:SFX_AUDIO_OWNERS});
  const header=await readFile(resolve(root,sourcePath));
  const headerText=header.toString('utf8');
  const body=extractWebSfxBody(headerText);
  assert.match(body,/node\.start\(0, v\.offset\);/,
    'the production WebAudio source must start at the current render time');

  const files=new Map();
  const hashes={};
  const allSourceProducts=[...productCoverage.consumers,...productCoverage.otherAudioProducts];
  for(const path of new Set(allSourceProducts.map(row=>row.path))){
    const bytes=await readFile(resolve(root,path));files.set(path,bytes.toString('utf8'));
    hashes[path]=createHash('sha256').update(bytes).digest('hex');
  }
  const consumers=assertConsumerCoverage(files,productCoverage.consumers);
  const otherAudioProducts=assertOtherAudioCoverage(files,productCoverage.otherAudioProducts);
  const probeSources=await dependencySnapshot(root,['tools/ports/sfx-queue-invariants.mjs',
    'launcher/package.json','launcher/package-lock.json','launcher/config/workspace.json']);
  return {body,headerHash:createHash('sha256').update(header).digest('hex'),hashes,consumers,otherAudioProducts,probeSources};
}

async function renderScenario(page,body,queuedFrames,mode){
  return page.evaluate(async({body,queuedFrames,mode,queueLeft,queueRight,sfxSamples})=>{
    const sampleRate=44100;
    const context=new OfflineAudioContext(2,queuedFrames+2,sampleRate);
    const queued=context.createBuffer(2,queuedFrames,sampleRate);
    queued.getChannelData(0).fill(queueLeft);
    queued.getChannelData(1).fill(queueRight);
    const queuedVoice=context.createBufferSource();
    queuedVoice.buffer=queued;queuedVoice.connect(context.destination);queuedVoice.start(0);

    const Module={SDL3:{audioContext:context},eaglerSfxQueuedFrames:queuedFrames};
    const heap=new Uint8Array(new ArrayBuffer(16));
    new Float32Array(heap.buffer,0,4).set(sfxSamples.flat());
    const command=new Function('Module','HEAPU8','op','id','pcm','frames','channels','rate','format','value',body);
    const upload=command(Module,heap,0,7,0,2,2,sampleRate,0,0);
    if(upload!==1)throw new Error('production WebSfx upload rejected the existing PCM fixture');
    const play=command(Module,heap,2,7,0,0,0,0,0,0);
    if(play!==1)throw new Error('production WebSfx play did not create a direct output voice');

    // OfflineAudioContext renders both the queued PCM source and production
    // WebSfx graph into the same destination, so assertions inspect actual
    // mixed output samples rather than source events or planned start times.
    const rendered=await context.startRendering();
    const left=rendered.getChannelData(0),right=rendered.getChannelData(1);
    const pair=frame=>[left[frame],right[frame]];
    return {mode,queuedFrames,renderedFrames:rendered.length,sampleRate,
      firstSamples:[pair(0),pair(1)],queueTailSample:pair(queuedFrames-1),
      queueTailSamples:[pair(queuedFrames),pair(queuedFrames+1)]};
  },{body,queuedFrames,mode,queueLeft:oldQueueSamples.left,queueRight:oldQueueSamples.right,sfxSamples});
}

function makeProbeActionWindow(start,capture,end){
  const consumer={baseline:{available:true,complete:true,phase:'consumer-run',sequence:1,stage:0,activePlayerShots:0,consumed:{fire:false}},
    held:{available:true,complete:true,phase:'consumer-run',sequence:2,stage:0,activePlayerShots:1,consumed:{fire:true}},
    released:{available:true,complete:true,phase:'consumer-run',sequence:3,stage:0,activePlayerShots:1,consumed:{fire:false}}};
  return{source:'SDL3.audioContext.currentTime',game:'th08',variant:'normal',code:'KeyZ',sessionId:start.sessionId,
    startContextTime:start.clock.currentTime,endContextTime:end.currentTime,contextIndex:start.clock.contextIndex,sampleRate:start.clock.sampleRate,
    inputCapture:capture,historyReadiness:start.historyReadiness,
    clockEvidence:{start:start.clock,end:{...end,source:'SDL3.audioContext.currentTime'}},releaseEvidence:{source:'readRuntimeObservation.input',baselineValue:0,consumedValue:1,
      nativeValue:0,contextIndex:end.contextIndex,currentTime:end.currentTime,consumer}};
}

async function waitForNextRenderedFrame(frame,afterTime){
  await frame.evaluate(async after=>{const context=window.__webSfxComponent.context,started=performance.now();
    while(!(context.currentTime>after)){
      if(performance.now()-started>3000)throw new Error('The running AudioContext did not advance past the scheduled source frame');
      await new Promise(resolve=>setTimeout(resolve,5));
    }
  },afterTime);
}

async function beginProbeAction(frame,page){
  const started=await frame.evaluate(()=>window.__eaglerDesktopAudioProbe.beginInputWindow('th08','KeyZ'));
  assert.equal(started.historyReady,true,started.historyError);
  await frame.locator('#focus').focus();
  await page.keyboard.down('z');
  return started;
}

async function finishProbeAction(frame,page,started){
  await page.keyboard.up('z');
  const end=await frame.evaluate(()=>window.__eaglerDesktopAudioProbe.clockFor('th08'));
  assert.equal(end.available,true,'the selected SDL3 AudioContext clock remains available at key release');
  assert.equal(end.state,'running');
  await waitForNextRenderedFrame(frame,end.currentTime);
  const capture=await frame.evaluate(()=>window.__eaglerDesktopAudioProbe.endInputWindow());
  return{start:started,end,capture};
}

async function runLiveWebAudioProbe(browser,body){
  const parentHtml='<!doctype html><meta charset="utf-8"><title>SFX WebAudio fixture</title><iframe id="audio-probe" src="/audio-probe.html"></iframe>';
  const childHtml='<!doctype html><meta charset="utf-8"><button id="start-context">Start AudioContext</button><input id="focus" aria-label="SFX probe key">'+
    '<script>document.querySelector("#start-context").addEventListener("click",async()=>{const context=new AudioContext();await context.resume();window.__fixtureContext=context;});</script>';
  const fixture=await createFixtureServer(async(request,response)=>{
    const path=new URL(request.url,'http://localhost').pathname;
    if(path==='/'){response.writeHead(200,{'content-type':'text/html; charset=utf-8','cache-control':'no-store'});response.end(parentHtml);return;}
    if(path==='/audio-probe.html'){response.writeHead(200,{'content-type':'text/html; charset=utf-8','cache-control':'no-store'});response.end(childHtml);return;}
    response.writeHead(404,{'content-type':'text/plain'});response.end('not found');
  });
  const page=await browser.newPage();
  try{
    await page.goto(fixture.base,{waitUntil:'load'});
    const frame=page.frames().find(candidate=>new URL(candidate.url()).pathname==='/audio-probe.html');
    assert.ok(frame,'the localhost child iframe is available for AudioWorklet secure-context execution');
    await frame.locator('#start-context').click();
    const installed=await frame.evaluate(({musicSource,adapterSource,webSfxBody})=>{
      const context=window.__fixtureContext;if(!context)throw new Error('The AudioContext gesture did not create a context');
      (0,eval)(musicSource);
      const Module={SDL3:{audioContext:context}},frames=Math.round(context.sampleRate*.16),heap=new ArrayBuffer(frames*2*4),heapU8=new Uint8Array(heap),samples=new Float32Array(heap);
      let inputNonZeroSamples=0;for(let frame=0;frame<frames;frame++){
        const left=frame%2===0?.25:-.25,right=frame%2===0?-.125:.125;samples[frame*2]=left;samples[frame*2+1]=right;
        if(left!==0)inputNonZeroSamples++;if(right!==0)inputNonZeroSamples++;
      }
      const command=new Function('Module','HEAPU8','op','id','pcm','frames','channels','rate','format','value',webSfxBody);
      const upload=command(Module,heapU8,0,7,0,frames,2,context.sampleRate,0,0);
      if(upload!==1)throw new Error('The production WebSfx upload rejected the known PCM fixture');
      window.__webSfxTestRuntime={Audio:Module};
      window.__eaglerDesktopAudioRuntimeMount={game:'th08',variant:'normal',runtime:{global:'__webSfxTestRuntime',module:'Audio'}};
      (0,eval)(adapterSource);
      const pool=Module.eaglerSfxOutput,voice=pool?.voices?.get(7),probe=window.__eaglerDesktopAudioProbe;
      if(!voice?.merge||!probe?.available)throw new Error('The production output graph or shared desktop probe did not initialize');
      window.__webSfxComponent={context,Module,heapU8,frames,webSfxCall:(op,value=0)=>command(Module,heapU8,op,7,0,frames,2,context.sampleRate,0,value)};
      let owner=window.AudioBufferSourceNode.prototype;while(owner&&!Object.hasOwn(owner,'stop'))owner=Object.getPrototypeOf(owner);
      return{contextState:context.state,sampleRate:context.sampleRate,bufferDuration:voice.buffer.duration,frames,
        inputNonZeroSamples,bufferNonZeroSamples:voice.buffer.getChannelData(0).reduce((count,sample)=>count+(sample!==0?1:0),0),
        stopDescriptorOwner:owner?.constructor?.name??null,stopIsInherited:!Object.hasOwn(window.AudioBufferSourceNode.prototype,'stop'),
        stopHookAvailable:probe.startJournalSnapshot().stopHookAvailable};
    },{musicSource:MUSIC_PCM_PROBE_SOURCE,adapterSource:DESKTOP_AUDIO_PROBE_ADAPTER_SOURCE,webSfxBody:body});
    assert.equal(installed.contextState,'running','the trusted button gesture starts the real AudioContext');
    assert.equal(installed.stopIsInherited,true,'the browser fixture uses the inherited AudioScheduledSourceNode.stop API');
    assert.equal(installed.stopHookAvailable,true,'the probe wraps the browser API descriptor owner');
    assert.equal(installed.bufferNonZeroSamples,installed.frames,'the uploaded source buffer contains known nonzero PCM');

    const positiveStart=await beginProbeAction(frame,page);
    const firstStart=await frame.evaluate(()=>{const fixture=window.__webSfxComponent;fixture.webSfxCall(2);return fixture.context.currentTime;});
    await waitForNextRenderedFrame(frame,firstStart);
    const transition=await frame.evaluate(()=>{const fixture=window.__webSfxComponent,context=fixture.context;
      fixture.webSfxCall(3);const firstStop=context.currentTime;fixture.webSfxCall(2);return{firstStop,secondStart:context.currentTime};});
    await waitForNextRenderedFrame(frame,transition.secondStart);
    const finalStop=await frame.evaluate(()=>{const fixture=window.__webSfxComponent;fixture.webSfxCall(3);return fixture.context.currentTime;});
    const positiveSession=await finishProbeAction(frame,page,positiveStart);
    const positive=positiveSession.capture.sfxStartJournal;
    assert.equal(positive.available,true,positive.historyError);
    assert.equal(positive.stopHookAvailable,true);
    assert.equal(positive.events.length,2,'both actual production source starts are captured');
    assert.equal(positive.events[0].stopEventCount,1,'the inherited native stop call is observed for the first source');
    assert.equal(positive.events[1].stopEventCount,1,'the inherited native stop call is observed for the retrigger');
    assert.ok(positive.events[0].endFrame<=positive.events[1].startFrame,'the stop-to-retrigger source intervals do not overlap');
    for(const event of positive.events){
      assert.equal(event.sourceIntervalAvailable,true,event.sourceIntervalReason);
      assert.equal(event.pcmHistory.available,true,event.pcmHistory.reason);
      assert.equal(event.pcmHistory.covered,true,'the AudioWorklet retains the full rendered source interval');
      assert.equal(event.pcmHistory.gaps,0);
      assert.ok(event.pcmHistory.nonZeroFrames>0,'actual post-volume merge output contains rendered nonzero samples');
      assert.equal(event.outputRoute.connected,true,'the production merge remains connected to AudioContext.destination');
    }
    const positiveMap=mapDesktopSfxStartJournal(positiveSession.capture,{activeObservations:[]});
    const positiveActionWindow=makeProbeActionWindow(positiveStart,positiveSession.capture,positiveSession.end);
    const positiveVerdict=correlateInputSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},
      {actionWindow:positiveActionWindow,nativeSfx:{available:true,...positiveMap}});
    assert.equal(positiveVerdict.status,'passed',positiveVerdict.reason);

    const muteStart=await frame.evaluate(()=>window.__webSfxComponent.webSfxCall(5,0));
    assert.equal(muteStart,1,'the real WebSfx volume command sets the production voice to zero');
    const mutedInput=await beginProbeAction(frame,page);
    const mutedStart=await frame.evaluate(()=>{window.__webSfxComponent.webSfxCall(2);return window.__webSfxComponent.context.currentTime;});
    await waitForNextRenderedFrame(frame,mutedStart);
    await frame.evaluate(()=>window.__webSfxComponent.webSfxCall(3));
    const mutedSession=await finishProbeAction(frame,page,mutedInput),muted=mutedSession.capture.sfxStartJournal.events[0];
    assert.equal(muted.sourceIntervalAvailable,true,muted.sourceIntervalReason);
    assert.equal(muted.pcmHistory.covered,true,'the silent output interval is fully rendered');
    assert.ok(muted.pcmHistory.frames>0);
    assert.equal(muted.pcmHistory.nonZeroFrames,0,'volume zero produces an all-zero rendered output interval');
    assert.equal(muted.pcmHistory.peakAbs,0);
    const mutedMap=mapDesktopSfxStartJournal(mutedSession.capture,{activeObservations:[]});
    const mutedVerdict=correlateInputSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},
      {actionWindow:makeProbeActionWindow(mutedInput,mutedSession.capture,mutedSession.end),nativeSfx:{available:true,...mutedMap}});
    assert.equal(mutedVerdict.status,'failed','the existing classifier fails a fully captured zero-output source');

    const restored=await frame.evaluate(()=>{const fixture=window.__webSfxComponent,voice=fixture.Module.eaglerSfxOutput.voices.get(7);
      fixture.webSfxCall(5,1);voice.merge.disconnect(fixture.context.destination);return fixture.context.currentTime;});
    assert.ok(Number.isFinite(restored));
    const disconnectedInput=await beginProbeAction(frame,page);
    const disconnectedStart=await frame.evaluate(()=>{window.__webSfxComponent.webSfxCall(2);return window.__webSfxComponent.context.currentTime;});
    await waitForNextRenderedFrame(frame,disconnectedStart);
    await frame.evaluate(()=>window.__webSfxComponent.webSfxCall(3));
    const disconnectedSession=await finishProbeAction(frame,page,disconnectedInput),disconnected=disconnectedSession.capture.sfxStartJournal.events[0];
    assert.equal(disconnected.pcmHistory.covered,true);
    assert.ok(disconnected.pcmHistory.nonZeroFrames>0,'the parallel observation tap still sees PCM after destination disconnection');
    assert.equal(disconnected.outputRoute.available,true);
    assert.equal(disconnected.outputRoute.connected,false,'the production destination edge is explicitly disconnected');
    const disconnectedMap=mapDesktopSfxStartJournal(disconnectedSession.capture,{activeObservations:[]});
    const disconnectedVerdict=correlateInputSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},
      {actionWindow:makeProbeActionWindow(disconnectedInput,disconnectedSession.capture,disconnectedSession.end),
        nativeSfx:{available:true,...disconnectedMap}});
    assert.equal(disconnectedVerdict.status,'failed','parallel-tap PCM cannot pass after the production destination edge is removed');

    return{fixture:{origin:fixture.base,childFrame:'/audio-probe.html',secureContext:await frame.evaluate(()=>window.isSecureContext),
        contextState:installed.contextState,sampleRate:installed.sampleRate,bufferDuration:installed.bufferDuration,
        inputPcmNonZeroSamples:installed.inputNonZeroSamples,uploadedBufferNonZeroFrames:installed.bufferNonZeroSamples,
        stopDescriptorOwner:installed.stopDescriptorOwner,stopHookAvailable:installed.stopHookAvailable},
      retrigger:{voiceId:positive.events[0].voiceId,outputTap:positive.events[0].outputTapIndex,sourceNodeIds:positive.events.map(event=>event.sourceNodeIdentity.nodeId),
        startFrames:positive.events.map(event=>event.startFrame),endFrames:positive.events.map(event=>event.endFrame),
        closeBoundary:positive.closeBoundary,trustedEdges:positiveSession.capture.events.map(event=>({type:event.type,isTrusted:event.isTrusted,
          contextIndex:event.contextIndex,contextTime:event.contextTime,sessionSequence:event.sessionSequence})),
        stopCounts:positive.events.map(event=>event.stopEventCount),pcm:positive.events.map(event=>({covered:event.pcmHistory.covered,
          frames:event.pcmHistory.frames,nonZeroFrames:event.pcmHistory.nonZeroFrames,peakAbs:event.pcmHistory.peakAbs})),
        classifierStatus:positiveVerdict.status},
      volumeZero:{inputPcmNonZeroSamples:installed.inputNonZeroSamples,covered:muted.pcmHistory.covered,frames:muted.pcmHistory.frames,
        closeBoundary:mutedSession.capture.sfxStartJournal.closeBoundary,
        nonZeroFrames:muted.pcmHistory.nonZeroFrames,peakAbs:muted.pcmHistory.peakAbs,classifierStatus:mutedVerdict.status},
      disconnectedDestination:{covered:disconnected.pcmHistory.covered,nonZeroFrames:disconnected.pcmHistory.nonZeroFrames,
        closeBoundary:disconnectedSession.capture.sfxStartJournal.closeBoundary,
        route:disconnected.outputRoute,accepted:false,classifierStatus:disconnectedVerdict.status,reason:disconnectedVerdict.reason},
      classifierConsumerScope:'synthetic consumer-run receipt is used only to reach the existing PCM/route classifier; this fixture does not exercise a game consumer'};
  }finally{
    try{await page.close();}finally{await fixture.close();}
  }
}

function parseHighWater(source,path){
  const patterns=[
    /HIGH_WATER_FRAMES\s*=\s*(\d+)/,
    /high_water\s*=\s*(\d+)/,
    /high\s*=\s*(\d+)/,
    /queued\s*>=\s*(\d+)/,
  ];
  for(const pattern of patterns){const match=pattern.exec(source);if(match)return Number(match[1]);}
  throw new Error(`could not identify a production PCM queue high-water mark in ${path}`);
}

async function productionQueueDepths(){
  const specs=[
    {path:'ports/th07/src/SoundPlayer.cpp',expected:3072},
    {path:'ports/th11-mp/web/cpp/sdl/AudioDevice.cpp',expected:6144},
  ];
  const rows=[];
  for(const spec of specs){
    const source=await readFile(resolve(root,spec.path),'utf8');
    const frames=parseHighWater(source,spec.path);
    assert.equal(frames,spec.expected,`${spec.path}: queue depth fixture no longer matches the production high-water mark`);
    rows.push({path:spec.path,frames});
  }
  assert.equal(new Set(rows.map(row=>row.frames)).size,2,'production queue-depth scenarios must exercise distinct lengths');
  return rows;
}

export async function runSfxQueueInvariant({output}){
  return runWithReport({output,schema:'eagler-touhou/sfx-queue-invariants/1'},async(result,save)=>{
    const evidence=await sourceEvidence();
    const depths=await productionQueueDepths();
    const {launchBrowser}=await import('../browser/launch.mjs');
    const browser=await launchBrowser();
    try{
      const version=browser.version();
      const page=await browser.newPage();
      await page.setContent('<!doctype html><meta charset="utf-8"><title>SFX queue invariant</title>');
      const baseline=[],negativeControls=[];
      for(const {frames} of depths){
        const rendered=await renderScenario(page,evidence.body,frames,'production');
        assertRenderedSamples(rendered);baseline.push(rendered);
        const mutation=makeQueueTailMutation(evidence.body);
        const delayed=await renderScenario(page,mutation,frames,'queue-tail-negative-control');
        assertMutationWaitsForTail(delayed);negativeControls.push(delayed);
      }
      const gainCalibrationCapture=await capturePresentationGainCalibration(page);
      const gainCalibration=assertPresentationGainCalibration(gainCalibrationCapture);
      const liveWebAudioProbe=await runLiveWebAudioProbe(browser,evidence.body);
      const postflight=await sourceEvidence();
      assert.equal(postflight.headerHash,evidence.headerHash,'WebSfx production source changed during rendering');
      assert.deepEqual(postflight.hashes,evidence.hashes,'An audio consumer changed during rendering');
      assert.deepEqual(postflight.probeSources,evidence.probeSources,'Audio probe sources changed during rendering');
      Object.assign(result,{passed:true,browser:{engine:'Chromium',version,context:'OfflineAudioContext and running AudioContext',sampleRate:44100},
        probeSources:evidence.probeSources,sourceStable:true,
        componentRendering:{meaning:'shared WebSfx production component contributes samples to AudioContext output while previously queued mixed PCM remains pending',
          publicOwner:sourcePath,publicOwnerSha256:evidence.headerHash,
          sfxPcmFixture:{path:existingPcmFixture,samples:sfxSamples},
          queuePcm:{source:'offline BufferSource representing already queued SDL mixed PCM',channels:{left:oldQueueSamples.left,right:oldQueueSamples.right},
            productionHighWaterFrames:depths},
          negativeControl:{mutation:'node.start(Module["eaglerSfxQueuedFrames"] / ctx.sampleRate, v.offset)',
            expected:'SFX samples begin at the prequeued PCM tail and fail the overlap assertion'}},
        consumerSourceWiring:{coverage:'static source wiring only; does not prove in-game audible output',
          products:evidence.consumers,sourceSha256:evidence.hashes,otherAudioProducts:evidence.otherAudioProducts},
        productionRender:baseline,queueTailMutationRender:negativeControls,
        gainCalibration:{...gainCalibration,capture:gainCalibrationCapture},liveWebAudioProbe});
      result.status='passed';await save();
    }finally{await browser.close();}
  });
}

const argumentsList=process.argv.slice(2);
if(import.meta.main){
  let output;
  try{
    assertCliArguments(argumentsList,{values:['output']});
    const argument=argumentsList.find(value=>value.startsWith('--output='));
    assert.ok(argument,'Required: --output=report-directory');
    output=resolve(argument.slice('--output='.length));
    const info=await stat(output).catch(error=>error.code==='ENOENT'?null:Promise.reject(error));
    assert.ok(!info||info.isDirectory(),'--output must name a directory');
    const report=await runSfxQueueInvariant({output});
    console.log(JSON.stringify({status:report.status,passed:report.passed,output:resolve(output,'report.json'),
      queueFrames:report.productionRender?.map(scenario=>scenario.queuedFrames)||[],browser:report.browser}));
    if(!report.passed){console.error(report.failure||'SFX queue invariant failed');process.exitCode=1;}
  }catch(error){
    console.error(error.stack||error);process.exitCode=1;
  }
}
