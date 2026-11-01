/** Production React chat + real room owner, synthetic DOM/socket/clock only.
 * Original browser geometry/held-iframe-key assertions remain unrun. */
import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import { mkdir, mkdtemp, rm } from 'node:fs/promises';
import { resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { build } from 'esbuild';
import { execFileSync } from 'node:child_process';
import { authoredSourcesPlugin } from './authored-sources.mjs';
import { installMountedDom } from './mounted-dom-environment.mjs';
const project = fileURLToPath(new URL('../../', import.meta.url));
let env, work, owner, React, createRoot, mounted;
before(async () => {
  env = installMountedDom();
  React = await import('react');
  ({ createRoot } = await import('react-dom/client'));
  await mkdir(resolve(project, '.cache'), {
    recursive: true,
  });
  work = await mkdtemp(resolve(project, '.cache/quick-chat-mounted-'));
  const output = resolve(work, 'entry.mjs');
  await build({
    absWorkingDir: project,
    stdin: {
      resolveDir: project,
      loader: 'ts',
      contents: `
    export {MultiplayerQuickChat} from './app/components/player/MultiplayerQuickChat.tsx';
    export {RoomQuickChat} from './app/components/room/RoomQuickChat.tsx';
    export {PlayerSurface} from './app/components/player/PlayerSurface.tsx';
    export {createMultiplayerQuickChatModel} from './src/launcher/multiplayer-quick-chat-model.mts';
    export {QUICK_CHAT_ROWS} from './src/contracts/multiplayer-quick-chat.mts';
    export {createRoomSession} from './app/session/room-session.ts';
    export {createMultiplayerIdentityStore} from './src/launcher/multiplayer-identity.mts';
    export {createMultiplayerRoomSessionStore} from './src/launcher/multiplayer-room-session.mts';
    export {createDecisionStore} from './app/models/decisions.ts';
    export {LocaleProvider, translate} from './app/i18n.tsx';
  `,
    },
    outfile: output,
    bundle: true,
    platform: 'node',
    format: 'esm',
    jsx: 'automatic',
    packages: 'external',
    plugins: [authoredSourcesPlugin(project)],
    logLevel: 'silent',
  });
  owner = await import(pathToFileURL(output).href);
});
afterEach(async () => {
  if (mounted) {
    await React.act(async () => {
      mounted.root.unmount();
    });
    mounted.model.dispose();
    mounted.dispose?.();
    mounted = null;
  }
  assert.deepEqual(env.errors, []);
  env.document.body.replaceChildren();
});
after(async () => {
  env.close();
  await rm(work, {
    recursive: true,
    force: true,
  });
});
class Clock {
  now = 0;
  next = 0;
  tasks = new Map();
  setTimeout = (fn, delay) => {
    const id = ++this.next;
    this.tasks.set(id, {
      fn,
      at: this.now + delay,
    });
    return id;
  };
  clearTimeout = (id) => {
    this.tasks.delete(id);
  };
  tick(ms) {
    const end = this.now + ms;
    for (;;) {
      const item = [...this.tasks]
        .filter(([, v]) => v.at <= end)
        .sort((a, b) => a[1].at - b[1].at)[0];
      if (!item) break;
      this.tasks.delete(item[0]);
      this.now = item[1].at;
      item[1].fn();
    }
    this.now = end;
  }
}
const context = () => ({
  visible: true,
  room: 'th06mp-1234',
  serial: 7,
  localSeat: 0,
  connected: true,
  language: 'zh-CN',
  seats: [
    {
      clientId: 'a',
      name: 'Local',
    },
    {
      clientId: 'b',
      name: '<img src=x onerror=alert(1)>',
    },
  ],
});
const message = (extra = {}) => ({
  type: 'quick-chat',
  room: 'th06mp-1234',
  serial: 7,
  seat: 1,
  clientId: 'b',
  phrase: '1',
  ...extra,
});
const act = (callback) =>
  React.act(async () => {
    callback();
    await Promise.resolve();
  });
const query = (selector) => {
  const value = env.document.querySelector(selector);
  assert.ok(value, selector);
  return value;
};
async function mount(model, { strict = false, dispose, room = false } = {}) {
  const host = env.document.createElement('div');
  env.document.body.append(host);
  const root = createRoot(host);
  const element = React.createElement(
    owner.LocaleProvider,
    {
      locale: 'en',
    },
    React.createElement(
      owner.PlayerSurface,
      {
        open: true,
        editing: false,
        onElement: () => {},
      },
      React.createElement(React.Fragment, null,
        React.createElement(owner.MultiplayerQuickChat, {model}),
        room === 'split' ? React.createElement(React.Fragment, null,
          React.createElement('div', {id:'roomUpperSpace'}, React.createElement(owner.RoomQuickChat, {model,part:'log'})),
          React.createElement('div', {id:'roomBottomControls'}, React.createElement(owner.RoomQuickChat, {model,part:'actions'})))
          : room ? React.createElement(owner.RoomQuickChat, {model}) : null),
    ),
  );
  mounted = {
    root,
    model,
    dispose,
  };
  await act(() =>
    root.render(strict ? React.createElement(React.StrictMode, null, element) : element),
  );
}
function fixture() {
  const clock = new Clock(),
    calls = [];
  const model = owner.createMultiplayerQuickChatModel({
    timers: clock,
    voice: (id) => calls.push(['voice', id]),
    send: (value) => calls.push(['send', value]),
  });
  model.update(context());
  return {
    clock,
    calls,
    model,
  };
}
test('pregame room chat is independent and shares safe text, send and ten-second expiry', async () => {
  const {model,clock,calls}=fixture();
  model.update({...context(),inRoom:true});
  await mount(model,{room:true,strict:true});
  assert.equal(query('.mp-quick-chat').hidden,true);
  assert.equal(query('#mpRoomChat').contains(query('.mp-quick-chat')),false);
  await act(()=>query('#mpRoomChat [data-phrase="1"]').click());
  assert.equal(calls.at(-1)[0],'send');
  await act(()=>model.receive(message()));
  assert.equal(query('#mpRoomChat .mp-room-chat-log strong').textContent,'P2(<img src=x onerror=alert(1)>)');
  assert.equal(query('#mpRoomChat').querySelector('img'),null);
  await act(()=>clock.tick(9999));
  assert.equal(query('#mpRoomChat .mp-room-chat-log').children.length,1);
  await act(()=>clock.tick(1));
  assert.equal(query('#mpRoomChat .mp-room-chat-log').children.length,0);
});
test('room messages stay above the controls and sending closes the upward picker', async () => {
  const {model,clock,calls}=fixture();model.update({...context(),inRoom:true});
  await mount(model,{room:'split',strict:true});
  assert.equal(query('#roomBottomControls').querySelector('.mp-room-chat-log'),null);
  const actions=query('#mpRoomChat .room-quick-chat-trigger');await act(()=>actions.click());
  assert.equal(query('#roomBottomControls').querySelector('.room-quick-chat-picker'),null);
  await act(()=>query('.room-quick-chat-picker [data-phrase="1"]').click());
  assert.equal(calls.at(-1)[0],'send');assert.equal(actions.getAttribute('aria-expanded'),'false');
  await act(()=>model.receive(message()));
  assert.equal(query('#roomUpperSpace .mp-room-chat-log').children.length,1);
  await act(()=>clock.tick(10000));
  assert.equal(query('#roomUpperSpace .mp-room-chat-log').children.length,0);
});

test('actual React chat keeps main copy, safe author text, stable controls, mute history and Player identity', async () => {
  const { model, calls } = fixture();
  await mount(model);
  const frame = query('#gameFrame');
  await act(() => query('.mp-quick-chat-prompt').click());
  assert.deepEqual(
    [...env.document.querySelectorAll('.mp-quick-chat-row')].map((row) =>
      [...row.querySelectorAll('button')].map((button) => button.textContent),
    ),
    owner.QUICK_CHAT_ROWS.map((row) => row.map((phrase) => phrase.zh)),
  );
  const phrase = query('[data-phrase="1"]');
  phrase.focus();
  await act(() => {
    model.update({
      ...context(),
    });
    model.receive(message());
  });
  assert.equal(query('[data-phrase="1"]'), phrase);
  assert.equal(env.document.activeElement, phrase);
  assert.equal(query('.mp-quick-chat-log').children.length, 1);
  assert.equal(query('.mp-quick-chat-log').querySelector('img'), null);
  assert.equal(query('.mp-quick-chat-log strong').textContent, 'P2(<img src=x onerror=alert(1)>)');
  await act(() => phrase.click());
  assert.deepEqual(calls.at(-1), [
    'send',
    {
      type: 'quick-chat',
      phrase: '1',
      serial: 7,
    },
  ]);
  await act(() => {
    query('.mp-quick-chat-prompt').click();
  });
  await act(() => query('.mp-quick-chat-mute').click());
  await act(() => query('.mp-quick-chat-mute-member').click());
  const voices = calls.filter(([kind]) => kind === 'voice').length;
  await act(() => model.receive(message()));
  assert.equal(query('.mp-quick-chat-log').children.length, 0);
  assert.equal(calls.filter(([kind]) => kind === 'voice').length, voices);
  await act(() => query('.mp-quick-chat-mute-member').click());
  assert.equal(query('.mp-quick-chat-log').children.length, 2);
  await act(() =>
    model.update({
      ...context(),
      localSeat: null,
    }),
  );
  assert.equal(query('.mp-quick-chat-prompt').disabled, false);
  assert.equal(query('[data-phrase="1"]').disabled, true);
  await act(() =>
    model.update({
      ...context(),
      connected: false,
    }),
  );
  assert.equal(query('.mp-quick-chat-prompt').disabled, true);
  assert.equal(query('#gameFrame'), frame);
});
test('native input effects cancel focus transfer and isolate input without cancelling touch scrolling', async () => {
  const { model } = fixture();
  await mount(model, {
    strict: true,
  });
  const root = query('.mp-quick-chat'),
    button = query('.mp-quick-chat-prompt');
  let leaked = 0;
  const leak = () => leaked++;
  env.document.addEventListener('pointerdown', leak);
  try {
    const pointer = new env.window.PointerEvent('pointerdown', {
      bubbles: true,
      cancelable: true,
    });
    button.dispatchEvent(pointer);
    assert.equal(pointer.defaultPrevented, true);
    assert.equal(leaked, 0);
    const mouse = new env.window.MouseEvent('mousedown', {
      bubbles: true,
      cancelable: true,
    });
    button.dispatchEvent(mouse);
    assert.equal(mouse.defaultPrevented, true);
    const touch = new env.window.Event('touchmove', {
      bubbles: true,
      cancelable: true,
    });
    root.dispatchEvent(touch);
    assert.equal(touch.defaultPrevented, false);
    await act(() => button.click());
    assert.equal(model.getSnapshot().pickerOpen, true);
    await act(() =>
      root.dispatchEvent(
        new env.window.KeyboardEvent('keydown', {
          key: 'Escape',
          bubbles: true,
        }),
      ),
    );
    assert.equal(model.getSnapshot().pickerOpen, false);
  } finally {
    env.document.removeEventListener('pointerdown', leak);
  }
});
function animations() {
  const previous = env.window.HTMLElement.prototype.animate,
    media = globalThis.matchMedia;
  const records = [];
  globalThis.matchMedia = () => ({
    matches: false,
  });
  env.window.HTMLElement.prototype.animate = function (frames, options) {
    let finish, reject;
    const record = {
      element: this,
      frames,
      options,
      canceled: false,
      finished: new Promise((resolve, rejectPromise) => {
        finish = resolve;
        reject = rejectPromise;
      }),
      finish: () => finish(),
      cancel() {
        this.canceled = true;
        reject(new Error('Synthetic animation canceled'));
      },
    };
    // Browser Animation.finished may reject on cancellation without a listener
    // for movement-only animations. Suppress fixture-global rejection noise.
    record.finished.catch(() => {});
    records.push(record);
    return record;
  };
  return {
    records,
    restore() {
      env.window.HTMLElement.prototype.animate = previous;
      globalThis.matchMedia = media;
    },
  };
}
test('StrictMode expiry waits fade completion; reset, unmount and dispose cancel owned animation/timer work', async () => {
  const native = animations();
  try {
    const { model, clock } = fixture();
    await mount(model, {
      strict: true,
    });
    await act(() => model.receive(message()));
    const arrival = native.records.find((record) => record.options.duration === 240);
    assert.equal(arrival.frames[0].opacity, 0);
    await act(() => clock.tick(10000));
    const fade = native.records.find((record) => record.options.duration === 280);
    assert.ok(fade);
    assert.equal(query('.mp-quick-chat-log').children.length, 1);
    await act(() => fade.finish());
    assert.equal(query('.mp-quick-chat-log').children.length, 0);
    await act(() => model.receive(message()));
    await act(() => clock.tick(10000));
    const stale = native.records.findLast((record) => record.options.duration === 280);
    await act(() =>
      model.update({
        ...context(),
        serial: 8,
      }),
    );
    assert.equal(stale.canceled, true);
    await act(() => {
      stale.finish();
      model.receive(
        message({
          serial: 8,
        }),
      );
    });
    assert.equal(query('.mp-quick-chat-log').children.length, 1);
    await act(() => clock.tick(10000));
    const pending = native.records.findLast((record) => record.options.duration === 280);
    await act(() => mounted.root.unmount());
    assert.equal(pending.canceled, true);
    assert.equal(model.getSnapshot().entries.length, 0);
    model.dispose();
    assert.equal(clock.tasks.size, 0);
    mounted = null;
  } finally {
    native.restore();
  }
});
class Socket {
  readyState = 0;
  sent = [];
  listeners = new Map();
  addEventListener(type, fn) {
    this.listeners.set(type, [...(this.listeners.get(type) || []), fn]);
  }
  emit(type, event = {}) {
    for (const fn of this.listeners.get(type) || []) fn(event);
  }
  open() {
    this.readyState = 1;
    this.emit('open');
  }
  message(value) {
    this.emit('message', {
      data: JSON.stringify(value),
    });
  }
  send(value) {
    this.sent.push(JSON.parse(value));
  }
  close() {
    this.readyState = 3;
    this.emit('close', {
      code: 1000,
    });
  }
}
test('actual RoomSession routes valid socket chat into React and UI sends through the same service', async () => {
  const sockets = [],
    decisions = owner.createDecisionStore(),
    clock = new Clock();
  const identity = owner.createMultiplayerIdentityStore({
    persistentStorage: null,
    sessionStorage: null,
    randomWords: () => [1234567, 7654321],
  });
  const settings = {
    language: 'en',
    options: {
      touchEnabled: false,
      touchMovementMode: 'joystick',
    },
  };
  const subscriptions = {
    subscribe: () => () => {},
  };
  let launched = true,
    replay = false;
  const previousAudio = globalThis.Audio;
  const voices = [];
  globalThis.Audio = class {
    constructor(src) {
      this.src = src;
    }
    play() {
      voices.push(this.src);
      return Promise.resolve();
    }
  };
  const session = owner.createRoomSession({
    window: env.window,
    document: env.document,
    storage: null,
    identity,
    sessions: owner.createMultiplayerRoomSessionStore({
      storage: null,
    }),
    settingsModel: {
      ...subscriptions,
      getSnapshot: () => settings,
    },
    sitePreferences: {
      ...subscriptions,
      getSnapshot: () => ({
        lessMotion: true,
      }),
    },
    runtime: () => ({
      getSnapshot: () => ({
        launched,
        runtimeVariant: 'multiplayer',
      }),
    }),
    replayViewer: () => replay,
    titleOverlayOpen: () => false,
    memberId: () => 'member123456',
    createSocket: () => {
      const socket = new Socket();
      sockets.push(socket);
      return socket;
    },
    relayUrl: () => 'wss://relay.invalid/socket',
    settings: () => ({
      touchEnabled: false,
      touchMovementMode: 'joystick',
      mobileDevice: false,
      iceServers: [],
    }),
    setMovementMode() {},
    decisions,
    translate: (key) => key,
    notify() {},
    prepareResources: async () => {},
    preparationFailed() {},
    beginManualImport() {},
    launch: async () => {},
    checkGame: async () => {},
    operationFailed() {},
    isLaunched: () => launched,
    routes: {
      restore() {},
      enter() {},
      settleInvite() {},
      leave() {},
    },
    now: () => clock.now,
    random: () => 0,
    setTimeout: clock.setTimeout,
    clearTimeout: clock.clearTimeout,
  });
  try {
    await mount(session.quickChat, {
      dispose: () => {
        session.dispose();
        decisions.dispose();
      },
    });
    await act(() => session.service.enterRoom('th06mp', '1234', true));
    const socket = sockets[0];
    await act(() => {
      socket.open();
      socket.message({
        type: 'state',
        room: {
          code: '1234',
          playerCount: 2,
          difficulty: 1,
          phase: 'running',
          inputDelay: 0,
          predictionLimit: 8,
          startSerial: 7,
          seats: [
            {
              clientId: identity.lobbyClientId('th06mp'),
              name: 'A',
              loadout: 0,
              ready: true,
            },
            {
              clientId: 'remote123456',
              name: 'B',
              loadout: 0,
              ready: true,
            },
          ],
          spectators: [{clientId: 'viewer123456', name: 'Viewer'}],
        },
      });
    });
    assert.equal(session.quickChat.getSnapshot().context.visible, true);
    assert.equal(query('.mp-quick-chat').hidden, false);
    await act(() => {
      socket.message(
        message({
          serial: 0,
          clientId: 'remote123456',
        }),
      );
      socket.message(
        message({
          clientId: 'remote123456',
        }),
      );
    });
    assert.equal(query('.mp-quick-chat-log').children.length, 1);
    assert.equal(voices.length, 1);
    await act(() => socket.message(message({
      seat: -1,
      spectator: true,
      clientId: 'viewer123456',
      phrase: 'thanks',
    })));
    assert.equal(query('.mp-quick-chat-log').children.length, 2);
    assert.match(query('.mp-quick-chat-log').children[1].textContent, /Spectator\(Viewer\).*Thanks for the games/);
    assert.equal(voices.length, 1, 'players see spectator messages without playing spectator voice');
    await act(() => query('[data-phrase="1"]').click());
    assert.deepEqual(socket.sent.at(-1), {
      type: 'quick-chat',
      phrase: '1',
      serial: 7,
    });
    const frame = query('#gameFrame');
    replay = true;
    await act(() => session.sync());
    assert.equal(query('.mp-quick-chat').hidden, true);
    await act(() =>
      socket.message(
        message({
          clientId: 'remote123456',
        }),
      ),
    );
    assert.equal(session.quickChat.getSnapshot().entries.length, 2);
    replay = false;
    launched = false;
    await act(() => session.sync());
    assert.equal(query('.mp-quick-chat').hidden, true);
    assert.equal(query('#gameFrame'), frame);
  } finally {
    globalThis.Audio = previousAudio;
  }
});

test('non-TH09 spectator quick chat mounts, sends through the shared model, and renders the observer identity', async () => {
  const {model, calls} = fixture();
  model.update({...context(), room: 'th07mp-5678', localSeat: null, localSpectator: true,
    localClientId: 'viewer123456',
    spectators: [
      {clientId: 'viewer123456', name: 'Local viewer'},
      {clientId: 'viewer654321', name: 'Remote viewer'},
    ]});
  await mount(model);
  assert.equal(query('.mp-quick-chat').hidden, false);
  assert.equal(query('[data-phrase="thanks"]').disabled, false);
  await act(() => query('[data-phrase="thanks"]').click());
  assert.deepEqual(calls.at(-1), ['send', {type: 'quick-chat', phrase: 'thanks', serial: 7}]);
  await act(() => model.receive({
    type: 'quick-chat', room: 'th07mp-5678', serial: 7, seat: -1,
    spectator: true, clientId: 'viewer654321', phrase: 'thanks',
  }));
  assert.match(query('.mp-quick-chat-log').textContent, /Spectator\(Remote viewer\).*谢谢指教/);
});

test('model disposal while React remains mounted cancels active fade and leaves no rows or timers', async () => {
  const native = animations();
  try {
    const { model, clock } = fixture();
    await mount(model, {
      strict: true,
    });
    await act(() => model.receive(message()));
    await act(() => clock.tick(10000));
    const fade = native.records.find((record) => record.options.duration === 280);
    assert.ok(fade);
    await act(() => model.dispose());
    assert.equal(fade.canceled, true);
    assert.equal(query('.mp-quick-chat-log').children.length, 0);
    assert.equal(clock.tasks.size, 0);
    await act(() => fade.finish());
    assert.equal(query('.mp-quick-chat-log').children.length, 0);
  } finally {
    native.restore();
  }
});
test('retained portals evict visible and muted rows at 50 and cancel all detached fades on reset', async () => {
  const native = animations();
  try {
    const { model, clock } = fixture();
    await mount(model, {
      strict: true,
    });
    for (let index = 0; index < 55; index++) await act(() => model.receive(message()));
    assert.equal(query('.mp-quick-chat-log').children.length, 50);
    await act(() => model.toggleMember('b'));
    for (let index = 0; index < 55; index++) await act(() => model.receive(message()));
    assert.equal(model.getSnapshot().entries.length, 50);
    assert.equal(query('.mp-quick-chat-log').children.length, 0);
    await act(() => model.toggleMember('b'));
    assert.equal(query('.mp-quick-chat-log').children.length, 50);
    await act(() => clock.tick(10000));
    const fades = native.records.filter((record) => record.options.duration === 280);
    assert.equal(fades.length, 50);
    await act(() => model.toggleMember('b'));
    assert.equal(
      fades.some((fade) => fade.canceled),
      false,
    );
    await act(() =>
      model.update({
        ...context(),
        serial: 8,
      }),
    );
    assert.equal(model.getSnapshot().entries.length, 0);
    assert.equal(
      fades.every((fade) => fade.canceled),
      true,
    );
    assert.equal(clock.tasks.size, 0);
  } finally {
    native.restore();
  }
});
test('presentation remount cancels detached fade but preserves another entry and its original model timer', async () => {
  const native = animations();
  try {
    const { model, clock } = fixture();
    await mount(model, {
      strict: true,
    });
    await act(() => model.receive(message()));
    await act(() => clock.tick(10000));
    const fade = native.records.find((record) => record.options.duration === 280);
    await act(() => {
      model.toggleMember('b');
      model.receive(message());
    });
    assert.equal(model.getSnapshot().entries.length, 2);
    await act(() => mounted.root.unmount());
    mounted = null;
    assert.equal(fade.canceled, true);
    assert.equal(model.getSnapshot().entries.length, 1);
    assert.equal(clock.tasks.size, 1);
    await mount(model, {
      strict: true,
    });
    assert.equal(query('.mp-quick-chat-log').children.length, 0);
    await act(() => model.toggleMember('b'));
    assert.equal(query('.mp-quick-chat-log').children.length, 1);
    await act(() => clock.tick(10000));
    await act(() => native.records.findLast((record) => record.options.duration === 280).finish());
    assert.equal(query('.mp-quick-chat-log').children.length, 0);
    assert.equal(clock.tasks.size, 0);
  } finally {
    native.restore();
  }
});
