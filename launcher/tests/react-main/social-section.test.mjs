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
let env,work,React,createRoot,components,root,originalFetch,originalLocation,originalLocalStorage,originalCrypto;

beforeAll(async()=>{
  env=installMountedDom();React=await import('react');({createRoot}=await import('react-dom/client'));
  originalFetch=globalThis.fetch; originalLocation=Object.getOwnPropertyDescriptor(globalThis,'location');
  originalLocalStorage=Object.getOwnPropertyDescriptor(globalThis,'localStorage');originalCrypto=Object.getOwnPropertyDescriptor(globalThis,'crypto');
  Object.defineProperty(globalThis,'location',{configurable:true,value:env.window.location});
  Object.defineProperty(globalThis,'localStorage',{configurable:true,value:env.window.localStorage});
  Object.defineProperty(globalThis,'crypto',{configurable:true,value:{randomUUID:()=>`social-${Math.random()}`}});
  work=await mkdtemp(resolve(project,'.cache/social-section-'));
  await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`
    export {SocialProvider,SocialSection,SocialFollowControl} from './app/components/launcher/SocialSection.tsx';
    export {notifyAccountChange} from './app/services/account-session.ts';
    export {useAccountSession} from './app/services/account-session.ts';
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
  delete globalThis.__socialViewer;env.window.localStorage.clear();
  assert.deepEqual(env.errors.splice(0),[]);
});

afterAll(async()=>{
  env?.close();
  if(originalLocation)Object.defineProperty(globalThis,'location',originalLocation);else delete globalThis.location;
  if(originalLocalStorage)Object.defineProperty(globalThis,'localStorage',originalLocalStorage);else delete globalThis.localStorage;
  if(originalCrypto)Object.defineProperty(globalThis,'crypto',originalCrypto);else delete globalThis.crypto;
  if(work)await rm(work,{recursive:true,force:true});
});

function json(data,status=200){return new Response(JSON.stringify(data),{status,headers:{'Content-Type':'application/json'}});}
function useApi(snapshot){
  const calls=[],reads=[];globalThis.__socialViewer={username:'alice',name:'Alice'};
  globalThis.fetch=async(url,init={})=>{
    const path=new URL(String(url),env.window.location.href).pathname;
    if(path==='/api/ready')return json({ready:true});
    if(path==='/api/auth/get-session')return json({user:globalThis.__socialViewer});
    if(path==='/api/v1/profiles/alice/social'&&init.method!=='POST'){reads.push(path);return json(snapshot);}
    if(path==='/api/v1/me/following'&&init.method==='POST'){
      const body=JSON.parse(init.body);calls.push(body);snapshot.following=body.follow;return json({following:body.follow});
    }
    return json({message:'not found'},404);
  };
  return {calls,reads};
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
  const {calls,reads}=useApi(snapshot);
  function ProfileSocial(){return React.createElement(React.Fragment,null,
    React.createElement(components.SocialProvider,{username:'alice'},React.createElement(React.Fragment,null,
      React.createElement(components.SocialFollowControl,{username:'alice'}),
      React.createElement(components.SocialSection,{username:'alice'}))));}
  const page=await mount(React.createElement(ProfileSocial));
  await waitFor(()=>reads.length===1,'the shared provider fetches one snapshot');
  assert.equal(reads.length,1,'the profile section and follow action share one social request');
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
    React.createElement(components.SocialProvider,{username:'alice'},React.createElement(React.Fragment,null,
      React.createElement(components.SocialFollowControl,{username:'alice'}),
      React.createElement(components.SocialSection,{username:'alice'}))));}
  const page=await mount(React.createElement(ProfileSocial));
  assert.match(page.textContent,/This list is private/);
  assert.match(page.textContent,/Sign in to follow/);
  assert.equal(page.querySelector('a[href*="login.html"]')?.textContent,'Sign in to follow');
  assert.equal(page.querySelector('.profile-social-users'),null);
});

function deferred(){let resolve;const promise=new Promise(done=>{resolve=done;});return {promise,resolve};}
async function waitFor(predicate,message){
  await React.act(async()=>{for(let attempt=0;attempt<80&&!predicate();attempt++)await new Promise(resolve=>setTimeout(resolve,2));});
  assert.ok(predicate(),message);
}

test('refreshes deduplicated social requests, ignores late results, and reloads for a new viewer',async()=>{
  let viewer='alice';const sessions=[];const requests=[];
  globalThis.fetch=(url,init={})=>{
    const path=new URL(String(url),env.window.location.href).pathname;
    if(path==='/api/ready')return Promise.resolve(json({ready:true}));
    if(path==='/api/auth/get-session'){sessions.push(viewer);return Promise.resolve(json({user:{username:viewer,name:viewer}}));}
    if(path==='/api/v1/profiles/alice/social'){
      const task=deferred();requests.push({task,signal:init.signal});return task.promise;
    }
    return Promise.resolve(json({message:'not found'},404));
  };
  function ViewerBoundSocial(){
    const {state}=components.useAccountSession();
    if(state.status==='loading')return null;
    const viewer=state.status==='member'?state.user.username:state.status;
    return React.createElement(components.SocialProvider,{key:viewer,username:'alice'},React.createElement(components.SocialSection,{username:'alice'}));
  }
  const page=await mount(React.createElement(ViewerBoundSocial));
  await waitFor(()=>requests.length===1,`the signed-in viewer loads one social snapshot (sessions=${sessions.join(',')}, requests=${requests.length})`);
  await React.act(async()=>env.window.dispatchEvent(new env.window.Event('account-social-change')));
  await waitFor(()=>requests.length===2,'a social update starts one replacement request');
  assert.equal(requests[0].signal.aborted,true,'the older request is cancelled on refresh');
  await React.act(async()=>requests[1].task.resolve(json({visible:true,following:false,friends:[{username:'current',nickname:'Current'}],followingUsers:[]})));
  await waitFor(()=>page.textContent.includes('Current'),'the latest social snapshot is visible');
  await React.act(async()=>requests[0].task.resolve(json({visible:true,following:false,friends:[{username:'stale',nickname:'Stale'}],followingUsers:[]})));
  assert.match(page.textContent,/Current/);assert.doesNotMatch(page.textContent,/Stale/);

  viewer='bob';const previousSessionCalls=sessions.length;
  await React.act(async()=>components.notifyAccountChange());
  await waitFor(()=>sessions.length>previousSessionCalls&&requests.length===3,'the changed viewer gets a fresh social request');
  await React.act(async()=>requests[2].task.resolve(json({visible:true,following:true,friends:[{username:'new-viewer',nickname:'New viewer'}],followingUsers:[]})));
  await waitFor(()=>page.textContent.includes('New viewer'),'the new viewer receives the updated relationship state');
  assert.doesNotMatch(page.textContent,/Current|Stale/);
});
