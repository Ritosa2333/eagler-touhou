/** Mounted dialog/disclosure behavior; browser screenshots cover rendering and native focus. */
import {test, beforeAll, afterAll, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
const project = fileURLToPath(new URL('../../', import.meta.url));
let env, work, React, createRoot, api, root, host;
const act = fn => React.act(async () => {fn(); await Promise.resolve();});
beforeAll(async () => {
  env = installMountedDom(); React = await import('react'); ({createRoot} = await import('react-dom/client'));
  work = await mkdtemp(resolve(project, '.cache/help-components-'));
  const outfile = resolve(work, 'help.mjs');
  await build({absWorkingDir:project, stdin:{resolveDir:project, loader:'ts', contents:`export * from './app/components/player/TouchHelp'; export * from './app/i18n';`}, outfile, bundle:true, packages:'external', platform:'node', format:'esm', jsx:'automatic', logLevel:'silent'});
  api = await import(pathToFileURL(outfile).href);
});
afterEach(async () => {if(root) await act(() => root.unmount()); root=null; host?.remove(); assert.deepEqual(env.errors.splice(0), []);});
afterAll(async () => {env.close(); await rm(work,{recursive:true,force:true});});
const n = selector => {const node=env.document.querySelector(selector); assert.ok(node,selector); return node;};
async function mount(initial={}, locale='zh-CN') {
  host=env.document.createElement('section'); host.id='player'; env.document.body.append(host); root=createRoot(host);
  let props={open:true,touchEnabled:false,focusMode:'hold-button',showIosFullscreenHelp:false,...initial}, closes=0;
  function render() {root.render(React.createElement(api.LocaleProvider,{locale},React.createElement(api.TouchHelp,{...props,onCloseRequest(){closes++;props={...props,open:false};render();}})));}
  await act(render);
  return {get closes(){return closes;}, async update(next){props={...props,...next};await act(render);}};
}
const click = selector => act(() => n(selector).click());
for(const locale of ['zh-CN','en','ja']) test(`${locale}: shared modal and disclosure expose translated keyboard and practice instructions`,async()=>{
  const f=await mount({},locale), history=env.window.history.length;
  assert.equal(n('#touchHelp').localName,'dialog');assert.equal(n('#touchHelp').open,true);
  assert.equal(n('#touchHelpTitle').textContent,api.translate(locale,'help.title'));
  assert.equal(n('#help-controls .options-group-head').getAttribute('aria-expanded'),'true');
  assert.ok(n('#help-controls').textContent.includes(api.translate(locale,'help.fireConfirm')));
  assert.equal(env.document.querySelector('#help-touch'),null);
  await click('#help-thprac .options-group-head');
  assert.equal(n('#help-controls .game-help-section').hidden,true);
  assert.equal(n('#help-thprac .game-help-section').hidden,false);
  assert.ok(n('#help-thprac').textContent.includes('Backspace'));
  assert.ok(n('#help-thprac').textContent.includes(api.translate(locale,'help.thpracReplayDesktop')));
  assert.equal(n('#touchHelpClose svg').getAttribute('stroke'),'currentColor');
  await click('#touchHelpClose');assert.equal(f.closes,1);assert.equal(n('#touchHelp').open,false);
  assert.equal(host.classList.contains('help-visible'),false);assert.equal(env.window.history.length,history);
  await f.update({open:true});assert.equal(n('#help-controls .game-help-section').hidden,false);
});
for(const focusMode of ['hold-button','toggle-button','two-finger']) test(`${focusMode}: touch instructions match configured focus and do not create demonstration animations`,async()=>{
  await mount({touchEnabled:true,focusMode});
  assert.equal(n('#help-touch .game-help-section').hidden,false);
  const key=focusMode==='two-finger'?'help.focusTwoFingerSummary':focusMode==='toggle-button'?'help.focusToggleSummary':'help.focusHoldSummary';
  assert.ok(n('#help-touch').textContent.includes(api.translate('zh-CN',key)));
  assert.ok(n('#help-touch').textContent.includes(api.translate('zh-CN','help.dialogueControls')));
  await click('#help-thprac .options-group-head');
  assert.ok(n('#help-thprac').textContent.includes(api.translate('zh-CN','touch.cheatsAndSettings')));
  assert.equal(n('#help-thprac').querySelector('kbd'),null);
  assert.equal(env.document.querySelector('.demo-finger,.shot-stream,.guide-replay'),null);
});
for(const ios of [false,true]) test(`${ios?'iOS':'Android'}: orientation uses prose steps and current device instructions`,async()=>{
  await mount({touchEnabled:true,showIosFullscreenHelp:ios});await click('#help-orientation .options-group-head');
  assert.equal(n('#help-orientation ol').children.length,ios?3:2);
  assert.ok(n('#help-orientation').textContent.includes(api.translate('zh-CN',ios?'help.iosWebAppStep':'help.autoRotateStep')));
  assert.equal(env.document.querySelector('.rotation-phone'),null);
});
test('fighting help uses the same modal and keyboard list with its own controls',async()=>{
  await mount({touchEnabled:true,nativeControls:true});
  for(const key of ['help.fightingA','help.fightingB','help.fightingCD','help.fightingNote'])assert.ok(n('#help-controls').textContent.includes(api.translate('zh-CN',key)));
  assert.equal(env.document.querySelector('#help-thprac,#help-touch'),null);
});
for(const method of ['cancel','backdrop']) test(`${method}: dismissal delegates once and reopening resets the disclosure without history`,async()=>{
  const f=await mount();const href=env.window.location.href;await click('#help-thprac .options-group-head');
  if(method==='cancel') await act(()=>n('#touchHelp').dispatchEvent(new env.window.Event('cancel',{cancelable:true})));
  else await click('#touchHelp');
  assert.equal(f.closes,1);assert.equal(n('#touchHelp').open,false);
  await f.update({open:true});assert.equal(n('#help-controls .game-help-section').hidden,false);
  assert.equal(env.window.location.href,href);
});
