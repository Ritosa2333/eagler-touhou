import test, {before} from 'node:test';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
import {fileURLToPath} from 'node:url';
let KeyboardMapping,wasdKeyboardAliases;
before(async()=>{
  const result=await build({stdin:{contents:`export {KeyboardMapping} from './src/launcher/keyboard-mapping.mts';export {wasdKeyboardAliases} from './src/launcher/keyboard-preferences.mts';`,resolveDir:fileURLToPath(new URL('../../',import.meta.url))},bundle:true,write:false,format:'esm',platform:'node'});
  ({KeyboardMapping,wasdKeyboardAliases}=await import('data:text/javascript;base64,'+Buffer.from(result.outputFiles[0].text).toString('base64')));
});
const event=(code,extra={})=>({type:'keydown',code,key:'',keyCode:0,location:0,...extra});
const context=()=>({target:{},game:'th06',epoch:1,launched:true,ready:true,spectator:false});
const compact=changes=>changes?.map(e=>[e.code,e.down]);

test('numeric editing preserves top-row, keypad, punctuation and edit keys',()=>{
  for(const code of [...Array.from({length:10},(_,i)=>'Digit'+i),...Array.from({length:10},(_,i)=>'Numpad'+i),
    'Minus','Period','NumpadSubtract','NumpadDecimal','Delete','Insert','Home','End','PageUp','PageDown','Backspace','Enter']){
    const owner=new KeyboardMapping(),ctx=context();
    assert.equal(owner.supports(code),true,code);
    assert.deepEqual(compact(owner.map(event(code),true,'physical','standard',ctx)),[[code,true]],code);
    assert.deepEqual(compact(owner.map(event(code),false,'physical','standard',ctx)),[[code,false]],code);
  }
});
test('the default mapping preserves original keys without adding WASD aliases',()=>{
  for(const code of ['ArrowUp','ArrowDown','ArrowLeft','ArrowRight','KeyW','KeyA','KeyS','KeyD','Space']){
    const owner=new KeyboardMapping(),ctx=context();
    assert.deepEqual(compact(owner.map(event(code),true,'physical','standard',ctx)),[[code,true]]);
    assert.deepEqual(compact(owner.map(event(code),false,'physical','standard',ctx)),[[code,false]]);
  }
});

test('the optional WASD preset aliases have the same canonical meaning as the original controls',()=>{
  for(const [input,output] of Object.entries({KeyW:'ArrowUp',KeyA:'ArrowLeft',KeyS:'ArrowDown',KeyD:'ArrowRight',KeyJ:'KeyZ',KeyK:'KeyX',KeyL:'ShiftLeft',Space:'ShiftLeft',KeyH:'KeyC'})){
    const owner=new KeyboardMapping(()=>wasdKeyboardAliases),ctx=context();
    assert.deepEqual(compact(owner.map(event(input),true,'physical','standard',ctx)),[[output,true]]);
    assert.deepEqual(compact(owner.map(event(input),false,'physical','standard',ctx)),[[output,false]]);
  }
});
test('releasing either alias preserves another physical or touch owner',()=>{
  const owner=new KeyboardMapping(()=>wasdKeyboardAliases),ctx=context();
  assert.deepEqual(compact(owner.map(event('KeyZ'),true,'physical','standard',ctx)),[['KeyZ',true]]);
  assert.deepEqual(owner.map(event('KeyJ'),true,'physical','standard',ctx),[]);
  assert.deepEqual(owner.map(event('KeyZ'),false,'physical','standard',ctx),[]);
  assert.deepEqual(compact(owner.map(event('KeyJ'),false,'physical','standard',ctx)),[['KeyZ',false]]);
  owner.map(event('KeyH'),true,'physical','standard',ctx);
  assert.deepEqual(owner.map(event('KeyC'),true,'virtual','standard',ctx),[]);
  assert.deepEqual(owner.map(event('KeyH'),false,'physical','standard',ctx),[]);
  assert.deepEqual(compact(owner.map(event('KeyC'),false,'virtual','standard',ctx)),[['KeyC',false]]);
});
test('modifier sides, aliases and degraded WebView releases retain independent owners',()=>{
  const owner=new KeyboardMapping(()=>wasdKeyboardAliases),ctx=context();
  owner.map(event('ShiftRight',{location:2}),true,'physical','standard',ctx);
  owner.map(event('KeyL'),true,'physical','standard',ctx);
  assert.deepEqual(compact(owner.map(event('Unidentified',{key:'shift',keyCode:16,location:2}),false,'physical','standard',ctx)),[['ShiftRight',false]]);
  assert.deepEqual(compact(owner.map(event('Unidentified',{key:'l',keyCode:76}),false,'physical','standard',ctx)),[['ShiftLeft',false]]);
  assert.deepEqual(compact(owner.map(event('Unidentified',{key:'j',keyCode:83}),true,'physical','standard',ctx)),[['KeyZ',true]],'key takes priority over a conflicting legacy keyCode');
  assert.deepEqual(compact(owner.map(event('Unidentified',{key:'Unidentified',keyCode:104,location:3}),true,'physical','standard',ctx)),[['Numpad8',true]],'legacy Numpad identity stays on the Numpad lane');
});
test('clear cancels physical and virtual owners and an old repeat cannot re-arm',()=>{
  const owner=new KeyboardMapping(()=>wasdKeyboardAliases),ctx=context();
  owner.map(event('KeyW'),true,'physical','standard',ctx);
  owner.map(event('KeyC'),true,'virtual','standard',ctx);
  assert.deepEqual(compact(owner.clear()),[['ArrowUp',false],['KeyC',false]]);
  assert.deepEqual(owner.map(event('KeyW',{repeat:true}),true,'physical','standard',ctx),[]);
  assert.deepEqual(owner.map(event('KeyW'),false,'physical','standard',ctx),[]);
  assert.deepEqual(owner.map(event('KeyC'),false,'virtual','standard',ctx),[]);
  assert.deepEqual(compact(owner.map(event('KeyW'),true,'physical','standard',ctx)),[['ArrowUp',true]]);
});
test('TH09 split profiles route the new keys through the selected keyboard device',()=>{
  for(const [profile,pairs] of Object.entries({'th09-left':{KeyW:'KeyT',KeyA:'KeyF',KeyS:'KeyB',KeyD:'KeyH',KeyJ:'KeyZ'},'th09-right':{KeyW:'Numpad8',KeyA:'Numpad4',KeyS:'Numpad2',KeyD:'Numpad6',KeyJ:'ArrowDown',KeyK:'ArrowRight',KeyL:'ArrowLeft'}})){
    for(const [key,output] of Object.entries(pairs)){
      const owner=new KeyboardMapping(()=>wasdKeyboardAliases);assert.deepEqual(compact(owner.map(event(key),true,'physical',profile,context())),[[output,true]]);
    }
  }
});
test('a new runtime epoch receives no held state or release from its predecessor',()=>{
  const owner=new KeyboardMapping(()=>wasdKeyboardAliases),ctx=context();owner.map(event('KeyJ'),true,'physical','standard',ctx);
  assert.deepEqual(compact(owner.map(event('KeyK'),true,'physical','standard',{...ctx,epoch:2})),[['KeyX',true]]);
});
