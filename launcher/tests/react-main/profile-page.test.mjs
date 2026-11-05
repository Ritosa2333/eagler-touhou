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
  `},bundle:true,platform:'node',format:'esm',jsx:'automatic',packages:'external',loader:{'.webp':'dataurl'},outfile:resolve(work,'owners.mjs'),plugins:[{name:'profile-api-fixture',setup(context){
    context.onResolve({filter:/account-client/},()=>({path:'profile-api-fixture',namespace:'fixture'}));
    context.onLoad({filter:/.*/,namespace:'fixture'},()=>({loader:'js',contents:`
      export class AccountRequestError extends Error {constructor(code,status){super(code);this.code=code;this.status=status;}}
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
