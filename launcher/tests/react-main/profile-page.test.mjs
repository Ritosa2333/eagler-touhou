/** Mounted production profile. Checks section ownership and stored visibility, not CSS pixels. */
import {test, beforeAll, afterAll, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';

const project=fileURLToPath(new URL('../../',import.meta.url));
let env,work,React,createRoot,owners,root,moveHistory;
beforeAll(async()=>{
  env=installMountedDom();React=await import('react');({createRoot}=await import('react-dom/client'));
  // This fixture checks UI ownership. Canvas drawing is covered by renderer/browser checks.
  const context=new Proxy({measureText:text=>({width:String(text).length*7})},{get:(target,key)=>target[key]??(()=>{})});
  env.window.HTMLCanvasElement.prototype.getContext=()=>context;
  Object.defineProperty(env.document,'fonts',{configurable:true,value:{ready:Promise.resolve()}});
  work=await mkdtemp(resolve(project,'.cache/profile-page-'));
  await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`
    export {ProfilePage} from './app/components/launcher/ProfilePreview.tsx';
    export {CareerShowcase} from './app/components/launcher/CareerShowcase.tsx';
    export {LocaleProvider,translate} from './app/i18n.tsx';
    export {writeBookPreference} from './app/components/launcher/book-preferences.ts';
    export {MemoryRouter,useNavigate} from 'react-router';
  `},bundle:true,platform:'node',format:'esm',jsx:'automatic',packages:'external',loader:{'.webp':'dataurl'},outfile:resolve(work,'owners.mjs'),plugins:[authoredSourcesPlugin(project)],logLevel:'silent'});
  owners=await import(pathToFileURL(resolve(work,'owners.mjs')).href);
});
afterEach(async()=>{if(root)await React.act(async()=>root.unmount());root=null;env.document.body.replaceChildren();env.window.localStorage.clear();assert.deepEqual(env.errors.splice(0),[]);});
afterAll(async()=>{env?.close();if(work)await rm(work,{recursive:true,force:true});});
function HistoryProbe(){moveHistory=owners.useNavigate();return null;}
async function mount(Component=owners.ProfilePage,props={},initialEntries=['/profile.html?lang=en']){const container=env.document.createElement('div');env.document.body.append(container);root=createRoot(container);await React.act(async()=>root.render(React.createElement(owners.MemoryRouter,{initialEntries},React.createElement(owners.LocaleProvider,{locale:'en'},React.createElement(Component,props),React.createElement(HistoryProbe)))));}

test('public navigation locates sections already present on the page',async()=>{
  await mount();
  const sections=['career','records','content','friends'].map(key=>env.document.getElementById('profile-'+key));
  assert.ok(sections.every(Boolean));
  const links=[...env.document.querySelectorAll('.profile-tabs a')];
  assert.deepEqual(links.map(link=>link.hash),['#profile-overview','#profile-career','#profile-records','#profile-content','#profile-friends']);
  assert.ok(links.every(link=>new URL(link.href).searchParams.get('lang')==='en'));
  await React.act(async()=>links.find(link=>link.hash==='#profile-records').click());
  for(const node of sections)assert.equal(env.document.getElementById(node.id),node,'anchor selection must not swap sections');
  assert.equal(env.document.querySelectorAll('.profile-name .profile-bio').length,1);
  assert.equal(env.document.querySelector('.profile-aside .profile-bio'),null);
  assert.equal(env.document.body.textContent.includes(owners.translate('en','profile.recent')),false);
});

test('profile deep links and Back/Forward select their actual anchor',async()=>{
  await mount(owners.ProfilePage,{},['/profile.html?lang=en#profile-career']);
  const current=()=>env.document.querySelector('.profile-tabs [aria-current=location]')?.hash;
  assert.equal(current(),'#profile-career');
  const career=env.document.getElementById('profile-career');
  await React.act(async()=>env.document.querySelector('.profile-tabs a[href$="#profile-records"]').click());
  assert.equal(current(),'#profile-records');
  await React.act(async()=>moveHistory(-1));assert.equal(current(),'#profile-career');
  await React.act(async()=>moveHistory(1));assert.equal(current(),'#profile-records');
  assert.equal(env.document.getElementById('profile-career'),career);
});

test('legacy display preferences still restrict the visitor view',async()=>{
  owners.writeBookPreference('player-profile-display-v1',{visible:{career:false,records:true,content:false,friends:true,recent:true},order:['favorites','recent','career']});
  await mount();
  assert.ok(env.document.getElementById('profile-career'),'self view can inspect its own hidden content');
  await React.act(async()=>env.document.querySelector('.profile-perspective').click());
  assert.equal(env.document.getElementById('profile-career'),null);
  assert.equal(env.document.getElementById('profile-content'),null);
  assert.ok(env.document.getElementById('profile-records'));
  assert.ok(env.document.getElementById('profile-friends'));
  assert.equal(env.document.querySelectorAll('.profile-tabs a').length,3);
  assert.equal(env.document.querySelector('.profile-reminder-demo'),null);
  assert.equal(env.document.querySelector('.profile-social-tools label'),null);
});

test('visitor chart exposes viewing and export controls without a write path',async()=>{
  let saves=0;
  await mount(owners.CareerShowcase,{records:{},onSave(){saves++;},initiallyOpen:true,readOnly:true});
  assert.equal(env.document.querySelector('.career-viewer input[type=file]'),null);
  const labels=[...env.document.querySelectorAll('.career-viewer button')].map(button=>button.textContent);
  for(const key of ['career.importJson','career.save','career.edit'])assert.equal(labels.includes(owners.translate('en',key)),false);
  await React.act(async()=>env.document.querySelector('.career-check [role=checkbox]').click());
  assert.equal(env.window.localStorage.length,0,'visitor display adjustments stay in this view');
  assert.equal(saves,0);
});
