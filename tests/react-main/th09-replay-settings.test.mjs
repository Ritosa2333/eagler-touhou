// Mounted real components and models with synthetic storage/Runtime file ports.
// Real native/browser persistence is covered by the retained TH09 browser run.
import test, {before, after, afterEach} from 'node:test';
import assert from 'node:assert/strict';
import {existsSync} from 'node:fs';
import {mkdtemp, rm} from 'node:fs/promises';
import {createRequire} from 'node:module';
import {dirname, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
const project=fileURLToPath(new URL('../../',import.meta.url)),require=createRequire(resolve(project,'package.json'));
let env,React,createRoot,api,work,root;
before(async()=>{
  env=installMountedDom();React=await import('react');({createRoot}=await import('react-dom/client'));
  env.window.matchMedia=globalThis.matchMedia=query=>({matches:true,media:query,addEventListener(){},removeEventListener(){}});
  work=await mkdtemp(resolve(project,'.cache/th09-replay-settings-'));
  await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`
    export * from './app/components/room/MultiplayerSettingsControls.tsx';
    export * from './app/components/notices/ReplayDialog.tsx';
    export * from './app/models/game-settings.ts'; export * from './app/models/replays.ts';
    export * from './app/i18n.tsx'; export {gameRuntimeOptions} from './app/services/game-preparation.ts';`},bundle:true,platform:'node',format:'esm',jsx:'automatic',outfile:resolve(work,'test.mjs'),logLevel:'silent',plugins:[{name:'authored',setup(ctx){
      ctx.onResolve({filter:/^[^./]/},args=>({path:args.path.startsWith('node:')?args.path:pathToFileURL(require.resolve(args.path)).href,external:true}));
      ctx.onResolve({filter:/^\.\.?\/.*\.mjs$/},args=>{const p=resolve(dirname(args.importer),args.path).replace(/\.mjs$/,'.mts');if(existsSync(p))return {path:p};});
    }}]});api=await import(pathToFileURL(resolve(work,'test.mjs')));
});
afterEach(async()=>{if(root)await act(()=>root.unmount());root=null;env.document.body.replaceChildren();assert.deepEqual(env.errors.splice(0),[]);});
after(async()=>{env.close();await rm(work,{recursive:true,force:true});});
const act=fn=>React.act(async()=>{await fn?.();for(let i=0;i<10;i++)await Promise.resolve();});
async function mount(component){const host=env.document.createElement('div');env.document.body.append(host);root=createRoot(host);await act(()=>root.render(React.createElement(api.LocaleProvider,{locale:'zh-CN'},component)));}
const node=selector=>{const n=env.document.querySelector(selector);assert.ok(n,selector);return n;};
const context=productId=>({productId,uiLocale:'zh-CN',languages:[{id:'ja',title:'日本語'}],musicAvailability:{audio:true,midiAvailable:false,importServer:false,remoteOggAdvertised:false},webMidiAvailable:false,mobile:false});
test('TH09 default-on switch sits above Replay viewer/import actions and persists independently of shared single-player settings',async()=>{
  const saved=new Map(),storage={getItem:key=>saved.get(key)??null,setItem:(key,value)=>saved.set(key,value)},changes=[];
  const model=api.createGameSettingsModel({storage,onChange:change=>changes.push(change)});model.hydrate(context('th09mp'));
  function Controls(){const s=React.useSyncExternalStore(model.subscribe,model.getSnapshot);return React.createElement(api.MultiplayerSettingsControls,{onReplayViewer(){},onImport(){},...(s.context.productId==='th09mp'?{autoReplay:{enabled:s.replayAutoSave,onChange:model.setReplayAutoSave}}:{})});}
  await mount(React.createElement(Controls));const toggle=node('#mpReplayAutoSave');
  assert.equal(toggle.getAttribute('aria-checked'),'true');assert.ok(toggle.compareDocumentPosition(node('#mpReplayViewer'))&4);assert.ok(toggle.compareDocumentPosition(node('#mpGamePackageImport'))&4);
  await act(()=>toggle.click());assert.equal(model.getSnapshot().replayAutoSave,false);
  assert.equal(api.settingsChangeResetsRuntime(changes.at(-1)),false);
  await act(()=>model.setShareSingleplayerSettings(false));assert.equal(model.getSnapshot().replayAutoSave,false);
  const restored=api.createGameSettingsModel({storage});restored.hydrate(context('th09mp'));assert.equal(restored.getSnapshot().replayAutoSave,false);
  await act(()=>model.hydrate(context('th07mp')));assert.equal(env.document.querySelector('#mpReplayAutoSave'),null);
});
test('Replay manager groups saved matchups as folders, keeps legacy files at root and returns to root after deleting the folder contents',async()=>{
  const match=(names,startedAt)=>({schema:'eagler-touhou/th09-match-replay/1',names,startedAt,frames:600,winner:0});
  const files=new Map([['replay/xxx vs abdjfal/th9_ud0000.rpy',{bytes:[1],replayMatch:match(['xxx','abdjfal'],1000)}],['replay/xxx vs abdjfal/th9_ud0001.rpyx',{bytes:[2],replayMatch:match(['xxx','abdjfal'],2000)}],['replay/other vs player/th9_ud0002.rpy',{bytes:[3],replayMatch:match(['other','player'],3000)}],['replay/th9_01.rpy',{bytes:[4]}]]);
  const model=api.createReplayModel({runtime:{getSnapshot:()=>({ready:true,epoch:1,game:'th09',launched:false}),withFileSession:async(game,run)=>run({epoch:1,sync:async()=>{},send:async(command,p)=>{
    if(command==='list')return {files:[...files].map(([path,f])=>({path,size:f.bytes.length,...(f.replayMatch?{replayMatch:f.replayMatch}:{})}))};
    if(command==='remove')files.delete(p.path);if(command==='read')return {bytes:files.get(p.path).bytes};return {};
  }})},prepareFiles:async()=>{},releasePrepared:async()=>{},translate:api.translate.bind(null,'zh-CN'),prompt:()=>null,confirm:async()=>true,download(){},toast(){},afterPaint:async()=>{},afterClose(){}});
  await model.open('th09mp');await mount(React.createElement(api.ReplayDialog,{model,open:true,onCloseRequest(){},onImportRequest(){},onDropFile:async()=>{},onImportError:assert.fail}));
  assert.equal(env.document.querySelectorAll('[data-replay-folder]').length,2);assert.equal(env.document.querySelectorAll('[data-replay-file]').length,1);assert.equal(env.document.querySelector('[data-replay-parent]'),null);
  assert.equal(env.document.querySelectorAll('[data-replay-download-folder]').length,2);
  const folder=[...env.document.querySelectorAll('[data-replay-folder]')].find(n=>n.textContent.includes('xxx vs abdjfal'));assert.ok(folder.querySelector('svg'));
  await act(()=>folder.click());assert.equal(env.document.querySelectorAll('[data-replay-file]').length,2);assert.equal(node('.replay-path strong').textContent,'xxx vs abdjfal');
  assert.equal(node('[data-replay-parent]').textContent,'..');
  await act(()=>node('[data-replay-parent]').click());
  assert.equal(env.document.querySelector('[data-replay-parent]'),null);assert.equal(env.document.querySelectorAll('[data-replay-folder]').length,2);
  await act(()=>[...env.document.querySelectorAll('[data-replay-folder]')].find(n=>n.textContent.includes('xxx vs abdjfal')).click());
  await act(()=>model.remove('replay/xxx vs abdjfal/th9_ud0000.rpy'));await act(()=>model.remove('replay/xxx vs abdjfal/th9_ud0001.rpyx'));
  assert.equal(env.document.querySelector('.replay-path strong'),null);assert.equal(env.document.querySelectorAll('[data-replay-folder]').length,1);assert.equal(node('[data-replay-file] .replay-name').textContent,'th9_01.rpy');model.dispose();
});

test('direct selection is a TH09 Replay launch option for normal and multiplayer, never room transport', () => {
  for(const productId of ['th09','th09mp']){
    const model=api.createGameSettingsModel({storage:{getItem:()=>null,setItem(){}}});model.hydrate(context(productId));
    const input={productId,settings:model.getSnapshot(),replayPath:'replay/收藏/th9_ud0000.rpyx',...(productId==='th09mp'?{multiplayer:{kind:'replay'}}:{})};
    const options=api.gameRuntimeOptions(input,'ja','none',null);assert.equal(options.replayPath,input.replayPath);assert.equal(options.netplayMode,undefined);assert.equal(options.replayViewer,productId==='th09mp'?true:undefined);
    assert.throws(()=>api.gameRuntimeOptions({...input,replayPath:'replay/../score.dat'},'ja','none',null),/Invalid direct Replay/);
  }
});
