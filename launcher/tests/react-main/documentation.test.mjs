import {test,beforeAll,afterAll} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp,mkdir,writeFile,readFile,rm} from 'node:fs/promises';
import {execFileSync} from 'node:child_process';
import {resolve,dirname} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {renderToStaticMarkup} from 'react-dom/server';
import {createElement} from 'react';
const project=fileURLToPath(new URL('../../',import.meta.url));
let work,api;
beforeAll(async()=>{
  work=await mkdtemp(resolve(project,'.cache/documentation-test-'));
  const outfile=resolve(work,'api.mjs');
  await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`export * from './app/components/documents/content.tsx';export * from './app/components/documents/catalog.ts';export * from './scripts/ui-rewrite/document-history.ts';export * from './src/launcher/i18n.mts';`},outfile,bundle:true,packages:'external',platform:'node',format:'esm',jsx:'automatic',logLevel:'silent'});
  api=await import(pathToFileURL(outfile).href);
});
afterAll(async()=>{await rm(work,{recursive:true,force:true});});
const markup=body=>renderToStaticMarkup(createElement('div',null,body));
test('authored headings and localized safe links produce matching TOC anchors',()=>{
  const {body,toc}=api.prepareHelpDocument('<h1>Title</h1><h2>One &amp; two</h2><h2>One &amp; two</h2><a href="lobby.html">Lobby</a><a href="compatibility.html">Help</a><a href="#section-2">Jump</a>','/app/','ja');
  const html=markup(body);
  assert.deepEqual(toc,[{title:'One & two',url:'#section-1',depth:2},{title:'One & two',url:'#section-2',depth:2}]);
  assert.ok(!html.includes('<h1'));assert.ok(html.includes('id="section-2"'));
  assert.ok(html.includes('/app/lobby.html?lang=ja'));assert.ok(html.includes('/app/compatibility.ja.html'));assert.ok(html.includes('href="#section-2"'));
});
test('document rendering drops executable content and limits nested fragments',()=>{
  const source='<script>alert(1)</script><svg onload="bad()"></svg><iframe src="https://example.com"></iframe><a href="javascript:bad()" onclick="bad()" style="color:red">link</a><img src="data:text/html,bad"/><p class="unknown markdown-blockquote">safe</p>';
  const html=markup(api.prepareHelpDocument(source,'/','en').body);
  assert.ok(!/script|svg|iframe|javascript|onclick|style=|data:text|unknown/.test(html));assert.ok(html.includes('markdown-blockquote'));assert.ok(html.includes('safe'));
  assert.doesNotThrow(()=>markup(api.prepareHelpDocument('<div>'.repeat(120)+'hidden'+'</div>'.repeat(120),'/','en').body));
});
test('document history follows content provenance and detects uncommitted edits',async()=>{
  const repo=resolve(work,'repo');await mkdir(repo);
  const git=(...args)=>execFileSync('git',['-c','user.name=Doc Writer','-c','user.email=docs@example.invalid',...args],{cwd:repo,encoding:'utf8',stdio:['ignore','pipe','pipe']});
  git('init','-q');
  for(const doc of api.helpDocuments){await mkdir(dirname(resolve(repo,doc.source)),{recursive:true});await writeFile(resolve(repo,doc.source),'Published content\n');}
  for(const locale of ['en','ja']){await mkdir(resolve(repo,'messages'),{recursive:true});await writeFile(resolve(repo,`messages/${locale}.json`),JSON.stringify(Object.fromEntries(api.helpDocuments.map(doc=>[doc.content.replaceAll('.','_'),'Published content'])),null,2)+'\n');}
  git('add','.');git('commit','-qm','Initial documents');
  const original=api.readDocumentHistory(repo);assert.equal(original['zh-CN'].faq.author,'Doc Writer');assert.equal(original.en.faq.author,'Doc Writer');
  const file=resolve(repo,'messages/en.json'),catalog=JSON.parse(await readFile(file,'utf8'));catalog.other='An unrelated control';await writeFile(file,JSON.stringify(catalog,null,2)+'\n');
  git('add','.');git('-c','user.name=Other Writer','commit','-qm','Change another control');
  assert.equal(api.readDocumentHistory(repo).en.faq.commit,original.en.faq.commit,'Unrelated message edits do not replace the document author');
  await writeFile(resolve(repo,'docs/FAQ.md'),'Uncommitted revision\n');catalog.content_faq='Uncommitted translation';await writeFile(file,JSON.stringify(catalog,null,2)+'\n');
  const dirty=api.readDocumentHistory(repo);assert.equal(dirty['zh-CN'].faq.pending,true);assert.equal(dirty.en.faq.pending,true);
  const japanese=await readFile(resolve(repo,'messages/ja.json'),'utf8');git('rm','messages/ja.json');git('commit','-qam','Remove translation fixture');await writeFile(resolve(repo,'messages/ja.json'),japanese);
  assert.equal(api.readDocumentHistory(repo).ja.about.pending,true,'Untracked translations are explicitly marked');
});

test('all documentation languages render their authored headings and preserve About contact links',()=>{
  for(const locale of ['zh-CN','en','ja'])for(const page of api.helpDocuments){
    const source=api.translateUi(locale,page.content),{body,toc}=api.prepareHelpDocument(source,'/app/',locale),html=markup(body);
    assert.ok(html.length>100,`${locale} ${page.id}`);assert.ok(toc.length>0);
    for(const heading of toc)assert.ok(html.includes(`id="${heading.url.slice(1)}"`));
    assert.ok(!html.includes('<h1'),'Page title is provided by DocsTitle');
    if(page.id==='about')assert.ok(html.includes('mailto:ayanews@outlook.com'));
  }
});
