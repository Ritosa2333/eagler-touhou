import {ORIGINAL_BGM,AKYU_BGM,ORIGINAL_SFX,musicFileName} from './audio-layout.mjs';
import {createSfx} from './sfx.mjs';
import {digest} from './data.mjs';
// BGM and FM effects are separate owners. Pausing or disabling music never
// suspends the effect part; only visibility, Replay transport and retirement do.
export function createAudio(module,emit,environment=globalThis){
 let mode='none',decodeMode='stream',musicSource='original',context,sfx,element,source,gain,elementURL,loopTimer;
 let selected='',track='',generation=0,volume=1,rate=1,paused=false,hidden=false,gated=false,closed=false,position=0,startedAt=0,currentDefinition;
 const cache=new Map(),missing=new Set();
 const read=path=>{try{return module.FS.readFile(path);}catch{return null;}};
 const ctx=()=>context??=new environment.AudioContext({latencyHint:'interactive'});
 function report(error){if(!closed)emit('music-error',{failed:1,message:error.message});}
 function definition(name){const base=name.replace(/\.(?:m2|m)$/i,'').toLowerCase();if(base==='logo')return null;const tracks=musicSource==='akyu'?AKYU_BGM.tracks:ORIGINAL_BGM.tracks;const value=tracks[base+'-m86']??tracks[base+'-m26'];return value?{...value,path:'/music/'+musicFileName(value.url)}:null;}
 function stop(remember=false){
  if(remember&&source){position+=(ctx().currentTime-startedAt)*rate;const end=currentDefinition?.loopEnd??source.buffer.duration,start=currentDefinition?.loopStart??0;if(end>start&&position>=end)position=start+(position-end)%(end-start);}
  generation++;source?.stop();source=null;clearInterval(loopTimer);loopTimer=null;
  if(element){if(remember)position=element.currentTime;element.pause();element.src='';element=null;}
  if(elementURL){URL.revokeObjectURL(elementURL);elementURL=null;}
 }
 async function buffer(path){let pending=cache.get(path);if(!pending){const bytes=read(path);if(!bytes)throw Error('Missing BGM: '+path);pending=ctx().decodeAudioData(bytes.buffer.slice(bytes.byteOffset,bytes.byteOffset+bytes.byteLength));cache.set(path,pending);pending.catch(()=>cache.delete(path));while(cache.size>3){const key=cache.keys().next().value;if(key===path)break;cache.delete(key);}}return pending;}
 async function play(restart){
  if(closed||mode==='none'||!selected||paused||hidden||gated)return;
  if(!restart&&(source||element)){if(element){const item=element;await item.play().catch(error=>{if(item===element&&!paused&&!hidden&&!gated)report(error);});}return;}
  stop(!restart);if(restart)position=0;track=selected;const def=definition(track);currentDefinition=def;if(!def)return;
  const serial=generation,bytes=read(def.path);if(!bytes){if(!missing.has(def.path)){missing.add(def.path);emit('music-incomplete',{failed:1,path:def.path});}return;}
  if(bytes.length!==def.bytes||await digest(bytes)!==def.audioSha256)throw Error('PR #143 BGM identity mismatch: '+def.path);
  if(serial!==generation||closed)return;
  if(decodeMode==='full'){
   const audio=await buffer(def.path);if(serial!==generation||closed||hidden||gated||paused)return;
   source=ctx().createBufferSource();source.buffer=audio;source.loop=true;source.loopStart=def.fullLoop?0:def.loopStart;source.loopEnd=def.fullLoop?audio.duration:Math.min(def.loopEnd,audio.duration);source.playbackRate.value=rate;gain=ctx().createGain();gain.gain.value=volume;source.connect(gain).connect(ctx().destination);startedAt=ctx().currentTime;source.start(0,Math.min(position,audio.duration));void ctx().resume().catch(report);
  }else{
   elementURL=URL.createObjectURL(new Blob([bytes],{type:'audio/ogg; codecs=opus'}));element=new environment.Audio(elementURL);element.preload='auto';element.volume=volume;element.playbackRate=rate;element.currentTime=position;element.loop=!!def.fullLoop;
   const item=element;const failure=error=>{if(item===element&&serial===generation&&!closed&&!paused&&!hidden&&!gated)report(error);};const loop=()=>{if(item===element&&def.loopEnd>def.loopStart&&element.currentTime>=def.loopEnd)element.currentTime=def.loopStart+(element.currentTime-def.loopEnd);};
   element.addEventListener('timeupdate',loop);element.addEventListener('ended',()=>{if(item===element&&!closed&&!paused&&!hidden&&!gated){element.currentTime=def.loopStart;void element.play().catch(failure);}});element.addEventListener('error',()=>failure(Error('BGM decode failed: '+def.path)));if(!def.fullLoop)loopTimer=setInterval(loop,15);await element.play().catch(failure);
  }
 }
 async function initialize(){if(sfx||!environment.AudioContext)return;const bytes=read('/data/sfx/effects.pcm');if(!bytes||await digest(bytes)!==ORIGINAL_SFX.sha256)throw Error('Install the verified PR #143 FM effect bank');sfx=createSfx(ctx(),bytes);}
 function resumeContext(){if(context&&!hidden&&!gated)void context.resume().catch(report);}
 return {
  initialize,
  async configure(music,options){stop();mode=music;decodeMode=options.oggDecodeMode;musicSource=options.musicSource??'original';await initialize();},
  select(name){selected=name;},
  action(action,value){
   if(action===0){stop();position=0;}
   else if(action===1){paused=false;void play(true).catch(report);}
   else if(action===2){paused=true;stop(true);}
   else if(action===3){paused=false;void play(false).catch(report);}
   else if(action===4){rate=Math.max(.1,Math.min(8,value));if(element)element.playbackRate=rate;if(source)source.playbackRate.value=rate;}
   else if(action===5){volume=Math.max(0,Math.min(1,value));if(element)element.volume=volume;if(gain)gain.gain.value=volume;}
  },
  sound(index){if(!closed&&!hidden&&!gated)sfx?.trigger(index);},
  pump(){sfx?.pump();},
  playback(value){if(gated===value)return;gated=value;if(value){stop(true);sfx?.clear();}else{resumeContext();void play(false).catch(report);}},
  visibility(value){if(hidden===value)return;hidden=value;if(hidden){stop(true);sfx?.clear();if(context)void context.suspend();}else{resumeContext();void play(false).catch(report);}},
  resume(){resumeContext();if(element&&!paused&&!hidden&&!gated)void element.play().catch(report);},
  resourceChanged(path){if(path===currentDefinition?.path&&!source&&!element&&!paused){missing.delete(path);void play(false).catch(report);}},
  health(){return {backend:'uth05-browser-audio',robust:context?.state==='running'||!context,mode,decodeMode,musicSource,track,path:currentDefinition?.path??null,playing:!!source||!!element&&!element.paused,loopStart:currentDefinition?.loopStart??0,loopEnd:currentDefinition?.loopEnd??0,queuedMs:0,underruns:0,sfx:sfx?.info()??null};},
  close(){closed=true;stop();sfx?.close();cache.clear();if(context)void context.close();},
 };
}
