import {onBgmVersion} from '../bgm-version.js';
import {loadMusicTrack} from '../bgm-track.js';
import {musicRoomTracks} from '../music-room.js';
import {assetBytes,freshJSON} from '../preload-cache.js';
let effectsManifest;

// Sound is local presentation; downloads and audio clocks never gate a game step.
export class LocalAudio{
  constructor(){this.unsubscribeVersion=onBgmVersion(()=>{++this.trackRequest;this.source?.stop();this.source?.disconnect();this.source=null;this.trackKey=null;this.buffers.clear();if(this.core)this.update(this.core);});this.volume=.35;this.buffers=new Map();this.effects=new Map();this.cursor=0;this.stopped=false;this.trackRequest=0;}
  async unlock(){
    if(this.stopped)return;
    this.context??=new AudioContext();this.gain??=this.context.createGain();this.gain.gain.value=this.volume;
    if(!this.connected){this.gain.connect(this.context.destination);this.connected=true;}
    await this.context.resume();this.onStatus?.('本机声音已启用');
    if(this.core)this.update(this.core);
  }
  setVolume(value){this.volume=Math.max(0,Math.min(1,value));if(this.gain)this.gain.gain.value=this.volume;}
  async track(key){
    if(this.trackKey===key||this.stopped)return;
    const request=++this.trackRequest;
    this.trackKey=key;this.source?.stop();this.source?.disconnect();this.source=null;
    if(key==='silent')return;
    try{
      if(!this.buffers.has(key)){
        const pending=loadMusicTrack(this.context,key);
        this.buffers.set(key,pending);
        void pending.catch(()=>{if(this.buffers.get(key)===pending)this.buffers.delete(key);});
      }
      const [buffer,loop]=await this.buffers.get(key);
      if(this.stopped||this.trackRequest!==request)return;
      const source=this.context.createBufferSource();source.buffer=buffer;source.loop=true;
      source.loopStart=loop.loopStart;source.loopEnd=loop.loopEnd;source.connect(this.gain);source.start();this.source=source;
      this.onStatus?.(`本机音乐：${key}`);
    }catch{if(this.trackRequest===request){this.trackKey=null;this.onStatus?.('本机音乐读取失败，可重新启用声音');}}
  }
  update(core){
    this.core=core;const end=core.th_get(11,0)>>>0;
    if(!this.context||this.context.state!=='running'||this.stopped){this.cursor=end;return;}
    const cue=core.th_get(79,0),key=cue===2?'silent':musicRoomTracks[cue<2?cue:cue-3]?.key||'silent';
    if(this.trackKey!==key)void this.track(key);
    if(this.cursor>end)this.cursor=0;this.cursor=Math.max(this.cursor,end-128);
    let latestBuffer=null;
    for(;this.cursor<end;this.cursor++){
      const id=core.th_sound_event(this.cursor);if(!id)continue;
      if(!this.effects.has(id)){
        this.effects.set(id,null);
        effectsManifest??=freshJSON('sfx/manifest.json');
        effectsManifest.catch(()=>{effectsManifest=null;});
        effectsManifest.then(manifest=>assetBytes(`sfx/${String(id).padStart(2,'0')}.wav`,{hash:manifest.sounds.find(sound=>sound.id===id)?.sha256})).then(bytes=>this.context.decodeAudioData(bytes.buffer))
          .then(buffer=>this.effects.set(id,buffer)).catch(()=>this.effects.delete(id));
      }
      const buffer=this.effects.get(id);if(!buffer)continue;
      latestBuffer=buffer;
    }
    // Effects are monophonic. Earlier events in the same tick were immediately
    // stopped before the audio clock could play them; create only the last one.
    if(latestBuffer){
      this.effect?.stop();this.effect?.disconnect();
      const source=this.context.createBufferSource();source.buffer=latestBuffer;source.connect(this.gain);
      source.onended=()=>{source.disconnect();if(this.effect===source)this.effect=null;};
      source.start();this.effect=source;
    }
  }
  stop(){this.unsubscribeVersion();this.stopped=true;++this.trackRequest;this.source?.stop();this.source?.disconnect();this.source=null;this.effect?.stop();this.effect?.disconnect();this.effect=null;void this.context?.close();}
}
