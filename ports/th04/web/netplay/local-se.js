import {assetBytes} from '../preload-cache.js';

// Confirmed commands only. Complete samples run on the audio clock even when
// network/game steps stop arriving; playback never writes simulation state.
export class LocalSe {
  constructor(context){
    this.context=context;this.buffers=new Map();this.sources=new Set();
    this.sequence=0;this.played=0;this.disposed=false;this.voice=null;
    this.status='浏览器音效：正在加载';this.abort=new AbortController();
    this.onState=()=>{if(context.state!=='running')this.stop();};
    context.addEventListener('statechange',this.onState);
  }
  async load(){
    const base=new URL('../assets/',import.meta.url);
    const response=await fetch(new URL('se-manifest.json',base),{signal:this.abort.signal});
    if(!response.ok)throw Error('音效清单读取失败');
    const manifest=await response.json();
    if(manifest.schema!==1||manifest.sampleRate!==44100||manifest.channels!==1||manifest.clips.length!==17)throw Error('音效资源版本不匹配');
    await Promise.all(manifest.clips.map(async(clip,id)=>{
      if(clip.id!==id||clip.url!==`se-${String(id).padStart(2,'0')}.wav`)throw Error('音效编号不匹配');
      const bytes=await assetBytes(`assets/${clip.url}`,{hash:clip.sha256,size:clip.bytes,signal:this.abort.signal});
      const buffer=await this.context.decodeAudioData(bytes.buffer);
      if(!this.disposed)this.buffers.set(id,buffer);
    }));
    if(!this.disposed)this.status='浏览器音效：独立播放（原版 FM）';
  }
  confirm({sequence,ring}){
    if(this.disposed)return;
    const count=(sequence-this.sequence)&65535;
    if(count>8)throw Error('音效事件超过桥接队列容量');
    for(let i=0;i<count;i++){
      const id=ring[(this.sequence+i)&7];
      if(!Number.isInteger(id)||id<0||id>=17)throw Error('原生音效编号无效');
      this.play(id);
    }
    this.sequence=sequence;
  }
  play(id){
    const buffer=this.buffers.get(id),ctx=this.context;
    // Suspended audio must not accumulate commands to play after unlocking.
    if(this.disposed||!buffer||ctx.state!=='running')return;
    const at=ctx.currentTime+0.003;
    // PMD has one SE voice; native priority arbitration chose this command.
    this.stop(at);
    const source=ctx.createBufferSource(),gain=ctx.createGain();
    source.buffer=buffer;source.connect(gain);gain.connect(ctx.destination);
    gain.gain.setValueAtTime(0,at);gain.gain.linearRampToValueAtTime(1,at+0.001);
    const item={source,gain,at};this.sources.add(item);this.voice=item;this.played++;
    source.onended=()=>{this.sources.delete(item);if(this.voice===item)this.voice=null;source.disconnect();gain.disconnect();};
    source.start(at);
  }
  stop(at=this.context.currentTime){
    for(const item of this.sources){
      try{
        if(item.at>=at)item.source.stop(at);
        else{item.gain.gain.cancelScheduledValues(at);item.gain.gain.setValueAtTime(1,at);item.gain.gain.linearRampToValueAtTime(0,at+0.002);item.source.stop(at+0.002);}
      }catch{/* The audio thread may already have ended the source. */}
    }
    this.sources.clear();this.voice=null;
  }
  info(){return {status:this.status,sequence:this.sequence,played:this.played,active:this.voice?1:0,loaded:this.buffers.size};}
  dispose(){if(this.disposed)return;this.disposed=true;this.abort.abort();this.stop();this.buffers.clear();this.context.removeEventListener('statechange',this.onState);}
}

export function readNativeSe(heap,data,observer){
  if(heap[data+observer.bgm_mode]!==0||heap[data+observer.se_mode]!==1)throw Error('原生音效桥接模式不匹配');
  return {sequence:new DataView(heap.buffer,heap.byteOffset,heap.byteLength).getUint16(data+observer.se_sequence,true),
    ring:heap.slice(data+observer.se_ring,data+observer.se_ring+8)};
}
