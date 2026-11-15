// Integration boundary for the isolated CPU decoder and the existing single
// Asyncify stack. CPU work never holds runtime.run() while it is in flight.
export class ArchiveReadAhead {
  constructor(decoder, runtime) {
    this.decoder=decoder;this.runtime=runtime;this.cpuTail=Promise.resolve();
    this.generation=0;this.batch=null;this.closed=false;
  }
  schedule(names, callback) {
    if(this.closed)throw new Error('Archive read-ahead closed');
    const generation=this.generation;
    const decode=this.cpuTail.then(()=>this.decoder.decode(names));
    this.cpuTail=decode.catch(()=>{});
    const payload=decode.then(entries=>({entries}),error=>({error}));
    const ready=payload.then(value=>({ok:!value.error,generation,names:[...names]}));
    const delivered=payload.then(value=>this.runtime.run(async()=>{
      if(this.batch!==null)throw new Error('Overlapping archive callback owner');
      const stale=this.closed||generation!==this.generation;
      this.batch=stale?{error:new Error('Stale archive read-ahead callback')}:
        value.error?{error:value.error}:{entries:new Map(value.entries.map(entry=>[entry.name,entry.bytes]))};
      try {
        // Even stale callbacks must be delivered: the C++ generation check
        // owns and frees its token. Dropping a callback here would leak it.
        return await callback();
      } finally {this.batch=null;}
    }));
    return {ready,delivered};
  }
  size(name) {
    if(!this.batch)return -1;
    if(this.batch.error)throw this.batch.error;
    return this.batch.entries.has(name)?this.batch.entries.get(name).byteLength:-1;
  }
  copy(name, destination) {
    const size=this.size(name);
    if(size<0)return -1;
    if(!(destination instanceof Uint8Array)||destination.byteLength!==size)
      throw new Error('Archive read-ahead destination size changed');
    destination.set(this.batch.entries.get(name));return size;
  }
  cancel() {++this.generation;}
  close() {
    if(this.closed)return;
    this.closed=true;++this.generation;this.decoder.close();
  }
}
