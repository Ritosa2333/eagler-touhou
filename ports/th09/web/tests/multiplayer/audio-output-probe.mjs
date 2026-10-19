// Test-only observation of the existing SDL output callback. No replacement
// audio, changes to the mixer, or new production ABI. Based on TH07's probe;
// retain block timing and bounded samples as well as AudioContext progress.
export function installAudioOutputProbe() {
 const attached=new WeakSet();let enabled=false,origin=0;
 const stats={blocks:[],nonzeroBlocks:0,sampledValues:0,maxAmplitude:0,overflow:false};
 window.audioOutputProbe={
  observe(sdl,active){
   enabled=active;
   const context=sdl?.audioContext,node=sdl?.audio_playback?.scriptProcessorNode;
   if(!node||attached.has(node)||typeof node.onaudioprocess!=='function')return;
   const original=node.onaudioprocess;
   node.onaudioprocess=function(event){
    const result=original.call(this,event);
    if(enabled&&event.outputBuffer?.numberOfChannels){
     const now=performance.now();if(!origin)origin=now;
     const samples=event.outputBuffer.getChannelData(0);let peak=0;
     for(let i=0,stride=Math.max(1,Math.ceil(samples.length/64));i<samples.length;i+=stride){
      peak=Math.max(peak,Math.abs(samples[i]));++stats.sampledValues;
     }
     ++stats.nonzeroBlocks; if(peak<=1e-6)--stats.nonzeroBlocks;
     stats.maxAmplitude=Math.max(stats.maxAmplitude,peak);
     if(stats.blocks.length>=50000){stats.overflow=true;throw Error('audio output sample overflow');}
     stats.blocks.push([now-origin,event.playbackTime,context.currentTime,samples.length/event.outputBuffer.sampleRate,peak]);
    }
    return result;
   };
   attached.add(node);
  },
  snapshot(){return stats;},
 };
}
