/** Real documentation route hydration; synthetic DOM does not prove layout. */
import {test,beforeAll,afterAll,afterEach} from 'vitest';
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
  originalFetch=globalThis.fetch;globalThis.fetch=async()=>({ok:false});
  env.window.SVGElement.prototype.getTotalLength=()=>0;
  React=await import('react');({hydrateRoot}=await import('react-dom/client'));({renderToString}=await import('react-dom/server'));
  work=await mkdtemp(resolve(project,'.cache/documentation-hydration-'));
  await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`export {default as HelpRoute} from './app/routes/help.tsx';export {helpDocuments} from './app/components/documents/catalog.ts';export {translate} from './app/i18n.tsx';export {createMemoryRouter,RouterProvider} from 'react-router';`},outfile:resolve(work,'api.mjs'),bundle:true,packages:'external',platform:'node',format:'esm',jsx:'automatic',loader:{'.css':'empty','.webp':'dataurl'},logLevel:'silent',plugins:[authoredSourcesPlugin(project),{name:'revision-fixture',setup(ctx){ctx.onResolve({filter:/^virtual:document-history$/},()=>({path:'revision',namespace:'fixture'}));ctx.onLoad({filter:/.*/,namespace:'fixture'},()=>({contents:'export default {}'}));}}]});
  api=await import(pathToFileURL(resolve(work,'api.mjs')).href);
});
afterEach(async()=>{if(root)await React.act(async()=>root.unmount());root=null;env.document.body.replaceChildren();env.document.documentElement.removeAttribute('data-ui-locale');env.window.localStorage.clear();assert.deepEqual(env.errors.splice(0),[]);});
afterAll(async()=>{globalThis.fetch=originalFetch;env?.close();await rm(work,{recursive:true,force:true});});
function application(path){const router=api.createMemoryRouter([{path:'*',element:React.createElement(api.HelpRoute)}],{initialEntries:[path]});return React.createElement(api.RouterProvider,{router});}
for(const path of ['/about.html','/faq.html','/guide.html','/multiplayer.html'])for(const locale of ['en','ja'])test(`${path} query ${locale} hydrates localized document navigation and pagination`,async()=>{
  const container=env.document.createElement('div');env.document.body.append(container);
  container.innerHTML=renderToString(application(path));
  const recovered=[];
  await React.act(async()=>{root=hydrateRoot(container,application(path+'?lang='+locale),{onRecoverableError:error=>recovered.push(error.message)});});
  const sidebar=container.querySelector('#nd-sidebar');assert.ok(sidebar);
  for(const page of api.helpDocuments){const links=[...sidebar.querySelectorAll(`a[href="/${page.path}?lang=${locale}"]`)];assert.ok(links.some(link=>link.textContent===api.translate(locale,page.title)),`${locale} ${page.path}: ${links.map(link=>link.textContent)}`);}
  for(const link of container.querySelectorAll('#nd-page a[href]')){
    const page=api.helpDocuments.find(page=>link.getAttribute('href')===`/${page.path}?lang=${locale}`);
    if(page)assert.ok(link.textContent.includes(api.translate(locale,page.title)),link.textContent);
  }
  assert.deepEqual(recovered,[]);
});
