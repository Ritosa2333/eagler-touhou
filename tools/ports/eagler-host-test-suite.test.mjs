import assert from 'node:assert/strict';
import test from 'node:test';
import {mkdtempSync,mkdirSync,readFileSync,rmSync,writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join,resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {resolveEaglerHostTestTarget,validateEaglerHostTestTarget,assertFontAliasContract} from './eagler-host-test-suite.mjs';

const root=mkdtempSync(join(tmpdir(),'eagler-host-owner-'));
process.on('exit',()=>rmSync(root,{recursive:true,force:true}));
function fixtureUrl(path){return pathToFileURL(path).href;}
function target(owner,fontAliases){
  const base=join(root,'ports',owner),testPath=join(base,'portable','test-eagler-host.mjs');
  const runtimePath=join(base,'web','sdl-runtime','eagler-host.mjs');
  const shellPath=join(base,'web','sdl-runtime','shell.mjs');
  for(const path of [testPath,runtimePath,shellPath]){
    mkdirSync(join(path,'..'),{recursive:true});writeFileSync(path,'// synthetic owner source\n');
  }
  return{owner,fontAliases,testUrl:fixtureUrl(testPath),runtimeUrl:fixtureUrl(runtimePath),shellUrl:fixtureUrl(shellPath)};
}
const th08Fonts=[{language:null,source:'/msgothic.ttc',target:'/fonts/msgothic.ttc'}];
const th10Fonts=[
  {language:'jp',source:'/msgothic.ttc',target:'/fonts/msgothic.ttc'},
  {language:'chs',source:'/unifont.otf',target:'/fonts/simhei.ttf'},
];
const sourceRoot=process.env.EAGLER_HOST_SUITE_SOURCE_ROOT??resolve(import.meta.dirname,'../..');
const hostOwners=['th08','th08-mp','th10','th10-mp'];
const aliasesFor=owner=>owner.startsWith('th10')?th10Fonts:th08Fonts;
const hostPath=owner=>join(sourceRoot,'ports',owner,'web','sdl-runtime','eagler-host.mjs');
async function importHost(owner){return import(pathToFileURL(hostPath(owner)).href);}
function fontAliasFunctionSource(source){
  const match=source.match(/^export function ensureSharedFontAlias\([^\n]*\)\s*\{[\s\S]*?^\}/m);
  assert.ok(match,'the production Runtime must export ensureSharedFontAlias');
  return match[0];
}

test('explicit owner resolves the exact normal and MP Runtime source',()=>{
  const normalTarget=target('th10',th10Fonts);
  const normal=resolveEaglerHostTestTarget(normalTarget);
  assert.equal(normal.game,'th10');assert.equal(normal.variant,'normal');
  assert.match(normal.runtimeUrl,/ports\/th10\/web\/sdl-runtime\/eagler-host\.mjs$/);
  const mp=resolveEaglerHostTestTarget(target('th10-mp',th10Fonts));
  assert.equal(mp.game,'th10');assert.equal(mp.variant,'multiplayer');
  assert.match(mp.runtimeUrl,/ports\/th10-mp\/web\/sdl-runtime\/eagler-host\.mjs$/);
});

test('a copied wrapper or misdirected Runtime module fails on owner mismatch',()=>{
  assert.throws(()=>resolveEaglerHostTestTarget({...target('th10-mp',th10Fonts),owner:'th08'}),
    /Test owner does not match/);
  const wrongModule=target('th10',th10Fonts);
  wrongModule.runtimeUrl=target('th08').runtimeUrl;
  assert.throws(()=>validateEaglerHostTestTarget(wrongModule),/runtimeUrl must be/);
  const wrongShell=target('th10',th10Fonts);
  wrongShell.shellUrl=target('th08').shellUrl;
  assert.throws(()=>validateEaglerHostTestTarget(wrongShell),/shellUrl must be/);
});

test('font aliases are wrapper-supplied and the shared check executes every declared operation',()=>{
  const calls=[];
  function correct(Module,language){
    calls.push(language);
    const source=language==='chs'?'/unifont.otf':'/msgothic.ttc';
    const target=language==='chs'?'/fonts/simhei.ttf':'/fonts/msgothic.ttc';
    Module.FS.mkdirTree('/fonts');Module.FS.stat(source);try{Module.FS.unlink(target);}catch{}Module.FS.symlink(source,target);
    return{source,target};
  }
  assert.deepEqual(assertFontAliasContract(th10Fonts,correct),[
    {source:'/msgothic.ttc',target:'/fonts/msgothic.ttc'},
    {source:'/unifont.otf',target:'/fonts/simhei.ttf'},
  ]);
  assert.deepEqual(calls,['jp','chs']);

  function wrongChs(Module,language){
    if(language==='chs')return correct(Module,'jp');
    return correct(Module,language);
  }
  assert.throws(()=>assertFontAliasContract(th10Fonts,wrongChs),/Font alias .* declared Launcher paths/);
});

test('a different explicit product owner uses the same shared font assertion',()=>{
  function ensure(Module){
    Module.FS.mkdirTree('/fonts');Module.FS.stat('/msgothic.ttc');try{Module.FS.unlink('/fonts/msgothic.ttc');}catch{}
    Module.FS.symlink('/msgothic.ttc','/fonts/msgothic.ttc');
    return{source:'/msgothic.ttc',target:'/fonts/msgothic.ttc'};
  }
  const nextOwner=target('th-next',th08Fonts);
  const next=resolveEaglerHostTestTarget(nextOwner);
  assert.equal(next.game,'th-next');
  assert.deepEqual(assertFontAliasContract(next.fontAliases,ensure),[
    {source:'/msgothic.ttc',target:'/fonts/msgothic.ttc'},
  ]);
});

test('omitted or malformed font aliases fail before owner tests can register',()=>{
  const source=target('th10',th10Fonts);
  assert.throws(()=>resolveEaglerHostTestTarget({...source,fontAliases:[]}),/non-empty fontAliases/);
  assert.throws(()=>resolveEaglerHostTestTarget({...source,fontAliases:[{source:'/msgothic.ttc',target:'/fonts/msgothic.ttc'}]}),
    /language must be/);
  assert.throws(()=>resolveEaglerHostTestTarget({...source,fontAliases:[{language:'jp',target:'/fonts/msgothic.ttc'}]}),
    /source must be/);
  assert.throws(()=>resolveEaglerHostTestTarget({...source,fontAliases:[{language:'jp',source:'/msgothic.ttc',target:'/tmp/alias.ttc'}]}),
    /target must be under/);
});

test('wrong alias and absent Runtime font source fail the operation-level check',()=>{
  function missing(Module,language){
    const source=language==='chs'?'/unifont.otf':'/missing-msgothic.ttc';
    Module.FS.stat(source);
    return{source,target:language==='chs'?'/fonts/simhei.ttf':'/fonts/msgothic.ttc'};
  }
  assert.throws(()=>assertFontAliasContract(th10Fonts,missing),/missing path \/missing-msgothic\.ttc/);
});

test('all four real normal and MP Runtime font operations satisfy the independent file and directory model',async()=>{
  for(const owner of hostOwners){
    const host=await importHost(owner),aliases=aliasesFor(owner);
    assert.equal(typeof host.ensureSharedFontAlias,'function',owner+' exports the Runtime font operation');
    assert.deepEqual(assertFontAliasContract(aliases,host.ensureSharedFontAlias),aliases.map(({source,target})=>({source,target})));
  }
});

test('the real Runtime font operation rejects a missing declared source file',async()=>{
  for(const owner of hostOwners){
    const host=await importHost(owner),alias=aliasesFor(owner)[0];
    const absent={...alias,source:'/absent-by-test/'+alias.source.slice(1)};
    assert.throws(()=>assertFontAliasContract([absent],host.ensureSharedFontAlias),
      /Shared Runtime font unavailable/,owner+' must fail when its actual source font is absent');
  }
});

test('the operation-level filesystem rejects a single-point mutation removing production mkdirTree',async()=>{
  for(const owner of hostOwners){
    const original=fontAliasFunctionSource(readFileSync(hostPath(owner),'utf8'));
    const mkdirCall=/^\s*Module\.FS\.mkdirTree\('\/fonts'\);\s*$/m;
    assert.match(original,mkdirCall,owner+' production operation must create the target parent');
    const mutant=original.replace(mkdirCall,'');
    assert.notEqual(mutant,original,owner+' must produce the targeted no-mkdir mutation');
    const executable=mutant.replace(/^export function ensureSharedFontAlias/,'return function ensureSharedFontAlias');
    const noMkdirMutation=new Function(executable)();
    assert.throws(()=>assertFontAliasContract(aliasesFor(owner),noMkdirMutation),/missing symlink parent/,
      owner+' must not link into a directory that the operation failed to create');
  }
});
