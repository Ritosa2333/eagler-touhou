import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {duckOriginalDemoMusic} from '../../site/runtime/original-title-scenes.mjs';

const play=readFileSync(new URL('../../site/play.mjs',import.meta.url),'utf8');
function section(from,to){
  const start=play.indexOf(from),end=play.indexOf(to,start+from.length);
  assert.ok(start>=0&&end>start);return play.slice(start,end);
}
const bridge=[
  section('function stopAudio(','function stopTitleSource('),
  section('async function prepareGameSounds(','function currentVariantKey('),
  section('function startTitleDemoMusic(','async function startTitleDemo('),
  section('function leaveTitleDemo(','function requestTitleDemoExit('),
  section('function disposeOriginalMain(','function nativeMenuSnapshot('),
  section('function returnToTitle(','function releaseOriginalPauseInput('),
].join('\n');

// Actual page transitions and teardown. Only WebAudio/DOM/native Title are
// controlled ports; no user audio device, persistence or World is used.
for(const withTitleMix of [false,true])test(`the original title source survives the complete Demo round trip (${withTitleMix?'native title mix':'direct title mix'})`,async()=>{
  const calls={started:0,stopped:0,closed:0,fxDisposed:0,titleDisposed:0,booted:0};
  const node=(level=1)=>({gain:{value:level},targets:new Set(),connect(to){this.targets.add(to);},disconnect(to){if(to)this.targets.delete(to);else this.targets.clear();}});
  const gain=node(.8),bgm=node(.72),source=node();
  source.start=()=>calls.started++;source.stop=()=>calls.stopped++;
  source.connect(withTitleMix?bgm:gain);
  const audio={state:'running',createGain:()=>node(),close:async()=>{calls.closed++;}};
  const playback={cancel(){}};
  const titleAudio=withTitleMix?{bgmGain:bgm,dispose(){calls.titleDisposed++;}}:null;
  const run={playback},state={audio,gain,source,titleAudio,titleDemoRun:run,replayPlayback:playback,pressed:new Set(),
    module:{_th128_boot_title(){calls.booted++;return 1;}},worldMode:false};
  const fx=class {constructor(context,output){assert.equal(context,audio);assert.equal(output,gain);}async load(){}dispose(){calls.fxDisposed++;}};
  const nop=()=>{};
  const api=runInNewContext(bridge+'\n;({prepareGameSounds,installPreparedStageAudio,leaveTitleDemo,stopAudio})',{
    state,menu:{},menuMusic:{cache:new Map()},document:{querySelector:()=>null},GameSoundQueue:fx,readJson:async()=>({}),safeAssetUrl:nop,
    StageMusic:class {constructor(){assert.fail('Demo requested a stage track');}},musicOutput:(_context,output)=>output,
    startTitleSource:()=>assert.fail('The already running Title source was restarted'),recordOwnedHeardTrack:()=>assert.fail('Demo changed heard tracks'),
    duckOriginalDemoMusic,titleDemoClock:{reset:nop},disposeOriginalStageTextRasters:nop,disposeOriginalMainMusicRasters:nop,
    thpracLogicClock:{reset:nop},clock:{reset:nop},thpracToolsPanel:null,closeOrdinaryWorldAudio:nop,
    stopMenuMusic:nop,recordOwnedMainSession:nop,cancelOriginalEnding:nop,discardPendingCarry:nop,resetHighFrameratePresentation:nop,
    syncGameplayFrameUI:nop,enterTitleMenu:nop,beginOriginalSceneLoading:()=>null,error:assert.fail,
  });
  const pending=await api.prepareGameSounds('/original/',new AbortController().signal,['th128_01.wav']);
  api.stopAudio({keepTitleMusic:true}); // retire the Title controller at the World mutation fence
  api.installPreparedStageAudio(pending);state.worldMode=true;
  assert.equal(state.source,source);assert.equal(state.audio,audio);assert.equal(state.stageMusic,null);
  const bus=[...source.targets][0];assert.equal(bus.gain.value,withTitleMix?.18:.25);assert.equal(gain.gain.value,.8);
  api.leaveTitleDemo(run);
  assert.equal(state.source,source);assert.equal(state.audio,audio);assert.equal(state.titleAudio,titleAudio);
  assert.deepEqual([...source.targets],[withTitleMix?bgm:gain]);assert.equal(bgm.gain.value,.72);
  assert.deepEqual(calls,{started:0,stopped:0,closed:0,fxDisposed:1,titleDisposed:0,booted:1});
  api.stopAudio();assert.equal(calls.stopped,1);assert.equal(calls.closed,1);
});
