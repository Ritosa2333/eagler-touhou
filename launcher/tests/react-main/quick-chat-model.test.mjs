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
    now() {
      return now;
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
    notifications = [],
    effects = [];
  const model = createMultiplayerQuickChatModel({
    timers: clock,
    now: clock.now,
    send(value) {
      sends.push(value);
      effects.push(['send', value]);
    },
    voice(id) {
      voices.push(id);
      effects.push(['voice', id]);
    },
    notify(code) {
      notifications.push(code);
    },
  });
  t.onTestFinished(() => model.dispose());
  if (initial) model.update(initial);
  return {
    model,
    clock,
    sends,
    voices,
    notifications,
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
test('picker toggle, openComposer and dismiss preserve the picker transition table', (t) => {
  const { model } = fixture(t);
  const expect = pickerOpen => assert.equal(model.getSnapshot().pickerOpen, pickerOpen);
  expect(false);
  model.togglePicker();
  expect(true);
  model.togglePicker();
  expect(false);
  model.openComposer();
  expect(true);
  model.togglePicker();
  expect(false);
  model.togglePicker();
  expect(true);
  model.dismiss();
  expect(false);
  const dismissed = model.getSnapshot();
  model.dismiss();
  assert.deepEqual(model.getSnapshot(), dismissed);
});
test('openComposer idempotently opens the picker', (t) => {
  const {model}=fixture(t);
  model.openComposer();
  assert.equal(model.getSnapshot().pickerOpen,true);
  const opened=model.getSnapshot();
  model.openComposer();
  assert.deepEqual(model.getSnapshot(),opened);
  model.dismiss();
  assert.equal(model.getSnapshot().pickerOpen,false);
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
test('guest sees free chat beside quick chat but cannot send free text', (t) => {
  const {model, sends, voices, clock} = fixture(t, context({localClientId:'local'}));
  model.receive({type:'chat-status',registered:false,authorId:'',bannedUntil:0});
  model.setDraft('guest text');
  model.sendText();
  model.receive({type:'free-chat',room:'ROOM',serial:7,seat:1,clientId:'peer',authorId:'account-2',messageId:4,name:'Marisa',text:'hello room'});
  model.receive(message());

  assert.equal(model.getSnapshot().entries.length,2);
  assert.equal(model.getSnapshot().entries[0].phrase.en,'hello room');
  assert.equal(model.getSnapshot().entries[0].messageId,4);
  assert.equal(model.getSnapshot().registered,false);
  assert.deepEqual(sends,[]);
  assert.deepEqual(voices,['thanks']);
  assert.equal(clock.jobs.size,2,'live quick chat and free text share the ten-second expiry');
  model.sendPhrase('thanks');
  assert.deepEqual(sends,[{type:'quick-chat',phrase:'thanks',serial:7}]);
});
test('guest mute is local and suppresses another member’s incoming voice', (t) => {
  const {model,sends,voices}=fixture(t,context({localClientId:'local'}));
  model.receive({type:'chat-status',registered:false,authorId:'',bannedUntil:0});
  model.toggleMember('peer');
  assert.deepEqual(model.getSnapshot().muted,['peer']);
  model.receive(message());
  assert.equal(model.getSnapshot().entries.length,1,'muted chat stays in history for the presenter to hide');
  assert.deepEqual(voices,[]);
  assert.deepEqual(sends,[],'local mute does not call an account or relay endpoint');
  model.toggleMember('peer');
  model.receive(message({phrase:'follow-me'}));
  assert.deepEqual(model.getSnapshot().muted,[]);
  assert.deepEqual(voices,['follow-me'],'unmuting restores incoming voice');
  assert.deepEqual(sends,[]);
});
test('live free text and bot expire in game while room history and profile updates survive', (t) => {
  const {model,clock,voices}=fixture(t,context({inRoom:true}));
  model.receive({type:'free-chat',room:'ROOM',serial:7,seat:1,clientId:'peer',authorId:'account-2',messageId:10,name:'Marisa',text:'hello'});
  const settlement={id:2,author:'ritosa',settledAt:100,changes:[]};
  model.receive({type:'th09-rating',room:'ROOM',settlement});
  clock.advance(9000);
  model.receive({type:'th09-rating',room:'ROOM',settlement:{...settlement,authorProfile:{name:'Ritosa'}}});
  assert.ok(model.getSnapshot().entries.every(entry=>!entry.expiredInGame));
  clock.advance(1000);
  assert.equal(model.getSnapshot().entries.length,2);
  assert.ok(model.getSnapshot().entries.every(entry=>entry.expiredInGame));
  assert.equal(model.getSnapshot().entries[1].name,'Ritosa');
  assert.deepEqual(clock.deadlines(),[]);
  model.update(context({inRoom:false}));
  assert.ok(model.getSnapshot().entries.every(entry=>entry.expiredInGame));
  assert.deepEqual(voices,[]);
});

test('free chat uses the shared entry list, accepts history once and never plays or deletes history', (t) => {
  const {model,voices,clock}=fixture(t);
  const free=(messageId,text)=>({type:'free-chat',room:'ROOM',serial:7,seat:1,clientId:'peer',authorId:'account-2',messageId,name:'Marisa',text});
  model.receive(free(10,'first'));
  model.receive({type:'chat-history',throughId:11,entries:[free(10,'duplicate'),free(11,'history')]});

  assert.deepEqual(model.getSnapshot().entries.map(entry=>[entry.messageId,entry.phrase.en]),[[10,'duplicate'],[11,'history']]);
  assert.deepEqual(voices,[]);
  assert.deepEqual(clock.deadlines(),[],'free chat did not create an expiry timer');
  clock.advance(10000);
  assert.deepEqual(model.getSnapshot().entries.map(entry=>entry.messageId),[10,11]);
  assert.deepEqual(clock.deadlines(),[]);
});
test('rating settlements survive run transitions and deduplicate replayed history', (t) => {
  const {model,voices}=fixture(t);
  const ranked={id:2,author:'ritosa',authorProfile:{name:'Ritosa',avatar:'0123456789abcdef'},settledAt:100,
    changes:[{username:'alice',before:null,after:1370}]};
  const unrated={id:3,author:'ritosa',settledAt:200,changes:[]};
  const live=settlement=>model.receive({type:'th09-rating',room:'ROOM',settlement});
  live(ranked);
  live(ranked);
  model.update(context({serial:8}));
  model.receive({type:'chat-history',room:'ROOM',throughId:0,entries:[],ratings:[ranked,unrated]});
  live(unrated);
  assert.deepEqual(model.getSnapshot().entries.map(entry=>entry.rating),[ranked,unrated]);
  assert.deepEqual(model.getSnapshot().entries.map(entry=>entry.name),['Ritosa','bot']);
  assert.deepEqual(voices,[]);
  model.update(context({room:'NEXT',serial:8}));
  live(ranked);
  assert.deepEqual(model.getSnapshot().entries,[]);
});
test('free chat history merges in message order when a newer live message arrives first', (t) => {
  const {model}=fixture(t);
  const free=messageId=>({type:'free-chat',room:'ROOM',serial:7,seat:1,clientId:'peer',authorId:'account-2',messageId,name:'Marisa',text:`message ${messageId}`});
  model.receive(free(51));
  model.receive({type:'chat-history',throughId:50,entries:Array.from({length:50},(_,index)=>free(index+1))});

  assert.deepEqual(model.getSnapshot().entries.map(entry=>entry.messageId),Array.from({length:50},(_,index)=>index+2));
  assert.equal(model.getSnapshot().entries.at(-1).phrase.en,'message 51');
});
test('free chat shares the fifty-entry cap and cancels evicted expiry timers', (t) => {
  const {model,voices,clock}=fixture(t);
  for(let messageId=1;messageId<=55;messageId++)model.receive({type:'free-chat',room:'ROOM',serial:7,seat:1,
    clientId:'peer',authorId:'account-2',messageId,name:'Marisa',text:`message ${messageId}`});
  assert.deepEqual(model.getSnapshot().entries.map(entry=>entry.messageId),Array.from({length:50},(_,index)=>index+6));
  assert.deepEqual(voices,[]);
  assert.equal(clock.jobs.size,50);
});
test('free chat accepts a labeled spectator and rejects mismatched seat and spectator roles', (t) => {
  const {model,voices}=fixture(t,context({spectators:[{clientId:'viewer',name:'Observer'}]}));
  const base={type:'free-chat',room:'ROOM',serial:7,clientId:'viewer',authorId:'account-3',messageId:1,name:'Observer',text:'watching'};
  model.receive({...base,seat:-1,spectator:true});
  assert.deepEqual(model.getSnapshot().entries.map(entry=>[entry.seat,entry.spectator,entry.name]),[[-1,true,'Observer']]);
  model.receive({...base,messageId:2,seat:-1,spectator:false});
  model.receive({...base,messageId:3,seat:0,spectator:true});
  model.receive({...base,messageId:4,seat:3,spectator:false});
  assert.deepEqual(model.getSnapshot().entries.map(entry=>entry.messageId),[1]);
  assert.deepEqual(voices,[]);
});
test('legacy history still loads while retracted messages stay suppressed', (t) => {
  const {model}=fixture(t);
  const free=(messageId,text)=>({type:'free-chat',room:'ROOM',serial:7,seat:1,clientId:'peer',
    authorId:'account-2',messageId,name:'Marisa',text});
  model.receive(free(4,'old live message'));
  model.receive(free(6,'new live message'));
  model.receive({type:'chat-retract',authorId:'account-2',throughId:4,bannedUntil:0});
  model.receive({type:'chat-history',throughId:5,entries:[free(4,'retracted from old history'),free(5,'legacy history')]});

  assert.deepEqual(model.getSnapshot().entries.map(entry=>entry.messageId).sort((a,b)=>a-b),[5,6]);
  assert.ok(model.getSnapshot().entries.every(entry=>entry.phrase.en!=='retracted from old history'));
});
test('older history watermarks cannot replace newer history or erase later live messages', (t) => {
  const {model}=fixture(t);
  const free=(messageId,text)=>({type:'free-chat',room:'ROOM',serial:7,seat:1,clientId:'peer',
    authorId:'account-2',messageId,name:'Marisa',text});
  model.receive({type:'chat-history',throughId:10,entries:[free(9,'snapshot nine'),free(10,'snapshot ten')]});
  model.receive(free(11,'live eleven'));
  model.receive({type:'chat-history',throughId:9,entries:[free(9,'stale snapshot nine')]});

  assert.deepEqual(model.getSnapshot().entries.map(entry=>[entry.messageId,entry.phrase.en]),[
    [9,'snapshot nine'],[10,'snapshot ten'],[11,'live eleven'],
  ]);
});
test('verified member sends trimmed text and waits for server results before enabling another send', (t) => {
  const {model,sends,notifications}=fixture(t,context({localClientId:'local'}));
  model.receive({type:'chat-status',registered:true,authorId:'account-1',bannedUntil:0});
  model.setDraft('  hello everyone  ');
  model.sendText();
  model.sendText();
  assert.deepEqual(sends,[{type:'free-chat',text:'hello everyone',serial:7}]);
  assert.equal(model.getSnapshot().sending,true);
  assert.equal(model.getSnapshot().draft,'');
  assert.equal(model.getSnapshot().entries.at(-1).delivery,'pending');

  model.setDraft('typed while sending');
  model.receive({type:'chat-result',action:'free-chat',ok:true});
  assert.equal(model.getSnapshot().sending,true);
  model.receive({type:'free-chat',room:'ROOM',serial:7,seat:0,clientId:'local',authorId:'account-1',messageId:42,name:'Local',text:'hello everyone'});
  assert.equal(model.getSnapshot().sending,false);
  assert.equal(model.getSnapshot().entries.length,1);
  assert.equal(model.getSnapshot().entries[0].messageId,42);
  assert.equal(model.getSnapshot().draft,'typed while sending');
  model.sendText();
  assert.deepEqual(sends.at(-1),{type:'free-chat',text:'typed while sending',serial:7});
  model.receive({type:'chat-result',action:'free-chat',ok:false,code:'send-rate-limited'});
  assert.equal(model.getSnapshot().sending,false);
  assert.equal(model.getSnapshot().draft,'');
  assert.equal(model.getSnapshot().entries.at(-1).delivery,'failed');
  assert.equal(model.getSnapshot().entries.at(-1).phrase.zh,'typed while sending');
  assert.equal(model.getSnapshot().sendError,'send-rate-limited');
  assert.deepEqual(notifications,[]);
  model.receive({type:'chat-result',action:'free-chat',ok:false,code:'sign-in-required'});
  assert.equal(model.getSnapshot().registered,false);
  assert.deepEqual(notifications,[]);
});
test('send timeout releases pending state, then disconnect clears verification and its timer', (t) => {
  const {model,clock,sends,notifications}=fixture(t,context({localClientId:'local'}));
  model.receive({type:'chat-status',registered:true,authorId:'account-1',bannedUntil:0});
  model.setDraft('keep this text');
  model.sendText();
  assert.equal(model.getSnapshot().sending,true);
  clock.advance(11999);
  assert.equal(model.getSnapshot().sending,true);
  clock.advance(1);
  assert.equal(model.getSnapshot().sending,false);
  assert.equal(model.getSnapshot().draft,'');
  assert.equal(model.getSnapshot().entries.at(-1).delivery,'failed');
  assert.equal(model.getSnapshot().sendError,'service-unavailable');
  assert.deepEqual(notifications,[]);

  model.setDraft('keep this text'); model.sendText();
  assert.equal(model.getSnapshot().sendError,undefined);
  assert.equal(clock.jobs.size,1);
  model.update(context({localClientId:'local',connected:false}));
  assert.equal(model.getSnapshot().registered,false);
  assert.equal(model.getSnapshot().authorId,'');
  assert.equal(model.getSnapshot().sending,false);
  assert.equal(clock.jobs.size,0);
  assert.equal(sends.length,2);
});
test('server ban blocks free text until its deadline and clears through the room clock', (t) => {
  const {model,clock,sends,notifications}=fixture(t,context({localClientId:'local'}));
  model.receive({type:'chat-status',registered:true,authorId:'account-1',bannedUntil:0});
  model.receive({type:'chat-retract',authorId:'account-1',throughId:9,bannedUntil:5000});
  assert.equal(model.getSnapshot().bannedUntil,5000);
  model.setDraft('still banned');
  model.sendText();
  assert.deepEqual(sends,[]);
  assert.deepEqual(notifications,['chat-banned']);
  clock.advance(5000);
  assert.equal(model.getSnapshot().bannedUntil,0);
  model.sendText();
  assert.deepEqual(sends,[{type:'free-chat',text:'still banned',serial:7}]);
});
test('room participants including guests can report other authors by message id', (t) => {
  const {model,sends,notifications}=fixture(t,context({localClientId:'local'}));
  model.receive({type:'chat-status',registered:true,authorId:'account-1',bannedUntil:0});
  const free=(messageId,authorId)=>({type:'free-chat',room:'ROOM',serial:7,seat:1,clientId:'peer',authorId,messageId,name:'Marisa',text:'hello'});
  model.receive(free(20,'account-2'));
  model.receive(free(21,'account-1'));
  model.reportMessage(20, '   ');
  model.reportMessage(20, 'x'.repeat(501));
  assert.deepEqual(sends,[]);
  model.reportMessage(20, 'test reason');
  model.reportMessage(21, 'test reason');
  assert.deepEqual(sends,[{type:'chat-report',messageId:20,reason:'test reason'}]);
  model.receive({type:'chat-result',action:'chat-report',ok:true});
  model.receive({type:'chat-result',action:'chat-report',ok:false,code:'report-rate-limited'});
  assert.deepEqual(notifications,['reported','report-rate-limited']);
  model.receive({type:'chat-status',registered:false,authorId:'',bannedUntil:0});
  model.reportMessage(20, 'test reason');
  assert.deepEqual(sends,[{type:'chat-report',messageId:20,reason:'test reason'},{type:'chat-report',messageId:20,reason:'test reason'}]);
});
test('retraction removes an author prefix, preserves quick chat and drops delayed free messages', (t) => {
  const {model,clock}=fixture(t);
  const free=(messageId,authorId)=>({type:'free-chat',room:'ROOM',serial:7,seat:1,clientId:'peer',authorId,messageId,name:'Marisa',text:`message ${messageId}`});
  model.receive(message({phrase:'thanks'}));
  model.receive(free(5,'account-2'));
  model.receive(free(6,'account-2'));
  model.receive(free(7,'account-3'));
  model.receive({type:'chat-retract',authorId:'account-2',throughId:5,bannedUntil:0});

  assert.deepEqual(model.getSnapshot().entries.map(entry=>entry.messageId??entry.phrase.id),['thanks',6,7]);
  model.receive(free(5,'account-2'));
  model.receive(free(4,'account-2'));
  assert.deepEqual(model.getSnapshot().entries.map(entry=>entry.messageId??entry.phrase.id),['thanks',6,7]);
  model.receive(free(8,'account-2'));
  assert.deepEqual(model.getSnapshot().entries.map(entry=>entry.messageId??entry.phrase.id),['thanks',6,7,8]);
  assert.equal(clock.jobs.size,4,'retract cancels only the removed entry expiry');
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
    assert.equal(clock.jobs.size, 1);
  }
  model.update(
    context({
      serial: 8,
    }),
  );
  assert.deepEqual(model.getSnapshot().entries, []);
  assert.deepEqual(model.getSnapshot().muted, []);
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

test('custom slots persist and keyboard sends the displayed page', (t) => {
  const previous = Object.getOwnPropertyDescriptor(globalThis, 'localStorage');
  const data = new Map();
  Object.defineProperty(globalThis, 'localStorage', {configurable: true, value: {
    getItem: key => data.get(key) ?? null,
    setItem: (key, value) => data.set(key, value),
  }});
  t.onTestFinished(() => previous ? Object.defineProperty(globalThis, 'localStorage', previous) : delete globalThis.localStorage);
  const {model, sends} = fixture(t);
  model.setSlot(1, 'you-first');
  model.openComposer();
  model.sendSlot(2);
  assert.equal(sends.at(-1).phrase, 'you-first');
  const other = fixture(t);
  assert.equal(other.model.getSnapshot().slots[1], 'you-first');
  other.model.openComposer();
  other.model.setPickerPage(2);
  other.model.sendSlot(1);
  assert.equal(other.sends.at(-1).phrase, phrases[9].id);
  other.model.openComposer();
  assert.equal(other.model.getSnapshot().pickerPage, 0);
});
