/** Real documentation route hydration; synthetic DOM does not prove layout. */
import {test,beforeAll,afterAll,afterEach,vi} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp,rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';
const project=fileURLToPath(new URL('../../',import.meta.url));
let env,work,api,React,root,hydrateRoot,renderToString,originalFetch;
beforeAll(async()=>{
  env=installMountedDom();
  vi.stubGlobal('AbortController',env.window.AbortController);
  env.window.Element.prototype.getAnimations ??= ()=>[];
  originalFetch=globalThis.fetch;globalThis.fetch=async()=>({ok:false});
  env.window.SVGElement.prototype.getTotalLength=()=>0;
  React=await import('react');({hydrateRoot}=await import('react-dom/client'));({renderToString}=await import('react-dom/server'));
  work=await mkdtemp(resolve(project,'.cache/documentation-hydration-'));
  await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`export {default as HelpRoute} from './app/routes/help.tsx';export {helpDocuments} from './app/components/documents/catalog.ts';export {translate} from './app/i18n.tsx';export {createMemoryRouter,RouterProvider} from 'react-router';`},outfile:resolve(work,'api.mjs'),bundle:true,packages:'external',platform:'node',format:'esm',jsx:'automatic',loader:{'.css':'empty','.webp':'dataurl'},logLevel:'silent',plugins:[authoredSourcesPlugin(project),{name:'revision-fixture',setup(ctx){ctx.onResolve({filter:/^virtual:document-history$/},()=>({path:'revision',namespace:'fixture'}));ctx.onLoad({filter:/.*/,namespace:'fixture'},()=>({contents:'export default {}'}));}}]});
  api=await import(pathToFileURL(resolve(work,'api.mjs')).href);
});
afterEach(async()=>{if(root)await React.act(async()=>root.unmount());root=null;env.document.body.replaceChildren();env.document.documentElement.removeAttribute('data-ui-locale');env.window.localStorage.clear();assert.deepEqual(env.errors.splice(0),[]);});
afterAll(async()=>{globalThis.fetch=originalFetch;vi.unstubAllGlobals();env?.close();await rm(work,{recursive:true,force:true});});
function application(path){const router=api.createMemoryRouter([{path:'*',element:React.createElement(api.HelpRoute)}],{initialEntries:[path]});return React.createElement(api.RouterProvider,{router});}
for(const path of ['/about.html','/faq.html','/guide.html','/multiplayer.html','/docs/multiplayer','/docs/multiplayer/coop','/docs/multiplayer/th06','/docs/multiplayer/th07','/docs/multiplayer/th19'])for(const locale of ['en','ja'])test(`${path} query ${locale} hydrates localized document navigation and pagination`,async()=>{
  const container=env.document.createElement('div');env.document.body.append(container);
  container.innerHTML=renderToString(application(path));
  const recovered=[];
  await React.act(async()=>{root=hydrateRoot(container,application(path+'?lang='+locale),{onRecoverableError:error=>recovered.push(error.message)});});
  const sidebar=container.querySelector('#nd-sidebar');assert.ok(sidebar);
  assert.ok(sidebar.textContent.includes(api.translate(locale,'document.title')));
  assert.ok(sidebar.textContent.includes(api.translate(locale,'nav.help')));
  const articles=api.helpDocuments.filter(page=>page.id!=='about');
  for(const page of articles){const multiplayer=page.id==='multiplayer';const href=multiplayer?'/docs/multiplayer':`/${page.path}`;const links=[...sidebar.querySelectorAll(`a[href="${href}?lang=${locale}"]`)];assert.ok(links.some(link=>link.textContent===api.translate(locale,multiplayer?'document.multiplayer.title':page.title)),`${locale} ${href}: ${links.map(link=>link.textContent)}`);}
  assert.equal(sidebar.querySelector('a[href*="/about.html"]'),null,'the help sidebar lists help articles');
  const neighbours={
    '/guide.html':['/faq.html'], '/faq.html':['/guide.html','/docs/multiplayer'],
    '/docs/multiplayer':['/faq.html','/docs/multiplayer/coop'],
    '/docs/multiplayer/coop':['/docs/multiplayer','/docs/multiplayer/th06'],
    '/docs/multiplayer/th06':['/docs/multiplayer/coop','/docs/multiplayer/th07'],
    '/docs/multiplayer/th07':['/docs/multiplayer/th06','/docs/multiplayer/th08'],
    '/docs/multiplayer/th19':['/docs/multiplayer/th09'],
  };
  const pagination=[...container.querySelectorAll('#nd-page > nav a[href]')];
  assert.deepEqual(pagination.map(link=>link.getAttribute('href')),(neighbours[path]??[]).map(href=>`${href}?lang=${locale}`));
  if(path.endsWith('/th07')) {
    const article=container.querySelector('[data-guide-topic="th07"]');
    assert.equal(article.querySelectorAll('h2').length,3);
    assert.equal(article.querySelector('table'),null);
    assert.ok(article.querySelector(`a[href="/docs/multiplayer/coop?lang=${locale}"]`));
  }
  if(path.endsWith('/th06')) assert.equal(container.querySelector('[data-guide-topic="th06"]').textContent,api.translate(locale,'guide.none'));
  if(path.endsWith('/coop')) {
    const article=container.querySelector('[data-guide-topic="coop"]');
    const summary=article.firstElementChild;
    assert.equal(summary.tagName,'TABLE');
    const links=[...summary.querySelectorAll('a[href^="#"]')];
    assert.equal(links.length,9);
    assert.equal(links[0].hash,'#challenge-mode');
    for(const link of links) assert.ok(article.querySelector(`h2${link.hash}`),link.hash);
    const exceptions=article.lastElementChild;
    assert.equal(exceptions.getAttribute('role'),'note');
    assert.ok(exceptions.querySelector(`a[href="/docs/multiplayer/th10?lang=${locale}"]`));
  }
  assert.deepEqual(recovered,[]);
});
