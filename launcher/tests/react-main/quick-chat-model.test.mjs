/** Deterministic state/timer tests and a pinned-original differential oracle.
 * Synthetic DOM only: no browser, layout, audio playback or deployment claim.
 */
import {test, beforeAll as before, afterAll as after} from 'vitest';
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdir, mkdtemp, rm } from 'node:fs/promises';
import { resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { build } from 'esbuild';
import { JSDOM } from 'jsdom';
import { authoredSourcesPlugin } from './authored-sources.mjs';
const root = fileURLToPath(new URL('../../', import.meta.url));
let work, createMultiplayerQuickChatModel;
import { QUICK_CHAT_PHRASES as phrases } from '../../src/contracts/multiplayer-quick-chat.mts';
before(async () => {
  await mkdir(resolve(root, '.cache'), {
    recursive: true,
  });
  work = await mkdtemp(resolve(root, '.cache/quick-chat-model-'));
  const modelFile = resolve(work, 'model.mjs');
  await build({
    entryPoints: [resolve(root, 'src/launcher/multiplayer-quick-chat-model.mts')],
    outfile: modelFile,
    bundle: true,
    format: 'esm',
    platform: 'node',
    logLevel: 'silent',
    plugins: [authoredSourcesPlugin(root)],
  });
  ({ createMultiplayerQuickChatModel } = await import(pathToFileURL(modelFile).href));
});
after(async () => {
  if (work)
    await rm(work, {
      recursive: true,
      force: true,
    });
});
function fakeClock() {
  let now = 0,
    serial = 0;
  const jobs = new Map();
  return {
    jobs,
    setTimeout(handler, ms) {
      const id = ++serial;
      jobs.set(id, {
        handler,
        due: now + ms,
        ms,
      });
      return id;
    },
    clearTimeout(id) {
      jobs.delete(id);
    },
    advance(ms) {
      const target = now + ms;
      for (;;) {
        const next = [...jobs]
          .filter(([, job]) => job.due <= target)
          .sort((a, b) => a[1].due - b[1].due || a[0] - b[0])[0];
        if (!next) break;
        const [id, job] = next;
        now = job.due;
        jobs.delete(id);
        job.handler();
      }
      now = target;
    },
    deadlines() {
      return [...jobs.values()].map((job) => job.due - now).sort((a, b) => a - b);
    },
  };
}
function context(overrides = {}) {
  return {
    visible: true,
    room: 'ROOM',
    serial: 7,
    localSeat: 0,
    connected: true,
    language: 'en',
    lessMotion: true,
    seats: [
      {
        clientId: 'local',
        name: 'Reimu',
      },
      {
        clientId: 'peer',
        name: 'Marisa',
      },
      null,
      {
        clientId: 'other',
        name: 'Sanae',
      },
    ],
    ...overrides,
  };
}
function message(overrides = {}) {
  return {
    type: 'quick-chat',
    room: 'ROOM',
    serial: 7,
    seat: 1,
    clientId: 'peer',
    phrase: 'thanks',
    ...overrides,
  };
}
function fixture(t, initial = context()) {
  const clock = fakeClock(),
    sends = [],
    voices = [],
    effects = [];
  const model = createMultiplayerQuickChatModel({
    timers: clock,
    send(value) {
      sends.push(value);
      effects.push(['send', value]);
    },
    voice(id) {
      voices.push(id);
      effects.push(['voice', id]);
    },
  });
  t.onTestFinished(() => model.dispose());
  if (initial) model.update(initial);
  return {
    model,
    clock,
    sends,
    voices,
    effects,
  };
}
const projectEntries = (entries) =>
  entries.map(({ clientId, seat, name, phrase }) => ({
    clientId,
    seat,
    name,
    phrase,
  }));

test('admitted spectators send text to all, with no spectator voice for players', (t) => {
  const observers=[{clientId:'viewer',name:'观众'}];
  const {model,voices,sends}=fixture(t,context({spectators:observers}));
  model.receive(message({seat:-1,clientId:'viewer',spectator:true}));
  assert.equal(model.getSnapshot().entries.length,1);
  assert.deepEqual(voices,[]);
  model.receive(message({seat:-1,clientId:'intruder',spectator:true}));
  assert.equal(model.getSnapshot().entries.length,1);
  model.receive(message());assert.equal(voices.length,1);
  model.update(context({spectators:observers,localSeat:null,localSpectator:true}));
  model.receive(message({seat:-1,clientId:'viewer',spectator:true}));assert.equal(voices.length,2);
  model.sendPhrase('thanks');assert.deepEqual(sends,[{type:'quick-chat',phrase:'thanks',serial:7}]);
});
const flush = async () => {
  await Promise.resolve();
  await Promise.resolve();
};
function deferred() {
  let resolve;
  const promise = new Promise((done) => {
    resolve = done;
  });
  return {
    promise,
    resolve,
  };
}
test('snapshot identity is stable for no-op updates and subscriptions unsubscribe', (t) => {
  const { model } = fixture(t, null);
  const empty = model.getSnapshot();
  assert.equal(empty.context, null);
  assert.deepEqual(empty.entries, []);
  assert.deepEqual(empty.muted, []);
  assert.equal(model.getSnapshot(), empty);
  let notifications = 0;
  const unsubscribe = model.subscribe(() => notifications++);
  model.update(context());
  const first = model.getSnapshot();
  assert.notEqual(first, empty);
  assert.equal(notifications, 1);
  model.update(context());
  assert.equal(model.getSnapshot(), first);
  assert.equal(notifications, 1);
  model.update(
    context({
      seats: context().seats.map((seat) =>
        seat
          ? {
              ...seat,
            }
          : null,
      ),
    }),
  );
  assert.equal(model.getSnapshot(), first);
  assert.equal(notifications, 1);
  unsubscribe();
  model.togglePicker();
  assert.equal(notifications, 1);
});
test('context seats are copied and previously published snapshots retain their values', (t) => {
  const { model } = fixture(t, null),
    input = context();
  model.update(input);
  const previous = model.getSnapshot();
  input.seats[1].name = 'Mutated outside model';
  input.seats.push({
    clientId: 'extra',
    name: 'Extra',
  });
  assert.equal(previous.context.seats[1].name, 'Marisa');
  assert.equal(previous.context.seats.length, 4);
  model.receive(message());
  const received = model.getSnapshot();
  assert.deepEqual(previous.entries, []);
  assert.equal(received.entries[0].name, 'Marisa');
  model.toggleMember('peer');
  assert.deepEqual(received.muted, []);
  model.update(
    context({
      seats: [
        context().seats[0],
        {
          clientId: 'peer',
          name: 'Renamed',
        },
        null,
      ],
    }),
  );
  assert.equal(model.getSnapshot().entries[0].name, 'Marisa');
  assert.equal(received.context.seats[1].name, 'Marisa');
});
test('receive rejects invalid room, serial, phrase, integer seat and seat identity without effects', (t) => {
  const { model, clock, voices } = fixture(t, null);
  model.receive(message());
  assert.equal(model.getSnapshot().entries.length, 0);
  model.update(context());
  const before = model.getSnapshot();
  const invalid = [
    {
      room: 'OTHER',
    },
    {
      room: undefined,
    },
    {
      serial: 8,
    },
    {
      serial: '7',
    },
    {
      phrase: 'arbitrary text',
    },
    {
      phrase: 1,
    },
    {
      phrase: undefined,
    },
    {
      seat: '1',
    },
    {
      seat: 1.1,
    },
    {
      seat: NaN,
    },
    {
      seat: Infinity,
    },
    {
      seat: -1,
    },
    {
      seat: 2,
    },
    {
      seat: 4,
    },
    {
      clientId: 'impostor',
    },
    {
      clientId: undefined,
    },
  ];
  for (const patch of invalid) {
    model.receive(message(patch));
    assert.equal(model.getSnapshot(), before, JSON.stringify(patch));
  }
  assert.deepEqual(voices, []);
  assert.equal(clock.jobs.size, 0);
  model.update(
    context({
      visible: false,
    }),
  );
  const hidden = model.getSnapshot();
  model.receive(message());
  assert.equal(model.getSnapshot(), hidden);
});
test('receive uses authoritative seat names and ignores connection, spectator and extra packet fields', (t) => {
  const { model, voices } = fixture(
    t,
    context({
      connected: false,
      localSeat: null,
    }),
  );
  model.receive(
    message({
      type: 'other',
      name: '<forged sender>',
      text: 'arbitrary text',
    }),
  );
  const entry = model.getSnapshot().entries[0];
  assert.equal(entry.clientId, 'peer');
  assert.equal(entry.seat, 1);
  assert.equal(entry.name, 'Marisa');
  assert.deepEqual(
    entry.phrase,
    phrases.find((phrase) => phrase.id === 'thanks'),
  );
  assert.equal(typeof entry.id, 'number');
  assert.deepEqual(voices, ['thanks']);
});
test('picker, mute/back and dismiss preserve the original transition table', (t) => {
  const { model } = fixture(t);
  const expect = (pickerOpen, muteOpen) => {
    assert.equal(model.getSnapshot().pickerOpen, pickerOpen);
    assert.equal(model.getSnapshot().muteOpen, muteOpen);
  };
  expect(false, false);
  model.togglePicker();
  expect(true, false);
  model.togglePicker();
  expect(false, false);
  model.toggleMute();
  expect(false, true);
  model.toggleMute();
  expect(true, false);
  model.toggleMute();
  expect(false, true);
  model.togglePicker();
  expect(false, false);
  model.togglePicker();
  model.dismiss();
  expect(false, false);
  const dismissed = model.getSnapshot();
  model.dismiss();
  assert.deepEqual(model.getSnapshot(), dismissed);
});
test('outbound voice precedes the exact relay payload, closes picker and has no local throttle or optimistic echo', (t) => {
  const { model, effects, sends, voices, clock } = fixture(t);
  model.togglePicker();
  model.sendPhrase('follow-me');
  model.sendPhrase('follow-me');
  model.sendPhrase('stop-fire');
  assert.deepEqual(sends, [
    {
      type: 'quick-chat',
      phrase: 'follow-me',
      serial: 7,
    },
    {
      type: 'quick-chat',
      phrase: 'follow-me',
      serial: 7,
    },
    {
      type: 'quick-chat',
      phrase: 'stop-fire',
      serial: 7,
    },
  ]);
  assert.deepEqual(voices, ['follow-me', 'follow-me', 'stop-fire']);
  assert.deepEqual(
    effects.map((effect) => effect[0]),
    ['voice', 'send', 'voice', 'send', 'voice', 'send'],
  );
  assert.equal(model.getSnapshot().pickerOpen, false);
  assert.deepEqual(model.getSnapshot().entries, []);
  assert.equal(clock.jobs.size, 0);
  model.update(
    context({
      serial: 9,
    }),
  );
  model.sendPhrase('1');
  assert.equal(sends.at(-1).serial, 9);
});
test('outbound invalid phrase, disconnected and spectator guards do not emit voice or payload', (t) => {
  const { model, sends, voices } = fixture(t, null);
  model.sendPhrase('thanks');
  model.update(context());
  model.sendPhrase('missing');
  model.sendPhrase(1);
  model.update(
    context({
      connected: false,
    }),
  );
  model.sendPhrase('thanks');
  model.update(
    context({
      localSeat: null,
    }),
  );
  model.sendPhrase('thanks');
  assert.deepEqual(sends, []);
  assert.deepEqual(voices, []);
  // The pinned sender checks seat and connection, without a visibility guard.
  model.update(
    context({
      visible: false,
    }),
  );
  model.sendPhrase('thanks');
  assert.equal(sends.length, 1);
});
test('muted messages are retained with timers, suppress only inbound voice and remain available when unmuted', (t) => {
  const { model, voices, clock } = fixture(t);
  model.receive(message());
  model.toggleMember('peer');
  model.receive(
    message({
      phrase: 'follow-me',
    }),
  );
  const snapshot = model.getSnapshot();
  assert.deepEqual(snapshot.muted, ['peer']);
  assert.equal(snapshot.entries.length, 2);
  assert.equal(clock.jobs.size, 2);
  assert.deepEqual(voices, ['thanks']);
  model.sendPhrase('thanks');
  assert.deepEqual(voices, ['thanks', 'thanks']);
  model.toggleMember('peer');
  assert.deepEqual(model.getSnapshot().muted, []);
  assert.deepEqual(
    model.getSnapshot().entries.map((entry) => entry.phrase.id),
    ['thanks', 'follow-me'],
  );
  assert.deepEqual(voices, ['thanks', 'thanks']);
});
test('visibility, connection, language, seat changes and motion preserve entries/mutes; only room or serial reset', (t) => {
  const { model, clock } = fixture(t);
  model.receive(message());
  model.toggleMember('peer');
  model.toggleMute();
  const entry = model.getSnapshot().entries[0];
  for (const patch of [
    {
      visible: false,
    },
    {
      connected: false,
    },
    {
      language: 'zh',
    },
    {
      localSeat: null,
    },
    {
      lessMotion: false,
    },
    {
      seats: [
        {
          clientId: 'new',
          name: 'New',
        },
      ],
    },
    {},
  ]) {
    model.update(context(patch));
    const snapshot = model.getSnapshot();
    assert.deepEqual(snapshot.entries, [entry]);
    assert.deepEqual(snapshot.muted, ['peer']);
    assert.equal(snapshot.muteOpen, true);
    assert.equal(clock.jobs.size, 1);
  }
  model.update(
    context({
      serial: 8,
    }),
  );
  assert.deepEqual(model.getSnapshot().entries, []);
  assert.deepEqual(model.getSnapshot().muted, []);
  assert.equal(model.getSnapshot().muteOpen, false);
  assert.equal(model.getSnapshot().pickerOpen, false);
  assert.equal(clock.jobs.size, 0);
  model.receive(
    message({
      serial: 8,
    }),
  );
  model.toggleMember('peer');
  model.togglePicker();
  model.update(
    context({
      room: 'NEXT',
      serial: 8,
    }),
  );
  assert.deepEqual(model.getSnapshot().entries, []);
  assert.deepEqual(model.getSnapshot().muted, []);
  assert.equal(model.getSnapshot().pickerOpen, false);
  assert.equal(clock.jobs.size, 0);
});
test('the 50-entry bound includes muted entries, evicts oldest first and cancels its timer', (t) => {
  const { model, clock, voices } = fixture(t);
  model.toggleMember('peer');
  for (let index = 0; index < 55; index++)
    model.receive(
      message({
        phrase: phrases[index % phrases.length].id,
      }),
    );
  const { entries } = model.getSnapshot();
  assert.equal(entries.length, 50);
  assert.equal(new Set(entries.map((entry) => entry.id)).size, 50);
  assert.deepEqual(
    entries.map((entry) => entry.phrase.id),
    Array.from(
      {
        length: 50,
      },
      (_, index) => phrases[(index + 5) % phrases.length].id,
    ),
  );
  assert.deepEqual(voices, []);
  assert.equal(clock.jobs.size, 50);
  model.toggleMember('peer');
  assert.equal(model.getSnapshot().entries.length, 50);
  clock.advance(10000);
  assert.deepEqual(model.getSnapshot().entries, []);
  assert.equal(clock.jobs.size, 0);
});
test('each duplicate has an independent identity and expires at exactly 10000 ms', (t) => {
  const { model, clock } = fixture(t);
  model.receive(message());
  clock.advance(1000);
  model.receive(message());
  const [first, second] = model.getSnapshot().entries;
  assert.notEqual(first.id, second.id);
  assert.deepEqual(clock.deadlines(), [9000, 10000]);
  clock.advance(8999);
  assert.equal(model.getSnapshot().entries.length, 2);
  clock.advance(1);
  assert.deepEqual(model.getSnapshot().entries, [second]);
  clock.advance(999);
  assert.deepEqual(model.getSnapshot().entries, [second]);
  clock.advance(1);
  assert.deepEqual(model.getSnapshot().entries, []);
});
test('expiry presenter starts after 10000 ms and owns completion without extending the original timer', async (t) => {
  const { model, clock } = fixture(t),
    gate = deferred(),
    presented = [];
  model.setExpiryPresenter((entry) => {
    presented.push(entry);
    return gate.promise;
  });
  model.receive(message());
  const entry = model.getSnapshot().entries[0];
  clock.advance(9999);
  assert.deepEqual(presented, []);
  clock.advance(1);
  assert.deepEqual(presented, [entry]);
  assert.deepEqual(model.getSnapshot().entries, [entry]);
  assert.equal(clock.jobs.size, 0);
  gate.resolve();
  await flush();
  assert.deepEqual(model.getSnapshot().entries, []);
});
test('a synchronous expiry presenter removes immediately and detaching restores timer-only removal', (t) => {
  const { model, clock } = fixture(t),
    presented = [];
  const detach = model.setExpiryPresenter((entry) => {
    presented.push(entry.id);
  });
  model.receive(message());
  const id = model.getSnapshot().entries[0].id;
  clock.advance(10000);
  assert.deepEqual(presented, [id]);
  assert.deepEqual(model.getSnapshot().entries, []);
  detach();
  model.receive(message());
  clock.advance(10000);
  assert.deepEqual(presented, [id]);
  assert.deepEqual(model.getSnapshot().entries, []);
});
test('late presentation completion after a room reset cannot remove a new-session entry', async (t) => {
  const { model, clock } = fixture(t),
    gate = deferred();
  const detach = model.setExpiryPresenter(() => gate.promise);
  model.receive(message());
  const old = model.getSnapshot().entries[0];
  clock.advance(10000);
  model.update(
    context({
      room: 'NEXT',
    }),
  );
  detach();
  model.receive(
    message({
      room: 'NEXT',
    }),
  );
  const current = model.getSnapshot().entries[0];
  assert.notEqual(current.id, old.id);
  gate.resolve();
  await flush();
  assert.deepEqual(model.getSnapshot().entries, [current]);
  clock.advance(10000);
  assert.deepEqual(model.getSnapshot().entries, []);
});
test('dispose clears scheduled expiry and late presenter completion cannot notify subscribers', async (t) => {
  const { model, clock } = fixture(t),
    gate = deferred();
  let notifications = 0;
  model.subscribe(() => notifications++);
  model.setExpiryPresenter(() => gate.promise);
  model.receive(message());
  clock.advance(10000);
  model.receive(message());
  assert.equal(clock.jobs.size, 1);
  model.dispose();
  const afterDispose = notifications;
  assert.equal(clock.jobs.size, 0);
  gate.resolve();
  await flush();
  clock.advance(10000);
  assert.equal(notifications, afterDispose);
  model.dispose();
});
