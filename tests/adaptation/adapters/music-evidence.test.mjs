import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,writeFileSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {createHash} from 'node:crypto';
import {assertMusicRequirement,assessMusicEvidence} from './music-evidence.mjs';
import {parseTh08MusicCatalog} from './titles/th08-music.mjs';
import {runtimePcm} from '../../../tools/ports/fixtures/music-pcm-window.mjs';
import {TH08_OGG_PCM_ROLE,TH08_MIDI_PCM_ROLE} from '../../../tools/ports/music-browser-helpers.mjs';
import {correlateTh08MidiSends} from '../../../tools/ports/music-browser-result.mjs';
import {nativeTabFixture} from '../fixtures/native-tab-lifecycle.mjs';
const product={game:'th08',variant:'normal'};
function fixture(t){
  const directory=mkdtempSync(join(tmpdir(),'music-acceptance-'));t.after(()=>rmSync(directory,{recursive:true,force:true}));
  const receipt=(name,bytes)=>{const path=join(directory,name);writeFileSync(path,bytes);return{path,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')};};
  const comments=Buffer.from(Array.from({length:21},(_,i)=>`@bgm/th08_${String(i).padStart(2,'0')}.mid\nTitle ${i}\nDescription`).join('\n'));
  const format=Buffer.alloc(21*52+1);
  for(let i=0;i<21;i++){const offset=i*52;format.write(`th08_${String(i).padStart(2,'0')}.wav`,offset,'ascii');
    format.writeUInt32LE(4,offset+24);format.writeUInt32LE(400,offset+28);format.writeUInt16LE(1,offset+32);format.writeUInt16LE(2,offset+34);
    format.writeUInt32LE(44100,offset+36);format.writeUInt16LE(4,offset+44);format.writeUInt16LE(16,offset+46);}
  const catalog=parseTh08MusicCatalog(comments,format),files=catalog.map(track=>({name:track.file,...receipt(track.file,Buffer.from(track.file))}));
  const data={bytes:64,sha256:'d'.repeat(64)},runtime={packageSha256:'c'.repeat(64)};
  const oggScene=(track,decodeMode)=>{
    const pcm=runtimePcm(TH08_OGG_PCM_ROLE,{startTime:1}),identity=pcm.identity;
    const target={role:identity.role,available:true,contextIndex:identity.contextIndex,tapIds:identity.tapIds,sampleRate:identity.sampleRate,
      state:'running',currentTime:1,stateChanges:[],stateChangeCount:0,stateChangeDrops:0};
    const buffering={queuedFrames:3072,queuedSampleRate:44100,outputBufferFrames:2048,analysisWindowFrames:2048,baseLatency:.01};
    const sample=(time,attempt)=>({attempt,trackFile:track.file,native:{name:track.file.replace('.ogg','.wav'),exists:true,soundReady:true,
      pending:false,playing:true,outputPaused:false,queuedFrames:3072,cursorFrames:Math.floor(time*44100)},target:{...target,currentTime:time},buffering:{...buffering}});
    const sourceBoundary={file:track.file,basis:{...buffering,target:{...target,currentTime:.75}},firstObservedAtAttempt:1,
      settle:{status:'complete',requiredFrames:7920,startedAtTime:.75,completedAtTime:1,elapsedFrames:12000},samples:[sample(.75,1),sample(1,2)]};
    const input=files.find(file=>file.name===track.file);
    return{native:{name:track.file.replace('.ogg','.wav'),exists:true,soundReady:true,pending:false,playing:true,lastMusicSong:track.index===0?1:0,
      totalFrames:track.totalFrames,introFrames:track.introFrames,sampleRate:track.sampleRate},trackFile:track.file,input,decoder:{enabled:true,decodeMode},pcm,
      pcmCollection:{status:'observed',expectedFile:track.file,observedFile:track.file,nativeReady:true,attempts:2,maxSamples:32,pcmObservedAtAttempt:1,
        windowCount:1,captureCount:1,outputObserved:true,nonZeroSamples:pcm.nonZeroSamples,target,sourceBoundary}};
  };
  const midiScene=path=>{const stage=path.includes('_00'),nativeEvents=[{bytes:[0x90,60,100]},{bytes:[0x80,60,0]},...(stage?[{bytes:[0x90,62,100]},{bytes:[0x80,62,0]}]:[])],synthSends=nativeEvents.map(row=>row.bytes);
    const sendCorrelation=correlateTh08MidiSends(nativeEvents,synthSends);
    return{native:{path,active:true,invalid:false,fileIndex:0,trackCount:1,messageCount:nativeEvents.length,sysexCount:0,noteOnCount:stage?2:1,openCount:stage?2:1,lastMusicSong:stage?1:0},
      nativeEvents,synthSends,sendCorrelation,events:sendCorrelation.events,pcm:runtimePcm(TH08_MIDI_PCM_ROLE),contextState:'running'};};
  const modes=[0,1].map(decodeMode=>({mode:decodeMode?'ogg-full':'ogg',title:oggScene(catalog[1],decodeMode),start:{audio:oggScene(catalog[0],decodeMode)}}));
  for(const mode of modes){const input=files[1];mode.fixtureResponses={responses:[{url:'https://fixture/music/th08/'+input.name,status:200,
    actual:{bytes:input.bytes,sha256:input.sha256},expected:{bytes:input.bytes,sha256:input.sha256}}]};}
  modes.push({mode:'midi',midiBridge:true,title:midiScene('bgm/th08_01.mid'),start:{audio:midiScene('bgm/th08_00.mid')}});
  const target={...product,status:'passed',passed:true,modes,checks:[],packageIdentity:runtime,dataIdentity:data,identityPostflight:{runtime,data},httpFailures:[]};
  const driver={schema:'eagler-touhou/music-browser/1',targets:[target],musicInputs:{th08:{files}},musicMapEvidence:{th08:{data,
    entries:{comments:receipt('comments.bin',comments),format:receipt('format.bin',format)},tracks:catalog.map(track=>({...track,input:files[track.index]}))}}};
  return{driver,target,product};
}
test('complete music proof independently verifies native OGG and MIDI title to stage',t=>{
  for(const id of ['AUDIO-01','MIDI-02']){
    const args=fixture(t),observation=assertMusicRequirement({...args,id});
    const entry={id,status:'passed',complete:true,observation};args.target.checks.push(entry);
    assert.equal(assessMusicEvidence({...args,entry}).valid,true);
  }
});

function withBackground(args){
  args.target.lifecycleBrowser={processId:123,browserVersion:{userAgent:'Chrome'},scope:'same-window-tab',osOcclusion:'disabled-for-tab-isolation'};
  args.target.lifecycleCleanup={errors:[],gateway:{errors:[]}};
  for(const scene of args.target.modes.filter(scene=>scene.mode.startsWith('ogg'))){
    const native={...scene.start.audio.native,cursorFrames:100,outputPaused:false},paused={...native,cursorFrames:110,outputPaused:true,visibilityState:'hidden'};
    scene.start.background={nativeLifecycle:nativeTabFixture(),decoderSamples:Array.from({length:5},()=>({...scene.title.decoder})),
      observationInput:{mode:'ogg',before:native,paused,settled:structuredClone(paused),resumed:{...native,cursorFrames:120,visibilityState:'visible'},resumedOutput:scene.start.audio}};
  }return args;
}
test('music recovery requires native visibility and source-bound actual output in both decoder modes',t=>{
  assertMusicRequirement({...withBackground(fixture(t)),id:'AUDIO-04'});
});
for(const [name,mutate] of Object.entries({
  'synthetic background':a=>a.target.modes[0].start.background.nativeLifecycle.restored.runtime.events[0].trusted=false,
  'background source changed':a=>a.target.modes[0].start.background.observationInput.settled.name='other.wav',
  'cursor advanced while hidden':a=>a.target.modes[1].start.background.observationInput.settled.cursorFrames++,
  'wrong decoder after restore':a=>a.target.modes[1].start.background.decoderSamples[4].decodeMode=0,
  'missing actual output':a=>a.target.modes[0].start.background.observationInput.resumedOutput.pcm.windows[0].taps[0].nonZero=0,
  'browser cleanup error':a=>a.target.lifecycleCleanup.errors.push('failed'),
}))test(`background music gate rejects ${name}`,t=>{const args=withBackground(fixture(t));mutate(args);assert.throws(()=>assertMusicRequirement({...args,id:'AUDIO-04'}));});
for(const [name,id,mutate] of [
  ['wrong OGG title','AUDIO-01',a=>a.target.modes[0].title.native.name='th08_00.wav'],
  ['wrong stage song','AUDIO-01',a=>a.target.modes[0].start.audio.native.lastMusicSong=0],
  ['wrong decoder policy','AUDIO-01',a=>a.target.modes[1].title.decoder.decodeMode=0],
  ['stale playing summary','AUDIO-01',a=>a.target.modes[0].title.pcm.windows[0].taps[0].nonZero=0],
  ['buffer from previous source','AUDIO-01',a=>a.target.modes[0].title.pcmCollection.sourceBoundary.samples[0].native.name='th08_00.wav'],
  ['wrong source duration','AUDIO-01',a=>a.target.modes[0].start.audio.native.totalFrames++],
  ['input bytes replaced','AUDIO-01',a=>writeFileSync(a.driver.musicInputs.th08.files[0].path,'replaced')],
  ['delivered music replaced','AUDIO-01',a=>{const row=a.target.modes[0].fixtureResponses.responses[0];row.actual.sha256='b'.repeat(64);row.expected.sha256=row.actual.sha256;}],
  ['catalog omitted track','AUDIO-01',a=>a.driver.musicMapEvidence.th08.tracks.pop()],
  ['postflight package changed','AUDIO-01',a=>a.target.identityPostflight={runtime:{packageSha256:'b'.repeat(64)},data:a.target.dataIdentity}],
  ['wrong MIDI stage','MIDI-02',a=>a.target.modes[2].start.audio.native.path='bgm/th08_01.mid'],
  ['MIDI send dropped','MIDI-02',a=>a.target.modes[2].title.synthSends.pop()],
  ['MIDI notes changed','MIDI-02',a=>a.target.modes[2].title.synthSends[0]=[0x90,61,100]],
  ['MIDI output suspended','MIDI-02',a=>a.target.modes[2].title.pcm.windows[0].target.state='suspended'],
  ['MIDI stage native counter differs','MIDI-02',a=>a.target.modes[2].start.audio.native.messageCount++],
  ['only cached title notes','MIDI-02',a=>a.target.modes[2].start.audio.native.noteOnCount=1],
  ['missing actual menu','MIDI-01',a=>a.target.launcherMusic={status:'observed',passed:true}],
  ['missing actual fallback','AUDIO-05',a=>a.target.launcherMusic={status:'observed',passed:true}],
  ['missing resource separation','MIDI-04',a=>a.target.launcherMusic={status:'observed',passed:true}],
])test(`complete music proof rejects ${name}`,t=>{const args=fixture(t);mutate(args);assert.throws(()=>assertMusicRequirement({...args,id}));});
