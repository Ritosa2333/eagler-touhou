/** Mounted production profile. Checks section ownership and stored visibility, not CSS pixels. */
import {test, beforeAll, afterAll, afterEach, beforeEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';

const project=fileURLToPath(new URL('../../',import.meta.url));
let env,work,React,createRoot,owners,root,moveHistory;
const profileDetails={bio:'Profile text',avatar:'',cover:'',links:[],display:{
  visible:{friends:true,career:true,records:true,content:true},order:['career','records','content','friends','favorites'],
}};
const saveStatistics={game:'th06',parserVersion:1,format:'classic',complete:true,playtimeSeconds:3661,applicationTimeSeconds:null,
  playCount:2,clearCount:1,oneCreditClearCount:null,loadouts:[],progress:[{character:0,shot:0,mode:'legacy',difficulty:'normal',cleared:true,
    oneCreditClear:false,clearCount:1,playCount:2,bestScore:1234,playtimeSeconds:3600}],warnings:[]};
const ownerStatistics={career:true,records:true,playtimeSeconds:3661,games:[{product:'th06',uploadedAt:'2025-01-01T00:00:00.000Z',statistics:saveStatistics}]};
const visitorStatistics={career:false,records:true,playtimeSeconds:3661,games:[{product:'th06',uploadedAt:'2025-01-01T00:00:00.000Z',
  statistics:{...saveStatistics,progress:[]}}]};
function remoteProfile(username){return {username,nickname:'Reimu',joinedAt:'2024-01-01T00:00:00.000Z',details:profileDetails};}
beforeAll(async()=>{
  env=installMountedDom();React=await import('react');({createRoot}=await import('react-dom/client'));
  env.window.scrollTo=()=>{};
  // This fixture checks UI ownership. Canvas drawing is covered by renderer/browser checks.
  const context=new Proxy({measureText:text=>({width:String(text).length*7})},{get:(target,key)=>target[key]??(()=>{})});
  env.window.HTMLCanvasElement.prototype.getContext=()=>context;
  Object.defineProperty(env.document,'fonts',{configurable:true,value:{ready:Promise.resolve()}});
  work=await mkdtemp(resolve(project,'.cache/profile-page-'));
  await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`
    export {ProfilePage} from './app/components/launcher/ProfilePreview.tsx';
    export {CareerShowcase} from './app/components/launcher/CareerShowcase.tsx';
    export {notifyAccountChange} from './app/services/account-session.ts';
    export {AccountRequestError} from './app/services/account-client.ts';
    export {CloudSaves} from './app/components/launcher/CloudSaves.tsx';
    export {LocaleProvider,translate} from './app/i18n.tsx';
    export {writeBookPreference} from './app/components/launcher/book-preferences.ts';
    export {MemoryRouter,useNavigate} from 'react-router';
  `},bundle:true,platform:'node',format:'esm',jsx:'automatic',packages:'external',loader:{'.webp':'dataurl'},outfile:resolve(work,'owners.mjs'),plugins:[{name:'profile-api-fixture',setup(context){
    context.onResolve({filter:/account-client/},()=>({path:'profile-api-fixture',namespace:'fixture'}));
    context.onLoad({filter:/.*/,namespace:'fixture'},()=>({loader:'js',contents:`
      export class AccountRequestError extends Error {constructor(code,status){super(code);this.name='AccountRequestError';this.code=code;this.status=status;}}
      export function cancelAccountRequests() {}
      export async function accountRequest(path,body,options){const request=globalThis.__profileAccountRequest;if(!request)throw Error('profile API fixture is missing');return request(path,body,options);}
    `}));
  }},authoredSourcesPlugin(project)],logLevel:'silent'});
  owners=await import(pathToFileURL(resolve(work,'owners.mjs')).href);
});
beforeEach(()=>{globalThis.__profileAccountRequest=async path=>{
  if(path==='auth/get-session')return {user:{username:'player'}};
  const profile=path.match(/^v1\/profiles\/([^/]+)$/);
  if(profile)return remoteProfile(decodeURIComponent(profile[1]));
  const statistics=path.match(/^v1\/profiles\/([^/]+)\/statistics$/);
  if(statistics)return decodeURIComponent(statistics[1])==='visitor'?visitorStatistics:ownerStatistics;
  if(/^v1\/profiles\/[^/]+\/social$/.test(path))return {visible:true,following:null,friends:[],followingUsers:[]};
  throw Error(`Unexpected profile API request: ${path}`);
};});
afterEach(async()=>{delete globalThis.__profileAccountRequest;if(root)await React.act(async()=>root.unmount());root=null;env.document.body.replaceChildren();env.window.localStorage.clear();assert.deepEqual(env.errors.splice(0),[]);});
afterAll(async()=>{env?.close();if(work)await rm(work,{recursive:true,force:true});});
function HistoryProbe(){moveHistory=owners.useNavigate();return null;}
async function mount(Component=owners.ProfilePage,props={},initialEntries=['/profile.html?lang=en']){const container=env.document.createElement('div');env.document.body.append(container);root=createRoot(container);await React.act(async()=>root.render(React.createElement(owners.MemoryRouter,{initialEntries},React.createElement(owners.LocaleProvider,{locale:'en'},React.createElement(Component,props),React.createElement(HistoryProbe)))));}
async function mountCloudSync(initialEntries=['/u/player?lang=en']){const container=env.document.createElement('div');env.document.body.append(container);root=createRoot(container);await React.act(async()=>root.render(React.createElement(owners.MemoryRouter,{initialEntries},React.createElement(owners.LocaleProvider,{locale:'en'},React.createElement(React.Fragment,null,React.createElement(owners.ProfilePage),React.createElement(owners.CloudSaves)),React.createElement(HistoryProbe)))));}
async function waitForProfile(predicate){await React.act(async()=>{for(let attempt=0;attempt<80&&!predicate();attempt++)await new Promise(resolve=>setTimeout(resolve,2));});assert.ok(predicate(),'profile requests did not settle');}

test('public navigation locates sections already present on the page',async()=>{
  await mount();
  await waitForProfile(()=>['career','records','friends'].every(key=>env.document.getElementById('profile-'+key)));
  const sections=['career','records','friends'].map(key=>env.document.getElementById('profile-'+key));
  assert.ok(sections.every(Boolean));
  const links=[...env.document.querySelectorAll('.profile-nav-mobile a')];
  assert.deepEqual(links.map(link=>link.hash),['#profile-overview','#profile-career','#profile-records','#profile-friends']);
  assert.ok(links.every(link=>new URL(link.href).searchParams.get('lang')==='en'));
  await React.act(async()=>links.find(link=>link.hash==='#profile-records').click());
  for(const node of sections)assert.equal(env.document.getElementById(node.id),node,'anchor selection must not swap sections');
  assert.equal(env.document.querySelector('.profile-header-details .profile-bio').textContent,'Profile text');
  assert.ok(env.document.querySelector('.profile-aside .profile-bio'));
});

test('profile deep links and Back/Forward select their actual anchor',async()=>{
  await mount(owners.ProfilePage,{},['/profile.html?lang=en#profile-career']);
  await waitForProfile(()=>!!env.document.querySelector('.profile-nav-mobile a[aria-current="location"]'));
  const current=()=>env.document.querySelector('.profile-nav-mobile [aria-current=location]')?.hash;
  assert.equal(current(),'#profile-career');
  const career=env.document.getElementById('profile-career');
  await React.act(async()=>env.document.querySelector('.profile-nav-mobile a[href$="#profile-records"]').click());
  assert.equal(current(),'#profile-records');
  await React.act(async()=>moveHistory(-1));assert.equal(current(),'#profile-career');
  await React.act(async()=>moveHistory(1));assert.equal(current(),'#profile-records');
  assert.equal(env.document.getElementById('profile-career'),career);
});

test('public profile statistics omit career data hidden by the profile privacy settings',async()=>{
  await mount(owners.ProfilePage,{},['/u/visitor?lang=en']);
  await waitForProfile(()=>!!env.document.getElementById('profile-records'));
  assert.equal(env.document.getElementById('profile-career'),null);
  assert.equal(env.document.getElementById('profile-content'),null);
  assert.ok(env.document.getElementById('profile-records'));
  assert.ok(env.document.getElementById('profile-friends'));
  assert.deepEqual([...env.document.querySelectorAll('.profile-nav-mobile a')].map(link=>link.hash),['#profile-overview','#profile-records','#profile-friends']);
  assert.equal(env.document.querySelector('.profile-social-tools label'),null);
});

test('profile ownership and private statistics refresh when the signed-in account changes',async()=>{
  let viewer='player',profileCalls=0,statisticsCalls=0;
  globalThis.__profileAccountRequest=async path=>{
    if(path==='auth/get-session')return viewer?{user:{username:viewer,name:viewer}}:{user:null};
    const profile=path.match(/^v1\/profiles\/([^/]+)$/);
    if(profile){profileCalls++;return remoteProfile(decodeURIComponent(profile[1]));}
    const statistics=path.match(/^v1\/profiles\/([^/]+)\/statistics$/);
    if(statistics){statisticsCalls++;return viewer==='player'?ownerStatistics:visitorStatistics;}
    if(/^v1\/profiles\/[^/]+\/social$/.test(path))return {visible:true,following:null,friends:[],followingUsers:[]};
    throw Error(`Unexpected profile API request: ${path}`);
  };
  await mount(owners.ProfilePage,{},['/u/player?lang=en']);
  await waitForProfile(()=>!!env.document.getElementById('profile-career'));
  assert.ok(env.document.querySelector('.profile-actions button'), 'the signed-in owner gets profile actions');
  const previousProfiles=profileCalls,previousStatistics=statisticsCalls;
  viewer='visitor';
  await React.act(async()=>owners.notifyAccountChange());
  await waitForProfile(()=>profileCalls>previousProfiles&&statisticsCalls>previousStatistics&&!!env.document.getElementById('profile-records'));
  assert.equal(env.document.getElementById('profile-career'),null,'the new viewer receives only public career data');
  assert.equal(env.document.getElementById('profile-records')?.querySelector('.profile-flight-total')?.textContent,'Total flight time 1h 1m');
  assert.equal(env.document.querySelector('.profile-actions button'),null,'owner editing controls disappear after the session changes');
});

test('background session failures preserve an edited owner form while unauthorized sessions lose owner controls',async()=>{
  let sessionError=null,sessionChecks=0;
  globalThis.__profileAccountRequest=async path=>{
    if(path==='auth/get-session'){
      sessionChecks++;
      if(sessionError)throw sessionError;
      return {user:{username:'player',name:'Player'}};
    }
    const profile=path.match(/^v1\/profiles\/([^/]+)$/);
    if(profile)return remoteProfile(decodeURIComponent(profile[1]));
    const statistics=path.match(/^v1\/profiles\/([^/]+)\/statistics$/);
    if(statistics)return ownerStatistics;
    if(/^v1\/profiles\/[^/]+\/social$/.test(path))return {visible:true,following:null,friends:[],followingUsers:[]};
    throw Error(`Unexpected profile API request: ${path}`);
  };
  Object.defineProperty(env.document,'visibilityState',{configurable:true,value:'visible'});
  const originalNow=Date.now;let fakeNow=originalNow();Date.now=()=>fakeNow;
  try{
    await mount(owners.ProfilePage,{},['/u/player?lang=en#edit']);
    await waitForProfile(()=>!!env.document.querySelector('#settings-bio'));
    fakeNow+=31_000;
    const bio=env.document.querySelector('#settings-bio');
    const setter=Object.getOwnPropertyDescriptor(env.window.HTMLTextAreaElement.prototype,'value').set;
    await React.act(async()=>{setter.call(bio,'Draft preserved after a temporary failure');bio.dispatchEvent(new env.window.Event('input',{bubbles:true}));});
    await React.act(async()=>{sessionError=new Error('temporary network failure');env.window.dispatchEvent(new env.window.Event('focus'));});
    await waitForProfile(()=>sessionChecks>=2);
    assert.equal(env.document.querySelector('#settings-bio')?.value,'Draft preserved after a temporary failure');
    assert.equal(env.document.querySelector('.account-settings')!==null,true,'the owner editor stays mounted during a background outage');

    await React.act(async()=>{sessionError=new owners.AccountRequestError('SESSION_EXPIRED',401);env.window.dispatchEvent(new env.window.Event('focus'));});
    await waitForProfile(()=>sessionChecks>=3&&env.document.querySelector('#settings-bio')===null);
    assert.equal(env.document.querySelector('.profile-actions button'),null,'an unauthorized session no longer receives owner controls');
  }finally{Date.now=originalNow;}
});

test('cloud save uploads and deletions refresh the mounted profile statistics',async()=>{
  let savePresent=false,statisticsCalls=0;
  const file={name:'score.dat',bytes:1,sha256:'a'.repeat(64)};
  const statistics={game:'th06',parserVersion:1,format:'classic',complete:true,playtimeSeconds:3661,applicationTimeSeconds:null,
    playCount:1,clearCount:1,oneCreditClearCount:null,loadouts:[],progress:[],warnings:[]};
  const uploaded={product:'th06',revision:1,uploadedAt:'2026-11-05T00:00:00.000Z',files:[file],statistics};
  const listing=()=>({products:[{id:'th06',game:'th06',multiplayer:false,files:['score.dat'],requiredFiles:['score.dat'],revision:savePresent?1:0}],
    saves:savePresent?[uploaded]:[],limits:{fileBytes:1024,totalBytes:2048,files:6}});
  const profileStatistics=()=>savePresent?{career:true,records:true,playtimeSeconds:3661,games:[{product:'th06',uploadedAt:uploaded.uploadedAt,statistics}]}:
    {career:false,records:true,playtimeSeconds:null,games:[]};
  globalThis.__profileAccountRequest=async(path,body)=>{
    if(path==='auth/get-session')return {user:{username:'player',name:'Player'}};
    if(path==='v1/profiles/player')return remoteProfile('player');
    if(path==='v1/profiles/player/statistics'){statisticsCalls++;return profileStatistics();}
    if(path==='v1/profiles/player/social')return {visible:true,following:null,friends:[],followingUsers:[]};
    if(path==='v1/me/cloud-saves')return listing();
    if(path==='v1/me/cloud-saves/th06'){
      assert.ok(body instanceof env.window.FormData);
      assert.equal(body.get('baseRevision'),'0');
      assert.equal(body.getAll('files').length,1);
      savePresent=true;return uploaded;
    }
    if(path==='v1/me/cloud-saves/th06/delete'){savePresent=false;return {revision:2};}
    throw Error(`Unexpected cloud-save API request: ${path}`);
  };
  await mountCloudSync();
  await waitForProfile(()=>statisticsCalls>=1&&!!env.document.querySelector('.cloud-save-section'));
  assert.equal(env.document.getElementById('profile-records'),null,'the profile starts without uploaded save statistics');

  const input=env.document.querySelector('.cloud-save-upload input[type=file]');
  Object.defineProperty(input,'files',{configurable:true,value:[new env.window.File([Uint8Array.of(1)],'score.dat')]});
  await React.act(async()=>input.dispatchEvent(new env.window.Event('change',{bubbles:true})));
  const upload=[...env.document.querySelectorAll('.cloud-save-upload button')].find(button=>button.textContent.includes(owners.translate('en','cloudSave.upload')));
  assert.ok(upload,'the selected valid save can be uploaded');
  await React.act(async()=>upload.click());
  await waitForProfile(()=>statisticsCalls>=2&&!!env.document.getElementById('profile-records'));
  assert.match(env.document.querySelector('#profile-records')?.textContent??'',/1h 1m/);

  const deleteButton=env.document.querySelector(`.cloud-save-current button[aria-label="${owners.translate('en','cloudSave.delete')}"]`);
  assert.ok(deleteButton,'the uploaded revision can be deleted');
  await React.act(async()=>deleteButton.click());
  assert.equal(env.document.getElementById('decisionDialog')?.open,true);
  await React.act(async()=>env.document.getElementById('decisionConfirm').click());
  await waitForProfile(()=>statisticsCalls>=3&&env.document.getElementById('profile-records')===null);
  assert.equal(env.document.querySelector('.cloud-save-current'),null,'the deleted cloud revision leaves the settings list');
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
