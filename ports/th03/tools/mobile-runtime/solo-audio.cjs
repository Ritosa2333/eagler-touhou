// Local PCM presentation only; no BGM files, resampling or simulation stepping.
function openBufferedSoloAudio(Module, channels, frames, render) {
  const sdl=Module.SDL2, audio=sdl.audio, context=sdl.audioContext;
  const duration=frames/context.sampleRate, lead=0.025;
  const ahead=Math.max(0.18,duration*2), pending=new Set();
  let end=0, closed=false, timer, underruns=0;
  function flush(){
    for(const source of pending){try{source.stop();}catch{}source.disconnect();}
    pending.clear();end=0;
  }
  function pump(){
    if(closed||sdl.audio!==audio)return;
    if(context.state!=='running'){
      flush();
      if(context.state==='suspended'&&globalThis.navigator?.userActivation?.hasBeenActive)
        context.resume().catch(()=>{});
      return;
    }
    const now=context.currentTime;
    if(end<now){if(end)underruns++;end=now+lead;}
    // Bounded catch-up: never generate an unbounded queue after a long stall.
    for(let count=0;count<4&&end-now<ahead;count++){
      const buffer=context.createBuffer(channels,frames,context.sampleRate);
      audio.currentOutputBuffer=buffer;
      try{render();}finally{audio.currentOutputBuffer=undefined;}
      const source=context.createBufferSource();source.buffer=buffer;
      source.connect(context.destination);pending.add(source);
      source.onended=()=>{pending.delete(source);source.disconnect();};
      source.start(end);end+=duration;
    }
  }
  function onState(){if(context.state!=='running')flush();else pump();}
  // SDL's original close path owns this handle and cancels every queued source.
  audio.scriptProcessorNode={disconnect(){
    if(closed)return;closed=true;clearInterval(timer);
    context.removeEventListener('statechange',onState);flush();
    if(Module.soloAudioInfo===info)delete Module.soloAudioInfo;
  }};
  const info=()=>({mode:'buffered-native',underruns,queuedMs:Math.max(0,end-context.currentTime)*1000});
  Module.soloAudioInfo=info;
  context.addEventListener('statechange',onState);
  // SDL opens the device before its native callback buffers are initialized.
  // Rendering synchronously here re-enters SDL_OpenAudio and corrupts memory
  // (including HDD configuration). First render only after this stack returns.
  timer=setInterval(pump,25);
}
module.exports={injection:()=>openBufferedSoloAudio.toString()+'\n',openBufferedSoloAudio};
