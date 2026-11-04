import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';

globalThis.addEventListener=()=>{};
globalThis.location={origin:'http://localhost'};
const keyboard=new URL('../browser/directory-keyboard.mjs',import.meta.url).href;

for(const game of ['th01','th02','th04']) test(`${game} retains short keyboard input until sampled`,async()=>{
  const root=new URL(`../../ports/${game}/web/`,import.meta.url);
  let source=await readFile(new URL('managed-input.mjs',root),'utf8');
  source=source.replace("'./directory-keyboard.mjs'",JSON.stringify(keyboard))
    .replace("'./touch-input.js'",JSON.stringify(new URL('touch-input.js',root).href));
  const {managedInput}=await import(`data:text/javascript;base64,${Buffer.from(source).toString('base64')}`);
  const input=managedInput({game,epoch:1,active:()=>true,options:()=>({touchEnabled:false})});
  const key=(code,down)=>input.key({code,down});
  const sample=()=>game==='th01'?input.nativeSample():input.sample();
  const shoot=game==='th01'?4:32,bomb=game==='th01'?4096:16;
  key('KeyZ',true);key('KeyZ',false);
  assert.equal(sample()&shoot,shoot);assert.equal(sample()&shoot,0);
  key(game==='th01'?'KeyC':'KeyX',true);key(game==='th01'?'KeyC':'KeyX',false);
  assert.equal(sample()&bomb,bomb);assert.equal(sample()&bomb,0);
  key('ArrowLeft',true);assert.notEqual(sample()&(game==='th01'?1:4),0);
  assert.notEqual(sample()&(game==='th01'?1:4),0);key('ArrowLeft',false);
  assert.equal(sample()&(game==='th01'?1:4),0);
  key('KeyZ',true);key('KeyZ',false);input.clear();assert.equal(sample(),0);
});
