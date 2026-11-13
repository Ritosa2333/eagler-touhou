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
      const entries = [];let total = 0;
      for (const name of message.names) {
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
        entries.push({name, bytes:copy.buffer});
      }
      send({id, command, ok:true, entries}, entries.map(entry => entry.bytes));return;
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
