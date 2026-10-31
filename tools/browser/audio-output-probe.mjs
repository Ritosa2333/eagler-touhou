// Test-side observation of actual SDL Web Audio output. Installed before the
// Runtime starts; no player build, native export or game state is modified.
export function installPcmProbe(){
  const probe={frames:0,peak:0,reset(){this.frames=0;this.peak=0;}};
  globalThis.__sdlAudioProbe=probe;
  const prototype=globalThis.AudioContext?.prototype;
  if(!prototype?.createScriptProcessor)return;
  const create=prototype.createScriptProcessor;
  prototype.createScriptProcessor=function(...args){
    const node=create.apply(this,args);
    queueMicrotask(()=>{
      const process=node.onaudioprocess;
      if(typeof process!=='function')return;
      node.onaudioprocess=function(event){
        process.call(this,event);
        const buffer=event.outputBuffer;probe.frames+=buffer.length;
        for(let channel=0;channel<buffer.numberOfChannels;channel++){
          for(const sample of buffer.getChannelData(channel))probe.peak=Math.max(probe.peak,Math.abs(sample));
        }
      };
    });
    return node;
  };
}

export async function samplePcm(frame){
  // Retire the old device queue before opening the observation window.
  await new Promise(resolve=>setTimeout(resolve,250));
  await frame.evaluate(()=>globalThis.__sdlAudioProbe.reset());
  await new Promise(resolve=>setTimeout(resolve,300));
  return frame.evaluate(()=>({frames:__sdlAudioProbe.frames,peak:__sdlAudioProbe.peak}));
}

// Observe the actual browser start calls separately from the queued SDL mixer.
export function installSfxStartProbe(){
  globalThis.__sfxStarts=[];
  const prototype=globalThis.AudioBufferSourceNode?.prototype;if(!prototype)return;
  const start=prototype.start;
  prototype.start=function(when=0,...args){
    let peak=0;const buffer=this.buffer;
    if(buffer)for(let channel=0;channel<buffer.numberOfChannels;channel++){
      const pcm=buffer.getChannelData(channel);
      for(let i=0;i<Math.min(pcm.length,4096);i++)peak=Math.max(peak,Math.abs(pcm[i]));
    }
    if(globalThis.__sfxStarts.length<256)globalThis.__sfxStarts.push({when,peak,channels:buffer?.numberOfChannels});
    return start.call(this,when,...args);
  };
}
