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
    const source=this.source;
    this.source=null;
    if(source){try{source.stop();}catch{}}
  }

  async play(track,isCurrent=()=>true) {
    this.stop();
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
    source.connect(gain);
    gain.connect(this.context.destination);
    source.onended=()=>{if(this.source===source)this.source=null;};
    this.source=source;
    source.start();
    return true;
  }
}
