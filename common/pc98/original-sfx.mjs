import {extractOriginalEffectBank} from './pmd-archives.mjs';

export class OriginalSfx {
  static async create(game,fat){
    const context=new AudioContext({sampleRate:44100,latencyHint:'interactive'});await context.resume();
    await context.audioWorklet.addModule(new URL('./sfx-worklet.js',import.meta.url));
    const node=new AudioWorkletNode(context,'pc98-original-effects',{numberOfInputs:0,numberOfOutputs:1,outputChannelCount:[1]});
    node.connect(context.destination);
    const worker=new Worker(new URL('./sfx-worker.mjs',import.meta.url),{type:'module'}),instance=new OriginalSfx(context,node,worker,game);
    const folder={th02:'HUMAROKU',th03:'YUMEZIKU',th04:'GENSO'}[game];
    const driver=fat.read(fat.find(folder+'/PMD.COM'));
    const bank=extractOriginalEffectBank(game,fat).bytes;
    const {sfxDriver:fixture}=await import('./sfx-driver.mjs');
    const wasm=await(await fetch(new URL('vendor/dosbox-x/wdosbox-x-rollback.wasm',import.meta.url))).arrayBuffer();
    await new Promise((resolve,reject)=>{
      worker.onmessage=({data})=>{if(data.type==='ready')resolve();else if(data.type==='audio')node.port.postMessage(data.samples,[data.samples.buffer]);else if(data.type==='error')reject(Error(data.message));};
      worker.onerror=event=>reject(Error(event.message));
      worker.postMessage({type:'start',driver,bank,fixture,wasm});
    });
    return instance;
  }
  constructor(context,node,worker,game){this.context=context;this.node=node;this.worker=worker;this.game=game;this.lastShot=-Infinity;this.played=0;this.peak=0;this.sequence=0;this.status='原版 FM 音效';
    node.port.onmessage=({data})=>{this.peak=Math.max(this.peak,data.peak);};}
  command(ax){if((ax>>>8)===12)this.play(ax&255);}
  play(id){
    // TH02's SE 1 is the original shot sound. Full-rate fire can request it
    // every frame; space out playback without changing the game's shots.
    if(this.game==='th02'&&id===1){const now=this.context.currentTime;if(now-this.lastShot<0.12)return;this.lastShot=now;}
    this.worker.postMessage({type:'effect',id});this.played++;
  }
  confirm({sequence,ring}){const count=(sequence-this.sequence)&65535;for(let i=0;i<count;i++)this.play(ring[(this.sequence+i)&7]);this.sequence=sequence;}
  async resume(){await this.context.resume();}
  stop(){this.worker.postMessage({type:'effect',id:0});}
  info(){return {mode:'original-pmd',ready:true,sequence:this.sequence,played:this.played,peak:this.peak};}
  dispose(){this.worker.postMessage({type:'stop'});this.node.disconnect();void this.context.close();}
}
