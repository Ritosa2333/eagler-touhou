import test from'node:test';import assert from'node:assert/strict';import fs from'node:fs';
import{StageMusic,OriginalTitleAudio,GameSoundQueue}from'../../site/runtime/sound.mjs';
import{createPageLaunchContext,installPageLaunchContext}from'../../site/runtime/page-launch-context.mjs';
import{graph,track,buffer}from'./graph-fixture.mjs';
let configReads=0;
const launchOwner=createPageLaunchContext({config:{music:'none',resources:Array.from({length:16},(_,i)=>({path:'fixture/'+i,url:'blob:authored-'+i,bytes:1024}))}});
installPageLaunchContext(new Proxy(launchOwner,{get(target,key,receiver){if(key==='config')configReads++;return Reflect.get(target,key,receiver);}}));
const policyURL=new URL('../../site/runtime/music-output.mjs',import.meta.url),policy=fs.existsSync(policyURL)?await import(policyURL):{};
const play=fs.readFileSync(new URL('../../site/play.mjs',import.meta.url),'utf8').replace(/\r\n/g,'\n');
function load(name,next,env){
 const a=play.indexOf('function '+name+'('),start=play.slice(Math.max(0,a-6),a)==='async '?a-6:a,b=play.indexOf('function '+next+'(',a+1),end=play.slice(Math.max(0,b-6),b)==='async '?b-6:b;
 assert(a>=0&&end>start);return new Function(...Object.keys(env),play.slice(start,end)+';return '+name+';')(...Object.values(env));
}
function stage(){const g=graph(),music=new StageMusic(g.context,g.master);music.track={...track};music.buffer=buffer;music.preloaded.set(track.name,{track:music.track,buffer});return{...g,music};}
function title(g){return new OriginalTitleAudio({context:g.context,gain:g.master,base:'/',tracks:[track],resolveUrl:()=>'',signal:new AbortController().signal,onStopTitle(){},onStartTitle(){throw Error('not a restart test');},onPreview:async()=>{},bgmLedger:{async commit(){throw Error('no receipt write in graph routing test');}}});}
test('session none creates one private zero BGM destination while OGG passes through and bad modes fail',()=>{
 assert.equal(typeof policy.musicOutput,'function','BGM output policy is absent');assert.equal(typeof policy.musicOutputEnabled,'function');
 const g=graph(),count=g.nodes.length;assert.equal(policy.musicOutput(g.context,g.master,'ogg'),g.master);assert.equal(g.nodes.length,count);
 const gate=policy.musicOutput(g.context,g.master);assert.notEqual(gate,g.master);assert.equal(gate.gain.value,0);assert.equal(policy.musicOutput(g.context,g.master),gate);assert.equal(g.nodes.length,count+1);
 assert.equal(g.master.gain.value,.95);assert.equal(policy.musicOutputEnabled(),false);assert.equal(policy.musicOutputEnabled('ogg'),true);assert.throws(()=>policy.musicOutput(g.context,g.master,'midi'),/music|mode/i);
});
test('real StageMusic is muted downstream while the actual SFX queue still emits',()=>{
 const g=stage();g.music.start();assert.equal(g.level(g.music.source),0,'StageMusic BGM leaks in none mode');
 const sounds=new GameSoundQueue(g.context,g.master);sounds.buffers.set(7,{buffer,volumeRaw:0,loop:false});
 assert.equal(sounds.consumeNativeRequest(7,0,0),true);assert.equal(sounds.flush(),1);assert(g.level(g.sources.at(-1))>0,'SFX was muted with BGM');assert.equal(g.master.gain.value,.95);
 sounds.dispose();g.music.dispose();
});
test('actual fade, stop, switch and disposal preserve the zero downstream gate',()=>{
 const g=stage();g.music.start();const first=g.music.source;g.music.fadeOutFrames(180);assert.equal(first.stopAt,3);assert.equal(g.level(first),0);
 g.music.switchTo(track.name);assert.equal(g.level(g.music.source),0);g.music.cancelFade();assert.equal(g.level(g.music.source),0);g.music.dispose();assert.equal(g.music.source,null);assert.equal(g.level(first),0);
});
test('OriginalTitleAudio saved BGM volume changes cannot reopen the disabled output',()=>{
 const g=graph(),audio=title(g);audio.music.track={...track};audio.music.buffer=buffer;audio.music.start();audio.setVolumes(100,80);
 assert.equal(g.level(audio.music.source),0,'original title music bypasses session gate');assert.equal(g.master.gain.value,.95);audio.setVolumes(0,100);assert.equal(g.level(audio.music.source),0);audio.dispose();
});
test('ordinary sealed Music7 adoption keeps the same source muted after reconnecting to title volume',()=>{
 const g=stage();g.music.start();const source=g.music.source,audio=title(g),value=audio.adoptOrdinaryTitleMusic(g.music);
 assert.equal(value.source,source);assert.equal(g.sources.length,1);audio.setVolumes(100,100);assert.equal(g.level(source),0,'Music7 adoption bypasses disabled BGM');audio.dispose();
});
test('actual legacy title source connects to the music policy, not the SFX destination',()=>{
 const g=graph(),state={audio:g.context,gain:g.master,buffer,track:{...track},source:null,ordinaryMainMusic:null};
 const run=load('startTitleSource','stopMenuMusic',{state,stopTitleSource(){},recordOwnedHeardTrack:async()=>{},error:assert.fail,musicOutput:policy.musicOutput});
 run();assert(state.source.started);assert.equal(g.level(state.source),0,'legacy title leaks BGM');
});
test('actual legacy music-room path remains muted after resume and uses cached decoded bytes',async()=>{
 const g=graph(),menu={musicRequestId:0},menuMusic={cache:new Map([[track.file,buffer]])},state={audio:g.context,gain:g.master,controller:new AbortController()};
 const run=load('playMenuTrack','showMusicPage',{menu,menuMusic,state,unlockMenuAudio:async()=>true,stopTitleSource(){},stopMenuMusic(){},updateMenuCopy(){},populateMusicRows(){},updateMenuSelection(){},checkpointFetch(){assert.fail('cached track should not fetch');},safeAssetUrl(){},musicOutput:policy.musicOutput,musicOutputEnabled:policy.musicOutputEnabled});
 await run(track);assert(menuMusic.source?.started,'current music-room source was not created');assert.equal(g.level(menuMusic.source),0,'legacy music room leaks BGM');
});
test('actual PCM ending output allocation remains muted through its own fade gain',()=>{
 const g=graph(),source=fs.readFileSync(new URL('../../site/runtime/native-ending-browser-host-v4.mjs',import.meta.url),'utf8').replace(/\r\n/g,'\n');
 const a=source.indexOf('output=audio.createGain();'),b=source.indexOf('\n    }catch(error){',a);assert(a>=0&&b>a);
 const output=new Function('audio','gain','isCurrent','musicOutput','let output;'+source.slice(a,b)+';return output;')(g.context,g.master,()=>true,policy.musicOutput);
 const signal=g.context.createBufferSource();signal.connect(output);output.gain.setValueAtTime(1,0);assert.equal(g.level(signal),0,'ending PCM leaks BGM');output.gain.setValueAtTime(.5,1);assert.equal(g.level(signal),0);output.disconnect();
});
test('silent output does not produce a Web heard-track settings write',async()=>{
 let writes=0;const state={audio:{state:'running'},worldMode:true,replayPlayback:null};
 const run=load('recordOwnedHeardTrack','recordOwnedMainSession',{state,Promise,updateOwnedMainSettings(){writes++;return Promise.resolve({seenMask:1});},nativeMenuSnapshot(){assert.fail('not a menu test');},musicOutputEnabled:policy.musicOutputEnabled});
 await run('th128_07.wav');assert.equal(writes,0,'silent BGM was marked heard by Web settings helper');
});
test('repeated input and BGM policy checks do not clone the complete resource configuration again',()=>{
 policy.musicOutputEnabled();const before=configReads;
 for(let i=0;i<1000;i++)assert.equal(policy.musicOutputEnabled(),false);
 assert.equal(configReads-before,0,'immutable launch configuration was cloned again on repeated policy reads');
});
