import test from 'node:test';
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {setTimeout as delay} from 'node:timers/promises';
import {WebSocket} from 'ws';

const type = 'listen-together';
const first = {platform: 'netease', id: '123'};
const second = {platform: 'qq', id: '0039MnYb0qxYhV'};

async function setup(t, enabled) {
  const child = spawn(process.execPath, ['src/netplay-relay.mjs'], {
    cwd: new URL('../', import.meta.url), windowsHide: true, stdio: ['ignore', 'pipe', 'pipe'],
    env: {...process.env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1', EAGLER_NETPLAY_RELAY_PORT: '0',
      EAGLER_NETPLAY_LISTEN_TOGETHER: enabled ? '1' : ''},
  });
  const sockets = [];
  t.after(() => {
    for (const socket of sockets) socket.terminate();
    child.kill();
  });
  let log = '';
  child.stdout.on('data', chunk => { log += chunk; });
  child.stderr.on('data', chunk => { log += chunk; });
  async function wait(find, label) {
    for (let end = Date.now() + 5000; Date.now() < end;) {
      const value = find();
      if (value) return value;
      await delay(10);
    }
    assert.fail(label + '\n' + log);
  }
  const port = (await wait(() => /listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log), 'Relay startup'))[1];

  async function open(id) {
    const room = 'th09mp-listen-test';
    const socket = new WebSocket('ws://127.0.0.1:' + port + '/?' +
      new URLSearchParams({room, lobby: id, member: 'private_' + id}));
    sockets.push(socket);
    const messages = [];
    socket.on('message', data => { messages.push(JSON.parse(String(data))); });
    socket.on('error', () => {});
    const peer = {
      messages,
      socket,
      wait: (find, label) => wait(() => messages.find(find), label),
      send: message => socket.send(JSON.stringify(message)),
      music: (action, fields) => socket.send(JSON.stringify({type, action, ...fields})),
    };
    const initial = await peer.wait(message => message.type === 'state', 'initial state');
    assert.equal(initial.listenTogether, enabled ? 2 : undefined);
    return peer;
  }

  const host = await open('host_lobby');
  const guest = await open('guest_lobby');
  const viewer = await open('viewer_lobby');
  host.send({type: 'take-seat', seat: 0, loadout: 0});
  guest.send({type: 'take-seat', seat: 1, loadout: 0});
  viewer.send({type: 'spectate'});
  await host.wait(message => message.room?.seats?.[1] && message.room.spectatorCount === 1, 'roles');
  return {host, guest, viewer, open};
}

test('disabled Relay does not advertise or emit music state', {timeout: 15000}, async t => {
  const {host, guest, viewer} = await setup(t, false);
  host.music('invite', {track: first});
  await host.wait(message => message.type === 'error' && message.error === 'unknown lobby message', 'legacy dispatch');
  host.send({type: 'set-ready', ready: true});
  await guest.wait(message => message.room?.seats?.[0]?.ready, 'ordinary lobby dispatch');
  for (const peer of [host, guest, viewer])
    assert.equal(peer.messages.some(message => message.type === type), false);
});

test('players and viewers opt into playback independently, and only players can control it', {timeout: 15000}, async t => {
  const {host, guest, viewer, open} = await setup(t, true);
  host.send({type: 'set-ready', ready: true});
  await guest.wait(message => message.room?.seats?.[0]?.ready, 'ready state');
  assert.equal(host.messages.filter(message => message.type === type).at(-1).session, null);
  host.music('invite', {track: {...first, url: 'https://ignored.example/'}});
  const offer = await guest.wait(message => message.type === type && message.session, 'invite');
  assert.deepEqual(offer.track, first);
  assert.ok(Number.isInteger(offer.startedAt));
  assert.equal(offer.joined, false);
  assert.deepEqual(offer.participants, ['host_lobby']);

  viewer.music('join', {session: offer.session});
  const listening = await viewer.wait(message => message.session === offer.session && message.joined, 'viewer opts in');
  assert.deepEqual(listening.participants, ['host_lobby']);
  viewer.music('select', {session: offer.session, revision: listening.revision, track: second});
  viewer.music('sync');
  await viewer.wait(message => message.session === offer.session && message.joined &&
    viewer.messages.filter(item => item.session === offer.session && item.joined).length >= 2, 'viewer sync');
  assert.equal(viewer.messages.some(message => message.track?.id === second.id), false);

  guest.music('join', {session: offer.session});
  const started = await guest.wait(message => message.session === offer.session && message.joined, 'player opts in');
  assert.deepEqual(started.participants, ['host_lobby', 'guest_lobby']);
  guest.music('select', {session: offer.session, revision: started.revision, track: second});
  const changed = await host.wait(message => message.type === type && message.track?.id === second.id, 'track change');
  assert.ok(changed.startedAt >= started.startedAt);
  assert.ok(changed.serverTime >= changed.startedAt);
  assert.equal(host.messages.filter(message => message.type === 'state').at(-1).room.seats[0].ready, true);

  const resumed = await open('guest_lobby');
  await resumed.wait(message => message.type === type && message.track?.id === second.id, 'reconnect snapshot');
  resumed.music('leave', {session: offer.session});
  await resumed.wait(message => message.session === offer.session && message.joined === false, 'leaving client stops');
  viewer.music('sync');
  const continuing = await viewer.wait(message => message.session === offer.session && message.joined &&
    message.track?.id === second.id && message.participants.length === 1, 'other listeners continue');
  assert.deepEqual(continuing.participants, ['host_lobby']);
});

test('stale concurrent changes are rejected and rate limits still allow an independent exit', {timeout: 15000}, async t => {
  const {host, guest, open} = await setup(t, true);
  host.music('invite', {track: first});
  const offer = await guest.wait(m => m.type === type && m.session, 'offer');
  guest.music('join', {session: offer.session});
  const state = await guest.wait(m => m.session === offer.session && m.joined, 'consent');
  const select = {session: state.session, revision: state.revision, track: second};
  host.music('select', select);
  const change = await guest.wait(m => m.type === type && m.revision > state.revision, 'first writer');
  guest.music('select', {...select, track: first});
  host.music('select', select);
  guest.music('sync');
  await guest.wait(m => m.type === type && m.revision === change.revision &&
    guest.messages.filter(x => x.type === type && x.revision === change.revision).length >= 3, 'stale writer snapshot');
  assert.ok(guest.messages.filter(m => m.revision === change.revision).every(m => m.track.id === second.id));
  for (let i = 0; i < 12; i++) host.music('sync');
  await host.wait(m => m.type === type && m.error === 'rate-limited', 'bounded controls');
  host.music('leave', {session: state.session});
  await host.wait(m => m.session === state.session && m.joined === false, 'exit during rate limit');
  const reconnect = await open('host_lobby');
  const snapshot = await reconnect.wait(m => m.type === type && m.session === state.session, 'reconnect gets current offer');
  assert.equal(snapshot.joined, false);
  assert.deepEqual(snapshot.participants, ['guest_lobby']);
});

test('three-player membership is independent and stale sockets cannot control music', async () => {
  const {createListenTogether} = await import('../src/listen-together.mjs');
  const sockets = new Map(['a','b','c'].map(id => [id,{readyState:1, messages:[]} ]));
  const room = {lobby:{seats:['a','b','c'].map(clientId => ({clientId}))}, lobbyClients:sockets};
  const music = createListenTogether({send:(socket,data)=>socket?.messages.push(JSON.parse(data)),now:()=>1000});
  const send = (id,action,extra={}) => music.handle(room,id,sockets.get(id),{action,...extra});
  const current = id => sockets.get(id).messages.at(-1);
  send('a','invite',{track:{platform:'qq',id:'https://evil.example'}});
  assert.equal(current('b'), undefined);
  send('a','invite',{track:first});
  const offer=current('b');
  music.handle(room,'b',{readyState:1},{action:'join',session:offer.session});
  music.sync(room,'b');
  assert.equal(current('b').joined,false);
  send('b','join',{session:offer.session});
  send('c','join',{session:offer.session});
  send('a','leave',{session:offer.session});
  assert.deepEqual(current('a').participants,['b','c']);
  assert.equal(current('a').joined,false);
  room.lobby.seats[1]=null;
  music.sync(room,'b');
  assert.deepEqual(current('b').participants,['c']);
  assert.equal(current('b').joined,true);
  send('b','select',{session:offer.session,revision:offer.revision,track:second});
  music.sync(room,'c');
  assert.deepEqual(current('c').track,first);
  music.close(room);
  music.sync(room,'c');
  assert.equal(current('c').session,null);
});

test('shared transport is versioned, freezes the room clock and excludes nonparticipants', async () => {
  const {createListenTogether}=await import('../src/listen-together.mjs');
  let time=1000;
  const sockets=new Map(['a','b','c'].map(id=>[id,{readyState:1,messages:[]}]));
  const room={lobby:{seats:['a','b','c'].map(clientId=>({clientId}))},lobbyClients:sockets};
  const music=createListenTogether({send:(socket,data)=>socket.messages.push(JSON.parse(data)),now:()=>time});
  const send=(id,action,extra={})=>music.handle(room,id,sockets.get(id),{action,...extra});
  const current=()=>sockets.get('a').messages.at(-1);
  send('a','invite',{track:first});send('b','join',{session:current().session});
  const start=current();time=3000;
  send('c','pause',{session:start.session,revision:start.revision});assert.equal(current(),start);
  send('a','pause',{session:start.session,revision:start.revision});const paused=current();assert.equal(paused.pausedAt,3000);
  time=8000;send('b','resume',{session:start.session,revision:start.revision});assert.equal(current(),paused);
  send('b','resume',{session:start.session,revision:paused.revision});const resumed=current();
  assert.equal(resumed.pausedAt,null);assert.equal(resumed.serverTime-resumed.startedAt,2000);
  send('a','pause',{session:start.session,revision:resumed.revision});const again=current();
  send('b','select',{session:start.session,revision:again.revision,track:second});assert.equal(current().pausedAt,null);
});

function queueFixture() {
  let time=10000;
  const sockets=new Map(['a','b','c'].map(id=>[id,{readyState:1,messages:[]}]));
  const room={lobby:{seats:['a','b','c'].map(clientId=>({clientId}))},lobbyClients:sockets};
  return {sockets,room,now:()=>time,tick:ms=>{time+=ms;}};
}
test('seek, repeat modes and end reports share one authoritative queue',async()=>{
  const {createListenTogether}=await import('../src/listen-together.mjs');const f=queueFixture();
  const music=createListenTogether({send:(s,m)=>s.messages.push(JSON.parse(m)),now:f.now,random:()=>.2});
  const last=()=>f.sockets.get('a').messages.at(-1);
  const send=(action,fields={},id='a')=>{f.tick(1100);const state=last();music.handle(f.room,id,f.sockets.get(id),{action,session:state?.session,revision:state?.revision,...fields});};
  const third={platform:'netease',id:'third'};
  send('invite',{track:first,queue:[first,second,third]});send('join',{},'b');
  send('seek',{positionMs:25000});assert.equal(last().serverTime-last().startedAt,25000);
  send('pause');send('seek',{positionMs:12000});assert.equal(last().pausedAt-last().startedAt,12000);
  send('resume');assert.equal(last().serverTime-last().startedAt,12000);
  send('mode',{repeat:'one',shuffle:false});const old=last();
  send('ended',{playback:old.playback,durationMs:120000});assert.deepEqual(last().track,first);assert.ok(last().playback>old.playback);
  const once=last();send('ended',{playback:old.playback,durationMs:120000},'b');assert.equal(last(),once);
  send('next');assert.deepEqual(last().track,second);send('next');assert.deepEqual(last().track,third);
  send('mode',{repeat:'all',shuffle:false});send('next');assert.deepEqual(last().track,first);
  send('mode',{repeat:'off',shuffle:false});send('select',{track:third});
  send('ended',{playback:last().playback,durationMs:120000});assert.equal(last().ended,true);
  send('resume');assert.equal(last().ended,false);assert.equal(last().startedAt,last().serverTime);
  send('mode',{repeat:'all',shuffle:true});const visited=[last().track.id];
  send('next');visited.push(last().track.id);send('next');visited.push(last().track.id);
  assert.equal(new Set(visited).size,3);
});
test('queue edits are bounded and observers cannot seek or change modes',async()=>{
  const {createListenTogether}=await import('../src/listen-together.mjs');const f=queueFixture();
  const music=createListenTogether({send:(s,m)=>s.messages.push(JSON.parse(m)),now:f.now});
  const last=()=>f.sockets.get('a').messages.at(-1);
  const send=(action,fields={},id='a')=>{f.tick(1100);const state=last();music.handle(f.room,id,f.sockets.get(id),{action,session:state?.session,revision:state?.revision,...fields});};
  send('invite',{track:first,queue:Array.from({length:101},()=>first)});assert.equal(last(),undefined);
  send('invite',{track:first});send('join',{},'b');send('enqueue',{track:{...second,name:'标题'.repeat(100),artist:'artist',url:'https://untrusted.example'}});
  assert.equal(last().queue.length,2);assert.equal(last().queue[1].name.length,64);assert.equal(last().queue[1].url,undefined);
  send('enqueue',{track:second});assert.equal(last().queue.length,2);
  send('remove',{index:0});assert.equal(last().queue.length,2);
  send('remove',{index:1});assert.equal(last().queue.length,1);
  const state=last();send('seek',{positionMs:NaN});send('seek',{positionMs:86400001});assert.equal(last(),state);
  send('mode',{repeat:'all',shuffle:true},'c');assert.equal(last(),state);
  f.room.lobby.seats[2]=null;send('seek',{positionMs:2000},'c');assert.equal(last(),state);
});

test('consent starts at the invitation position and all joined players can edit the shared queue', async () => {
  const {createListenTogether} = await import('../src/listen-together.mjs');
  const f = queueFixture();
  const music = createListenTogether({send: (socket, payload) => socket.messages.push(JSON.parse(payload)), now: f.now});
  const last = () => f.sockets.get('a').messages.at(-1);
  const send = (id, action, fields = {}) => music.handle(f.room, id, f.sockets.get(id), {
    action, session: last()?.session, revision: last()?.revision, ...fields,
  });
  send('a', 'invite', {track: second, queue: [first, second], positionMs: 42500, paused: true, repeat: 'all'});
  const offer = last();
  f.tick(15000);
  send('b', 'select', {track: first});
  send('b', 'remove', {index: 0});
  assert.equal(last(), offer);
  send('b', 'join');
  const started = last();
  assert.equal(started.pausedAt - started.startedAt, 42500);
  assert.deepEqual(started.queue.map(track => track.id), [first.id, second.id]);
  assert.equal(started.repeat, 'all');
  send('c', 'select', {track: first});
  assert.equal(last(), started);
  send('b', 'select', {track: first});
  assert.deepEqual(last().track, first);
  assert.deepEqual(last().participants, ['a', 'b']);
});
