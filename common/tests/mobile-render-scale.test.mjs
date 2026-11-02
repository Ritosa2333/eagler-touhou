import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {createRequire} from 'node:module';
const {parse}=createRequire(new URL('../../launcher/package.json',import.meta.url))('acorn');
for(const game of ['th15','th15-mp']){
 const source=readFileSync(new URL(`../../ports/${game}/web/sdl-runtime/shell.mjs`,import.meta.url),'utf8');
 const ast=parse(source,{ecmaVersion:'latest',sourceType:'module'});
 const node=ast.body.find(n=>n.type==='FunctionDeclaration'&&n.id.name==='renderScale');
 assert.ok(node);
 for(const coarse of [false,true]){
  const resolve=runInNewContext(source.slice(node.start,node.end)+';renderScale',{
   matchMedia:query=>{assert.equal(query,'(any-pointer: coarse)');return {matches:coarse};},
  });
  assert.equal(resolve({},0),coarse?1:2);
  assert.equal(resolve({},NaN),coarse?1:2);
  assert.equal(resolve({},1),1);assert.equal(resolve({},2),2);
  assert.equal(resolve({renderScale:1},2),1);assert.equal(resolve({renderScale:2},1),2);
 }
 const resolve=runInNewContext(source.slice(node.start,node.end)+';renderScale',{});
 assert.equal(resolve({},0),2,'desktop fallback without pointer-query support');
}
console.log('TH15 desktop/tablet resolution defaults and explicit-setting precedence: PASS');
