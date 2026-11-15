/** Mounted lazy Replay host, its history owner, and nested file navigation. */
import {afterAll, afterEach, beforeAll, test, vi} from 'vitest';
import assert from 'node:assert/strict';
import {installMountedDom} from './mounted-dom-environment.mjs';

const navigation = vi.hoisted(() => ({routeOpen: true, currentPage: 'options', listeners: new Set(), closeInfoDialog: null}));
const replayChunk = vi.hoisted(() => ({deferred: false, onStart: null, release: null}));
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
      surface: navigation.currentPage,
    };
  }};
});
vi.mock('../../app/components/player/ReplayWorkspace.tsx', async () => {
  if (replayChunk.deferred) {
    replayChunk.deferred = false;
    replayChunk.onStart?.();
    await new Promise(resolve => {replayChunk.release = resolve;});
  }
  return vi.importActual('../../app/components/player/ReplayWorkspace.tsx');
});

let env, React, createRoot, ReplayWorkspaceHost, SurfaceLayersContext, LocaleProvider;
beforeAll(async () => {
  env = installMountedDom();
  React = await import('react');
  ({createRoot} = await import('react-dom/client'));
  ({ReplayWorkspaceHost} = await import('../../app/BrowserLauncher.tsx'));
  ({SurfaceLayersContext} = await import('../../app/navigation/surface-layer.tsx'));
  ({LocaleProvider} = await import('../../app/i18n.tsx'));
});
afterEach(() => {
  navigation.routeOpen = true;
  navigation.currentPage = 'options';
  navigation.listeners.clear();
  navigation.closeInfoDialog = null;
  env.document.body.replaceChildren();
  assert.deepEqual(env.errors.splice(0), [], 'No unexpected React or DOM errors');
});
afterAll(() => env?.close());

function fixture({routeOpen = true, productId = 'th06', filesOpen = false, replayViewer = false,
  requestRuntimeClose = async () => true, failOpen = false, rows = []} = {}) {
  navigation.routeOpen = routeOpen;
  navigation.currentPage = routeOpen ? 'replay' : 'options';
  const events = [], subscribers = new Set();
  let snapshot = {open: filesOpen, phase: filesOpen ? 'ready' : 'idle', productId, rows, animateRows: false, descriptions: {}, inspecting: null};
  const layers = {entries: [{}], index: 0, writes: [], set: null, back: null};
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
      publish({open: true, phase: 'loading', productId: product, rows: []});
      if (failOpen) {
        publish({phase: 'error'});
        return Promise.reject(Error('replay index failed'));
      }
      publish({phase: 'ready', rows});
      return Promise.resolve();
    },
    setProduct(product) {publish({productId: product});},
  };
  let commitBack = null;
  navigation.closeInfoDialog = () => {
    events.push('history.back.requested');
    commitBack = () => {
      navigation.routeOpen = false;
      navigation.currentPage = 'options';
      for (const listener of navigation.listeners) listener();
      events.push('history.back.committed');
    };
  };
  const appSnapshot = {replayViewer, locale: 'en'};
  const session = {
    baseUrl: 'https://launcher.invalid/', nativeTouch: {isFullscreen: () => false},
    subscribe: () => () => {}, getSnapshot: () => appSnapshot, getReplays: () => model,
    requestRuntimeClose: async () => {events.push('runtime.close.request'); return requestRuntimeClose();},
    reportError: error => {events.push(`session.reportError:${error.message}`);},
  };
  return {events, model, session, layers, getSnapshot: () => snapshot, commitBack: () => commitBack?.()};
}

function LayerHistory({store, children}) {
  const [values, setValues] = React.useState(store.entries[0]);
  store.set = (id, value) => {
    const next = {...store.entries[store.index]};
    if (value === null) delete next[id];
    else next[id] = {value, parentPosition: store.index};
    store.entries = [...store.entries.slice(0, store.index + 1), next];
    store.index++;
    store.writes.push({id, value});
    setValues(next);
  };
  store.back = () => {
    if (store.index === 0) return;
    store.index--;
    setValues(store.entries[store.index]);
  };
  return React.createElement(SurfaceLayersContext.Provider, {value: {values, set: store.set}}, children);
}

async function mount(f) {
  const container = env.document.createElement('div');
  env.document.body.append(container);
  const root = createRoot(container);
  await React.act(async () => root.render(React.createElement(LocaleProvider, {locale: 'en'},
    React.createElement(LayerHistory, {store: f.layers}, React.createElement(ReplayWorkspaceHost, {session: f.session})))));
  return {container, unmount: () => React.act(async () => root.unmount())};
}
async function click(element) {await React.act(async () => element.click());}
async function setRouteOpen(open) {
  await React.act(async () => {
    navigation.routeOpen = open;
    navigation.currentPage = open ? 'replay' : 'options';
    for (const listener of navigation.listeners) listener();
  });
}
async function setCurrentPage(page) {
  await React.act(async () => {
    navigation.currentPage = page;
    for (const listener of navigation.listeners) listener();
  });
}

async function waitFor(predicate) {
  await React.act(async () => {
    for (let attempt = 0; attempt < 100 && !predicate(); attempt++) await new Promise(resolve => setTimeout(resolve, 2));
  });
  assert.ok(predicate(), 'Replay view did not settle');
}

test('Back works before the lazy Replay chunk resolves, and Forward opens the retained host', async () => {
  replayChunk.deferred = true;
  let markStarted;
  const started = new Promise(resolve => {markStarted = resolve;});
  replayChunk.onStart = markStarted;
  const f = fixture(), view = await mount(f);
  try {
    await started;
    assert.equal(view.container.querySelector('.replay-file-browser'), null, 'the delayed view has not mounted');
    assert.deepEqual(f.events, ['model.open:th06'], 'the host starts listing before its view chunk is ready');
    await setRouteOpen(false);
    assert.equal(f.getSnapshot().open, false, 'browser Back closes the model while the chunk is pending');
    await React.act(async () => {replayChunk.release?.(); await new Promise(resolve => setTimeout(resolve, 0));});
    assert.equal(view.container.querySelector('.replay-file-browser'), null, 'resolving the chunk does not reopen the departed route');

    await setRouteOpen(true);
    await waitFor(() => !!view.container.querySelector('.replay-file-browser'));
    assert.equal(f.events.filter(event => event === 'model.open:th06').length, 2);
  } finally {
    replayChunk.onStart = null;
    replayChunk.release = null;
    await view.unmount();
  }
});

test('delayed Back commit closes Replay, while Forward reopens the retained manager', async () => {
  const f = fixture(), view = await mount(f);
  try {
    await waitFor(() => !!view.container.querySelector('.replay-file-browser'));
    assert.deepEqual(f.events, ['model.open:th06'], 'first route entry initializes the selected product once');
    await click(view.container.querySelector('.replay-back'));
    assert.deepEqual(f.events.slice(1), ['history.back.requested'], 'model stays open while Back is pending');
    assert.equal(f.getSnapshot().open, true);
    await React.act(async () => f.commitBack());
    assert.equal(navigation.routeOpen, false);
    assert.equal(f.getSnapshot().open, false, 'committed route departure closes the model');
    assert.deepEqual(f.events.slice(1), ['history.back.requested', 'history.back.committed', 'model.close']);

    await setRouteOpen(true);
    await waitFor(() => !!view.container.querySelector('.replay-file-browser'));
    assert.equal(f.getSnapshot().open, true, 'Forward reopens the manager');
    assert.equal(f.events.filter(event => event === 'model.open:th06').length, 2);
  } finally {await view.unmount();}
});

test('route opened before its product arrives initializes when the product becomes available', async () => {
  const f = fixture({productId: null}), view = await mount(f);
  try {
    assert.deepEqual(f.events, []);
    await React.act(async () => f.model.setProduct('th06'));
    assert.deepEqual(f.events, ['model.open:th06']);
    assert.equal(f.getSnapshot().open, true);
  } finally {await view.unmount();}
});

test('cancelled Runtime close leaves Replay open and does not request navigation', async () => {
  const f = fixture({filesOpen: true, replayViewer: true, requestRuntimeClose: async () => false}), view = await mount(f);
  try {
    await waitFor(() => !!view.container.querySelector('.replay-file-browser'));
    await click(view.container.querySelector('.replay-back'));
    assert.equal(navigation.routeOpen, true);
    assert.equal(f.getSnapshot().open, true);
    assert.equal(f.events.includes('history.back.requested'), false);
    assert.ok(view.container.querySelector('.replay-file-browser'), 'manager remains usable after save/close cancellation');
  } finally {await view.unmount();}
});

test('failed Runtime close reports the error and leaves Replay usable', async () => {
  const f = fixture({filesOpen: true, replayViewer: true, requestRuntimeClose: async () => {throw Error('runtime close failed');}}), view = await mount(f);
  try {
    await waitFor(() => !!view.container.querySelector('.replay-file-browser'));
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
  const f = fixture({filesOpen: true}), view = await mount(f);
  try {
    await waitFor(() => !!view.container.querySelector('.replay-file-browser'));
    await click(view.container.querySelector('.replay-back'));
    assert.equal(navigation.routeOpen, true);
    assert.equal(f.getSnapshot().open, true);
    assert.ok(view.container.querySelector('.replay-file-browser'));
  } finally {await view.unmount();}
});

test('a failed recording index returns to options and does not reopen after the next Back', async () => {
  const f = fixture({failOpen: true}), view = await mount(f);
  try {
    await waitFor(() => !!view.container.querySelector('.replay-workspace-empty'));
    assert.equal(f.getSnapshot().phase, 'error');
    assert.ok(f.events.includes('session.reportError:replay index failed'));
    await click(view.container.querySelector('.replay-back'));
    await React.act(async () => f.commitBack());
    assert.equal(navigation.routeOpen, false, 'Replay Back returns to options');
    assert.equal(navigation.currentPage, 'options');
    assert.equal(f.getSnapshot().open, false);
    await setCurrentPage('library');
    assert.equal(navigation.currentPage, 'library', 'a second Back reaches the main list');
    assert.equal(f.events.filter(event => event.startsWith('model.open:')).length, 1, 'the next Back to the library does not reopen Replay');
  } finally {await view.unmount();}
});

test('browser Back returns from a replay folder and the retained view keeps its layer id', async () => {
  const rows = [{path: 'replay/season-01/score.rpy', name: 'score.rpy', size: 128}];
  const f = fixture({filesOpen: true, productId: 'th09mp', rows}), view = await mount(f);
  try {
    const folder = () => view.container.querySelector('[data-replay-folder="replay/season-01"]');
    const recording = () => view.container.querySelector('[data-replay-file="replay/season-01/score.rpy"]');
    assert.ok(folder());
    await click(folder());
    assert.ok(recording(), 'opening the folder reveals its recording');
    const layerId = f.layers.writes.at(-1)?.id;
    assert.match(layerId, /^replay-folder:/);

    await React.act(async () => f.layers.back());
    assert.equal(recording(), null, 'browser Back restores the parent folder list');
    assert.ok(folder());
    await click(folder());
    assert.equal(f.layers.writes.at(-1)?.id, layerId, 'Forward uses the retained Replay instance layer id');
    assert.ok(recording());

    await setRouteOpen(false);
    assert.equal(f.getSnapshot().open, false);
    await setRouteOpen(true);
    assert.ok(recording(), 'Forward reopens the retained child folder');
    assert.equal(f.layers.writes.at(-1)?.id, layerId);
  } finally {await view.unmount();}
});
