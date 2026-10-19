import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {mkdtemp,mkdir,readFile,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve,join,relative} from 'node:path';
import {createRuntimeFixtureSite} from './runtime-fixture-site.mjs';
import {musicTitleSceneOutcome} from './music-browser-result.mjs';
import {describeNativeMusic,describeNoneMusicControl,mapTrackToInput,nativeAudioStatsShape,runtimeMusicResources,
  readNativeAudioState,selectedMusicMappingGames,summarizeMusicPcm,validateMusicManifest,verifyMusicInputs} from './music-browser-helpers.mjs';

const root=resolve(import.meta.dirname,'../..');
const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');

test('native title maps are loaded only for games selected in this sparse checkout run',()=>{
  assert.deepEqual(selectedMusicMappingGames(['th08']),[]);
  assert.deepEqual(selectedMusicMappingGames(['th11']),['th11']);
  assert.deepEqual(selectedMusicMappingGames(['th15','th08']),['th15']);
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
  ]),{captureCount:2,contexts:['running'],sampleRate:44100,peakAbs:.5,maxRms:.2,nonZeroSamples:76,outputObserved:true});
  assert.equal(summarizeMusicPcm([]).outputObserved,false);
  assert.deepEqual(mapTrackToInput('th11_02.ogg',[{name:'th11_02.ogg',bytes:10,sha256:'a'.repeat(64)}]),
    {name:'th11_02.ogg',path:undefined,bytes:10,sha256:'a'.repeat(64)});
});

test('music=none control requires a running silent output window after settling',()=>{
  assert.deepEqual(describeNoneMusicControl({captureCount:6,contexts:['running'],nonZeroSamples:0,outputObserved:false}),
    {status:'silent-after-settling',settleMs:1800,captureWindowMs:600,samples:6,contextStates:['running']});
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
