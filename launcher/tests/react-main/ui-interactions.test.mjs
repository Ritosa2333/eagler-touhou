/** Mounted Radix behavior. Pixel layout and real device input are checked separately. */
import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, readFile, rm} from 'node:fs/promises';
import {parse} from '@babel/parser';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom, uiContractAttributes} from './mounted-dom-environment.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));

function assertSharedControls(source,path){
    const tree=parse(source,{sourceType:'module',plugins:['typescript','jsx']});
    const visit=node=>{
      if(node.type==='JSXOpeningElement'&&node.name.type==='JSXIdentifier'){
        assert.ok(!['button','input','select','textarea'].includes(node.name.name),`${path}: use the shared control entry for ${node.name.name}`);
      }
      for(const value of Object.values(node)){
        if(Array.isArray(value)){for(const child of value){if(child?.type)visit(child);}}
        else if(value&&typeof value==='object'&&value.type){visit(value);}
      }
    };
    visit(tree);
}
test('library and directory compose controls through the shared UI entry',async()=>{
  assert.throws(()=>assertSharedControls('const Example=()=> <div><button>Duplicate</button></div>','probe'),/shared control entry/);
  assertSharedControls('const Example=()=> <BookButton/>','probe');
  for(const path of ['launcher/BookLibrary.tsx','launcher/BookDirectoryGames.tsx','directory/LobbyRoomTools.tsx','directory/DirectoryRoomRows.tsx']){
    assertSharedControls(await readFile(resolve(project,'app/components',path),'utf8'),path);
  }
  const bridge=await readFile(resolve(project,'app/components/launcher/BookButton.tsx'),'utf8');
  assert.match(bridge,/export\s*\{Button as BookButton\}\s*from '\.\.\/\.\.\/ui\/controls'/);
});
let env, React, createRoot, root, work, api;
const act = fn => React.act(async () => {fn(); await new Promise(resolve => setTimeout(resolve, 20));});
const node = selector => {const result = env.document.querySelector(selector); assert.ok(result, selector); return result;};
const key = (target, key, code = key) => act(() => target.dispatchEvent(new env.window.KeyboardEvent('keydown', {key, code, bubbles:true, cancelable:true})));
async function mount(Component, props) {
  const host = env.document.createElement('div'); env.document.body.append(host);
  root = createRoot(host); await act(() => root.render(React.createElement(Component, props)));
}

test('shared progress exposes labelled zero, fractional, completed and indeterminate states', async () => {
  await mount(api.Progress, {'aria-label':'Resource download', value:null});
  const progress=node('[role="progressbar"]');
  assert.equal(progress.getAttribute('aria-label'),'Resource download');
  assert.equal(progress.hasAttribute('aria-valuenow'),false);
  assert.equal(progress.dataset.state,'indeterminate');
  for(const [value,max,expected,state] of [[0,129,0,'loading'],[64.5,129,64.5,'loading'],[200,129,129,'complete']]) {
    await act(()=>root.render(React.createElement(api.Progress,{'aria-label':'Resource download',value,max})));
    assert.equal(node('[role="progressbar"]'),progress);
    assert.equal(progress.getAttribute('aria-valuemin'),'0');
    assert.equal(progress.getAttribute('aria-valuemax'),String(max));
    assert.equal(progress.getAttribute('aria-valuenow'),String(expected));
    assert.equal(progress.dataset.state,state);
    if(value===64.5) assert.equal(progress.getAttribute('aria-valuetext'),'50%');
  }
  await act(()=>root.render(React.createElement(api.Progress,{'aria-label':'Resource download',value:Infinity,max:0})));
  assert.equal(progress.getAttribute('aria-valuemax'),'100');
  assert.equal(progress.hasAttribute('aria-valuenow'),false);
  assert.equal(progress.dataset.state,'indeterminate');
});

test('shared text controls preserve labels, refs, native form validation and submitted values', async () => {
  const input=React.createRef(), textarea=React.createRef();
  function Fields() {
    return React.createElement('form',null,
      React.createElement('label',{htmlFor:'website'},'Website'),
      React.createElement(api.Input,{ref:input,id:'website',name:'website',type:'url',required:true,defaultValue:'invalid','data-action':'edit-website'}),
      React.createElement('label',{htmlFor:'memo'},'Memo'),
      React.createElement(api.Textarea,{ref:textarea,id:'memo',name:'memo',rows:3,defaultValue:'first\n第二行'}));
  }
  await mount(Fields);
  assert.equal(input.current,node('#website')); assert.equal(textarea.current,node('#memo'));
  assert.equal(input.current.labels[0].textContent,'Website'); assert.equal(textarea.current.labels[0].textContent,'Memo');
  const expected=env.document.createElement('input');
  for(const [name,value] of Object.entries({id:'website',name:'website',type:'url',required:'',value:'invalid','data-action':'edit-website'})) expected.setAttribute(name,value);
  const attrs=element=>uiContractAttributes(element).sort((a,b)=>a.name.localeCompare(b.name));
  assert.deepEqual(attrs(input.current),attrs(expected));
  assert.equal(input.current.form.checkValidity(),false);
  input.current.value='https://example.org/profile';
  assert.equal(input.current.form.checkValidity(),true);
  const values=new env.window.FormData(input.current.form);
  assert.equal(values.get('website'),'https://example.org/profile'); assert.equal(values.get('memo'),'first\n第二行');
});
before(async () => {
  env = installMountedDom(); React = await import('react'); ({createRoot} = await import('react-dom/client'));
  work = await mkdtemp(resolve(project, '.cache/ui-interactions-'));
  const outfile = resolve(work, 'fixture.mjs');
  await build({absWorkingDir:project, stdin:{resolveDir:project, loader:'tsx', contents:`
    import {useState} from 'react';
    import {LocaleProvider} from './app/i18n';
    import {ActionMenu,ActionMenuItem} from './app/ui/action-menu';
    import {Button} from './app/ui/button';
    export {Button};
    export {Progress} from './app/ui/progress';
    export {Input,Textarea} from './app/ui/input';
    import {ConfirmationDialog} from './app/components/ConfirmationDialog';
    export function TextDecisionFixture({onConfirm,onCancel}) {return <LocaleProvider locale="en"><ConfirmationDialog open title="Rename" message="" confirmText="Rename" cancelText="Cancel" confirmOnEnter
      input={{label:'Filename',defaultValue:'th10_01.rpy',description:'Use a native replay filename',validate:value=>value.startsWith('th10_')?null:'Use the th10_ prefix'}} onConfirm={onConfirm} onCancel={onCancel}/></LocaleProvider>;}
    import {InputSettings} from './app/components/settings/InputSettings';
    import {GamepadSettings} from './app/components/settings/GamepadSettings';
    import {KeyboardSettings} from './app/components/settings/KeyboardSettings';
    import {CareerShowcase} from './app/components/launcher/CareerShowcase';
    import {SiteNavigationRail, SiteBottomNavigation} from './app/components/launcher/SiteNavigation';
    import {Gamepad2,UserRound} from 'lucide-react';
    import {usePopupPadding} from './app/ui/use-popup-padding';
    import {createKeyboardPreferences} from './src/launcher/keyboard-preferences.mts';
    export {createKeyboardPreferences};
    export {translate} from './app/i18n';
    export function KeyboardLocaleFixture({locale,model}) {return <LocaleProvider locale={locale}><KeyboardSettings model={model}/></LocaleProvider>;}
    export function CareerLocaleFixture({locale,onSave}) {return <LocaleProvider locale={locale}><ConfirmationDialog open={false} title="Global" message="" confirmText="OK" cancelText="Cancel" onConfirm={()=>{}} onCancel={()=>{}}/><CareerShowcase records={{}} onSave={onSave} initiallyOpen/></LocaleProvider>;}
    const navGroups=[{id:'play',label:'Play',items:[{id:'single',label:'Single player',href:'./',icon:Gamepad2}]},{id:'site',label:'Site',items:[{id:'account',label:'Account',href:'profile.html',icon:UserRound}]}];
    export function NavigationFixture({onSelect}) {return <LocaleProvider locale="en"><SiteNavigationRail groups={navGroups} activeId="single" onSelect={onSelect} storageKey="navigation-fixture"/><SiteBottomNavigation items={navGroups[0].items} more={navGroups[1].items} activeId="single" onSelect={onSelect}/></LocaleProvider>;}
    export function PopupPaddingFixture({open}) {const value=usePopupPadding(open);return <output data-padding={JSON.stringify(value)}/>;}
    import {createGamepadMappingModel as gamepadModel} from './src/launcher/gamepad-mapping.mts';
    import {createTh09LocalInput} from './src/launcher/th09-local-input.mts';
    import {useStoreSelector,shallowEqual} from './app/shared/react-store';
    export {createGamepadMappingModel} from './src/launcher/gamepad-mapping.mts';
    export function MenuFixture({select}) {
      return <div data-ui-surface="game"><ActionMenu label="File actions" trigger={<Button>Manage replay</Button>}>
        <ActionMenuItem onSelect={()=>select('download')}>Download</ActionMenuItem>
        <ActionMenuItem disabled onSelect={()=>select('blocked')}>Unavailable</ActionMenuItem>
        <ActionMenuItem destructive onSelect={()=>select('delete')}>Delete</ActionMenuItem>
      </ActionMenu></div>;
    }
    export function InputFixture() {
      const [keyboard]=useState(()=>createKeyboardPreferences(null));
      return <LocaleProvider locale="en"><InputSettings keyboard={keyboard}/></LocaleProvider>;
    }
    export function InputDevicesFixture(){
      const [models]=useState(()=>{const storage={getItem:()=>null,setItem(){}};return{keyboard:createKeyboardPreferences(storage),gamepad:gamepadModel({storage,getGamepads:()=>[]}),th09Local:createTh09LocalInput(storage)};});
      return <LocaleProvider locale="en"><InputSettings {...models}/></LocaleProvider>;
    }
    export function DecisionFixture() {
      const [open,setOpen]=useState(false);
      return <LocaleProvider locale="en"><ActionMenu label="Replay actions" trigger={<Button>Manage replay</Button>}><ActionMenuItem onAction={()=>setOpen(true)}>Delete</ActionMenuItem></ActionMenu>
        <ConfirmationDialog open={open} title="Delete replay" message="Specimen" confirmText="Delete" cancelText="Cancel" tone="danger" onConfirm={()=>setOpen(false)} onCancel={()=>setOpen(false)}/></LocaleProvider>;
    }
    export function GamepadFixture({model}) {return <LocaleProvider locale="en"><GamepadSettings model={model} active={false}/></LocaleProvider>;}
    export function SelectedStoreFixture({store,field,onRender}) {
      const selected=useStoreSelector(store,value=>({label:value[field]}),shallowEqual);
      onRender();return <output>{selected.label}</output>;
    }
  `}, outfile, bundle:true, platform:'node', format:'esm', packages:'external', jsx:'automatic', loader:{'.css':'empty'}, logLevel:'silent'});
  api = await import(pathToFileURL(outfile).href);
});
afterEach(async () => {
  if (root) await act(() => root.unmount()); root = null;
  delete env.document.fullscreenElement;
  assert.deepEqual(env.errors.splice(0), []); env.document.body.replaceChildren();
});
after(async () => {env.close(); await rm(work, {recursive:true, force:true});});

test('store selections ignore unrelated updates and follow selected values', async () => {
  let snapshot={value:1,other:0},renders=0;
  const listeners=new Set(),store={subscribe:listener=>{listeners.add(listener);return()=>listeners.delete(listener);},getSnapshot:()=>snapshot};
  await mount(api.SelectedStoreFixture,{store,field:'value',onRender:()=>renders++});
  const initial=renders;
  await act(()=>{snapshot={value:1,other:2};for(const listener of listeners)listener();});
  assert.equal(renders,initial);assert.equal(node('output').textContent,'1');
  await act(()=>{snapshot={value:3,other:2};for(const listener of listeners)listener();});
  assert.equal(renders,initial+1);assert.equal(node('output').textContent,'3');
});

test('control comparison retains native labels and hooks, and the real button retains ref, submit and disabled behavior', async () => {
  const form = env.document.createElement('form'); env.document.body.append(form);
  let submits = 0; form.addEventListener('submit', event => {event.preventDefault(); submits++;});
  const ref = React.createRef(), props = {id: 'contractSubmit', type: 'submit', 'aria-label': 'Save', 'data-action': 'confirm', ref, children: 'Save'};
  await mount(api.Button, props);
  const button = node('#contractSubmit'); form.append(button.parentElement);
  assert.equal(ref.current, button);
  const original = env.document.createElement('button'); original.id = 'contractSubmit'; original.setAttribute('aria-label', 'Save'); original.dataset.action = 'confirm';
  assert.deepEqual(uiContractAttributes(button), uiContractAttributes(original));
  await act(() => button.click()); assert.equal(submits, 1);
  await act(() => root.render(React.createElement(api.Button, {...props, disabled: true})));
  assert.equal(ref.current, button); assert.ok(uiContractAttributes(button).some(attribute => attribute.name === 'disabled'));
  await act(() => button.click()); assert.equal(submits, 1);
  button.type = 'button';
  assert.ok(uiContractAttributes(button).some(attribute => attribute.name === 'type' && attribute.value === 'button'));
});

test('store selections follow a changed selector and release subscriptions', async () => {
  const snapshot={value:1,other:2},listeners=new Set();
  const store={subscribe:listener=>{listeners.add(listener);return()=>listeners.delete(listener);},getSnapshot:()=>snapshot};
  const onRender=()=>{};
  await mount(api.SelectedStoreFixture,{store,field:'value',onRender});
  await act(()=>root.render(React.createElement(api.SelectedStoreFixture,{store,field:'other',onRender})));
  assert.equal(node('output').textContent,'2');
  await act(()=>root.unmount());root=null;
  assert.equal(listeners.size,0);
});

test('file menus share keyboard navigation, disabled gates, Escape and focus return', async () => {
  const calls = []; await mount(api.MenuFixture, {select:value=>calls.push(value)});
  const trigger = node('button'); await act(() => trigger.focus()); await key(trigger, 'ArrowDown');
  const menu = node('[role="menu"]'); assert.equal(menu.dataset.uiSurface, 'game');
  assert.equal(env.document.activeElement.textContent, 'Download');
  await key(env.document.activeElement, 'ArrowDown'); assert.equal(env.document.activeElement.textContent, 'Delete');
  await key(env.document.activeElement, 'Enter'); assert.deepEqual(calls, ['delete']);
  assert.equal(env.document.querySelector('[role="menu"]'), null); assert.equal(env.document.activeElement, trigger);
  await key(trigger, 'ArrowDown'); await key(node('[role="menu"]'), 'Escape');
  assert.equal(env.document.querySelector('[role="menu"]'), null); assert.equal(env.document.activeElement, trigger);
});

test('fullscreen menus use the player host and close on scope changes', async () => {
  const player = env.document.createElement('section'), viewport = env.document.createElement('div');
  player.id = 'player'; player.append(viewport); env.document.body.append(player);
  Object.defineProperty(env.document, 'fullscreenElement', {configurable:true, value:viewport});
  await mount(api.MenuFixture, {select(){}}); await key(node('button'), 'ArrowDown');
  assert.ok(viewport.contains(node('[role="menu"]')));
  await act(() => env.document.dispatchEvent(new env.window.Event('fullscreenchange')));
  assert.equal(env.document.querySelector('[role="menu"]'), null);
});

test('windowed player menus share the player stacking context', async () => {
  await mount(api.MenuFixture,{select(){}});
  const trigger=node('button'),player=env.document.createElement('section');
  player.id='player';env.document.body.append(player);player.append(trigger.closest('[data-ui-surface]').parentElement);
  await key(trigger,'ArrowDown');
  assert.ok(player.contains(node('[role="menu"]')),'The menu must appear above the player, including outside fullscreen');
  await key(node('[role="menu"]'),'Escape');assert.equal(env.document.activeElement,trigger);
});

test('a menu-opened native confirmation returns focus to the surviving trigger after cancel', async () => {
  await mount(api.DecisionFixture);
  const trigger = node('[aria-haspopup="menu"]'); await key(trigger,'ArrowDown');
  await key(node('[role="menuitem"]'),'Enter'); assert.equal(node('#decisionDialog').open,true);
  await act(()=>node('#decisionCancel').click()); assert.equal(node('#decisionDialog').open,false);
  assert.equal(env.document.activeElement,trigger);
});

test('invalid rename stays open with the draft and inline error, then accepts a corrected name', async () => {
  const confirmed=[],cancelled=[];
  await mount(api.TextDecisionFixture,{onConfirm:value=>confirmed.push(value),onCancel:()=>cancelled.push(true)});
  const dialog=node('#decisionDialog'),input=node('#decisionInput');
  input.value='draft.rpy';
  await act(()=>node('#decisionConfirm').click());
  assert.equal(dialog.open,true);assert.equal(input.value,'draft.rpy');
  assert.equal(input.getAttribute('aria-invalid'),'true');
  assert.equal(node('#decisionInputHelp').getAttribute('role'),'alert');
  assert.equal(node('#decisionInputHelp').textContent,'Use the th10_ prefix');
  assert.deepEqual(confirmed,[]);assert.deepEqual(cancelled,[]);
  assert.equal(env.document.activeElement,input);
  input.value='th10_ud0001.rpy';
  await act(()=>node('#decisionConfirm').click());
  assert.equal(dialog.open,false);assert.deepEqual(confirmed,['th10_ud0001.rpy']);
});

test('input capture consumes its first Escape and the next Escape returns focus, including fullscreen relocation', async () => {
  await mount(api.InputFixture);
  const trigger = node('.input-settings-entry'); await act(() => trigger.click());
  const dialog = node('[role="dialog"]'), binding = node('.input-binding-button');
  const player = env.document.createElement('section'), viewport = env.document.createElement('div');
  player.id = 'player'; player.append(viewport); env.document.body.append(player);
  await act(() => binding.click()); assert.equal(binding.getAttribute('aria-pressed'), 'true');
  Object.defineProperty(env.document, 'fullscreenElement', {configurable:true, value:viewport});
  await act(() => env.document.dispatchEvent(new env.window.Event('fullscreenchange')));
  assert.equal(node('[role="dialog"]'), dialog); assert.ok(viewport.contains(dialog));
  assert.equal(binding.isConnected, true);
  assert.equal(node('.input-binding-button'), binding);
  await key(binding, 'Escape'); assert.equal(binding.getAttribute('aria-pressed'), 'false'); assert.equal(node('[role="dialog"]'), dialog);
  await key(binding, 'Escape'); assert.equal(env.document.querySelector('[role="dialog"]'), null);
  assert.equal(env.document.activeElement, trigger);
});

test('switching device and local control context reveals the new panel from its top',async()=>{
  await mount(api.InputDevicesFixture);await act(()=>node('.input-settings-entry').click());
  const scroll=node('.input-dialog-scroll');scroll.scrollTop=240;
  const tab=[...env.document.querySelectorAll('.input-device-tabs [role=tab]')].find(e=>e.textContent==='Gamepad');
  await key(tab,'Enter');assert.equal(tab.getAttribute('aria-selected'),'true');assert.equal(scroll.scrollTop,0);
  scroll.scrollTop=180;await act(()=>node('.input-context-toolbar .ui-select-trigger').click());
  const local=[...env.document.querySelectorAll('[role=option]')].find(e=>e.textContent==='Local two-player');
  await act(()=>local.click());assert.equal(scroll.scrollTop,0);
  assert.equal(node('.input-context-toolbar select').value,'local');
});

test('gamepad threshold uses percent input and persists the device fraction within the allowed range', async () => {
  const saved = new Map(), storage = {getItem:key=>saved.get(key)??null, setItem:(key,value)=>saved.set(key,value)};
  const pad = {id:'Test controller',index:0,connected:true,mapping:'standard',buttons:Array.from({length:17},()=>({pressed:false,value:0})),axes:[0,0]};
  const model = api.createGamepadMappingModel({storage,getGamepads:()=>[pad]}); model.refresh();
  const device = model.getSnapshot().devices[0].key; model.select(0,device);
  await mount(api.GamepadFixture, {model});
  await key(node('[role="slider"]'), 'End'); assert.equal(model.getSnapshot().profiles[device].threshold,.9);
  await key(node('[role="slider"]'), 'Home'); assert.equal(model.getSnapshot().profiles[device].threshold,.1);
  await key(node('[role="slider"]'), 'ArrowRight'); assert.equal(model.getSnapshot().profiles[device].threshold,.11);
  const restored = api.createGamepadMappingModel({storage,getGamepads:()=>[pad]}); restored.refresh();
  assert.equal(restored.getSnapshot().profiles[device].threshold,.11);
});

test('input labels and stored feedback follow three locales while bindings, capture and DOM identity stay stable', async () => {
  const saved=new Map();let writes=0;
  const model=api.createKeyboardPreferences({getItem:key=>saved.get(key)??null,setItem(key,value){saved.set(key,value);writes++;}});
  model.saveLayout('My layout');
  await mount(api.KeyboardLocaleFixture,{locale:'en',model});
  const remove=[...env.document.querySelectorAll('button')].find(button=>button.textContent==='Delete layout');
  await act(()=>remove.click());
  const table=node('.input-binding-table'), binding=node('[data-action="up"] .input-binding-button');
  const snapshot=model.getSnapshot(), layout=model.getLayoutSnapshot(), beforeWrites=writes;
  for(const [locale,heading] of [['zh-CN','动作'],['ja','操作'],['en','Action']]) {
    await act(()=>root.render(React.createElement(api.KeyboardLocaleFixture,{locale,model})));
    assert.equal(node('.input-binding-table'),table);assert.equal(node('[data-action="up"] .input-binding-button'),binding);
    assert.equal(node('thead th').textContent,heading);
    assert.equal(node('.input-config-footer [role="status"]').textContent,api.translate(locale,'input.text.saved_layout_deleted_current_bindings_are_retained'));
    assert.equal(model.getSnapshot(),snapshot);assert.equal(model.getLayoutSnapshot(),layout);assert.equal(writes,beforeWrites);
  }
  await act(()=>binding.click());
  await act(()=>root.render(React.createElement(api.KeyboardLocaleFixture,{locale:'ja',model})));
  assert.equal(binding.getAttribute('aria-pressed'),'true');
  assert.equal(node('.input-binding-listening').textContent,api.translate('ja','input.text.press_a_control'));
  await key(binding,'Escape');assert.equal(binding.getAttribute('aria-pressed'),'false');assert.equal(writes,beforeWrites);
});


test('career locale changes preserve the selected draft and nested discard decisions never save it', async () => {
  const canvas = env.window.HTMLCanvasElement.prototype;
  const context = Object.getOwnPropertyDescriptor(canvas, 'getContext');
  const fonts = Object.getOwnPropertyDescriptor(env.document, 'fonts');
  const labels = [];
  Object.defineProperty(env.document, 'fonts', {configurable:true, value:{ready:Promise.resolve()}});
  Object.defineProperty(canvas, 'getContext', {configurable:true, value:() => new Proxy({fillText:text=>labels.push(text)}, {get:(target,key)=>target[key]??(()=>{})})});
  const saves=[];
  try {
    await mount(api.CareerLocaleFixture,{locale:'en',onSave:value=>saves.push(value)});
    const pressText=async (selector,text)=>act(()=>{const button=[...env.document.querySelectorAll(selector)].find(e=>e.textContent===text);assert.ok(button,text);button.click();});
    await pressText('.career-viewer-actions button','Edit records');
    await act(()=>node('[data-locate-cell="EOSD-Ra-X"]').click());
    await act(()=>node('.career-cell-form .career-check button').click());
    await act(()=>node('.career-cell-form button[type="submit"]').click());
    const viewer=node('.career-viewer'), sheet=node('.career-map-sheet'), field=node('textarea[name="note"]');
    field.value='Unsaved local draft';
    const transform=sheet.style.transform;
    await pressText('.career-viewer-actions button','×');
    const decision=node('#careerDecisionDialog');assert.equal(decision.open,true);
    assert.equal(decision.contains(sheet),false);assert.equal(node('.career-viewer'),viewer);
    assert.equal(sheet.style.transform,transform);
    const ids=[...env.document.querySelectorAll('[id]')].map(e=>e.id);assert.equal(new Set(ids).size,ids.length);
    await act(()=>root.render(React.createElement(api.CareerLocaleFixture,{locale:'ja',onSave:value=>saves.push(value)})));
    assert.equal(node('#careerDecisionDialog'),decision);assert.equal(node('.career-viewer'),viewer);
    assert.equal(node('textarea[name="note"]'),field);assert.equal(field.value,'Unsaved local draft');
    assert.ok(labels.includes('整数作品'));assert.equal(sheet.style.transform,transform);
    assert.equal(node('.career-cell-form .career-check button').getAttribute('aria-checked'),'true');
    await act(()=>node('#careerDecisionCancel').click());assert.equal(env.document.querySelector('#careerDecisionDialog'),null);
    await pressText('.career-viewer-actions button','×');await act(()=>node('#careerDecisionConfirm').click());
    assert.equal(env.document.querySelector('.career-viewer[open]'),null);assert.deepEqual(saves,[]);
    assert.equal(env.document.body.style.overflow,'');
  } finally {
    if(root){await act(()=>root.unmount());root=null;}
    Object.defineProperty(canvas,'getContext',context);
    if(fonts)Object.defineProperty(env.document,'fonts',fonts);else delete env.document.fonts;
  }
});


test('navigation groups remember disclosure and bottom-menu keyboard actions return focus before selection', async () => {
  const calls=[];
  const renderProps={onSelect:item=>calls.push(item.id)};
  await mount(api.NavigationFixture,renderProps);
  const group=node('.site-navigation-group'), link=node('.site-navigation-rail a');
  assert.equal(group.getAttribute('aria-expanded'),'true');
  assert.equal(link.getAttribute('aria-current'),'page');
  await act(()=>link.click());assert.deepEqual(calls,['single']);
  await act(()=>group.click());assert.equal(group.getAttribute('aria-expanded'),'false');
  assert.equal(node('#'+group.getAttribute('aria-controls').replaceAll(':','\\:')).hidden,true);
  await act(()=>root.unmount());root=null;
  await mount(api.NavigationFixture,renderProps);
  assert.equal(node('.site-navigation-group').getAttribute('aria-expanded'),'false');
  const trigger=node('.site-bottom-navigation button');await act(()=>trigger.focus());await key(trigger,'ArrowDown');
  const menu=node('[role="menu"]');assert.equal(menu.textContent,'Account');
  await key(env.document.activeElement,'Escape');assert.equal(env.document.querySelector('[role="menu"]'),null);
  assert.equal(env.document.activeElement,trigger);
  await key(trigger,'ArrowDown');await key(env.document.activeElement,'Enter');
  await React.act(async()=>{await new Promise(resolve=>setTimeout(resolve,30));});
  assert.deepEqual(calls,['single','account']);assert.equal(env.document.activeElement,trigger);
});


test('popup collisions include safe-area edges and update on rotation only while open', async () => {
  const style=env.document.documentElement.style;
  const sides=['top','right','bottom','left'];
  try {
    for(const[side,size]of [['top','20px'],['right','8px'],['bottom','32px'],['left','-1px']])style.setProperty('--ui-safe-'+side,size);
    await mount(api.PopupPaddingFixture,{open:false});
    const read=()=>JSON.parse(node('output').dataset.padding);
    assert.deepEqual(read(),{top:12,right:12,bottom:12,left:12});
    await act(()=>root.render(React.createElement(api.PopupPaddingFixture,{open:true})));
    assert.deepEqual(read(),{top:32,right:20,bottom:44,left:12});
    style.setProperty('--ui-safe-top','0px');style.setProperty('--ui-safe-left','20px');
    await act(()=>env.window.dispatchEvent(new env.window.Event('resize')));
    assert.deepEqual(read(),{top:12,right:20,bottom:44,left:32});
    await act(()=>root.render(React.createElement(api.PopupPaddingFixture,{open:false})));
    style.setProperty('--ui-safe-top','40px');await act(()=>env.window.dispatchEvent(new env.window.Event('resize')));
    assert.equal(read().top,12);
  } finally {for(const side of sides)style.removeProperty('--ui-safe-'+side);}
});
