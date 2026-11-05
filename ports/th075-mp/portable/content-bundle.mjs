// This DATA format contains local game inputs, never Runtime code or user saves.
export const CONTENT_SCHEMA = 'eagler-touhou/th075-content/1';
export const CONTENT_LAYOUT = {schema:CONTENT_SCHEMA,roots:['/fonts','/resources/base','/resources/patched','/music/catalog.bin']};
const safePath = path => typeof path === 'string' && path.length <= 240 &&
  /^\/(?:fonts\/(?:original-glyphs\.bin)|music\/catalog\.bin|resources\/(?:base|patched)\/[a-zA-Z0-9_./ -]+\.z)$/.test(path) &&
  path.split('/').slice(1).every(part => part && part !== '.' && part !== '..');

export function parseContentBundle(buffer) {
  // The Launcher supplies this object from its own Window realm.
  if (Object.prototype.toString.call(buffer) !== '[object ArrayBuffer]' || buffer.byteLength < 12 || buffer.byteLength > 512*1024*1024) throw Error('Invalid TH075 DATA size');
  const bytes = new Uint8Array(buffer), decoder = new TextDecoder('utf-8',{fatal:true});
  if (decoder.decode(bytes.subarray(0,8)) !== 'E75DATA1') throw Error('Invalid TH075 DATA header');
  const length = new DataView(buffer).getUint32(8,true), start = 12+length;
  if (length > 1024*1024 || start > bytes.length) throw Error('Invalid TH075 DATA index');
  const table = JSON.parse(decoder.decode(bytes.subarray(12,start)));
  if (table.schema !== CONTENT_SCHEMA || !Array.isArray(table.files) || table.files.length > 4096) throw Error('Invalid TH075 DATA schema');
  const entries = new Map(); let end = 0;
  for (const file of table.files) {
    if (!safePath(file.path) || entries.has(file.path) || !Number.isSafeInteger(file.offset) || file.offset !== end ||
        !Number.isSafeInteger(file.bytes) || file.bytes <= 0 || file.bytes > 128*1024*1024 ||
        start+file.offset+file.bytes > bytes.length || !/^[a-f0-9]{64}$/.test(file.sha256)) throw Error('Invalid TH075 DATA entry');
    entries.set(file.path,file); end += file.bytes;
  }
  if (start+end !== bytes.length || !entries.has('/fonts/original-glyphs.bin') || !entries.has('/music/catalog.bin')) throw Error('Incomplete TH075 DATA');
  return {identity:JSON.stringify(table.files.map(f=>[f.path,f.bytes,f.sha256])),paths:[...entries.keys()], async read(path) {
    const file = entries.get(path); if (!file) throw Error('TH075 DATA path is not declared: '+path);
    const value = bytes.slice(start+file.offset,start+file.offset+file.bytes);
    const hash = Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',value)),x=>x.toString(16).padStart(2,'0')).join('');
    if (hash !== file.sha256) throw Error('Corrupt TH075 DATA resource: '+path);
    return value;
  }};
}
