import assert from 'node:assert/strict';
import {createHash,randomUUID} from 'node:crypto';
import {execFileSync} from 'node:child_process';
import {mkdir,readFile,stat,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {createRuntimeFixtureSite} from './runtime-fixture-site.mjs';
import {readVerifiedRuntimeFile,selectRuntimeTargets,verifyPrivateData,verifyRuntimePackage} from './acceptance-contracts.mjs';
import {sourceSnapshot} from '../testing/source-snapshot.mjs';
import {dependencySnapshot} from '../testing/dependency-snapshot.mjs';
import {parseTh08MusicCatalog,parseTh08MessageMusicCues} from '../../tests/adaptation/adapters/titles/th08-music.mjs';
import {controlValidationScene,validationSceneRequest,assertControlledSceneArmed,assertControlledScene} from '../../tests/adaptation/adapters/scene.mjs';
import {dialogueTouchSceneRequest,readDialogueTouchWords,normalizeDialogueTouchManifest,normalizeDialogueTouchObservation,requestDialogueTouchMessage} from '../../tests/adaptation/adapters/dialogue.mjs';
import {collectMusicCueTransition,assessMusicCueTransition} from './music-cue-transition.mjs';
import {readSfxLatencySceneState,sfxLatencySceneRequest} from '../../tests/adaptation/adapters/audio.mjs';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {verifyLauncherFixtureMusic} from './launcher-runtime-fixture.mjs';
import {inspectOfflineUpdateDescriptor} from './offline-runtime-update.mjs';
import {readRuntimeObservation,runtimeObservationRequest} from './runtime-observations.mjs';
import {mergeMusicBehaviorResult,correlateTh08MidiSends,th08MidiTransportCount,musicScenarioErrors,musicScenarioPassed,attachMusicStageErrors,attachTh08OggFaultModeAssessment,musicTitleSceneOutcome,th08BackgroundContinuityOutcome,th08MidiCapabilityOutcome,th08MidiPauseOutcome,
  th08MidiRestartOutcome,th08MidiTitleOutcome,th08MusicModeSeparationOutcome,th08OggAcceptanceOutcome,th08OggFaultOutcome,
  th08OggPauseOutcome,th08OggRestartOutcome,th08OggTitleOutcome,th08OggTrackFile,th08MusicFaultRecoveryOutcome,th08MusicFaultJourneyOutcome,th08MusicRoomOutcome,th08OggDecodeVariantsOutcome,th08MusicStopDrainFrames} from './music-browser-result.mjs';
import {MUSIC_BROWSER_GAMES,MUSIC_PCM_PROBE_SOURCE,installMusicProtocolCapture,describeNativeMusic,describeNoneMusicControl,mapTrackToInput,
  nativeAudioStatsShape,readNativeAudioState,runtimeMusicResources,selectedMusicMappingGames,summarizeMusicPcm,
  summarizeRuntimePcmWindow,summarizeRelatedRuntimePcmWindow,collectRuntimePcmWindow,collectRuntimeOutputPcm,collectExpectedTrackRuntimePcm,RUNTIME_PCM_SAMPLE_INTERVAL_MS,
  TH08_OGG_PCM_ROLE,TH08_MIDI_PCM_ROLE,
  TH08_MIDI_VOICE_PCM_ROLE,verifyMusicInputs,pressConsumedRuntimeKey} from './music-browser-helpers.mjs';

const arg=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
const outputArgument=arg('output'),requestedOutput=outputArgument?resolve(outputArgument):null;
const root=resolve(import.meta.dirname,'../..');
let outputExists=false,outputIsDirectory=false,outputInspectionError=null;
if(requestedOutput){
  try{const info=await stat(requestedOutput);outputExists=true;outputIsDirectory=info.isDirectory();}
  catch(error){if(error.code!=='ENOENT'){outputExists=true;outputInspectionError=error;}}
}
const output=requestedOutput&&(!outputExists||outputIsDirectory)?requestedOutput:
  resolve(root,'artifacts/validation',`music-browser-cli-${randomUUID()}`);
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const titleTrack={th11:'th11_00.ogg',th15:'th15_01.ogg'};
const th08TitleOgg='th08_01.ogg',th08TitleMidi='bgm/th08_01.mid';
// GameplayControl.cpp uses stage_music[game.stage]; the first stage is index 0 -> song id 1.
const th08FirstStageSong=1;
const th08FirstStageOgg='th08_00.ogg';

function observeMusicProtocolHistory(){
  if(!Array.isArray(window.events))throw Error('Music Runtime protocol event history is unavailable');
  window.__eaglerMusicProtocolStages=window.events.map((event,index)=>({index,event:event?.event??null,stage:window.__eaglerMusicProtocolStage??'host-navigation'}));
  const originalPush=window.events.push;
  window.events.push=function(...items){
    for(let offset=0;offset<items.length;offset++)window.__eaglerMusicProtocolStages.push({
      index:this.length+offset,event:items[offset]?.event??null,stage:window.__eaglerMusicProtocolStage??'unscoped'});
    return originalPush.apply(this,items);
  };
}

async function recoverTh08MusicFault({page,frame,modeRecord,target,fixture,sharedResources,configure,midiBridgeSource,markStage,drainProtocol}){
  const recovery=modeRecord.recovery={selectedMode:'midi',capability:PRODUCT_GAMES.th08.musicCapabilities,runtimeResources:[],before:modeRecord.runtimeFrameIdentity,
    selectionEntry:'eagler-touhou/1 configure.music before launch',after:null};
  await markStage('fault-recovery-retire');
  if(modeRecord.mode==='corrupt-ogg'){
    await frame.evaluate(()=>window.__th08Runtime.stop());
    await page.waitForFunction(()=>events.some(event=>event.event==='exit'),null,{timeout:30000});
    recovery.exit=await page.evaluate(()=>events.find(event=>event.event==='exit'));
  }
  modeRecord.beforeRecoveryEvents=await page.evaluate(()=>window.events);
  modeRecord.beforeRecoveryEventStages=await page.evaluate(()=>window.__eaglerMusicProtocolStages);
  await markStage('fault-recovery-navigation');
  await drainProtocol();
  await page.goto(fixture.hostUrl(target,recovery.before.epoch+1));
  recovery.oldFrameDetached=frame.isDetached();
  await page.waitForFunction(()=>events.some(event=>event.event==='ready'),null,{timeout:120000});
  await page.evaluate(observeMusicProtocolHistory);
  const nextFrame=page.frames().find(candidate=>candidate.url().includes(`/runtime/${target.id}/`));assert.ok(nextFrame,'Recovery Runtime frame is unavailable');
  recovery.after=await bindMusicRuntimeFrame(page,nextFrame);
  await markStage('fault-recovery-configure');
  recovery.configureResponse=await page.evaluate(configure,{sharedResources,runtimeResources:[],musicMode:'midi',options:target.options??{}});
  recovery.observedMode=await nextFrame.evaluate(()=>window.__th08Runtime.Module.touhouMusicMode);
  await markStage('fault-recovery-midi-bridge');
  recovery.midiBridge=await installTh08MidiBridge(page,fixture,midiBridgeSource);
  await markStage('fault-recovery-launch');await page.evaluate(()=>rpc('launch'));
  await page.waitForFunction(()=>events.some(event=>event.event==='first-frame'),null,{timeout:120000});
  recovery.firstFrame=await page.evaluate(()=>events.find(event=>event.event==='first-frame'));
  await page.evaluate(()=>{window.__eaglerMidiRuntimeLaunched=true;document.querySelector('iframe')?.focus();});
  await page.waitForFunction(()=>window.__eaglerTinySynth?.getAudioContext?.().state==='running',null,{timeout:30000});
  recovery.audioReady=await page.evaluate(()=>{const context=window.__eaglerTinySynth.getAudioContext();return{state:context.state,currentTime:context.currentTime};});
  await markStage('fault-recovery-output');recovery.scene=await collectTh08MidiScene(nextFrame,page,th08TitleMidi);
  await page.screenshot({path:resolve(output,`th08-${modeRecord.mode}-midi-recovery.png`)});
  return nextFrame;
}

function midiBridgeFactorySource(){
  const moduleUrl=pathToFileURL(resolve(root,'launcher/app/services/midi-bridge.ts')).href;
  const script=`import {createMidiBridge} from ${JSON.stringify(moduleUrl)};process.stdout.write(createMidiBridge.toString());`;
  return execFileSync(process.execPath,['--experimental-strip-types','--input-type=module','-e',script],
    {cwd:root,encoding:'utf8',maxBuffer:1024*1024});
}

async function pressKey(page,code,holdMs=100){
  await page.evaluate(code=>rpc('keyboard',{code,down:true,inputSource:'physical'}),code);
  try{await page.waitForTimeout(holdMs);}finally{
    await page.evaluate(code=>rpc('keyboard',{code,down:false,inputSource:'physical'}),code);
  }
  await page.waitForTimeout(160);
}

async function waitTh08Observation(frame,predicate,{timeoutMs=8000,intervalMs=100,label='TH08 observation'}={}){
  for(let elapsed=0;elapsed<=timeoutMs;elapsed+=intervalMs){
    const result=await readTh08Sample(frame);
    if(predicate(result))return result;
    await frame.waitForTimeout(intervalMs);
  }
  throw Error(`${label} did not reach the required native state within ${timeoutMs}ms`);
}

async function readTh08Sample(frame,{audioRole=null}={}){
  const raw=await frame.evaluate(readNativeAudioState,{game:'th08',includeRuntimePcm:audioRole===TH08_OGG_PCM_ROLE,
    includeMidiHost:audioRole===TH08_MIDI_PCM_ROLE});
  const runtime=await frame.evaluate(readRuntimeObservation,runtimeObservationRequest({game:'th08',variant:'normal'}));
  return {native:raw.musicObservation,midi:raw.midiObservation,decoder:raw.decoder,runtime,runtimePcm:raw.runtimePcm??null,midiHost:raw.midiHost??null};
}

function hasMidiNote(bytes,kind){
  if(!Array.isArray(bytes)||bytes.length<3)return false;
  const status=bytes[0]&0xf0;
  return kind==='on'?status===0x90&&bytes[2]>0:status===0x80||(status===0x90&&bytes[2]===0);
}

async function collectTh08MidiScene(frame,page,expectedPath=th08TitleMidi,{nativeOffset=0,sendOffset=0}={}){
  const pcmWindows=[];let native=null,host=null,runtime=null,sendCorrelation=null;
  for(let sample=0;sample<80;sample++){
    const state=await frame.evaluate(readNativeAudioState,{game:'th08',includeMidiHost:true});
    native=state.midiObservation;host=state.midiHost;
    runtime=await frame.evaluate(readRuntimeObservation,runtimeObservationRequest({game:'th08',variant:'normal'}));
    if(host?.pcmTarget?.available===true)pcmWindows.push({target:host.pcmTarget,captures:host.captures});
    sendCorrelation=correlateTh08MidiSends((host.nativeEvents??[]).slice(nativeOffset),(host.sentMessages??[]).slice(sendOffset));
    const noteSent=sendCorrelation.events.some(event=>hasMidiNote(event.bytes,'on')&&hasMidiNote(event.sentBytes,'on'));
    const noteOffSent=sendCorrelation.events.some(event=>hasMidiNote(event.bytes,'off')&&hasMidiNote(event.sentBytes,'off'));
    const pcm=summarizeRuntimePcmWindow(pcmWindows,{role:TH08_MIDI_PCM_ROLE,
      expectedTarget:pcmWindows[0]?.target,minimumWindows:1});
    if((expectedPath==null||native?.path===expectedPath)&&native?.active&&native.invalid===false&&native.noteOnCount>0&&
      th08MidiTransportCount(native)===sendCorrelation.eventCount&&sendCorrelation.status==='matched'&&noteSent&&noteOffSent&&
      pcm.status==='complete'&&pcm.outputObserved)break;
    await page.waitForTimeout(100);
  }
  const pcm=summarizeRuntimePcmWindow(pcmWindows,{role:TH08_MIDI_PCM_ROLE,
    expectedTarget:pcmWindows[0]?.target,minimumWindows:1});
  return {native,runtime,events:sendCorrelation?.events??[],nativeEvents:(host?.nativeEvents??[]).slice(nativeOffset),
    synthSends:(host?.sentMessages??[]).slice(sendOffset),nativeOffset,sendOffset,sendCorrelation,contextState:host?.contextState??null,
    contextIndex:host?.contextIndex??-1,pcmTarget:host?.pcmTarget??null,pcm};
}

async function collectTh08OggScene(frame,page,resources,{expectedFile=null,rejectedSource=false,stoppedSource=false}={}){
  let raw=null,native=null;
  const readSample=async()=>{
    raw=await frame.evaluate(readNativeAudioState,{game:'th08',includeRuntimePcm:true});
    native={...raw.musicObservation,lastMusicSong:raw.midiObservation?.lastMusicSong??-1};
    const target=raw.runtimePcm?.target??null,captures=raw.runtimePcm?.captures??[];
    return {native,trackFile:th08OggTrackFile(native.name),target,captures,
      buffering:{queuedFrames:native.queuedFrames,queuedSampleRate:44100,outputBufferFrames:raw.runtimePcm?.outputBufferFrames,
        baseLatency:raw.runtimePcm?.baseLatency,analysisWindowFrames:Math.max(0,...captures.filter(capture=>
          capture.contextIndex===target?.contextIndex&&target.tapIds?.includes(capture.tapId)).map(capture=>capture.samples))}};
  };
  if(rejectedSource||stoppedSource){
    let drainBasis=null,settleFrames=4096;
    if(stoppedSource){
      const sample=await readSample(),target=sample.target;
      const captures=sample.captures.filter(capture=>capture.contextIndex===target?.contextIndex&&target.tapIds?.includes(capture.tapId));
      drainBasis={native:sample.native,target,outputBufferFrames:raw.runtimePcm?.outputBufferFrames,
        baseLatency:raw.runtimePcm?.baseLatency,analysisWindowFrames:Math.max(0,...captures.map(capture=>capture.samples))};
      settleFrames=th08MusicStopDrainFrames(drainBasis);
      assert.notEqual(settleFrames,null,'Stopped music needs the actual SDL queue and browser output buffering');
    }
    const {lastSample,collection}=await collectRuntimeOutputPcm({role:TH08_OGG_PCM_ROLE,readSample,
      settleFrames,wait:milliseconds=>page.waitForTimeout(milliseconds)});
    const {pcmWindow,...pcmCollection}=collection;
    const file=lastSample?.trackFile??null;
    return {native:lastSample?.native??native,decoder:raw?.decoder??null,trackFile:file,input:mapTrackToInput(file,resources),
      pcmTarget:lastSample?.target??null,pcm:pcmWindow??null,pcmCollection,...(drainBasis?{drainBasis}:{})};
  }
  const collected=await collectExpectedTrackRuntimePcm({role:TH08_OGG_PCM_ROLE,expectedFile,maxSamples:32,
    pollIntervalMs:RUNTIME_PCM_SAMPLE_INTERVAL_MS,wait:milliseconds=>page.waitForTimeout(milliseconds),
    readSample});
  native??=collected.lastSample?.native??null;
  const file=th08OggTrackFile(native?.name);
  const pcm=collected.pcmWindow;
  const {pcmWindow:_pcmWindow,lastSample:_lastSample,...pcmCollection}=collected;
  return {native,decoder:raw?.decoder??null,trackFile:file,input:mapTrackToInput(file,resources),
    pcmTarget:pcmCollection.target??raw?.runtimePcm?.target??null,pcm,
    pcmCollection:{...pcmCollection,windowCount:pcm?.windowCount??0,captureCount:pcm?.captureCount??0,
      firstCurrentTime:pcm?.firstCurrentTime??null,lastCurrentTime:pcm?.lastCurrentTime??null,
      elapsedFrames:pcm?.elapsedFrames??null,sampleRate:pcm?.sampleRate??null,
      outputObserved:pcm?.outputObserved??null,nonZeroSamples:pcm?.nonZeroSamples??null,
      target:pcmCollection.target??null}};
}

async function collectTh08BossMusicCue(page,frame,resources,recipe){
  const target={game:'th08',variant:'normal'},request=dialogueTouchSceneRequest(target),contract=request.contract;
  const arm=await frame.evaluate(controlValidationScene,validationSceneRequest({...target,action:'arm'}));assertControlledSceneArmed(arm);
  let runtime=await frame.evaluate(readRuntimeObservation,runtimeObservationRequest(target));
  for(let press=0;press<28&&!gameplayReached('th08',runtime);press++){
    await pressStart(page);runtime=await frame.evaluate(readRuntimeObservation,runtimeObservationRequest(target));
  }
  assert.ok(gameplayReached('th08',runtime),'Boss cue fixture did not reach gameplay');
  const initial=assertControlledScene(await frame.evaluate(controlValidationScene,validationSceneRequest(target)));
  const beforeAudio=await collectTh08OggScene(frame,page,resources,{expectedFile:recipe.beforeFile});
  const observe=async()=>{
    const raw=await frame.evaluate(readDialogueTouchWords,{...request,operation:'observation'});
    const native=normalizeDialogueTouchObservation(raw,contract);assert.equal(native.available,true,native.reason);
    const audio=await readTh08Sample(frame);
    return{runtime:native.runtime,frame:native.hostFrames,file:th08OggTrackFile(audio.native.name),request:native.request,native,music:audio.native,
      inputs:{fire:native.touch.fireEnabled,skip:Boolean(native.dialogue.currentInput&contract.skipMask)}};
  };
  const controls=fireEnabled=>page.evaluate(fireEnabled=>rpc('touch-controls',{fireEnabled,focusEnabled:false,bombSerial:0,
    escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100}),fireEnabled);
  const transition=await collectMusicCueTransition({
    readSource:async()=>{
      const raw=normalizeDialogueTouchManifest(await frame.evaluate(readDialogueTouchWords,{...request,operation:'manifest'}),contract);
      assert.equal(raw.available,true,raw.reason);
      const message=raw.source.message;
      return{runtime:raw.runtime,stage:raw.source.stage,identity:{asset:message.asset,bytes:message.byteLength,
        hash:{algorithm:'fnv1a64',value:message.fnv64},entryCount:message.entryCount},entries:raw.candidates.map(row=>row.entry),raw};
    },
    observe,
    request:async entry=>{
      const held=await controls(true);assert.equal(held.ok,true);
      const response=await frame.evaluate(requestDialogueTouchMessage,{contract,entry});
      if(response.accepted)await page.evaluate(()=>rpc('keyboard',{code:'ControlLeft',down:true,inputSource:'physical'}));
      return response;
    },
    wait:()=>page.waitForTimeout(20),
    release:async()=>{
      await page.evaluate(()=>rpc('keyboard',{code:'ControlLeft',down:false,inputSource:'physical'}));await controls(false);
    },
  },recipe);
  const audio=await collectTh08OggScene(frame,page,resources,{expectedFile:recipe.afterFile});
  const final=assertControlledScene(await frame.evaluate(controlValidationScene,validationSceneRequest(target)));
  return{transition,beforeAudio,audio,controlled:{arm,initial,final}};
}

async function collectTh08MusicRoom(page,frame,resources,catalog){
  const result={tracks:[],loop:null,fade:null,inputs:[]};
  const inputRequest=sfxLatencySceneRequest({game:'th08',variant:'normal'});
  const masks={ArrowDown:32,ArrowUp:16,KeyZ:1,KeyR:16384,ControlLeft:256,Escape:8};
  const press=async code=>result.inputs.push(await pressConsumedRuntimeKey({code,mask:masks[code],
    readInput:()=>frame.evaluate(readSfxLatencySceneState,inputRequest),
    setKey:(code,down)=>page.evaluate(({code,down})=>rpc('keyboard',{code,down,inputSource:'physical'}),{code,down}),
    wait:ms=>page.waitForTimeout(ms)}));
  // Native title cursor anchors each key press; no direct menu or audio calls.
  for(let step=0;step<12;step++){
    const sample=await readTh08Sample(frame);
    if(sample.runtime?.status?.cursor===6)break;
    await press('ArrowDown');
  }
  assert.equal((await readTh08Sample(frame)).runtime?.status?.cursor,6,'Music Room title cursor');
  await press('KeyZ');
  result.entered=(await waitTh08Observation(frame,sample=>sample.runtime?.status?.supervisorActive===8,{label:'Music Room entry'})).runtime;
  await page.waitForTimeout(500);
  for(const track of catalog){
    await press('KeyZ');
    const scene=await collectTh08OggScene(frame,page,resources,{expectedFile:track.file});
    scene.observation=(await readTh08Sample(frame)).runtime;scene.index=track.index;
    result.tracks.push(scene);await press('ArrowDown');
  }
  const shortest=catalog.reduce((a,b)=>a.totalFrames/a.sampleRate<=b.totalFrames/b.sampleRate?a:b);
  // One complete table walk wraps back to cursor zero.
  for(let index=0;index<shortest.index;index++)await press('ArrowDown');
  // Selecting the different loop track starts it once; Reset is exercised after fade.
  await press('KeyZ');
  const loop=result.loop={index:shortest.index,samples:[],before:null,after:null};
  loop.before=await collectTh08OggScene(frame,page,resources,{expectedFile:shortest.file});
  const deadline=Date.now()+Math.ceil(shortest.totalFrames/shortest.sampleRate*1000)+5000;
  while(Date.now()<deadline){
    const {native,runtimePcm}=await readTh08Sample(frame,{audioRole:TH08_OGG_PCM_ROLE});
    loop.samples.push({...native,at:Date.now(),audioTime:runtimePcm?.target?.currentTime??null});
    if(loop.samples.length>1&&native.cursorFrames<loop.samples.at(-2).cursorFrames)break;
    await page.waitForTimeout(100);
  }
  loop.after=await collectTh08OggScene(frame,page,resources,{expectedFile:shortest.file});
  const fade=result.fade={samples:[],quiet:null,reset:null};
  fade.samples.push({native:(await readTh08Sample(frame)).native,at:Date.now()});
  await press('ControlLeft');
  for(let index=0;index<100;index++){
    const native=(await readTh08Sample(frame)).native;fade.samples.push({native,at:Date.now()});
    if(native?.playing===false)break;
    await page.waitForTimeout(100);
  }
  // Drain output independently after the production fade has stopped its source.
  fade.quiet=await collectTh08OggScene(frame,page,resources,{stoppedSource:true});
  await press('KeyR');
  fade.reset=await collectTh08OggScene(frame,page,resources,{expectedFile:shortest.file});
  await press('Escape');
  result.exited=(await waitTh08Observation(frame,sample=>sample.runtime?.status?.supervisorActive===1,{label:'Music Room exit'})).runtime;
  for(let step=0;step<12;step++){
    if((await readTh08Sample(frame)).runtime?.status?.cursor===0)break;
    await press('ArrowUp');
  }
  assert.equal((await readTh08Sample(frame)).runtime?.status?.cursor,0,'Return to Start title cursor');
  return result;
}

async function pauseTh08Gameplay(page,frame,mode){
  const before=await readTh08Sample(frame);
  if(before.runtime?.worldProgress?.comparable!==true)throw Error('TH08 pause scene requires native comparable gameplay');
  await pressKey(page,'Escape',120);
  const paused=await waitTh08Observation(frame,state=>state.runtime?.worldProgress?.state?.paused===true,
    {label:'TH08 gameplay pause'});
  await page.waitForTimeout(350);
  const settled=await readTh08Sample(frame);
  await pressKey(page,'Escape',120);
  const resumed=await waitTh08Observation(frame,state=>state.runtime?.worldProgress?.state?.paused===false,
    {label:'TH08 gameplay resume'});
  if(mode==='ogg'){
    const observationInput={before:before.native,paused:paused.native,settled:settled.native,resumed:resumed.native,
      pauseEntered:true,resumeExited:true};
    return {...th08OggPauseOutcome(observationInput),observationInput,
      decoderSamples:[before.decoder,paused.decoder,settled.decoder,resumed.decoder]};
  }
  const bridgeContext=await page.evaluate(()=>window.__eaglerTinySynth?.getAudioContext?.().state??null);
  return th08MidiPauseOutcome({before:before.midi,paused:paused.midi,resumed:resumed.midi,
    pauseEntered:true,resumeExited:true,bridgeContext});
}

async function backgroundTh08Gameplay(page,frame,mode){
  const before=await readTh08Sample(frame);
  const bridgeBefore=mode==='midi'?await page.evaluate(()=>window.__eaglerTinySynth?.getAudioContext?.().state??null):null;
  await setTh08Hidden(frame,page,true);
  const hiddenState=await waitTh08Observation(frame,state=>mode==='ogg'?state.native?.outputPaused===true:
    state.midi?.active===true&&state.runtime?.worldProgress?.state?.loading===false,
    {label:`TH08 ${mode} hidden state`});
  await page.waitForTimeout(350);
  const settled=await readTh08Sample(frame);
  const bridgePaused=mode==='midi'?await page.evaluate(()=>window.__eaglerTinySynth?.getAudioContext?.().state??null):null;
  await setTh08Hidden(frame,page,false);
  const resumed=await waitTh08Observation(frame,state=>mode==='ogg'?state.native?.outputPaused===false&&state.native?.playing===true:
    state.midi?.active===true&&state.midi?.elapsedTicks>settled.midi?.elapsedTicks,
    {label:`TH08 ${mode} foreground recovery`});
  const resumedOutput=mode==='ogg'?await collectTh08OggScene(frame,page,[],{expectedFile:th08OggTrackFile(before.native?.name)}):
    await collectTh08MidiScene(frame,page,before.midi?.path);
  const hiddenTag=value=>({...value,visibilityState:'hidden'}),visibleTag=value=>({...value,visibilityState:'visible'});
  const observationInput=mode==='ogg'?{mode,before:before.native,paused:hiddenTag(hiddenState.native),
    settled:hiddenTag(settled.native),resumed:visibleTag(resumed.native),resumedOutput}:{mode,before:before.midi,paused:hiddenTag(hiddenState.midi),
    settled:hiddenTag(settled.midi),resumed:visibleTag(resumed.midi),bridgeBefore,bridgePaused,
    resumedOutput,
    bridgeResumed:await page.evaluate(()=>window.__eaglerTinySynth?.getAudioContext?.().state??null)};
  const outcome=th08BackgroundContinuityOutcome(observationInput);
  return {...outcome,observationInput,...(mode==='ogg'?{decoderSamples:[before.decoder,hiddenState.decoder,settled.decoder,resumed.decoder,resumedOutput.decoder]}:{}),lifecycleSignal:'document.hidden override plus dispatched visibilitychange in host and Runtime documents'};
}

/** Bind the actual Playwright Runtime frame to its same-origin host iframe element. */
async function bindMusicRuntimeFrame(page,frame){
  const iframe=await frame.frameElement(),href=frame.url(),hostOrigin=new URL(page.url()).origin;
  await page.waitForFunction(({iframe,href,hostOrigin})=>{
    try{
      const target=iframe?.contentWindow,doc=iframe?.contentDocument;
      return iframe?.isConnected===true&&!!target&&!!doc&&target.location.href===href&&target.location.origin===hostOrigin&&
        doc.defaultView===target&&doc.readyState==='complete';
    }catch{return false;}
  },{iframe,href,hostOrigin},{timeout:15000});
  return page.evaluate(({iframe,href,hostOrigin})=>{
    const target=iframe?.contentWindow,doc=iframe?.contentDocument;
    if(!iframe?.isConnected||!target||!doc||target.location.href!==href||target.location.origin!==hostOrigin||
      doc.defaultView!==target||doc.readyState!=='complete')throw Error('Resolved Runtime frame no longer matches its host iframe identity');
    const epoch=new URL(href).searchParams.get('runtimeEpoch');
    if(!epoch||!Number.isSafeInteger(Number(epoch))||Number(epoch)<=0)throw Error('Resolved Runtime frame has no positive runtimeEpoch identity');
    window.__eaglerMusicRuntimeFrameBinding={iframe,target,document:doc,href,hostOrigin,epoch:Number(epoch)};
    return {href,hostOrigin,epoch:Number(epoch),readyState:doc.readyState};
  },{iframe,href,hostOrigin});
}

async function runtimeFrameStillBound(page){
  return page.evaluate(()=>{
    const binding=window.__eaglerMusicRuntimeFrameBinding;
    try{return !!binding&&binding.iframe?.isConnected===true&&binding.iframe.contentWindow===binding.target&&
      binding.iframe.contentDocument===binding.document&&binding.target.location.href===binding.href&&
      binding.target.location.origin===binding.hostOrigin&&binding.document.readyState==='complete';}
    catch{return false;}
  });
}

async function readTh08MidiBridgeState(page){
  return page.evaluate(()=>({
    closeEvents:window.__eaglerMidiCloseEvents?.length??0,
    lastCloseSentMessages:window.__eaglerMidiCloseEvents?.at(-1)?.sentMessages??null,
    lastCloseNativeEvents:window.__eaglerMidiCloseEvents?.at(-1)?.nativeEvents??null,
    lastCloseResetReceipt:window.__eaglerMidiCloseEvents?.at(-1)?.resetReceipt??null,
    closeResetReceipts:window.__eaglerMidiCloseEvents?.map(event=>event.resetReceipt??null)??[],
    ownedAudioSources:window.__eaglerMidiBridge?.getOwnedAudioSourceSnapshot?.()??null,
    resetCount:window.__eaglerTinySynthResetCount??0,
    activeVoices:window.__eaglerTinySynth?.notetab?.length??null,
    sentMessages:window.__eaglerTinySynthSends?.length??0,
    nativeEvents:window.__eaglerMidiNativeEvents?.length??0,
  }));
}

async function quietRuntimePcmWindow(frame,{role,expectedTarget,settleFrames=4096,retainCaptureRows=false}={}){
  const request=role===TH08_OGG_PCM_ROLE?{game:'th08',includeRuntimePcm:true}:
    [TH08_MIDI_PCM_ROLE,TH08_MIDI_VOICE_PCM_ROLE].includes(role)?{game:'th08',includeMidiHost:true}:null;
  if(!request)return {status:'incomplete',proof:'none',role,reason:'unsupported Runtime PCM target role'};
  return collectRuntimePcmWindow({role,expectedTarget,windowCount:8,settleFrames,retainCaptureRows,
    readWindow:async()=>{
      const state=await frame.evaluate(readNativeAudioState,request);
      const source=role===TH08_OGG_PCM_ROLE?state.runtimePcm:state.midiHost;
      const target=role===TH08_OGG_PCM_ROLE?source?.target:role===TH08_MIDI_VOICE_PCM_ROLE?source?.voicePcmTarget:source?.pcmTarget;
      return {target,captures:source?.captures??[],relatedTargets:role===TH08_MIDI_PCM_ROLE?{voicePcmTarget:source?.voicePcmTarget}:null};
    },wait:milliseconds=>frame.waitForTimeout(milliseconds)});
}

async function restartTh08Runtime(page,frame,mode,resources){
  const frameUrl=frame.url(),beforeBinding=await runtimeFrameStillBound(page);
  if(!beforeBinding)throw Error(`TH08 ${mode} restart began after Runtime frame identity changed`);
  const audioRole=mode==='ogg'?TH08_OGG_PCM_ROLE:TH08_MIDI_PCM_ROLE;
  const beforeState=mode==='ogg'?await collectTh08OggScene(frame,page,resources,{expectedFile:th08FirstStageOgg}):
    await readTh08Sample(frame,{audioRole});
  let beforeBridge=null;
  if(mode==='midi'){
    await page.waitForFunction(()=>window.__eaglerTinySynth?.notetab?.length>0,null,{timeout:12000});
    beforeBridge=await readTh08MidiBridgeState(page);
  }
  const priorExitCount=await page.evaluate(()=>events.filter(event=>event.event==='exit').length);
  const priorFirstFrameCount=await page.evaluate(()=>events.filter(event=>event.event==='first-frame').length);
  await frame.evaluate(()=>{
    if(typeof window.__th08Runtime?.stop!=='function')throw Error('TH08 Runtime close operation is unavailable');
    return window.__th08Runtime.stop();
  });
  await page.waitForFunction(prior=>events.filter(event=>event.event==='exit').length>prior,priorExitCount,{timeout:30000});
  const sameRuntimeAfterExit=await runtimeFrameStillBound(page);
  const quietWindow=await quietRuntimePcmWindow(frame,{role:audioRole,
    expectedTarget:mode==='ogg'?beforeState.pcmTarget:beforeState.midiHost?.pcmTarget,settleFrames:4096,retainCaptureRows:mode==='midi'});
  const stoppedDecoder=mode==='ogg'?(await frame.evaluate(readNativeAudioState,{game:'th08'})).decoder:null;
  let midiClose=null,voicePcmWindow=null;
  if(mode==='midi'){
    midiClose=await readTh08MidiBridgeState(page);
    voicePcmWindow=summarizeRelatedRuntimePcmWindow(quietWindow,{role:TH08_MIDI_VOICE_PCM_ROLE,
      relatedTargetKey:'voicePcmTarget',expectedTarget:beforeState.midiHost?.voicePcmTarget,minimumWindows:8});
    delete quietWindow.captureRows;
  }
  await frame.evaluate(()=>{
    if(typeof window.__th08Runtime?.launch!=='function')throw Error('TH08 Runtime relaunch operation is unavailable');
    return window.__th08Runtime.launch();
  });
  await page.waitForFunction(prior=>events.filter(event=>event.event==='first-frame').length>prior,priorFirstFrameCount,{timeout:120000});
  await page.waitForTimeout(700);
  const sameRuntimeAfterLaunch=await runtimeFrameStillBound(page);
  if(frame.url()!==frameUrl)throw Error(`TH08 ${mode} restart navigated to a different Runtime URL`);
  if(mode==='midi'){
    const reopened=await collectTh08MidiScene(frame,page,th08TitleMidi,
      {nativeOffset:midiClose?.lastCloseNativeEvents??0,sendOffset:midiClose?.lastCloseSentMessages??0});
    const reopenedState=await readTh08MidiBridgeState(page);
    const reopenedNativeEvents=reopened.nativeEvents;
    const reopenedSends=reopened.synthSends;
    const reopenedCorrelation=correlateTh08MidiSends(reopenedNativeEvents,reopenedSends);
    const reopenedEvents=reopenedCorrelation.events;
    const outcome=th08MidiRestartOutcome({before:beforeState.midi,closeEventsBefore:beforeBridge?.closeEvents,
      closeEventsAfter:midiClose?.closeEvents,resetCountBefore:beforeBridge?.resetCount,resetCountAfter:midiClose?.resetCount,
      activeVoicesBefore:beforeBridge?.activeVoices,activeVoicesAfter:midiClose?.activeVoices,
      closeResetReceipts:midiClose?.closeResetReceipts?.slice(beforeBridge?.closeEvents??0),
      postCloseMessages:Math.max(0,(midiClose?.sentMessages??0)-(midiClose?.lastCloseSentMessages??midiClose?.sentMessages??0)),
      postCloseNativeEvents:Math.max(0,(midiClose?.nativeEvents??0)-(midiClose?.lastCloseNativeEvents??0)),
      quietWindow,voicePcmWindow,reopenedEvents,reopenedCorrelation,reopened,sameRuntimeFrame:sameRuntimeAfterExit&&sameRuntimeAfterLaunch,exitObserved:true});
    return {...outcome,beforeState:beforeState.midi,closeState:midiClose,quietWindow,voicePcmWindow,reopenedState:reopened.native,
      reopenedPcm:reopened.pcm,frame:{url:frameUrl,sameRuntimeAfterExit,sameRuntimeAfterLaunch},
      reopenedEventCount:reopenedEvents.length,reopenedNativeEventCount:reopenedNativeEvents.length,
      reopenedSendCount:reopenedSends.length,hostEventsAfterCollect:reopenedState};
  }
  const reopened=await collectTh08OggScene(frame,page,resources,{expectedFile:th08TitleOgg});
  const before={native:beforeState.native,trackFile:beforeState.trackFile,input:beforeState.input,pcm:beforeState.pcm,
    pcmCollection:beforeState.pcmCollection};
  const observationInput={before,quietWindow,reopened,expectedBeforeFile:th08FirstStageOgg,expectedFile:th08TitleOgg,
    sameRuntimeFrame:sameRuntimeAfterExit&&sameRuntimeAfterLaunch,exitObserved:true};
  const outcome=th08OggRestartOutcome(observationInput);
  return {...outcome,observationInput,decoderSamples:[beforeState.decoder,stoppedDecoder,reopened.decoder],beforeState:beforeState.native,beforePcmCollection:beforeState.pcmCollection,
    reopenedState:reopened.native,reopenedPcm:reopened.pcm,reopenedPcmCollection:reopened.pcmCollection,
    frame:{url:frameUrl,sameRuntimeAfterExit,sameRuntimeAfterLaunch}};
}

async function installTh08MidiBridge(page,fixture,bridgeSource){
  const scriptUrl=`${fixture.base}/launcher/vendor/webaudio-tinysynth.min.js`;
  await page.addScriptTag({url:scriptUrl});
  const setup=await page.evaluate(({factorySource})=>{
    const binding=window.__eaglerMusicRuntimeFrameBinding;
    const frame=binding?.iframe;
    if(!frame?.contentWindow||!frame.contentDocument||frame.contentWindow!==binding.target||frame.contentDocument!==binding.document||
      frame.contentWindow.location.href!==binding.href||frame.contentWindow.location.origin!==binding.hostOrigin||
      frame.contentDocument.readyState!=='complete')throw Error('Resolved same-origin Runtime frame identity is unavailable for MIDI bridge');
    if(typeof window.WebAudioTinySynth!=='function')throw Error('The verified TinySynth vendor did not register its production constructor');
    window.__eaglerTinySynthSends=[];window.__eaglerMidiNativeEvents=[];window.__eaglerMidiCloseEvents=[];
    window.__eaglerTinySynthResetCount=0;window.__eaglerTinySynthResetReceipts=[];window.__eaglerMidiRuntimeLaunched=false;
    const context={epoch:binding.epoch,game:'th08',document:frame.contentDocument,target:frame.contentWindow,music:'midi'};
    const runtime={getMidiEventContext:()=>context,getSnapshot:()=>({launched:window.__eaglerMidiRuntimeLaunched}),subscribe:()=>()=>{}};
    const external={prepare:async()=>{},deliver:()=>false,panic:()=>{}};
    const sameContext=(a,b)=>a===b||!!a&&!!b&&a.epoch===b.epoch&&a.game===b.game&&a.document===b.document&&a.target===b.target;
    const createMidiBridge=new Function('sameContext',`return (${factorySource})`)(sameContext);
    const RealSynth=window.WebAudioTinySynth;
    const observedSynth=new Proxy(RealSynth,{construct(target,args){
      const synth=Reflect.construct(target,args,target);window.__eaglerTinySynth=synth;
      const send=synth.send.bind(synth);synth.send=message=>{const bytes=Array.from(message||[]);const result=send(message);
        window.__eaglerTinySynthSends.push(bytes);return result;};
      synth.__eaglerMusicVoiceTap=window.__eaglerMusicPcmProbe?.attachTap?.(synth.out,'launcher-tinysynth-voice',
        'tinysynth-voice-output')??null;
      const reset=synth.reset.bind(synth);synth.reset=()=>{
        window.__eaglerTinySynthResetCount++;
        const context=synth.getAudioContext?.(),buffer=synth.conv?.buffer;
        const receipt={kind:'tinysynth-reset',resetCount:window.__eaglerTinySynthResetCount,contextTime:context?.currentTime??null,
          sampleRate:context?.sampleRate??null,reverbEnabled:!!(synth.conv&&synth.rev),reverbBufferFrames:Number.isSafeInteger(buffer?.length)?buffer.length:null,
          activeVoicesBefore:Array.isArray(synth.notetab)?synth.notetab.length:null};
        Object.defineProperty(receipt,'reverbConvolverBefore',{value:synth.conv,configurable:true});
        Object.defineProperty(receipt,'reverbGainBefore',{value:synth.rev,configurable:true});
        Object.defineProperty(receipt,'compressorBefore',{value:synth.comp,configurable:true});
        Object.defineProperty(receipt,'impulseBefore',{value:buffer,configurable:true});
        const result=reset();receipt.activeVoicesAfter=Array.isArray(synth.notetab)?synth.notetab.length:null;
        window.__eaglerTinySynthResetReceipts.push(receipt);return result;
      };
      return synth;
    }});
    const bridge=createMidiBridge({runtime,external,loadSynth:async()=>observedSynth,webAudioAvailable:true,translate:key=>key,
      frame,window,document,playerOpen:()=>true});
    frame.contentWindow.addEventListener('touhou-midi',event=>{
      const detail=event?.detail;
      window.__eaglerMidiNativeEvents.push({bytes:Array.isArray(detail?.bytes)?detail.bytes.slice():[],time:Number(detail?.time)||0});
    });
    frame.contentWindow.addEventListener('touhou-midi-close',()=>{
      const synth=window.__eaglerTinySynth,receipt=window.__eaglerTinySynthResetReceipts.at(-1)??null;
      if(receipt){
        const beforeConvolver=receipt.reverbConvolverBefore,afterConvolver=synth?.conv;
        const beforeGain=receipt.reverbGainBefore,afterGain=synth?.rev;
        const beforeCompressor=receipt.compressorBefore,afterCompressor=synth?.comp;
        const buffer=afterConvolver?.buffer,probe=window.__eaglerMusicPcmProbe;
        receipt.reverbBufferFramesAfter=Number.isSafeInteger(buffer?.length)?buffer.length:0;
        receipt.convolverNodeReplaced=!!beforeConvolver&&!!afterConvolver&&beforeConvolver!==afterConvolver&&
          receipt.impulseBefore===buffer&&beforeConvolver.normalize===afterConvolver.normalize;
        receipt.reverbGainNodeReplaced=!!beforeGain&&!!afterGain&&beforeGain!==afterGain&&
          beforeGain.gain?.value===afterGain.gain?.value;
        receipt.wetPathReplaced=receipt.convolverNodeReplaced&&receipt.reverbGainNodeReplaced&&
          beforeCompressor===afterCompressor&&receipt.reverbEnabled===true;
        receipt.wetPathBefore={convolver:probe?.identifyNode?.(beforeConvolver,'tinysynth-reverb-convolver')??null,
          reverbGain:probe?.identifyNode?.(beforeGain,'tinysynth-reverb-gain')??null,
          compressor:probe?.identifyNode?.(beforeCompressor,'tinysynth-compressor-output')??null};
        receipt.wetPathAfter={convolver:probe?.identifyNode?.(afterConvolver,'tinysynth-reverb-convolver')??null,
          reverbGain:probe?.identifyNode?.(afterGain,'tinysynth-reverb-gain')??null,
          compressor:probe?.identifyNode?.(afterCompressor,'tinysynth-compressor-output')??null};
        delete receipt.reverbConvolverBefore;delete receipt.reverbGainBefore;delete receipt.compressorBefore;delete receipt.impulseBefore;
      }
      window.__eaglerMidiCloseEvents.push({sentMessages:window.__eaglerTinySynthSends.length,
        nativeEvents:window.__eaglerMidiNativeEvents.length,activeVoices:synth?.notetab?.length??null,
        resetCount:window.__eaglerTinySynthResetCount,resetReceipt:receipt,
        ownedAudioSources:window.__eaglerMidiBridge?.getOwnedAudioSourceSnapshot?.()??null});
    });
    window.__eaglerMidiBridge=bridge;window.__eaglerMidiBridgeIdentity={epoch:context.epoch,game:context.game,
      runtimeUrl:frame.contentWindow.location.href,hostOrigin:binding.hostOrigin,vendorConstructor:RealSynth.name};
    return window.__eaglerMidiBridgeIdentity;
  },{factorySource:bridgeSource});
  await page.evaluate(()=>window.__eaglerMidiBridge.prepare('midi'));
  await page.waitForFunction(()=>window.__eaglerTinySynth?.getAudioContext?.().state==='running',null,{timeout:15000});
  await page.waitForTimeout(500);
  await page.evaluate(()=>{window.__eaglerTinySynthSends=[];window.__eaglerMidiNativeEvents=[];});
  return setup;
}

async function setTh08Hidden(frame,page,hidden){
  await frame.evaluate(hidden=>{
    if(!Object.prototype.hasOwnProperty.call(document,'__th08AuditHidden')){
      Object.defineProperty(document,'__th08AuditHidden',{configurable:true,writable:true,value:false});
      Object.defineProperty(document,'hidden',{configurable:true,get(){return this.__th08AuditHidden;}});
      Object.defineProperty(document,'visibilityState',{configurable:true,get(){return this.__th08AuditHidden?'hidden':'visible';}});
    }
    document.__th08AuditHidden=hidden;document.dispatchEvent(new Event('visibilitychange'));
  },hidden);
  await page.evaluate(hidden=>{
    if(!Object.prototype.hasOwnProperty.call(document,'__th08AuditHidden')){
      Object.defineProperty(document,'__th08AuditHidden',{configurable:true,writable:true,value:false});
      Object.defineProperty(document,'hidden',{configurable:true,get(){return this.__th08AuditHidden;}});
      Object.defineProperty(document,'visibilityState',{configurable:true,get(){return this.__th08AuditHidden?'hidden':'visible';}});
    }
    document.__th08AuditHidden=hidden;document.dispatchEvent(new Event('visibilitychange'));
  },hidden);
  if(!hidden)await page.evaluate(()=>document.querySelector('iframe')?.focus());
  await frame.waitForTimeout(hidden?250:500);
}

function trackMapFromTh11(source) {
  return Object.fromEntries([...source.matchAll(/\{\s*(\d+)\s*,\s*"([A-Za-z0-9_]+)"\s*\}/g)].map(([,cue,stem])=>[cue,stem]));
}

function gameplayReached(game,observation) {
  if(game==='th08')return observation.worldProgress?.comparable===true;
  if(game==='th10')return observation.status?.screen===7;
  if(game==='th11'||game==='th15')return observation.phase?.name==='stage';
  if(game==='th20')return observation.phase?.name==='stage';
  return false;
}

async function pressStart(page) {
  await page.evaluate(()=>rpc('keyboard',{code:'KeyZ',down:true,inputSource:'physical'}));
  try{await page.waitForTimeout(120);}finally{
    await page.evaluate(()=>rpc('keyboard',{code:'KeyZ',down:false,inputSource:'physical'}));
  }
  await page.waitForTimeout(180);
}

const result=await runWithReport({output,schema:'eagler-touhou/music-browser/1'},async(report,save)=>{
  assertCliArguments(process.argv.slice(2),{values:['output','manifest','game','music-manifest','variant','packages','artwork-dir'],flags:['plan-only','launcher-ui','launcher-ui-only']});
  assert.ok(requestedOutput,'Required: --output=report-directory');
  if(outputInspectionError)throw outputInspectionError;
  assert.ok(!outputExists||outputIsDirectory,'--output must name a directory');
  assert.equal(arg('variant'),'normal','music-browser requires --variant=normal and real normal Runtime packages');
  const manifestArgument=arg('manifest'),musicArgument=arg('music-manifest'),gameArgument=arg('game');
  assert.ok(manifestArgument,'Required: --manifest=accepted-private-inputs.json');
  assert.ok(musicArgument,'Required: --music-manifest=private-local-music-inputs.json');
  assert.ok(gameArgument,'Required: --game=explicit-comma-separated-game-ids');
  const games=gameArgument.split(',');
  const launcherOnly=process.argv.includes('--launcher-ui-only'),launcherUi=launcherOnly||process.argv.includes('--launcher-ui');
  let launcherPackageMap=null,launcherPackageBytes=null,launcherPackagePath=null,launcherArtwork=null;
  const launcherArchives={};
  if(launcherUi){
    assert.deepEqual(games,['th08'],'Register title-owned Launcher music observations before selecting another product');
    assert.ok(arg('packages')&&arg('artwork-dir'),'Launcher music UI requires --packages and --artwork-dir');
    launcherPackagePath=resolve(arg('packages'));launcherPackageBytes=await readFile(launcherPackagePath);
    launcherPackageMap=JSON.parse(launcherPackageBytes.toString('utf8'));launcherArtwork=resolve(arg('artwork-dir'));
    for(const game of games){
      assert.equal(typeof launcherPackageMap?.[game],'string',`${game}: Launcher music ZIP input is missing`);
      const archive=await inspectOfflineUpdateDescriptor(launcherPackageMap[game]);
      assert.equal(archive.descriptor.game,game,`${game}: Launcher music ZIP belongs to another product`);
      launcherArchives[game]=archive;
    }
  }
  assert.ok(games.length&&games.every(Boolean),'--game must contain comma-separated game IDs');
  assert.equal(new Set(games).size,games.length,'--game must not contain duplicate game IDs');
  for(const game of games)assert.ok(MUSIC_BROWSER_GAMES.includes(game),`Unsupported music acceptance game ${game}`);
  const manifestPath=resolve(manifestArgument),musicManifestPath=resolve(musicArgument);
  const manifestBytes=await readFile(manifestPath),config=JSON.parse(manifestBytes.toString('utf8'));
  const selection=selectRuntimeTargets(config,{game:games.join(','),variant:'normal'});
  const musicManifestBytes=await readFile(musicManifestPath),musicManifest=JSON.parse(musicManifestBytes.toString('utf8'));
  const musicInputs=await verifyMusicInputs(musicManifest,{repoRoot:root,games});
  const launcherMusicPreflight={};
  if(launcherUi){
    for(const game of games){
      const archive=launcherArchives[game];
      const music=await verifyLauncherFixtureMusic({game,input:musicInputs[game],descriptor:archive.descriptor});
      launcherMusicPreflight[game]={path:archive.path,archiveSha256:archive.archiveSha256,music};
    }
    report.launcherMusicPreflight=launcherMusicPreflight;
    await save();
  }
  const packageBuilds=selection.targets.map(async target=>({target,identity:{
    runtime:await verifyRuntimePackage(target,{repoRoot:root}),data:await verifyPrivateData(target.data)}}));
  const verifiedPackages=await Promise.all(packageBuilds);
  const mappingGames=selectedMusicMappingGames(games);
  let th11Tracks={},th15Tracks=[];
  const musicMapEvidence={};
  const guardedSources={
    productCatalog:{path:'common/contracts/product-catalog.mjs',file:resolve(root,'common/contracts/product-catalog.mjs')},
    acceptanceContracts:{path:'tools/ports/acceptance-contracts.mjs',file:resolve(root,'tools/ports/acceptance-contracts.mjs')},
    nativeObservationModule:{path:'tools/ports/runtime-observations.mjs',file:resolve(root,'tools/ports/runtime-observations.mjs')},
    responseCapture:{path:'tools/browser/response-body-capture.mjs',file:resolve(root,'tools/browser/response-body-capture.mjs')},
    cliReport:{path:'tools/testing/cli-report.mjs',file:resolve(root,'tools/testing/cli-report.mjs')},
    cliOptions:{path:'tools/testing/cli-options.mjs',file:resolve(root,'tools/testing/cli-options.mjs')},
    browserLaunch:{path:'tools/browser/launch.mjs',file:resolve(root,'tools/browser/launch.mjs')},
    resultModel:{path:'tools/ports/music-browser-result.mjs',file:resolve(root,'tools/ports/music-browser-result.mjs')},
  };
  if(launcherUi){
    const dependencies=await dependencySnapshot(root,['tools/ports/launcher-music-journey.mjs']);
    for(const [index,source] of dependencies.entries())guardedSources[`launcherMusicDependency${index}`]={path:source.path,file:resolve(root,source.path)};
  }
  if(mappingGames.includes('th11')){
    const path=resolve(root,'ports/th11/web/cpp/game/MusicCatalog.hpp'),bytes=await readFile(path);
    th11Tracks=trackMapFromTh11(bytes.toString('utf8'));
    assert.ok(Object.keys(th11Tracks).length>0,'TH11 native music cue table was not found');
    guardedSources.th11MusicCatalog={path:'ports/th11/web/cpp/game/MusicCatalog.hpp',file:path};
    musicMapEvidence.th11={source:'ports/th11/web/cpp/game/MusicCatalog.hpp',sha256:hash(bytes),cueCount:Object.keys(th11Tracks).length};
  }
  if(mappingGames.includes('th15')){
    const path=resolve(root,'ports/th15/web/assets/music/manifest.json'),bytes=await readFile(path);
    const catalog=JSON.parse(bytes.toString('utf8'));
    th15Tracks=catalog.tracks.map(track=>track.filename.replace(/\.wav$/i,'.ogg'));
    guardedSources.th15MusicManifest={path:'ports/th15/web/assets/music/manifest.json',file:path};
    musicMapEvidence.th15={source:'ports/th15/web/assets/music/manifest.json',sha256:hash(bytes),layoutSha256:catalog.layoutSha256,trackCount:th15Tracks.length};
  }
  const th08Selected=games.includes('th08');
  let th08Catalog=[],th08BossRecipe=null;
  if(th08Selected){
    const item=verifiedPackages.find(item=>item.target.game==='th08');
    await mkdir(output,{recursive:true});
    const entries={};
    for(const [key,entry] of Object.entries({comments:'musiccmt.txt',format:'thbgm.fmt',bossMessage:'msg1a.dat',stage:'stage1.std'})){
      const destination=resolve(output,`th08-${key}.bin`);
      execFileSync('python',[resolve(root,'launcher/scripts/touhou_formats.py'),'extract-pbgz-entry','--archive',item.target.data,'--entry',entry,'--output',destination],{cwd:root,maxBuffer:1024*1024});
      const bytes=await readFile(destination);entries[key]={entry,path:destination,bytes:bytes.length,sha256:hash(bytes),content:bytes};
    }
    assert.deepEqual(await verifyPrivateData(item.target.data),item.identity.data,'DATA changed during independent music extraction');
    th08Catalog=parseTh08MusicCatalog(entries.comments.content,entries.format.content).map(track=>{
      const input=mapTrackToInput(track.file,musicInputs.th08.files);assert.ok(input,`Missing catalog OGG ${track.file}`);return{...track,input};
    });
    const parsed=parseTh08MessageMusicCues(entries.bossMessage.content,entries.stage.content,0);
    const cue=parsed.cues.find(cue=>cue.slot===1);assert.ok(cue,'Stage 1 message has no Boss music cue');
    const beforeFile=parsed.stage.paths[0].slice(4).replace(/\.mid$/,'.ogg');assert.equal(beforeFile,th08FirstStageOgg);
    assert.ok(th08Catalog.some(track=>track.file===cue.file),'Boss cue is absent from the independent Music Room catalog');
    th08BossRecipe={stage:0,entry:cue.entry,beforeFile,afterFile:cue.file,maxPolls:600,releaseInputs:['fire','skip'],maxCleanupPolls:30,
      acceptedStatus:dialogueTouchSceneRequest({game:'th08',variant:'normal'}).contract.request.acceptedStatus,
      cueSource:{asset:entries.bossMessage.entry,bytes:parsed.message.bytes,hash:{algorithm:'fnv1a64',value:parsed.message.fnv64},entryCount:parsed.message.entryCount}};
    musicMapEvidence.th08={data:item.identity.data,entries:Object.fromEntries(Object.entries(entries).map(([key,{content,...entry}])=>[key,entry])),tracks:th08Catalog,bossCue:{parsed,recipe:th08BossRecipe}};
  }
  const midiBridgeSource=th08Selected?midiBridgeFactorySource():null;
  const midiBridgeBytes=midiBridgeSource?Buffer.from(midiBridgeSource,'utf8'):null;
  const midiVendorPath=resolve(root,'launcher/public/vendor/webaudio-tinysynth.min.js');
  const midiVendorBytes=th08Selected?await readFile(midiVendorPath):null;
  if(th08Selected)Object.assign(guardedSources,{
    th08MusicParser:{path:'tests/adaptation/adapters/titles/th08-music.mjs',file:resolve(root,'tests/adaptation/adapters/titles/th08-music.mjs')},
    th08MusicExtractor:{path:'launcher/scripts/touhou_formats.py',file:resolve(root,'launcher/scripts/touhou_formats.py')},
    th08MusicRoom:{path:'ports/th08/web/cpp/game/MusicRoom.cpp',file:resolve(root,'ports/th08/web/cpp/game/MusicRoom.cpp')},
    th08AudioHost:{path:'ports/th08/web/cpp/sdl/AudioHost.cpp',file:resolve(root,'ports/th08/web/cpp/sdl/AudioHost.cpp')},
    th08MusicControl:{path:'ports/th08/web/cpp/game/MusicControl.cpp',file:resolve(root,'ports/th08/web/cpp/game/MusicControl.cpp')},
    th08MusicCommands:{path:'ports/th08/web/cpp/game/MusicCommands.cpp',file:resolve(root,'ports/th08/web/cpp/game/MusicCommands.cpp')},
    th08TitleFlow:{path:'ports/th08/web/cpp/game/TitleFlow.cpp',file:resolve(root,'ports/th08/web/cpp/game/TitleFlow.cpp')},
    th08GameplayControl:{path:'ports/th08/web/cpp/game/GameplayControl.cpp',file:resolve(root,'ports/th08/web/cpp/game/GameplayControl.cpp')},
    th08Shell:{path:'ports/th08/web/sdl-runtime/shell.mjs',file:resolve(root,'ports/th08/web/sdl-runtime/shell.mjs')},
    launcherMidiBridge:{path:'launcher/app/services/midi-bridge.ts',file:resolve(root,'launcher/app/services/midi-bridge.ts')},
  });
  for(const source of Object.values(guardedSources))source.bytes=await readFile(source.file);
  const maps={th11Tracks,th15Tracks};
  for(const game of games)if(titleTrack[game])
    assert.ok(musicInputs[game].files.some(file=>file.name===titleTrack[game]),`${game}: title music input ${titleTrack[game]} is missing`);
  const sourceSnapshotStart=sourceSnapshot(root),driverBytes=await readFile(new URL(import.meta.url));
  const helperBytes=await readFile(new URL('./music-browser-helpers.mjs',import.meta.url));
  const fixtureBytes=await readFile(new URL('./runtime-fixture-site.mjs',import.meta.url));
  const fixtureServerBytes=await readFile(resolve(root,'tools/browser/fixture-server.mjs'));
  const observationBytes=Buffer.from(`export const readRuntimeObservation = (${readRuntimeObservation.toString()});\n`,'utf8');
  const artifacts={
    harness:{path:'music-browser.mjs',sha256:hash(driverBytes)},
    helper:{path:'music-browser-helpers.mjs',sha256:hash(helperBytes)},
    runtimeFixture:{path:'runtime-fixture-site.mjs',sha256:hash(fixtureBytes)},
    fixtureServer:{path:'fixture-server.mjs',sha256:hash(fixtureServerBytes)},
    nativeObservationReader:{path:'runtime-observation-reader.mjs',sha256:hash(observationBytes)},
    audioProbe:{path:'audio-pcm-probe.js',sha256:hash(Buffer.from(MUSIC_PCM_PROBE_SOURCE,'utf8'))},
  };
  const artifactBytes={harness:driverBytes,helper:helperBytes,runtimeFixture:fixtureBytes,
    fixtureServer:fixtureServerBytes,
    nativeObservationReader:observationBytes,audioProbe:Buffer.from(MUSIC_PCM_PROBE_SOURCE,'utf8')};
  for(const [key,source] of Object.entries(guardedSources)){
    artifacts[key]={path:`source/${source.path.replaceAll('/','__')}`,sha256:hash(source.bytes)};
    artifactBytes[key]=source.bytes;
  }
  if(th08Selected){
    artifacts.midiBridgeFactory={path:'launcher-midi-bridge-factory.js',sha256:hash(midiBridgeBytes)};
    artifacts.tinySynthVendor={path:'webaudio-tinysynth.min.js',sha256:hash(midiVendorBytes)};
    artifactBytes.midiBridgeFactory=midiBridgeBytes;artifactBytes.tinySynthVendor=midiVendorBytes;
  }
  await mkdir(resolve(output,'source'),{recursive:true});
  await Promise.all(Object.entries(artifactBytes).map(([key,bytes])=>writeFile(resolve(output,artifacts[key].path),bytes)));
  Object.assign(report,{commit:execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),
    sourceSnapshotStart,manifest:{path:manifestPath,sha256:hash(await readFile(manifestPath))},
    musicManifest:{path:musicManifestPath,sha256:hash(musicManifestBytes),games:Object.keys(musicInputs)},
    musicInputs,musicMapEvidence,artifacts,
    launcherInputs:launcherUi?{packages:{path:launcherPackagePath,sha256:hash(launcherPackageBytes)},artwork:launcherArtwork,only:launcherOnly}:null,
    selection:{games:selection.games,variant:selection.variant,expectedTargets:selection.expected},
    scope:th08Selected?'Chromium | formal TH08 Runtime configure and launch | DATA-bound Music Room track table, native source and PCM, natural loop, keyboard fade and Reset | OGG title/stage, pause and document-hidden recovery | production Launcher MIDI bridge and TinySynth | corrupt/missing OGG diagnostics and public configure MIDI recovery. Displayed Music Room text, other scene transitions, Launcher preference UI and physical device behavior remain separate.':
      'Chromium | formal Runtime configure and launch | product-catalog OGG mounts | actual Runtime WebAudio output tap | title output scenes. AUDIO-01 full track switching, gameplay scene changes, looping and lifecycle coverage remains itemized separately. TH11 and TH15 expose mapped native track identity; TH08, TH10 and TH20 report decoder and PCM evidence without asserting a track ID. No MIDI or physical listening claim.',
    protocols:{runtime:'eagler-touhou/1',audio:'Parallel AnalyserNode tap through a muted branch; original destination connection remains direct',
      nativeStats:Object.fromEntries(games.map(game=>[game,nativeAudioStatsShape(game)]))},
    targets:[]});
  if(launcherOnly)report.scope='Real Launcher | visible music choices and selected-mode launch consent | persisted preference after reload | actual Runtime mode, decoder and PCM | isolated installed OGG object loss and production MIDI fallback';
  else if(launcherUi)report.scope+=' This invocation also runs the actual Launcher preference and resource-fallback UI journey.';
  await save();
  const verifyArtifacts=async()=>{
    const current={
      harness:await readFile(new URL(import.meta.url)),
      helper:await readFile(new URL('./music-browser-helpers.mjs',import.meta.url)),
      runtimeFixture:await readFile(new URL('./runtime-fixture-site.mjs',import.meta.url)),
      fixtureServer:await readFile(resolve(root,'tools/browser/fixture-server.mjs')),
      nativeObservationReader:Buffer.from(`export const readRuntimeObservation = (${readRuntimeObservation.toString()});\n`,'utf8'),
      audioProbe:Buffer.from(MUSIC_PCM_PROBE_SOURCE,'utf8'),
    };
    if(th08Selected){current.midiBridgeFactory=midiBridgeBytes;current.tinySynthVendor=await readFile(midiVendorPath);}
    for(const [key,source] of Object.entries(guardedSources))current[key]=await readFile(source.file);
    for(const [key,bytes] of Object.entries(current)){
      assert.equal(hash(bytes),artifacts[key].sha256,`${key} changed during music acceptance`);
      const saved=await readFile(resolve(output,artifacts[key].path));
      assert.equal(hash(saved),artifacts[key].sha256,`${key} evidence copy changed during music acceptance`);
    }
  };
  const snapshotMusicFiles=async game=>{
    const files=new Map();
    for(const entry of musicInputs[game].files){
      const bytes=await readFile(entry.path);
      assert.equal(bytes.length,entry.bytes,`${game}/${entry.name}: OGG changed while preparing fixture`);
      assert.equal(hash(bytes),entry.sha256,`${game}/${entry.name}: OGG changed while preparing fixture`);
      files.set(`/music/${game}/${entry.name}`,bytes);
    }
    return files;
  };
  const runLauncherUi=async(record,target,identity,shared)=>{
    const {runLauncherMusicJourney}=await import('./launcher-music-journey.mjs');
    const {launchBrowser}=await import('../browser/launch.mjs');
    const journey=await runLauncherMusicJourney({root,output:resolve(output,'launcher-ui-'+target.game),target,
      identity:identity.runtime,dataIdentity:identity.data,shared,artwork:launcherArtwork,packageMap:launcherPackageMap,
      musicInputs:musicInputs[target.game],pcmProbeSource:MUSIC_PCM_PROBE_SOURCE,readAudioState:readNativeAudioState,
      launchBrowser:({tlsIdentity})=>launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required',
        `--ignore-certificate-errors-spki-list=${tlsIdentity.spkiSha256}`]})});
    record.launcherMusic=journey;
    const passed=journey.status==='observed'&&journey.scenes.length===5&&journey.scenes.every(scene=>scene.passed===true)&&
      journey.resourceFault?.passed===true&&journey.cleanupErrors.length===0;
    const status=passed?'partial':journey.status==='failed'?'failed':'incomplete';
    const launcherEvidence={path:resolve(output,'launcher-ui-'+target.game,'launcher-music-journey.json'),status:journey.status,passed};
    if(launcherOnly){
      record.dataIdentity=identity.data;record.packageIdentity=identity.runtime;
      record.checks=['AUDIO-05','MIDI-01','MIDI-04'].map(id=>({id,status,passed,complete:false,launcherMusic:launcherEvidence,
        scope:'Real Launcher menu preferences, fresh Runtime none/MIDI/OGG decode selection and installed OGG failure fallback'}));
      record.scenario={id:'th08-launcher-music-ui',status:passed?'passed':status,passed};
      record.status=record.scenario.status;record.passed=passed;
    }else{
      for(const check of record.checks.filter(check=>['AUDIO-05','MIDI-01','MIDI-04'].includes(check.id))){
        Object.assign(check,mergeMusicBehaviorResult(check,{passed,status}),{launcherMusic:launcherEvidence});
      }
      record.scenario=mergeMusicBehaviorResult(record.scenario??{id:'th08-normal-audio-midi-scenes',status:record.status,passed:record.passed},
        {passed,status:passed?'passed':status});
      record.scenario.launcherUiPassed=passed;record.status=record.scenario.status;record.passed=record.scenario.passed;
    }
    await save();
  };
  if(process.argv.slice(2).includes('--plan-only')){
    for(const {target,identity} of verifiedPackages){
      const game=target.game;
      const files=await snapshotMusicFiles(game);
      const shared=Object.fromEntries(Object.entries(config.shared??{}).map(([name,path])=>[name,resolve(path)]));
      const fixture=await createRuntimeFixtureSite({repoRoot:root,targets:[target],shared,files,protocol:'https'});
      try{
        const fixtureIdentity=await fixture.verify(target);
        assert.deepEqual(fixtureIdentity,identity,`${game}: preflight Runtime/DATA identity mismatch`);
        const sharedIdentity=await fixture.verifyShared();
        const resources=runtimeMusicResources(game,musicInputs,{base:fixture.base});
        const delivered=[];
        for(const resource of resources){
          const response=await fixture.fetch(resource.url);assert.ok(response.ok,`${game}: OGG route returned HTTP ${response.status}`);
          const bytes=Buffer.from(await response.arrayBuffer());
          const expected=musicInputs[game].files.find(file=>file.name===resource.path.slice(resource.path.lastIndexOf('/')+1));
          assert.ok(expected,`${game}: OGG resource has no manifest identity`);
          assert.equal(bytes.length,resource.bytes,`${game}/${expected.name}: fixture route byte count mismatch`);
          assert.equal(hash(bytes),expected.sha256,`${game}/${expected.name}: fixture route SHA-256 mismatch`);
          delivered.push({name:expected.name,path:resource.path,url:new URL(resource.url).pathname,bytes:bytes.length,sha256:hash(bytes)});
        }
        assert.equal(fixture.httpFailures.length,0,`${game}: local OGG fixture reported an HTTP failure`);
        assert.deepEqual(await fixture.verify(target),fixtureIdentity,`${game}: package or DATA identity changed during preflight`);
        assert.deepEqual(await fixture.verifyShared(),sharedIdentity,`${game}: shared Runtime inputs changed during preflight`);
        report.targets.push({game,variant:'normal',status:'preflight-only',passed:false,
          runtimeIdentity:fixtureIdentity.runtime,dataIdentity:fixtureIdentity.data,
          sharedIdentity,fixture:{protocol:fixture.protocol,tlsIdentity:fixture.tlsIdentity},musicResources:delivered,nativeStats:nativeAudioStatsShape(game),
          trackIdentity:titleTrack[game]?'native-stat-map-available':'not-exposed-by-native-stats'});
      }finally{await fixture.close();}
    }
    const postflight=await verifyMusicInputs(musicManifest,{repoRoot:root,games});
    assert.deepEqual(postflight,musicInputs,'OGG resource identities changed during music preflight');
    assert.equal(hash(await readFile(manifestPath)),hash(manifestBytes),'Runtime input manifest changed during music preflight');
    assert.equal(hash(await readFile(musicManifestPath)),hash(musicManifestBytes),'Music input manifest changed during music preflight');
    await verifyArtifacts();
    Object.assign(report,{musicInputPostflight:postflight,sourceSnapshotPostflight:sourceSnapshot(root),
      preflight:{status:'passed',browserStarted:false,runtimePlayback:'not-run'},
      status:'incomplete',passed:false,incompleteReason:'Plan-only verifies inputs and delivery routes without starting a Runtime'});
    await save();return;
  }
  for(const {target,identity} of verifiedPackages){
      const game=target.game,record={game,variant:'normal',id:target.id,status:'running',passed:false,modes:[],checks:[],errors:[],consoleErrors:[],httpErrors:[]};
      report.targets.push(record);await save();
      const shared={...Object.fromEntries(Object.entries(config.shared??{}).map(([name,path])=>[name,resolve(path)]))};
      if(launcherOnly){await runLauncherUi(record,target,identity,shared);continue;}
      const files=await snapshotMusicFiles(game);
      if(game==='th08')files.set('/launcher/vendor/webaudio-tinysynth.min.js',midiVendorBytes);
      const fixture=await createRuntimeFixtureSite({repoRoot:root,targets:[target],shared,files,protocol:'https'});
      try{
      const fixtureIdentity=await fixture.verify(target);
      assert.deepEqual(fixtureIdentity,identity,`${game}: Runtime or DATA identity changed during preflight`);
      const sharedIdentity=await fixture.verifyShared();
      const {launchBrowser}=await import('../browser/launch.mjs');
      const browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required',
        `--ignore-certificate-errors-spki-list=${fixture.tlsIdentity.spkiSha256}`]});
      record.fixture={base:fixture.base,protocol:fixture.protocol,tlsIdentity:fixture.tlsIdentity};
      try{
        const modes=game==='th08'?['none','ogg','ogg-full','midi','corrupt-ogg','missing-ogg']:['none','ogg'];
        for(const mode of modes){
          const modeRecord={mode,status:'running',runtimeResources:[],title:null,start:null};record.modes.push(modeRecord);
          const context=await browser.newContext({viewport:{width:1100,height:820}});
          await context.addInitScript({content:game==='th08'?'window.__eaglerMusicPcmProbeTop=true;\n'+MUSIC_PCM_PROBE_SOURCE:MUSIC_PCM_PROBE_SOURCE});
          const page=await context.newPage();let frame=null,fixtureWatcher=null,activeStage='host-navigation';
          modeRecord.protocolCapture=[];
          await page.exposeBinding('__captureMusicProtocolFailure',(source,item)=>{
            if(source.page!==page||source.frame!==page.mainFrame()){
              record.errors.push({mode,stage:activeStage,error:'Music protocol capture came from another page or a child frame'});return;
            }
            modeRecord.protocolCapture.push({...item,receivedAt:new Date().toISOString()});
          });
          await page.addInitScript(installMusicProtocolCapture,{game});
          const drainProtocol=()=>page.evaluate(async()=>{
            if(typeof window.__flushMusicProtocolCaptures!=='function')throw Error('Music protocol capture is not installed');
            if(await window.__flushMusicProtocolCaptures()!==0)throw Error('Music protocol captures did not drain');
          });
          const markStage=async stageName=>{
            activeStage=stageName;
            if(page.url()!=='about:blank')await page.evaluate(name=>{window.__eaglerMusicProtocolStage=name;},stageName);
          };
          page.on('pageerror',error=>record.errors.push({mode,stage:activeStage,error:String(error)}));
          page.on('crash',()=>record.errors.push({mode,stage:activeStage,error:'Runtime browser page crashed'}));
          page.on('console',message=>{if(message.type()==='error')record.consoleErrors.push({mode,stage:activeStage,text:message.text(),url:message.location().url});});
          page.on('response',response=>{if(response.status()>=400){record.httpErrors.push({mode,stage:activeStage,url:response.url(),status:response.status()});
            if(mode==='missing-ogg'&&response.url()===modeRecord.missingOgg?.requestUrl){modeRecord.missingOgg.requestStatus=response.status();modeRecord.missingOgg.requestCount++;}}});
          page.on('requestfailed',request=>record.httpErrors.push({mode,stage:activeStage,url:request.url(),failure:request.failure()?.errorText??'request failed'}));
          try{
            fixtureWatcher=await fixture.watchPage(page,{output,label:`${game}-${mode}`,expectedResponse:mode==='missing-ogg'?(url,response)=>{
              if(url.href!==modeRecord.missingOgg?.requestUrl||response.status()!==404)return null;
              const missingBody=Buffer.from('Unknown Runtime fixture resource');
              return{kind:'controlled-missing-music',expected:{status:404,bytes:missingBody.length,sha256:hash(missingBody)},
                validate:({response,bytes})=>{assert.equal(response.status(),404);assert.deepEqual(bytes,missingBody);return{kind:'controlled-missing-music',status:404};}};
            }:undefined});
            await page.goto(fixture.hostUrl(target,mode==='none'?1:2));
            await markStage('host-ready');
            await page.waitForFunction(()=>events.some(event=>event.event==='ready'),null,{timeout:120000});
            await page.evaluate(observeMusicProtocolHistory);
            await markStage('runtime-frame-binding');
            frame=page.frames().find(candidate=>candidate.url().includes(`/runtime/${target.id}/`));assert.ok(frame,`${game}: Runtime frame missing`);
            if(game==='th08')modeRecord.runtimeFrameIdentity=await bindMusicRuntimeFrame(page,frame);
            const sharedResources=Object.keys(config.shared??{}).map(name=>({path:`/${name}`,url:`${fixture.base}/shared/${name}`}));
            const resources=musicInputs[game].files;
            const musicMode=mode==='none'?'none':mode==='midi'?'midi':'ogg';
            const runtimeResources=['ogg','ogg-full','corrupt-ogg','missing-ogg'].includes(mode)?runtimeMusicResources(game,musicInputs,{base:fixture.base}):[];
            modeRecord.runtimeResources=runtimeResources.map(resource=>({path:resource.path,bytes:resource.bytes}));
            const configure=({sharedResources,runtimeResources,musicMode,options})=>rpc('configure',{language:'ja',music:musicMode,
              resources:[],runtimeResources,sharedResources,thpracCatalog:[],options:{alwaysHitbox:false,touchEnabled:false,
                thpracEnabled:false,limitPresentationTo60:false,...options}});
            let corruptOriginal=null,corruptInjected=null,missingOriginal=null;
            if(mode==='corrupt-ogg'){
              const faultPath='/music/th08/th08_01.ogg';corruptOriginal=files.get(faultPath);
              assert.ok(corruptOriginal instanceof Uint8Array,'TH08 title OGG fixture bytes are unavailable for fault injection');
              corruptInjected=Buffer.from(corruptOriginal);corruptInjected.fill(0);
              if(corruptInjected.equals(corruptOriginal))corruptInjected[0]^=0xff;
              assert.equal(corruptInjected.length,corruptOriginal.length,'Corrupt OGG fixture must retain the verified resource length');
              modeRecord.corruptOgg={path:'/bgm-ogg/th08_01.ogg',originalBytes:corruptOriginal.length,
                originalSha256:hash(corruptOriginal),injectedBytes:corruptInjected.length,injectedSha256:hash(corruptInjected),
                injection:'zero-fill; flip byte 0 only if bytes equal the source'};
              files.set(faultPath,corruptInjected);
            }
            if(mode==='missing-ogg'){
              const faultPath='/music/th08/th08_01.ogg';missingOriginal=files.get(faultPath);assert.ok(missingOriginal instanceof Uint8Array);
              modeRecord.missingOgg={path:'/bgm-ogg/th08_01.ogg',requestUrl:`${fixture.base}${faultPath}`,requestStatus:null,requestCount:0,
                originalBytes:missingOriginal.length,originalSha256:hash(missingOriginal),injection:'remove exactly the title OGG fixture route'};
              files.delete(faultPath);
            }
            await markStage('configure');
            let configureError=null;
            const configuredOptions={...target.options,...(game==='th08'&&['ogg','ogg-full'].includes(mode)?{touchEnabled:true,oggDecodeMode:mode==='ogg-full'?'full':'stream'}:{})};
            modeRecord.configuredOptions=configuredOptions;
            try{await page.evaluate(configure,{sharedResources,runtimeResources,musicMode,options:configuredOptions});}
            catch(error){if(mode!=='missing-ogg')throw error;configureError=String(error.message??error);}
            finally{if(corruptOriginal)files.set('/music/th08/th08_01.ogg',corruptOriginal);if(missingOriginal)files.set('/music/th08/th08_01.ogg',missingOriginal);}
            if(mode==='missing-ogg'){
              assert.ok(configureError,'Missing OGG configure unexpectedly succeeded');
              const rejected=await page.evaluate(()=>events.filter(event=>event.ok===false));assert.equal(rejected.length,1);
              const startup=await frame.evaluate(()=>({app:window.__th08Runtime.app}));
              Object.assign(startup,await page.evaluate(()=>({ready:events.find(event=>event.event==='ready'),firstFrameCount:events.filter(event=>event.event==='first-frame').length})));
              Object.assign(modeRecord.missingOgg,{configureError,configureResponse:rejected[0],startup});
              await fixtureWatcher.flush();assert.equal(modeRecord.missingOgg.requestCount,1);
              frame=await recoverTh08MusicFault({page,frame,modeRecord,target,fixture,sharedResources,configure,midiBridgeSource,markStage,drainProtocol});
              continue;
            }
            if(mode==='midi'){await markStage('midi-bridge-setup');modeRecord.midiBridge=await installTh08MidiBridge(page,fixture,midiBridgeSource);}
            await markStage('runtime-launch');
            await page.evaluate(()=>rpc('launch'));
            await markStage('runtime-first-frame');
            await page.waitForFunction(()=>events.some(event=>event.event==='first-frame'),null,{timeout:120000});
            if(mode==='midi')await page.evaluate(()=>{window.__eaglerMidiRuntimeLaunched=true;document.querySelector('iframe')?.focus();});
            await page.waitForTimeout(1800);
            await markStage('title-observation');
            const collectScene=async({stage=false}={})=>{
              if(game==='th08'&&musicMode==='midi')return collectTh08MidiScene(frame,page,stage?null:th08TitleMidi);
              if(game==='th08'&&mode==='corrupt-ogg')return collectTh08OggScene(frame,page,resources,{rejectedSource:true});
              if(game==='th08'&&musicMode==='ogg')return collectTh08OggScene(frame,page,resources);
              const captures=[];let raw,native;
              for(let sample=0;sample<(mode==='none'?6:20);sample++){
                raw=await frame.evaluate(readNativeAudioState,game);native=describeNativeMusic(game,raw,maps);
                captures.push(...await frame.evaluate(()=>window.__eaglerMusicPcmProbe?.capture()??[]));
                if(mode==='ogg'&&native.trackFile&&summarizeMusicPcm(captures).outputObserved)break;
                await page.waitForTimeout(mode==='none'?100:250);
              }
              const pcm=summarizeMusicPcm(captures);
              raw=await frame.evaluate(readNativeAudioState,game);native=describeNativeMusic(game,raw,maps);
              const input=mapTrackToInput(native.trackFile,resources);
              return {native,decoder:raw.decoder,trackInput:input,pcm};
            };
            modeRecord.title=await collectScene();
            await markStage('title-observation');
            modeRecord.title.observation=await frame.evaluate(readRuntimeObservation,runtimeObservationRequest({game,variant:'normal'}));
            modeRecord.title.audioWindow={settleMs:1800,captureWindowMs:mode==='none'?600:null};
            await markStage('title-response-audit');
            await fixtureWatcher.flush();
            const oggResponses=fixtureWatcher.responses.filter(response=>new URL(response.url).pathname.startsWith(`/music/${game}/`));
            if(['ogg','ogg-full','corrupt-ogg'].includes(mode)){
              assert.ok(oggResponses.length,`${game}: Runtime did not request any verified OGG resource`);
              if(titleTrack[game])assert.ok(oggResponses.some(response=>new URL(response.url).pathname===`/music/${game}/${titleTrack[game]}`),
                `${game}: Runtime did not request the native title track OGG`);
            }
            modeRecord.fixtureResponses={capture:fixtureWatcher.capture,count:fixtureWatcher.responses.length,responses:fixtureWatcher.responses};
            modeRecord.requestedOggResources=oggResponses.map(response=>({url:response.url,expected:response.expected,actual:response.actual}));
            await markStage('title-audit');
            await page.screenshot({path:resolve(output,`${game}-${mode}-title.png`)});
            if(mode==='ogg'&&titleTrack[game]){
              assert.equal(modeRecord.title.native.trackFile,titleTrack[game],`${game}: native title track does not map to the declared title OGG`);
              assert.ok(modeRecord.title.trackInput,`${game}: native title track does not bind to a verified OGG input`);
              assert.ok(modeRecord.title.pcm.outputObserved,`${game}: native title track has no observed Runtime PCM output`);
            }
            if(mode==='ogg'&&!titleTrack[game]&&game!=='th08'){
              assert.ok(modeRecord.title.pcm.outputObserved,`${game}: OGG mode produced no observed Runtime PCM output`);
            }
            if(mode==='corrupt-ogg'){
              frame=await recoverTh08MusicFault({page,frame,modeRecord,target,fixture,sharedResources,configure,midiBridgeSource,markStage,drainProtocol});
              continue;
            }
            if(game==='th08'&&mode==='ogg'){
              await markStage('music-room');
              modeRecord.musicRoom=await collectTh08MusicRoom(page,frame,resources,th08Catalog);
            }
            const canObserveStart=['th08','th10','th11','th15'].includes(game);
            if(canObserveStart){
              await markStage('gameplay-navigation');
              let observation=modeRecord.title.observation,inputsSent=0;
              for(let press=0;press<28&&!gameplayReached(game,observation);press++){
                await pressStart(page);inputsSent++;observation=await frame.evaluate(readRuntimeObservation,runtimeObservationRequest({game,variant:'normal'}));
              }
              modeRecord.start={status:gameplayReached(game,observation)?'gameplay-observed':'start-input-sent',
                observation,inputsSent};
              if(gameplayReached(game,observation)){
                await markStage('gameplay-observation');
                modeRecord.start.audio=await collectScene({stage:true});
                modeRecord.start.audio.observation=observation;
                await page.screenshot({path:resolve(output,`${game}-${mode}-gameplay.png`)});
                if(game==='th08'&&['ogg','ogg-full'].includes(mode)){
                  await markStage('pause');
                  modeRecord.start.oggPause=await pauseTh08Gameplay(page,frame,'ogg');
                  await markStage('background');
                  modeRecord.start.background=await backgroundTh08Gameplay(page,frame,'ogg');
                  await markStage('restart');
                  modeRecord.start.restart=await restartTh08Runtime(page,frame,'ogg',resources);
                  await markStage('boss-cue');
                  modeRecord.start.bossCue=await collectTh08BossMusicCue(page,frame,resources,th08BossRecipe);
                }else if(game==='th08'&&mode==='midi'){
                  await markStage('pause');
                  modeRecord.start.midiPause=await pauseTh08Gameplay(page,frame,'midi');
                  await markStage('background');
                  modeRecord.start.background=await backgroundTh08Gameplay(page,frame,'midi');
                  await markStage('restart');
                  modeRecord.start.restart=await restartTh08Runtime(page,frame,'midi',resources);
                }
              }
            }else modeRecord.start={status:'not-run',reason:'TH20 native phase and UI observations are recorded; this driver does not yet implement title-specific navigation'};
            modeRecord.identityCoverage=game==='th08'||titleTrack[game]?'native-track-id-and-ogg-input':'decoder-and-pcm-only';
            modeRecord.status=['ogg','ogg-full','midi'].includes(mode)&&modeRecord.title.pcm?.outputObserved?'observed':'recorded';
            if(mode==='none'){
              modeRecord.control=describeNoneMusicControl(modeRecord.title.pcm);
              modeRecord.passed=modeRecord.control.status==='silent-after-settling';
            }else modeRecord.passed=modeRecord.title.pcm?.outputObserved===true;
          }catch(error){modeRecord.status='failed';modeRecord.passed=false;modeRecord.failedStage=activeStage;
            if(error.evidence)modeRecord.failureEvidence=error.evidence;
            modeRecord.error=String(error.stack||error);record.errors.push({mode,stage:activeStage,error:modeRecord.error});}
          finally{
            if(fixtureWatcher){
              activeStage='response-audit';
              await markStage('response-audit').catch(()=>{});
              try{await fixtureWatcher.flush();}catch(error){modeRecord.responseAuditError=String(error.stack||error);}
              await fixtureWatcher.stop();
              modeRecord.fixtureResponses??={capture:fixtureWatcher.capture,count:fixtureWatcher.responses.length,responses:fixtureWatcher.responses,errors:fixtureWatcher.errors};
            }
            await markStage('protocol-tail').catch(()=>{});
            const tailEvents=await page.evaluate(()=>{
              if(!Array.isArray(window.events))throw Error('Runtime protocol event history is unavailable');
              return window.events;
            }).catch(error=>{
              modeRecord.protocolTailError=String(error.stack||error);
              record.errors.push({mode,stage:'protocol-tail',error:modeRecord.protocolTailError});return[];
            });
            const tailStages=await page.evaluate(()=>window.__eaglerMusicProtocolStages??[]).catch(error=>{
              record.errors.push({mode,stage:'protocol-tail',error:String(error.stack||error)});return[];
            });
            modeRecord.events=[...(modeRecord.beforeRecoveryEvents??[]),...tailEvents];
            modeRecord.eventStages=[...(modeRecord.beforeRecoveryEventStages??[]),...tailStages.map(item=>({...item,index:item.index+(modeRecord.beforeRecoveryEvents?.length??0)}))];
            try{await drainProtocol();}catch(error){record.errors.push({mode,stage:'protocol-tail',error:String(error.stack||error)});}
            await context.close();await save();
          }
        }
        const ogg=record.modes.find(mode=>mode.mode==='ogg');
        const pcmOutput=Boolean(ogg?.title?.pcm?.outputObserved);
        const noneControl=record.modes.find(mode=>mode.mode==='none')?.control;
        record.dataIdentity=fixtureIdentity.data;record.packageIdentity=fixtureIdentity.runtime;
        record.scenarioErrors=musicScenarioErrors(record,{fixtureFailures:fixture.httpFailures});
        if(game==='th08'){
          const titleStages=['host-navigation','host-ready','runtime-frame-binding','configure','midi-bridge-setup',
            'runtime-launch','runtime-first-frame','title-observation','title-response-audit','title-audit'];
          const gameplayStages=[...titleStages,'gameplay-navigation','gameplay-observation'];
          const errorsFor=(mode,stages)=>musicScenarioErrors(record,{mode,stages});
          const attachStageErrors=attachMusicStageErrors;
          const midi=record.modes.find(mode=>mode.mode==='midi'),corrupt=record.modes.find(mode=>mode.mode==='corrupt-ogg'),missing=record.modes.find(mode=>mode.mode==='missing-ogg');
          const oggTitle=th08OggTitleOutcome({...ogg?.title,
            errors:errorsFor('ogg',titleStages)});
          const oggAcceptance=th08OggAcceptanceOutcome({title:oggTitle,expectedStageSong:th08FirstStageSong,
            stage:ogg?.start?.audio,errors:errorsFor('ogg',gameplayStages)});
          const musicRoom=th08MusicRoomOutcome({catalog:th08Catalog,journey:ogg?.musicRoom,errors:errorsFor('ogg',['music-room'])});
          oggAcceptance.musicRoom=musicRoom;oggAcceptance.passed&&=musicRoom.passed;
          if(!oggAcceptance.passed)oggAcceptance.status='failed';
          const oggPause=attachStageErrors(ogg?.start?.oggPause??{passed:false,status:'incomplete',complete:false,id:'AUDIO-03'},
            errorsFor('ogg',['pause']));
          const oggRestart=attachStageErrors(ogg?.start?.restart??{passed:false,status:'incomplete',complete:false,id:'AUDIO-03-restart'},
            errorsFor('ogg',['restart']));
          const audioLifecyclePassed=oggPause.passed===true&&oggRestart.passed===true&&musicRoom.passed===true;
          const audioLifecycleStatus=audioLifecyclePassed?'partial':[oggPause,oggRestart,musicRoom].some(check=>check.status==='failed')?'failed':'incomplete';
          const audioLifecycle={...oggPause,id:'AUDIO-03',passed:audioLifecyclePassed,
            status:audioLifecycleStatus,complete:false,restart:oggRestart,musicRoom};
          const oggBackground=attachStageErrors(ogg?.start?.background??{passed:false,status:'incomplete',complete:false,id:'AUDIO-04'},
            errorsFor('ogg',['background']));
          const midiTitle=th08MidiTitleOutcome({native:midi?.title?.native,events:midi?.title?.events??[],
            sendCorrelation:midi?.title?.sendCorrelation,pcm:midi?.title?.pcm,contextState:midi?.title?.contextState,
            errors:errorsFor('midi',titleStages)});
          const midiCapability=th08MidiCapabilityOutcome({capability:PRODUCT_GAMES.th08.musicCapabilities,
            consumerInstalled:Boolean(midi?.midiBridge),notesSent:midiTitle.observedSynthNoteSends>0,
            pcmObserved:midiTitle.pcmObserved});
          const midiPause=attachStageErrors(midi?.start?.midiPause??{passed:false,status:'incomplete',complete:false,id:'MIDI-03'},
            errorsFor('midi',['pause']));
          const midiBackground=attachStageErrors(midi?.start?.background??{passed:false,status:'incomplete',complete:false,id:'MIDI-03-background'},
            errorsFor('midi',['background']));
          const midiRestart=attachStageErrors(midi?.start?.restart??{passed:false,status:'incomplete',complete:false,id:'MIDI-03-restart'},
            errorsFor('midi',['restart']));
          const midiLifecyclePassed=midiPause.passed===true&&midiBackground.passed===true&&midiRestart.passed===true;
          const midiLifecycleStatus=midiLifecyclePassed?'partial':[midiPause,midiBackground,midiRestart].some(check=>check.status==='failed')?'failed':'incomplete';
          const midiLifecycle={...midiPause,id:'MIDI-03',passed:midiLifecyclePassed,
            status:midiLifecycleStatus,complete:false,
            background:midiBackground,restart:midiRestart};
          const midiSeparation=th08MusicModeSeparationOutcome({ogg:{passed:oggTitle.passed},midi:{passed:midiTitle.passed},
            midiWithoutOgg:{passed:midiTitle.passed,oggResources:midi?.runtimeResources?.length??null},
            restoredOgg:{passed:oggTitle.passed,oggResources:ogg?.runtimeResources?.length??0}});
          const corruptDiagnostics=(corrupt?.events??[]).filter(event=>event?.event==='notice').map(event=>event.message).filter(Boolean);
          const oggFault=th08OggFaultOutcome({fault:'corrupt',native:corrupt?.title?.native,pcm:corrupt?.title?.pcm,
            diagnostics:corruptDiagnostics,expectedFile:th08TitleOgg,
            injectedBytesSha256:corrupt?.corruptOgg?.injectedSha256,originalBytesSha256:corrupt?.corruptOgg?.originalSha256,
            errors:errorsFor('corrupt-ogg',titleStages)});
          if(corrupt){
            Object.assign(corrupt,attachTh08OggFaultModeAssessment(corrupt,oggFault));
          }
          const missingFault=th08OggFaultOutcome({fault:'missing',expectedFile:th08TitleOgg,...missing?.missingOgg,
            diagnostics:[missing?.missingOgg?.configureResponse?.error].filter(Boolean),errors:errorsFor('missing-ogg',titleStages)});
          const recoveryStages=['fault-recovery-retire','fault-recovery-navigation','fault-recovery-configure','fault-recovery-midi-bridge','fault-recovery-launch','fault-recovery-output'];
          const corruptRecovery=th08MusicFaultRecoveryOutcome({fault:oggFault,recovery:corrupt?.recovery,errors:errorsFor('corrupt-ogg',recoveryStages)});
          const missingRecovery=th08MusicFaultRecoveryOutcome({fault:missingFault,recovery:missing?.recovery,errors:errorsFor('missing-ogg',recoveryStages)});
          if(corrupt){corrupt.recoveryAssessment=corruptRecovery;corrupt.passed=corrupt.passed&&corruptRecovery.passed;corrupt.status=corrupt.passed?'passed':corruptRecovery.status;}
          if(missing){Object.assign(missing,attachTh08OggFaultModeAssessment(missing,missingFault));missing.recoveryAssessment=missingRecovery;
            missing.passed=missing.passed&&missingRecovery.passed;missing.status=missing.passed?'passed':missingRecovery.status;}
          const audioFault=th08MusicFaultJourneyOutcome({corrupt:oggFault,missing:missingFault,corruptRecovery,missingRecovery});
          const midiOutput={...midiTitle,id:'MIDI-02'};
          const audioBackground={...oggBackground,id:'AUDIO-04'};
          const bossCues=['ogg','ogg-full'].map(mode=>{
            const scene=record.modes.find(row=>row.mode===mode)?.start?.bossCue;
            const transition=assessMusicCueTransition(scene?.transition,th08BossRecipe);
            const audio=th08OggTitleOutcome({...scene?.audio,expectedFile:th08BossRecipe.afterFile,errors:errorsFor(mode,['boss-cue'])});
            const decoder=scene?.audio?.decoder,decoderMatched=decoder?.enabled===true&&decoder.decodeMode===(mode==='ogg'?0:1);
            return{mode,transition,audio,decoderMatched,passed:transition.passed&&audio.passed&&decoderMatched};
          });
          audioLifecycle.bossCues=bossCues;audioLifecycle.passed&&=bossCues.every(cue=>cue.passed);
          if(!audioLifecycle.passed)audioLifecycle.status='failed';
          const decoding=th08OggDecodeVariantsOutcome({stream:ogg,full:record.modes.find(mode=>mode.mode==='ogg-full'),
            errors:record.scenarioErrors});
          for(const variant of decoding.variants){
            const mode=record.modes.find(mode=>mode.mode===variant.mode);
            if(mode){mode.decodeLifecycle=variant;mode.passed&&=variant.passed;if(!mode.passed)mode.status=variant.status;}
          }
          for(const check of [oggAcceptance,audioLifecycle,audioBackground]){
            check.decoding=decoding;check.passed&&=decoding.passed;if(!check.passed)check.status='failed';
          }
          record.checks=[oggAcceptance,audioLifecycle,audioBackground,audioFault,midiCapability,midiOutput,midiLifecycle,midiSeparation];
          record.audio01=oggAcceptance;record.audio03=audioLifecycle;record.audio04=audioBackground;record.audio05=audioFault;
          record.midi01=midiCapability;record.midi02=midiOutput;record.midi03=midiLifecycle;record.midi04=midiSeparation;
          const allPassed=musicScenarioPassed(record.checks,record.scenarioErrors);
          record.scenario={id:'th08-normal-audio-midi-scenes',status:allPassed?'passed':'failed',passed:allPassed,
            scope:'DATA-bound Music Room sources and PCM, natural loop, keyboard fade and Reset; stream/full OGG title/stage, pause, background and reopen; missing/corrupt resource diagnostics and MIDI recovery; production Launcher MIDI output, pause and voice-reset/reopen; separate OGG/MIDI configurations'};
          record.status=record.scenario.status;record.passed=record.scenario.passed;
        }else{
        const sceneOutcome=musicTitleSceneOutcome({oggPcmObserved:pcmOutput,
          expectedTrack:titleTrack[game]??null,observedTrack:ogg?.title?.native?.trackFile??null,
          trackInput:ogg?.title?.trackInput??null,
          errors:record.scenarioErrors});
        record.scenario=sceneOutcome.scenario;record.checks=[sceneOutcome.coverage];
        record.audio01={...sceneOutcome.coverage,trackIdentity:sceneOutcome.trackIdentity,runtimePcm:pcmOutput,
          noneControl,titleTrack:ogg?.title?.native?.trackFile??null,titleInput:ogg?.title?.trackInput??null};
        record.status=sceneOutcome.scenario.status;record.passed=sceneOutcome.scenario.passed;
        }
        const post=await fixture.verify(target);
        assert.deepEqual(post,fixtureIdentity,`${game}: Runtime or DATA identity changed during music acceptance`);
        assert.deepEqual(await fixture.verifyShared(),sharedIdentity,`${game}: shared Runtime inputs changed during music acceptance`);
        record.identityPostflight=post;
      }catch(error){record.status='failed';record.passed=false;record.failure=String(error.stack||error);}
      finally{await browser.close();}
      }catch(error){record.status='failed';record.passed=false;record.failure=String(error.stack||error);}
      finally{record.httpFailures=fixture.httpFailures;await fixture.close();await save();}
      if(launcherUi)await runLauncherUi(record,target,identity,shared);
      const failedChecks=(record.checks??[]).filter(check=>check.passed===false).map(check=>check.id);
      const room=record.audio01?.musicRoom;
      console.log([`${game}: ${record.status.toUpperCase()}`,record.failure?.split('\n')[0],
        failedChecks.length?`checks: ${failedChecks.join(', ')}`:record.audio01?.trackIdentity,
        room?.passed===false?`Music Room: ${['table','loop','fade','drained'].filter(name=>room[name]===false).join(', ')}`:null]
        .filter(Boolean).join(' | '));
    }
  const musicInputsPostflight=await verifyMusicInputs(musicManifest,{repoRoot:root,games});
  assert.deepEqual(musicInputsPostflight,musicInputs,'OGG resource identities changed during acceptance');
  assert.equal(hash(await readFile(manifestPath)),hash(manifestBytes),'Runtime input manifest changed during music acceptance');
  assert.equal(hash(await readFile(musicManifestPath)),hash(musicManifestBytes),'Music input manifest changed during music acceptance');
  if(launcherUi)assert.equal(hash(await readFile(launcherPackagePath)),hash(launcherPackageBytes),'Launcher package map changed during music acceptance');
  await verifyArtifacts();
  report.musicInputPostflight=musicInputsPostflight;report.sourceSnapshotPostflight=sourceSnapshot(root);
  const failed=report.targets.some(target=>target.scenario?.passed!==true||target.status==='failed');
  report.scenario={id:launcherOnly?'th08-launcher-music-ui':games.includes('th08')?'th08-audio-midi-scenes':'music-title-ogg-scenes',status:failed?'failed':'passed',passed:!failed,
    targetCount:report.targets.length};
  report.coverage={status:'partial',complete:false,
    checks:report.targets.flatMap(target=>target.checks?.length?target.checks.map(check=>({game:target.game,...check})):[{game:target.game,...target.audio01}])};
  report.status=report.scenario.status;report.passed=report.scenario.passed;
  report.result=failed?'At least one selected music scene or controlled resource-diagnostic assertion failed.':'Selected music scenes and controlled resource-fault recovery passed; complete AUDIO/MIDI lifecycle coverage remains partial.';
  await save();
});

if(!result.passed){process.exitCode=1;if(result.failure)console.error(result.failure);}
