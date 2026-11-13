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
        value.error?{error:value.error}:{entries:new Map(value.entries.map(entry=>[entry.name,entry.bytes])),
          anms:new Map(value.entries.filter(entry=>entry.anm).map(entry=>[entry.name,entry.anm])),
          stds:new Map(value.entries.filter(entry=>entry.std).map(entry=>[entry.name,entry.std])),
          ecls:new Map(value.entries.filter(entry=>entry.ecl).map(entry=>[entry.name,entry.ecl])),
          shts:new Map(value.entries.filter(entry=>entry.sht).map(entry=>[entry.name,entry.sht]))};
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
  anmSize(name) {
    if(!this.batch)return -1;
    if(this.batch.error)throw this.batch.error;
    return this.batch.anms.has(name)?this.batch.anms.get(name).byteLength:-1;
  }
  anmCopy(name,destination) {
    const size=this.anmSize(name);if(size<0)return -1;
    if(!(destination instanceof Uint8Array)||destination.byteLength!==size)
      throw new Error('ANM transfer destination size changed');
    destination.set(this.batch.anms.get(name));return size;
  }
  parsedSize(kind,name) {
    if(kind!=='std'&&kind!=='ecl'&&kind!=='sht')throw new Error('Invalid parsed resource kind');
    if(!this.batch)return -1;if(this.batch.error)throw this.batch.error;
    const files=this.batch[kind+'s'];
    if(kind==='sht'&&this.batch.entries.has(name)&&!files.has(name))throw new Error('Parsed SHT transfer missing');
    return files.has(name)?files.get(name).byteLength:-1;
  }
  parsedCopy(kind,name,destination) {
    const size=this.parsedSize(kind,name);if(size<0)return -1;
    if(!(destination instanceof Uint8Array)||destination.byteLength!==size)
      throw new Error('Parsed resource destination size changed');
    destination.set(this.batch[kind+'s'].get(name));return size;
  }
  cancel() {++this.generation;}
  close() {
    if(this.closed)return;
    this.closed=true;++this.generation;this.decoder.close();
  }
}
