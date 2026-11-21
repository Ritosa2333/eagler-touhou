import fs from 'node:fs';
import vm from 'node:vm';
import assert from 'node:assert/strict';
import test from 'node:test';
const source=fs.readFileSync(process.env.TH128_PLAY_SOURCE??new URL('../site/play.mjs',import.meta.url),'utf8');
const match=/^async function ensureOriginalTitleAudio\(/m.exec(source);
assert(match,'production audio loader is present');
const body=source.slice(match.index,source.indexOf('\n}',match.index)+2);
function environment(){
 let created=0;
 const state={module:{},controller:new AbortController(),audio:{state:'running'},gain:{},base:'/owned/',
  manifest:{music:[]},mainAudioLoading:null,mainSavedSettings:{bgm:85,se:70}};
 const menu={audioUnlocked:false};
 let resolveLoad;
 const loadBoundary=new Promise(resolve=>{resolveLoad=resolve;});
 class AudioBoundary{
  constructor(options){Object.assign(this,options);this.sounds={};this.bgmGain={};this.disposed=false;created++;}
  async load(){await loadBoundary;}
  setVolumes(bgm,se){this.bgm=bgm;this.se=se;}
  dispose(){this.disposed=true;}
 }
 const context={state,menu,DOMException,OwnedMenuSoundLedger:class{close(){}},
  OriginalTitleAudio:AudioBoundary,TitleSoundWindowConsumer:class{},
  OriginalMainSettingsStore:{defaults:()=>({bgm:100,se:80})},
  safeAssetUrl(){throw Error('no asset lookup is expected before AudioBoundary.load');},
  readJson:async()=>({}),stopTitleSource(){},startTitleSource(){}};
 vm.createContext(context);vm.runInContext(body,context);
 return{state,menu,ensure:context.ensureOriginalTitleAudio,resolveLoad,created:()=>created};
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
