import {SpessaSynthProcessor,SoundBankLoader} from '../../vendor/spessasynth.mjs';
// MIDI device only. Original sequencing stays in C++; PCM is mixed by SDL.
class NativeMidi extends AudioWorkletProcessor {
 constructor(){super();this.paused=false;this.ready=false;this.disposed=false;this.synth=new SpessaSynthProcessor(sampleRate,{eventsEnabled:false});
  this.port.onmessage=async({data})=>{try{
   if(data.type==='bank'){await this.synth.processorInitialized;this.synth.soundBankManager.addSoundBank(SoundBankLoader.fromArrayBuffer(data.data.buffer),'windows-gm');this.ready=true;this.port.postMessage({type:'ready'});}
   if(data.type==='midi'&&this.ready)this.synth.processMessage(data.data);
   if(data.type==='pause')this.paused=data.paused;
   if(data.type==='reset'&&this.ready)for(let c=0;c<16;c++)for(const n of [120,121])this.synth.processMessage(Uint8Array.of(0xb0|c,n,0));
   if(data.type==='dispose'){this.disposed=true;this.ready=false;this.synth=null;}
  }catch(error){this.port.postMessage({type:'error',message:String(error)});}};
 }
 process(inputs,outputs){if(this.disposed)return false;const [left,right]=outputs[0];if(!left||!right)return true;left.fill(0);right.fill(0);if(this.ready&&!this.paused)this.synth.process(left,right);return true;}
}
registerProcessor('th08-native-midi',NativeMidi);
