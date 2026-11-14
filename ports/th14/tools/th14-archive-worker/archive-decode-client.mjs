export class ArchiveDecodeClient {
  constructor(worker) {
    this.worker = worker;this.next = 0;this.pending = new Map();
    this.ready = false;this.closed = false;this.decoding = false;
    const receive = message => {
      const row = this.pending.get(message?.id);
      if (!row) return;
      this.pending.delete(message.id);
      if (message.command !== row.command) {row.reject(new Error('Archive response owner changed'));return;}
      if (message.ok) row.resolve(message);else row.reject(new Error(message.error || 'Archive worker failed'));
    };
    const fail = error => {
      this.closed = true;this.ready = false;
      for (const row of this.pending.values()) row.reject(error instanceof Error ? error : new Error(String(error.message ?? error)));
      this.pending.clear();
    };
    if (typeof worker.addEventListener === 'function') {
      worker.addEventListener('message', event => receive(event.data));
      worker.addEventListener('error', fail);
    } else {
      worker.on('message', receive);worker.on('error', fail);
      worker.on('exit', code => {if (!this.closed) fail(new Error(`Archive worker exited: ${code}`));});
    }
  }
  request(command, extra={}, transfer=[]) {
    if (this.closed) return Promise.reject(new Error('Archive worker closed'));
    const id = ++this.next;
    return new Promise((resolve, reject) => {
      this.pending.set(id, {command, resolve, reject});
      try {this.worker.postMessage({id, command, ...extra}, transfer);}
      catch (error) {this.pending.delete(id);reject(error);}
    });
  }
  async open(bytes) {
    if (this.ready || !(bytes instanceof Uint8Array)) throw new Error('Invalid archive owner');
    // Preserve the main game's immutable input. Only this new copy transfers.
    const copy = bytes.slice();
    const result = await this.request('open', {bytes:copy.buffer}, [copy.buffer]);
    this.ready = true;return result.count;
  }
  async decode(names) {
    if (!this.ready || this.decoding) throw new Error('Archive worker is not ready for a new job');
    this.decoding = true;
    try {
      const result = await this.request('decode', {names});
      if (!Array.isArray(result.entries) || result.entries.length !== names.length ||
          result.entries.some((entry, i) => entry.name !== names[i] || !(entry.bytes instanceof ArrayBuffer)))
        throw new Error('Archive response names or buffers changed');
      return result.entries.map(entry => ({name:entry.name, bytes:new Uint8Array(entry.bytes)}));
    } finally {this.decoding = false;}
  }
  close() {
    if (this.closed) return;
    this.closed = true;this.ready = false;
    for (const row of this.pending.values()) row.reject(new Error('Archive worker cancelled'));
    this.pending.clear();this.worker.terminate();
  }
}
