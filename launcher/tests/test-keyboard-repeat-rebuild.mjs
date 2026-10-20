import {readFileSync} from 'node:fs';
import test from 'node:test';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
import {mkdir,mkdtemp,rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
// Locks the launcher-side keyboard ownership semantics fixed for th15's
// results-screen input residue: keyups are never dropped by mapping
// recognition, and a browser-reported auto-repeat rebuilds ownership after a
// runtime reset — but never after a cancelled lifecycle flush.
await mkdir('.cache',{recursive:true});const work=await mkdtemp(resolve('.cache/keyboard-repeat-'));
const outfile=resolve(work,'mapping.mjs');
let mappingApi;
try {
  await build({entryPoints:[resolve('src/launcher/keyboard-mapping.mts')],outfile,bundle:true,platform:'node',format:'esm',logLevel:'silent',
    plugins:[{name:'authored-contracts',setup(ctx){ctx.onResolve({filter:/\.mjs$/},args=>{if(!args.importer)return;const path=resolve(args.importer,'..',args.path).replace(/\.mjs$/,'.mts');try{readFileSync(path);return{path};}catch{}});}}]});
  mappingApi=await import(pathToFileURL(outfile).href);
} finally {if(!mappingApi) await rm(work,{recursive:true,force:true});}
const {KeyboardMapping}=mappingApi;
const context={target:{},game:'th15',epoch:1,launched:true,spectator:false};
const press=(mapping,code,keyCode,down,repeat=false)=>mapping.map({code,key:String.fromCharCode(keyCode).toLowerCase(),keyCode,location:0,repeat},down,'physical','standard',context);
const transitions=events=>events.map(({code,down})=>({code,down}));

test('a keyup is never dropped by mapping recognition',()=>{
  const mapping=new KeyboardMapping();
  // Fresh input still requires a recognized key.
  assert.equal(press(mapping,'F13',124,true),null,'unrecognized keydown stays rejected');
  // But its release passes through as a no-op instead of being dropped,
  // so no adapter can strand a held key by refusing the release.
  assert.deepEqual(press(mapping,'F13',124,false),[],'unrecognized keyup reaches the owners');
});

test('host-reported auto-repeat rebuilds ownership after a runtime reset',()=>{
  const mapping=new KeyboardMapping();
  assert.deepEqual(transitions(press(mapping,'KeyZ',90,true)),[{code:'KeyZ',down:true}]);
  // Runtime-side reset (the between-stage loading barrier): releases only.
  assert.deepEqual(transitions(mapping.clear(false)),[{code:'KeyZ',down:false}]);
  // The browser still reports Z held via auto-repeat: ownership rebuilds.
  assert.deepEqual(transitions(press(mapping,'KeyZ',90,true,true)),[{code:'KeyZ',down:true}],'repeat rebuilds a reset owner');
  // The rebuilt owner releases normally when the key is finally let go.
  assert.deepEqual(transitions(press(mapping,'KeyZ',90,false)),[{code:'KeyZ',down:false}]);
});

test('a cancelled lifecycle flush is never re-armed by auto-repeat',()=>{
  const mapping=new KeyboardMapping();
  assert.deepEqual(transitions(press(mapping,'KeyZ',90,true)),[{code:'KeyZ',down:true}]);
  assert.deepEqual(transitions(mapping.clear(true)),[{code:'KeyZ',down:false}]);
  assert.deepEqual(transitions(press(mapping,'KeyZ',90,true,true)),[],'repeat cannot re-arm cancelled input');
  assert.deepEqual(transitions(press(mapping,'KeyZ',90,true)),[{code:'KeyZ',down:true}],'a fresh DOWN still works');
});

test('the bundled fixture is cleaned up after the run',async()=>{await rm(work,{recursive:true,force:true});});
