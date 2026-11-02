/** Native form contracts with Radix popup behavior, under a synthetic DOM. */
import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp,rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';
const project=fileURLToPath(new URL('../../',import.meta.url));
let env,React,createRoot,api,work,root;
before(async()=>{
 env=installMountedDom();React=await import('react');({createRoot}=await import('react-dom/client'));
 env.window.HTMLElement.prototype.scrollIntoView=function(){};
 work=await mkdtemp(resolve(project,'.cache/radix-select-'));const outfile=resolve(work,'fixture.mjs');
 await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'tsx',contents:`
 import {useState} from 'react';
 import {MainSelect,MainSelectPrefix,closeMainSelectMenus} from './app/components/launcher/MainSelect';
 import {LocaleProvider} from './app/i18n';
 export {MainSelect,MainSelectPrefix,closeMainSelectMenus,LocaleProvider};
 export {SettingsSelect} from './app/components/settings/SettingsSelect';
 export function Fixture({accept=true,onChange,disabled=false,selectRef}) {
  const [value,setValue]=useState('a');
  return <LocaleProvider locale="en"><form><MainSelect ref={selectRef} name="game" id="game" aria-label="Game" required disabled={disabled} value={value} onChange={event=>{onChange?.(event.currentTarget.value);if(accept)setValue(event.currentTarget.value);}}>
   <option value="a">First game</option><option value="b">Second game</option><option value="c" disabled>Unavailable game</option>
  </MainSelect></form></LocaleProvider>;
 }
 `},outfile,bundle:true,packages:'external',platform:'node',format:'esm',jsx:'automatic',logLevel:'silent',plugins:[authoredSourcesPlugin(project)]});
 api=await import(pathToFileURL(outfile).href);
});
afterEach(async()=>{if(root)await React.act(async()=>root.unmount());root=null;env.document.body.replaceChildren();assert.deepEqual(env.errors.splice(0),[]);});
after(async()=>{env.close();await rm(work,{recursive:true,force:true});});
const act=fn=>React.act(async()=>{fn();await Promise.resolve();});
const node=s=>{const e=env.document.querySelector(s);assert.ok(e,s);return e;};
const key=(e,k)=>e.dispatchEvent(new env.window.KeyboardEvent('keydown',{key:k,bubbles:true,cancelable:true}));
async function mount(props={}){const host=env.document.createElement('div');env.document.body.append(host);root=createRoot(host);await act(()=>root.render(React.createElement(api.Fixture,props)));}
for(const accept of [true,false])test(`controlled native change and form value, consumer accepts=${accept}`,async()=>{
 const events=[];let native;await mount({accept,onChange:v=>events.push(v),selectRef:e=>native=e});
 assert.equal(native,node('select#game'));assert.equal(native.name,'game');assert.equal(native.required,true);
 await act(()=>key(node('.ui-select-trigger'),'ArrowDown'));
 assert.ok(node('[role=listbox]'));assert.equal(node('[data-value=c]').getAttribute('data-disabled'),'');
 await act(()=>key(node('[data-value=b]'),'Enter'));
 assert.deepEqual(events,['b']);assert.equal(native.value,accept?'b':'a');
 assert.equal(new env.window.FormData(node('form')).get('game'),accept?'b':'a');
 assert.equal(env.document.querySelector('[role=listbox]'),null);
});
test('Escape closes the popup and restores its trigger',async()=>{
 await mount();const trigger=node('.ui-select-trigger');await act(()=>key(trigger,'ArrowDown'));
 await act(()=>key(node('[role=listbox]'),'Escape'));assert.equal(env.document.querySelector('[role=listbox]'),null);assert.equal(env.document.activeElement,trigger);
});
test('disabled select stays disabled and cannot open',async()=>{await mount({disabled:true});assert.equal(node('.ui-select-trigger').disabled,true);assert.equal(node('select#game').disabled,true);});

test('settings select uses its visible field label as the accessible name',async()=>{
 const host=env.document.createElement('div');env.document.body.append(host);root=createRoot(host);
 await act(()=>root.render(React.createElement(api.LocaleProvider,{locale:'en'},React.createElement(api.SettingsSelect,{id:'language',label:'Game language',value:'ja',onChange(){}},React.createElement('option',{value:'ja'},'Japanese')))));
 const trigger=node('#language-trigger'),label=node('label[for="language-trigger"]');
 assert.equal(label.textContent,'Game language');assert.equal(trigger.getAttribute('aria-labelledby'),label.id);
 assert.equal(trigger.getAttribute('aria-label'),null,'generic fallback must not replace the field label');
});
test('shared close operation dismisses open menus before another overlay',async()=>{await mount();await act(()=>key(node('.ui-select-trigger'),'ArrowDown'));await act(()=>api.closeMainSelectMenus());assert.equal(env.document.querySelector('[role=listbox]'),null);});

test('a detached menu carries its explicit game surface and clears it for an application surface',async()=>{
 await mount();const form=node('form');form.setAttribute('data-ui-surface','game');
 await act(()=>key(node('.ui-select-trigger'),'ArrowDown'));
 assert.equal(node('[role=listbox]').getAttribute('data-ui-surface'),'game');
 assert.equal(node('[role=listbox]').closest('form'),null,'portal is detached from its trigger surface');
 await act(()=>api.closeMainSelectMenus());form.removeAttribute('data-ui-surface');
 await act(()=>key(node('.ui-select-trigger'),'ArrowDown'));
 assert.equal(node('[role=listbox]').getAttribute('data-ui-surface'),null);
});
