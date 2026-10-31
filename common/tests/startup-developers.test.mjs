import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
function fixture(){
  const calls={images:[],text:[],files:[],revoked:[]};
  const context={drawImage(...args){calls.images.push(args.slice(1));},scale(){},fillText(...args){calls.text.push(args);},
    getImageData:()=>({data:new Uint8Array(1280*960*4)})};
  const environment={STARTUP_DEVELOPERS:readFileSync(new URL('../browser/startup-developers.svg',import.meta.url),'utf8'),
    Blob:class{},URL:{createObjectURL:()=>'/credit',revokeObjectURL:url=>calls.revoked.push(url)},
    Image:class{decode(){return Promise.resolve();}},document:{createElement:()=>({getContext:()=>context})}};
  const source=readFileSync(new URL('../browser/startup-developers.mjs',import.meta.url),'utf8').replaceAll('export async function','async function');
  const install=runInNewContext(source+';installStartupBranding',environment);
  const core={FS:{writeFile:(path,bytes)=>calls.files.push({path,length:bytes.length})}};
  return {calls,install,core};
}
test('TH12 uses the shared transparent developer wordmark without inventing a build timestamp',async()=>{
  const f=fixture();await f.install(f.core,{game:'th12'});
  assert.equal(f.calls.images[0][1],312);assert.equal(f.calls.text.length,0);
  assert.deepEqual(f.calls.files,[{path:'/eagler-startup.rgba',length:1280*960*4}]);
  assert.deepEqual(f.calls.revoked,['/credit']);
});
test('existing startup credits preserve their placement and UTC+8 build timestamp',async()=>{
  const f=fixture();await f.install(f.core,{game:'th15',builtAt:'2026-10-31T01:02:00Z'});
  assert.equal(f.calls.images[0][1],336);
  assert.equal(f.calls.text[0][0],'build 2026.10.31 09:02 UTC+8');
  await assert.rejects(f.install(f.core,{game:'th10'}),/timestamp/);
});
