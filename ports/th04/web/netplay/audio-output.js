// Presentation only. No calls into the emulator, and no edits to its clock.
// AudioBufferSourceNode also works on LAN HTTP, unlike AudioWorklet modules
// which normally require a secure context on the second computer.
export class LocalAudioOutput {
  constructor(context,channels){
    this.context=context;this.channels=channels;this.pending=new Set();
    this.end=0;this.started=false;this.closed=false;this.underruns=0;this.resets=0;
    this.lead=0.012;this.maxAhead=0.060;
    this.onState=()=>{if(context.state!=='running')this.flush();};
    context.addEventListener('statechange',this.onState);
  }
  flush(){
    const now=this.context.currentTime;
    for(const item of this.pending){
      try{
        if(item.at>=now){item.source.stop();continue;}
        item.gain.gain.cancelScheduledValues(now);
        item.gain.gain.setValueAtTime(1,now);
        item.gain.gain.linearRampToValueAtTime(0,now+0.003);
        item.source.stop(now+0.003);
      }catch{/* Source may already have finished on the audio thread. */}
    }
    this.pending.clear();this.end=0;this.started=false;
  }
  push(samples){
    if(this.closed)return;
    const ctx=this.context;
    if(ctx.state!=='running'){this.flush();return;}
    const now=ctx.currentTime,duration=samples[0].length/44100;
    // Catch-up must not leave a growing tail of music behind the game.
    if(this.started&&this.end-now+duration>this.maxAhead){this.resets++;this.flush();}
    const gap=this.started&&this.end<now;
    if(gap&&now-this.end>0.002)this.underruns++;
    const fresh=!this.started||gap;
    const at=fresh?now+this.lead:Math.max(this.end,now);
    const buffer=ctx.createBuffer(this.channels,samples[0].length,44100);
    for(let c=0;c<this.channels;c++)buffer.copyToChannel(samples[c],c);
    const source=ctx.createBufferSource(),gain=ctx.createGain();
    source.buffer=buffer;source.connect(gain);gain.connect(ctx.destination);
    // Only restarted segments fade in. Ordinary adjacent blocks stay at unity
    // to preserve music continuity and pitch (no playback-rate correction).
    if(fresh){gain.gain.setValueAtTime(0,at);gain.gain.linearRampToValueAtTime(1,at+0.002);}
    const item={source,gain,at};this.pending.add(item);
    source.onended=()=>{this.pending.delete(item);source.disconnect();gain.disconnect();};
    source.start(at);this.end=at+duration;this.started=true;
  }
  info(){
    return {audioUnderruns:this.underruns,audioResets:this.resets,
      audioQueuedMs:Math.max(0,this.end-this.context.currentTime)*1000,
      audioDeviceMs:Number.isFinite(this.context.outputLatency)?this.context.outputLatency*1000:null};
  }
  disconnect(){
    this.closed=true;this.flush();this.context.removeEventListener('statechange',this.onState);
  }
}
