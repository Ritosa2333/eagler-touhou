// DirectSound volume/pan rules in 4544b0 and 4663e0. The two settings use
// different curves: sound effects use a cube, streamed music uses a square.
function nativeGain(base,percent,power){
  const value=Math.max(0,Math.min(100,percent))/100;
  if(!value)return 0;
  const db=Math.trunc((base+5000)*(1-Math.pow(1-value,power)))-5000;
  return Math.pow(10,Math.max(-10000,Math.min(0,db))/2000);
}
export const nativeSoundGain=(base,percent)=>nativeGain(base,percent,3);
export const nativeMusicGain=percent=>nativeGain(0,percent,2);

// Keep retail PCM at its authored rate. decodeAudioData resamples each WAV
// during loading, which changes the short 8-bit menu effects in Edge.
function pcmBuffer(context,bytes){
  const raw=bytes instanceof ArrayBuffer?new Uint8Array(bytes):
    new Uint8Array(bytes.buffer,bytes.byteOffset,bytes.byteLength);
  const view=new DataView(raw.buffer,raw.byteOffset,raw.byteLength);
  const tag=offset=>String.fromCharCode(...raw.subarray(offset,offset+4));
  if(raw.length<12||tag(0)!=='RIFF'||tag(8)!=='WAVE'||view.getUint32(4,true)+8>raw.length)
    throw Error('音效 WAV 文件无效');
  let format=null,data=null;
  for(let offset=12;offset+8<=raw.length;){
    const size=view.getUint32(offset+4,true),start=offset+8,end=start+size;
    if(end>raw.length)throw Error('音效 WAV 数据块无效');
    if(tag(offset)==='fmt '){
      if(size<16)throw Error('音效 WAV 格式块无效');
      format={code:view.getUint16(start,true),channels:view.getUint16(start+2,true),
        rate:view.getUint32(start+4,true),align:view.getUint16(start+12,true),bits:view.getUint16(start+14,true)};
    }else if(tag(offset)==='data')data={start,size};
    offset=end+(size&1);
  }
  if(!format||!data||format.code!==1||![1,2].includes(format.channels)||
    ![8,16].includes(format.bits)||!format.rate||
    format.align!==format.channels*format.bits/8||!data.size||data.size%format.align)
    throw Error('音效 WAV PCM 格式无效');
  const frames=data.size/format.align,buffer=context.createBuffer(format.channels,frames,format.rate);
  const output=Array.from({length:format.channels},(_,channel)=>buffer.getChannelData(channel));
  for(let frame=0;frame<frames;frame++)for(let channel=0;channel<format.channels;channel++){
    const at=data.start+frame*format.align+channel*format.bits/8;
    output[channel][frame]=format.bits===8?(view.getUint8(at)-128)/128:view.getInt16(at,true)/32768;
  }
  return buffer;
}

export class SoundPlayer {
  constructor({context,index,readBytes,volume=80,onError=console.warn}){
    this.context=context;this.index=new Map(index.map(record=>[record.id,record]));
    this.readBytes=readBytes;this.volume=volume;this.onError=onError;
    this.buffers=new Map();this.pending=new Map();this.voices=new Map();
  }
  load(file){
    if(this.buffers.has(file))return Promise.resolve(this.buffers.get(file));
    if(this.pending.has(file))return this.pending.get(file);
    const promise=Promise.resolve().then(()=>this.readBytes(file)).then(bytes=>
      pcmBuffer(this.context,bytes)
    ).then(buffer=>{this.buffers.set(file,buffer);return buffer;}).finally(()=>this.pending.delete(file));
    this.pending.set(file,promise);return promise;
  }
  preload(){return Promise.all([...new Set([...this.index.values()].map(r=>r.file))].map(file=>this.load(file)));}
  release(voice){
    const source=voice.source;voice.source=null;
    if(source){source.onended=null;try{source.stop();}catch{}}
    for(const node of voice.nodes??[])node.disconnect();
    voice.nodes=[];
  }
  start(voice){
    if(this.voices.get(voice.id)!==voice||voice.paused||!voice.buffer)return;
    const {context}=this,{buffer,record}=voice;
    if(!record.loop&&voice.offset>=buffer.duration){this.voices.delete(voice.id);return;}
    const source=context.createBufferSource(),gain=context.createGain();
    source.buffer=buffer;source.loop=!!record.loop;
    gain.gain.value=nativeSoundGain(record.volume,this.volume);
    // DirectSound attenuates only the channel away from the source. A
    // StereoPanner's equal-power crossfade changes the original centre level.
    gain.channelCount=2;gain.channelCountMode='explicit';gain.channelInterpretation='speakers';
    const pan=Math.max(-10000,Math.min(10000,voice.pan));
    source.connect(gain);voice.nodes=[source,gain];
    if(pan===0){
      // The explicit stereo gain already performs the same mono upmix and
      // stereo pass-through as two unity-gain split channels.
      gain.connect(context.destination);
    }else{
      const split=context.createChannelSplitter(2),left=context.createGain(),right=context.createGain(),merge=context.createChannelMerger(2);
      left.gain.value=pan>0?Math.pow(10,-pan/2000):1;
      right.gain.value=pan<0?Math.pow(10,pan/2000):1;
      gain.connect(split);split.connect(left,0);split.connect(right,1);
      left.connect(merge,0,0);right.connect(merge,0,1);merge.connect(context.destination);
      voice.nodes.push(split,left,right,merge);
    }
    voice.source=source;voice.gain=gain;
    voice.startedAt=context.currentTime;
    source.onended=()=>{if(voice.source!==source)return;this.release(voice);if(this.voices.get(voice.id)===voice)this.voices.delete(voice.id);};
    source.start(0,record.loop?voice.offset%buffer.duration:voice.offset);
  }
  play(event){
    const old=this.voices.get(event.id);if(old){this.release(old);this.voices.delete(event.id);}
    if(event.stop)return;
    const record=this.index.get(event.id);if(!record)return;
    const voice={id:event.id,record,pan:event.pan??0,offset:0,paused:false,buffer:this.buffers.get(record.file)};
    this.voices.set(event.id,voice);
    if(voice.buffer)this.start(voice);
    else this.load(record.file).then(buffer=>{voice.buffer=buffer;this.start(voice);}).catch(error=>{
      if(this.voices.get(event.id)===voice)this.voices.delete(event.id);
      this.onError(error);
    });
  }
  setVolume(value){this.volume=value;for(const voice of this.voices.values())if(voice.source)voice.gain.gain.value=nativeSoundGain(voice.record.volume,value);}
  pause(){
    // 423020 stops every active buffer and remembers which were playing;
    // UI sounds remain available while the gameplay buffers are stopped.
    for(const voice of this.voices.values())if(!voice.paused){
      if(voice.source)voice.offset+=Math.max(0,this.context.currentTime-voice.startedAt);
      voice.paused=true;this.release(voice);
    }
  }
  resume(){for(const voice of this.voices.values())if(voice.paused){voice.paused=false;this.start(voice);}}
  stop(){for(const voice of this.voices.values())this.release(voice);this.voices.clear();}
}
