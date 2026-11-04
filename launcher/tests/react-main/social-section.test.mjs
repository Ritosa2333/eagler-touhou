/** Social UI uses the relationship API responses and never fabricates people. */
import {test, beforeAll, afterAll, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';

const project=fileURLToPath(new URL('../../',import.meta.url));
let env,work,React,createRoot,components,root,originalFetch,originalLocation;

beforeAll(async()=>{
  env=installMountedDom();React=await import('react');({createRoot}=await import('react-dom/client'));
  originalFetch=globalThis.fetch; originalLocation=Object.getOwnPropertyDescriptor(globalThis,'location');
  Object.defineProperty(globalThis,'location',{configurable:true,value:env.window.location});
  work=await mkdtemp(resolve(project,'.cache/social-section-'));
  await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`
    export {SocialSection,SocialFollowControl} from './app/components/launcher/SocialSection.tsx';
    export {LocaleProvider} from './app/i18n.tsx';
    export {MemoryRouter} from 'react-router';
  `},bundle:true,platform:'node',format:'esm',jsx:'automatic',packages:'external',
    define:{'import.meta.env':JSON.stringify({VITE_ACCOUNTS_ENABLED:'true'})},
    outfile:resolve(work,'social.mjs'),plugins:[authoredSourcesPlugin(project)],logLevel:'silent'});
  components=await import(pathToFileURL(resolve(work,'social.mjs')).href);
});

afterEach(async()=>{
  if(root)await React.act(async()=>root.unmount());
  root=null;env.document.body.replaceChildren();
  globalThis.fetch=originalFetch;
  assert.deepEqual(env.errors.splice(0),[]);
});

afterAll(async()=>{
  env?.close();
  if(originalLocation)Object.defineProperty(globalThis,'location',originalLocation);else delete globalThis.location;
  if(work)await rm(work,{recursive:true,force:true});
});

function json(data,status=200){return new Response(JSON.stringify(data),{status,headers:{'Content-Type':'application/json'}});}
function useApi(snapshot){
  const calls=[];
  globalThis.fetch=async(url,init={})=>{
    const path=new URL(String(url),env.window.location.href).pathname;
    if(path==='/api/ready')return json({ready:true});
    if(path==='/api/v1/profiles/alice/social'&&init.method!=='POST')return json(snapshot);
    if(path==='/api/v1/me/following'&&init.method==='POST'){
      const body=JSON.parse(init.body);calls.push(body);snapshot.following=body.follow;return json({following:body.follow});
    }
    return json({message:'not found'},404);
  };
  return calls;
}
async function mount(children){
  const container=env.document.createElement('div');env.document.body.append(container);root=createRoot(container);
  await React.act(async()=>root.render(React.createElement(components.MemoryRouter,{initialEntries:['/u/alice']},
    React.createElement(components.LocaleProvider,{locale:'en'},children))));
  await React.act(async()=>{await new Promise(resolve=>setTimeout(resolve,0));});
  return container;
}

test('friends and following switch to server lists and the profile action persists follow state',async()=>{
  const snapshot={visible:true,following:false,friends:[{username:'bob',nickname:'Bob'}],followingUsers:[{username:'carol',nickname:'Carol'}]};
  const calls=useApi(snapshot);
  function ProfileSocial(){return React.createElement(React.Fragment,null,
    React.createElement(components.SocialFollowControl,{username:'alice'}),
    React.createElement(components.SocialSection,{username:'alice'}));}
  const page=await mount(React.createElement(ProfileSocial));
  assert.match(page.textContent,/Bob/);
  const follow=page.querySelector('button[aria-pressed="false"]');
  assert.ok(follow);
  await React.act(async()=>follow.dispatchEvent(new env.window.MouseEvent('click',{bubbles:true})));
  assert.deepEqual(calls,[{username:'alice',follow:true}]);
  assert.equal(page.querySelector('button[aria-pressed="true"]')?.textContent,'Following');
  await React.act(async()=>page.querySelectorAll('.profile-social-tools button')[1].dispatchEvent(new env.window.MouseEvent('click',{bubbles:true})));
  assert.match(page.textContent,/Carol/);
  assert.doesNotMatch(page.textContent,/Bob/);
});

test('hidden relationships are omitted and signed-out visitors get the login action',async()=>{
  const snapshot={visible:false,following:null,friends:[],followingUsers:[]};
  useApi(snapshot);
  function ProfileSocial(){return React.createElement(React.Fragment,null,
    React.createElement(components.SocialFollowControl,{username:'alice'}),
    React.createElement(components.SocialSection,{username:'alice'}));}
  const page=await mount(React.createElement(ProfileSocial));
  assert.match(page.textContent,/This list is private/);
  assert.match(page.textContent,/Sign in to follow/);
  assert.equal(page.querySelector('a[href*="login.html"]')?.textContent,'Sign in to follow');
  assert.equal(page.querySelector('.profile-social-users'),null);
});
