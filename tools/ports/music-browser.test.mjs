import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {runInNewContext} from 'node:vm';
import {mkdtemp,mkdir,readFile,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve,join,relative} from 'node:path';
import {createRuntimeFixtureSite} from './runtime-fixture-site.mjs';
import {parseTh08MusicCatalog,parseTh08MessageMusicCues} from '../../tests/adaptation/adapters/titles/th08-music.mjs';
import {selectMusicChecks,musicModesForChecks,th08MusicRoomOutcome,mergeMusicBehaviorResult,isVerifiedRuntimePcmWindow,th08OggDecodeVariantsOutcome,th08MusicStopDrainFrames} from './music-browser-result.mjs';
import {runtimePcm as fixtureRuntimePcm} from './fixtures/music-pcm-window.mjs';
import {assessDriverReport} from '../../tests/adaptation/results.mjs';
import {attachMusicStageErrors,attachTh08OggFaultModeAssessment,correlateTh08MidiSends,musicScenarioErrors,musicScenarioPassed,musicTitleSceneOutcome,th08BackgroundContinuityOutcome,th08MidiCapabilityOutcome,th08MidiPauseOutcome,
  th08MidiRestartOutcome,th08MidiTitleOutcome,th08MusicModeSeparationOutcome,th08OggAcceptanceOutcome,th08OggFaultOutcome,
  th08OggPauseOutcome,th08OggRestartOutcome,th08OggTitleOutcome,th08OggTrackFile,th08MusicFaultRecoveryOutcome,th08MusicFaultJourneyOutcome,isExpectedMissingMusicError} from './music-browser-result.mjs';
import {describeNativeMusic,describeNoneMusicControl,installMusicProtocolCapture,mapTrackToInput,nativeAudioStatsShape,runtimeMusicResources,
  decodeTh08MidiObservation,decodeTh08MusicObservation,readNativeAudioState,selectedMusicMappingGames,summarizeMusicPcm,
  validateMusicManifest,verifyMusicInputs,TH08_OGG_PCM_ROLE,TH08_MIDI_PCM_ROLE,TH08_MIDI_VOICE_PCM_ROLE,verifyRuntimeAudioClosedReceipt,pressConsumedRuntimeKey} from './music-browser-helpers.mjs';

const root=resolve(import.meta.dirname,'../..');
const runtimePcm=(role,options={})=>fixtureRuntimePcm(role,{startTime:1,...options});
test('music selection preserves explicit requirements and rejects empty, duplicate and foreign checks',()=>{
  assert.equal(selectMusicChecks('th08').length,8);
  assert.deepEqual(selectMusicChecks('th08','AUDIO-01,MIDI-02'),['AUDIO-01','MIDI-02']);
  for(const value of ['',',','MIDI-02,MIDI-02','INPUT-01'])assert.throws(()=>selectMusicChecks('th08',value));
  assert.throws(()=>selectMusicChecks('th11','MIDI-02'));
});
test('numbered music reruns execute only their source modes and retain the full default matrix',()=>{
  assert.deepEqual(musicModesForChecks('th08',['AUDIO-01']),['ogg','ogg-full']);
  assert.deepEqual(musicModesForChecks('th08',['MIDI-02']),['midi']);
  assert.deepEqual(musicModesForChecks('th08',['MIDI-04']),['ogg','ogg-full','midi']);
  assert.deepEqual(musicModesForChecks('th08',['AUDIO-05']),['corrupt-ogg','missing-ogg']);
  assert.deepEqual(musicModesForChecks('th08',selectMusicChecks('th08'),{baseline:true}),['none','ogg','ogg-full','midi','corrupt-ogg','missing-ogg']);
});

test('menu input waits for slow native consumption and release instead of a short wall-clock pulse',async()=>{
  let requested=0,current=0,previous=0,polls=0,edges=0;
  const writes=[];
  const input={code:'KeyZ',mask:1,maxPolls:12,readInput:async()=>({available:true,inputCurrent:current,inputPrevious:previous}),
    setKey:async(code,down)=>{writes.push(down);requested=down?1:0;},
    wait:async()=>{if(++polls%4===0){previous=current;current=requested;if(current&&!previous)edges++;}}};
  const result=await pressConsumedRuntimeKey(input);
  assert.deepEqual(writes,[true,false]);assert.equal(edges,1);
  assert.equal(current,0);assert.equal(previous,0);
  assert.ok(result.observations.find(row=>row.phase==='pressed').polls>2);
  assert.ok(result.observations.find(row=>row.phase==='released').polls>4);
  const stalledWrites=[];
  await assert.rejects(pressConsumedRuntimeKey({...input,maxPolls:3,
    readInput:async()=>({available:true,inputCurrent:0,inputPrevious:0}),wait:async()=>{},
    setKey:async(code,down)=>stalledWrites.push(down)}),/did not reach pressed/);
  assert.deepEqual(stalledWrites,[true,false],'a stalled consumer still releases the key');
});

test('OGG stream and full each require observed decoder policy, source PCM and every lifecycle',()=>{
  const record=decodeMode=>{
    const decoder={enabled:true,decodeMode};
    const scene=file=>{
      const pcm=runtimePcm(TH08_OGG_PCM_ROLE);
      return{native:{name:file.replace('.ogg','.wav'),exists:true,soundReady:true,pending:false,playing:true,outputPaused:false,cursorFrames:100,lastMusicSong:1},
        decoder:{...decoder},trackFile:file,input:{name:file,sha256:'a'.repeat(64)},pcm,pcmCollection:observedTrackPcmCollection(pcm,file)};
    };
    const title=scene('th08_01.ogg'),audio=scene('th08_00.ogg'),before=audio.native;
    const paused={...before,playing:false,cursorFrames:120},resumed={...before,cursorFrames:150};
    const lifecycle=(count,observationInput)=>({passed:true,observationInput:structuredClone(observationInput),decoderSamples:Array.from({length:count},()=>({...decoder}))});
    return{title,start:{audio,
      oggPause:lifecycle(4,{before,paused,settled:{...paused},resumed,pauseEntered:true,resumeExited:true}),
      background:lifecycle(5,{mode:'ogg',before,paused:{...before,outputPaused:true,visibilityState:'hidden'},
        settled:{...before,outputPaused:true,visibilityState:'hidden'},resumed:{...resumed,visibilityState:'visible'},resumedOutput:audio}),
      restart:lifecycle(3,{before:audio,quietWindow:quietWindow(TH08_OGG_PCM_ROLE),reopened:title,
        expectedBeforeFile:'th08_00.ogg',expectedFile:'th08_01.ogg',sameRuntimeFrame:true,exitObserved:true})}};
  };
  const stream=record(0),full=record(1);
  assert.equal(th08OggDecodeVariantsOutcome({stream,full}).passed,true);
  assert.equal(th08OggDecodeVariantsOutcome({stream}).passed,false,'full must really execute');
  assert.equal(th08OggDecodeVariantsOutcome({full}).passed,false,'stream must really execute');
  for(const mutate of [r=>r.title.decoder.decodeMode=0,r=>r.start.audio.decoder.decodeMode=0,
    r=>r.start.oggPause.decoderSamples[2].decodeMode=0,r=>r.start.background.decoderSamples[3].decodeMode=0,
    r=>r.start.restart.decoderSamples[2].decodeMode=0,r=>r.start.restart.decoderSamples.pop(),
    r=>delete r.start.oggPause.observationInput,r=>delete r.start.background.observationInput,r=>delete r.start.restart.observationInput,
    r=>r.start.oggPause.observationInput.resumed.cursorFrames=100,
    r=>r.start.background.observationInput.resumedOutput.pcm=runtimePcm(TH08_OGG_PCM_ROLE,{nonZeroPerWindow:0}),
    r=>delete r.start.background.observationInput.resumedOutput.pcm,
    r=>r.start.restart.observationInput.quietWindow=quietWindow(TH08_OGG_PCM_ROLE,{nonZeroPerWindow:1}),
    r=>r.start.restart.observationInput.reopened.pcm=runtimePcm(TH08_OGG_PCM_ROLE,{nonZeroPerWindow:0}),
    r=>r.start.audio.native.name='th08_02.wav',r=>r.title.pcm=runtimePcm(TH08_OGG_PCM_ROLE,{nonZeroPerWindow:0})]){
    const changed=structuredClone(full);mutate(changed);assert.equal(th08OggDecodeVariantsOutcome({stream,full:changed}).passed,false);
  }
  const staleSummary=structuredClone(full);
  for(const key of ['oggPause','background','restart'])staleSummary.start[key].passed=false;
  assert.equal(th08OggDecodeVariantsOutcome({stream,full:staleSummary}).passed,true,'raw evidence, rather than cached summaries, decides the variant');
  assert.equal(th08OggDecodeVariantsOutcome({stream,full,errors:[{mode:'ogg-full',channel:'console'}]}).passed,false);
});

test('passing Launcher scenes retain earlier Runtime failures and incomplete evidence',()=>{
  for(const previous of [{status:'failed',passed:false},{status:'incomplete',passed:false}]){
    assert.deepEqual(mergeMusicBehaviorResult(previous,{status:'partial',passed:true}),previous);
    assert.equal(mergeMusicBehaviorResult({status:'failed',passed:false},{status:'incomplete',passed:false}).status,'failed');
  }
  assert.equal(mergeMusicBehaviorResult({status:'passed',passed:true},{status:'failed',passed:false}).status,'failed');
  assert.equal(mergeMusicBehaviorResult({status:'partial',passed:true},{status:'passed',passed:true}).status,'partial');
});

test('PCM evidence needs real sample counts and advancing capture clocks, not summary counters',()=>{
  for(const role of [TH08_OGG_PCM_ROLE,TH08_MIDI_PCM_ROLE]){
    const baseline=runtimePcm(role,{windowCount:8});assert.equal(isVerifiedRuntimePcmWindow(baseline,role,{minimumWindows:8}),true);
    for(const mutate of [p=>p.windows[2].taps[0].samples=0,p=>p.windows[2].taps[0].samples=1,
      p=>p.windows[2].taps[0].currentTime=p.windows[1].taps[0].currentTime,p=>p.elapsedFrames+=480,
      p=>delete p.windows[2].taps[0].samples,p=>p.firstCurrentTime+=1,p=>p.lastCurrentTime+=1]){
      const changed=structuredClone(baseline);mutate(changed);assert.equal(isVerifiedRuntimePcmWindow(changed,role,{minimumWindows:8}),false);
    }
  }
});

test('independent th08 music tables join every track and reject missing or malformed formats',()=>{
  const comments=Buffer.from(Array.from({length:21},(_,index)=>`@bgm/th08_${String(index).padStart(2,'0')}.mid\r\nTitle ${index}\r\nDescription`).join('\r\n'));
  const format=Buffer.alloc(21*52+1);
  for(let index=0;index<21;index++){
    const p=index*52;format.write(`th08_${String(index).padStart(2,'0')}.wav`,p,'ascii');
    format.writeUInt32LE(4,p+24);format.writeUInt32LE(400,p+28);
    format.writeUInt16LE(1,p+32);format.writeUInt16LE(2,p+34);format.writeUInt32LE(44100,p+36);
    format.writeUInt16LE(4,p+44);format.writeUInt16LE(16,p+46);
  }
  assert.equal(parseTh08MusicCatalog(comments,format)[20].totalFrames,100);
  assert.throws(()=>parseTh08MusicCatalog(comments,format.subarray(0,20*52)));
  const bad=Buffer.from(format);bad.writeUInt32LE(401,28);assert.throws(()=>parseTh08MusicCatalog(comments,bad));
  assert.throws(()=>parseTh08MusicCatalog(Buffer.from('@bgm/th08_00.mid\nTitle'),format));
});

test('Boss music expectation comes from independent message instructions and StageHeader slots',()=>{
  const message=Buffer.alloc(16),stage=Buffer.alloc(0x490);
  message.writeUInt32LE(1,0);message.writeUInt32LE(8,4);message.writeUInt16LE(136,8);message[10]=7;message[11]=4;message.writeInt32LE(1,12);
  stage.write('bgm/th08_00.mid',0x290);stage.write('bgm/th08_03.mid',0x310);
  const parsed=parseTh08MessageMusicCues(message,stage,0);
  assert.deepEqual(parsed.cues,[{entry:0,offset:8,time:136,slot:1,path:'bgm/th08_03.mid',file:'th08_03.ogg'}]);
  assert.equal(parsed.stage.paths[0],'bgm/th08_00.mid');assert.equal(parsed.message.entryCount,1);
  for(const mutate of [b=>b.writeUInt32LE(9999,0),b=>b.writeUInt32LE(16,4),b=>b[11]=255,b=>b.writeInt32LE(3,12)]){
    const changed=Buffer.from(message);mutate(changed);assert.throws(()=>parseTh08MessageMusicCues(changed,stage,0));
  }
  assert.throws(()=>parseTh08MessageMusicCues(message,stage.subarray(0,0x48f),0));
});

test('Music Room requires all native sources, a natural timed wrap, gain fade, quiet PCM and keyboard reset',()=>{
  const catalog=Array.from({length:21},(_,index)=>({index,file:`th08_${String(index).padStart(2,'0')}.ogg`,
    input:{name:`th08_${String(index).padStart(2,'0')}.ogg`,sha256:'a'.repeat(64),bytes:100},introFrames:100,totalFrames:1000,sampleRate:100}));
  const scene=track=>{const pcm=runtimePcm(TH08_OGG_PCM_ROLE);return{index:track.index,native:{name:track.file.replace('.ogg','.wav'),exists:true,soundReady:true,pending:false,playing:true,cursorFrames:10,
    totalFrames:track.totalFrames,introFrames:track.introFrames,sampleRate:track.sampleRate},
    observation:{status:{supervisorActive:8}},input:{...track.input},pcm,pcmCollection:observedTrackPcmCollection(pcm,track.file)};};
  const native=cursor=>({name:'th08_00.wav',playing:true,totalFrames:1000,introFrames:100,sampleRate:100,cursorFrames:cursor});
  const quiet={pcm:runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:8,nonZeroPerWindow:0})};
  for(const window of quiet.pcm.windows){window.target.currentTime+=1;for(const tap of window.taps)tap.currentTime+=1;}
  quiet.pcm.firstCurrentTime+=1;quiet.pcm.lastCurrentTime+=1;
  quiet.drainBasis={native:{playing:false,outputPaused:false,pending:false,queuedFrames:3072},
    target:{...quiet.pcm.windows[0].target,tapIds:[0],currentTime:0.7},outputBufferFrames:2048,analysisWindowFrames:2048,baseLatency:0.01};
  quiet.pcmCollection={settle:{status:'complete',requiredFrames:th08MusicStopDrainFrames(quiet.drainBasis),
    startedAtTime:0.7,completedAtTime:2,elapsedFrames:62400}};
  const journey={entered:{status:{supervisorActive:8}},exited:{status:{supervisorActive:1}},tracks:catalog.map(scene),
    loop:{index:0,before:scene(catalog[0]),after:scene(catalog[0]),samples:[{...native(10),at:0,audioTime:1},{...native(980),at:9700,audioTime:10.7},{...native(110),at:10100,audioTime:11.1}]},
    fade:{samples:Array.from({length:9},(_,index)=>({at:index*1000,native:{name:'th08_00.wav',playing:index<8,gain:{name:'th08_00.wav',sourceAvailable:true,
      value:Math.pow(10,-index*625/2000),volume:-index*625,playing:index<8}}})),quiet,reset:scene(catalog[0])}};
  assert.equal(th08MusicRoomOutcome({catalog,journey}).passed,true);
  const delayedCatalog=catalog.map(track=>({...track,introFrames:300})),delayed=structuredClone(journey);
  for(const track of delayed.tracks)track.native.introFrames=300;
  for(const scene of [delayed.loop.before,delayed.loop.after,delayed.fade.reset])scene.native.introFrames=300;
  delayed.loop.samples=[{...native(200),introFrames:300,at:0,audioTime:1},
    {...native(980),introFrames:300,at:7800,audioTime:8.8},{...native(310),introFrames:300,at:8100,audioTime:9.1}];
  assert.equal(th08MusicRoomOutcome({catalog:delayedCatalog,journey:delayed}).loop,true,
    'output establishment can consume part of intro; natural-loop duration starts at the observed cursor');
  delayed.loop.samples.at(-1).at=3000;delayed.loop.samples.at(-1).audioTime=4;
  assert.equal(th08MusicRoomOutcome({catalog:delayedCatalog,journey:delayed}).loop,false,'an early cursor jump remains rejected');
  for(const mutate of [j=>j.tracks.pop(),j=>delete j.tracks[4].native,j=>j.loop.samples[2].at=8000,
    j=>j.loop.samples[2].cursorFrames=990,j=>delete j.loop.samples[1].audioTime,j=>j.loop.samples[2].audioTime=1,
    j=>j.fade.samples[3].native.gain.value=1,j=>j.fade.samples.forEach(s=>s.native.gain.value=1),j=>delete j.fade.samples[0].native.gain,
    j=>j.fade.quiet.pcm=runtimePcm(TH08_OGG_PCM_ROLE),j=>j.fade.reset.native.cursorFrames=600,
    j=>delete j.fade.quiet.drainBasis,j=>j.fade.quiet.drainBasis.native.playing=true,
    j=>j.fade.quiet.drainBasis.outputBufferFrames=null,j=>j.fade.quiet.pcmCollection.settle.requiredFrames=4096,
    j=>j.fade.quiet.pcmCollection.settle.completedAtTime=0.8,j=>j.fade.quiet.drainBasis.target.contextIndex=1,
    j=>j.fade.quiet.drainBasis.target.tapIds=[1],j=>j.fade.quiet.drainBasis.target.role='other',
    j=>j.fade.quiet.drainBasis.target.sourceIdentity={nodeId:1,nodeType:'AudioNode',role:'other'},
    j=>{j.fade.quiet.drainBasis.analysisWindowFrames=4096;j.fade.quiet.pcmCollection.settle.requiredFrames=th08MusicStopDrainFrames(j.fade.quiet.drainBasis);},
    j=>{j.fade.quiet.pcm.firstCurrentTime=1.02;j.fade.quiet.pcmCollection.settle.completedAtTime=1.02;}]){
    const changed=structuredClone(journey);mutate(changed);assert.equal(th08MusicRoomOutcome({catalog,journey:changed}).passed,false);
  }
});
const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
function quietWindow(role,{windowCount=8,nonZeroPerWindow=0}={}){
  const pcmWindow=runtimePcm(role,{windowCount,nonZeroPerWindow});
  return {status:'complete',proof:'runtime-output-pcm-window',role,quiet:pcmWindow.outputObserved===false,pcmWindow};
}
function observedTrackPcmCollection(pcm,file){
  const identity=pcm.identity,target={role:identity.role,available:true,contextIndex:identity.contextIndex,tapIds:identity.tapIds,
    sampleRate:identity.sampleRate,state:'running',currentTime:pcm.lastCurrentTime,stateChanges:[],stateChangeCount:0,stateChangeDrops:0};
  const buffering={queuedFrames:3072,queuedSampleRate:44100,outputBufferFrames:2048,analysisWindowFrames:2048,baseLatency:0.01};
  const firstTime=pcm.firstCurrentTime-.25,basis={...buffering,target:{...target,currentTime:firstTime}};
  const sample=(time,attempt)=>({attempt,trackFile:file,native:{name:file.replace('.ogg','.wav'),exists:true,soundReady:true,pending:false,
    playing:true,outputPaused:false,queuedFrames:3072,cursorFrames:Math.floor(time*44100)},target:{...target,currentTime:time},buffering:{...buffering}});
  const sourceBoundary={file,basis,firstObservedAtAttempt:1,settle:{status:'complete',requiredFrames:7920,startedAtTime:firstTime,
    completedAtTime:pcm.firstCurrentTime,elapsedFrames:(pcm.firstCurrentTime-firstTime)*48000},
    samples:[sample(firstTime,1),...pcm.windows.map((window,index)=>sample(window.target.currentTime,index+2))]};
  return {status:'observed',reason:null,expectedFile:file,observedFile:file,nativeReady:true,attempts:pcm.windowCount+1,sourceBoundary,
    maxSamples:32,pollIntervalMs:80,pcmObservedAtAttempt:pcm.windowCount,windowCount:pcm.windowCount,captureCount:pcm.captureCount,
    firstCurrentTime:pcm.firstCurrentTime,lastCurrentTime:pcm.lastCurrentTime,elapsedFrames:pcm.elapsedFrames,
    sampleRate:pcm.sampleRate,outputObserved:true,nonZeroSamples:pcm.nonZeroSamples,target};
}

test('native title maps are loaded only for games selected in this sparse checkout run',()=>{
  assert.deepEqual(selectedMusicMappingGames(['th08']),[]);
  assert.deepEqual(selectedMusicMappingGames(['th11']),['th11']);
  assert.deepEqual(selectedMusicMappingGames(['th15','th08']),['th15']);
});

test('OGG source acceptance reopens buffering and raw boundary samples instead of trusting a passed receipt',()=>{
  const pcm=runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:4});
  const scene={native:{name:'th08_01.wav',exists:true,soundReady:true,pending:false,playing:true},
    input:{name:'th08_01.ogg',sha256:'a'.repeat(64)},pcm,pcmCollection:observedTrackPcmCollection(pcm,'th08_01.ogg')};
  assert.equal(th08OggTitleOutcome(scene).passed,true);
  for(const mutate of [
    s=>delete s.pcmCollection.sourceBoundary,
    s=>s.pcmCollection.sourceBoundary.settle.requiredFrames=4096,
    s=>s.pcmCollection.sourceBoundary.settle.completedAtTime-=.01,
    s=>s.pcmCollection.sourceBoundary.basis.queuedFrames=0,
    s=>s.pcmCollection.sourceBoundary.basis.target.stateChangeCount=999,
    s=>s.pcmCollection.sourceBoundary.basis.analysisWindowFrames=4096,
    s=>s.pcmCollection.sourceBoundary.samples[0].native.queuedFrames=0,
    s=>s.pcmCollection.sourceBoundary.samples[0].native.name='th08_00.wav',
    s=>s.pcmCollection.sourceBoundary.samples[1].native.playing=false,
    s=>s.pcmCollection.sourceBoundary.samples[1].target.currentTime=.8,
    s=>s.pcmCollection.sourceBoundary.samples[1].target.tapIds=[1],
    s=>s.pcmCollection.sourceBoundary.samples[1].target.state='suspended',
    s=>s.pcmCollection.sourceBoundary.samples[1].buffering.outputBufferFrames=4096,
    s=>s.pcmCollection.sourceBoundary.samples[1].attempt+=1,
    s=>s.pcmCollection.sourceBoundary.samples.splice(1,1),
    s=>s.pcmCollection.sourceBoundary.samples.forEach(sample=>sample.native.cursorFrames=0),
  ]){
    const changed=structuredClone(scene);mutate(changed);
    assert.equal(th08OggTitleOutcome(changed).passed,false,'a cached observed result cannot cover missing or mismatched source evidence');
  }
});

test('music input manifests bind a complete OGG inventory to catalog paths and hashes',async t=>{
  const temp=await mkdtemp(join(tmpdir(),'eagler-music-contract-'));t.after(()=>rm(temp,{recursive:true,force:true}));
  const sourceDirectory=resolve(temp,'external-inputs/games/web-content/th08/bgm-ogg');await mkdir(sourceDirectory,{recursive:true});
  const contents=Buffer.from('verified OGG fixture bytes');
  const path=resolve(sourceDirectory,'th08_00.ogg');await writeFile(path,contents);
  const catalog={th08:{package:{musicSourceDirectories:{ogg:'bgm-ogg'},musicMounts:{ogg:'/bgm-ogg'}}}};
  const manifest={schema:'eagler-touhou/music-inputs/1',resourcePolicy:'private-local-paths',games:{th08:{
    sourceDirectory,mount:'/bgm-ogg',files:[{name:'th08_00.ogg',path,bytes:contents.length,sha256:sha256(contents)}]}}};
  const worktreeRoot=resolve(temp,'separate-worktree');
  assert.ok(relative(worktreeRoot,sourceDirectory).startsWith('..'),'fixture music input must be outside the worktree');
  assert.deepEqual(validateMusicManifest(manifest,{repoRoot:worktreeRoot,games:['th08'],catalog}).th08.files,manifest.games.th08.files);
  const identities=await verifyMusicInputs(manifest,{repoRoot:worktreeRoot,games:['th08'],catalog});
  assert.deepEqual(identities.th08,{sourceDirectory,mount:'/bgm-ogg',files:[{name:'th08_00.ogg',path,bytes:contents.length,sha256:sha256(contents)}]});
  assert.deepEqual(runtimeMusicResources('th08',identities,{base:'http://127.0.0.1:8143'}),[
    {path:'/bgm-ogg/th08_00.ogg',url:'http://127.0.0.1:8143/music/th08/th08_00.ogg',bytes:contents.length}]);
  await writeFile(path,'changed OGG input');
  await assert.rejects(()=>verifyMusicInputs(manifest,{repoRoot:worktreeRoot,games:['th08'],catalog}),/byte count changed|SHA-256 changed/);
});

test('same OGG basename is served from separate per-game routes without byte replacement',async t=>{
  const temp=await mkdtemp(join(tmpdir(),'eagler-music-routes-'));t.after(()=>rm(temp,{recursive:true,force:true}));
  const th15Path=resolve(temp,'th15.ogg'),th20Path=resolve(temp,'th20.ogg');
  const th15Bytes=Buffer.from('th15 track bytes'),th20Bytes=Buffer.from('th20 track bytes');
  await Promise.all([writeFile(th15Path,th15Bytes),writeFile(th20Path,th20Bytes)]);
  const fixture=await createRuntimeFixtureSite({repoRoot:root,targets:[{id:'th15'},{id:'th20'}],files:new Map([
    ['/music/th15/th128_08.ogg',th15Path],['/music/th20/th128_08.ogg',th20Path],
  ])});
  t.after(()=>fixture.close());
  const identity=(path,bytes)=>({th15:{mount:'/music',files:[{name:'th128_08.ogg',path,bytes:bytes.length,sha256:sha256(bytes)}]},
    th20:{mount:'/music',files:[{name:'th128_08.ogg',path,bytes:bytes.length,sha256:sha256(bytes)}]}});
  const th15Resource=runtimeMusicResources('th15',identity(th15Path,th15Bytes),{base:fixture.base})[0];
  const th20Resource=runtimeMusicResources('th20',identity(th20Path,th20Bytes),{base:fixture.base})[0];
  assert.notEqual(th15Resource.url,th20Resource.url);
  for(const [resource,expected] of [[th15Resource,th15Bytes],[th20Resource,th20Bytes]]){
    const response=await fetch(resource.url);assert.equal(response.status,200);
    assert.deepEqual(Buffer.from(await response.arrayBuffer()),expected);
  }
  assert.equal((await fetch(`${fixture.base}/shared/th128_08.ogg`)).status,404);
});

test('native track descriptions honor exact TH11 and TH15 statistics widths',()=>{
  const th11=describeNativeMusic('th11',{native:{source:'th11_audio_statistics',words:[1,2,3,4,5,2,7,8,0]}},
    {th11Tracks:{'2':'th11_02'}});
  assert.deepEqual({wordCount:th11.wordCount,trackIndex:th11.trackIndex,trackFile:th11.trackFile,cursor:th11.cursor},
    {wordCount:9,trackIndex:2,trackFile:'th11_02.ogg',cursor:null});
  assert.equal(th11.paused,false);
  assert.throws(()=>describeNativeMusic('th11',{native:{source:'th11_audio_statistics',words:Array(11).fill(0)}}),/exactly nine words/);
  const th15=describeNativeMusic('th15',{native:{source:'th15_audio_statistics',words:[1,2,3,4,5,1,7,8,0,901,11]}},
    {th15Tracks:['th15_01.ogg','th15_02.ogg']});
  assert.deepEqual({wordCount:th15.wordCount,trackIndex:th15.trackIndex,trackFile:th15.trackFile,cursor:th15.cursor},
    {wordCount:11,trackIndex:1,trackFile:'th15_02.ogg',cursor:901});
  assert.equal(nativeAudioStatsShape('th11').cursor,null);
  assert.equal(nativeAudioStatsShape('th15').cursor,9);
});

test('native audio reader resolves window.core as a supported Runtime module shape',t=>{
  const previous=globalThis.window,buffer=new ArrayBuffer(128),memory=new DataView(buffer);
  memory.setFloat32(16+40,.125,true);
  globalThis.window={__th20Runtime:{},core:{memory:{buffer},
    sdl_audio_stats:()=>16,sdl_music_stats:()=>64}};
  t.after(()=>{if(previous===undefined)delete globalThis.window;else globalThis.window=previous;});
  const state=readNativeAudioState('th20');
  assert.equal(state.native.words.length,12);
  assert.equal(state.native.rms,.125);
  assert.equal(state.decoder.words.length,6);
});

test('TH08 native observations decode independent fixed OGG cursor and MIDI sequencer samples',()=>{
  const music=decodeTh08MusicObservation([1,1,1,1,0,1,1,0,1200,0,14400000,4410,44100,2048,3072,0],'th08_01.wav');
  assert.deepEqual({abiVersion:music.abiVersion,exists:music.exists,paused:music.outputPaused,ready:music.soundReady,
    pending:music.pending,requested:music.requested,loop:music.loop,playing:music.playing,cursor:music.cursorFrames,
    sampleRate:music.sampleRate,name:music.name},
  {abiVersion:1,exists:true,paused:true,ready:true,pending:false,requested:true,loop:true,playing:false,cursor:1200,
    sampleRate:44100,name:'th08_01.wav'});
  const midi=decodeTh08MidiObservation([1,1,0,8,500000,480,2,2,300,0,1,40,400,50,20,18,30,0,1,0,300,1],'bgm/th08_01.mid');
  assert.deepEqual({active:midi.active,invalid:midi.invalid,fileIndex:midi.fileIndex,trackCount:midi.trackCount,
    activeTrackCount:midi.activeTrackCount,elapsedTicks:midi.elapsedTicks,fading:midi.fading,noteOns:midi.noteOnCount,
    noteOffs:midi.noteOffCount,path:midi.path,lastMusicSong:midi.lastMusicSong},
  {active:true,invalid:false,fileIndex:8,trackCount:2,activeTrackCount:2,elapsedTicks:300,fading:1,noteOns:20,
    noteOffs:18,path:'bgm/th08_01.mid',lastMusicSong:1});
  assert.throws(()=>decodeTh08MusicObservation([1,1,1],null),/exactly 16 words/);
  assert.throws(()=>decodeTh08MidiObservation(Array(21).fill(0),null),/exactly 22 words/);
});

test('TH08 native reader copies exported observations and source paths from the real module memory',t=>{
  const previous=globalThis.window,buffer=new ArrayBuffer(1024),memory=new DataView(buffer);
  const putWords=(pointer,values)=>values.forEach((value,index)=>memory.setUint32(pointer+index*4,value>>>0,true));
  putWords(64,[1,1,0,1,0,1,1,1,900,0,14400000,4410,44100,3072,6000,0]);
  putWords(160,[1,1,0,8,500000,480,2,2,300,0,0,0,0,50,20,18,30,0,1,0,300,1]);
  new Uint8Array(buffer,320,17).set([...Buffer.from('th08_01.wav'),0]);
  new Uint8Array(buffer,384,256).set([...Buffer.from('bgm/th08_01.mid'),0]);
  globalThis.window={__th08Runtime:{},core:{memory:{buffer},sdl_audio_stats:()=>16,sdl_music_stats:()=>32,
    sdl_music_observation:()=>64,sdl_midi_observation:()=>160,sdl_music_name:()=>320,sdl_midi_path:()=>384}};
  t.after(()=>{if(previous===undefined)delete globalThis.window;else globalThis.window=previous;});
  const state=readNativeAudioState('th08');
  assert.equal(state.musicObservation.cursorFrames,900);assert.equal(state.musicObservation.name,'th08_01.wav');
  const gainBits=new DataView(new ArrayBuffer(4));gainBits.setFloat32(0,0.25,true);
  window.core.sdl_music_gain_observation_v1=field=>[1,1,gainBits.getUint32(0,true),(-1204)>>>0,1,1][field];
  const gained=readNativeAudioState('th08').musicObservation.gain;
  assert.equal(gained.value,.25);assert.equal(gained.volume,-1204);assert.equal(gained.name,'th08_01.wav');
  assert.equal(gained.sourceAvailable,true);assert.equal(gained.playing,true);
  window.core.sdl_music_gain_observation_v1=field=>field===0?2:0;
  assert.throws(()=>readNativeAudioState('th08'),/gain ABI/);
  delete window.core.sdl_music_gain_observation_v1;
  assert.equal(state.midiObservation.elapsedTicks,300);assert.equal(state.midiObservation.path,'bgm/th08_01.mid');
  assert.equal(state.midiObservation.lastMusicSong,1);assert.equal(nativeAudioStatsShape('th08').midiObservationWordCount,22);
  const context={state:'running'},events=[{bytes:[0x90,60,100],time:1},{bytes:[0x80,60,0],time:2}],sends=events.map(event=>event.bytes);
  memory.setUint32(160+13*4,2,true);
  globalThis.window.parent={__eaglerTinySynth:{getAudioContext:()=>context},
    __eaglerMusicPcmProbe:{contexts:[context],capture:()=>[{contextIndex:0,nonZero:8,contextState:'running'}]},
    __eaglerMidiNativeEvents:events,__eaglerTinySynthSends:sends};
  const together=readNativeAudioState({game:'th08',includeMidiHost:true});
  assert.equal(together.midiObservation.messageCount,together.midiHost.nativeEvents.length,
    'one browser evaluation binds native counts and the corresponding transport window');
  assert.equal(together.midiHost.contextIndex,0);
  assert.equal(together.midiHost.contextState,'running');
  assert.equal(correlateTh08MidiSends(together.midiHost.nativeEvents,together.midiHost.sentMessages).status,'matched');
  assert.equal(together.midiHost.captures[0].nonZero,8);
});

test('TH08, TH10 and TH20 stay partial when decoder readiness has no proven track ID',()=>{
  for(const game of ['th08','th10','th20']){
    const result=describeNativeMusic(game,{native:{source:'sdl_audio_stats',words:Array(12).fill(0),rms:.02},
      decoder:{source:'sdl_music_stats',words:Array(6).fill(1),fullReady:1}});
    assert.equal(result.identityStatus,'partial-no-proven-track-id');
    assert.equal(result.trackIndex,null);assert.equal(result.trackFile,null);
    assert.equal(nativeAudioStatsShape(game).trackIndex,null);
  }
});

test('PCM summary uses samples from the observed Runtime output tap',()=>{
  assert.deepEqual(summarizeMusicPcm([
    {contextState:'running',sampleRate:44100,peakAbs:.25,rms:.1,nonZero:25},
    {contextState:'running',sampleRate:44100,peakAbs:.5,rms:.2,nonZero:51},
  ]),{captureCount:2,contexts:['running'],contextIndices:[],sampleRate:44100,peakAbs:.5,maxRms:.2,nonZeroSamples:76,outputObserved:true});
  assert.equal(summarizeMusicPcm([]).outputObserved,false);
  assert.deepEqual(mapTrackToInput('th11_02.ogg',[{name:'th11_02.ogg',bytes:10,sha256:'a'.repeat(64)}]),
    {name:'th11_02.ogg',path:undefined,bytes:10,sha256:'a'.repeat(64)});
});

test('music=none control requires a running silent output window after settling',()=>{
  assert.deepEqual(describeNoneMusicControl({captureCount:6,contexts:['running'],nonZeroSamples:0,outputObserved:false}),
    {status:'silent-after-settling',scope:'Runtime output PCM across observed Runtime audio taps',settleMs:1800,captureWindowMs:600,samples:6,contextStates:['running']});
  assert.equal(describeNoneMusicControl({captureCount:6,contexts:['running'],nonZeroSamples:12,outputObserved:true}).status,
    'output-observed-unclassified');
  assert.equal(describeNoneMusicControl({captureCount:0,contexts:[],nonZeroSamples:0,outputObserved:false}).status,
    'no-running-output-tap');
});

test('required title OGG scene may pass while AUDIO-01 full coverage remains partial',()=>{
  const th11=musicTitleSceneOutcome({oggPcmObserved:true,expectedTrack:'th11_00.ogg',
    observedTrack:'th11_00.ogg',trackInput:{name:'th11_00.ogg',sha256:'a'.repeat(64)},errors:[]});
  assert.equal(th11.scenario.status,'passed');
  assert.equal(th11.coverage.status,'partial');assert.equal(th11.coverage.complete,false);
  const th08=musicTitleSceneOutcome({oggPcmObserved:true,errors:[]});
  assert.equal(th08.scenario.status,'passed');assert.equal(th08.trackIdentity,'not-exposed-by-native-stats');
  assert.equal(musicTitleSceneOutcome({oggPcmObserved:true,expectedTrack:'th11_00.ogg',
    observedTrack:'th11_01.ogg',trackInput:{name:'th11_01.ogg',sha256:'b'.repeat(64)},errors:[]}).scenario.status,'failed');
  assert.equal(musicTitleSceneOutcome({oggPcmObserved:true,errors:['HTTP 404']}).scenario.status,'failed');
});

test('TH08 music lifecycle gates require source identity, actual output and cursor continuity',()=>{
  const pcm=runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:4,nonZeroPerWindow:25});
  const source={exists:true,soundReady:true,pending:false,playing:true,name:'th08_01.wav',cursorFrames:1000};
  assert.equal(th08OggTitleOutcome({native:source,input:{name:'th08_01.ogg',sha256:'a'.repeat(64)},pcm,
    pcmCollection:observedTrackPcmCollection(pcm,'th08_01.ogg')}).passed,true);
  assert.equal(th08OggTitleOutcome({native:{...source,name:'th08_02.wav'},input:{name:'th08_01.ogg',sha256:'a'.repeat(64)},pcm}).passed,false);
  const paused={...source,playing:false,cursorFrames:1200},settled={...paused},resumed={...source,cursorFrames:1500};
  assert.equal(th08OggPauseOutcome({before:source,paused,settled,resumed,pauseEntered:true,resumeExited:true}).passed,true);
  assert.equal(th08OggPauseOutcome({before:source,paused,settled:{...settled,cursorFrames:1201},resumed,
    pauseEntered:true,resumeExited:true}).passed,false);
  const background={mode:'ogg',before:source,
    paused:{...source,outputPaused:true,visibilityState:'hidden'},
    settled:{...source,outputPaused:true,cursorFrames:1000,visibilityState:'hidden'},
    resumed:{...source,outputPaused:false,cursorFrames:1500,visibilityState:'visible'}};
  assert.equal(th08BackgroundContinuityOutcome(background).status,'incomplete','cursor progress alone is not output recovery');
  assert.equal(th08BackgroundContinuityOutcome({...background,resumedOutput:{pcm,
    pcmCollection:observedTrackPcmCollection(pcm,'th08_01.ogg')}}).passed,true);
  assert.equal(th08BackgroundContinuityOutcome({...background,resumedOutput:{pcm,
    pcmCollection:observedTrackPcmCollection(pcm,'th08_02.ogg')}}).passed,false,'PCM must still belong to the retained track');
  assert.equal(th08BackgroundContinuityOutcome({mode:'ogg',before:source,
    paused:{...source,outputPaused:true,visibilityState:'hidden'},
    settled:{...source,outputPaused:true,cursorFrames:1000,visibilityState:'hidden'},
    resumed:{...source,name:'th08_02.wav',outputPaused:false,cursorFrames:1500,visibilityState:'visible'}}).passed,false);
  assert.equal(th08OggTrackFile('bgm\\th08_03.wav'),'th08_03.ogg');
  const title=th08OggTitleOutcome({native:source,input:{name:'th08_01.ogg',sha256:'a'.repeat(64)},pcm,
    pcmCollection:observedTrackPcmCollection(pcm,'th08_01.ogg')});
  const stage=th08OggAcceptanceOutcome({title,expectedStageSong:1,stage:{native:{...source,name:'th08_00.wav',lastMusicSong:1},
    input:{name:'th08_00.ogg',sha256:'b'.repeat(64)},pcm,pcmCollection:observedTrackPcmCollection(pcm,'th08_00.ogg')}});
  assert.equal(stage.passed,true);assert.equal(stage.complete,false);
  assert.equal(th08OggAcceptanceOutcome({title,expectedStageSong:3,stage:{native:{...source,name:'th08_00.wav',lastMusicSong:1},
    input:{name:'th08_00.ogg',sha256:'b'.repeat(64)},pcm}}).passed,false);
});

test('TH08 OGG restart requires an actually silent close window and a new title source on the same Runtime frame',()=>{
  const beforePcm=runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:4});
  const reopenedPcm=runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:4});
  const result=th08OggRestartOutcome({
    before:{native:{exists:true,soundReady:true,pending:false,outputPaused:false,playing:true,name:'th08_00.wav'},trackFile:'th08_00.ogg',
      input:{name:'th08_00.ogg',sha256:'a'.repeat(64)},pcm:beforePcm,pcmCollection:observedTrackPcmCollection(beforePcm,'th08_00.ogg')},
    quietWindow:quietWindow(TH08_OGG_PCM_ROLE),
    reopened:{native:{exists:true,soundReady:true,pending:false,outputPaused:false,playing:true,name:'th08_01.wav'},trackFile:'th08_01.ogg',
      input:{name:'th08_01.ogg',sha256:'b'.repeat(64)},pcm:reopenedPcm,pcmCollection:observedTrackPcmCollection(reopenedPcm,'th08_01.ogg')},
    expectedBeforeFile:'th08_00.ogg',expectedFile:'th08_01.ogg',sameRuntimeFrame:true,exitObserved:true,
  });
  assert.equal(result.passed,true);
  assert.equal(result.beforeNativePlaying,true);
  assert.equal(result.beforeTrackIdentity,true);
  assert.equal(result.beforeOutputObserved,true);
  assert.equal(result.beforePcmCollectionValid,true);
  assert.equal(result.reopenedPcmCollectionValid,true);
  assert.equal(result.beforePcm.role,TH08_OGG_PCM_ROLE,'the complete active-track PCM window is retained for review');
  assert.equal(result.beforePcmCollection.target.role,TH08_OGG_PCM_ROLE);
  assert.equal(result.beforeTrack,'th08_00.ogg','the pre-close source is the first-stage track');
  assert.equal(result.reopenedTrack,'th08_01.ogg','the post-launch source is the title track');
  assert.equal(th08OggRestartOutcome({
    before:{native:{exists:true,soundReady:true,playing:true},trackFile:'th08_00.ogg',pcm:runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:4})},
    quietWindow:quietWindow(TH08_OGG_PCM_ROLE,{nonZeroPerWindow:1}),
    reopened:{native:{exists:true,soundReady:true,playing:true},trackFile:'th08_01.ogg',
      input:{name:'th08_01.ogg',sha256:'b'.repeat(64)},pcm:runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:4})},sameRuntimeFrame:true,exitObserved:true,
  }).passed,false,'old OGG samples after exit must fail the restart check');

  const activeBefore={native:{exists:true,soundReady:true,pending:false,outputPaused:false,playing:true,name:'th08_00.wav'},trackFile:'th08_00.ogg',
    input:{name:'th08_00.ogg',sha256:'a'.repeat(64)},pcm:runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:8,nonZeroPerWindow:0}),
    pcmCollection:{status:'bounded-no-output',reason:'No non-zero PCM was captured from the expected playing track in 32 bounded samples',
      expectedFile:'th08_00.ogg',observedFile:'th08_00.ogg',nativeReady:true,attempts:32,maxSamples:32,pollIntervalMs:80,
      pcmObservedAtAttempt:null,windowCount:8,captureCount:8,firstCurrentTime:2,lastCurrentTime:3,elapsedFrames:48000,
      sampleRate:48000,outputObserved:false,nonZeroSamples:0,target:null}};
  const insufficient=th08OggRestartOutcome({before:activeBefore,quietWindow:quietWindow(TH08_OGG_PCM_ROLE),
    reopened:{native:{exists:true,soundReady:true,pending:false,outputPaused:false,playing:true,name:'th08_01.wav'},trackFile:'th08_01.ogg',
      input:{name:'th08_01.ogg',sha256:'b'.repeat(64)},pcm:runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:4})},
    expectedBeforeFile:'th08_00.ogg',sameRuntimeFrame:true,exitObserved:true});
  assert.equal(insufficient.status,'incomplete','native playing with a bounded silent/insufficient sample window is not labeled as product failure');
  assert.equal(insufficient.beforeNativePlaying,true);
  assert.equal(insufficient.beforeTrackIdentity,true);
  assert.equal(insufficient.beforeOutputObserved,false);
  assert.match(insufficient.beforePcmReason,/No non-zero PCM was captured/);
  assert.equal(insufficient.beforePcmCollection.attempts,32);
  assert.equal(insufficient.beforePcmCollection.observedFile,'th08_00.ogg');
  assert.equal(insufficient.beforePcm.role,TH08_OGG_PCM_ROLE);
  const wrongBeforeTrack=th08OggRestartOutcome({before:{...activeBefore,pcmCollection:{...activeBefore.pcmCollection,status:'observed'},
    pcm:runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:8,nonZeroPerWindow:1})},
    quietWindow:quietWindow(TH08_OGG_PCM_ROLE),reopened:{native:{exists:true,soundReady:true,playing:true,name:'th08_01.wav'},trackFile:'th08_01.ogg',
      input:{name:'th08_01.ogg',sha256:'b'.repeat(64)},pcm:runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:4})},
    expectedBeforeFile:'th08_02.ogg',sameRuntimeFrame:true,exitObserved:true});
  assert.equal(wrongBeforeTrack.status,'failed','the stage source identity is independent from the title reopen expectation');

  const beforeTarget={role:TH08_OGG_PCM_ROLE,available:true,contextIndex:0,tapIds:[0],sampleRate:48000,state:'running',currentTime:1,
    stateChanges:[{sequence:1,state:'running',currentTime:0.5}],stateChangeCount:1,stateChangeDrops:0};
  const afterTarget={...beforeTarget,state:'closed',stateChanges:[...beforeTarget.stateChanges,{sequence:2,state:'closed',currentTime:1}],stateChangeCount:2};
  const receipt=verifyRuntimeAudioClosedReceipt(beforeTarget,afterTarget,{role:TH08_OGG_PCM_ROLE});
  assert.equal(receipt.status,'passed');
  const alteredHistory=verifyRuntimeAudioClosedReceipt(beforeTarget,{...afterTarget,
    stateChanges:[{sequence:1,state:'suspended',currentTime:0.5},{sequence:2,state:'closed',currentTime:1}]},{role:TH08_OGG_PCM_ROLE});
  assert.equal(alteredHistory.status,'incomplete','a close receipt must preserve the observed pre-close history');
  const closedWindow={status:'closed',proof:'closed-context-lifecycle',role:TH08_OGG_PCM_ROLE,quiet:true,target:afterTarget,lifecycleReceipt:receipt,
    pcmWindow:{status:'incomplete',captureCount:0,windowCount:0,outputObserved:null}};
  const closedBeforePcm=runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:4}),closedReopenedPcm=runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:4});
  const closedResult=th08OggRestartOutcome({
    before:{native:{exists:true,soundReady:true,pending:false,outputPaused:false,playing:true,name:'th08_00.wav'},trackFile:'th08_00.ogg',
      input:{name:'th08_00.ogg',sha256:'a'.repeat(64)},pcm:closedBeforePcm,pcmCollection:observedTrackPcmCollection(closedBeforePcm,'th08_00.ogg')},
    quietWindow:closedWindow,
    reopened:{native:{exists:true,soundReady:true,pending:false,outputPaused:false,playing:true,name:'th08_01.wav'},trackFile:'th08_01.ogg',
      input:{name:'th08_01.ogg',sha256:'b'.repeat(64)},pcm:closedReopenedPcm,pcmCollection:observedTrackPcmCollection(closedReopenedPcm,'th08_01.ogg')},
    sameRuntimeFrame:true,exitObserved:true,
  });
  assert.equal(closedResult.passed,true);
  assert.equal(closedResult.quietProof,'closed-context-lifecycle');
});

test('TH08 MIDI acceptance requires the title SMF, real messages and audible TinySynth PCM',()=>{
  const native={active:true,invalid:false,fileIndex:8,trackCount:2,sysexCount:0,messageCount:2,noteOnCount:3,path:'bgm/th08_01.mid'};
  const nativeEvents=[{bytes:[0x90,60,100],time:10},{bytes:[0x80,60,0],time:100}];
  const correlation=correlateTh08MidiSends(nativeEvents,[[0x90,60,100],[0x80,60,0]]);
  const events=correlation.events;
  const pcm=runtimePcm(TH08_MIDI_PCM_ROLE,{windowCount:3,nonZeroPerWindow:15});
  assert.equal(th08MidiTitleOutcome({native,events,sendCorrelation:correlation,pcm,contextState:'running'}).passed,true);
  const sysexBytes=[0xf0,0x7e,0x7f,0x09,0x01,0xf7];
  const withSysex=[...nativeEvents,{bytes:sysexBytes,time:200}];
  const sysexCorrelation=correlateTh08MidiSends(withSysex,[...nativeEvents.map(event=>event.bytes),sysexBytes]);
  const sysexInput={native:{...native,sysexCount:1},events:sysexCorrelation.events,
    sendCorrelation:sysexCorrelation,pcm,contextState:'running'};
  assert.equal(th08MidiTitleOutcome(sysexInput).passed,true,'short_message and long_message have independent native counters');
  assert.equal(th08MidiTitleOutcome({...sysexInput,native:{...native,messageCount:3,sysexCount:0}}).passed,false,
    'matching the total cannot hide a short-message/SysEx classification mismatch');
  assert.equal(th08MidiTitleOutcome({...sysexInput,native:{...native,sysexCount:undefined}}).passed,false);
  assert.equal(th08MidiTitleOutcome({native:{...native,path:'wrong.mid'},events,sendCorrelation:correlation,pcm,contextState:'running'}).passed,false);
  assert.equal(th08MidiTitleOutcome({native,events,sendCorrelation:correlation,pcm:{...pcm,outputObserved:false},contextState:'running'}).passed,false);
  assert.equal(th08MidiPauseOutcome({before:{active:true,path:'bgm/a.mid',elapsedTicks:10},
    paused:{active:true,path:'bgm/a.mid',elapsedTicks:30},resumed:{active:true,path:'bgm/a.mid',elapsedTicks:40},
    pauseEntered:true,resumeExited:true,bridgeContext:'running'}).passed,true);
  assert.equal(th08MidiPauseOutcome({before:{active:true,path:'bgm/a.mid',elapsedTicks:10},
    paused:{active:true,path:'bgm/a.mid',elapsedTicks:10},resumed:{active:true,path:'bgm/a.mid',elapsedTicks:40},
    pauseEntered:true,resumeExited:true,bridgeContext:'running'}).passed,false);
  assert.equal(th08MidiTitleOutcome({native,events:events.map(({bytes,time})=>({bytes,time})),sendCorrelation:correlation,pcm,contextState:'running'}).passed,false,
    'native bytes alone do not prove the Launcher consumer sent the messages');
  assert.equal(th08MidiTitleOutcome({native,events:[events[0]],sendCorrelation:correlation,pcm,contextState:'running'}).passed,false,
    'a forwarded Note On without a matching Note Off is not a complete observed title message window');
  assert.equal(th08MidiCapabilityOutcome({capability:{midi:true},consumerInstalled:true,notesSent:true,pcmObserved:true}).status,'partial');
  const separation=th08MusicModeSeparationOutcome({ogg:{passed:true},midi:{passed:true},
    midiWithoutOgg:{passed:true,oggResources:0},restoredOgg:{passed:true,oggResources:21}});
  assert.equal(separation.passed,true);assert.equal(separation.restoredInSameRuntime,false);
  assert.equal(th08MusicModeSeparationOutcome({ogg:{passed:true},midi:{passed:true},
    midiWithoutOgg:{passed:true,oggResources:1},restoredOgg:{passed:true,oggResources:21}}).passed,false);
});

test('th08 MIDI event-to-synth correlation is exact, ordered and all-or-nothing',()=>{
  const native=[{bytes:[0x90,60,100],time:10},{bytes:[0x80,60,0],time:100}];
  const sends=[[0x90,60,100],[0x80,60,0]];
  const matched=correlateTh08MidiSends(native,sends);
  assert.deepEqual({status:matched.status,eventCount:matched.eventCount,sendCount:matched.sendCount,matchedCount:matched.matchedCount},
    {status:'matched',eventCount:2,sendCount:2,matchedCount:2});
  assert.deepEqual(matched.events.map(event=>event.sentBytes),sends);
  for(const wrong of [
    [[0x80,60,0],[0x90,60,100]],
    [[0x90,60,99],[0x80,60,0]],
    [[0x90,60,100]],
    [[0x90,60,100],[0x80,60,0],[0xb0,64,0]],
  ]){
    const result=correlateTh08MidiSends(native,wrong);
    assert.equal(result.status,'mismatch');
    assert.ok(result.events.every(event=>event.sentBytes===null),'no partial event/send association can pass');
  }
  assert.equal(correlateTh08MidiSends([{bytes:[0x90,60,100]}],[[0x90,60,100]]).status,'matched');
  const resetReceipt={kind:'tinysynth-reset',resetCount:2,contextTime:12.4,sampleRate:48000,reverbEnabled:true,
    reverbBufferFrames:24000,reverbBufferFramesAfter:24000,wetPathReplaced:true,convolverNodeReplaced:true,reverbGainNodeReplaced:true,
    activeVoicesBefore:1,activeVoicesAfter:0};
  const restart=th08MidiRestartOutcome({before:{active:true,invalid:false,path:'bgm/th08_00.mid',sysexCount:0,messageCount:2,noteOnCount:1},
    closeEventsBefore:1,closeEventsAfter:2,resetCountBefore:1,resetCountAfter:2,activeVoicesBefore:1,activeVoicesAfter:0,
    closeResetReceipt:resetReceipt,postCloseNativeEvents:0,postCloseMessages:0,quietWindow:quietWindow(TH08_MIDI_PCM_ROLE),
    voicePcmWindow:quietWindow(TH08_MIDI_VOICE_PCM_ROLE),
    reopenedEvents:matched.events,reopenedCorrelation:matched,reopened:{native:{active:true,invalid:false,path:'bgm/th08_01.mid',
      sysexCount:0,messageCount:2,noteOnCount:1},pcm:runtimePcm(TH08_MIDI_PCM_ROLE,{windowCount:3}),contextState:'running'},sameRuntimeFrame:true,exitObserved:true});
  assert.equal(restart.passed,true);
  const wrongCorrelation=correlateTh08MidiSends(native,[[0x80,60,0],[0x90,60,100]]);
  assert.equal(th08MidiRestartOutcome({before:{active:true,invalid:false,path:'bgm/th08_00.mid',sysexCount:0,messageCount:2,noteOnCount:1},
    closeEventsBefore:1,closeEventsAfter:2,resetCountBefore:1,resetCountAfter:2,activeVoicesBefore:1,activeVoicesAfter:0,
    postCloseNativeEvents:0,postCloseMessages:0,quietWindow:quietWindow(TH08_MIDI_PCM_ROLE),
    reopenedEvents:wrongCorrelation.events,reopenedCorrelation:wrongCorrelation,
    reopened:{native:{active:true,invalid:false,path:'bgm/th08_01.mid',sysexCount:0,messageCount:2,noteOnCount:1},pcm:runtimePcm(TH08_MIDI_PCM_ROLE,{windowCount:3}),contextState:'running'},
    sameRuntimeFrame:true,exitObserved:true}).passed,false);
});

test('TH08 MIDI close clears the TinySynth reverb tail and rejects nonzero final output',()=>{
  const before={active:true,invalid:false,path:'bgm/th08_00.mid',sysexCount:0,messageCount:14,noteOnCount:4};
  const reopened={native:{active:true,invalid:false,path:'bgm/th08_01.mid',sysexCount:0,messageCount:2,noteOnCount:3},
    pcm:runtimePcm(TH08_MIDI_PCM_ROLE,{windowCount:3}),contextState:'running'};
  const correlation=correlateTh08MidiSends([{bytes:[0x90,60,100]},{bytes:[0x80,60,0]}],[[0x90,60,100],[0x80,60,0]]);
  const events=correlation.events;
  const quiet=quietWindow(TH08_MIDI_PCM_ROLE);
  const voiceQuiet=quietWindow(TH08_MIDI_VOICE_PCM_ROLE);
  const firstReceipt={kind:'tinysynth-reset',resetCount:3,contextTime:12.4,sampleRate:48000,reverbEnabled:true,
    reverbBufferFrames:24000,reverbBufferFramesAfter:24000,wetPathReplaced:true,convolverNodeReplaced:true,reverbGainNodeReplaced:true,
    activeVoicesBefore:3,activeVoicesAfter:0};
  const laterReceipt={...firstReceipt,resetCount:4,activeVoicesBefore:0,activeVoicesAfter:0};
  const input={before,closeEventsBefore:1,closeEventsAfter:3,resetCountBefore:2,resetCountAfter:4,
    activeVoicesBefore:3,activeVoicesAfter:0,closeResetReceipts:[firstReceipt,laterReceipt],postCloseNativeEvents:0,postCloseMessages:0,
    quietWindow:quiet,voicePcmWindow:voiceQuiet,reopenedEvents:events,reopenedCorrelation:correlation,reopened,
    sameRuntimeFrame:true,exitObserved:true};
  const pass=th08MidiRestartOutcome(input);
  assert.equal(pass.passed,true);
  assert.equal(pass.voiceOutputSilent,true);
  assert.equal(pass.outputQuiet,true);
  assert.equal(pass.voiceClearReceiptIndex,0,'the first active-voice clear is the close boundary proof');
  assert.equal(pass.subsequentResetCount,1);
  assert.equal(pass.laterResetsClear,true,'later resets in the same close interval must also leave the voices clear');
  assert.equal(pass.resetReceipt.resetCount,3,'a later zero-to-zero reset must not replace the first activity-to-zero receipt');
  const decayingOutput=th08MidiRestartOutcome({...input,quietWindow:quietWindow(TH08_MIDI_PCM_ROLE,{nonZeroPerWindow:8})});
  assert.equal(decayingOutput.passed,false,'every complete nonzero final-mix window after close is a stop failure');
  assert.equal(decayingOutput.status,'failed');
  assert.equal(decayingOutput.effectTailObserved,true);
  assert.equal(decayingOutput.quietStatus,'failed');
  assert.match(decayingOutput.quietReason,/remained nonzero after close/);
  assert.equal(decayingOutput.outputQuiet,false,'the report retains the actual final-mix PCM state');
  const misalignedVoice=runtimePcm(TH08_MIDI_VOICE_PCM_ROLE,{windowCount:8});
  misalignedVoice.windows.forEach(window=>{window.target.currentTime+=.001;window.taps[0].currentTime+=.001;});
  misalignedVoice.firstCurrentTime+=.001;misalignedVoice.lastCurrentTime+=.001;
  const unlocatedTail=th08MidiRestartOutcome({...input,quietWindow:quietWindow(TH08_MIDI_PCM_ROLE,{nonZeroPerWindow:8}),
    voicePcmWindow:quietWindow(TH08_MIDI_VOICE_PCM_ROLE)});
  assert.equal(unlocatedTail.status,'failed','nonzero final mix remains a product failure even if source timing cannot attribute it');
  assert.equal(unlocatedTail.effectTailObserved,true,'the same-sample source and final-mix windows locate residual wet output');
  const misalignedTail=th08MidiRestartOutcome({...input,quietWindow:quietWindow(TH08_MIDI_PCM_ROLE,{nonZeroPerWindow:8}),
    voicePcmWindow:{status:'complete',proof:'runtime-output-pcm-window',role:TH08_MIDI_VOICE_PCM_ROLE,pcmWindow:misalignedVoice}});
  assert.equal(misalignedTail.status,'failed');
  assert.equal(misalignedTail.effectTailObserved,false,'different sample times cannot locate the residual in the wet branch');
  assert.equal(misalignedTail.effectAttributionStatus,'incomplete');
  const missingOutputNode=structuredClone(quiet.pcmWindow);delete missingOutputNode.identity.sourceIdentity;
  const missingSourceIdentity=th08MidiRestartOutcome({...input,quietWindow:{...quiet,pcmWindow:missingOutputNode}});
  assert.equal(missingSourceIdentity.status,'incomplete','quiet PCM from an unidentified output node cannot prove the TinySynth compressor tap');
  assert.equal(th08MidiRestartOutcome({...input,activeVoicesAfter:1,closeResetReceipts:[firstReceipt,{...laterReceipt,activeVoicesAfter:1}]}).passed,false,
    'voices surviving native close must fail');
  assert.equal(th08MidiRestartOutcome({...input,postCloseMessages:1}).passed,false,'MIDI messages between close and reopen must fail');
  assert.equal(th08MidiRestartOutcome({...input,voicePcmWindow:quietWindow(TH08_MIDI_VOICE_PCM_ROLE,{nonZeroPerWindow:1})}).passed,false,
    'nonzero pre-effect TinySynth voice output after reset must fail');
  assert.equal(th08MidiRestartOutcome({...input,voicePcmWindow:null}).status,'incomplete','missing source PCM evidence is incomplete');
  assert.equal(th08MidiRestartOutcome({...input,closeResetReceipts:[]}).status,'incomplete','missing reset timing/voice receipt is incomplete');
  assert.equal(th08MidiRestartOutcome({...input,closeResetReceipts:[firstReceipt,{...laterReceipt,resetCount:5}]}).passed,false,
    'the close event must bind to the observed production reset call');
  assert.equal(th08MidiRestartOutcome({...input,closeResetReceipts:[firstReceipt,{...laterReceipt,wetPathReplaced:false}]}).passed,false,
    'every close receipt must prove the old wet branch was replaced');
  assert.equal(th08MidiRestartOutcome({...input,closeResetReceipts:[{...firstReceipt,activeVoicesAfter:2},laterReceipt]}).passed,false,
    'a later zero-to-zero receipt cannot hide an uncleared first reset');
  assert.equal(th08MidiRestartOutcome({...input,sameRuntimeFrame:false}).passed,false,'a new iframe cannot satisfy same-frame restart');

  const beforeTarget={role:TH08_MIDI_PCM_ROLE,available:true,contextIndex:0,tapIds:[0],sampleRate:48000,state:'running',currentTime:1,
    stateChanges:[],stateChangeCount:0,stateChangeDrops:0};
  const afterTarget={...beforeTarget,state:'closed',stateChanges:[{sequence:1,state:'closed',currentTime:1}],stateChangeCount:1};
  const lifecycle=verifyRuntimeAudioClosedReceipt(beforeTarget,afterTarget,{role:TH08_MIDI_PCM_ROLE});
  const closedMidiWindow={status:'closed',proof:'closed-context-lifecycle',role:TH08_MIDI_PCM_ROLE,target:afterTarget,lifecycleReceipt:lifecycle,
    pcmWindow:{status:'incomplete',windowCount:0,captureCount:0,outputObserved:null}};
  const midiClosedResult=th08MidiRestartOutcome({...input,quietWindow:closedMidiWindow});
  assert.equal(midiClosedResult.status,'incomplete','TinySynth keeps its AudioContext; a close receipt cannot replace the Runtime output and source windows');
});

test('TinySynth close contract is tied to the production bridge reset, voice pruning and output graph',async()=>{
  const bridge=await readFile(resolve(root,'launcher/app/services/midi-bridge.ts'),'utf8');
  const vendor=await readFile(resolve(root,'launcher/public/vendor/webaudio-tinysynth.min.js'),'utf8');
  assert.match(bridge,/const resetSynth = \(\) => \{synth\?\.reset\(\);\}/);
  assert.match(bridge,/const onClose: EventListener = \(\) => \{if \(authenticated\(next\)\) reset\(\);\}/);
  assert.match(bridge,/instance\.getAudioContext\(\)/);
  assert.match(bridge,/context\.createConvolver\(\)/);
  assert.match(bridge,/context\.createGain\(\)/);
  assert.match(bridge,/output\.disconnect\(oldConvolver\)/);
  assert.match(bridge,/oldConvolver\.disconnect\(\)/);
  assert.match(bridge,/oldReverb\.disconnect\(\)/);
  assert.match(bridge,/replacementReverb\.connect\(compressor\)/);
  assert.match(bridge,/graph\.conv = replacementConvolver/);
  assert.match(bridge,/graph\.rev = replacementReverb/);
  for(const sourceContract of [
    'reset:()=>{for(let i=0;i<16;++i)this.setProgram(i,0),this.setBendRange(i,256),this.setModulation(i,0),this.setChVol(i,100),this.setPan(i,64),this.resetAllControllers(i),this.allSoundOff(i),this.rhythm[i]=0',
    'this.resetAllControllers(i),this.allSoundOff(i)',
    'case 120:case 123:case 124:case 125:case 126:case 127:this.allSoundOff(ch)',
    'allSoundOff:ch=>{for(let i=this.notetab.length-1;i>=0;--i)',
    'this._pruneNote(nt),this.notetab.splice(i,1)',
    'nt.o[k].stop()',
    'nt.g[k].gain.value=0',
    'this.rhythm[ch]&&(o[i].onended=()=>{try{o[i].detune&&this.chmod[ch].disconnect(o[i].detune)}catch(e){}},o[i].stop(t+p[0].d*this.releaseRatio))',
    'this.rhythm[ch]||this.notetab.push({t:t,e:99999,ch:ch,n:n,o:o,g:g,t2:t+pn.a,v:vp,r:r,f:0})',
    'this.out.connect(this.conv),this.conv.connect(this.rev),this.rev.connect(this.comp)',
    'this.out.connect(this.comp),this.comp.connect(this.dest)',
  ])assert.ok(vendor.includes(sourceContract),`production TinySynth source lost ${sourceContract}`);
});

test('TH08 OGG fault is rejected only when the real source is pending, silent and diagnosed',()=>{
  const native={exists:true,name:'th08_01.wav',pending:true,soundReady:false,playing:false};
  const silent=runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:8,nonZeroPerWindow:0});
  assert.equal(th08OggFaultOutcome({fault:'missing',native,pcm:silent,requestStatus:404,
    configureError:'Resource download failed: /bgm-ogg/th08_01.ogg'}).passed,false,'missing download must prove rejected configure before any native launch');
  assert.equal(th08OggFaultOutcome({fault:'corrupt',native,pcm:silent,diagnostics:['音乐资源错误：/bgm-ogg/th08_01.ogg（OGG 解码失败）'],
    injectedBytesSha256:'b'.repeat(64),originalBytesSha256:'a'.repeat(64)}).passed,true);
  assert.equal(th08OggFaultOutcome({fault:'corrupt',native,pcm:silent,diagnostics:[],
    injectedBytesSha256:'b'.repeat(64),originalBytesSha256:'a'.repeat(64)}).status,'failed');
  assert.equal(th08OggFaultOutcome({fault:'missing',native,pcm:silent,
    requestStatus:200,configureError:'Resource download failed: /bgm-ogg/th08_01.ogg'}).passed,false);
  assert.equal(th08OggFaultOutcome({fault:'corrupt',native,pcm:{outputObserved:true},
    diagnostics:['音乐资源错误：/bgm-ogg/th08_01.ogg（OGG 解码失败）'],
    injectedBytesSha256:'b'.repeat(64),originalBytesSha256:'a'.repeat(64)}).passed,false);
  for(const pcm of [null,{outputObserved:false},runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:1,nonZeroPerWindow:0}),
    runtimePcm(TH08_MIDI_PCM_ROLE,{windowCount:8,nonZeroPerWindow:0})])
    assert.equal(th08OggFaultOutcome({fault:'corrupt',native,pcm,
      diagnostics:['音乐资源错误：/bgm-ogg/th08_01.ogg（OGG 解码失败）'],
      injectedBytesSha256:'b'.repeat(64),originalBytesSha256:'a'.repeat(64)}).passed,false,
    'silence requires eight actual windows from the identified Runtime output');
});

test('missing music proves the request-bound configure rejection and unlaunched Runtime, without inventing PCM silence',()=>{
  const ready={protocol:'eagler-touhou/1',game:'th08',epoch:2,event:'ready'},
    rejection={protocol:'eagler-touhou/1',game:'th08',epoch:2,request:'missing-request',ok:false,error:'Error: Resource download failed: /bgm-ogg/th08_01.ogg'};
  const input={fault:'missing',requestStatus:404,configureError:rejection.error,configureResponse:rejection,
    diagnostics:[rejection.error],startup:{app:0,firstFrameCount:0,ready}};
  const accepted=th08OggFaultOutcome(input);assert.equal(accepted.passed,true);assert.equal(accepted.startupRejected,true);
  assert.equal(accepted.outputVerified,false);assert.equal(accepted.silent,false);
  for(const mutate of [v=>v.startup.app=9,v=>v.startup.firstFrameCount=1,v=>delete v.configureResponse,
    v=>v.configureResponse.epoch=3,v=>v.configureResponse.ok=true,v=>v.requestStatus=200,v=>v.configureError='another file']){
    const changed=structuredClone(input);mutate(changed);assert.equal(th08OggFaultOutcome(changed).passed,false);
  }
  const fault={...input,path:'/bgm-ogg/th08_01.ogg',requestUrl:'https://127.0.0.1/music/th08/th08_01.ogg',requestCount:1};
  const http={url:fault.requestUrl,status:404},consoleError={url:fault.requestUrl,text:'Failed to load resource: 404'};
  for(const [channel,value] of [['http-or-request',http],['console',consoleError],['runtime-protocol',rejection]])
    assert.equal(isExpectedMissingMusicError({channel,value,stage:'configure',fault}),true);
  for(const value of [{...http,url:'https://127.0.0.1/data/th08'}, {...http,status:500}, {...http,failure:'net::ERR_ABORTED'}])
    assert.equal(isExpectedMissingMusicError({channel:'http-or-request',value,stage:'configure',fault}),false);
  assert.equal(isExpectedMissingMusicError({channel:'runtime-protocol',value:{...rejection,request:'other'},stage:'configure',fault}),false);
  assert.equal(isExpectedMissingMusicError({channel:'http-or-request',value:http,stage:'fault-recovery-launch',fault}),false);
  const mode={mode:'missing-ogg',missingOgg:fault,events:[rejection],eventStages:[{index:0,stage:'configure'}]};
  const record={modes:[mode],httpErrors:[{...http,mode:mode.mode,stage:'configure'}],consoleErrors:[{...consoleError,mode:mode.mode,stage:'configure'}]};
  assert.deepEqual(musicScenarioErrors(record),[]);assert.equal(record.httpErrors.length,1,'expected negative raw errors remain retained');
  mode.events.push({...rejection,request:'unexpected'});mode.eventStages.push({index:1,stage:'configure'});
  assert.equal(musicScenarioErrors(record).length,1,'the next unexpected rejection still fails');
  mode.events[1]={...rejection};assert.equal(musicScenarioErrors(record).length,1,'a repeated expected response is not another allowance');
  mode.events.length=1;record.httpErrors.push({...record.httpErrors[0]});
  assert.equal(musicScenarioErrors(record).length,1,'a repeated same-URL HTTP failure cannot borrow the original negative');
  record.httpErrors.length=1;record.consoleErrors.push({...record.consoleErrors[0]});
  assert.equal(musicScenarioErrors(record).length,1,'the console allowance is also single-use');
});

test('music protocol failures survive an old document snapshot and reject foreign events or failed binding drains',async()=>{
  const callbacks={},captured=[],runtime={},window={__eaglerMusicProtocolStage:'fault-recovery-navigation',
    __captureMusicProtocolFailure:async value=>captured.push(value)};window.top=window;
  const frame={contentWindow:runtime,src:'https://local.test/runtime/th08.html?runtimeEpoch=2'};
  runInNewContext(`(${installMusicProtocolCapture.toString()})({game:'th08'})`,{window,URL,
    document:{querySelector:()=>frame},location:{origin:'https://local.test',href:'https://local.test/host?epoch=2'},
    addEventListener:(name,callback)=>callbacks[name]=callback});
  const data={protocol:'eagler-touhou/1',game:'th08',epoch:2,event:'abort',error:'late prior-document abort'},
    event={origin:'https://local.test',source:runtime,data};
  for(const foreign of [{...event,origin:'https://foreign.test'},{...event,source:{}},
    {...event,data:{...data,epoch:3}},{...event,data:{...data,game:'th07'}}])callbacks.message(foreign);
  assert.equal(captured.length,0);
  const mode={mode:'corrupt-ogg',events:[],eventStages:[],protocolCapture:captured};
  callbacks.message(event);assert.equal(await window.__flushMusicProtocolCaptures(),0);
  assert.equal(mode.events.length,0,'the old event snapshot never sees the late abort');
  const errors=musicScenarioErrors({modes:[mode]});assert.equal(errors.length,1);
  assert.equal(errors[0].value.error,'late prior-document abort');assert.equal(errors[0].stage,'fault-recovery-navigation');
  window.__captureMusicProtocolFailure=()=>Promise.reject(Error('binding lost during navigation'));
  callbacks.message(event);await assert.rejects(window.__flushMusicProtocolCaptures(),/binding lost during navigation/);
});

test('fault recovery requires a new configured epoch and real MIDI messages and PCM, while UI coverage stays partial',()=>{
  const native={active:true,invalid:false,fileIndex:8,trackCount:2,sysexCount:0,messageCount:2,noteOnCount:3,path:'bgm/th08_01.mid'},
    correlation=correlateTh08MidiSends([{bytes:[0x90,60,100],time:10},{bytes:[0x80,60,0],time:100}],[[0x90,60,100],[0x80,60,0]]);
  const fault={fault:'corrupt',passed:true,diagnosticVisible:true},
    recovery={selectedMode:'midi',observedMode:'midi',capability:{midi:true},audioReady:{state:'running',currentTime:1},runtimeResources:[],before:{href:'/old?runtimeEpoch=2',epoch:2},after:{href:'/new?runtimeEpoch=3',epoch:3},oldFrameDetached:true,
      configureResponse:{protocol:'eagler-touhou/1',game:'th08',epoch:3,ok:true},
      firstFrame:{protocol:'eagler-touhou/1',game:'th08',epoch:3,event:'first-frame'},
      exit:{protocol:'eagler-touhou/1',game:'th08',epoch:2,event:'exit',code:0,status:'success'},
      scene:{native,sendCorrelation:correlation,events:correlation.events,contextState:'running',pcm:runtimePcm(TH08_MIDI_PCM_ROLE)}};
  const accepted=th08MusicFaultRecoveryOutcome({fault,recovery});assert.equal(accepted.passed,true);assert.equal(accepted.complete,false);
  for(const mutate of [v=>v.after.epoch=2,v=>v.observedMode='ogg',v=>v.configureResponse.epoch=2,v=>v.oldFrameDetached=false,
    v=>v.capability.midi=false,v=>v.firstFrame.epoch=2,v=>v.exit.code=1,v=>v.runtimeResources.push({path:'old.ogg'}),
    v=>v.scene.pcm.outputObserved=false,v=>v.scene.events=[],v=>v.scene.native.path='wrong.mid',v=>v.audioReady.state='suspended',v=>delete v.audioReady,
    v=>v.scene.pcm.windows[0].target.state='suspended']){
    const changed=structuredClone(recovery);mutate(changed);assert.equal(th08MusicFaultRecoveryOutcome({fault,recovery:changed}).passed,false);
  }
  assert.equal(th08MusicFaultRecoveryOutcome({fault,recovery,errors:[{channel:'http'}]}).passed,false);
  const missingRecovery=structuredClone(recovery);delete missingRecovery.exit;
  assert.equal(th08MusicFaultRecoveryOutcome({fault:{...fault,fault:'missing'},recovery:missingRecovery}).passed,true);
  const journey=th08MusicFaultJourneyOutcome({corrupt:fault,missing:{...fault,fault:'missing'},corruptRecovery:accepted,missingRecovery:accepted});
  assert.equal(journey.passed,true);assert.equal(journey.status,'partial');assert.equal(journey.complete,false);
  assert.equal(th08MusicFaultJourneyOutcome({corrupt:fault,corruptRecovery:accepted}).passed,false);
});

test('corrupt OGG mode reports its behavior verdict while AUDIO-05 keeps partial coverage',async()=>{
  const producer=await readFile(resolve(import.meta.dirname,'music-browser.mjs'),'utf8');
  assert.match(producer,/Object\.assign\(corrupt,attachTh08OggFaultModeAssessment\(corrupt,oggFault\)\)/,
    'the browser producer must publish its mode through the assessed result path');
  const native={exists:true,name:'th08_01.wav',pending:true,soundReady:false,playing:false};
  const silent=runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:8,nonZeroPerWindow:0});
  const diagnose=['音乐资源错误：/bgm-ogg/th08_01.ogg（OGG 解码失败）'];
  const buildReport=assessment=>{
    const mode=attachTh08OggFaultModeAssessment({mode:'corrupt-ogg',status:'recorded'},assessment);
    return{schema:'eagler-touhou/music-browser/1',runId:'music-behavior-verdict',startedAt:'2026-10-21T00:00:00.000Z',
      finishedAt:'2026-10-21T00:00:01.000Z',status:assessment.passed?'passed':assessment.status,passed:assessment.passed,
      scope:{game:'th08',variant:'normal'},coverage:{status:'partial',complete:false},
      checks:[{...assessment,id:'AUDIO-05'}],modes:[mode]};
  };
  const assessment=th08OggFaultOutcome({fault:'corrupt',native,pcm:silent,diagnostics:diagnose,
    injectedBytesSha256:'b'.repeat(64),originalBytesSha256:'a'.repeat(64)});
  const report=buildReport(assessment),mode=report.modes[0];
  assert.equal(assessment.passed,true);assert.equal(assessment.status,'partial');assert.equal(assessment.complete,false);
  assert.equal(mode.status,'passed');assert.equal(mode.passed,true);
  assert.equal(mode.faultAssessment.status,'partial');assert.equal(mode.faultAssessment.complete,false);
  assert.equal(report.checks[0].status,'partial');assert.equal(report.coverage.complete,false);
  const selected={game:'th08',variant:'normal'},processResult={status:'passed',code:0};
  const accepted=assessDriverReport(selected,processResult,report);
  assert.equal(accepted.status,'passed','generic result assessment accepts the passed behavior mode');
  assert.equal(accepted.scenarios.find(scene=>scene.id==='corrupt-ogg').status,'passed');
  assert.equal(accepted.checks.find(check=>check.id==='AUDIO-05').status,'partial',
    'the numbered result keeps its own coverage status');
  assert.equal(accepted.adaptationComplete,false);

  const missing=th08OggFaultOutcome({fault:'corrupt'}),missingReport=buildReport(missing);
  assert.equal(missingReport.modes[0].status,'incomplete');
  assert.equal(assessDriverReport(selected,processResult,missingReport).status,'incomplete',
    'missing source, PCM, diagnostics, or fault identity cannot pass through the mode adapter');
  const undiagnosed=th08OggFaultOutcome({fault:'corrupt',native,pcm:silent,diagnostics:[],
    injectedBytesSha256:'b'.repeat(64),originalBytesSha256:'a'.repeat(64)}),failedReport=buildReport(undiagnosed);
  assert.equal(failedReport.modes[0].status,'failed');
  assert.equal(assessDriverReport(selected,processResult,failedReport).status,'failed',
    'a proven resource rejection with no visible diagnostic remains a failure');
});

test('MIDI foreground recovery requires synth PCM as well as retained sequencer time',()=>{
  const native={path:'th08_00.mid',active:true,elapsedTicks:100};
  const input={mode:'midi',before:native,paused:{...native,visibilityState:'hidden'},
    settled:{...native,visibilityState:'hidden'},resumed:{...native,elapsedTicks:120,visibilityState:'visible'},
    bridgeBefore:'running',bridgePaused:'suspended',bridgeResumed:'running'};
  assert.equal(th08BackgroundContinuityOutcome(input).status,'incomplete');
  assert.equal(th08BackgroundContinuityOutcome({...input,resumedOutput:{pcm:
    runtimePcm(TH08_MIDI_PCM_ROLE,{windowCount:2,nonZeroPerWindow:25})}}).passed,true);
  assert.equal(th08BackgroundContinuityOutcome({...input,resumedOutput:{pcm:
    runtimePcm(TH08_MIDI_PCM_ROLE,{windowCount:2,nonZeroPerWindow:0})}}).passed,false);
  assert.equal(th08BackgroundContinuityOutcome({...input,resumedOutput:{pcm:
    runtimePcm(TH08_OGG_PCM_ROLE,{windowCount:2,nonZeroPerWindow:25})}}).passed,false);
});

test('audible PCM cannot hide console, late response audit, request, or native protocol failures',()=>{
  const faults=[
    {errors:[{mode:'ogg',error:'Protocol history could not be read after renderer crash'}]},
    {consoleErrors:['Audio graph failed']},
    {httpErrors:[{url:'https://fixture.test/track.ogg',failure:'net::ERR_FAILED'}]},
    {modes:[{mode:'ogg',responseAuditError:'Late Runtime response identity mismatch'}]},
    {modes:[{mode:'ogg',fixtureResponses:{errors:['Browser body was truncated']}}]},
    {modes:[{mode:'ogg',events:[{event:'error',error:'Native decoder failed'}]}]},
    {modes:[{mode:'ogg',events:[{event:'abort',error:'Runtime stopped'}]}]},
    {modes:[{mode:'ogg',events:[{request:'configure',ok:false,error:'Rejected'}]}]},
  ];
  for(const record of faults){
    const errors=musicScenarioErrors(record);assert.equal(errors.length,1);
    assert.equal(musicTitleSceneOutcome({oggPcmObserved:true,errors}).scenario.status,'failed');
  }
  assert.deepEqual(musicScenarioErrors({modes:[{mode:'ogg',events:[{event:'runtime-info',ok:true}]}]}),[]);
});

test('music errors stay in their observed mode and stage while the global audit retains restart failures',()=>{
  const record={errors:[
    {mode:'ogg',stage:'title-observation',error:'Title observation failed'},
    {mode:'ogg',stage:'restart',error:'Relaunch GL error'},
    {mode:'midi',stage:'restart',error:'MIDI relaunch GL error'},
  ],consoleErrors:[{mode:'ogg',stage:'restart',text:'Relaunch console error'}],httpErrors:[],modes:[
    {mode:'ogg',events:[{event:'error',error:'title protocol error'},{event:'error',error:'restart protocol error'}],
      eventStages:[{index:0,stage:'title-observation'},{index:1,stage:'restart'}]},
  ]};
  const title=musicScenarioErrors(record,{mode:'ogg',stages:['title-observation']});
  assert.deepEqual(title.map(error=>error.value.error),['Title observation failed','title protocol error']);
  const restart=musicScenarioErrors(record,{mode:'ogg',stages:['restart']});
  assert.deepEqual(restart.map(error=>error.value.error??error.value.text),['Relaunch GL error','Relaunch console error','restart protocol error']);
  const global=musicScenarioErrors(record);
  assert.ok(global.some(error=>error.value.error==='Relaunch GL error'));
  assert.ok(global.some(error=>error.value.error==='MIDI relaunch GL error'));
  assert.equal(musicScenarioPassed([{passed:true}],global),false,'a restart error remains an overall scenario failure');
  assert.equal(musicScenarioPassed([{passed:true}],[]),true,'a clean full error audit preserves passing checks');
  assert.equal(musicScenarioErrors(record,{mode:'midi',stages:['title-observation']}).length,0);
});

test('stage error attachment preserves incomplete observations and only promotes captured errors to failed',()=>{
  assert.deepEqual(attachMusicStageErrors({id:'AUDIO-03-restart',status:'incomplete',passed:false,
    quietReason:'target AudioContext clock did not advance'},[]),
  {id:'AUDIO-03-restart',status:'incomplete',passed:false,quietReason:'target AudioContext clock did not advance',stageErrors:[]});
  assert.equal(attachMusicStageErrors({id:'AUDIO-03-restart',status:'incomplete',passed:false},[
    {channel:'runtime-protocol',value:'audio operation error'},
  ]).status,'failed');
  assert.equal(attachMusicStageErrors({id:'AUDIO-03-restart',status:'failed',passed:false},[]).status,'failed',
    'a measured product failure remains failed without a stage error');
});

test('music-browser rejects unknown, duplicate and empty CLI values with a fresh failed report',async t=>{
  const temp=await mkdtemp(join(tmpdir(),'eagler-music-cli-'));t.after(()=>rm(temp,{recursive:true,force:true}));
  const driver=resolve(import.meta.dirname,'music-browser.mjs');
  const cases=[
    {name:'unknown',extra:['--misspelled=true'],expected:'unknown option'},
    {name:'duplicate',extra:['--game=th11'],expected:'Duplicate option'},
    {name:'empty-game',replace:'game',extra:['--game='],expected:'requires =VALUE'},
    {name:'empty-music-manifest',replace:'music-manifest',extra:['--music-manifest='],expected:'requires =VALUE'},
  ];
  for(const {name,extra,replace,expected} of cases){
    const output=join(temp,name);
    const base=['--output='+output,'--manifest=unused.json','--music-manifest=unused.json','--game=th11','--variant=normal'];
    const args=[...(replace?base.filter(value=>!value.startsWith(`--${replace}=`)):base),...extra];
    const run=spawnSync(process.execPath,[driver,...args],{cwd:root,encoding:'utf8',timeout:30000});
    assert.equal(run.status,1,`${name}: ${run.stdout}\n${run.stderr}`);
    const report=JSON.parse(await readFile(join(output,'report.json'),'utf8'));
    assert.equal(report.status,'failed');assert.equal(report.passed,false);
    assert.match(report.failure,new RegExp(expected,'i'));
  }
});

test('Launcher music ZIP errors fail before reading Runtime inputs or starting a scene',async t=>{
  const temp=await mkdtemp(join(tmpdir(),'eagler-music-zip-preflight-'));t.after(()=>rm(temp,{recursive:true,force:true}));
  const invalid=join(temp,'invalid.zip');await writeFile(invalid,'not a ZIP');
  const cases=[
    {name:'missing-map',packages:{},expected:/Launcher music ZIP input is missing/},
    {name:'missing-zip',packages:{th08:join(temp,'missing.zip')},expected:/missing\.zip/},
    {name:'invalid-zip',packages:{th08:invalid},expected:/invalid ZIP file/i},
  ];
  for(const {name,packages,expected} of cases){
    const output=join(temp,name),map=join(temp,name+'.json');await writeFile(map,JSON.stringify(packages));
    const run=spawnSync(process.execPath,[resolve(import.meta.dirname,'music-browser.mjs'),
      '--output='+output,'--manifest=unread-runtime.json','--music-manifest=unread-music.json','--game=th08',
      '--variant=normal','--launcher-ui','--packages='+map,'--artwork-dir='+temp],
      {cwd:root,encoding:'utf8',timeout:30000});
    assert.equal(run.status,1,run.stdout+'\n'+run.stderr);
    const report=JSON.parse(await readFile(join(output,'report.json'),'utf8'));
    assert.equal(report.status,'failed');assert.match(report.failure,expected);assert.deepEqual(report.targets,[]);
    assert.ok(!report.failure.includes('unread-runtime.json'),'ZIP errors must be found before Runtime verification');
    assert.ok(!report.server,'input rejection must precede scene/server setup');
  }
});
