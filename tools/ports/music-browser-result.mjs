import {TH08_MIDI_PCM_ROLE,TH08_MIDI_VOICE_PCM_ROLE,TH08_OGG_PCM_ROLE,runtimePcmTargetIdentity,runtimePcmSourceHorizonFrames} from './music-browser-helpers.mjs';
import assert from 'node:assert/strict';

export function selectMusicChecks(game,value){
  const allowed=musicAdaptationIds(game);if(value===undefined)return allowed;
  const selected=value.split(',');assert.ok(selected.length&&selected.every(id=>allowed.includes(id)),`Unsupported music checks for ${game}`);
  assert.equal(new Set(selected).size,selected.length,'Duplicate music checks');return selected;
}

export function musicModesForChecks(game,checks,{baseline=false}={}){
  if(game!=='th08')return['none','ogg'];
  const modes=new Set(baseline?['none']:[]);
  for(const id of checks){
    for(const mode of id==='AUDIO-05'?['corrupt-ogg','missing-ogg']:
      id==='MIDI-04'?['ogg','ogg-full','midi']:id.startsWith('MIDI-')?['midi']:['ogg','ogg-full'])modes.add(mode);
  }
  return['none','ogg','ogg-full','midi','corrupt-ogg','missing-ogg'].filter(mode=>modes.has(mode));
}

/** IDs whose operations and observations are implemented by this music driver. */
export function musicAdaptationIds(game){
  return game==='th08'?['AUDIO-01','AUDIO-03','AUDIO-04','AUDIO-05','MIDI-01','MIDI-02','MIDI-03','MIDI-04']:['AUDIO-01'];
}

/** Additional scenes retain already observed failures and missing evidence. */
export function mergeMusicBehaviorResult(previous,next){
  const passed=previous?.passed===true&&next?.passed===true;
  const statuses=[previous?.status,next?.status];
  const status=statuses.includes('failed')?'failed':!passed?'incomplete':statuses.includes('partial')?'partial':'passed';
  return{...previous,passed,status};
}

/** Bind one Runtime MIDI event to its actual TinySynth send by source order and exact bytes. */
export function correlateTh08MidiSends(events=[],sends=[]){
  const validBytes=value=>Array.isArray(value)&&value.every(byte=>Number.isInteger(byte)&&byte>=0&&byte<=255);
  const native=Array.isArray(events)?events:[],consumer=Array.isArray(sends)?sends:[];
  const comparisons=[];let matchedCount=0;
  for(let index=0;index<Math.min(native.length,consumer.length);index++){
    const expected=native[index]?.bytes,actual=consumer[index];
    const matched=validBytes(expected)&&validBytes(actual)&&expected.length===actual.length&&
      expected.every((byte,byteIndex)=>byte===actual[byteIndex]);
    if(matched)matchedCount++;
    else if(comparisons.length<8)comparisons.push({index,expected:validBytes(expected)?expected:null,actual:validBytes(actual)?actual:null});
  }
  const complete=native.length===consumer.length&&matchedCount===native.length;
  const correlated=native.map((event,index)=>({...event,sentBytes:complete?consumer[index].slice():null}));
  return{status:complete?'matched':'mismatch',eventCount:native.length,sendCount:consumer.length,matchedCount,
    mismatches:comparisons,events:correlated};
}

function midiCorrelationSummary(value){
  if(!value)return null;
  return{status:value.status,eventCount:value.eventCount,sendCount:value.sendCount,matchedCount:value.matchedCount,
    mismatches:value.mismatches??[]};
}

/** GameAudioManager counts short_message and long_message independently. */
export function th08MidiTransportCount(native){
  return Number.isSafeInteger(native?.messageCount)&&native.messageCount>=0&&
    Number.isSafeInteger(native?.sysexCount)&&native.sysexCount>=0?native.messageCount+native.sysexCount:null;
}

function midiCountsMatch(native,events){
  const sysex=events.filter(event=>[0xf0,0xf7].includes(event?.bytes?.[0])).length;
  return th08MidiTransportCount(native)===events.length&&native.sysexCount===sysex&&native.messageCount===events.length-sysex;
}

/** Preserve every error channel; optional mode/stage filters bind errors to one checked scene. */
export function musicScenarioErrors(record,{fixtureFailures=[],mode:selectedMode=null,stages=null}={}){
  const errors=[],expectedCounts=new Map();
  const append=(channel,values,{mode=null,stage=null}={})=>{
    for(const value of values??[]){
      const itemMode=value&&typeof value==='object'&&value.mode!==undefined?value.mode:mode;
      const itemStage=value&&typeof value==='object'&&value.stage!==undefined?value.stage:stage;
      if(selectedMode&&itemMode!==selectedMode)continue;
      if(stages&&(!itemStage||!stages.includes(itemStage)))continue;
      const fault=record.modes?.find(item=>item.mode===itemMode)?.missingOgg;
      if(isExpectedMissingMusicError({channel,value,stage:itemStage,fault})){
        const key=`${itemMode}:${channel}`,seen=expectedCounts.get(key)??0;expectedCounts.set(key,seen+1);
        if(seen===0)continue;
      }
      errors.push({channel,...(itemMode?{mode:itemMode}:{}),...(itemStage?{stage:itemStage}:{}),value});
    }
  };
  append('page-or-scene',record.errors);append('console',record.consoleErrors);
  append('http-or-request',record.httpErrors);append('fixture-server',fixtureFailures);
  for(const mode of record.modes??[]){
    if(mode.responseAuditError)append('response-audit',[mode.responseAuditError],{mode:mode.mode,stage:'response-audit'});
    append('response-identity',mode.fixtureResponses?.errors,{mode:mode.mode,stage:'response-audit'});
    if(Array.isArray(mode.protocolCapture)){
      for(const item of mode.protocolCapture)append('runtime-protocol',[item.event],{mode:mode.mode,stage:item.stage});
      continue;
    }
    const eventStages=new Map((mode.eventStages??[]).map(item=>[item.index,item.stage]));
    for(let index=0;index<(mode.events??[]).length;index++){
      const event=mode.events[index];
      if(event&&(event.event==='error'||event.event==='abort'||event.ok===false))
        append('runtime-protocol',[event],{mode:mode.mode,stage:eventStages.get(index)});
    }
  }
  return errors;
}

/** Only the exact request-bound missing-resource negative is an expected failure. */
export function isExpectedMissingMusicError({channel,value,stage,fault}={}){
  if(stage!=='configure'||fault?.requestStatus!==404||fault.requestCount!==1||typeof fault.requestUrl!=='string'||
    fault.configureResponse?.ok!==false||typeof fault.configureResponse.request!=='string'||
    !fault.configureResponse.error?.includes(fault.path))return false;
  if(channel==='http-or-request')return value?.url===fault.requestUrl&&value.status===404&&!value.failure;
  if(channel==='console')return value?.url===fault.requestUrl&&/404/.test(value.text??'');
  if(channel==='runtime-protocol')return value?.request===fault.configureResponse.request&&
    value.epoch===fault.configureResponse.epoch&&value.game==='th08'&&value.protocol==='eagler-touhou/1'&&
    value.ok===false&&value.error===fault.configureResponse.error;
  return false;
}

/** A mode-scoped check can pass while any captured error still fails the overall journey. */
export function musicScenarioPassed(checks,allErrors){
  return Array.isArray(checks)&&checks.length>0&&checks.every(check=>check?.passed===true)&&
    Array.isArray(allErrors)&&allErrors.length===0;
}

/** Keep missing observations incomplete; captured stage errors still fail the scene. */
export function attachMusicStageErrors(outcome,errors=[]){
  const stageErrors=Array.isArray(errors)?errors:[];
  const passed=outcome?.passed===true&&stageErrors.length===0;
  const status=stageErrors.length>0?'failed':outcome?.status==='incomplete'?'incomplete':passed?(outcome.status??'partial'):'failed';
  return {...outcome,passed,status,stageErrors};
}

/** Report the executed title scene separately from AUDIO-01 full-scope coverage. */
export function musicTitleSceneOutcome({oggPcmObserved,expectedTrack=null,observedTrack=null,trackInput=null,errors=[]}={}) {
  const trackIdentityPassed=expectedTrack==null||
    (observedTrack===expectedTrack&&trackInput?.name===expectedTrack&&typeof trackInput.sha256==='string');
  const passed=oggPcmObserved===true&&trackIdentityPassed&&errors.length===0;
  return {
    scenario:{id:'title-ogg-output',status:passed?'passed':'failed',passed,
      scope:'Runtime title output after OGG configure and launch'},
    coverage:{id:'AUDIO-01',status:passed?'partial':'failed',complete:false,
      scope:'Full track switching, gameplay scene changes, looping and lifecycle coverage remain unrun'},
    trackIdentity:expectedTrack==null?'not-exposed-by-native-stats':trackIdentityPassed?'verified':'failed',
  };
}

/** TH08's source names the title SMF; the Runtime OGG backend uses its WAV-shaped basename. */
export function th08OggTrackFile(nativeName){
  if(typeof nativeName!=='string'||!nativeName)return null;
  const basename=nativeName.replaceAll('\\','/').split('/').at(-1);
  return basename?.replace(/\.wav$/i,'.ogg')??null;
}

/** A real OGG title scene must bind the native source name, verified input and output PCM. */
export function th08OggTitleOutcome({native,expectedFile='th08_01.ogg',input=null,pcm,pcmCollection,errors=[]}={}){
  const observedFile=native?.name?.replace(/\.wav$/i,'.ogg')??null;
  const identity=observedFile===expectedFile&&input?.name===expectedFile&&typeof input.sha256==='string';
  const ready=native?.exists===true&&native.soundReady===true&&native.pending===false&&native.playing===true;
  const pcmPassed=isVerifiedRuntimePcmWindow(pcm,TH08_OGG_PCM_ROLE)&&pcm.outputObserved===true&&
    observedTrackPcmCollection(pcmCollection,pcm,expectedFile);
  const passed=identity&&ready&&pcmPassed&&errors.length===0;
  return {id:'th08-ogg-title-source-pcm',status:passed?'passed':'failed',passed,complete:false,expectedFile,
    observedName:native?.name??null,observedFile,input:input?{name:input.name,sha256:input.sha256}:null,
    decoderReady:ready,pcmObserved:pcm?.outputObserved===true,pcmTargetVerified:pcmPassed,
    pcmScope:'Runtime output PCM from the identified Module.SDL3.audioContext mixer context',
    scope:'Title OGG source identity and Runtime output PCM; full track/lifecycle coverage is separate'};
}

export function th08OggAcceptanceOutcome({title,stage,expectedStageSong=1,errors=[]}={}){
  const titlePassed=title?.passed===true;
  const stageFile=th08OggTrackFile(stage?.native?.name);
  const stagePassed=stage?.native?.exists===true&&stage.native.soundReady===true&&stage.native.pending===false&&
    stage.native.playing===true&&stage.native.lastMusicSong===expectedStageSong&&typeof title?.observedFile==='string'&&stageFile!==title.observedFile&&
    stage?.input?.name===stageFile&&typeof stage.input.sha256==='string'&&
    isVerifiedRuntimePcmWindow(stage?.pcm,TH08_OGG_PCM_ROLE)&&stage.pcm.outputObserved===true&&
    observedTrackPcmCollection(stage.pcmCollection,stage.pcm,stageFile);
  const passed=titlePassed&&stagePassed&&errors.length===0;
  return {id:'AUDIO-01',status:passed?'partial':'failed',complete:false,passed,
    title:{passed:titlePassed,track:title?.observedFile??null,pcm:title?.pcmObserved===true},
    stage:{passed:stagePassed,expectedSong:expectedStageSong,observedSong:stage?.native?.lastMusicSong??null,
      track:stageFile,input:stage?.input?.name??null,pcm:stage?.pcm?.outputObserved===true},
    pcmScope:'Runtime output PCM from the identified Module.SDL3.audioContext mixer context',
    scope:'Verified title and first-stage OGG sources with Runtime PCM; complete track table, all scene changes and looping remain separate'};
}

/** Decoder policy is observed at every real lifecycle boundary, alongside existing source/PCM checks. */
export function th08OggDecodeVariantOutcome({mode,record,errors=[],requiredBoundaries=['title','stage','pause','background','restart']}={}){
  const expected=mode==='ogg'?0:mode==='ogg-full'?1:null;
  const title=th08OggTitleOutcome({...record?.title,errors});
  const source=th08OggAcceptanceOutcome({title,stage:record?.start?.audio,errors});
  const pause=record?.start?.oggPause,background=record?.start?.background,restart=record?.start?.restart;
  const boundaries={title:[record?.title?.decoder],stage:[record?.start?.audio?.decoder],
    pause:pause?.decoderSamples,background:background?.decoderSamples,restart:restart?.decoderSamples};
  const counts={title:1,stage:1,pause:4,background:5,restart:3};
  assert.ok(Array.isArray(requiredBoundaries)&&requiredBoundaries.includes('title')&&requiredBoundaries.includes('stage')&&
    requiredBoundaries.every(name=>Object.hasOwn(counts,name))&&new Set(requiredBoundaries).size===requiredBoundaries.length);
  const decoderMatched=expected!==null&&requiredBoundaries.every(name=>
    Array.isArray(boundaries[name])&&boundaries[name].length===counts[name]&&boundaries[name].every(decoder=>
      decoder?.enabled===true&&decoder.decodeMode===expected));
  const recomputed={pause:th08OggPauseOutcome(pause?.observationInput),
    background:background?.observationInput?.mode==='ogg'?th08BackgroundContinuityOutcome(background.observationInput):null,
    restart:th08OggRestartOutcome(restart?.observationInput)};
  const passed=source.passed&&decoderMatched&&requiredBoundaries.filter(name=>Object.hasOwn(recomputed,name)).every(name=>recomputed[name]?.passed===true)&&errors.length===0;
  return{mode,expectedDecodeMode:expected,passed,status:passed?'partial':'failed',complete:false,
    source,decoderMatched,requiredBoundaries,decoderBoundaries:boundaries,recomputed,pause:pause??null,background:background??null,restart:restart??null,
    scope:'Verified title/stage OGG sources and PCM, native decoder policy, pause, background recovery and same-frame close/reopen'};
}

export function th08OggDecodeVariantsOutcome({stream,full,errors=[],requiredBoundaries}={}){
  const variants=[th08OggDecodeVariantOutcome({mode:'ogg',record:stream,errors:errors.filter(error=>error.mode==='ogg'),requiredBoundaries}),
    th08OggDecodeVariantOutcome({mode:'ogg-full',record:full,errors:errors.filter(error=>error.mode==='ogg-full'),requiredBoundaries})];
  const passed=variants.every(variant=>variant.passed);
  return{passed,status:passed?'partial':'failed',complete:false,variants,
    scope:'Stream and full decoding share source, PCM and lifecycle semantics; other scenes and listening remain separately scoped'};
}

/** Account for queued mixer PCM, the SDL output block and the analyser's lookback. */
export function th08MusicStopDrainFrames(basis){
  const {native,target,outputBufferFrames,analysisWindowFrames,baseLatency}=basis??{};
  if(native?.playing!==false||native.outputPaused!==false||native.pending!==false||
    !Number.isInteger(native.queuedFrames)||native.queuedFrames<0||native.queuedFrames>3072||
    target?.role!==TH08_OGG_PCM_ROLE||target.available!==true||target.state!=='running'||
    !Number.isFinite(target.sampleRate)||target.sampleRate<=0||!Number.isFinite(target.currentTime)||
    !Number.isInteger(outputBufferFrames)||outputBufferFrames<256||outputBufferFrames>16384||
    !Number.isInteger(analysisWindowFrames)||analysisWindowFrames<2048||analysisWindowFrames>32768||
    !Number.isFinite(baseLatency)||baseLatency<0||baseLatency>1)return null;
  // AudioHost's mixer/SDL input is fixed at 44100 Hz; the output context may differ.
  return runtimePcmSourceHorizonFrames({queuedFrames:native.queuedFrames,queuedSampleRate:44100,
    target,outputBufferFrames,analysisWindowFrames,baseLatency});
}

export function th08MusicRoomOutcome({catalog=[],journey,errors=[]}={}){
  const tracks=journey?.tracks??[];
  const table=catalog.length===21&&tracks.length===catalog.length&&tracks.every((scene,index)=>
    scene.index===index&&scene.observation?.status?.supervisorActive===8&&
    scene.native?.totalFrames===catalog[index].totalFrames&&scene.native?.introFrames===catalog[index].introFrames&&scene.native?.sampleRate===catalog[index].sampleRate&&
    scene.input?.sha256===catalog[index].input?.sha256&&scene.input?.bytes===catalog[index].input?.bytes&&
    th08OggTitleOutcome({...scene,expectedFile:catalog[index].file}).passed);
  const expected=catalog.find(track=>track.index===journey?.loop?.index),samples=journey?.loop?.samples??[];
  const sampleValid=sample=>sample?.name===expected?.file.replace(/\.ogg$/,'.wav')&&sample.playing===true&&
    sample.totalFrames===expected?.totalFrames&&sample.introFrames===expected?.introFrames&&sample.sampleRate===expected?.sampleRate&&
    Number.isFinite(sample.cursorFrames)&&sample.cursorFrames>=0&&sample.cursorFrames<expected.totalFrames;
  const wrap=samples.findIndex((sample,index)=>index>0&&sampleValid(sample)&&sampleValid(samples[index-1])&&
    sample.at>samples[index-1].at&&sample.cursorFrames<samples[index-1].cursorFrames&&
    samples[index-1].cursorFrames>expected.totalFrames-expected.sampleRate&&sample.cursorFrames>=expected.introFrames&&
    sample.cursorFrames<expected.introFrames+expected.sampleRate);
  const loopBasis=journey?.loop?.before?.pcmCollection?.sourceBoundary?.basis;
  const loopHorizon=runtimePcmSourceHorizonFrames(loopBasis);
  const remainingSeconds=expected&&samples.length?(expected.totalFrames-samples[0].cursorFrames)/expected.sampleRate:null;
  const timingBudgetSeconds=loopHorizon===null?null:loopHorizon/loopBasis.target.sampleRate;
  const loop=!!expected&&samples.length>2&&samples.every((sample,index)=>sampleValid(sample)&&Number.isFinite(sample.audioTime)&&
    (!index||sample.at>samples[index-1].at&&sample.audioTime>samples[index-1].audioTime))&&wrap>0&&
    samples[0].cursorFrames<expected.introFrames&&timingBudgetSeconds!==null&&
    (samples.at(-1).at-samples[0].at)/1000>=remainingSeconds-timingBudgetSeconds&&
    samples.at(-1).audioTime-samples[0].audioTime>=remainingSeconds-timingBudgetSeconds&&
    th08OggTitleOutcome({...journey.loop.before,expectedFile:expected.file}).passed&&
    th08OggTitleOutcome({...journey.loop.after,expectedFile:expected.file}).passed;
  const fadeSamples=journey?.fade?.samples??[],gains=fadeSamples.map(sample=>sample.native?.gain);
  const fadeIdentity=!!expected&&gains.length>=8&&fadeSamples.every(sample=>sample.native?.name===expected.file.replace(/\.ogg$/,'.wav')&&
    sample.native?.gain?.name===sample.native.name&&sample.native.gain.sourceAvailable===true&&Number.isFinite(sample.native.gain.value)&&
    sample.native.gain.playing===sample.native.playing);
  const quiet=journey?.fade?.quiet,drainFrames=th08MusicStopDrainFrames(quiet?.drainBasis),settle=quiet?.pcmCollection?.settle;
  const drainIdentity=runtimePcmTargetIdentity(quiet?.drainBasis?.target),quietWindows=quiet?.pcm?.windows;
  const drained=drainFrames!==null&&drainIdentity.ok&&JSON.stringify(drainIdentity.identity)===JSON.stringify(quiet?.pcm?.identity)&&
    Array.isArray(quietWindows)&&quietWindows.length>0&&quietWindows[0]?.target?.currentTime===settle?.completedAtTime&&
    quiet.drainBasis.analysisWindowFrames===Math.max(0,...quietWindows.flatMap(window=>(window.taps??[]).map(tap=>tap.samples)))&&
    settle?.status==='complete'&&settle.requiredFrames===drainFrames&&
    settle.startedAtTime>=quiet.drainBasis.target.currentTime&&settle.elapsedFrames>=drainFrames&&
    (settle.completedAtTime-settle.startedAtTime)*quiet.drainBasis.target.sampleRate>=drainFrames&&
    quiet.pcm?.firstCurrentTime>=settle.completedAtTime&&quiet.pcm.identity?.contextIndex===quiet.drainBasis.target.contextIndex&&
    quiet.pcm.sampleRate===quiet.drainBasis.target.sampleRate;
  const fade=fadeIdentity&&drained&&gains[0].playing===true&&gains[0].value>0&&new Set(gains.map(gain=>gain.volume)).size>=8&&
    gains.every((gain,index)=>Number.isInteger(gain.volume)&&gain.value>=0&&
      Math.abs(gain.value-Math.pow(10,gain.volume/2000))<=Math.max(1e-7,Math.pow(10,gain.volume/2000)*1e-5)&&
      (!index||gain.value<=gains[index-1].value+1e-6&&gain.volume<=gains[index-1].volume))&&
    gains.at(-1).volume<=-4900&&gains.at(-1).playing===false&&fadeSamples.at(-1).at-fadeSamples[0].at>=7500&&
    isVerifiedRuntimePcmWindow(journey.fade.quiet?.pcm,TH08_OGG_PCM_ROLE,{minimumWindows:8})&&journey.fade.quiet.pcm.outputObserved===false&&
    th08OggTitleOutcome({...journey.fade.reset,expectedFile:expected.file}).passed&&journey.fade.reset.native.cursorFrames<expected.sampleRate*5;
  const passed=table&&loop&&fade&&journey?.entered?.status?.supervisorActive===8&&
    journey?.exited?.status?.supervisorActive===1&&errors.length===0;
  return{id:'th08-music-room',passed,status:passed?'partial':'failed',complete:false,table,loop,fade,
    expectedTracks:catalog.length,observedTracks:tracks.length,drained,drainFrames,errors,
    loopTiming:{remainingSeconds,timingBudgetSeconds,firstCursorFrames:samples[0]?.cursorFrames??null},
    scope:'Music Room resource table and PCM, natural loop, keyboard fade and Reset; displayed title text and other scene transitions remain separate'};
}

function cursorSame(a,b){return Number.isFinite(a)&&Number.isFinite(b)&&a===b;}
export function th08OggPauseOutcome({before,paused,settled,resumed,pauseEntered,resumeExited}={}){
  const sameTrack=typeof before?.name==='string'&&before.name===paused?.name&&paused.name===settled?.name&&settled.name===resumed?.name;
  const held=before?.playing===true&&paused?.playing===false&&settled?.playing===false&&
    cursorSame(paused?.cursorFrames,settled?.cursorFrames);
  const resumedFromPlace=resumed?.playing===true&&resumed?.cursorFrames>settled?.cursorFrames;
  const passed=sameTrack&&held&&resumedFromPlace&&pauseEntered===true&&resumeExited===true;
  return {id:'AUDIO-03',status:passed?'partial':'failed',complete:false,passed,sameTrack,held,
    resumedFromPlace,pauseEntered:pauseEntered===true,resumeExited:resumeExited===true,
    cursors:[before?.cursorFrames??null,paused?.cursorFrames??null,settled?.cursorFrames??null,resumed?.cursorFrames??null],
    scope:'Gameplay pause holds the active OGG cursor and resume advances the same source'};
}

export function th08MidiPauseOutcome({before,paused,resumed,pauseEntered,resumeExited,bridgeContext='running'}={}){
  const sameTrack=typeof before?.path==='string'&&before.path===paused?.path&&paused.path===resumed?.path;
  const sequencerKeepsTime=before?.active===true&&paused?.active===true&&resumed?.active===true&&
    paused?.elapsedTicks>before?.elapsedTicks&&resumed?.elapsedTicks>paused?.elapsedTicks;
  const passed=sameTrack&&sequencerKeepsTime&&pauseEntered===true&&resumeExited===true&&bridgeContext==='running';
  return {id:'MIDI-03',status:passed?'partial':'failed',complete:false,passed,sameTrack,sequencerKeepsTime,
    pauseEntered:pauseEntered===true,resumeExited:resumeExited===true,bridgeContext,
    elapsedTicks:[before?.elapsedTicks??null,paused?.elapsedTicks??null,resumed?.elapsedTicks??null],
    scope:'Native MIDI timing continues through the gameplay pause path, as selected by TH08 MusicCommands'};
}

/** Recompute source establishment from raw native/clock samples and measured buffers. */
export function isVerifiedOggSourceBoundary(boundary,pcm,expectedFile){
  const {basis,settle,samples}=boundary??{},frames=runtimePcmSourceHorizonFrames(basis);
  const identity=runtimePcmTargetIdentity(basis?.target);
  if(frames===null||basis.queuedSampleRate!==44100||basis.queuedFrames>3072||boundary.file!==expectedFile||
    !identity.ok||identity.identity.role!==TH08_OGG_PCM_ROLE||JSON.stringify(identity.identity)!==JSON.stringify(pcm?.identity)||
    !Array.isArray(samples)||samples.length<2||!Number.isSafeInteger(boundary.firstObservedAtAttempt)||boundary.firstObservedAtAttempt<1||
    !Array.isArray(pcm?.windows)||!pcm.windows.length||
    settle?.status!=='complete'||settle.requiredFrames!==frames||settle.startedAtTime!==basis.target.currentTime||
    basis.analysisWindowFrames!==Math.max(0,...pcm.windows.flatMap(window=>(window.taps??[]).map(tap=>tap.samples))))return false;
  if(JSON.stringify(samples[0]?.target)!==JSON.stringify(basis.target))return false;
  for(let index=0;index<samples.length;index++){
    const {native,target,trackFile,buffering,attempt}=samples[index]??{},current=runtimePcmTargetIdentity(target);
    if(attempt!==boundary.firstObservedAtAttempt+index||!Number.isFinite(native?.cursorFrames)||native.cursorFrames<0||
      trackFile!==expectedFile||th08OggTrackFile(native?.name)!==expectedFile||native?.exists!==true||
      native.soundReady!==true||native.pending!==false||native.playing!==true||native.outputPaused!==false||
      !current.ok||JSON.stringify(current.identity)!==JSON.stringify(identity.identity)||target.state!=='running'||
      !Number.isFinite(target.currentTime)||target.stateChangeDrops!==0||!Array.isArray(target.stateChanges)||
      target.stateChangeCount!==target.stateChanges.length||target.stateChanges.some((change,i)=>change.sequence!==i+1)||
      target.stateChanges.some(change=>change.sequence>basis.target.stateChangeCount&&change.state!=='running')||
      (index===0?target.currentTime!==basis.target.currentTime:target.currentTime<=samples[index-1].target.currentTime)||
      buffering?.queuedFrames!==native.queuedFrames||!Number.isInteger(native.queuedFrames)||native.queuedFrames<0||native.queuedFrames>3072||
      ['queuedSampleRate','outputBufferFrames','analysisWindowFrames','baseLatency'].some(key=>buffering?.[key]!==basis[key]))return false;
  }
  if(samples[0].buffering.queuedFrames!==basis.queuedFrames)return false;
  if(samples.every(sample=>sample.native.cursorFrames===samples[0].native.cursorFrames))return false;
  const established=samples.findIndex(sample=>(sample.target.currentTime-basis.target.currentTime)*basis.target.sampleRate>=frames);
  if(established<0||settle.completedAtTime!==samples[established].target.currentTime||
    settle.elapsedFrames!==(settle.completedAtTime-settle.startedAtTime)*basis.target.sampleRate)return false;
  const outputSamples=samples.slice(established);
  return outputSamples.length===pcm.windows.length&&outputSamples.every((sample,index)=>sample.target.currentTime===pcm.windows[index].target.currentTime);
}

/** A Runtime close must clear the active OGG output before its same-frame relaunch. */
function observedTrackPcmCollection(collection,pcm,expectedFile){
  const target=runtimePcmTargetIdentity(collection?.target);
  const pcmIdentity=runtimePcmTargetIdentity(pcm?.identity&&{...pcm.identity,available:true});
  return collection?.status==='observed'&&collection.expectedFile===expectedFile&&collection.observedFile===expectedFile&&
    collection.nativeReady===true&&Number.isSafeInteger(collection.attempts)&&collection.attempts>0&&
    Number.isSafeInteger(collection.maxSamples)&&collection.maxSamples>=collection.attempts&&
    Number.isSafeInteger(collection.pcmObservedAtAttempt)&&collection.pcmObservedAtAttempt>0&&
    collection.pcmObservedAtAttempt<=collection.attempts&&collection.outputObserved===true&&
    Number.isSafeInteger(collection.nonZeroSamples)&&collection.nonZeroSamples>0&&
    collection.nonZeroSamples===pcm?.nonZeroSamples&&collection.captureCount===pcm?.captureCount&&
    collection.windowCount===pcm?.windowCount&&target.ok&&pcmIdentity.ok&&
    JSON.stringify(target.identity)===JSON.stringify(pcmIdentity.identity)&&
    target.identity.role===TH08_OGG_PCM_ROLE&&isVerifiedOggSourceBoundary(collection.sourceBoundary,pcm,expectedFile)&&
    collection.attempts===collection.sourceBoundary.firstObservedAtAttempt+collection.sourceBoundary.samples.length-1;
}

export function th08OggRestartOutcome({before,quietWindow,reopened,expectedBeforeFile='th08_00.ogg',expectedFile='th08_01.ogg',sameRuntimeFrame,exitObserved}={}){
  const beforeNativePlaying=before?.native?.exists===true&&before.native.soundReady===true&&before.native.pending===false&&
    before.native.playing===true&&before.native.outputPaused===false;
  const beforeTrackIdentity=before?.trackFile===expectedBeforeFile&&before?.input?.name===expectedBeforeFile&&
    /^[a-f0-9]{64}$/.test(before?.input?.sha256??'')&&before?.native?.name?.replace(/\.wav$/i,'.ogg')===expectedBeforeFile;
  const beforePcmVerified=isVerifiedRuntimePcmWindow(before?.pcm,TH08_OGG_PCM_ROLE);
  const beforePcmCollection=before?.pcmCollection??null;
  const beforePcmSignalObserved=beforePcmVerified&&before.pcm.outputObserved===true;
  const beforePcmCollectionValid=beforePcmSignalObserved&&observedTrackPcmCollection(beforePcmCollection,before.pcm,expectedBeforeFile);
  const beforeOutputObserved=beforePcmCollectionValid;
  const beforePcmStatus=beforeOutputObserved?'passed':'incomplete';
  const beforePcmReason=beforeOutputObserved?null:beforePcmSignalObserved?
    'Track-matched PCM collection receipt does not agree with the captured source and samples':
    beforePcmCollection?.reason??before?.pcm?.reason??
    (beforePcmVerified?'The bounded active-track capture did not observe non-zero PCM':'Active-track PCM capture is incomplete');
  const beforePlaying=beforeNativePlaying&&beforeTrackIdentity&&beforeOutputObserved;
  const quietEvidence=stoppedRuntimeAudioEvidence(quietWindow,TH08_OGG_PCM_ROLE,{allowClosed:true});
  const quiet=quietEvidence.passed;
  const reopenedNativePlaying=reopened?.native?.exists===true&&reopened.native.soundReady===true&&
    reopened.native.pending===false&&reopened.native.playing===true&&reopened.native.outputPaused===false;
  const reopenedTrackIdentity=reopened?.trackFile===expectedFile&&reopened?.input?.name===expectedFile&&
    /^[a-f0-9]{64}$/.test(reopened?.input?.sha256??'')&&reopened?.native?.name?.replace(/\.wav$/i,'.ogg')===expectedFile;
  const reopenedPcmVerified=isVerifiedRuntimePcmWindow(reopened?.pcm,TH08_OGG_PCM_ROLE);
  const reopenedPcmSignalObserved=reopenedPcmVerified&&reopened.pcm.outputObserved===true;
  const reopenedPcmCollectionValid=reopenedPcmSignalObserved&&
    observedTrackPcmCollection(reopened?.pcmCollection,reopened.pcm,expectedFile);
  const reopenedOutputObserved=reopenedPcmCollectionValid;
  const reopenedPcmStatus=reopenedOutputObserved?'passed':'incomplete';
  const reopenedPcmReason=reopenedOutputObserved?null:reopenedPcmSignalObserved?
    'Track-matched PCM collection receipt does not agree with the captured source and samples':
    reopened?.pcmCollection?.reason??reopened?.pcm?.reason??
    (reopenedPcmVerified?'The bounded reopened-track capture did not observe non-zero PCM':'Reopened-track PCM capture is incomplete');
  const reopenedSource=reopenedNativePlaying&&reopenedTrackIdentity&&reopenedOutputObserved;
  const otherChecksPassed=beforePlaying&&reopenedSource&&sameRuntimeFrame===true&&exitObserved===true;
  const passed=otherChecksPassed&&quiet;
  const provenFailure=!beforeNativePlaying||!beforeTrackIdentity||!reopenedNativePlaying||!reopenedTrackIdentity||
    sameRuntimeFrame!==true||exitObserved!==true||quietEvidence.status==='failed';
  const status=passed?'partial':provenFailure?'failed':'incomplete';
  return {id:'AUDIO-03-restart',status,passed,complete:false,beforePlaying,beforeNativePlaying,
    beforeNativeStatus:beforeNativePlaying?'passed':'failed',beforeTrackIdentity,beforeTrackIdentityStatus:beforeTrackIdentity?'passed':'failed',
    expectedBeforeTrack:expectedBeforeFile,beforeOutputObserved,beforePcmSignalObserved,beforePcmCollectionValid,
    beforePcmStatus,beforePcmReason,
    beforePcm:before?.pcm??null,beforePcmCollection,
    quiet,quietStatus:quietEvidence.status,
    quietReason:quietEvidence.reason??null,
    reopenedSource,reopenedNativePlaying,reopenedNativeStatus:reopenedNativePlaying?'passed':'failed',
    reopenedTrackIdentity,reopenedTrackIdentityStatus:reopenedTrackIdentity?'passed':'failed',
    reopenedOutputObserved,reopenedPcmSignalObserved,reopenedPcmCollectionValid,reopenedPcmStatus,reopenedPcmReason,
    reopenedPcmCollection:reopened?.pcmCollection??null,
    sameRuntimeFrame:sameRuntimeFrame===true,exitObserved:exitObserved===true,
    beforeTrack:before?.trackFile??null,expectedReopenedTrack:expectedFile,reopenedTrack:reopened?.trackFile??null,
    quietProof:quietEvidence.proof,
    pcmScope:'Runtime output PCM from the identified Module.SDL3.audioContext mixer context',
    quietWindow:quietWindow??null,
    scope:'The Runtime output graph stops with a verified quiet PCM window or an identity-matched closed-context receipt, then the same Runtime frame reopens the title OGG source with PCM'};
}

/** The real Launcher bridge must panic active TinySynth voices on Runtime close. */
export function th08MidiRestartOutcome({before,closeEventsBefore,closeEventsAfter,resetCountBefore,resetCountAfter,
  activeVoicesBefore,activeVoicesAfter,closeResetReceipts,closeResetReceipt,postCloseMessages,postCloseNativeEvents,quietWindow,voicePcmWindow,
  reopenedEvents=[],reopenedCorrelation,reopened,sameRuntimeFrame,exitObserved}={}){
  const beforePlaying=before?.active===true&&before?.invalid===false&&typeof before?.path==='string'&&before.path.length>0&&
    before?.messageCount>0&&before?.noteOnCount>0&&activeVoicesBefore>0;
  const closeObserved=Number.isSafeInteger(closeEventsBefore)&&Number.isSafeInteger(closeEventsAfter)&&closeEventsAfter>closeEventsBefore;
  const receipts=Array.isArray(closeResetReceipts)?closeResetReceipts:closeResetReceipt?[closeResetReceipt]:[];
  const resetCountDelta=Number.isSafeInteger(resetCountBefore)&&Number.isSafeInteger(resetCountAfter)?resetCountAfter-resetCountBefore:null;
  const receiptSequenceComplete=resetCountDelta!==null&&resetCountDelta>0&&receipts.length===resetCountDelta&&
    receipts.every((receipt,index)=>receipt?.resetCount===resetCountBefore+index+1);
  const voiceClearIndex=receipts.findIndex(receipt=>receipt?.activeVoicesBefore>0&&receipt?.activeVoicesAfter===0);
  const voiceClearReceipt=voiceClearIndex<0?null:receipts[voiceClearIndex];
  const wetResetReceiptValid=receipt=>receipt?.kind==='tinysynth-reset'&&Number.isFinite(receipt.contextTime)&&
    Number.isFinite(receipt.sampleRate)&&receipt.sampleRate>0&&Number.isSafeInteger(receipt.reverbBufferFrames)&&
    receipt.reverbBufferFrames>0&&Number.isSafeInteger(receipt.reverbBufferFramesAfter)&&
    receipt.reverbBufferFramesAfter===receipt.reverbBufferFrames&&receipt.reverbEnabled===true&&
    receipt.wetPathReplaced===true&&receipt.convolverNodeReplaced===true&&receipt.reverbGainNodeReplaced===true;
  const firstVoiceClearValid=!!voiceClearReceipt&&voiceClearReceipt.activeVoicesBefore>0&&voiceClearReceipt.activeVoicesAfter===0&&
    wetResetReceiptValid(voiceClearReceipt);
  const laterResetsClear=voiceClearIndex>=0&&receipts.slice(voiceClearIndex+1).every(receipt=>receipt?.activeVoicesAfter===0&&wetResetReceiptValid(receipt));
  const closeReceiptsValid=receiptSequenceComplete&&voiceClearIndex>=0&&receipts.slice(0,voiceClearIndex).every(receipt=>
    receipt?.activeVoicesAfter===0&&wetResetReceiptValid(receipt))&&firstVoiceClearValid&&laterResetsClear;
  const resetReceiptValid=closeReceiptsValid;
  const selectedResetReceipt=voiceClearReceipt??(receipts.at(-1)??null);
  const resetCountAdvanced=Number.isSafeInteger(resetCountBefore)&&Number.isSafeInteger(resetCountAfter)&&resetCountAfter>resetCountBefore;
  const synthReset=resetCountAdvanced&&resetReceiptValid;
  const voicesCleared=activeVoicesAfter===0&&activeVoicesBefore>0;
  const noPlaybackAfterClose=postCloseMessages===0&&postCloseNativeEvents===0;
  const finalPcm=quietWindow?.pcmWindow;
  const finalMixVerified=quietWindow?.status==='complete'&&quietWindow?.proof==='runtime-output-pcm-window'&&
    quietWindow.role===TH08_MIDI_PCM_ROLE&&isVerifiedRuntimePcmWindow(finalPcm,TH08_MIDI_PCM_ROLE,{minimumWindows:8});
  const outputQuiet=finalMixVerified&&finalPcm.outputObserved===false&&finalPcm.nonZeroSamples===0;
  const finalMixFailed=finalMixVerified&&finalPcm.outputObserved===true&&finalPcm.nonZeroSamples>0;
  const voicePcm=voicePcmWindow?.pcmWindow;
  const voicePcmVerified=voicePcmWindow?.status==='complete'&&voicePcmWindow?.proof==='runtime-output-pcm-window'&&
    voicePcmWindow.role===TH08_MIDI_VOICE_PCM_ROLE&&isVerifiedRuntimePcmWindow(voicePcm,TH08_MIDI_VOICE_PCM_ROLE,{minimumWindows:8});
  const voiceOutputSilent=voicePcmVerified&&voicePcm.outputObserved===false&&voicePcm.nonZeroSamples===0;
  const voiceOutputFailed=voicePcmVerified&&voicePcm.outputObserved===true&&voicePcm.nonZeroSamples>0;
  const sourceOutputWindowsAligned=runtimePcmWindowsAligned(finalPcm,voicePcm);
  const effectTailObserved=finalMixFailed&&voiceOutputSilent&&sourceOutputWindowsAligned;
  const replayedNotes=reopenedCorrelation?.status==='matched'&&reopenedCorrelation.eventCount===reopenedCorrelation.sendCount&&
    reopenedCorrelation.eventCount===reopenedEvents.length&&reopenedCorrelation.matchedCount===reopenedEvents.length&&
    midiCountsMatch(reopened?.native,reopenedEvents)&&
    reopenedEvents.some(event=>hasTh08MidiNote(event?.sentBytes,'on'))&&
    reopenedEvents.some(event=>hasTh08MidiNote(event?.sentBytes,'off'));
  const reopenedSource=reopened?.native?.active===true&&reopened.native.invalid===false&&reopened.native.path==='bgm/th08_01.mid'&&
    reopened.native.messageCount>0&&reopened.native.noteOnCount>0&&replayedNotes&&
    isVerifiedRuntimePcmWindow(reopened?.pcm,TH08_MIDI_PCM_ROLE)&&reopened.pcm.outputObserved===true&&
    reopened?.contextState==='running';
  const otherChecksPassed=beforePlaying&&closeObserved&&synthReset&&voicesCleared&&noPlaybackAfterClose&&reopenedSource&&
    sameRuntimeFrame===true&&exitObserved===true;
  const passed=otherChecksPassed&&finalMixVerified&&outputQuiet&&voiceOutputSilent;
  const provenFailure=!beforePlaying||!closeObserved||!resetCountAdvanced||Boolean(receipts.length&&!resetReceiptValid)||
    !voicesCleared||!noPlaybackAfterClose||!reopenedSource||sameRuntimeFrame!==true||exitObserved!==true||voiceOutputFailed||finalMixFailed;
  const status=passed?'partial':provenFailure?'failed':'incomplete';
  const finalPcmReason=finalMixVerified?null:quietWindow?.reason??finalPcm?.reason??'TinySynth final-output PCM window is incomplete';
  const voicePcmReason=voicePcmVerified?null:voicePcmWindow?.reason??voicePcm?.reason??'TinySynth pre-effect voice PCM window is incomplete';
  return {id:'MIDI-03-restart',status,passed,complete:false,beforePlaying,closeObserved,synthReset,
    voicesCleared,noPlaybackAfterClose,outputQuiet,voiceOutputSilent,voiceOutputStatus:voicePcmVerified?voiceOutputSilent?'silent':'nonzero':'incomplete',
    voicePcmReason,voicePcmWindow:voicePcmWindow??null,sourceOutputWindowsAligned,effectAttributionStatus:sourceOutputWindowsAligned?'complete':'incomplete',
    resetReceiptValid,closeResetReceiptCount:receipts.length,voiceClearReceiptIndex:voiceClearIndex,
    subsequentResetCount:voiceClearIndex<0?null:receipts.length-voiceClearIndex-1,laterResetsClear,
    resetReceipt:selectedResetReceipt?{kind:selectedResetReceipt.kind,resetCount:selectedResetReceipt.resetCount,contextTime:selectedResetReceipt.contextTime,
      sampleRate:selectedResetReceipt.sampleRate,reverbEnabled:selectedResetReceipt.reverbEnabled,
      reverbBufferFrames:selectedResetReceipt.reverbBufferFrames,reverbBufferFramesAfter:selectedResetReceipt.reverbBufferFramesAfter,
      wetPathReplaced:selectedResetReceipt.wetPathReplaced,convolverNodeReplaced:selectedResetReceipt.convolverNodeReplaced,
      reverbGainNodeReplaced:selectedResetReceipt.reverbGainNodeReplaced,
      activeVoicesBefore:selectedResetReceipt.activeVoicesBefore,
      activeVoicesAfter:selectedResetReceipt.activeVoicesAfter}:null,finalMixVerified,effectTailObserved,
    finalOutput:{role:TH08_MIDI_PCM_ROLE,status:finalMixVerified?'complete':'incomplete',outputObserved:finalMixVerified?finalPcm.outputObserved:null,
      nonZeroSamples:finalMixVerified?finalPcm.nonZeroSamples:null,firstCurrentTime:finalMixVerified?finalPcm.firstCurrentTime:null,
      lastCurrentTime:finalMixVerified?finalPcm.lastCurrentTime:null,windowCount:finalMixVerified?finalPcm.windowCount:null},
    finalPcmReason,quietProof:outputQuiet?'runtime-output-pcm-window':null,
    quietStatus:finalMixVerified?outputQuiet?'passed':'failed':'incomplete',
    quietReason:finalMixFailed?`Runtime output PCM remained nonzero after close (${finalPcm.nonZeroSamples} samples)`:finalPcmReason,
    closeEvents:[closeEventsBefore??null,closeEventsAfter??null],resetCounts:[resetCountBefore??null,resetCountAfter??null],
    activeVoices:[activeVoicesBefore??null,activeVoicesAfter??null],postCloseMessages:postCloseMessages??null,
    postCloseNativeEvents:postCloseNativeEvents??null,
    replayedNoteEvents:reopenedEvents.filter(event=>hasTh08MidiNote(event?.sentBytes,'on')||hasTh08MidiNote(event?.sentBytes,'off')).length,
    reopenedSendCorrelation:midiCorrelationSummary(reopenedCorrelation),
    scope:'Runtime close clears the TinySynth voice and old wet effect path; simultaneous source and final output PCM windows must be silent before the same Runtime frame starts a new title MIDI stream'};
}

function runtimePcmWindowsAligned(output,voice){
  if(output?.status!=='complete'||voice?.status!=='complete'||output.windowCount!==voice.windowCount||
     output.identity?.contextIndex!==voice.identity?.contextIndex||output.identity?.sampleRate!==voice.identity?.sampleRate||
     !Array.isArray(output.windows)||!Array.isArray(voice.windows)||output.windows.length!==voice.windows.length)return false;
  return output.windows.every((window,index)=>{
    const other=voice.windows[index];
    return window?.target?.available===true&&other?.target?.available===true&&
      window.target.contextIndex===other.target.contextIndex&&Number.isFinite(window.target.currentTime)&&
      window.target.currentTime===other?.target?.currentTime&&
      window.target.state===other?.target?.state&&window.target.sampleRate===other?.target?.sampleRate&&
      Array.isArray(window.taps)&&Array.isArray(other?.taps)&&window.taps.length===1&&other.taps.length===1&&
      window.taps[0].contextState==='running'&&other.taps[0].contextState==='running'&&
      window.taps[0].sampleRate===other.taps[0].sampleRate&&window.taps[0].samples===other.taps[0].samples&&
      window.taps[0].currentTime===other.taps[0].currentTime&&window.taps[0].currentTime===window.target.currentTime&&
      other.taps[0].currentTime===other.target.currentTime;
  });
}

function hasTh08MidiNote(bytes,kind){
  if(!Array.isArray(bytes)||bytes.length<3)return false;
  const status=bytes[0]&0xf0;
  return kind==='on'?status===0x90&&bytes[2]>0:status===0x80||(status===0x90&&bytes[2]===0);
}

/** MIDI proof requires the native sequencer path, real bridged bytes and the Launcher TinySynth output tap. */
export function th08MidiTitleOutcome({native,expectedPath='bgm/th08_01.mid',events=[],sendCorrelation,pcm,contextState,errors=[]}={}){
  const isNoteOn=bytes=>Array.isArray(bytes)&&bytes.length>=3&&(bytes[0]&0xf0)===0x90&&bytes[2]>0;
  const isNoteOff=bytes=>Array.isArray(bytes)&&bytes.length>=3&&((bytes[0]&0xf0)===0x80||((bytes[0]&0xf0)===0x90&&bytes[2]===0));
  const notes=events.filter(event=>isNoteOn(event.bytes)||isNoteOff(event.bytes)).length;
  const source=native?.path===expectedPath&&native?.active===true&&native?.invalid===false&&native?.fileIndex>=0&&native?.trackCount>0;
  const sentNoteOns=events.filter(event=>isNoteOn(event.sentBytes)).length;
  const sentNoteOffs=events.filter(event=>isNoteOff(event.sentBytes)).length;
  const sentNotes=sentNoteOns+sentNoteOffs;
  const orderedTransport=sendCorrelation?.status==='matched'&&sendCorrelation.eventCount===events.length&&
    sendCorrelation.sendCount===events.length&&sendCorrelation.matchedCount===events.length&&
    midiCountsMatch(native,events);
  const transport=orderedTransport&&native?.messageCount>0&&native?.noteOnCount>0&&notes>0&&sentNoteOns>0&&sentNoteOffs>0;
  const audible=isVerifiedRuntimePcmWindow(pcm,TH08_MIDI_PCM_ROLE)&&pcm?.outputObserved===true&&contextState==='running';
  const passed=source&&transport&&audible&&errors.length===0;
  return {id:'th08-midi-title-bridge-pcm',status:passed?'passed':'failed',passed,complete:false,expectedPath,
    observedPath:native?.path??null,nativeMessages:native?.messageCount??0,nativeSysex:native?.sysexCount??0,
    nativeTransportCount:th08MidiTransportCount(native),nativeNoteOns:native?.noteOnCount??0,
    observedNoteEvents:notes,observedSynthNoteOns:sentNoteOns,observedSynthNoteOffs:sentNoteOffs,observedSynthNoteSends:sentNotes,
    contextState:contextState??null,pcmObserved:pcm?.outputObserved===true,sendCorrelation:midiCorrelationSummary(sendCorrelation),
    pcmScope:'Runtime output PCM from the identity-verified Launcher TinySynth AudioContext',
    scope:'Title SMF identity, native sequencer messages, Launcher MIDI bridge and TinySynth PCM'};
}

export function isVerifiedRuntimePcmWindow(pcm,role,{minimumWindows=1}={}){
  const identity=pcm?.identity,windows=pcm?.windows;
  const expectedSourceRole=role===TH08_MIDI_PCM_ROLE?'tinysynth-compressor-output':role===TH08_MIDI_VOICE_PCM_ROLE?'tinysynth-voice-output':null;
  if(pcm?.status!=='complete'||pcm.role!==role||identity?.role!==role||!Number.isInteger(identity.contextIndex)||identity.contextIndex<0||
     !Number.isFinite(identity.sampleRate)||identity.sampleRate<=0||!Array.isArray(identity.tapIds)||identity.tapIds.length===0||
     expectedSourceRole&&identity.sourceIdentity?.role!==expectedSourceRole||
     !Number.isInteger(pcm.windowCount)||pcm.windowCount<minimumWindows||!Array.isArray(windows)||windows.length!==pcm.windowCount||
     (pcm.windowCount>1&&(!Number.isFinite(pcm.elapsedFrames)||pcm.elapsedFrames<=0))||!Array.isArray(pcm.issues)||pcm.issues.length>0)return false;
  let previous=null;let captureCount=0,nonZeroSamples=0;const tapTimes=new Map();
  for(const window of windows){
    const target=window?.target,taps=window?.taps;
    if(target?.role!==role||target.available!==true||target.contextIndex!==identity.contextIndex||target.state!=='running'||
       target.sampleRate!==identity.sampleRate||!Number.isFinite(target.currentTime)||!Number.isSafeInteger(target.stateChangeCount)||
       expectedSourceRole&&JSON.stringify(target.sourceIdentity)!==JSON.stringify(identity.sourceIdentity)||
       !Number.isSafeInteger(target.stateChangeDrops)||target.stateChangeDrops!==0||!Array.isArray(target.stateChanges)||
       target.stateChangeCount!==target.stateChanges.length||!Array.isArray(taps)||taps.length!==identity.tapIds.length)return false;
    if(previous){
      if(target.currentTime<=previous.currentTime||target.stateChangeCount<previous.stateChangeCount)return false;
      if(target.stateChanges.some(change=>change.sequence>previous.stateChangeCount&&change.state!=='running'))return false;
    }
    for(const tapId of identity.tapIds){
      const matches=taps.filter(tap=>tap?.tapId===tapId);
      if(matches.length!==1)return false;
      const tap=matches[0];
      if(tap.contextState!=='running'||tap.sampleRate!==identity.sampleRate||!Number.isFinite(tap.currentTime)||
         expectedSourceRole&&JSON.stringify(tap.sourceIdentity)!==JSON.stringify(identity.sourceIdentity)||
         !Number.isSafeInteger(tap.samples)||tap.samples<=0||!Number.isSafeInteger(tap.nonZero)||tap.nonZero<0||tap.nonZero>tap.samples||
         !Number.isFinite(tap.peakAbs)||tap.peakAbs<0||!Number.isFinite(tap.rms)||tap.rms<0||
         tapTimes.has(tapId)&&tap.currentTime<=tapTimes.get(tapId))return false;
      tapTimes.set(tapId,tap.currentTime);
      captureCount++;nonZeroSamples+=tap.nonZero;
    }
    previous=target;
  }
  const elapsedFrames=windows.length>1?(windows.at(-1).target.currentTime-windows[0].target.currentTime)*identity.sampleRate:null;
  return pcm.firstCurrentTime===windows[0].target.currentTime&&pcm.lastCurrentTime===windows.at(-1).target.currentTime&&
    (elapsedFrames===null||Math.abs(pcm.elapsedFrames-elapsedFrames)<1e-6)&&
    captureCount===pcm.captureCount&&nonZeroSamples===pcm.nonZeroSamples&&pcm.outputObserved===(nonZeroSamples>0);
}

function stoppedRuntimeAudioEvidence(window,role,{allowClosed=false}={}){
  const pcm=window?.pcmWindow;
  const silentPcm=window?.status==='complete'&&window.proof==='runtime-output-pcm-window'&&window.role===role&&
    isVerifiedRuntimePcmWindow(pcm,role,{minimumWindows:8})&&pcm.outputObserved===false&&pcm.nonZeroSamples===0;
  if(silentPcm)return {passed:true,status:'passed',proof:'runtime-output-pcm-window',reason:null};
  const receipt=window?.lifecycleReceipt,identity=receipt?.identity;
  const closedTarget=runtimePcmTargetIdentity(window?.target);
  const closed=allowClosed&&window?.status==='closed'&&window.proof==='closed-context-lifecycle'&&window.role===role&&
    receipt?.status==='passed'&&receipt.kind==='runtime-audio-context-closed'&&receipt.role===role&&
    identity?.role===role&&Number.isInteger(identity.contextIndex)&&identity.contextIndex>=0&&Number.isFinite(identity.sampleRate)&&identity.sampleRate>0&&
    Array.isArray(identity.tapIds)&&identity.tapIds.length>0&&new Set(identity.tapIds).size===identity.tapIds.length&&
    identity.tapIds.every(tapId=>Number.isInteger(tapId)&&tapId>=0)&&closedTarget.ok&&window.target.role===role&&window.target.state==='closed'&&
    JSON.stringify(closedTarget.identity)===JSON.stringify({role,contextIndex:identity.contextIndex,
      tapIds:[...identity.tapIds].sort((a,b)=>a-b),sampleRate:identity.sampleRate})&&
    receipt.beforeState==='running'&&receipt.afterState==='closed'&&Number.isSafeInteger(receipt.beforeStateChangeCount)&&
    Number.isSafeInteger(receipt.afterStateChangeCount)&&receipt.afterStateChangeCount>receipt.beforeStateChangeCount&&
    Number.isFinite(receipt.beforeCurrentTime)&&Number.isFinite(receipt.afterCurrentTime)&&Array.isArray(receipt.issues)&&receipt.issues.length===0&&
    pcm?.status==='incomplete'&&pcm.captureCount===0&&pcm.windowCount===0&&pcm.outputObserved===null;
  if(closed)return {passed:true,status:'passed',proof:'closed-context-lifecycle',reason:null};
  const completePcmViolation=window?.status==='complete'&&window.proof==='runtime-output-pcm-window'&&window.role===role&&
    isVerifiedRuntimePcmWindow(pcm,role,{minimumWindows:8})&&pcm.outputObserved===true&&pcm.nonZeroSamples>0;
  if(completePcmViolation)return {passed:false,status:'failed',proof:null,
    reason:`Runtime output PCM remained nonzero after close (${pcm.nonZeroSamples} samples)`};
  const reason=window?.status==='incomplete'?window.reason??pcm?.reason??receipt?.reason??'Runtime audio stop evidence is incomplete':
    window?.status==='closed'?receipt?.reason??'closed-context lifecycle receipt is incomplete':
    !window?'Runtime audio stop observation is missing':'Runtime audio stop evidence is not a verified quiet window';
  return {passed:false,status:'incomplete',proof:null,reason};
}

/** A foreground recovery must retain one source and its cursor, then resume progress. */
export function th08BackgroundContinuityOutcome({mode,before,paused,settled,resumed,resumedOutput,bridgeBefore,bridgePaused,bridgeResumed}={}){
  if(mode==='ogg'){
    const sameName=typeof before?.name==='string'&&before.name===paused?.name&&paused?.name===settled?.name&&settled?.name===resumed?.name;
    const stopped=paused?.outputPaused===true&&settled?.outputPaused===true&&paused?.cursorFrames===settled?.cursorFrames&&
      paused?.visibilityState==='hidden'&&settled?.visibilityState==='hidden';
    const continued=resumed?.outputPaused===false&&resumed?.cursorFrames>settled?.cursorFrames&&resumed?.visibilityState==='visible';
    const nativeContinued=sameName&&before?.playing===true&&paused?.soundReady===true&&stopped&&continued&&resumed?.playing===true;
    const outputObserved=isVerifiedRuntimePcmWindow(resumedOutput?.pcm,TH08_OGG_PCM_ROLE)&&resumedOutput.pcm.outputObserved===true&&
      observedTrackPcmCollection(resumedOutput.pcmCollection,resumedOutput.pcm,th08OggTrackFile(before?.name));
    const passed=nativeContinued&&outputObserved;
    return {id:'th08-ogg-background-cursor-continuity',status:passed?'passed':nativeContinued?'incomplete':'failed',passed,complete:false,
      nativeContinued,outputObserved,resumedOutput:resumedOutput??null,
      sameSource:sameName,pausedCursor:paused?.cursorFrames??null,settledCursor:settled?.cursorFrames??null,
      resumedCursor:resumed?.cursorFrames??null,scope:'AudioHost retains its source and cursor across suspension, then the same track reaches actual Runtime PCM after recovery'};
  }
  const samePath=typeof before?.path==='string'&&before.path===paused?.path&&paused?.path===settled?.path&&settled?.path===resumed?.path;
  const held=paused?.active===true&&settled?.active===true&&paused?.elapsedTicks===settled?.elapsedTicks&&
    paused?.visibilityState==='hidden'&&settled?.visibilityState==='hidden';
  const continued=resumed?.active===true&&resumed?.elapsedTicks>settled?.elapsedTicks&&resumed?.visibilityState==='visible';
  const synthResumed=bridgeBefore==='running'&&bridgePaused==='suspended'&&bridgeResumed==='running';
  const nativeContinued=samePath&&held&&continued&&synthResumed;
  const outputObserved=isVerifiedRuntimePcmWindow(resumedOutput?.pcm,TH08_MIDI_PCM_ROLE)&&resumedOutput.pcm.outputObserved===true;
  const passed=nativeContinued&&outputObserved;
  return {id:'th08-midi-background-sequencer-continuity',status:passed?'passed':nativeContinued?'incomplete':'failed',passed,complete:false,
    nativeContinued,outputObserved,resumedOutput:resumedOutput??null,
    sameSource:samePath,pausedTicks:paused?.elapsedTicks??null,settledTicks:settled?.elapsedTicks??null,
    resumedTicks:resumed?.elapsedTicks??null,bridgeStates:[bridgeBefore??null,bridgePaused??null,bridgeResumed??null],
    scope:'Native MIDI elapsed ticks hold during suspension, then continue through the retained Launcher synth with actual output PCM'};
}

export function th08MidiCapabilityOutcome({capability,consumerInstalled,notesSent,pcmObserved}={}){
  const declared=capability?.midi===true;
  const path=consumerInstalled===true&&notesSent===true&&pcmObserved===true;
  const passed=declared&&path;
  return {id:'MIDI-01',status:passed?'partial':'failed',complete:false,passed,declared,path,
    scope:'Product capability declaration and actual Runtime event to Launcher TinySynth path; Launcher settings UI remains outside this fixture'};
}

export function th08MusicModeSeparationOutcome({ogg,midi,midiWithoutOgg,restoredOgg}={}){
  const midiOnly=!!midiWithoutOgg?.passed&&midiWithoutOgg?.oggResources===0;
  const oggRestored=!!restoredOgg?.passed&&restoredOgg?.oggResources>0;
  const separate=!!ogg?.passed&&!!midi?.passed&&midiOnly&&oggRestored;
  return {id:'MIDI-04',status:separate?'partial':'failed',complete:false,passed:separate,
    oggSelected:!!ogg?.passed,midiSelected:!!midi?.passed,
    midiWithoutOgg:midiOnly,oggRestored,restoredInSameRuntime:false,
    scope:'MIDI runs with no OGG bytes; a separate fresh Runtime configuration uses the verified OGG source. Mode selection is applied before launch.'};
}

/** Corrupt-resource observation binds the fault to the live OGG source, not a UI label. */
export function th08OggFaultOutcome({fault,native,pcm,diagnostics=[],expectedFile='th08_01.ogg',expectedPath=`/bgm-ogg/${expectedFile}`,requestStatus,configureError,
  injectedBytesSha256,originalBytesSha256,startup,configureResponse,errors=[]}={}){
  const missing=fault==='missing';
  const source=missing?true:typeof native?.name==='string'&&native.name.replace(/\.wav$/i,'.ogg')===expectedFile;
  const rejected=missing?requestStatus===404&&typeof configureError==='string'&&configureError.includes(expectedPath):
    native?.exists===true&&native?.pending===true&&native?.soundReady===false&&native?.playing===false;
  const visible=diagnostics.some(value=>typeof value==='string'&&value.includes(expectedPath))||
    missing&&typeof configureError==='string'&&configureError.includes(expectedPath);
  const outputVerified=isVerifiedRuntimePcmWindow(pcm,TH08_OGG_PCM_ROLE,{minimumWindows:8});
  const startupRejected=missing&&startup?.app===0&&startup.firstFrameCount===0&&startup.ready?.event==='ready'&&
    startup.ready.protocol==='eagler-touhou/1'&&startup.ready.game==='th08'&&Number.isSafeInteger(startup.ready.epoch)&&
    configureResponse?.ok===false&&typeof configureResponse.request==='string'&&configureResponse.epoch===startup.ready.epoch&&
    configureResponse.game==='th08'&&configureResponse.protocol==='eagler-touhou/1'&&configureResponse.error?.includes(expectedPath);
  const silent=!missing&&outputVerified&&pcm.outputObserved===false;
  const noFaultOutput=missing?startupRejected:silent;
  const faultIdentity=missing||typeof injectedBytesSha256==='string'&&typeof originalBytesSha256==='string'&&
    injectedBytesSha256!==originalBytesSha256;
  const passed=source&&rejected&&visible&&noFaultOutput&&faultIdentity&&errors.length===0;
  const evidenced=source&&rejected&&noFaultOutput&&faultIdentity;
  return {id:`th08-ogg-${fault}-resource-diagnostic`,status:passed?'partial':evidenced?'failed':'incomplete',passed,complete:false,fault,expectedFile,
    expectedPath,sourceRejected:rejected,silent,outputVerified,startupRejected,diagnosticVisible:visible,diagnostics,requestStatus:requestStatus??null,
    configureError:configureError??null,injectedBytesSha256:injectedBytesSha256??null,originalBytesSha256:originalBytesSha256??null,
    scope:'Controlled OGG source fault is rejected and a user-facing resource diagnostic identifies the affected file'};
}

/** Mode recovery uses a new configured Runtime; the running Runtime has no mode-switch command. */
export function th08MusicFaultRecoveryOutcome({fault,recovery,errors=[]}={}){
  const scene=recovery?.scene;
  const output=th08MidiTitleOutcome({native:scene?.native,events:scene?.events,sendCorrelation:scene?.sendCorrelation,
    pcm:scene?.pcm,contextState:scene?.contextState,errors});
  const selected=recovery?.selectedMode==='midi'&&recovery.observedMode==='midi'&&recovery.capability?.midi===true&&
    recovery.configureResponse?.ok===true&&recovery.configureResponse.game==='th08'&&
    recovery.configureResponse.protocol==='eagler-touhou/1'&&recovery.configureResponse.epoch===recovery.after?.epoch&&recovery.runtimeResources?.length===0;
  const old=recovery?.before,next=recovery?.after;
  const fresh=Number.isSafeInteger(old?.epoch)&&Number.isSafeInteger(next?.epoch)&&next.epoch>old.epoch&&
    old.href!==next.href&&recovery.oldFrameDetached===true;
  const first=recovery?.firstFrame;
  const launched=first?.protocol==='eagler-touhou/1'&&first.game==='th08'&&first.event==='first-frame'&&first.epoch===next?.epoch;
  const retired=fault?.fault==='missing'||recovery?.exit?.protocol==='eagler-touhou/1'&&recovery.exit.game==='th08'&&
    recovery.exit.epoch===old?.epoch&&recovery.exit.event==='exit'&&recovery.exit.code===0&&recovery.exit.status==='success';
  const diagnosed=fault?.passed===true&&fault.diagnosticVisible===true;
  const audioReady=recovery?.audioReady?.state==='running'&&Number.isFinite(recovery.audioReady.currentTime);
  const passed=diagnosed&&selected&&fresh&&launched&&retired&&audioReady&&output.passed===true&&errors.length===0;
  return{id:'th08-music-fault-mode-recovery',passed,status:passed?'partial':errors.length||scene&&output.status==='failed'?'failed':'incomplete',complete:false,
    diagnosed,selected,fresh,launched,retired,audioReady,output,
    scope:'Runtime resource diagnostic followed by public configure MIDI selection in a fresh epoch and verified MIDI output; Launcher settings UI is separate'};
}

export function th08MusicFaultJourneyOutcome({corrupt,missing,corruptRecovery,missingRecovery}={}){
  const parts=[corrupt,missing,corruptRecovery,missingRecovery],passed=parts.every(part=>part?.passed===true);
  return{id:'AUDIO-05',passed,status:passed?'partial':parts.some(part=>part?.status==='failed')?'failed':'incomplete',complete:false,
    corrupt,missing,corruptRecovery,missingRecovery,
    scope:'Missing and corrupt OGG diagnostics with fresh-epoch MIDI recovery through the production Runtime protocol; Launcher preference UI remains separate'};
}

/** Keep a controlled OGG mode's behavior verdict separate from its partial AUDIO-05 coverage assessment. */
export function attachTh08OggFaultModeAssessment(mode,assessment){
  const passed=assessment?.passed===true;
  const status=passed?'passed':assessment?.status==='failed'?'failed':'incomplete';
  return{...mode,status,passed,faultAssessment:assessment??null};
}
