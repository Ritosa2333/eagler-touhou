import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createHash} from 'node:crypto';
import {openPinnedPreviewResources} from '../runtime/preview-resources.mjs';

const bytes=value=>new TextEncoder().encode(value);
const pin=(url,value)=>({url,bytes:value.length,sha256:createHash('sha256').update(value).digest('hex')});
const core=bytes('core'),wasm=bytes('wasm'),data=bytes('dat');
const files={core:pin('/native/core.mjs',core),wasm:pin('/native/core.wasm',wasm),dat:pin('/native/th128.dat',data)};
const contents=new Map([[files.core.url,core],[files.wasm.url,wasm],[files.dat.url,data]]);

test('preview binds the loadable core and original data to an immutable resource manifest',async()=>{
  const requests=[];
  const input=await openPinnedPreviewResources(files,async url=>{
    requests.push(url);return contents.get(url).slice();
  });
  assert.equal(input.build.coreSHA256,files.wasm.sha256);
  assert.match(input.build.resourceSHA256,/^[a-f0-9]{64}$/);
  assert.notEqual(input.build.resourceSHA256,files.dat.sha256,'the room binds the complete input manifest');
  const one=await input.read('dat');one[0]=0;
  assert.deepEqual(await input.read('dat'),data);
  assert.equal(requests.filter(url=>url===files.dat.url).length,1);
  assert.throws(()=>input.url('unlisted'),/unlisted/);
});

test('exchanged DAT and disconnected path fail before a World can be opened',async()=>{
  await assert.rejects(openPinnedPreviewResources(files,async url=>url===files.dat.url?bytes('bad'):contents.get(url)),/dat checksum/);
  await assert.rejects(openPinnedPreviewResources({...files,core:{...files.core,url:'//example.invalid/core.mjs'}},async()=>assert.fail('unexpected read')),/same-origin/);
});
