import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {NativeMusic,ConfirmedMusic} from '../web/native-music.js';
const patch=JSON.parse(await readFile(new URL('../web/native-patch.json',import.meta.url))).startup.music;
const heap=new Uint8Array(4*1024*1024),view=new DataView(heap.buffer),base=0x10000,segment=0x2000;
const at=base+segment*16+patch.mailbox_com_offset;
heap.set(new TextEncoder().encode(patch.signature),at);
view.setUint16(at+16,segment,true);
view.setUint16(base+0x180,0x180,true);view.setUint16(base+0x182,segment,true);
const reader=new NativeMusic(patch),heard=[],player={command:(...args)=>heard.push(args)},queue=new ConfirmedMusic(player);
function event(sequence,ax,hash){
  const pos=at+20+((sequence-1)&63)*8;
  view.setUint16(pos,sequence,true);view.setUint16(pos+2,ax,true);view.setUint32(pos+4,hash,true);
  view.setUint16(at+18,sequence,true);
}
// A speculative song never reaches Web Audio; only the corrected replay does.
assert.deepEqual(reader.read(heap),[]);const saved=reader.capture();
event(1,0,123);queue.stage(10,reader.read(heap));queue.confirm(10);assert.deepEqual(heard,[]);
queue.rewind(10);reader.restore(saved);event(1,0,456);queue.stage(10,reader.read(heap));
queue.confirm(11);queue.confirm(11);assert.deepEqual(heard,[[0,456]]);
event(2,0x202,456);event(3,0x100,456);queue.stage(11,reader.read(heap));queue.confirm(12);
assert.deepEqual(heard,[[0,456],[0x202,456],[0x100,456]]);
// Ring wrap, sequence wrap and immutable command copies.
reader.sequence=65534;event(65535,0x1900,456);event(0,0,789);
assert.deepEqual(reader.read(heap),[[0x1900,456],[0,789]]);
assert.deepEqual(reader.read(heap),[]);
view.setUint16(base+0x182,0,true);assert.equal(reader.valid(heap,at),false);
const manifest=JSON.parse(await readFile(new URL('../web/bgm/manifest.json',import.meta.url)));
assert.equal(Object.keys(manifest.tracks).length,21);assert.equal(manifest.codec,'opus');
assert.equal(manifest.bitrate,64000);assert.equal(manifest.sampleRate,48000);
for(const [id,track] of Object.entries(manifest.tracks)){
  assert(Object.values(manifest.headers).includes(id));assert(track.loopStart<=track.loopEnd);
  const audio=await readFile(new URL('../web/bgm/'+track.url,import.meta.url));
  assert.equal(audio.length,track.bytes);assert.equal(audio.subarray(0,4).toString(),'OggS');
  assert(audio.includes(Buffer.from('OpusHead')));
}
console.log('PASS: confirmed-only music, speculative discard/replay, no duplicate events, ring/sequence wrap, live-vector validation and all 21 Opus assets');
