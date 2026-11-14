import {test} from 'node:test';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
import {pathToFileURL,fileURLToPath} from 'node:url';
const root = fileURLToPath(new URL('../../',import.meta.url));
const out = root+'/.cache/replay-practice-pointer-test.mjs';
await build({entryPoints:[root+'/app/services/replay-practice-pointer.ts'],outfile:out,bundle:true,platform:'node',format:'esm'});
const {bindReplayPracticePointer} = await import(pathToFileURL(out).href);
function fixture(game='th14') {
  const frame = new EventTarget(), attrs = new Map();
  let gameUrl='https://test.invalid/runtime/'+game+'/generation/th14.html'; frame.src='';
  frame.ownerDocument={baseURI:'https://test.invalid/'};
  frame.setAttribute=(n,v)=>attrs.set(n,v);frame.removeAttribute=n=>attrs.delete(n);
  const replace=()=>frame.contentDocument={URL:gameUrl,defaultView:new EventTarget()};replace();
  return {frame,attrs,replace,gameUrl(value){gameUrl=value;},send(target,detail){target.dispatchEvent(new CustomEvent('eagler-thprac-menu',{detail}));}};
}
test('current TH14 modal alone grants and releases replay pointer ownership',()=>{
 const f=fixture(),off=bindReplayPracticePointer(f.frame),target=f.frame.contentDocument.defaultView;
 assert.equal(f.attrs.size,0);
 for(const detail of [undefined,{}, {open:1},{open:'true'}]){f.send(target,detail);assert.equal(f.attrs.size,0);}
 f.send(target,{open:true});assert.equal(f.attrs.get('data-th14-practice-panel'),'open');
 f.send(target,{open:false});assert.equal(f.attrs.size,0);off();
});
test('load clears ownership and retires the previous document',()=>{
 const f=fixture(),off=bindReplayPracticePointer(f.frame),old=f.frame.contentDocument.defaultView;
 f.send(old,{open:true});assert.equal(f.attrs.size,1);
 f.replace();f.send(old,{open:true}); // stale events cannot grant into replacement
 f.frame.dispatchEvent(new Event('load'));assert.equal(f.attrs.size,0);
 f.send(old,{open:true});assert.equal(f.attrs.size,0);
 f.send(f.frame.contentDocument.defaultView,{open:true});assert.equal(f.attrs.size,1);off();
 assert.equal(f.attrs.size,0);f.send(f.frame.contentDocument.defaultView,{open:true});assert.equal(f.attrs.size,0);
});
for(const game of ['th13','th19','th20','th140'])test('other title '+game+' remains blocked',()=>{
 const f=fixture(game),off=bindReplayPracticePointer(f.frame);f.send(f.frame.contentDocument.defaultView,{open:true});assert.equal(f.attrs.size,0);off();
});
test('TH14 to another title drops previous modal ownership',()=>{
 const f=fixture(),off=bindReplayPracticePointer(f.frame),old=f.frame.contentDocument.defaultView;
 f.send(old,{open:true});f.gameUrl('https://test.invalid/runtime/th19/g/th19.html');f.replace();f.frame.dispatchEvent(new Event('load'));
 f.send(old,{open:true});f.send(f.frame.contentDocument.defaultView,{open:true});assert.equal(f.attrs.size,0);off();
});
