import {bgmVersion,onBgmVersion,resolveBgmTrack,loadMidiTracks} from '../bgm-version.js';
import {assetBytes} from '../preload-cache.js';
// Local presentation only. The game emits commands; the audio device owns time.
// Never seek to a network tick, refill PCM per frame, or write guest memory here.
export class LocalBgm {
  constructor(context,report){
    this.context=context;this.report=report;this.newTracks={};
    this.unsubscribeVersion=onBgmVersion(()=>{
      if(this.closed||this.lastHash===undefined||!this.manifest)return;
      const previous=this.requested,hash=this.lastHash;
      this.play(hash);
      if(previous&&this.requested?.generation===this.generation){
        this.requested.volume=previous.volume;this.requested.fade=previous.fade;
      }
    });this.encoded=new Map();this.decoded=new Map();
    this.downloads=new Map();this.pendingCommands=[];this.abort=new AbortController();
    this.generation=0;this.sequence=0;this.mailbox=-1;this.active=null;this.closed=false;
    this.status='本地音乐：准备中';this.volume=context.createGain();
    this.volume.gain.value=0.7;this.volume.connect(context.destination);
  }
  async load(config){
    const response=await fetch(new URL('../bgm/manifest.json',import.meta.url),
      {cache:'no-store',signal:AbortSignal.any([this.abort.signal,AbortSignal.timeout(15000)])});
    if(!response.ok)throw Error('本地音乐清单读取失败');
    const manifest=await response.json();
    const alternate=await fetch(new URL('../bgm/akyu/manifest.json',import.meta.url),{signal:this.abort.signal});
    if(!alternate.ok)throw Error('新版音乐清单读取失败');
    this.newTracks=(await alternate.json()).tracks;
    this.midiTracks=await loadMidiTracks(this.abort.signal);
    this.manifest=manifest;
    if(this.closed)return;
    this.status='本地音乐：按需加载';
    const commands=this.pendingCommands.splice(0);
    for(const [ax,hash]of commands)this.command(ax,hash);
    const first=Number(config.difficulty)===4?'st06':'st00';
    // Warm compressed opening tracks without delaying the game or allocating
    // two full decoded waveforms before the driver selects its music mode.
    if(!commands.length&&config.warmOpening!==false)for(const type of ['m26','m86'])this.prefetch(`${first}-${type}`);
  }
  track(id){return resolveBgmTrack(id,this.manifest.tracks,this.newTracks,this.midiTracks);}
  download(id,track=this.track(id)){
    const cacheKey=track?.url;
    if(this.encoded.has(cacheKey))return Promise.resolve(this.encoded.get(cacheKey));
    if(this.downloads.has(cacheKey))return this.downloads.get(cacheKey);
    if(!track)return Promise.reject(Error(`缺少本地曲目：${id}`));
    const pending=(async()=>{
      const bytes=await assetBytes(`bgm/${track.url}`,{hash:track.audioSha256||track.sha256,size:track.bytes,
        signal:AbortSignal.any([this.abort.signal,AbortSignal.timeout(30000)])});
      if(!this.closed)this.encoded.set(cacheKey,bytes.buffer);
      return bytes.buffer;
    })();
    this.downloads.set(cacheKey,pending);
    pending.then(()=>this.downloads.delete(cacheKey),()=>this.downloads.delete(cacheKey));
    return pending;
  }
  prefetch(id){
    if(!this.closed&&this.manifest.tracks[id])this.download(id).catch(()=>{});
  }
  decode(id,track=this.track(id)){
    const cacheKey=track?.url;
    if(!this.manifest.tracks[id])return Promise.reject(Error(`缺少本地曲目：${id}`));
    if(!this.decoded.has(cacheKey)){
      const pending=this.download(id,track).then(bytes=>{
        if(this.closed)throw Error('房间已关闭');
        return this.context.decodeAudioData(bytes.slice(0));
      });
      this.decoded.set(cacheKey,pending);
      pending.catch(()=>{if(this.decoded.get(cacheKey)===pending)this.decoded.delete(cacheKey);});
    }
    return this.decoded.get(cacheKey);
  }
  setVolume(value){this.volume.gain.setTargetAtTime(value,this.context.currentTime,0.015);}
  retire(){
    const item=this.active;if(!item)return;
    const now=this.context.currentTime;
    item.gain.gain.cancelAndHoldAtTime(now);
    item.gain.gain.linearRampToValueAtTime(0,now+0.015);
    item.source.stop(now+0.016);this.active=null;
  }
  stop(){this.lastHash=undefined;this.requested=null;this.generation++;this.retire();this.status='本地音乐：已停止';}
  play(hash){
    this.lastHash=hash;
    const id=this.manifest.headers[String(hash)];
    this.retire();const generation=++this.generation;
    if(!id){this.lastHash=undefined;this.requested=null;this.status=`本地音乐：未识别曲目 ${hash.toString(16)}`;return;}
    // Commands arriving while decoding belong to this generation only.
    const item={id,generation,source:null,gain:null,volume:1,fade:null};
    this.requested=item;this.status=`本地音乐：正在加载 ${id}（不影响游戏）`;
    const track=this.track(id);
    this.decode(id,track).then(buffer=>{
      if(this.closed||generation!==this.generation)return;
      const source=this.context.createBufferSource(),gain=this.context.createGain();
      source.buffer=buffer;
      const loopEnd=track.fullLoop?buffer.duration:track.loopEnd;
      if(loopEnd>track.loopStart){
        if(loopEnd>buffer.duration+0.025)throw Error(`本地曲目长度不足：${id}`);
        source.loop=true;source.loopStart=track.loopStart;source.loopEnd=Math.min(loopEnd,buffer.duration);
      }
      gain.gain.value=item.volume;source.connect(gain);gain.connect(this.volume);
      item.source=source;item.gain=gain;this.active=item;
      source.onended=()=>{source.disconnect();gain.disconnect();if(this.active===item)this.active=null;};
      source.start();if(item.fade)this.fade(item.fade);
      this.status=`本地音乐：${id}`;
      // Fetch likely next songs while the current source plays independently.
      const stage=/^st(\d\d)([bc]?)-(m26|m86)$/.exec(id);
      if(stage){
        if(!stage[2])this.prefetch(`st${stage[1]}b-${stage[3]}`);
        const next=Number(stage[1])+1;
        if(next<=6)this.prefetch(`st${String(next).padStart(2,'0')}-${stage[3]}`);
      }
      const th02=/^stage([0-5])-m86$/.exec(id);
      if(th02){const n=Number(th02[1]);this.prefetch(`${n===4?'mima':`boss${n===5?5:n+1}`}-m86`);if(n<4)this.prefetch(`stage${n+1}-m86`);}
      // Only keep the current decoded waveform. Compressed tracks stay local.
      for(const key of this.decoded.keys())if(key!==track.url)this.decoded.delete(key);
    }).catch(error=>{if(generation===this.generation)this.status=`本地音乐错误：${error.message}`;});
  }
  fade(speed){
    const item=this.requested;if(!item||item.generation!==this.generation)return;
    item.fade=speed;
    if(!item.gain)return;
    const now=this.context.currentTime;
    item.gain.gain.cancelAndHoldAtTime(now);
    if(!speed)return;
    // PMD changes fade level every eight 9.216 ms Timer A interrupts. Use local
    // audio time for the envelope; a stalled network must not stall the fade.
    const signed=speed<128?speed:speed-256;
    const target=signed>0?0:1;
    const duration=Math.max(0.02,Math.abs(target-item.gain.gain.value)*256*0.073728/Math.abs(signed));
    item.gain.gain.linearRampToValueAtTime(target,now+duration);item.volume=target;
  }
  commandTrack(ax,name){
    const id=this.manifest.tracks[`${name}-m86`]?`${name}-m86`:`${name}-m26`;
    const entry=Object.entries(this.manifest.headers).find(([,track])=>track===id);
    if((ax>>>8)===0&&!entry){this.status=`浏览器音乐：缺少曲目 ${name}`;return;}
    this.command(ax,entry?Number(entry[0]):0);
  }
  command(ax,hash){
    if(!this.manifest){
      this.pendingCommands.push([ax,hash]);
      if(this.pendingCommands.length>64)this.pendingCommands.shift();
      return;
    }
    const ah=ax>>>8,al=ax&255;
    if(ah===0)this.play(hash);
    else if(ah===1)this.stop();
    else if(ah===2)this.fade(al);
    else if(ah===0x19){
      const item=this.requested;
      if(item?.generation===this.generation){
        item.volume=(255-al)/255;item.fade=null;
        if(item.gain){item.gain.gain.cancelScheduledValues(this.context.currentTime);item.gain.gain.setValueAtTime(item.volume,this.context.currentTime);}
      }
    }
    // PMD pause commands and network waiting intentionally do not pause BGM.
  }
  poll(heap,base,patch){
    if(this.closed)return;
    const view=new DataView(heap.buffer);
    const segment=view.getUint16(base+0x60*4+2,true);
    const at=base+segment*16+patch.mailbox_com_offset;
    const signature=patch.signature;
    if(at<base||at+20+64*8>base+0xa0000)return;
    for(let i=0;i<signature.length;i++)if(heap[at+i]!==signature.charCodeAt(i))return;
    if(view.getUint16(at+16,true)!==segment)return;
    const latest=view.getUint16(at+18,true);
    let count=(latest-this.sequence)&65535;
    if(count>64){this.sequence=(latest-64)&65535;count=64;}
    for(let i=0;i<count;i++){
      const expected=(this.sequence+1)&65535;
      const event=at+20+(this.sequence&63)*8;
      if(view.getUint16(event,true)!==expected){this.status='本地音乐：事件队列不匹配';return;}
      this.command(view.getUint16(event+2,true),view.getUint32(event+4,true));
      this.sequence=expected;
    }
    this.mailbox=at;
  }
  dispose(){this.unsubscribeVersion();this.closed=true;this.abort.abort();this.stop();this.pendingCommands=[];this.encoded.clear();this.decoded.clear();this.volume.disconnect();}
}
