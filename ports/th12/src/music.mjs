// Native StageInfo 4aebf0 has road/Boss filenames at +10/+14. The game
// preloads them into slots 0/1 at 4221d1/4221e1; MSG19 selects slot1 at
// 42089d. This player receives that logical selection from the game.
export class MusicPlayer {
  constructor({context,readBytes,volume=.35}) {
    this.context=context;
    this.readBytes=readBytes;
    this.volume=volume;
    this.buffers=new Map();
    this.pending=new Map();
    this.source=null;
    this.request=0;
    this.track=null;this.offset=0;this.startedAt=0;this.paused=false;this.fadeState=null;
  }

  load(track) {
    const file=track.file;
    if(this.buffers.has(file))return Promise.resolve(this.buffers.get(file));
    if(this.pending.has(file))return this.pending.get(file);
    const promise=Promise.resolve().then(()=>this.readBytes(file)).then(bytes=>{
      // decodeAudioData may detach its input; copy only the requested range
      // so that shared resource caches remain usable on a later start.
      const encoded=bytes instanceof ArrayBuffer?bytes.slice(0):
        bytes.buffer.slice(bytes.byteOffset,bytes.byteOffset+bytes.byteLength);
      return this.context.decodeAudioData(encoded);
    }).then(buffer=>{
      this.buffers.set(file,buffer);
      return buffer;
    }).finally(()=>{
      if(this.pending.get(file)===promise)this.pending.delete(file);
    });
    this.pending.set(file,promise);
    return promise;
  }

  preload(tracks) {
    return Promise.all(tracks.map(track=>this.load(track)));
  }

  stop() {
    this.request++;
    this.track=null;this.offset=0;this.paused=false;this.fadeState=null;
    this.detach();
  }

  detach() {
    const source=this.source;
    this.source=null;const gain=this.gain;this.gain=null;
    if(source){source.onended=null;try{source.stop();}catch{}source.disconnect();}
    gain?.disconnect();
  }

  setVolume(value){value=Math.max(0,Math.min(1,value));if(value===this.volume)return;this.volume=value;this.fadeState=null;if(this.gain){this.gain.gain.cancelScheduledValues(this.context.currentTime);this.gain.gain.value=value;}}
  fade(seconds=1){
    const now=this.context.currentTime,from=this.fadeLevel(now);
    this.fadeState={from,start:now,duration:Math.max(0,seconds)};
    this.scheduleFade();
  }
  //4666c0 fades the DirectSound volume to-5000 and then stops. The audible
  //gain is exponential in that decibel ramp, rather than linear amplitude.
  fadeLevel(now=this.context.currentTime){const f=this.fadeState;if(!f)return this.volume;const t=Math.min(1,Math.max(0,now-f.start)/Math.max(f.duration,Number.EPSILON));return !f.from||t>=1?0:f.from*Math.pow(Math.pow(10,-2.5)/f.from,t);}
  scheduleFade(){
    if(!this.gain||!this.fadeState)return;
    const now=this.context.currentTime,from=this.fadeLevel(now),end=this.fadeState.start+this.fadeState.duration;
    const gain=this.gain.gain;gain.cancelScheduledValues(now);gain.setValueAtTime(from,now);
    if(from>0&&end>now){gain.exponentialRampToValueAtTime(Math.pow(10,-2.5),end);gain.setValueAtTime(0,end);}
  }
  offsetAt(seconds){const track=this.track,length=(track?.loopEnd??0)-(track?.loopStart??0);return length>0&&seconds>=track.loopEnd?track.loopStart+(seconds-track.loopStart)%length:seconds;}
  pause(){
    if(!this.track||this.paused)return;
    const now=this.context.currentTime;
    if(this.source)this.offset=this.offsetAt(this.offset+Math.max(0,now-this.startedAt));
    if(this.fadeState)this.fadeState={from:this.fadeLevel(now),start:now,duration:Math.max(0,this.fadeState.start+this.fadeState.duration-now)};
    this.paused=true;++this.request;this.detach();
  }
  async resume(isCurrent=()=>true){
    if(!this.paused||!this.track)return false;
    const track=this.track,offsetSeconds=this.offset,fade=this.fadeState;
    const played=await this.play(track,isCurrent,{offsetSeconds});
    if(played&&fade){this.fadeState={...fade,start:this.context.currentTime};this.scheduleFade();}
    return played;
  }

  async play(track,isCurrent=()=>true,{offsetSeconds=0}={}) {
    this.stop();
    this.track=track;this.offset=this.offsetAt(Number.isFinite(offsetSeconds)?Math.max(0,offsetSeconds):0);
    const request=this.request;
    if(!isCurrent())return false;
    let buffer;
    try{buffer=await this.load(track);}catch(error){
      if(request!==this.request||!isCurrent())return false;
      throw error;
    }
    if(request!==this.request||!isCurrent())return false;
    const source=this.context.createBufferSource(),gain=this.context.createGain();
    source.buffer=buffer;
    source.loop=true;
    source.loopStart=track.loopStart;
    source.loopEnd=track.loopEnd;
    gain.gain.value=this.volume;
    this.gain=gain;
    source.connect(gain);
    gain.connect(this.context.destination);
    source.onended=()=>{if(this.source===source)this.source=null;};
    this.source=source;
    this.startedAt=this.context.currentTime;
    source.start(0,this.offset);
    this.scheduleFade();
    return true;
  }
}
