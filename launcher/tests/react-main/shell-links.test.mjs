/** Synthetic DOM: real Router links and main-derived masthead/select ownership. */
import {test, vi, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {existsSync} from 'node:fs';
import {createRequire} from 'node:module';
import {dirname, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';
const here=dirname(fileURLToPath(import.meta.url)),project=resolve(here,'../..'),require=createRequire(resolve(project,'package.json'));
let env,React,createRoot,owners,buildDirectory,root;
before(async()=>{
 env=installMountedDom(); React=await import(pathToFileURL(require.resolve('react'))); ({createRoot}=await import(pathToFileURL(require.resolve('react-dom/client'))));
 buildDirectory=await mkdtemp(resolve(here,'shell-links-build-'));
 await require('esbuild').build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`
 export * from './app/components/launcher/OptionsPanel.tsx';
 export * from './app/components/launcher/LauncherMasthead.tsx';
 export * from './app/components/launcher/MainSelect.tsx';
 export * from './app/components/launcher/products.ts';
 export * from './app/components/room/MultiplayerDiagnostics.tsx';
 export * from './app/components/room/MultiplayerOnlineFold.tsx';
 export * from './app/i18n.tsx';
 export {MemoryRouter,useLocation} from 'react-router';
 `},bundle:true,platform:'node',format:'esm',jsx:'automatic',loader:{'.webp':'dataurl'},outfile:resolve(buildDirectory,'bundle.mjs'),logLevel:'silent',plugins:[authoredSourcesPlugin(project),{name:'dependencies',setup(ctx){ctx.onResolve({filter:/^[^./]/},args=>({path:args.path,external:true}));ctx.onResolve({filter:/^\.\.?\/.*\.mjs$/},args=>{const path=resolve(dirname(args.importer),args.path).replace(/\.mjs$/,'.mts');if(existsSync(path))return {path};});}}]});
 owners=await import(pathToFileURL(resolve(buildDirectory,'bundle.mjs')));
});
afterEach(async()=>{if(root)await React.act(async()=>root.unmount());root=null;env.document.body.replaceChildren();assert.deepEqual(env.errors.splice(0),[]);});
after(async()=>{env?.close();if(buildDirectory)await rm(buildDirectory,{recursive:true,force:true});});
const el=(name,props,...children)=>React.createElement(name,props,...children);
const n=selector=>{const node=env.document.querySelector(selector);assert.ok(node,selector);return node;};
function Address(){const value=owners.useLocation();return el('output',{id:'address'},value.pathname+value.search+value.hash);}
async function mount(component){const container=env.document.createElement('div');env.document.body.append(container);root=createRoot(container);await React.act(async()=>root.render(el(owners.MemoryRouter,{initialEntries:['/en.html?game=th07mp']},el(owners.LocaleProvider,{locale:'en'},component,el(Address)))));}
async function click(selector){await React.act(async()=>n(selector).click());}
for(const href of ['lobby.html','lobby.html?game=th06mp&test=1#rooms'])test(`Options lobby link preserves selected product and central Router: ${href}`,async()=>{
 const product=owners.createLibraryProducts(['th07mp'],x=>x,()=>'?game=th07mp')[0];
 await mount(el(owners.OptionsPanel,{product,open:true,onBack(){},lobbyHref:href,assetUrl:x=>x},el('span',null,'body')));
 const anchor=n('#optionsLobbyLink');assert.equal(anchor.tagName,'A');assert.equal(new URL(anchor.href).searchParams.get('game'),'th07mp');
 await click('#optionsLobbyLink');assert.equal(n('#address').textContent,href.includes('test=1')?'/lobby.html?game=th07mp&test=1#rooms':'/lobby.html?game=th07mp');
});
test('Original preselection MP diagnostics subtree exists hidden and retains one owner across SP/MP changes',async()=>{
 const products=owners.createLibraryProducts(['th06','th07mp'],x=>x,id=>'?game='+id);let select,guides=0;
 function Surface(){const[index,setIndex]=React.useState(0);select=setIndex;return el(owners.OptionsPanel,{product:products[index],open:false,onBack(){},assetUrl:x=>x,
  multiplayerDiagnostics:el(owners.MultiplayerDiagnostics,{getRelayUrl:()=>'',onGuide(){guides++;}})},el('section',{id:'active-settings-body'},'Synthetic active settings slot'));}
 await mount(el(Surface));const shell=n('#mpShell'),fold=n('#mpSettingsFold'),diagnostics=n('#mpNetworkDiagnostics'),check=n('#mpNetworkCheck'),results=n('#mpNetworkResults');
 assert.equal(shell.hidden,true);assert.equal(fold.children.length,0);assert.equal(n('#active-settings-body').parentElement.className,'options-scroll');
 // Exact original th07mp-ui48–50 order assertion, before MP selection.
 assert.ok(check.compareDocumentPosition(results)&env.window.Node.DOCUMENT_POSITION_FOLLOWING);
 for(const index of [1,0,1]){await React.act(async()=>select(index));assert.equal(n('#mpShell'),shell);assert.equal(n('#mpSettingsFold'),fold);assert.equal(n('#mpNetworkDiagnostics'),diagnostics);assert.equal(n('#mpNetworkCheck'),check);assert.equal(n('#mpNetworkResults'),results);assert.equal(shell.hidden,index===0);assert.equal(env.document.querySelectorAll('#active-settings-body').length,1);assert.equal(env.document.querySelectorAll('#mpNetworkResults').length,1);}
 await click('#mpGuideOpen');assert.equal(guides,1);assert.equal(results.hidden,true,'Mounting retained probe presentation does not start a probe');
});
test('Original hidden legacy online fold preserves readiness, retained normalized input and required action ports',async()=>{
 let update,creates=0,invalid=0;const joins=[];
 function Surface(){const[configuration,setConfiguration]=React.useState('loading');update=setConfiguration;return el(owners.MultiplayerOnlineFold,{configuration,onCreate(){creates++;},onJoin(code){joins.push(code);},onInvalidCode(){invalid++;}});}
 await mount(el(Surface));const fold=n('#mpOnlineFold'),head=n('[data-mp-fold=online]'),input=n('#mpJoinCode');
 assert.equal(fold.hidden,true,'Mature main intentionally keeps this legacy entry hidden');assert.equal(head.disabled,true);assert.equal(head.title,owners.translate('en','multiplayer.configLoading'));
 await click('#mpCreateRoom');assert.equal(creates,0);await React.act(async()=>update('missing'));assert.equal(head.title,owners.translate('en','multiplayer.serviceMissing'));
 await React.act(async()=>update('ready'));assert.equal(head.disabled,false);assert.equal(head.getAttribute('aria-expanded'),'true');assert.equal(fold.hidden,true);assert.equal(input.disabled,false);
 await click('#mpCreateRoom');assert.equal(creates,1);await click('#mpJoinRoom');assert.equal(invalid,1);
 await React.act(async()=>{input.value='abc12x34567890';input.dispatchEvent(new env.window.Event('input',{bubbles:true}));});assert.equal(input.value,'12345678');
 await click('#mpJoinRoom');assert.deepEqual(joins,['12345678']);await click('[data-mp-fold=online]');assert.equal(head.getAttribute('aria-expanded'),'false');assert.equal(n('[data-mp-fold-body=online]').hasAttribute('inert'),true);
 await React.act(async()=>update('missing'));await React.act(async()=>update('ready'));assert.equal(head.getAttribute('aria-expanded'),'true');assert.equal(input.value,'12345678');assert.equal(fold.hidden,true);
});
function Masthead({variant='lobby'}){const[open,setOpen]=React.useState(false);return el(owners.LauncherMasthead,{variant,assetUrl:x=>x,launcherHref:'/en.html',faqHref:'faq.html',aboutHref:'about.html',menuOpen:open,onMenuOpenChange:setOpen,lessMotion:false,onToggleMotion(){},noticeEnabled:true,onToggleNotice(){},diagnosticsEnabled:false,onToggleDiagnostics(){},onDonation(){},onFirstUse(){},languageControl:el(owners.MainSelect,{id:'uiLanguageSelect',className:'option-select ui-language-select','data-trigger-i18n':'ui.language.menu',value:'en',onChange(){}},el('option',{value:'zh-CN'},'简体中文'),el('option',{value:'en'},'English'),el('option',{value:'ja'},'日本語'))});}
test('Lobby launcher back keeps original anchor and uses Router, standalone docs remain native',async()=>{await mount(el(Masthead));assert.equal(n('#launcherLink').getAttribute('href'),'/en.html');assert.equal(n('a[href="faq.html"]').getAttribute('data-discover'),null);await click('#launcherLink');assert.equal(n('#address').textContent,'/en.html');});
async function openMasthead() {await React.act(async()=>{const trigger=n('#mastheadMenuToggle');trigger.focus();trigger.dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'ArrowDown',bubbles:true,cancelable:true}));});await vi.waitFor(async()=>{await React.act(async()=>{await new Promise(resolve=>env.window.requestAnimationFrame(resolve));});assert.ok(env.document.querySelector('#mastheadMenuPanel'));});}
test('Radix masthead menu opens by keyboard and Escape restores its trigger',async()=>{
 await mount(el(Masthead));await openMasthead();assert.equal(n('#mastheadMenuPanel').getAttribute('role'),'menu');
 await React.act(async()=>n('#mastheadMenuPanel').dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'Escape',bubbles:true,cancelable:true})));
 assert.equal(n('#mastheadMenuToggle').getAttribute('aria-expanded'),'false');await vi.waitFor(async()=>{await React.act(async()=>{await new Promise(resolve=>env.window.requestAnimationFrame(resolve));});assert.equal(env.document.activeElement.id,'mastheadMenuToggle');});
});
test('language choices are visible in the main menu and dispatch the selected locale',async()=>{
 await mount(el(Masthead));await openMasthead();const language=n('.masthead-menu-language');assert.equal(language.getAttribute('aria-label'),owners.translate('en','ui.language'));
 const options=[...env.document.querySelectorAll('[role=menuitemradio]')];assert.equal(options.length,3);assert.deepEqual(options.map(item=>item.textContent),['Simplified Chinese','English','Japanese']);assert.equal(options.find(item=>item.getAttribute('aria-checked')==='true').textContent,'English');
 const selected=[];n('#uiLanguageSelect').addEventListener('change',event=>selected.push(event.target.value));
 await React.act(async()=>options.find(item=>item.getAttribute('data-state')!=='checked').click());assert.deepEqual(selected,['zh-CN']);
 await vi.waitFor(async()=>{await React.act(async()=>{await new Promise(resolve=>env.window.requestAnimationFrame(resolve));});assert.equal(env.document.querySelector('#mastheadMenuPanel'),null);assert.equal(env.document.activeElement.id,'mastheadMenuToggle');});await openMasthead();await React.act(async()=>[...env.document.querySelectorAll('[role=menuitemradio]')].find(item=>item.textContent==='Japanese').click());assert.deepEqual(selected,['zh-CN','ja']);
});
test('GitHub and donation remain the final two menu commands',async()=>{
 await mount(el(Masthead));await openMasthead();const items=[...n('#mastheadMenuPanel').querySelectorAll('.ui-menu-item')];
 assert.equal(items.at(-2).textContent,'GitHub');assert.ok(items.at(-1).classList.contains('book-menu-donation'));
});

test('avatar navigates to the profile address and selects only the profile destination', async () => {
  await mount(el(Masthead)); await click('.book-profile-trigger');
  assert.equal(n('#address').textContent, '/profile.html?lang=en');
  assert.equal(n('.book-profile-trigger').getAttribute('aria-current'), 'page');
  assert.equal(env.document.querySelectorAll('.book-main-nav [aria-current]').length, 0);
});


test('account, settings and help groups expose the handbook in its help group',async()=>{
 await mount(el(Masthead));await openMasthead();
 for(const key of ['nav.account','nav.settings','nav.help'])assert.ok(n('#mastheadMenuPanel').querySelector('[role="group"][aria-label="'+owners.translate('en',key)+'"]'));
 const link=n('#mastheadMenuPanel a[href="ui-review.html?lang=en"]');
 assert.equal(link.closest('[role="group"]').getAttribute('aria-label'),owners.translate('en','nav.help'));
 assert.equal(link.getAttribute('target'),'_blank');assert.equal(link.getAttribute('rel'),'noopener noreferrer');
});
