import {installNativeTimeline} from './native-animation-environment.mjs';
/** Production React chat + real room owner, synthetic DOM/socket/clock only.
 * Original browser geometry/held-iframe-key assertions remain unrun. */
import {test, beforeAll as before, afterAll as after, afterEach, vi} from 'vitest';
import assert from 'node:assert/strict';
import { mkdir, mkdtemp, rm } from 'node:fs/promises';
import { resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { build } from 'esbuild';
import { execFileSync } from 'node:child_process';
import { authoredSourcesPlugin } from './authored-sources.mjs';
import { installMountedDom } from './mounted-dom-environment.mjs';
import { TEST_FEATURES_KEY } from '../../src/launcher/test-features.mts';
const project = fileURLToPath(new URL('../../', import.meta.url));
let env, work, owner, ownerAccounts, React, createRoot, mounted;
before(async () => {
  env = installMountedDom();
  React = await import('react');
  ({ createRoot } = await import('react-dom/client'));
  await mkdir(resolve(project, '.cache'), {
    recursive: true,
  });
  work = await mkdtemp(resolve(project, '.cache/quick-chat-mounted-'));
  const contents = `
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
    export {accountSession, notifyAccountChange} from './app/services/account-session.ts';
    export {accountsEnabled} from './app/release-features.ts';
  `;
  const output = resolve(work, 'entry.mjs');
  await build({
    absWorkingDir: project,
    stdin: {
      resolveDir: project,
      loader: 'ts',
      contents,
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
  const accountOutput=resolve(work,'entry-accounts.mjs');
  await build({
    absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents},outfile:accountOutput,
    bundle:true,platform:'node',format:'esm',jsx:'automatic',packages:'external',
    define:{'import.meta.env':JSON.stringify({VITE_ACCOUNTS_ENABLED:'true'})},
    plugins:[{name:'account-request-fixture',setup(context){
      context.onResolve({filter:/account-client/},()=>({path:'account-client-fixture',namespace:'fixture'}));
      context.onLoad({filter:/.*/,namespace:'fixture'},()=>({loader:'js',contents:`
        export class AccountRequestError extends Error {constructor(code,status){super(code);this.name='AccountRequestError';this.code=code;this.status=status;}}
        export function cancelAccountRequests() {}
        export async function accountRequest(path,body,options){
          const request=globalThis.__roomAccountRequest;
          if(!request)throw Error('room account API fixture is missing');
          return request(path,body,options);
        }
      `}));
    }},authoredSourcesPlugin(project)],logLevel:'silent'});
  ownerAccounts=await import(pathToFileURL(accountOutput).href);
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
async function mount(model, { strict = false, dispose, room = false, decisions = {askText:async()=> 'test reason'} } = {}) {
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
        React.createElement(owner.MultiplayerQuickChat, {model,iconUrl:'/assets/player-chat.svg',decisions}),
        room === 'split' ? React.createElement(React.Fragment, null,
          React.createElement('div', {id:'roomUpperSpace'}, React.createElement(owner.RoomQuickChat, {model,part:'log',decisions})),
          React.createElement('div', {id:'roomBottomControls'}, React.createElement(owner.RoomQuickChat, {model,part:'actions'})))
          : room ? React.createElement(owner.RoomQuickChat, {model,decisions}) : null),
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
    calls = [],
    notifications = [];
  const model = owner.createMultiplayerQuickChatModel({
    timers: clock,
    now: () => clock.now,
    voice: (id) => calls.push(['voice', id]),
    send: (value) => calls.push(['send', value]),
    notify: (code) => notifications.push(code),
  });
  model.update(context());
  return {
    clock,
    calls,
    notifications,
    model,
  };
}
async function chooseRoomPhrase(calls) {
  await act(() => query('.room-quick-chat-picker [data-phrase="1"]').click());
  // Radix restores focus in the next task before running a queued menu action.
  await React.act(async () => {
    await vi.waitFor(() => assert.equal(calls.at(-1)?.[0], 'send'), {timeout: 1000, interval: 1});
  });
}
test('pregame room chat is independent and shares safe text, send and ten-second expiry', async () => {
  const {model,clock,calls}=fixture();
  model.update({...context(),inRoom:true});
  await mount(model,{room:true,strict:true});
  assert.equal(query('.mp-quick-chat').hidden,true);
  assert.equal(query('#mpRoomChat').contains(query('.mp-quick-chat')),false);
  await act(()=>query('#mpRoomChat .room-quick-chat-trigger').dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'ArrowDown',bubbles:true,cancelable:true})));
  await chooseRoomPhrase(calls);
  assert.equal(calls.at(-1)[0],'send');
  await act(()=>model.receive(message()));
  assert.equal(query('#mpRoomChat .mp-room-chat-log strong').textContent,'<img src=x onerror=alert(1)>');
  assert.equal(query('#mpRoomChat').querySelector('img'),null);
  await act(()=>clock.tick(9999));
  assert.equal(query('#mpRoomChat .mp-room-chat-log').children.length,1);
  await act(()=>clock.tick(1));
  assert.equal(query('#mpRoomChat .mp-room-chat-log').children.length,0);
});
test('chat uses and updates the matching participant profile avatar', async () => {
  const {model}=fixture();
  const original=owner; owner=ownerAccounts;
  try {await mount(model,{room:true});} finally {owner=original;}
  await act(()=>model.receive(message()));
  const current=context();
  current.seats[1].profile={username:'marisa',avatar:'1234567890abcdef'};
  await act(()=>model.update(current));
  assert.match(query('#mpRoomChat .chat-avatar img').getAttribute('src'), /\/api\/v1\/profiles\/marisa\/avatar\/1234567890abcdef$/);
  const changed=context(); changed.seats[1].profile={username:'marisa',avatar:'abcdef1234567890'};
  await act(()=>model.update(changed));
  assert.match(query('#mpRoomChat .chat-avatar img').getAttribute('src'), /abcdef1234567890$/);
});
test('settlement chat uses a plain bot until the published account is available', async () => {
  const {model}=fixture();
  const room='th09mp-1234';
  model.update({...context(),room});
  const original=owner; owner=ownerAccounts;
  try {await mount(model,{room:true});} finally {owner=original;}
  const settlement={id:1,author:'ritosa',settledAt:100,changes:[]};
  await act(()=>model.receive({type:'th09-rating',room,settlement}));
  const row=query('#mpRoomChat .room-chat-entry');
  assert.equal(row.querySelector('.chat-sender').textContent,'bot');
  assert.equal(row.querySelector('.chat-avatar, a'),null);

  const authorProfile={name:'Ritosa',avatar:'1234567890abcdef'};
  await act(()=>model.receive({type:'chat-history',room,throughId:0,entries:[],ratings:[{...settlement,authorProfile}]}));
  assert.equal(env.document.querySelectorAll('#mpRoomChat .room-chat-entry').length,1);
  for (const view of ['#mpRoomChat','.mp-quick-chat-log']) {
    assert.equal(query(`${view} .chat-role`).textContent,'bot');
    assert.equal(query(`${view} .chat-sender a`).textContent,'Ritosa');
    assert.match(query(`${view} .chat-sender a`).getAttribute('href'), /^\/u\/ritosa(?:\?|$)/);
    assert.match(query(`${view} a.chat-avatar`).getAttribute('href'), /^\/u\/ritosa(?:\?|$)/);
    assert.equal(query(`${view} .chat-avatar img`).getAttribute('src'),'/api/v1/profiles/ritosa/avatar/1234567890abcdef');
  }
});
test('account-enabled chat gives guests the same input with a sign-in hint', async () => {
  const {model,calls}=fixture();
  const original=owner; owner=ownerAccounts;
  try {await mount(model,{room:true});} finally {owner=original;}
  const views=['#mpRoomChat','.mp-quick-chat-picker'];
  const inputs=views.map(view=>query(`${view} input[aria-label="Message"]`));
  for (const input of inputs) {
    assert.equal(input.placeholder,'Sign in to chat');
    assert.equal(input.disabled,true);
    const form=input.closest('form');
    assert.equal(form.querySelector('button[aria-label="Send"]').disabled,true);
    await act(()=>form.dispatchEvent(new env.window.Event('submit',{bubbles:true,cancelable:true})));
  }
  assert.deepEqual(calls,[]);
  assert.equal(query('#mpRoomChat button[aria-label="Quick chat"]').disabled,false);
  await act(()=>model.receive({type:'chat-status',registered:true,authorId:'account-1',bannedUntil:0}));
  for (const [index,view] of views.entries()) {
    assert.equal(query(`${view} input[aria-label="Message"]`),inputs[index]);
    assert.equal(inputs[index].disabled,false);
  }
});
test('room messages stay above the controls and sending closes the upward picker', async () => {
  const {model,clock,calls}=fixture();model.update({...context(),inRoom:true});
  await mount(model,{room:'split',strict:true});
  assert.equal(query('#roomBottomControls').querySelector('.mp-room-chat-log'),null);
  const actions=query('#mpRoomChat .room-quick-chat-trigger');await act(()=>actions.dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'ArrowDown',bubbles:true,cancelable:true})));
  assert.equal(query('#roomBottomControls').querySelector('.room-quick-chat-picker'),null);
  await chooseRoomPhrase(calls);
  assert.equal(calls.at(-1)[0],'send');assert.equal(actions.getAttribute('aria-expanded'),'false');
  await act(()=>model.receive(message()));
  assert.equal(query('#roomUpperSpace .mp-room-chat-log').children.length,1);
  await act(()=>clock.tick(10000));
  assert.equal(query('#roomUpperSpace .mp-room-chat-log').children.length,0);
});

test('guest room UI keeps quick chat and shared history; registered chat can send and report', async () => {
  const {model,calls,notifications}=fixture();
  await mount(model,{room:true,strict:true});
  assert.equal(env.document.querySelector('#mpRoomChat input[aria-label="Message"]'),null);
  assert.ok(query('#mpRoomChat .room-quick-chat-trigger'));

  const free=(messageId,authorId,clientId,seat,name,text)=>({type:'free-chat',room:'th06mp-1234',serial:7,
    messageId,authorId,clientId,seat,spectator:seat===-1,name,text});
  await act(()=>model.receive(free(30,'account-2','b',1,'Marisa','Hello room')));
  assert.equal(query('#mpRoomChat .room-chat-message').textContent,'MarisaP2Hello room');
  assert.equal(query('#mpRoomChat .chat-avatar').textContent,'M');
  assert.equal(query('.mp-quick-chat-log .room-chat-message').textContent,'MarisaP2Hello room',
    'the gameplay and room views render the same model entry');
  assert.equal(calls.filter(([kind])=>kind==='voice').length,0,'free chat never plays quick-chat voice');

  const muteTrigger=query('#mpRoomChat .room-chat-mute-trigger');
  const mutePointer=new env.window.PointerEvent('pointerdown',{button:0,bubbles:true,cancelable:true});
  await act(()=>muteTrigger.dispatchEvent(mutePointer));
  await React.act(async()=>vi.waitFor(()=>assert.ok(env.document.querySelector('[role="menuitemcheckbox"]')),{timeout:1000,interval:1}));
  const memberOption=[...env.document.querySelectorAll('[role="menuitemcheckbox"]')].find(item=>item.textContent.includes('P2'));
  assert.ok(memberOption);
  await React.act(async()=>{
    memberOption.click();
    await vi.waitFor(()=>assert.deepEqual(model.getSnapshot().muted,['b']),{timeout:1000,interval:1});
  });
  assert.deepEqual(model.getSnapshot().muted,['b']);
  assert.equal(env.document.querySelectorAll('#mpRoomChat .room-chat-entry').length,0,'room text hides for a guest mute');
  assert.equal(query('.mp-quick-chat-log .room-chat-entry').hidden,true);
  assert.deepEqual(calls,[],'guest mute stays local');
  await React.act(async()=>{
    memberOption.click();
    await vi.waitFor(()=>assert.deepEqual(model.getSnapshot().muted,[]),{timeout:1000,interval:1});
  });
  assert.deepEqual(model.getSnapshot().muted,[],'the shared checkbox menu can unmute without closing');
  assert.equal(query('#mpRoomChat .room-chat-entry .room-chat-message').textContent,'MarisaP2Hello room');

  await act(()=>model.receive({type:'chat-status',registered:true,authorId:'account-1',bannedUntil:0}));
  const input=query('#mpRoomChat input[aria-label="Message"]');
  const setter=Object.getOwnPropertyDescriptor(env.window.HTMLInputElement.prototype,'value').set;
  setter.call(input,'  mounted message  ');
  await act(()=>input.dispatchEvent(new env.window.Event('input',{bubbles:true})));
  await act(()=>query('#mpRoomChat button[aria-label="Send"]').click());
  assert.deepEqual(calls.at(-1),['send',{type:'free-chat',text:'mounted message',serial:7}]);
  await act(()=>model.receive({type:'chat-result',action:'free-chat',ok:true}));
  assert.equal(input.value,'');

  await act(()=>model.receive(free(31,'account-1','a',0,'Local','My own message')));
  const actions=env.document.querySelectorAll('#mpRoomChat .room-chat-entry button[aria-label="Message actions"]');
  assert.equal(actions.length,1,'another author has a message action and the local author has none');
  const ownRow=[...env.document.querySelectorAll('#mpRoomChat .room-chat-entry')].find(row=>row.textContent.includes('My own message'));
  assert.ok(ownRow);assert.equal(ownRow.querySelector('button[aria-label="Message actions"]'),null,'own messages cannot be muted or reported');
  assert.equal(actions[0].getAttribute('data-variant'),'ghost');
  assert.ok(actions[0].classList.contains('text-inherit'));
  await act(()=>actions[0].dispatchEvent(new env.window.PointerEvent('pointerdown',{button:0,bubbles:true,cancelable:true})));
  await React.act(async()=>vi.waitFor(()=>assert.ok(env.document.querySelector('[role="menuitem"]')),{timeout:1000,interval:1}));
  const reportMenu=query('[role="menu"]');
  assert.ok(reportMenu.classList.contains('room-chat-message-menu'));
  assert.equal(reportMenu.getAttribute('data-side'),'bottom');
  assert.equal(reportMenu.getAttribute('data-align'),'end');
  assert.match(reportMenu.style.minWidth,/140px/);
  const actionItems=[...reportMenu.querySelectorAll('[role="menuitem"]')];
  assert.deepEqual(actionItems.map(item=>item.textContent.trim()).sort(),['Mute messages','Report']);
  const report=actionItems.find(item=>item.textContent.includes('Report'));
  assert.ok(report);
  assert.equal(report.textContent.includes('Report'),true);
  await React.act(async()=>{
    report.click();
    await vi.waitFor(()=>assert.deepEqual(calls.at(-1),['send',{type:'chat-report',messageId:30,reason:'test reason'}]),{timeout:1000,interval:1});
  });
  await act(()=>model.receive({type:'chat-result',action:'chat-report',ok:false,code:'report-rate-limited'}));
  assert.deepEqual(notifications,['report-rate-limited']);
});

test('guest gameplay chat opens numbered quick phrases and keeps the shared mute controls', async () => {
  const {model,calls}=fixture();
  await mount(model,{strict:true});
  const frame=query('#gameFrame');
  const quickTrigger=query('.mp-quick-chat-prompt');
  assert.equal(env.document.querySelector('.mp-quick-chat input[aria-label="Message"]'),null);
  await act(()=>quickTrigger.click());
  assert.deepEqual([...query('.mp-quick-chat-slots').querySelectorAll('[aria-keyshortcuts]')].map(
    (button)=>button.getAttribute('aria-keyshortcuts')), ['1','2','3','4','5','6','7','8','9']);
  const phrase=query('.mp-quick-chat-slots [data-phrase="1"]');
  phrase.focus();
  await act(()=>model.receive(message()));
  assert.equal(query('.mp-quick-chat-log .room-chat-entry').querySelector('button'),null);
  assert.equal(query('.mp-quick-chat-log .room-chat-entry').hidden,false);
  assert.equal(query('.mp-quick-chat-log').querySelector('img'),null);
  assert.equal(query('.mp-quick-chat-log strong').textContent,'<img src=x onerror=alert(1)>');
  const retainedRow=query('.mp-quick-chat-log .room-chat-entry');
  assert.equal(env.document.activeElement,phrase);

  await React.act(async()=>{
    phrase.click();
    await vi.waitFor(()=>assert.deepEqual(calls.at(-1),['send',{type:'quick-chat',phrase:'1',serial:7}]),{timeout:1000,interval:1});
  });
  assert.equal(query('.mp-quick-chat-picker').hidden,true);
  const sentBeforeMute=calls.filter(([kind])=>kind==='send');
  await act(()=>quickTrigger.click());
  const muteTrigger=query('.mp-quick-chat .room-chat-mute-trigger');
  await act(()=>muteTrigger.dispatchEvent(new env.window.PointerEvent('pointerdown',{button:0,bubbles:true,cancelable:true})));
  const mutedMember=query('[role="menuitemcheckbox"]');
  assert.match(mutedMember.textContent,/P2/);
  assert.equal(mutedMember.getAttribute('aria-checked'),'false');
  await act(()=>mutedMember.click());
  assert.deepEqual(model.getSnapshot().muted,['b']);
  assert.equal(query('.mp-quick-chat-log .room-chat-entry'),retainedRow);
  assert.equal(retainedRow.hidden,true);
  const voices=calls.filter(([kind])=>kind==='voice').length;
  await act(()=>model.receive(message({phrase:'follow-me'})));
  assert.equal(model.getSnapshot().entries.length,2);
  assert.ok([...query('.mp-quick-chat-log').children].every(row=>row.hidden),'muted text remains hidden');
  assert.equal(calls.filter(([kind])=>kind==='voice').length,voices,'mute also suppresses later incoming voice');

  assert.equal(mutedMember.getAttribute('aria-checked'),'true');
  await React.act(async()=>{
    mutedMember.click();
    await vi.waitFor(()=>assert.deepEqual(model.getSnapshot().muted,[]),{timeout:1000,interval:1});
  });
  assert.deepEqual(model.getSnapshot().muted,[]);
  assert.equal(retainedRow.hidden,false,'unmuting restores the same text row');
  assert.equal(calls.filter(([kind])=>kind==='send').length,sentBeforeMute.length,'mute and unmute are local');
  const restoredVoices=calls.filter(([kind])=>kind==='voice').length;
  await act(()=>model.receive(message({phrase:'stop-fire'})));
  assert.equal(calls.filter(([kind])=>kind==='voice').length,restoredVoices+1,'unmuting restores incoming voice');
  assert.ok([...query('.mp-quick-chat-log').children].every(row=>!row.hidden));

  await act(()=>model.update({...context(),localSeat:null}));
  assert.equal(quickTrigger.disabled,true);
  await act(()=>model.update({...context(),connected:false}));
  assert.equal(quickTrigger.disabled,true);
  assert.equal(query('#gameFrame'),frame);
});
test('gameplay chat isolates pointer and touch without cancelling native controls or scrolling', async () => {
  const { model } = fixture();
  model.receive({type:'chat-status',registered:true,authorId:'account-1',bannedUntil:0});
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
    assert.equal(pointer.defaultPrevented, false);
    assert.equal(leaked, 0);
    const mouse = new env.window.MouseEvent('mousedown', {
      bubbles: true,
      cancelable: true,
    });
    button.dispatchEvent(mouse);
    assert.equal(mouse.defaultPrevented, false);
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
test('gameplay free-chat input keeps pointer focus, isolates game keys, and blocks composing Enter', async () => {
  const {model,calls}=fixture();
  await mount(model,{strict:true});
  await act(()=>model.receive({type:'chat-status',registered:true,authorId:'account-1',bannedUntil:0}));
  await act(()=>query('.mp-quick-chat-prompt').click());
  const input=query('.mp-quick-chat-picker input[aria-label="Message"]');
  const pointer=new env.window.PointerEvent('pointerdown',{bubbles:true,cancelable:true});
  input.dispatchEvent(pointer);
  assert.equal(pointer.defaultPrevented,false);
  input.focus();
  assert.equal(env.document.activeElement,input);

  let escaped=0;
  const observe=()=>escaped++;
  env.document.addEventListener('keydown',observe);
  try {
    input.dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'w',bubbles:true,cancelable:true}));
    assert.equal(escaped,0,'ordinary gameplay keys remain isolated inside the chat control');
    const composingEnter=new env.window.KeyboardEvent('keydown',{key:'Enter',isComposing:true,bubbles:true,cancelable:true});
    input.dispatchEvent(composingEnter);
    assert.equal(composingEnter.defaultPrevented,true,'IME confirmation does not submit the message');
    assert.equal(calls.some(([kind,value])=>kind==='send'&&value.type==='free-chat'),false);
  } finally {
    env.document.removeEventListener('keydown',observe);
  }
});
test('tapping another player’s free chat opens a report confirmation and sends only on confirm', async () => {
  const {model,calls}=fixture();
  const decisions=owner.createDecisionStore();
  await mount(model,{strict:true,decisions,dispose:decisions.dispose});
  await act(()=>model.receive({type:'free-chat',room:'th06mp-1234',serial:7,messageId:30,seat:1,spectator:false,
    clientId:'other',authorId:'account-2',name:'Marisa',text:'Hello room'}));
  const trigger=query('.mp-quick-chat-log .chat-report-trigger');
  assert.match(trigger.textContent,/Hello room/);
  const pointer=new env.window.PointerEvent('pointerdown',{button:0,bubbles:true,cancelable:true});
  await act(()=>trigger.dispatchEvent(pointer));
  assert.equal(pointer.defaultPrevented,false);
  await act(()=>trigger.click());
  assert.equal(decisions.getSnapshot().message,'Hello room');
  assert.equal(decisions.getSnapshot().confirmText,'Report');
  assert.equal(env.document.querySelector('[role="menu"]'),null);
  await act(()=>decisions.resolve('cancel'));
  assert.deepEqual(calls,[]);
  await act(()=>trigger.click());
  await act(()=>decisions.resolve('confirm','test reason'));
  assert.deepEqual(calls,[['send',{type:'chat-report',messageId:30,reason:'test reason'}]]);
});
test('openComposer focuses the quick panel and typing hides slots while Enter and Escape still work', async () => {
  const {model,calls}=fixture();
  await mount(model,{strict:true});
  const prompt=query('.mp-quick-chat-prompt'),icon=prompt.querySelector('img');
  assert.equal(icon.getAttribute('src'),'/assets/player-chat.svg');
  await act(()=>model.receive({type:'chat-status',registered:true,authorId:'account-1',bannedUntil:0}));
  assert.equal(query('.mp-quick-chat-prompt'),prompt);
  assert.equal(prompt.querySelector('img'),icon);
  await act(()=>model.openComposer());
  const input=query('.mp-quick-chat-picker input[aria-label="Message"]'),frame=query('#gameFrame'),
    form=input.closest('form'),send=query('.mp-quick-chat-picker button[aria-label="Send"]');
  assert.equal(env.document.activeElement,query('.mp-quick-chat-picker'));
  assert.equal(input.placeholder,'Press Tab to switch to free chat');
  assert.ok(query('.mp-quick-chat-slots [aria-keyshortcuts="9"]'));
  input.focus();

  const setter=Object.getOwnPropertyDescriptor(env.window.HTMLInputElement.prototype,'value').set;
  setter.call(input,'mounted message');
  await act(()=>input.dispatchEvent(new env.window.Event('input',{bubbles:true})));
  assert.equal(model.getSnapshot().editingText,true);
  assert.equal(env.document.querySelector('.mp-quick-chat-slots'),null);
  assert.equal(env.document.activeElement,input);
  const composingEnter=new env.window.KeyboardEvent('keydown',{key:'Enter',isComposing:true,bubbles:true,cancelable:true});
  await act(()=>input.dispatchEvent(composingEnter));
  assert.equal(composingEnter.defaultPrevented,true);
  assert.equal(calls.some(([kind,value])=>kind==='send'&&value.type==='free-chat'),false);

  const enter=new env.window.KeyboardEvent('keydown',{key:'Enter',bubbles:true,cancelable:true});
  await act(()=>input.dispatchEvent(enter));
  assert.equal(enter.defaultPrevented,false,'ordinary Enter remains available for form submission');
  await act(()=>form.dispatchEvent(new env.window.SubmitEvent('submit',{bubbles:true,cancelable:true,submitter:send})));
  assert.deepEqual(calls.at(-1),['send',{type:'free-chat',text:'mounted message',serial:7}]);

  const composingEscape=new env.window.KeyboardEvent('keydown',{key:'Escape',isComposing:true,bubbles:true,cancelable:true});
  await act(()=>input.dispatchEvent(composingEscape));
  assert.equal(model.getSnapshot().pickerOpen,true,'IME composition keeps the composer open');
  assert.equal(env.document.activeElement,input);
  const escape=new env.window.KeyboardEvent('keydown',{key:'Escape',bubbles:true,cancelable:true});
  await act(()=>input.dispatchEvent(escape));
  assert.equal(escape.defaultPrevented,true);
  assert.equal(model.getSnapshot().pickerOpen,false);
  assert.equal(env.document.activeElement,frame,'Escape returns keyboard focus to the game iframe');
  await act(()=>model.openComposer());
  assert.ok(query('.mp-quick-chat-slots [aria-keyshortcuts="9"]'));
});
test('typing after the slots close does not measure retained message rows', async () => {
  const {model}=fixture();
  await mount(model);
  await act(()=>model.receive(message()));
  await act(()=>model.receive({type:'chat-status',registered:true,authorId:'account-1',bannedUntil:0}));
  await act(()=>query('.mp-quick-chat-prompt').click());
  await act(()=>model.setDraft('t'));
  const row=query('.mp-quick-chat-log .room-chat-entry');
  let measurements=0;
  const measure=row.getBoundingClientRect.bind(row);
  row.getBoundingClientRect=()=>{measurements++;return measure();};

  await act(()=>model.setDraft('typing'));
  assert.equal(measurements,0);
});
const expiryAnimation = record => record.keyframes.opacity?.[1] === '0';
function animations() {
  const previous = env.window.HTMLElement.prototype.animate, previousGetAnimations = env.window.HTMLElement.prototype.getAnimations;
  const media = globalThis.matchMedia, windowMedia = env.window.matchMedia;
  const records = [];
  globalThis.matchMedia = env.window.matchMedia = () => ({matches:false, addEventListener(){}, removeEventListener(){}});
  installNativeTimeline(env.window, records);
  return {records, restore() {
    env.window.HTMLElement.prototype.animate = previous;
    env.window.HTMLElement.prototype.getAnimations = previousGetAnimations;
    globalThis.matchMedia = media; env.window.matchMedia = windowMedia;
  }};
}
test('gameplay free text and bot fade after ten seconds and remain readable in the room', async () => {
  const native=animations();
  try {
    const {model,clock}=fixture();
    await mount(model,{room:true,strict:true});
    await act(()=>model.receive({type:'free-chat',room:'th06mp-1234',serial:7,seat:1,clientId:'b',authorId:'account-2',messageId:10,name:'Marisa',text:'hello'}));
    await act(()=>model.receive({type:'th09-rating',room:'th06mp-1234',settlement:{id:1,author:'ritosa',settledAt:100,changes:[]}}));
    assert.equal([...query('.mp-quick-chat-log').children].filter(row=>!row.hidden).length,2);
    await act(()=>clock.tick(9999));
    assert.equal(native.records.filter(expiryAnimation).length,0);
    await act(()=>clock.tick(1));
    const fades=native.records.filter(expiryAnimation);
    assert.equal(fades.length,2);
    await act(()=>fades.forEach(animation=>animation.finish()));
    assert.equal([...query('.mp-quick-chat-log').children].filter(row=>!row.hidden).length,0);
    assert.equal(model.getSnapshot().entries.length,2);
    await act(()=>model.update({...context(),inRoom:true}));
    assert.equal([...query('#mpRoomChat .mp-room-chat-log').children].filter(row=>!row.hidden).length,2);
    assert.match(query('#mpRoomChat .mp-room-chat-log').textContent,/hello/);
    assert.match(query('#mpRoomChat .mp-room-chat-log').textContent,/The match has ended/);
    await act(()=>model.receive({type:'free-chat',room:'th06mp-1234',serial:7,seat:1,clientId:'b',authorId:'account-2',messageId:11,name:'Marisa',text:'room message'}));
    await act(()=>clock.tick(10000));
    assert.equal([...query('#mpRoomChat .mp-room-chat-log').children].filter(row=>!row.hidden).length,3);
  } finally {native.restore();}
});

test('StrictMode expiry waits fade completion; reset, unmount and dispose cancel owned animation/timer work', async () => {
  const native = animations();
  try {
    const { model, clock } = fixture();
    await mount(model, {
      strict: true,
    });
    await act(() => model.receive(message()));
    const arrival = native.records.find(record => record.keyframes.opacity?.[1] === '1');
    assert.equal(Number(arrival.keyframes.opacity[0]), 0);
    assert.equal(arrival.options.duration, 180);
    await act(() => clock.tick(10000));
    const fade = native.records.find(expiryAnimation);
    assert.ok(fade);
    assert.equal(query('.mp-quick-chat-log').children.length, 1);
    await act(() => fade.finish());
    assert.equal(query('.mp-quick-chat-log').children.length, 0);
    await act(() => model.receive(message()));
    await act(() => clock.tick(10000));
    const stale = native.records.findLast(expiryAnimation);
    await act(() =>
      model.update({
        ...context(),
        serial: 8,
      }),
    );
    assert.equal(stale.cancelled, true);
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
    const pending = native.records.findLast(expiryAnimation);
    await act(() => mounted.root.unmount());
    assert.equal(pending.cancelled, true);
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
function makeRoomSession(bundle, options = {}) {
  const sockets = [], notifications = [], decisions = bundle.createDecisionStore(), clock = new Clock();
  const identity = bundle.createMultiplayerIdentityStore({
    persistentStorage: null,
    sessionStorage: null,
    randomWords: () => [1234567, 7654321],
  });
  let launched = true;
  const settings = {language:'en',options:{touchEnabled:false,touchMovementMode:'joystick'}};
  const subscriptions = {subscribe:()=>()=>{},getSnapshot:()=>settings};
  const session = bundle.createRoomSession({
    window:env.window,document:env.document,storage:null,identity,
    sessions:bundle.createMultiplayerRoomSessionStore({storage:null}),
    settingsModel:subscriptions,
    sitePreferences:{subscribe:()=>()=>{},getSnapshot:()=>({lessMotion:true})},
    runtime:()=>({getSnapshot:()=>({launched,runtimeVariant:'multiplayer'})}),
    replayViewer:()=>false,titleOverlayOpen:()=>false,memberId:()=> 'member123456',
    createSocket:()=>{const socket=new Socket();sockets.push(socket);return socket;},
    relayUrl:()=> 'wss://relay.invalid/socket',
    settings:()=>({touchEnabled:false,touchMovementMode:'joystick',mobileDevice:false,iceServers:[]}),
    setMovementMode(){},decisions,translate:key=>key,notify:message=>notifications.push(message),
    prepareResources:async()=>{},preparationFailed(){},beginManualImport(){},
    launch:async()=>{options.onLaunch?.();},checkGame:async()=>{options.onCheck?.();},operationFailed(){},
    isLaunched:()=>launched,routes:{restore(){},enter(){},settleInvite(){},leave(){}},
    now:()=>clock.now,random:()=>0,setTimeout:clock.setTimeout,clearTimeout:clock.clearTimeout,
  });
  return {session,sockets,notifications,decisions,clock,identity,setLaunched:value=>{launched=value;}};
}
const flush = async () => {for(let index=0;index<30;index++) await Promise.resolve();};
function deferred() {let resolve;const promise=new Promise(done=>{resolve=done;});return {promise,resolve};}
function roomSnapshot(identity, overrides = {}) {
  return {code:'1234',playerCount:2,difficulty:1,phase:'running',inputDelay:0,predictionLimit:8,startSerial:7,
    seats:[{clientId:identity.lobbyClientId('th06mp'),name:'A',loadout:0,ready:true},
      {clientId:'remote123456',name:'B',loadout:0,ready:true}],spectators:[],...overrides};
}
test('actual RoomSession routes valid socket chat into React and UI sends through the same service', async () => {
  const sockets = [],
    decisions = owner.createDecisionStore(),
    clock = new Clock(),
    notifications=[];
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
    notify(message) {notifications.push(message);},
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
      room:true,
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
    assert.match(query('.mp-quick-chat-log').children[1].textContent, /ViewerSpectator.*Thanks for the games/);
    assert.equal(voices.length, 1, 'players see spectator messages without playing spectator voice');
    const quickTrigger=query('#mpRoomChat .room-quick-chat-trigger');
    await act(()=>quickTrigger.dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'ArrowDown',bubbles:true,cancelable:true})));
    await React.act(async()=>vi.waitFor(()=>assert.ok(env.document.querySelector('.room-quick-chat-picker [data-phrase="1"]')),{timeout:1000,interval:1}));
    await React.act(async()=>{
      query('.room-quick-chat-picker [data-phrase="1"]').click();
      await vi.waitFor(()=>assert.deepEqual(socket.sent.at(-1),{type:'quick-chat',phrase:'1',serial:7}),{timeout:1000,interval:1});
    });
    assert.deepEqual(socket.sent.at(-1), {
      type: 'quick-chat',
      phrase: '1',
      serial: 7,
    });
    await act(()=>socket.message({type:'chat-status',registered:true,authorId:'account-1',bannedUntil:0}));
    assert.equal(session.quickChat.getSnapshot().registered,true);
    const input=query('#mpRoomChat input[aria-label="Message"]');
    const setter=Object.getOwnPropertyDescriptor(env.window.HTMLInputElement.prototype,'value').set;
    setter.call(input,'  room session message  ');
    await act(()=>input.dispatchEvent(new env.window.Event('input',{bubbles:true})));
    await act(()=>query('#mpRoomChat button[aria-label="Send"]').click());
    assert.deepEqual(socket.sent.at(-1),{type:'free-chat',text:'room session message',serial:7});
    await act(()=>socket.message({type:'chat-result',action:'free-chat',ok:false,code:'send-rate-limited'}));
    assert.deepEqual(notifications,['chat.sendRateLimited']);

    const free=(messageId,authorId,clientId,seat,name,text)=>({type:'free-chat',room:'th06mp-1234',serial:7,
      messageId,authorId,clientId,seat,spectator:seat===-1,name,text});
    const voiceCount=voices.length;
    await act(()=>socket.message(free(51,'account-2','remote123456',1,'B','from socket')));
    assert.equal(session.quickChat.getSnapshot().entries.at(-1).messageId,51);
    assert.ok([...env.document.querySelectorAll('#mpRoomChat .room-chat-message')].some(entry=>entry.textContent==='BP2from socket'));
    assert.equal(voices.length,voiceCount,'free messages route without quick-chat voice');
    await act(()=>socket.message({type:'chat-history',throughId:52,entries:[free(51,'account-2','remote123456',1,'B','duplicate'),free(52,'account-2','remote123456',1,'B','history')]}));
    assert.deepEqual(session.quickChat.getSnapshot().entries.filter(entry=>entry.messageId).map(entry=>entry.messageId),[51,52]);
    const reportButton=query('#mpRoomChat .room-chat-entry button[aria-label="Message actions"]');
    await act(()=>reportButton.dispatchEvent(new env.window.PointerEvent('pointerdown',{button:0,bubbles:true,cancelable:true})));
    await React.act(async()=>vi.waitFor(()=>assert.ok(env.document.querySelector('[role="menuitem"]')),{timeout:1000,interval:1}));
    const reportItem=[...env.document.querySelectorAll('[role="menuitem"]')].find(item=>item.textContent.includes('Report'));
    assert.ok(reportItem);
    await React.act(async()=>{
      reportItem.click();
      await vi.waitFor(()=>assert.deepEqual(socket.sent.at(-1),{type:'chat-report',messageId:51,reason:'test reason'}),{timeout:1000,interval:1});
    });
    await act(()=>socket.message({type:'chat-result',action:'chat-report',ok:false,code:'report-rate-limited'}));
    assert.deepEqual(notifications,['chat.sendRateLimited','chat.reportRateLimited']);
    await act(()=>socket.message({type:'chat-retract',authorId:'account-2',throughId:51,bannedUntil:0}));
    assert.deepEqual(session.quickChat.getSnapshot().entries.filter(entry=>entry.messageId).map(entry=>entry.messageId),[52]);
    await act(()=>socket.message(free(51,'account-2','remote123456',1,'B','delayed')));
    assert.deepEqual(session.quickChat.getSnapshot().entries.filter(entry=>entry.messageId).map(entry=>entry.messageId),[52]);
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
    assert.equal(session.quickChat.getSnapshot().entries.length, 3);
    assert.deepEqual(session.quickChat.getSnapshot().entries.filter(entry=>entry.messageId).map(entry=>entry.messageId),[52]);
    replay = false;
    launched = false;
    await act(() => session.sync());
    assert.equal(query('.mp-quick-chat').hidden, true);
    assert.equal(query('#gameFrame'), frame);
  } finally {
    globalThis.Audio = previousAudio;
  }
});

test('RoomSession gates listen-together on opt-in and routes a peer offer back through its room socket', async () => {
  const previousOptIn = env.window.localStorage.getItem(TEST_FEATURES_KEY);
  const media = env.window.HTMLMediaElement.prototype;
  const mediaMethods = {play:media.play,pause:media.pause,load:media.load};
  Object.assign(media, {play:()=>Promise.resolve(),pause(){},load(){}});
  env.window.localStorage.setItem(TEST_FEATURES_KEY, '1');
  const room = makeRoomSession(owner);
  try {
    assert.ok(room.session.music, 'test opt-in creates the RoomMusic owner');
    await act(() => room.session.service.enterRoom('th06mp', '1234', true));
    const socket = room.sockets[0];
    await act(() => {
      socket.open();
      socket.message({type:'state',listenTogether:2,room:roomSnapshot(room.identity,{phase:'lobby'})});
    });
    assert.equal(room.session.service.getSnapshot().listenTogetherSupported, true);
    const music = room.session.music.getSnapshot();
    assert.equal(music.enabled, true);
    assert.equal(music.connected, true);
    assert.equal(music.seated, true);
    assert.equal(music.syncState, 'solo');

    const track = {platform:'netease',id:'song-1',name:'Song',artist:'Artist'};
    await act(() => socket.message({type:'listen-together',session:'peer-session',revision:1,track,
      participants:['remote123456'],joined:false,queue:[track],
      queueIndex:0,playback:1,startedAt:null,pausedAt:null,serverTime:room.clock.now}));
    assert.equal(room.session.music.getSnapshot().syncState, 'offered');
    room.session.music.requestSync();
    assert.deepEqual(socket.sent.at(-1), {type:'listen-together',action:'join',session:'peer-session'});
  } finally {
    room.session.dispose();
    room.decisions.dispose();
    Object.assign(media, mediaMethods);
    if (previousOptIn === null) env.window.localStorage.removeItem(TEST_FEATURES_KEY);
    else env.window.localStorage.setItem(TEST_FEATURES_KEY, previousOptIn);
  }
});

test('RoomSession only sends nudges while the non-host player is ready', async () => {
  const room = makeRoomSession(owner);
  const service = room.session.service;
  try {
    await act(() => service.enterRoom('th06mp', '1234', true));
    const socket = room.sockets[0];
    const lobby = roomSnapshot(room.identity,{phase:'lobby'});
    const [local,peer] = lobby.seats;
    socket.open();
    for (const ready of [false,true,false]) {
      socket.message({type:'state',roomActions:1,room:{...lobby,seats:[peer,{...local,ready}]}});
      assert.equal(service.getSnapshot().canNudge,ready);
      const before = socket.sent.length;
      service.nudge();
      assert.deepEqual(socket.sent.slice(before),ready ? [{type:'room-nudge'}] : []);
    }
  } finally {
    room.session.dispose();
    room.decisions.dispose();
  }
});

test('guest-only RoomSession makes no account request or identity subscription', async () => {
  const originalSubscribe=owner.accountSession.subscribe, originalFetch=globalThis.fetch;
  let subscriptions=0, requests=0;
  owner.accountSession.subscribe=(...args)=>{subscriptions++;return originalSubscribe(...args);};
  globalThis.fetch=async()=>{requests++;throw Error('guest-only room attempted an account request');};
  const room=makeRoomSession(owner);
  try {
    assert.equal(owner.accountsEnabled,false);
    await mount(room.session.quickChat,{room:true,dispose:()=>{room.session.dispose();room.decisions.dispose();}});
    assert.equal(subscriptions,0);
    assert.equal(requests,0);
  } finally {
    owner.accountSession.subscribe=originalSubscribe;
    globalThis.fetch=originalFetch;
  }
});

test('account identity change replaces only the room socket and clears old chat state', async () => {
  const authReplies=[], nicknameReplies=[], oldTicket=deferred(), originalSetTimeout=env.window.setTimeout,
    originalClearTimeout=env.window.clearTimeout, timerCallbacks=new Map(), launchCalls=[], checkCalls=[];
  let timerId=0;
  globalThis.__roomAccountRequest=(path)=>{
    if(path==='auth/get-session'){const reply=deferred();authReplies.push(reply);return reply.promise;}
    if(path==='v1/me/room-nickname'){
      nicknameReplies.push(path);
      return nicknameReplies.length===1?oldTicket.promise:Promise.resolve({ticket:'bob-ticket'});
    }
    throw Error(`unexpected account request: ${path}`);
  };
  const room=makeRoomSession(ownerAccounts,{onLaunch:()=>launchCalls.push(true),onCheck:()=>checkCalls.push(true)});
  try {
    assert.equal(ownerAccounts.accountsEnabled,true);
    await mount(room.session.quickChat,{room:true,dispose:()=>{room.session.dispose();room.decisions.dispose();}});
    await flush();
    assert.equal(authReplies.length,1);
    await React.act(async()=>{authReplies[0].resolve({user:{username:'alice',name:'Alice'}});await flush();});
    await act(()=>room.session.service.enterRoom('th06mp','1234',true));
    const aliceSocket=room.sockets[0];
    await act(()=>{
      aliceSocket.open();
      aliceSocket.message({type:'state',room:roomSnapshot(room.identity)});
      aliceSocket.message({type:'chat-status',registered:true,authorId:'alice-id',bannedUntil:0});
    });
    assert.equal(room.session.quickChat.getSnapshot().registered,true);
    const frame=query('#gameFrame');
    await act(()=>{room.session.quickChat.openComposer();room.session.quickChat.setDraft('Alice private draft');});
    env.window.setTimeout=(callback)=>{const id=++timerId;timerCallbacks.set(id,callback);return id;};
    env.window.clearTimeout=id=>timerCallbacks.delete(id);
    await act(()=>room.session.quickChat.sendText());
    const sendTimer=[...timerCallbacks.keys()][0];
    assert.ok(sendTimer,'free-chat send timeout started');
    await act(()=>aliceSocket.message({type:'state',accountChallenge:'alice-challenge',room:roomSnapshot(room.identity)}));
    assert.equal(nicknameReplies.length,1);

    await act(()=>ownerAccounts.notifyAccountChange());
    await flush();
    assert.equal(authReplies.length,2);
    assert.equal(room.session.quickChat.getSnapshot().registered,false);
    assert.equal(room.session.quickChat.getSnapshot().draft,'');
    assert.equal(room.session.quickChat.getSnapshot().pickerOpen,false);
    assert.equal(room.session.quickChat.getSnapshot().sending,false);
    assert.equal(timerCallbacks.has(sendTimer),false,'identity invalidation cancels the old account send timeout');
    assert.equal(env.document.querySelector('#mpRoomChat input[aria-label="Message"]'),null);
    await act(()=>aliceSocket.message({type:'chat-status',registered:true,authorId:'stale-alice-id',bannedUntil:0}));
    assert.equal(room.session.quickChat.getSnapshot().registered,false,'the old socket cannot re-enable Alice chat');

    await React.act(async()=>{authReplies[1].resolve({user:{username:'bob',name:'Bob'}});await flush();});
    assert.equal(room.sockets.length,2,'the account refresh opens one successor socket');
    const bobSocket=room.sockets[1];
    assert.equal(aliceSocket.readyState,3);
    assert.equal(room.session.service.getSnapshot().room.code,'1234');
    assert.equal(room.session.service.getSnapshot().seat,0);
    assert.equal(room.session.service.getSnapshot().ready,true);
    assert.equal(query('#gameFrame'),frame,'room identity refresh leaves the active Runtime surface in place');
    assert.deepEqual(launchCalls,[]);
    assert.deepEqual(checkCalls,[]);
    await act(()=>{
      bobSocket.open();
      bobSocket.message({type:'state',accountChallenge:'bob-challenge',room:roomSnapshot(room.identity)});
    });
    await flush();
    assert.equal(nicknameReplies.length,2);
    assert.ok(bobSocket.sent.some(message=>message.type==='account-nickname'&&message.ticket==='bob-ticket'));
    assert.equal(room.session.quickChat.getSnapshot().registered,false,'a new identity waits for fresh chat-status');
    await act(()=>bobSocket.message({type:'chat-status',registered:true,authorId:'bob-id',bannedUntil:0}));
    assert.equal(room.session.quickChat.getSnapshot().registered,true);
    await React.act(async()=>{oldTicket.resolve({ticket:'alice-ticket'});await flush();});
    assert.equal(bobSocket.sent.some(message=>message.type==='account-nickname'&&message.ticket==='alice-ticket'),false,
      'Alice ticket completion is never sent over Bob socket');
    assert.equal(aliceSocket.sent.some(message=>message.type==='account-nickname'&&message.ticket==='alice-ticket'),false,
      'the superseded socket cannot receive the late ticket');
    assert.equal(query('#gameFrame'),frame);
  } finally {
    env.window.setTimeout=originalSetTimeout;
    env.window.clearTimeout=originalClearTimeout;
    globalThis.__roomAccountRequest=undefined;
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
  const quickTrigger=query('.mp-quick-chat-prompt');
  assert.equal(quickTrigger.disabled,false);
  await act(()=>quickTrigger.click());
  await act(()=>model.setPickerPage(2));
  await React.act(async()=>{
    query('.mp-quick-chat-slots [data-phrase="thanks"]').click();
    await vi.waitFor(()=>assert.deepEqual(calls.at(-1),['send',{type:'quick-chat',phrase:'thanks',serial:7}]),{timeout:1000,interval:1});
  });
  assert.deepEqual(calls.at(-1), ['send', {type: 'quick-chat', phrase: 'thanks', serial: 7}]);
  await act(() => model.receive({
    type: 'quick-chat', room: 'th07mp-5678', serial: 7, seat: -1,
    spectator: true, clientId: 'viewer654321', phrase: 'thanks',
  }));
  assert.match(query('.mp-quick-chat-log').textContent, /Remote viewerSpectator.*谢谢指教/);
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
    const fade = native.records.find(expiryAnimation);
    assert.ok(fade);
    await act(() => model.dispose());
    assert.equal(fade.cancelled, true);
    assert.equal(query('.mp-quick-chat-log').children.length, 0);
    assert.equal(clock.tasks.size, 0);
    await act(() => fade.finish());
    assert.equal(query('.mp-quick-chat-log').children.length, 0);
  } finally {
    native.restore();
  }
});
test('muted rows stay mounted through eviction and reset cancels their active expiry fades', async () => {
  const native = animations();
  try {
    const { model, clock } = fixture();
    await mount(model, {
      strict: true,
    });
    for (let index = 0; index < 55; index++) await act(() => model.receive(message()));
    assert.equal(query('.mp-quick-chat-log').children.length, 50);
    await act(() => model.toggleMember('b'));
    assert.ok([...query('.mp-quick-chat-log').children].every(row=>row.hidden));
    for (let index = 0; index < 55; index++) await act(() => model.receive(message()));
    assert.equal(model.getSnapshot().entries.length, 50);
    assert.equal(query('.mp-quick-chat-log').children.length, 50);
    const mutedRows=[...query('.mp-quick-chat-log').children];
    assert.ok(mutedRows.every(row=>row.hidden));
    await act(() => model.toggleMember('b'));
    assert.equal(query('.mp-quick-chat-log').children.length, 50);
    assert.deepEqual([...query('.mp-quick-chat-log').children],mutedRows);
    assert.ok(mutedRows.every(row=>!row.hidden));
    await act(() => clock.tick(10000));
    const fades = native.records.filter(expiryAnimation);
    assert.equal(fades.length, 50);
    await act(() => model.toggleMember('b'));
    assert.equal(
      fades.some((fade) => fade.cancelled),
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
      fades.every((fade) => fade.cancelled),
      true,
    );
    assert.equal(clock.tasks.size, 0);
  } finally {
    native.restore();
  }
});
test('presentation remount cancels an active fade but preserves another entry and its model timer', async () => {
  const native = animations();
  try {
    const { model, clock } = fixture();
    await mount(model, {
      strict: true,
    });
    await act(() => model.receive(message()));
    await act(() => clock.tick(10000));
    const fade = native.records.find(expiryAnimation);
    await act(() => {
      model.toggleMember('b');
      model.receive(message());
    });
    assert.equal(model.getSnapshot().entries.length, 2);
    await act(() => mounted.root.unmount());
    mounted = null;
    assert.equal(fade.cancelled, true);
    assert.equal(model.getSnapshot().entries.length, 1);
    assert.equal(clock.tasks.size, 1);
    await mount(model, {
      strict: true,
    });
    assert.equal(query('.mp-quick-chat-log').children.length, 1);
    assert.equal(query('.mp-quick-chat-log .room-chat-entry').hidden,true);
    await act(() => model.toggleMember('b'));
    assert.equal(query('.mp-quick-chat-log').children.length, 1);
    assert.equal(query('.mp-quick-chat-log .room-chat-entry').hidden,false);
    await act(() => clock.tick(10000));
    await act(() => native.records.findLast(expiryAnimation).finish());
    assert.equal(query('.mp-quick-chat-log').children.length, 0);
    assert.equal(clock.tasks.size, 0);
  } finally {
    native.restore();
  }
});

test('room settings replace a favorite and gameplay uses the same slot', async () => {
  const {model, calls} = fixture();
  model.update({...context(), inRoom:true});
  await mount(model, {room:true});
  await act(()=>query('#mpRoomChat .room-quick-chat-trigger').dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'ArrowDown',bubbles:true,cancelable:true})));
  await act(()=>query('.room-quick-chat-picker [aria-label="Edit favorites"]').click());
  await act(()=>query('.room-quick-chat-picker [data-phrase="request-life"]').click());
  await act(()=>query('.room-quick-chat-picker [data-phrase="you-first"]').click());
  assert.equal(model.getSnapshot().slots[1], 'you-first');
  await act(()=>model.update({...context(), inRoom:false}));
  await act(()=>model.openComposer());
  assert.ok(query('.mp-quick-chat-slots [data-phrase="you-first"]'));
  await act(()=>model.sendSlot(2));
  assert.equal(calls.at(-1)[1].phrase, 'you-first');
  env.window.localStorage.removeItem('eagler.quick-chat.slots.v1');
});
