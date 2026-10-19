import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readdir,readFile,stat} from 'node:fs/promises';
import {isAbsolute,resolve} from 'node:path';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';

export const MUSIC_BROWSER_GAMES=Object.freeze(['th08','th10','th11','th15','th20']);
export function selectedMusicMappingGames(games=[]) {
  return games.filter(game=>game==='th11'||game==='th15');
}
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');

export const MUSIC_PCM_PROBE_SOURCE=`(()=>{
 if(window===window.top||!window.AudioContext||!window.AudioNode)return;
 const probe={contexts:[],taps:[],capture(){const out=[];for(const analyser of this.taps){
  const samples=new Float32Array(analyser.fftSize);analyser.getFloatTimeDomainData(samples);
  let peakAbs=0,squares=0,nonZero=0;for(const value of samples){const absolute=Math.abs(value);if(absolute>peakAbs)peakAbs=absolute;if(value!==0)nonZero++;squares+=value*value;}
  out.push({contextState:analyser.context.state,sampleRate:analyser.context.sampleRate,currentTime:analyser.context.currentTime,
   samples:samples.length,peakAbs,rms:Math.sqrt(squares/samples.length),nonZero});
 }return out;},summary(){return{contexts:this.contexts.map(context=>({state:context.state,sampleRate:context.sampleRate,currentTime:context.currentTime})),tapCount:this.taps.length};}};
 const nativeConnect=AudioNode.prototype.connect,wrapped=new WeakSet();
 AudioNode.prototype.connect=function(destination,...args){
  if(destination===this.context.destination&&!wrapped.has(this)){
   wrapped.add(this);const context=this.context;if(!probe.contexts.includes(context))probe.contexts.push(context);
   const analyser=context.createAnalyser();analyser.fftSize=2048;analyser.smoothingTimeConstant=0;
   const output=Number.isInteger(args[0])?args[0]:0;nativeConnect.call(this,analyser,output,0);nativeConnect.call(analyser,destination);
   probe.taps.push(analyser);return destination;
  }
  return nativeConnect.call(this,destination,...args);
 };
 window.__eaglerMusicPcmProbe=probe;
})();`;

/** Read the Runtime's own exported audio buffers; no track playback is synthesized by the harness. */
export function readNativeAudioState(game) {
  const runtime=window[`__${game}Runtime`],module=runtime?.Module??window.Module;
  const core=runtime?.core??window.core??module;
  const buffer=core?.memory?.buffer??module?.HEAPU8?.buffer??window.HEAPU8?.buffer;
  const exported=name=>{for(const owner of [core,module]){const fn=owner?.[name]??owner?.[`_${name}`];if(typeof fn==='function')return fn.bind(owner);}return null;};
  const words=(name,count)=>{
    const fn=exported(name);if(!fn||!buffer)return null;const pointer=fn();
    if(!Number.isInteger(pointer)||pointer<=0||pointer+count*4>buffer.byteLength)return null;
    return Array.from(new Uint32Array(buffer,pointer,count));
  };
  if(game==='th11')return {game,native:{source:'th11_audio_statistics',words:words('th11_audio_statistics',9)},decoder:null};
  if(game==='th15')return {game,native:{source:'th15_audio_statistics',words:words('th15_audio_statistics',11)},decoder:null};
  const audioWords=words('sdl_audio_stats',12),musicWords=words('sdl_music_stats',6);
  let rms=null;
  const audioPointer=exported('sdl_audio_stats')?.();
  if(Number.isInteger(audioPointer)&&audioPointer>0&&audioPointer+44<=buffer?.byteLength)
    rms=new DataView(buffer).getFloat32(audioPointer+40,true);
  let decoder=null;
  if(musicWords&&game==='th08')decoder={enabled:musicWords[0]!==0,decodeMode:musicWords[1],soundReady:musicWords[2]!==0,
    pending:musicWords[3],bufferCount:musicWords[4],resourceChanges:musicWords[5]};
  else if(musicWords)decoder={enabled:musicWords[0]!==0,decodeMode:musicWords[1],streamCount:musicWords[2],
    waiting:musicWords[3],valid:musicWords[4],fullReady:musicWords[5]};
  return {game,native:{source:'sdl_audio_stats',words:audioWords,rms},decoder:decoder?{source:'sdl_music_stats',words:musicWords,...decoder}:null};
}

/** Validate an explicit, catalog-bound OGG inventory without opening a browser. */
export function validateMusicManifest(manifest,{repoRoot,games,catalog=PRODUCT_GAMES}={}) {
  assert.equal(manifest?.schema,'eagler-touhou/music-inputs/1','Unsupported music input manifest');
  assert.equal(manifest?.resourcePolicy,'private-local-paths','Music inputs must remain local private resources');
  assert.ok(Array.isArray(games)&&games.length>0,'Explicit nonempty OGG game selection required');
  assert.equal(new Set(games).size,games.length,'Music game selection contains duplicates');
  const selected=Object.create(null);
  for(const game of games){
    assert.ok(MUSIC_BROWSER_GAMES.includes(game),`Unsupported music acceptance game ${game}`);
    const contract=catalog[game]?.package;
    const music=manifest.games?.[game];
    assert.ok(contract?.musicSourceDirectories?.ogg&&contract?.musicMounts?.ogg,`${game}: catalog has no OGG source/mount contract`);
    assert.ok(music&&Array.isArray(music.files)&&music.files.length>0,`${game}: OGG resources are unavailable in the manifest`);
    assert.ok(isAbsolute(music.sourceDirectory),`${game}: source directory must be an explicit absolute local path`);
    const expectedSuffix=['games','web-content',game,...contract.musicSourceDirectories.ogg.split(/[\\/]+/).filter(Boolean)];
    const sourceDirectory=resolve(music.sourceDirectory),sourceParts=sourceDirectory.split(/[\\/]+/).filter(Boolean);
    assert.ok(sourceParts.length>=expectedSuffix.length&&sourceParts.slice(-expectedSuffix.length).join('/').toLowerCase()===expectedSuffix.join('/').toLowerCase(),
      `${game}: source directory differs from product catalog`);
    assert.equal(music.mount,contract.musicMounts.ogg,`${game}: mount differs from product catalog`);
    const names=new Set();
    for(const file of music.files){
      assert.ok(file&&typeof file==='object'&&!Array.isArray(file),`${game}: malformed OGG file entry`);
      assert.match(file.name,/^[A-Za-z0-9_.-]+\.ogg$/i,`${game}: OGG resource must use a safe basename`);
      assert.ok(isAbsolute(file.path),`${game}/${file.name}: OGG path must be an explicit absolute local path`);
      assert.ok(!names.has(file.name),`${game}: duplicate OGG resource ${file.name}`);names.add(file.name);
      assert.equal(resolve(file.path),resolve(sourceDirectory,file.name),`${game}: OGG resource path differs from its catalog source`);
      assert.ok(Number.isSafeInteger(file.bytes)&&file.bytes>0,`${game}/${file.name}: invalid byte count`);
      assert.match(file.sha256,/^[a-f0-9]{64}$/,`${game}/${file.name}: invalid SHA-256`);
    }
    assert.deepEqual(music.files.map(file=>file.name),[...names].sort((a,b)=>a.localeCompare(b)),`${game}: OGG inventory must be sorted by filename`);
    selected[game]={sourceDirectory,mount:music.mount,files:music.files};
  }
  return selected;
}

/** Bind every manifest entry to the current complete source-directory inventory. */
export async function verifyMusicInputs(manifest,{repoRoot,games,catalog=PRODUCT_GAMES}={}) {
  const selected=validateMusicManifest(manifest,{repoRoot,games,catalog}),identities={};
  for(const [game,input] of Object.entries(selected)){
    const actual=(await readdir(input.sourceDirectory,{withFileTypes:true}))
      .filter(entry=>entry.isFile()&&/\.ogg$/i.test(entry.name)).map(entry=>entry.name).sort((a,b)=>a.localeCompare(b));
    assert.deepEqual(input.files.map(file=>file.name),actual,`${game}: OGG source inventory changed or is incomplete`);
    identities[game]={sourceDirectory:input.sourceDirectory,mount:input.mount,files:[]};
    for(const file of input.files){
      const path=resolve(file.path),info=await stat(path),bytes=await readFile(path);
      assert.ok(info.isFile(),`${game}/${file.name}: OGG resource must be a regular file`);
      assert.equal(bytes.length,file.bytes,`${game}/${file.name}: OGG byte count changed`);
      assert.equal(digest(bytes),file.sha256,`${game}/${file.name}: OGG SHA-256 changed`);
      identities[game].files.push({name:file.name,path,bytes:file.bytes,sha256:file.sha256});
    }
  }
  return identities;
}

export function runtimeMusicResources(game,identity,{base}) {
  assert.ok(base,'Runtime fixture base URL is required');
  const input=identity?.[game];assert.ok(input?.mount&&Array.isArray(input.files)&&input.files.length>0,`${game}: verified OGG inputs are required`);
  return input.files.map(file=>({path:`${input.mount}/${file.name}`,url:`${base}/music/${game}/${file.name}`,bytes:file.bytes}));
}

/** Interpret only native fields whose exact ABI width and index are source-verified. */
export function describeNativeMusic(game,raw,{th11Tracks={},th15Tracks=[]}={}) {
  if(game==='th11'){
    assert.equal(raw?.native?.source,'th11_audio_statistics');
    assert.equal(raw.native.words?.length,9,'TH11 audio statistics ABI contains exactly nine words');
    const cue=raw.native.words[5]===0xffffffff?-1:raw.native.words[5];
    const stem=cue<0?null:th11Tracks[String(cue)]??null;
    return {nativeSource:raw.native.source,wordCount:9,trackIndex:cue,trackFile:stem?`${stem}.ogg`:null,
      cursor:null,paused:raw.native.words[8]!==0,nativePcmRms:null,decoder:raw.decoder??null};
  }
  if(game==='th15'){
    assert.equal(raw?.native?.source,'th15_audio_statistics');
    assert.equal(raw.native.words?.length,11,'TH15 audio statistics ABI contains exactly eleven words');
    const track=raw.native.words[5]===0xffffffff?-1:raw.native.words[5];
    return {nativeSource:raw.native.source,wordCount:11,trackIndex:track,
      trackFile:track<0?null:th15Tracks[track]??null,cursor:raw.native.words[9],paused:raw.native.words[8]!==0,
      nativePcmRms:null,decoder:raw.decoder??null};
  }
  assert.ok(['th08','th10','th20'].includes(game),`Unsupported native music ABI ${game}`);
  return {nativeSource:raw?.native?.source??null,wordCount:raw?.native?.words?.length??0,trackIndex:null,trackFile:null,
    cursor:null,paused:null,nativePcmRms:raw?.native?.rms??null,decoder:raw?.decoder??null,
    identityStatus:'partial-no-proven-track-id'};
}

export function summarizeMusicPcm(captures=[]) {
  const usable=captures.filter(item=>item&&typeof item==='object');
  return {captureCount:usable.length,contexts:[...new Set(usable.map(item=>item.contextState).filter(Boolean))],
    sampleRate:usable.find(item=>Number.isFinite(item.sampleRate))?.sampleRate??null,
    peakAbs:usable.reduce((max,item)=>Math.max(max,Number(item.peakAbs)||0),0),
    maxRms:usable.reduce((max,item)=>Math.max(max,Number(item.rms)||0),0),
    nonZeroSamples:usable.reduce((sum,item)=>sum+(Number(item.nonZero)||0),0),
    outputObserved:usable.some(item=>Number(item.nonZero)>0)};
}

export function describeNoneMusicControl(pcm,{settleMs=1800,captureWindowMs=600}={}) {
  const running=pcm?.captureCount>0&&pcm.contexts?.includes('running');
  const silent=running&&pcm.nonZeroSamples===0;
  return {status:silent?'silent-after-settling':pcm?.outputObserved?'output-observed-unclassified':'no-running-output-tap',
    settleMs,captureWindowMs,samples:Number(pcm?.captureCount)||0,contextStates:pcm?.contexts??[]};
}

export function nativeAudioStatsShape(game) {
  if(game==='th11')return {source:'th11_audio_statistics',wordCount:9,trackIndex:5,paused:8,cursor:null};
  if(game==='th15')return {source:'th15_audio_statistics',wordCount:11,trackIndex:5,paused:8,cursor:9};
  assert.ok(['th08','th10','th20'].includes(game));
  return {source:'sdl_audio_stats + sdl_music_stats',audioWordCount:12,musicWordCount:6,trackIndex:null,cursor:null};
}

export function mapTrackToInput(trackFile,files=[]) {
  if(!trackFile)return null;
  const input=files.find(file=>file.name===trackFile);
  return input?{name:input.name,path:input.path,bytes:input.bytes,sha256:input.sha256}:null;
}
