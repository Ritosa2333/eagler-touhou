/** Mounted production profile. Checks section ownership and stored visibility, not CSS pixels. */
import {test, beforeAll, afterAll, afterEach, beforeEach, vi} from 'vitest';
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
function remoteProfile(username){return {username,nickname:'Reimu',joinedAt:'2024-01-01T00:00:00.000Z',countryCode:'CN',regionCode:'TW',details:profileDetails};}
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
      export function mailCaptchaError() {}
      export async function accountRequest(path,body,options){const request=globalThis.__profileAccountRequest;if(!request)throw Error('profile API fixture is missing');return request(path,body,options);}
    `}));
  }},authoredSourcesPlugin(project)],logLevel:'silent'});
  owners=await import(pathToFileURL(resolve(work,'owners.mjs')).href);
});
beforeEach(()=>{globalThis.__profileAccountRequest=async (path,body,options)=>{
  if(path==='auth/get-session')return {user:{username:'player'}};
  if(/^v1\/profiles\/[^/]+\/th09-duels$/.test(path))return {summary:{games:0,wins:0},duels:[]};
  const profile=path.match(/^v1\/profiles\/([^/]+)$/);
  if(profile)return remoteProfile(decodeURIComponent(profile[1]));
  const statistics=path.match(/^v1\/profiles\/([^/]+)\/statistics$/);
  if(statistics)return decodeURIComponent(statistics[1])==='visitor'?visitorStatistics:ownerStatistics;
  if(/^v1\/profiles\/[^/]+\/social$/.test(path))return {
    visible:true,following:null,followerCount:1,followingCount:0,friends:[],followingUsers:[],
    followers:options?.query?.get('summary') ? [] : [{username:'marisa',nickname:'Marisa'}],
  };
  throw Error(`Unexpected profile API request: ${path}`);
};});
afterEach(async()=>{delete globalThis.__profileAccountRequest;if(root)await React.act(async()=>root.unmount());root=null;env.document.body.replaceChildren();env.window.localStorage.clear();assert.deepEqual(env.errors.splice(0),[]);});
afterAll(async()=>{env?.close();if(work)await rm(work,{recursive:true,force:true});});
function HistoryProbe(){moveHistory=owners.useNavigate();return null;}
async function mount(Component=owners.ProfilePage,props={},initialEntries=['/profile.html?lang=en']){const container=env.document.createElement('div');env.document.body.append(container);root=createRoot(container);await React.act(async()=>root.render(React.createElement(owners.MemoryRouter,{initialEntries},React.createElement(owners.LocaleProvider,{locale:'en'},React.createElement(Component,props),React.createElement(HistoryProbe)))));}
async function mountCloudSync(initialEntries=['/u/player?lang=en']){const container=env.document.createElement('div');env.document.body.append(container);root=createRoot(container);await React.act(async()=>root.render(React.createElement(owners.MemoryRouter,{initialEntries},React.createElement(owners.LocaleProvider,{locale:'en'},React.createElement(React.Fragment,null,React.createElement(owners.ProfilePage),React.createElement(owners.CloudSaves)),React.createElement(HistoryProbe)))));}
async function waitForProfile(predicate){await React.act(async()=>{for(let attempt=0;attempt<80&&!predicate();attempt++)await new Promise(resolve=>setTimeout(resolve,2));});assert.ok(predicate(),'profile requests did not settle');}

test('the editor keeps the saved draft while the public profile waits for approval',async()=>{
  const fixture=globalThis.__profileAccountRequest;
  let approved=false, submitted;
  const draft={...profileDetails,name:'Reimu',bio:'Private draft'};
  globalThis.__profileAccountRequest=async (path,body)=>{
    if(path==='v1/me/profile'){
      if(body){submitted=body;return {saved:true,review:{bio:'pending'}};}
      return {draft,review:{bio:'pending'}};
    }
    if(path==='v1/profiles/player') return {...remoteProfile('player'),reviewPending:!approved,
      details:{...profileDetails,bio:approved?draft.bio:''}};
    return fixture(path,body);
  };
  const realTimeout=globalThis.setTimeout;
  const timers=vi.spyOn(globalThis,'setTimeout').mockImplementation((callback,delay,...args)=>
    realTimeout(callback,delay===10000?10:delay,...args));
  try {
    await mount(owners.ProfilePage,{},['/u/player?lang=en#edit']);
    await waitForProfile(()=>!!env.document.querySelector('#settings-bio'));
    assert.equal(env.document.querySelector('#settings-bio').value,draft.bio);
    await React.act(async()=>env.document.querySelector('.settings-save-profile').click());
    assert.equal(submitted.bio,draft.bio);
    await React.act(async()=>moveHistory('/u/player?lang=en'));
    const publicBio=()=>env.document.querySelector('.profile-header-details .profile-bio')?.textContent;
    assert.equal(publicBio(),owners.translate('en','profile.bio.missing'));
    approved=true;
    await waitForProfile(()=>publicBio()===draft.bio);
  } finally {timers.mockRestore();}
});

test('lays out the page before any API and renders sections without waiting for the session or each other',async()=>{
  const pending = () => {let resolve;const promise=new Promise(done=>{resolve=done;});return {promise,resolve};};
  const auth=pending(), profile=pending(), statistics=pending(), duels=pending(), social=pending();
  const calls=[];
  globalThis.__profileAccountRequest=async(path,body,options)=>{
    calls.push({path,options});
    if(path==='auth/get-session')return auth.promise;
    if(path==='v1/profiles/visitor')return profile.promise;
    if(path.endsWith('/statistics'))return statistics.promise;
    if(path.endsWith('/th09-duels'))return duels.promise;
    if(path.endsWith('/social'))return social.promise;
    throw Error(`Unexpected profile API request: ${path}`);
  };
  await mount(owners.ProfilePage,{},['/u/visitor?lang=en']);
  await waitForProfile(()=>calls.some(({path})=>path==='v1/profiles/visitor'));
  assert.ok(env.document.querySelector('.profile-loading-banner'));
  await React.act(async()=>profile.resolve(remoteProfile('visitor')));
  await waitForProfile(()=>calls.filter(({path})=>/\/(statistics|th09-duels|social)$/.test(path)).length===3);
  assert.ok(calls.filter(({path})=>/\/(statistics|th09-duels|social)$/.test(path)).every(({options})=>options.anonymous));
  const header=env.document.querySelector('#profile-name');
  assert.equal(header.textContent,'Reimu');
  assert.equal(env.document.querySelectorAll('.profile-content .profile-loading-panel').length,1);
  await React.act(async()=>duels.resolve({summary:{games:1,wins:1},durationFrames:null,duels:[]}));
  await waitForProfile(()=>!!env.document.getElementById('profile-versus'));
  assert.equal(env.document.querySelector('#profile-records'),null);
  await React.act(async()=>{auth.resolve(null);statistics.resolve(visitorStatistics);social.resolve({visible:true,following:null,followers:[],friends:[],followingUsers:[]});});
  await React.act(async()=>env.document.querySelector('.profile-game-data-switch button').click());
  await waitForProfile(()=>!!env.document.querySelector('.profile-flight-total'));
  assert.equal(env.document.querySelector('#profile-name'),header);
  assert.equal(calls.filter(({path})=>path==='v1/profiles/visitor').length,1);
});

test('saves default view and visibility and previews two duel rows across dates',async()=>{
  const request = globalThis.__profileAccountRequest;
  const BaseResizeObserver = globalThis.ResizeObserver;
  let resizeDuels, savedDisplay;
  globalThis.ResizeObserver = class extends BaseResizeObserver {
    constructor(callback){super();this.callback=callback;}
    observe(element){if(element.classList.contains('profile-th09'))resizeDuels=width=>this.callback([{contentRect:{width}}]);}
  };
  const duels = [0,1,1,1,2,2].map((day,index)=>({
    duelId:`00000000-0000-4000-8000-${String(index).padStart(12,'0')}`,
    playedAt:Date.UTC(2026,10,10-day,12),wins:1,losses:0,durationFrames:null,opponent:null,
  }));
  globalThis.__profileAccountRequest = async (path,body,options) => {
    if(path==='v1/profiles/player/th09-duels')return {summary:{games:6,wins:6},durationFrames:null,duels};
    if(path==='v1/me/profile'&&body){savedDisplay=body.display;return {saved:true};}
    return request(path,body,options);
  };
  const sectionOrder = () => [...env.document.querySelectorAll('.profile-content > section')].map(section=>section.id);
  const duelItems = () => env.document.querySelectorAll('.profile-th09-history li').length;
  try {
    await mount(owners.ProfilePage,{},['/u/player?lang=en']);
    await waitForProfile(()=>sectionOrder().length===1&&!!resizeDuels);
    assert.deepEqual(sectionOrder(),['profile-versus']);
    assert.equal(duelItems(),2);
    await React.act(async()=>resizeDuels(720));
    assert.equal(duelItems(),3,'a short date group still uses a whole row');
    await React.act(async()=>env.document.querySelector('.profile-th09-duels button[aria-expanded]').click());
    assert.equal(duelItems(),6);

    await React.act(async()=>moveHistory('/u/player?lang=en#edit'));
    await waitForProfile(()=>!!env.document.querySelector('#settings-game-data-default-all'));
    await React.act(async()=>env.document.querySelector('#settings-game-data-default-all').click());
    await React.act(async()=>env.document.querySelector('#settings-game-data-th09').click());
    await React.act(async()=>env.document.querySelector('.settings-save-profile').click());
    assert.equal(savedDisplay.gameDataDefault,'all');
    assert.equal(savedDisplay.visible.versus,false);

    await React.act(async()=>moveHistory('/u/player?lang=en'));
    await waitForProfile(()=>!!env.document.querySelector('.profile-clear-chart'));
    assert.deepEqual(sectionOrder(),['profile-versus']);
    assert.equal(env.document.querySelectorAll('.profile-game-data-switch button').length,1);
  } finally {globalThis.ResizeObserver=BaseResizeObserver;}
});

test('public navigation locates sections already present on the page',async()=>{
  await mount();
  await waitForProfile(()=>['versus'].every(key=>env.document.getElementById('profile-'+key)));
  const sections=['versus'].map(key=>env.document.getElementById('profile-'+key));
  assert.ok(sections.every(Boolean));
  const links=[...env.document.querySelectorAll('.profile-nav-mobile a')];
  assert.deepEqual(links.map(link=>link.hash),['#profile-overview','#profile-versus']);
  assert.ok(links.every(link=>new URL(link.href).searchParams.get('lang')==='en'));
  await React.act(async()=>links.find(link=>link.hash==='#profile-versus').click());
  for(const node of sections)assert.equal(env.document.getElementById(node.id),node,'anchor selection must not swap sections');
  assert.equal(env.document.querySelector('.profile-header-details .profile-bio').textContent,'Profile text');
  assert.ok(env.document.querySelector('.profile-aside .profile-bio'));
  assert.equal(env.document.querySelector('.profile-country').textContent,'China');
  assert.equal(env.document.querySelector('.profile-country').getAttribute('lang'),'en');
  await waitForProfile(()=>!!env.document.querySelector('.profile-social-counts a'));
  await React.act(async()=>env.document.querySelector('.profile-social-counts a').click());
  await waitForProfile(()=>!!env.document.querySelector('.profile-social-users a'));
  assert.equal(env.document.querySelector('.profile-social-tabs [aria-current="page"]').textContent,'Followers');
  assert.equal(env.document.querySelector('.profile-social-users a').textContent,'Marisa');
  assert.equal(env.document.querySelector('.profile-page-banner'),null);
  await React.act(async()=>env.document.querySelector('.profile-social-back').click());
  await waitForProfile(()=>!!env.document.querySelector('.profile-page-banner'));

});

test('profile deep links and Back/Forward select their actual anchor',async()=>{
  await mount(owners.ProfilePage,{},['/profile.html?lang=en#profile-career']);
  await waitForProfile(()=>!!env.document.querySelector('.profile-nav-mobile a[aria-current="location"]'));
  const current=()=>env.document.querySelector('.profile-nav-mobile [aria-current=location]')?.hash;
  assert.equal(current(),'#profile-versus');
  await waitForProfile(()=>!!env.document.querySelector('.profile-clear-chart'));
  const card=env.document.getElementById('profile-versus');
  await React.act(async()=>moveHistory('/u/player?lang=en#profile-versus'));
  assert.equal(current(),'#profile-versus');
  await React.act(async()=>moveHistory(-1));
  assert.equal(current(),'#profile-versus');
  await React.act(async()=>moveHistory(1));
  assert.equal(env.document.getElementById('profile-versus'),card);
});

test('public profile statistics omit career data hidden by the profile privacy settings',async()=>{
  await mount(owners.ProfilePage,{},['/u/visitor?lang=en#profile-records']);
  await waitForProfile(()=>!!env.document.querySelector('.profile-flight-total'));
  assert.equal(env.document.querySelector('.profile-clear-chart'),null);
  assert.equal(env.document.getElementById('profile-content'),null);
  assert.ok(env.document.querySelector('.profile-flight-total'));
  assert.equal(env.document.getElementById('profile-friends'),null);
  assert.deepEqual([...env.document.querySelectorAll('.profile-nav-mobile a')].map(link=>link.hash),['#profile-overview','#profile-versus']);
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
  await mount(owners.ProfilePage,{},['/u/player?lang=en#profile-career']);
  await waitForProfile(()=>!!env.document.querySelector('.profile-clear-chart'));
  assert.ok(env.document.querySelector('.profile-actions button'), 'the signed-in owner gets profile actions');
  const previousProfiles=profileCalls,previousStatistics=statisticsCalls;
  viewer='visitor';
  await React.act(async()=>owners.notifyAccountChange());
  await waitForProfile(()=>statisticsCalls>previousStatistics&&!!env.document.querySelector('.profile-flight-total'));
  assert.equal(profileCalls,previousProfiles,'the public profile remains mounted across viewer changes');
  assert.equal(env.document.querySelector('.profile-clear-chart'),null,'the new viewer receives only public career data');
  assert.equal(env.document.querySelector('.profile-flight-total')?.textContent,'Total flight time 1h 1m');
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
    saves:savePresent?[uploaded]:[],pending:[{product:'th06',deviceId:'bbbbbbbb-bbbb-4bbb-8bbb-bbbbbbbbbbbb',runId:'cccccccc-cccc-4ccc-8ccc-cccccccccccc',baseRevision:0,startedAt:'2026-11-05T00:00:00.000Z'}],
    limits:{fileBytes:1024,totalBytes:2048,files:6}});
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
  await mountCloudSync(['/u/player?lang=en#profile-records']);
  await waitForProfile(()=>statisticsCalls>=1&&!!env.document.querySelector('.cloud-save-section'));
  assert.equal(env.document.querySelector('.profile-flight-total'),null,'the profile starts without uploaded save statistics');

  await waitForProfile(()=>!!env.document.querySelector('.cloud-save-add button'));
  assert.equal(env.document.querySelector('.cloud-save-row'),null,'a stale server marker does not imply a cloud save or a conflict');
  await React.act(async()=>env.document.querySelector('.cloud-save-add button').dispatchEvent(
    new env.window.KeyboardEvent('keydown',{key:'ArrowDown',bubbles:true})));
  await waitForProfile(()=>!!env.document.querySelector('[role="option"]'));
  await React.act(async()=>env.document.querySelector('[role="option"]').click());
  await waitForProfile(()=>env.document.querySelector('.cloud-save-row[aria-expanded="true"]'));

  const input=env.document.querySelector('.cloud-save-upload input[type=file]');
  Object.defineProperty(input,'files',{configurable:true,value:[new env.window.File([Uint8Array.of(1)],'score.dat')]});
  await React.act(async()=>input.dispatchEvent(new env.window.Event('change',{bubbles:true})));
  const upload=[...env.document.querySelectorAll('.cloud-save-upload button')].find(button=>button.textContent.includes(owners.translate('en','cloudSave.upload')));
  assert.ok(upload,'the selected valid save can be uploaded');
  await React.act(async()=>upload.click());
  await waitForProfile(()=>statisticsCalls>=2&&!!env.document.querySelector('.profile-flight-total'));
  assert.match(env.document.querySelector('.profile-flight-total')?.textContent??'',/1h 1m/);

  const deleteButton=env.document.querySelector('.cloud-save-delete');
  assert.ok(deleteButton,'the uploaded revision can be deleted');
  await React.act(async()=>deleteButton.click());
  assert.equal(env.document.getElementById('cloudSaveDeleteDialog')?.open,true);
  await React.act(async()=>env.document.getElementById('cloudSaveDeleteConfirm').click());
  await waitForProfile(()=>statisticsCalls>=3&&env.document.querySelector('.profile-flight-total')===null);
  assert.equal(env.document.querySelector('.cloud-save-current'),null,'the deleted cloud revision leaves the settings list');
  assert.equal(env.document.querySelector('.cloud-save-row')?.getAttribute('aria-expanded'),'true','the selected product stays expanded after deletion');
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

test('profile shows all-time TH09 results and keeps paginated duels in the same panel',async({onTestFinished})=>{
  env.document.body.classList.add('less-motion');
  onTestFinished(()=>env.document.body.classList.remove('less-motion'));
  const original=globalThis.__profileAccountRequest;
  const requests=[];
  const endpoint='v1/profiles/player/th09-duels';
  globalThis.__profileAccountRequest=async (path,body,options)=>{
    if (path!==endpoint) return original(path,body,options);
    const cursor=options?.query?.get('cursor');
    requests.push({path,cursor});
    const playedAt=Date.UTC(2026,10,6);
    return !cursor ? {
      summary:{games:9,wins:5},nextCursor:'later',
      ratingHistory:[{at:playedAt-86_400_000,rating:1470},{at:playedAt,rating:1530}],
      rankHistory:[{at:playedAt-86_400_000,rank:14},{at:playedAt,rank:8}],
      characters:[{character:1,games:4,wins:3}],
      characterRatings:[{character:1,rating:1410,interval:120,games:4,days:2,rank:7,
        history:[{at:playedAt-43_200_000,rating:1390},{at:playedAt,rating:1410}]}],
      duels:[{duelId:'a1c12a01-195d-49f3-b229-15aac5260704',wins:5,losses:4,playedAt,opponent:{username:'marisa',displayName:'Marisa',avatar:'0123456789abcdef'}}],
    } : {
      summary:{games:9,wins:5},nextCursor:null,
      duels:[{duelId:'a1c12a01-195d-49f3-b229-15aac5260705',wins:0,losses:1,playedAt,opponent:null}],
    };
  };
  await mount(owners.ProfilePage,{},['/u/player?lang=en']);
  await waitForProfile(()=>!!env.document.querySelector('.profile-th09-history li'));
  const panel=env.document.querySelector('.profile-th09');
  assert.deepEqual([...panel.querySelectorAll('dd')].map(node=>node.textContent),['5/9 (56%)']);
  assert.equal(panel.querySelector('.profile-th09-whr figcaption strong').textContent,'1,530');
  assert.equal(panel.querySelector('.profile-th09-whr polyline').getAttribute('points'),'0,80 1000,20');
  const scrubber=panel.querySelector('.profile-th09-whr [role="slider"]');
  await React.act(async()=>scrubber.dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'Home',bubbles:true})));
  assert.equal(panel.querySelector('.profile-th09-whr figcaption strong').textContent,'1,530');
  assert.equal(panel.querySelector('.profile-th09-whr-tooltip strong').textContent,'1,470');
  await React.act(async()=>scrubber.dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'End',bubbles:true})));
  await React.act(async()=>panel.querySelector('.profile-th09-series [data-slot="choice-item"]:last-child').click());
  assert.equal(panel.querySelector('.profile-th09-current > div:last-child strong').textContent,'#8');
  assert.equal(panel.querySelector('.profile-th09-whr polyline').getAttribute('points'),'0,100 1000,100 1000,0');
  await React.act(async()=>panel.querySelector('.profile-th09-series [data-slot="choice-item"]:first-child').click());
  const chart=panel.querySelector('.profile-th09-whr');
  const whrLine=chart.querySelector('polyline');
  await React.act(async()=>panel.querySelector('button.profile-th09-rating-toggle').click());
  assert.deepEqual([...panel.querySelectorAll('.profile-th09-whr')],[chart]);
  assert.deepEqual([...chart.querySelectorAll('.profile-th09-current strong')].map(node=>node.textContent),['1,530','#8']);
  assert.equal(chart.querySelector('.profile-th09-comparison-character strong').textContent,'1,410');
  assert.equal(chart.querySelector('.profile-th09-comparison-rank').textContent.trim(),'#7');
  assert.deepEqual([...chart.querySelectorAll('polyline')].map(line=>line.getAttribute('points')),['0,40 1000,10','500,80 1000,70']);
  assert.equal(chart.querySelector('polyline[data-muted]'),whrLine);
  await React.act(async()=>chart.querySelector('.profile-th09-series [data-slot="choice-item"]:last-child').click());
  assert.equal(chart.querySelector('polyline[data-muted]').getAttribute('points'),'0,100 1000,100 1000,0');
  assert.equal(chart.querySelector('polyline:not([data-muted])').getAttribute('points'),'500,60 1000,40');
  const rankScrubber=chart.querySelector('[role="slider"]');
  await React.act(async()=>rankScrubber.dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'Home',bubbles:true})));
  assert.equal(chart.querySelector('.profile-th09-whr-tooltip strong').textContent,'#14');
  assert.ok(panel.querySelector('.profile-th09-character[data-selected]'));
  assert.equal(panel.querySelector('.profile-th09-history li strong').textContent,'5:4');
  assert.equal(panel.querySelector('.profile-th09-history li a').getAttribute('href'),'/u/marisa');
  assert.equal(panel.querySelector('.profile-th09-history li .player-card-avatar-button img').getAttribute('src'),'/api/v1/profiles/marisa/avatar/0123456789abcdef');
  await React.act(async()=>panel.querySelector('.profile-th09-actions button').click());
  await waitForProfile(()=>panel.querySelectorAll('.profile-th09-history li').length===2);
  assert.equal(panel.querySelectorAll('.profile-th09-day').length,1,'pagination keeps the same date together');
  assert.equal(panel.querySelector('li:last-child .profile-th09-opponent').textContent,'Guest');
  assert.equal(chart.querySelector('figcaption strong').textContent,'1,410');
  await React.act(async()=>chart.querySelector('.profile-th09-comparison-back').click());
  assert.equal(chart.querySelector('figcaption strong').textContent,'1,530');
  assert.equal(chart.querySelectorAll('polyline').length,1);
  assert.equal(chart.querySelector('polyline').getAttribute('points'),'0,100 1000,100 1000,0');
  assert.equal(panel.querySelector('.profile-th09-character[data-selected]'),null);
  assert.deepEqual(requests,[{path:endpoint,cursor:undefined},{path:endpoint,cursor:'later'}]);
});
