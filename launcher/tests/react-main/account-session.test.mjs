/** Mounted account consumers share identity and reject responses from retired sessions. */
import {test, beforeAll, afterAll, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';

const project=fileURLToPath(new URL('../../',import.meta.url));
let env,work,React,createRoot,owners,root,originalLocation,originalLocalStorage,originalCrypto;

beforeAll(async()=>{
  env=installMountedDom();React=await import('react');({createRoot}=await import('react-dom/client'));
  originalLocation=Object.getOwnPropertyDescriptor(globalThis,'location');
  originalLocalStorage=Object.getOwnPropertyDescriptor(globalThis,'localStorage');
  originalCrypto=Object.getOwnPropertyDescriptor(globalThis,'crypto');
  Object.defineProperty(globalThis,'location',{configurable:true,value:{origin:env.window.location.origin,href:env.window.location.href,pathname:'/',search:'',replace(){}}});
  Object.defineProperty(globalThis,'localStorage',{configurable:true,value:env.window.localStorage});
  Object.defineProperty(globalThis,'crypto',{configurable:true,value:{randomUUID:()=>`session-${Math.random()}`}});
  work=await mkdtemp(resolve(project,'.cache/account-session-mounted-'));
  await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`
    export {useAccountSession,notifyAccountChange} from './app/services/account-session.ts';
    export {AccountMenu} from './app/components/launcher/AccountMenu.tsx';
    export {LocaleProvider} from './app/i18n.tsx';
    export {MemoryRouter} from 'react-router';
  `},bundle:true,platform:'node',format:'esm',jsx:'automatic',packages:'external',
    define:{'import.meta.env':JSON.stringify({VITE_ACCOUNTS_ENABLED:'true'})},
    outfile:resolve(work,'owners.mjs'),plugins:[{name:'account-request-fixture',setup(context){
      context.onResolve({filter:/account-client/},()=>({path:'account-request-fixture',namespace:'fixture'}));
      context.onLoad({filter:/.*/,namespace:'fixture'},()=>({loader:'js',contents:`
        export class AccountRequestError extends Error {constructor(code,status){super(code);this.name='AccountRequestError';this.code=code;this.status=status;}}
        export function cancelAccountRequests() {}
        export async function accountRequest(path,body,options){
          const request=globalThis.__accountRequest;
          if(!request)throw Error('account request fixture is missing');
          return request(path,body,options);
        }
      `}));
    }},authoredSourcesPlugin(project)],logLevel:'silent'});
  owners=await import(pathToFileURL(resolve(work,'owners.mjs')).href);
});

afterEach(async()=>{
  delete globalThis.__accountRequest;
  if(root)await React.act(async()=>root.unmount());
  root=null;env.document.body.replaceChildren();env.window.localStorage.clear();
  assert.deepEqual(env.errors.splice(0),[]);
});

afterAll(async()=>{
  env?.close();
  if(originalLocation)Object.defineProperty(globalThis,'location',originalLocation);else delete globalThis.location;
  if(originalLocalStorage)Object.defineProperty(globalThis,'localStorage',originalLocalStorage);else delete globalThis.localStorage;
  if(originalCrypto)Object.defineProperty(globalThis,'crypto',originalCrypto);else delete globalThis.crypto;
  if(work)await rm(work,{recursive:true,force:true});
});

function deferred(){let resolve;const promise=new Promise(done=>{resolve=done;});return {promise,resolve};}
function SessionProbe({name}){
  const {state}=owners.useAccountSession();
  const value=state.status==='member'?`${state.user.username}:${state.user.name}`:state.status;
  return React.createElement('output',{'data-session-probe':name},value);
}
async function mount(){
  const container=env.document.createElement('div');env.document.body.append(container);root=createRoot(container);
  await React.act(async()=>root.render(React.createElement(owners.MemoryRouter,{initialEntries:['/']},
    React.createElement(owners.LocaleProvider,{locale:'en'},React.createElement(React.Fragment,null,
      React.createElement(SessionProbe,{name:'masthead'}),React.createElement(SessionProbe,{name:'page'}),
      React.createElement(owners.AccountMenu,{interactive:true}))))));
  return container;
}
async function waitFor(predicate,message){
  await React.act(async()=>{for(let attempt=0;attempt<80&&!predicate();attempt++)await new Promise(resolve=>setTimeout(resolve,2));});
  assert.ok(predicate(),message);
}
function probe(name){return env.document.querySelector(`[data-session-probe="${name}"]`)?.textContent;}

test('same-page and cross-tab identity changes are shared, and late account data cannot restore a retired user',async()=>{
  const sessions=[],profiles=[];
  globalThis.__accountRequest=(path)=>{
    if(path==='auth/get-session'){const task=deferred();sessions.push(task);return task.promise;}
    const match=path.match(/^v1\/profiles\/([^/]+)$/);
    if(match){const task=deferred();profiles.push({username:decodeURIComponent(match[1]),task});return task.promise;}
    throw Error(`Unexpected account request: ${path}`);
  };
  const page=await mount();
  await waitFor(()=>sessions.length===1,'all account consumers share the initial session request');
  assert.equal(sessions.length,1);
  await React.act(async()=>sessions[0].resolve({user:{username:'alice',name:'Alice'}}));
  await waitFor(()=>profiles.some(item=>item.username==='alice'),'the account menu loads Alice profile details');
  const alice=profiles.find(item=>item.username==='alice');

  await React.act(async()=>owners.notifyAccountChange());
  await waitFor(()=>sessions.length===2,'same-page account changes request one shared session refresh');
  assert.equal(sessions.length,2);
  assert.equal(probe('masthead'),'loading','the old identity is hidden while the new session is checked');
  await React.act(async()=>owners.notifyAccountChange());
  await waitFor(()=>sessions.length===3,'a newer session change replaces the older pending request');
  await React.act(async()=>sessions[2].resolve({user:{username:'bob',name:'Bob'}}));
  await waitFor(()=>profiles.some(item=>item.username==='bob'),'the account menu loads Bob profile details');
  const bob=profiles.find(item=>item.username==='bob');
  await React.act(async()=>bob.task.resolve({nickname:'Bobby',details:{avatar:''}}));
  await React.act(async()=>alice.task.resolve({nickname:'Alice stale',details:{avatar:''}}));
  await React.act(async()=>sessions[1].resolve({user:{username:'alice',name:'Alice late'}}));
  assert.equal(probe('masthead'),'bob:Bob');
  assert.equal(probe('page'),'bob:Bob');
  assert.match(page.querySelector('.masthead-account-entry')?.textContent??'',/Bobby/);
  assert.doesNotMatch(page.querySelector('.masthead-account-entry')?.textContent??'',/Alice/);

  const oldUrl=env.window.location.href;
  await React.act(async()=>env.window.dispatchEvent(new env.window.StorageEvent('storage',{key:'account-session-changed',newValue:'other-tab'})));
  await waitFor(()=>sessions.length===4,'a cross-tab change refreshes the shared session');
  assert.equal(env.window.location.href,oldUrl,'the page stays mounted while session state changes');
  await React.act(async()=>sessions[3].resolve({user:null}));
  await waitFor(()=>probe('masthead')==='guest','sign-out removes the current member identity');
  assert.equal(probe('page'),'guest');
  assert.equal(page.querySelector('button.masthead-account-entry'),null);
  assert.ok(page.querySelector('.account-login-link'));
  assert.doesNotMatch(page.textContent,/Bobby|Alice stale/);
});

test('a failed sign-out leaves the account menu open with its error',async()=>{
  globalThis.__accountRequest=async path=>{
    if(path==='auth/get-session')return {user:{username:'alice',name:'Alice'}};
    if(path==='v1/profiles/alice')return {nickname:'Alice profile',details:{avatar:''}};
    if(path==='auth/sign-out')throw Error('sign-out unavailable');
    throw Error(`Unexpected account request: ${path}`);
  };
  const page=await mount();
  await waitFor(()=>!!page.querySelector('button.masthead-account-entry'),'the account profile is ready');
  await React.act(async()=>{
    const trigger=page.querySelector('button.masthead-account-entry');
    trigger.dispatchEvent(new env.window.PointerEvent('pointerdown',{bubbles:true,button:0,pointerType:'mouse'}));
    trigger.dispatchEvent(new env.window.PointerEvent('pointerup',{bubbles:true,button:0,pointerType:'mouse'}));
    trigger.click();
  });
  await waitFor(()=>[...env.document.querySelectorAll('[role="menuitem"]')].some(item=>item.textContent?.includes('Sign out')),'the account menu opens');
  const signOut=[...env.document.querySelectorAll('[role="menuitem"]')].find(item=>item.textContent?.includes('Sign out'));
  await React.act(async()=>signOut.dispatchEvent(new env.window.MouseEvent('click',{bubbles:true})));
  await waitFor(()=>!!env.document.querySelector('[role="alert"]'),'the failed sign-out is reported');
  assert.ok(env.document.querySelector('.account-menu-panel'),'the menu remains open so the error is visible');
  assert.match(env.document.querySelector('.account-menu-panel')?.textContent??'',/Alice profile/);
});
