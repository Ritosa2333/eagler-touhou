// Presentation never calls into WASM. Only an accepted lockstep tick may do so.
// This is imported by the generated, separate np21-lockstep.js runtime.
import {LocalAudioOutput} from './audio-output.js';
export function createDeterministicHost(module) {
  const NativeDate = globalThis.Date;
  const state = {tick: 0, now: 0, audio: null, keys: {}, rng: 0x4a3d1234};
  const epoch = module.lockstepEpoch ?? 946684800000;
  class VirtualDate extends NativeDate {
    constructor(...args) {
      if (!args.length) super(epoch + state.now);
      else if (args.length > 1) super(NativeDate.UTC(...args));
      else super(args[0]);
    }
    static now() { return epoch + state.now; }
    getFullYear() { return this.getUTCFullYear(); }
    getMonth() { return this.getUTCMonth(); }
    getDate() { return this.getUTCDate(); }
    getDay() { return this.getUTCDay(); }
    getHours() { return this.getUTCHours(); }
    getMinutes() { return this.getUTCMinutes(); }
    getSeconds() { return this.getUTCSeconds(); }
    getTimezoneOffset() { return 0; }
  }
  state.Date = VirtualDate;
  state.replaying=false;
  state.frame=null;
  state.outputs=new Map();
  state.beginFrame=frame=>{state.frame=frame;state.outputs.set(frame,[]);};
  state.endFrame=()=>{state.frame=null;};
  state.confirm=prefix=>{
    for(const [frame,blocks]of state.outputs)if(frame<prefix){
      for(const [output,samples]of blocks)output.push(samples);
      state.outputs.delete(frame);
    }
  };
  state.discard=frame=>{for(const key of state.outputs.keys())if(key>=frame)state.outputs.delete(key);};
  state.capture=()=>({tick:state.tick,now:state.now,rng:state.rng,keys:{...state.keys},
    audio:state.audio,next:state.audio?.next,produced:state.audio?.produced});
  state.restore=saved=>{
    if(state.audio!==saved.audio||state.audio?.sdl.audio!==state.audio?.owner)
      throw Error('回滚窗口内音频设备发生重建，无法安全恢复');
    state.tick=saved.tick;state.now=saved.now;state.rng=saved.rng;state.keys={...saved.keys};
    if(state.audio){state.audio.next=saved.next;state.audio.produced=saved.produced;}
    state.frame=null;
  };
  state.randomFill = view => {
    for (let i=0; i<view.length; ++i) {
      let n=state.rng; n^=n<<13; n^=n>>>17; n^=n<<5;
      state.rng=n>>>0; view[i]=n&255;
    }
  };
  state.openAudio = (sdl, channels, samples, render) => {
    state.audio?.output.disconnect();
    const output=new LocalAudioOutput(sdl.audioContext,channels);
    const audio={sdl,owner:sdl.audio,output,channels,samples,render,
      next:state.now+samples/44100*1000, produced:0};
    // Upstream SDL shutdown assigns onaudioprocess and calls disconnect on this
    // property. Supply the cleanup contract without a ScriptProcessor callback.
    sdl.audio.scriptProcessorNode=output;
    state.audio=audio;
  };
  state.advanceAudio=()=>{
    const a=state.audio;
    if(!a||a.sdl.audio!==a.owner)return;
    while(a.next<=state.now+1e-7){
      const channels=Array.from({length:a.channels},()=>new Float32Array(a.samples));
      a.owner.currentOutputBuffer={numberOfChannels:a.channels,
        getChannelData:index=>channels[index]};
      a.render(); // Native audio generation is scheduled by virtual time only.
      a.owner.currentOutputBuffer=undefined;
      a.produced++;
      if(state.frame===null)a.output.push(channels);
      else state.outputs.get(state.frame).push([a.output,channels]);
      a.next+=a.samples/44100*1000;
    }
  };
  return state;
}
