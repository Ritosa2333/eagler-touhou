// Complete one-shot buffers run on the audio device clock, never network PCM.
import {assetBytes} from '../preload-cache.js';
export class LocalSfx {
 constructor(context){
  this.context=context;this.buffers=new Map();this.active=null;this.closed=false;
  this.played=0;this.silent=0;this.dropped=0;this.lastId=null;
  this.abort=new AbortController();
  this.volume=context.createGain();this.volume.gain.value=1;this.volume.connect(context.destination);
  this.onState=()=>{if(context.state!=='running')this.stop();};context.addEventListener('statechange',this.onState);
 }
 async load(){
  const response=await fetch(new URL('../sfx/manifest.json',import.meta.url),{cache:'no-store',signal:this.abort.signal});
  if(!response.ok)throw Error('音效清单读取失败');const manifest=await response.json();
  if(manifest.game!=='th02'||manifest.sampleRate!==44100||manifest.channels!==1||Object.keys(manifest.effects).length!==19)throw Error('TH02 音效清单无效');
  this.manifest=manifest;
  // Small lossless bank: download/decode once before ready, no request on fire.
  let next=0;const entries=Object.entries(manifest.effects);
  await Promise.all(Array.from({length:3},async()=>{while(next<entries.length){
   const [id,item]=entries[next++],bytes=await assetBytes(`sfx/${item.file}`,{hash:item.sha256,signal:this.abort.signal});
   const buffer=await this.context.decodeAudioData(bytes.buffer);
   if(buffer.numberOfChannels!==1||Math.abs(buffer.duration-item.frames/44100)>.001)throw Error(`音效长度无效：${id}`);
   if(!this.closed)this.buffers.set(Number(id),buffer);
  }}));
 }
 play(id){
  if(this.closed)return;const item=this.manifest?.effects[id],buffer=this.buffers.get(id);
  if(!item||!buffer)throw Error(`未知 TH02 音效：${id}`);
  if(this.context.state!=='running'){this.dropped++;return;}
  // PMD uses a single SE voice: a new accepted event replaces the old sound.
  this.stop();if(item.silent){this.silent++;return;}
  const source=this.context.createBufferSource(),gain=this.context.createGain(),now=this.context.currentTime;
  source.buffer=buffer;source.connect(gain);gain.connect(this.volume);
  const voice={source,gain};this.active=voice;
  source.onended=()=>{source.disconnect();gain.disconnect();if(this.active===voice)this.active=null;};
  source.start(now);this.played++;this.lastId=id;
 }
 stop(){
  const voice=this.active;if(!voice)return;this.active=null;
  const now=this.context.currentTime;
  try{voice.gain.gain.cancelScheduledValues(now);voice.gain.gain.setValueAtTime(1,now);voice.gain.gain.linearRampToValueAtTime(0,now+.003);voice.source.stop(now+.003);}catch{}
 }
 info(){return {mode:'browser-one-shot',ready:this.buffers.size===19,played:this.played,silent:this.silent,dropped:this.dropped,lastId:this.lastId,active:!!this.active};}
 dispose(){this.closed=true;this.abort.abort();this.stop();this.buffers.clear();this.volume.disconnect();this.context.removeEventListener('statechange',this.onState);}
}
