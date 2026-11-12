/** Mounted profile editor and its single Router blocker. */
import {test, beforeAll, afterAll, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
const originalProfile = Object.freeze({
  name: 'Reimu', bio: 'Original bio', avatar: 'data:image/png;base64,YQ==', cover: 'data:image/png;base64,Yg==',
  links: Object.freeze([{label: 'Home', url: 'https://example.invalid'}]),
});
const originalDisplay = Object.freeze({
  visible: Object.freeze({friends: true, career: true, records: true, content: true}),
  order: Object.freeze(['career', 'records', 'content', 'friends', 'favorites']),
});

let env, work, React, createRoot, createMemoryRouter, RouterProvider, owner, mounted;
let createObjectUrlDescriptor, revokeObjectUrlDescriptor, imageDescriptor, imageLoadDescriptor, canvasGetContextDescriptor, canvasToBlobDescriptor, nextObjectUrl=0;
beforeAll(async () => {
  env = installMountedDom();
  // JSDOM File instances are not Node Blob instances. Model the browser URL API
  // used by the crop dialog without passing a synthetic File to Node's URL class.
  createObjectUrlDescriptor=Object.getOwnPropertyDescriptor(URL,'createObjectURL');
  revokeObjectUrlDescriptor=Object.getOwnPropertyDescriptor(URL,'revokeObjectURL');
  Object.defineProperty(URL,'createObjectURL',{configurable:true,writable:true,value(file) {
    assert.ok(file instanceof env.window.File);
    const url=`blob:profile-image-test/${++nextObjectUrl}`;
    globalThis.__cropImageSizes.set(url,globalThis.__selectedCropImageSize||{naturalWidth:1024,naturalHeight:1024});
    return url;
  }});
  Object.defineProperty(URL,'revokeObjectURL',{configurable:true,writable:true,value() {}});
  imageDescriptor=Object.getOwnPropertyDescriptor(globalThis,'Image');
  class TestImage {
    naturalWidth=0;naturalHeight=0;onload=null;onerror=null;_src='';
    set src(value){this._src=value;if(!value)return;const size=globalThis.__cropImageSizes.get(value);if(!size){this.onerror?.();return;}
      this.naturalWidth=size.naturalWidth;this.naturalHeight=size.naturalHeight;queueMicrotask(()=>this.onload?.());}
    get src(){return this._src;}
  }
  Object.defineProperty(globalThis,'Image',{configurable:true,writable:true,value:TestImage});
  canvasGetContextDescriptor=Object.getOwnPropertyDescriptor(env.window.HTMLCanvasElement.prototype,'getContext');
  canvasToBlobDescriptor=Object.getOwnPropertyDescriptor(env.window.HTMLCanvasElement.prototype,'toBlob');
  Object.defineProperty(env.window.HTMLCanvasElement.prototype,'getContext',{configurable:true,value:()=>({imageSmoothingQuality:'',drawImage(){}})});
  Object.defineProperty(env.window.HTMLCanvasElement.prototype,'toBlob',{configurable:true,value(callback,type){callback(new env.window.Blob([Uint8Array.of(1)],{type:type||'image/png'}));}});
  env.window.scrollTo = () => {};
  React = await import('react');
  ({createRoot} = await import('react-dom/client'));
  ({createMemoryRouter, RouterProvider} = await import('react-router'));
  work = await mkdtemp(resolve(project, '.cache/account-settings-unsaved-'));
  const file = resolve(work, 'owners.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'ts', contents: `
    export {AccountSettings} from './app/components/launcher/AccountSettings.tsx';
    export {SurfaceNavigationProvider} from './app/navigation/surface-navigation.tsx';
    export {LocaleProvider, translate} from './app/i18n.tsx';
    export {createDecisionStore} from './app/models/decisions.ts';
    export {prepareProfileImage, PROFILE_IMAGE_MAX_BYTES, PROFILE_IMAGE_SOURCE_MAX_BYTES} from './app/services/profile-image.ts';
    export {useLocation} from 'react-router';
  `}, bundle: true, platform: 'node', format: 'esm', jsx: 'automatic', packages: 'external', loader: {'.webp': 'dataurl'}, outfile: file,
  plugins: [
    {name: 'account-request-fixture', setup(context) {
      context.onResolve({filter: /account-client/}, () => ({path: 'account-client-fixture', namespace: 'fixture'}));
      context.onLoad({filter: /.*/, namespace: 'fixture'}, () => ({contents: `
        export class AccountRequestError extends Error {
          constructor(code, status) { super(code); this.name = 'AccountRequestError'; this.code = code; this.status = status; }
        }
        export function cancelAccountRequests() {}
        export function mailCaptchaError() {}
        export async function accountRequest(path, body) {
          const request = globalThis.__accountRequest;
          if (!request) throw new Error('account request fixture is not installed');
          return request(path, body);
        }
      `, loader: 'js'}));
    }},
    {name:'cropper-fixture',setup(context){
      context.onResolve({filter:/^react-easy-crop$/},()=>({path:'react-easy-crop-fixture',namespace:'cropper'}));
      context.onResolve({filter:/^react$/},()=>({path:'react',external:true}));
      context.onLoad({filter:/.*/,namespace:'cropper'},()=>({loader:'js',contents:`
        import React,{useEffect} from 'react';
        export default function CropperFixture(props){
          useEffect(()=>{const size=globalThis.__cropImageSizes.get(props.image);if(!size)return;
            props.setImageRef?.({current:{naturalWidth:size.naturalWidth,naturalHeight:size.naturalHeight}});
            props.onMediaLoaded(size);
            const area={x:0,y:0,width:100,height:100};props.onCropAreaChange(area,area);
          },[props.image]);
          return React.createElement('div',{'data-test-cropper':''});
        }
      `}));
    }},
    authoredSourcesPlugin(project),
  ], logLevel: 'silent'});
  owner = await import(pathToFileURL(file).href);
  const compressor = async (file, options) => {
    const calls = globalThis.__profileImageCalls;
    calls?.push({file, options});
    const output = await globalThis.__compressProfileImage(file, options);
    if (calls?.length) calls.at(-1).output = output;
    return output;
  };
  compressor.getDataUrlFromFile = file => globalThis.__profileImageDataUrl(file);
  env.window.imageCompression = compressor;
});

afterEach(async () => {
  delete globalThis.__accountRequest;
  delete globalThis.__profileImageCalls;
  delete globalThis.__compressProfileImage;
  delete globalThis.__profileImageDataUrl;
  if (mounted) {
    if (mounted.root) await React.act(async () => mounted.root.unmount());
    mounted.router.dispose();
    mounted.decisions.dispose();
    mounted = null;
  }
  env.document.body.replaceChildren();
  assert.deepEqual(env.errors.splice(0), []);
});

afterAll(async () => {
  if(createObjectUrlDescriptor)Object.defineProperty(URL,'createObjectURL',createObjectUrlDescriptor);else delete URL.createObjectURL;
  if(revokeObjectUrlDescriptor)Object.defineProperty(URL,'revokeObjectURL',revokeObjectUrlDescriptor);else delete URL.revokeObjectURL;
  if(canvasGetContextDescriptor)Object.defineProperty(env.window.HTMLCanvasElement.prototype,'getContext',canvasGetContextDescriptor);
  if(canvasToBlobDescriptor)Object.defineProperty(env.window.HTMLCanvasElement.prototype,'toBlob',canvasToBlobDescriptor);
  if(imageDescriptor)Object.defineProperty(globalThis,'Image',imageDescriptor);else delete globalThis.Image;
  env?.close();
  delete globalThis.__cropImageSizes;
  if (work) await rm(work, {recursive: true, force: true});
});

async function settle(action = () => {}) {
  await React.act(async () => {action(); await new Promise(resolve => setTimeout(resolve, 5));});
}

async function mount(initialEntries = [
  {pathname: '/', state: {launcherPageHistory: {version: 1, position: 0, page: 'launcher', pagePosition: 0, ancestors: {}}}},
  {pathname: '/profile.html', hash: '#edit', state: {launcherPageHistory: {version: 1, position: 1, page: 'profile', pagePosition: 1, ancestors: {launcher: 0}}}},
]) {
  const decisions = owner.createDecisionStore();
  function Harness() {
    const location = owner.useLocation();
    const [profile, setProfile] = React.useState({...originalProfile, links: originalProfile.links.map(link => ({...link}))});
    const [display, setDisplay] = React.useState({...originalDisplay, visible: {...originalDisplay.visible}, order: [...originalDisplay.order]});
    return React.createElement(owner.LocaleProvider, {locale: 'en'}, React.createElement(owner.SurfaceNavigationProvider, {
      dirty: false, isEditing: false, onDiscard() {}, decisions,
    }, React.createElement(React.Fragment, null, location.pathname === '/profile.html'
      ? React.createElement(owner.AccountSettings, {initialProfile: profile, initialDisplay: display, panel: location.hash || '#edit',
        onSave(next, nextDisplay) {setProfile(next); setDisplay(nextDisplay);}})
      : React.createElement('main', {'data-test-target': ''}, 'Target page'))));
  }
  const router = createMemoryRouter([{path: '*', element: React.createElement(Harness)}], {initialEntries});
  const container = env.document.createElement('div');
  env.document.body.append(container);
  const root = createRoot(container);
  mounted = {root, router, decisions};
  await settle(() => root.render(React.createElement(RouterProvider, {router})));
  return mounted;
}

function input(selector) {
  const node = env.document.querySelector(selector);
  assert.ok(node, `Expected control: ${selector}`);
  return node;
}

async function setValue(node, value) {
  await React.act(async () => {
    const prototype = node.localName === 'textarea' ? env.window.HTMLTextAreaElement.prototype : env.window.HTMLInputElement.prototype;
    Object.getOwnPropertyDescriptor(prototype, 'value').set.call(node, value);
    node.dispatchEvent(new env.window.Event('input', {bubbles: true}));
  });
}

async function click(node) { await settle(() => node.click()); }
function decisionOpen() { return env.document.getElementById('decisionDialog')?.open === true; }
async function navigateAway() {
  await settle(() => mounted.router.navigate('/'));
}
async function expectPrompt() {
  await settle();
  assert.equal(decisionOpen(), true, 'a dirty profile should open the shared navigation confirmation');
}
async function cancelPrompt() {
  await click(env.document.getElementById('decisionCancel'));
  await settle();
}
async function discardPrompt() {
  await click(env.document.getElementById('decisionConfirm'));
  await settle();
}
function beforeUnloadIsPrevented() {
  const event = new env.window.Event('beforeunload', {cancelable: true});
  env.window.dispatchEvent(event);
  return event.defaultPrevented;
}
function buttonWithLabel(label) {
  const found = [...env.document.querySelectorAll('button')].find(node => node.getAttribute('aria-label') === label);
  assert.ok(found, `Expected button with label: ${label}`);
  return found;
}

function imageFile(name, size = 200_000, type = 'image/png') {
  return new env.window.File([new Uint8Array(size)], name, {type});
}

function compressedFile(name, size = 149_999) {
  return new env.window.File([new Uint8Array(size)], name, {type: 'image/webp'});
}

function deferred() {
  let resolve;
  const promise = new Promise(done => {resolve = done;});
  return {promise, resolve};
}

function installCompressor(handler) {
  globalThis.__profileImageCalls = [];
  globalThis.__compressProfileImage = handler;
  globalThis.__profileImageDataUrl = file => `data:image/webp;base64,${file.name}`;
  return globalThis.__profileImageCalls;
}

async function chooseImage(selector, file) {
  globalThis.__cropImageSizes??=new Map();
  globalThis.__selectedCropImageSize=selector==='#settings-cover'?{naturalWidth:1700,naturalHeight:400}:{naturalWidth:1024,naturalHeight:1024};
  const control = input(selector);
  Object.defineProperty(control, 'files', {configurable: true, value: [file]});
  await settle(() => control.dispatchEvent(new env.window.Event('change', {bubbles: true})));
  let confirm;
  for(let attempt=0;attempt<80;attempt++){
    await settle();
    confirm=[...env.document.querySelectorAll('button')].find(node=>node.textContent===owner.translate('en','profile.image.useCrop'));
    if(confirm&&!confirm.disabled)break;
  }
  assert.ok(confirm&&!confirm.disabled,'the crop dialog is ready to confirm');
  if(selector==='#settings-avatar')assert.ok(env.document.querySelector('.profile-image-crop-preview img'),'the debounced room-avatar preview is ready before crop confirmation');
  await click(confirm);
}

async function waitForImageCalls(calls, count) {
  for (let attempt = 0; attempt < 30 && calls.length < count; attempt++) await settle();
  assert.equal(calls.length, count, `the selected image reached the compressor; errors=${env.errors.join('|')}; alerts=${[...env.document.querySelectorAll('[role="alert"]')].map(node=>node.textContent).join('|')}`);
}

test('profile image normalization uses the local worker, per-kind dimensions, and a hard 150000-byte cap', async () => {
  const controller = new AbortController();
  const calls = installCompressor(file => {
    if (file.name === 'cover.png' && !file.type.includes('webp')) return compressedFile('large-cover.webp', 180_000);
    if (file.name === 'large-cover.webp') return compressedFile('cover.webp', 150_000);
    return compressedFile('avatar.webp', 150_000);
  });
  const avatar = await owner.prepareProfileImage(imageFile('avatar.png'), 'avatar', controller.signal);
  const cover = await owner.prepareProfileImage(imageFile('cover.png'), 'cover', controller.signal);
  assert.equal(avatar, 'data:image/webp;base64,avatar.webp');
  assert.equal(cover, 'data:image/webp;base64,cover.webp');
  assert.equal(owner.PROFILE_IMAGE_MAX_BYTES, 150_000);
  assert.equal(calls.length, 3, 'an oversized first cover result is compressed again');
  assert.equal(calls[0].options.maxSizeMB, 150_000 / 1024 / 1024);
  assert.equal(calls[0].options.maxWidthOrHeight, 512);
  assert.equal(calls[0].options.useWebWorker, true);
  assert.equal(calls[0].options.libURL, 'https://launcher.invalid/vendor/browser-image-compression.min.js');
  assert.equal(calls[0].options.fileType, 'image/webp');
  assert.equal(calls[0].options.preserveExif, false);
  assert.equal(calls[1].options.maxWidthOrHeight, 1600);
  assert.equal(calls[2].options.maxWidthOrHeight, 384);
  assert.equal(calls[0].output.size, 150_000, 'the exact byte limit is accepted');
  assert.equal(calls[2].output.size, 150_000, 'the retry must also end at or under the exact limit');
  assert.ok(calls.every(call => call.options.signal === controller.signal));
});

test('profile image normalization rejects unsupported, oversized-source, and still-oversized results', async () => {
  const calls = installCompressor(file => compressedFile(file.name, 170_000));
  await assert.rejects(owner.prepareProfileImage(imageFile('unsupported.gif', 20, 'image/gif'), 'avatar'), error => error.code === 'invalid');
  await assert.rejects(owner.prepareProfileImage(imageFile('source-too-large.png', owner.PROFILE_IMAGE_SOURCE_MAX_BYTES + 1), 'cover'), error => error.code === 'sourceTooLarge');
  await assert.rejects(owner.prepareProfileImage(imageFile('too-large.png'), 'avatar'), error => error.code === 'readFailed');
  assert.equal(calls.length, 2, 'invalid inputs are rejected before compression, then both capped attempts run');
});

test('avatar and cover compression run independently and block save until both files are within the cap', async () => {
  const pending = new Map();
  const calls = installCompressor(file => {
    const task = deferred(); pending.set(calls.length, task); return task.promise;
  });
  const requests = [];
  globalThis.__accountRequest = async (path, body) => {requests.push({path, body}); return {};};
  await mount();
  await chooseImage('#settings-avatar', imageFile('selected-avatar.png'));
  await chooseImage('#settings-cover', imageFile('selected-cover.png'));
  await waitForImageCalls(calls, 2);
  const save = input('form button[type="submit"]');
  assert.equal(input('[role="status"]').textContent, owner.translate('en', 'profile.image.processing'));
  assert.equal(save.disabled, true);
  assert.equal(env.document.querySelectorAll('button[aria-busy="true"]').length, 2);
  assert.equal(beforeUnloadIsPrevented(), true, 'pending image work is part of the unsaved draft');
  assert.ok(calls.every(call => call.options.signal && !call.options.signal.aborted));

  await settle(() => pending.get(2).resolve(compressedFile('saved-cover.webp')));
  assert.equal(save.disabled, true, 'the other image can still be processing');
  assert.equal(env.document.querySelectorAll('button[aria-busy="true"]').length, 1);
  await settle(() => pending.get(1).resolve(compressedFile('saved-avatar.webp')));
  assert.equal(save.disabled, false);
  assert.equal(env.document.querySelectorAll('button[aria-busy="true"]').length, 0);
  assert.equal(input('.settings-preview-cover img').getAttribute('src'), 'data:image/webp;base64,saved-cover.webp');
  await click(save);
  await settle();
  assert.deepEqual(requests.map(request => request.path), ['v1/me/profile']);
  assert.equal(requests[0].body.avatar, 'data:image/webp;base64,saved-avatar.webp');
  assert.match(requests[0].body.avatarThumbnail,/^data:image\/webp;base64,/,'the browser-generated crop preview is uploaded with the profile');
  assert.equal(requests[0].body.cover, 'data:image/webp;base64,saved-cover.webp');
  assert.equal(beforeUnloadIsPrevented(), false);
});

test('replacing a pending image aborts the old work and ignores its late result', async () => {
  const pending = new Map();
  const calls = installCompressor(file => {
    const task = deferred(); pending.set(calls.length, task); return task.promise;
  });
  const requests = [];
  globalThis.__accountRequest = async (path, body) => {requests.push({path, body}); return {};};
  await mount();
  await chooseImage('#settings-avatar', imageFile('first.png'));
  await waitForImageCalls(calls, 1);
  const oldSignal = calls[0].options.signal;
  await chooseImage('#settings-avatar', imageFile('second.png'));
  await waitForImageCalls(calls, 2);
  assert.equal(oldSignal.aborted, true);
  assert.equal(calls[1].options.signal.aborted, false);
  await settle(() => pending.get(2).resolve(compressedFile('current.webp')));
  await settle(() => pending.get(1).resolve(compressedFile('stale.webp')));
  await click(input('form button[type="submit"]'));
  await settle();
  assert.equal(requests[0].body.avatar, 'data:image/webp;base64,current.webp');
});

test('removing or unmounting aborts pending image processing and blocks stale writes', async () => {
  const pending = new Map();
  const calls = installCompressor(file => {
    const task = deferred(); pending.set(calls.length, task); return task.promise;
  });
  const requests = [];
  globalThis.__accountRequest = async (path, body) => {requests.push({path, body}); return {};};
  await mount();
  await chooseImage('#settings-avatar', imageFile('remove-me.png'));
  await waitForImageCalls(calls, 1);
  const removeSignal = calls[0].options.signal;
  await click(buttonWithLabel(owner.translate('en', 'profile.avatar') + ' | ' + owner.translate('en', 'profile.remove')));
  assert.equal(removeSignal.aborted, true);
  await settle(() => pending.get(1).resolve(compressedFile('removed-stale.webp')));
  await click(input('form button[type="submit"]'));
  await settle();
  assert.equal(requests[0].body.avatar, '');

  const unmountCalls = installCompressor(file => {
    const task = deferred(); pending.set(unmountCalls.length, task); return task.promise;
  });
  await chooseImage('#settings-cover', imageFile('unmount-me.png'));
  await waitForImageCalls(unmountCalls, 1);
  const unmountSignal = unmountCalls[0].options.signal;
  await React.act(async () => mounted.root.unmount());
  mounted.root = null;
  assert.equal(unmountSignal.aborted, true);
  pending.get(1).resolve(compressedFile('unmounted-stale.webp'));
  await new Promise(resolve => setTimeout(resolve, 10));
  assert.deepEqual(env.errors.splice(0), []);
});

test('clean profile leaves without a prompt', async () => {
  await mount();
  await navigateAway();
  assert.equal(decisionOpen(), false);
  assert.ok(env.document.querySelector('[data-test-target]'));
});

for (const [name, edit] of [
  ['nickname', async () => setValue(input('#settings-name'), 'Reimu Hakure')],
  ['bio', async () => setValue(input('#settings-bio'), 'A changed introduction')],
  ['avatar image', async () => click(buttonWithLabel(owner.translate('en', 'profile.avatar') + ' | ' + owner.translate('en', 'profile.remove')))],
  ['cover image', async () => click(buttonWithLabel(owner.translate('en', 'profile.cover') + ' | ' + owner.translate('en', 'profile.remove')))],
  ['social link', async () => setValue([...env.document.querySelectorAll('input[aria-label]')].find(node => node.value === 'Home'), 'Homepage')],
  ['privacy', async () => click(input('#settings-friends'))],
]) test(`editing ${name} warns before leaving`, async () => {
  await mount();
  await edit();
  await navigateAway();
  await expectPrompt();
});

test('continue editing closes the prompt and preserves the draft', async () => {
  await mount();
  const name = input('#settings-name');
  await setValue(name, 'Reimu Hakure');
  await navigateAway();
  await expectPrompt();
  await cancelPrompt();
  assert.equal(mounted.router.state.location.pathname, '/profile.html');
  assert.equal(input('#settings-name').value, 'Reimu Hakure');
  assert.equal(decisionOpen(), false);
});

test('nickname drafts are capped at twelve code points', async () => {
  await mount();
  const name = input('#settings-name');
  await setValue(name, 'Reimu Hakurei');
  assert.equal([...name.value].length, 12);
  assert.equal(name.value, 'Reimu Hakure');
});

test('discard confirms navigation to the requested page', async () => {
  await mount();
  await setValue(input('#settings-bio'), 'Discard me');
  await navigateAway();
  await expectPrompt();
  await discardPrompt();
  assert.equal(mounted.router.state.location.pathname, '/');
  assert.ok(env.document.querySelector('[data-test-target]'));
});

test('back navigation warns and can be canceled or confirmed', async () => {
  await mount();
  await setValue(input('#settings-name'), 'Changed');
  await settle(() => mounted.router.navigate(-1));
  await expectPrompt();
  await cancelPrompt();
  assert.equal(mounted.router.state.location.pathname, '/profile.html');
  assert.equal(input('#settings-name').value, 'Changed');
  await settle(() => mounted.router.navigate(-1));
  await expectPrompt();
  await discardPrompt();
  assert.equal(mounted.router.state.location.pathname, '/');
});

test('returning all profile fields to their initial values clears dirty state', async () => {
  await mount();
  const name = input('#settings-name');
  await setValue(name, 'Changed');
  await setValue(input('#settings-bio'), 'Changed bio');
  await setValue(input('#settings-bio'), 'Original bio');
  await setValue(name, 'Reimu');
  assert.equal(beforeUnloadIsPrevented(), false);
  await navigateAway();
  assert.equal(decisionOpen(), false);
});

test('successful save updates the baseline and clears dirty state', async () => {
  const requests = [];
  globalThis.__accountRequest = async (path, body) => {requests.push({path, body}); return {};};
  await mount();
  await setValue(input('#settings-name'), 'Alice');
  await click(input('form button[type="submit"]'));
  await settle();
  assert.deepEqual(requests.map(request => request.path), ['v1/me/profile']);
  assert.equal(requests[0].body.name, 'Alice');
  assert.equal(input('#settings-name').value, 'Alice');
  assert.equal(beforeUnloadIsPrevented(), false);
  await navigateAway();
  assert.equal(decisionOpen(), false);
});

test('failed save keeps the profile dirty', async () => {
  globalThis.__accountRequest = async path => {
    if (path === 'v1/me/profile') throw new Error('save failed');
    return {};
  };
  await mount();
  const bio=input('#settings-bio');
  await setValue(bio, 'Unsaved after failure');
  await click(input('form button[type="submit"]'));
  await settle();
  assert.ok(env.document.querySelector('[role="alert"]'));
  assert.equal(input('#settings-bio'),bio,'the failed save keeps the same editing control mounted');
  assert.equal(bio.value,'Unsaved after failure','the failed save keeps the user draft');
  await navigateAway();
  await expectPrompt();
});

test('edits made during save stay in the form and remain dirty against the saved snapshot', async () => {
  let finishSave;
  const requests = [];
  globalThis.__accountRequest = (path, body) => {
    requests.push({path, body});
    if (path === 'v1/me/profile') return new Promise(resolve => {finishSave = resolve;});
    return Promise.resolve({});
  };
  await mount();
  await setValue(input('#settings-name'), 'Alice');
  await click(input('form button[type="submit"]'));
  for (let attempt = 0; attempt < 20 && !finishSave; attempt++) await settle();
  assert.equal(typeof finishSave, 'function', 'profile save reached its pending request');
  await setValue(input('#settings-bio'), 'Typed while saving');
  await settle(() => finishSave({}));
  assert.equal(input('#settings-name').value, 'Alice');
  assert.equal(input('#settings-bio').value, 'Typed while saving');
  assert.equal(beforeUnloadIsPrevented(), true);
  await navigateAway();
  await expectPrompt();
});

test('beforeunload guard is added while dirty, removed after reverting, and cleaned on unmount', async () => {
  await mount();
  assert.equal(beforeUnloadIsPrevented(), false);
  const name = input('#settings-name');
  await setValue(name, 'Changed');
  assert.equal(beforeUnloadIsPrevented(), true);
  await setValue(input('#settings-name'), 'Reimu');
  assert.equal(beforeUnloadIsPrevented(), false);
  await setValue(input('#settings-name'), 'Changed again');
  assert.equal(beforeUnloadIsPrevented(), true);
  await React.act(async () => mounted.root.unmount());
  mounted.root = null;
  assert.equal(beforeUnloadIsPrevented(), false);
});
