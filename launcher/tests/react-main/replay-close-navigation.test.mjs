/** Mounted ReplayWorkspace and navigation ordering; synthetic DOM only. */
import {afterAll, afterEach, beforeAll, test, vi} from 'vitest';
import assert from 'node:assert/strict';
import {installMountedDom} from './mounted-dom-environment.mjs';

const navigation = vi.hoisted(() => ({routeOpen: true, listeners: new Set(), closeInfoDialog: null}));
vi.mock('../../app/navigation/surface-navigation.tsx', async () => {
  const React = await import('react');
  return {useSurfaceNavigation() {
    React.useSyncExternalStore(callback => {
      navigation.listeners.add(callback);
      return () => navigation.listeners.delete(callback);
    }, () => navigation.routeOpen, () => navigation.routeOpen);
    return {
      infoDialogOpen: id => id === 'replayDialog' && navigation.routeOpen,
      closeInfoDialog: id => navigation.closeInfoDialog(id),
    };
  }};
});

let env, React, createRoot, ReplayWorkspace, LocaleProvider;
beforeAll(async () => {
  env = installMountedDom();
  React = await import('react');
  ({createRoot} = await import('react-dom/client'));
  ({ReplayWorkspace} = await import('../../app/components/player/ReplayWorkspace.tsx'));
  ({LocaleProvider} = await import('../../app/i18n.tsx'));
});
afterEach(() => {
  navigation.routeOpen = true;
  navigation.listeners.clear();
  navigation.closeInfoDialog = null;
  env.document.body.replaceChildren();
  assert.deepEqual(env.errors.splice(0), [], 'No unexpected React or DOM errors');
});
afterAll(() => env?.close());

function fixture({routeOpen = true, productId = 'th06', filesOpen = false, replayViewer = false, requestRuntimeClose = async () => true} = {}) {
  navigation.routeOpen = routeOpen;
  const events = [], subscribers = new Set();
  let snapshot = {open: filesOpen, phase: filesOpen ? 'ready' : 'idle', productId, rows: [], animateRows: false, descriptions: {}, inspecting: null};
  const publish = patch => {
    snapshot = {...snapshot, ...patch};
    for (const subscriber of subscribers) subscriber();
  };
  const model = {
    subscribe: subscriber => {subscribers.add(subscriber); return () => subscribers.delete(subscriber);},
    getSnapshot: () => snapshot,
    close() {events.push('model.close'); if (snapshot.open) publish({open: false});},
    open(product) {
      events.push(`model.open:${product}`);
      publish({open: true, phase: 'loading', productId: product});
      return Promise.resolve();
    },
    setProduct(product) {publish({productId: product});},
  };
  let commitBack = null;
  navigation.closeInfoDialog = () => {
    events.push('history.back.requested');
    commitBack = () => {
      navigation.routeOpen = false;
      for (const listener of navigation.listeners) listener();
      events.push('history.back.committed');
    };
  };
  const appSnapshot = {replayViewer, locale: 'en'};
  const session = {
    baseUrl: 'https://launcher.invalid/', nativeTouch: {isFullscreen: () => false},
    subscribe: () => () => {}, getSnapshot: () => appSnapshot, getReplays: () => model,
    requestRuntimeClose: async () => {events.push('runtime.close.request'); return requestRuntimeClose();},
    reportError: error => {throw error;},
  };
  return {events, model, session, getSnapshot: () => snapshot, commitBack: () => commitBack?.()};
}

async function mount(session) {
  const container = env.document.createElement('div');
  env.document.body.append(container);
  const root = createRoot(container);
  await React.act(async () => root.render(React.createElement(LocaleProvider, {locale: 'en'}, React.createElement(ReplayWorkspace, {session}))));
  return {container, unmount: () => React.act(async () => root.unmount())};
}
async function click(element) {await React.act(async () => element.click());}
async function setRouteOpen(open) {
  await React.act(async () => {
    navigation.routeOpen = open;
    for (const listener of navigation.listeners) listener();
  });
}

test('delayed Back commit never reopens Replay, while Forward opens a fresh manager', async () => {
  const f = fixture(), view = await mount(f.session);
  try {
    assert.deepEqual(f.events, ['model.open:th06'], 'first route entry initializes the selected product once');
    await click(view.container.querySelector('.replay-back'));
    assert.deepEqual(f.events.slice(1), ['history.back.requested'], 'model stays open while Back is pending');
    assert.equal(f.getSnapshot().open, true);
    await React.act(async () => f.commitBack());
    assert.equal(navigation.routeOpen, false);
    assert.equal(f.getSnapshot().open, false, 'committed route departure closes the model');
    assert.deepEqual(f.events.slice(1), ['history.back.requested', 'history.back.committed', 'model.close']);

    await setRouteOpen(true);
    assert.equal(f.getSnapshot().open, true, 'Forward reopens the manager');
    assert.equal(f.events.filter(event => event === 'model.open:th06').length, 2);
  } finally {await view.unmount();}
});

test('route opened before its product arrives initializes when the product becomes available', async () => {
  const f = fixture({productId: null}), view = await mount(f.session);
  try {
    assert.deepEqual(f.events, []);
    await React.act(async () => f.model.setProduct('th06'));
    assert.deepEqual(f.events, ['model.open:th06']);
    assert.equal(f.getSnapshot().open, true);
  } finally {await view.unmount();}
});

test('cancelled Runtime close leaves Replay open and does not request navigation', async () => {
  const f = fixture({filesOpen: true, replayViewer: true, requestRuntimeClose: async () => false}), view = await mount(f.session);
  try {
    await click(view.container.querySelector('.replay-back'));
    assert.equal(navigation.routeOpen, true);
    assert.equal(f.getSnapshot().open, true);
    assert.equal(f.events.includes('history.back.requested'), false);
    assert.ok(view.container.querySelector('.replay-file-browser'), 'manager remains usable after save/close cancellation');
  } finally {await view.unmount();}
});

test('failed Runtime close reports the error and leaves Replay usable', async () => {
  const f = fixture({filesOpen: true, replayViewer: true, requestRuntimeClose: async () => {throw Error('runtime close failed');}}), view = await mount(f.session);
  try {
    await click(view.container.querySelector('.replay-back'));
    await React.act(async () => {await new Promise(resolve => setTimeout(resolve, 0));});
    assert.equal(navigation.routeOpen, true);
    assert.equal(f.getSnapshot().open, true);
    assert.ok(f.events.includes('runtime.close.request'));
    assert.equal(f.events.includes('history.back.requested'), false);
    assert.ok(view.container.querySelector('.replay-file-browser'), 'file controls remain mounted after close rejection');
  } finally {await view.unmount();}
});

test('a rejected or delayed route close leaves Replay usable', async () => {
  const f = fixture({filesOpen: true}), view = await mount(f.session);
  try {
    await click(view.container.querySelector('.replay-back'));
    assert.equal(navigation.routeOpen, true);
    assert.equal(f.getSnapshot().open, true);
    assert.ok(view.container.querySelector('.replay-file-browser'));
  } finally {await view.unmount();}
});
