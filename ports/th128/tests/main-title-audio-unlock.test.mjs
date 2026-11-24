import fs from 'node:fs';
import vm from 'node:vm';
import assert from 'node:assert/strict';
import test from 'node:test';
const source=fs.readFileSync(process.env.TH128_PLAY_SOURCE??new URL('../site/play.mjs',import.meta.url),'utf8');
const body=['ensureOriginalTitleAudio','finishNormalPostrunMain','advanceNormalPostrun'].map(name=>{
 const start=source.indexOf('async function '+name+'(');assert(start>=0,'Missing production function '+name);
 return source.slice(start,source.indexOf('\n}',start)+2);
}).join('\n');
function environment(){
 let created=0;const calls=[];
 const state={module:{},controller:new AbortController(),audio:{state:'running'},gain:{},base:'/owned/',
  manifest:{music:[]},mainAudioLoading:null,mainSavedSettings:{bgm:85,se:70}};
 const menu={audioUnlocked:false};
 let resolveLoad;
 const loadBoundary=new Promise(resolve=>{resolveLoad=resolve;});
 class AudioBoundary{
  constructor(options){Object.assign(this,options);this.sounds={};this.bgmGain={};this.disposed=false;created++;}
  async load(){await loadBoundary;}
  setVolumes(bgm,se){this.bgm=bgm;this.se=se;}
  adoptOrdinaryTitleMusic(music){calls.push('adopt-music');return {source:music.source,track:music.track,buffer:music.buffer};}
  dispose(){this.disposed=true;}
 }
 const context={state,menu,DOMException,OwnedMenuSoundLedger:class{close(){}},
  OriginalTitleAudio:AudioBoundary,TitleSoundWindowConsumer:class{},
  OriginalMainSettingsStore:{defaults:()=>({bgm:100,se:80})},
  safeAssetUrl(){throw Error('no asset lookup is expected before AudioBoundary.load');},
  readJson:async()=>({}),stopTitleSource(){},startTitleSource(){},
  originalMainController:()=>({adoptHeld:held=>calls.push(['keys',...held])}),
  clock:{reset(){}},resetHighFrameratePresentation(){},syncNativeMenuState(){calls.push('sync-title');},status(){}};
 vm.createContext(context);vm.runInContext(body,context);
 return{state,menu,ensure:context.ensureOriginalTitleAudio,advancePostrun:context.advanceNormalPostrun,resolveLoad,created:()=>created,calls};
}
for(const unavailable of ['locked','suspended']){
 test('an '+unavailable+' title does not retain a resolved-null loading promise',async()=>{
  const env=environment();env.menu.audioUnlocked=unavailable!=='locked';
  if(unavailable==='suspended')env.state.audio.state='suspended';
  assert.equal(await env.ensure(),null);
  assert.equal(env.state.mainAudioLoading,null,'a skipped load is not an in-flight audio operation');
  assert.equal(env.created(),0);
 });
}
test('arrow navigation before first confirmation cannot poison later audio initialization',async()=>{
 const env=environment();
 assert.equal(await env.ensure(),null);
 env.menu.audioUnlocked=true;
 const loading=env.ensure();env.resolveLoad();
 const audio=await loading;
 assert(audio,'audio must initialize after the real unlock boundary becomes ready');
 assert.equal(env.state.titleAudio,audio);assert.equal(env.state.mainAudioLoading,null);
 assert.equal(audio.bgm,85);assert.equal(audio.se,70);assert.equal(env.created(),1);
});
test('concurrent ready requests share one genuine load and release its cache',async()=>{
 const env=environment();env.menu.audioUnlocked=true;
 const first=env.ensure(),second=env.ensure();
 assert.equal(env.created(),1);assert(env.state.mainAudioLoading);
 env.resolveLoad();
 const [a,b]=await Promise.all([first,second]);
 assert.equal(a,b);assert.equal(env.state.titleAudio,a);assert.equal(env.state.mainAudioLoading,null);
});
for(const audioState of ['running','suspended']){
 test('return after Replay save transfers the title on a '+audioState+' audio context even before title unlock',async()=>{
  const env=environment(),music={source:{},track:{name:'th128_07.wav'},buffer:{}},generation=31;
  Object.assign(env.state,{worldMode:true,shotMode:true,paused:false,pressed:new Set(['KeyX'])});
  env.state.audio.state=audioState;
  env.state.audio.resume=()=>assert.fail('postrun handoff must not force playback');
  env.state.normalPostrun={
   async step(){env.calls.push('replay-saved');return{generation,menu:{mainHandoffReady:true}};},
   async finishMain(){env.calls.push('commit-main');return{nativeCommitted:true,generation};},
   detachMainMusic(){env.calls.push('detach-music');return music;},
   close(){env.calls.push('close-postrun');}
  };
  env.resolveLoad();await env.advancePostrun();
  assert.deepEqual(env.calls,['replay-saved','commit-main','detach-music','adopt-music',['keys','KeyX'],'close-postrun','sync-title']);
  assert.equal(env.state.normalPostrun,null);assert.equal(env.state.worldMode,false);assert.equal(env.state.active,true);
  assert.equal(env.state.source,music.source);assert.equal(env.state.ordinaryMainMusic,music);
  assert.equal(env.state.audio.state,audioState);assert.equal(env.menu.page,'main');
 });
}
