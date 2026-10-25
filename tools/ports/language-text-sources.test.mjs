import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,rm,readFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join,dirname,resolve} from 'node:path';
import {prepareLanguagePackTextSources,resolveLanguageTextSource,languageSourceFnv} from './language-text-sources.mjs';

test('translated MSG bytes override original DATA only at the declared mounted member',async t=>{
  const output=await mkdtemp(join(tmpdir(),'language-text-sources-'));
  t.after(async()=>{assert.equal(dirname(resolve(output)),resolve(tmpdir()));await rm(output,{recursive:true,force:true});});
  const declaration={mountRoot:'/thcrap/title/',assets:['msg.dat','ecl.dat']},translated=Buffer.from('translated message');
  const translations=await prepareLanguagePackTextSources({output,declaration,packs:{en:{files:[
    {path:'/thcrap/another/msg.dat',bytes:Buffer.from('wrong game')},{path:'/thcrap/title/msg.dat',bytes:translated},
  ]}}});
  const original={kind:'original-data',path:'original'},ecl={kind:'original-data',path:'script'},sources={files:{'msg.dat':original,'ecl.dat':ecl},translations};
  const chosen=resolveLanguageTextSource(sources,{language:'en',original:false,asset:'msg.dat'});
  assert.equal(chosen.kind,'language-pack');assert.equal(chosen.member,'/thcrap/title/msg.dat');assert.deepEqual(await readFile(chosen.path),translated);
  assert.equal(chosen.fnv64,languageSourceFnv(translated));
  assert.deepEqual(resolveLanguageTextSource(sources,{language:'en',original:true,asset:'msg.dat'}),original);
  assert.deepEqual(resolveLanguageTextSource(sources,{language:'en',original:false,asset:'ecl.dat'}),ecl);
  assert.throws(()=>resolveLanguageTextSource(sources,{language:'en',original:false,asset:'missing.dat'}));
});
test('translated source preparation rejects path traversal and duplicate member ownership',async()=>{
  const args={output:tmpdir(),declaration:{mountRoot:'/thcrap/title/',assets:['msg.dat']}};
  await assert.rejects(prepareLanguagePackTextSources({...args,packs:{'../escape':{files:[]}}}));
  await assert.rejects(prepareLanguagePackTextSources({...args,packs:{en:{files:[{path:'/thcrap/title/msg.dat',bytes:[1]},{path:'/thcrap/title/msg.dat',bytes:[2]}]}}}));
});
