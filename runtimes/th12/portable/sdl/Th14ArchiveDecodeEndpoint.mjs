// Serial requests run in a dedicated worker with its own Wasm memory/codec.
export function createArchiveDecodeEndpoint(createModule, send) {
  const ready = createModule();
  let archivePointer = 0, opened = false, sequence = Promise.resolve();
  const encoder = new TextEncoder();
  async function handle(message) {
    const {id, command} = message ?? {};
    if (!Number.isSafeInteger(id) || id < 1) throw new Error('Invalid request ID');
    const module = await ready;
    if (command === 'open') {
      if (opened || !(message.bytes instanceof ArrayBuffer) || !message.bytes.byteLength ||
          message.bytes.byteLength > 128 * 1024 * 1024) throw new Error('Invalid archive open');
      archivePointer = module._malloc(message.bytes.byteLength);
      if (!archivePointer) throw new Error('Archive allocation failed');
      module.HEAPU8.set(new Uint8Array(message.bytes), archivePointer);
      const count = module._decoder_open(archivePointer, message.bytes.byteLength);
      if (!count) {
        module._free(archivePointer);archivePointer = 0;
        throw new Error(`Archive rejected: ${module._decoder_error()}`);
      }
      opened = true;
      send({id, command, ok:true, count});return;
    }
    if (command === 'decode') {
      if (!opened || !Array.isArray(message.names) || message.names.length < 1 ||
          message.names.length > 8) throw new Error('Invalid decode batch');
      const entries=[],dependencies=[],queued=new Set(message.names),jobs=message.names.map(name=>({name,dependency:false}));let total=0;
      for (let job=0;job<jobs.length;++job) {
        const {name,dependency}=jobs[job];
        if (typeof name !== 'string') throw new Error('Invalid archive name');
        const bytes = encoder.encode(name);
        if (!bytes.length || bytes.length > 128 || bytes.includes(0)) throw new Error('Invalid archive name');
        const pointer = module._malloc(bytes.length);
        if (!pointer) throw new Error('Name allocation failed');
        let decoded;
        try {
          module.HEAPU8.set(bytes, pointer);
          decoded = module._decoder_read(pointer, bytes.length);
        } finally {module._free(pointer);}
        if (!decoded) throw new Error(`Entry rejected: ${name} (${module._decoder_error()})`);
        const size = module._decoder_size();total += size;
        if (size > 64 * 1024 * 1024 || total > 128 * 1024 * 1024) throw new Error('Decoded batch exceeds budget');
        // The C++ buffer is overwritten by the next read. Transfer a distinct
        // copy; the consumer can never alias that mutable dictionary/output.
        const copy = size ? module.HEAPU8.slice(decoded, decoded+size) : new Uint8Array();
        let anm;
        if(name.endsWith('.anm')){
          const parsed=module._decoder_anm();
          if(!parsed)throw new Error(`ANM rejected: ${name} (${module._decoder_error()})`);
          const parsedSize=module._decoder_anm_size();total+=parsedSize;
          if(parsedSize<36||parsedSize>128*1024*1024||total>128*1024*1024)
            throw new Error('Parsed ANM batch exceeds budget');
          anm=module.HEAPU8.slice(parsed,parsed+parsedSize).buffer;
        }
        const kind=name.endsWith('.std')?'std':name.endsWith('.ecl')?'ecl':name.endsWith('.sht')?'sht':null;let parsedResource;
        if(kind){
          const parsed=kind==='std'?module._decoder_std():kind==='ecl'?module._decoder_ecl():module._decoder_sht();
          if(!parsed)throw new Error(`${kind.toUpperCase()} rejected: ${name} (${module._decoder_error()})`);
          const parsedSize=module._decoder_parsed_size();total+=parsedSize;
          if(parsedSize<36||parsedSize>128*1024*1024||total>128*1024*1024)
            throw new Error('Parsed resource batch exceeds budget');
          parsedResource=module.HEAPU8.slice(parsed,parsed+parsedSize).buffer;
          if(kind==='ecl'){
            const count=module._decoder_ecl_include_count();if(count>32)throw new Error('ECL include limit exceeded');
            for(let index=0;index<count;++index){
              const include=module.UTF8ToString(module._decoder_ecl_include_name(index));
              if(!include.endsWith('.ecl'))throw new Error('Invalid ECL include name');
              if(queued.has(include))continue;
              if(jobs.length-message.names.length>=32)throw new Error('ECL dependency batch exceeds budget');
              queued.add(include);jobs.push({name:include,dependency:true});
            }
          }
        }
        (dependency?dependencies:entries).push({name,bytes:copy.buffer,...(anm?{anm}:{}),...(kind?{[kind]:parsedResource}:{})});
      }
      send({id,command,ok:true,entries,dependencies},[...entries,...dependencies].flatMap(entry=>[entry.bytes,...['anm','std','ecl','sht'].filter(kind=>entry[kind]).map(kind=>entry[kind])]));return;
    }
    if (command === 'close') {
      module._decoder_close();
      if (archivePointer) module._free(archivePointer);
      archivePointer = 0;opened = false;send({id, command, ok:true});return;
    }
    throw new Error('Unknown archive command');
  }
  return message => {
    sequence = sequence.then(() => handle(message)).catch(error => {
      send({id:message?.id, command:message?.command, ok:false, error:String(error.message ?? error)});
    });
    return sequence;
  };
}
