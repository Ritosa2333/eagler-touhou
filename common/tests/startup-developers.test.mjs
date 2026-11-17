import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
function fixture(){
  const calls={images:[],text:[],files:[]};
  const context={drawImage(...args){calls.images.push(args.slice(1));},scale(){},fillText(...args){calls.text.push(args);},
    getImageData:()=>({data:new Uint8Array(1280*960*4)})};
  const environment={document:{createElement:()=>({getContext:()=>context})}};
  const source=readFileSync(new URL('../browser/startup-developers.mjs',import.meta.url),'utf8').replaceAll('export async function','async function');
  const install=runInNewContext(source+';installStartupBranding',environment);
  const core={FS:{writeFile:(path,bytes)=>calls.files.push({path,length:bytes.length})}};
  return {calls,install,core};
}
test('TH12 creates a transparent startup layer without a build timestamp',async()=>{
  const f=fixture();await f.install(f.core,{game:'th12'});
  assert.equal(f.calls.images.length,0);assert.equal(f.calls.text.length,0);
  assert.deepEqual(f.calls.files,[{path:'/eagler-startup.rgba',length:1280*960*4}]);
});
test('every supported game draws only its UTC+8 build timestamp',async()=>{
  for(const game of ['th10','th11','th12','th13','th15','th20']){
    const f=fixture();await f.install(f.core,{game,builtAt:'2026-10-31T01:02:00Z'});
    assert.equal(f.calls.images.length,0);
    assert.equal(f.calls.text[0][0],'build 2026.10.31 09:02 UTC+8');
    assert.deepEqual(f.calls.files,[{path:'/eagler-startup.rgba',length:1280*960*4}]);
  }
  const f=fixture();
  await assert.rejects(f.install(f.core,{game:'th10'}),/timestamp/);
});
