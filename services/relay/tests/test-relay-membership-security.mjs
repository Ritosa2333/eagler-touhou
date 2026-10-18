// Repository-local WebSocket integration: public IDs never authorize replacing
// a lobby session or claiming an admitted player/spectator transport.
import assert from 'node:assert/strict';
import { spawn } from 'node:child_process';
import { once } from 'node:events';
import { WebSocket } from 'ws';

const relay = spawn(process.execPath, ['src/netplay-relay.mjs'], {
  windowsHide: true, stdio: ['ignore', 'pipe', 'pipe'],
  env: { ...process.env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1',
    EAGLER_NETPLAY_RELAY_PORT: '0', EAGLER_NETPLAY_STUN_URLS: '' },
});
const sockets = [];
let log = '';
const port = await new Promise((resolve, reject) => {
  const timer = setTimeout(() => reject(new Error(`relay startup timeout: ${log}`)), 5000);
  relay.stderr.on('data', data => { log += data; });
  relay.stdout.on('data', data => {
    log += data;
    const match = /listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log);
    if (match) { clearTimeout(timer); resolve(Number(match[1])); }
  });
  relay.once('exit', () => { clearTimeout(timer); reject(new Error(`relay exited: ${log}`)); });
});

function connect(params) {
  const socket = new WebSocket(`ws://127.0.0.1:${port}/?${new URLSearchParams(params)}`);
  sockets.push(socket);
  const messages = [], waiters = [];
  socket.on('message', (data, binary) => {
    if (binary) return;
    const message = JSON.parse(String(data)); messages.push(message);
    for (const waiter of [...waiters]) if (waiter.test(message)) waiter.resolve(message);
  });
  return { socket, messages, wait(test) {
    const found = messages.find(test); if (found) return Promise.resolve(found);
    return new Promise((resolve, reject) => {
      const timer = setTimeout(() => reject(new Error(`relay message timeout: ${log}`)), 5000);
      const waiter = { test, resolve(value) { clearTimeout(timer); waiters.splice(waiters.indexOf(waiter), 1); resolve(value); } };
      waiters.push(waiter);
    });
  } };
}
const room = 'th09mp-6431';
const hostId = 'public_host_id', guestId = 'public_guest_id', viewerId = 'public_viewer_id';
const hostMember = 'private_host_member', guestMember = 'private_guest_member', viewerMember = 'private_viewer_member';
async function lobby(clientId, member) {
  const client = connect({ room, lobby: clientId, member });
  await client.wait(message => message.type === 'state');
  return client;
}
async function reject(params, code = 1008) {
  const client = connect(params);
  const [actual] = await once(client.socket, 'close');
  assert.equal(actual, code, JSON.stringify(params));
}
async function sendAndWait(client, message, test) {
  client.messages.length = 0;
  const result = client.wait(test);
  client.socket.send(JSON.stringify(message));
  return result;
}
try {
  const host = await lobby(hostId, hostMember), guest = await lobby(guestId, guestMember), viewer = await lobby(viewerId, viewerMember);
  await sendAndWait(host, { type: 'take-seat', seat: 0, loadout: 0, name: 'Host' }, m => m.room?.seats[0]?.name === 'Host');
  await sendAndWait(guest, { type: 'take-seat', seat: 1, loadout: 1 }, m => m.room?.seats[1]?.clientId === guestId);
  await sendAndWait(viewer, { type: 'spectate', name: 'Viewer' }, m => m.room?.spectators?.some(s => s.clientId === viewerId));
  for (const member of ['', hostId, 'private_attacker_member']) await reject({ room, lobby: hostId, member, intent: 'join' });
  assert.equal(host.socket.readyState, WebSocket.OPEN, 'spoofing must not close the host');
  await reject({ room, lobby: 'another_host_tab', member: hostMember }, 4009);
  const replaced = once(host.socket, 'close');
  const resumed = await lobby(hostId, hostMember);
  assert.equal((await replaced)[0], 4008, 'only the same member may replace its exact session');
  assert.equal(resumed.messages[0].room.seats[0].name, 'Host', 'legitimate reconnect preserves seat and state');
  await sendAndWait(resumed, { type: 'set-ready', ready: true }, m => m.room?.seats[0]?.ready);
  await sendAndWait(guest, { type: 'set-ready', ready: true }, m => m.room?.seats.slice(0, 2).every(s => s?.ready));
  const started = await sendAndWait(resumed, { type: 'start' }, m => m.type === 'start');
  const base = { room, run: String(started.serial), players: '2' };
  for (const role of [{ spectator: viewerId }, { player: '0' }, { player: '0', signal: '1' }]) {
    for (const member of ['', hostId, guestMember]) await reject({ ...base, ...role, member });
  }
  await reject({ ...base, run: '999', player: '0', member: hostMember });
  const player = connect({ ...base, player: '0', member: hostMember });
  await once(player.socket, 'open');
  const signal = connect({ ...base, player: '0', signal: '1', member: hostMember });
  await signal.wait(m => m.type === 'peers');
  await reject({ ...base, player: '0', signal: '1', member: guestMember });
  const spectator = connect({ ...base, spectator: viewerId, member: viewerMember });
  await once(spectator.socket, 'open');
  const frame = Buffer.alloc(47); frame.set([0xe8, 84, 57, 83, 80, 1, 3, 2, 0]);
  const received = once(spectator.socket, 'message'); player.socket.send(frame);
  assert.deepEqual((await received)[0], frame.subarray(1), 'rejected claims must not consume legitimate spectator admission');
  await reject({ ...base, spectator: viewerId, member: viewerMember }, 1008);
  assert.equal(signal.socket.readyState, WebSocket.OPEN, 'unauthorized signaling must not replace an admitted connection');
  for (const client of [resumed, guest, viewer]) {
    assert.ok(!JSON.stringify(client.messages).includes(hostMember));
    assert.ok(!JSON.stringify(client.messages).includes(viewerMember), 'membership credentials must never appear in room broadcasts');
  }
  // Host removals bind both the tab and the browser membership for this room.
  const kickRoom = 'th09mp-7431';
  const moderator = connect({ room: kickRoom, lobby: 'kick_host_client', member: 'kick_host_member', intent: 'create' });
  await moderator.wait((m) => m.type === 'state');
  await sendAndWait(moderator, { type: 'take-seat', seat: 0, loadout: 0 }, (m) => m.room?.seats[0]);
  const removed = connect({ room: kickRoom, lobby: 'kick_guest_client', member: 'kick_guest_member', intent: 'join' });
  await removed.wait((m) => m.type === 'state');
  await sendAndWait(removed, { type: 'take-seat', seat: 1, loadout: 1 }, (m) => m.room?.seats[1]);
  const kicked = once(removed.socket, 'close');
  await sendAndWait(moderator, { type: 'remove-player', seat: 1, clientId: 'kick_guest_client' },
    (m) => m.type === 'state' && !m.room?.seats[1]);
  assert.equal((await kicked)[0], 4010);
  for (const [lobbyId, member] of [
    ['kick_guest_client', 'kick_guest_member'],
    ['kick_new_tab_id', 'kick_guest_member'],
    ['kick_guest_client', 'kick_new_member'],
  ]) await reject({ room: kickRoom, lobby: lobbyId, member, intent: 'join' }, 4010);
  const replacement = connect({ room: kickRoom, lobby: 'other_guest_client', member: 'other_guest_member', intent: 'join' });
  await replacement.wait((m) => m.type === 'state');
  await sendAndWait(replacement, { type: 'spectate' }, (m) => m.room?.spectators?.some((s) => s.clientId === 'other_guest_client'));
  const spectatorKicked = once(replacement.socket, 'close');
  await sendAndWait(moderator, { type: 'remove-spectator', clientId: 'other_guest_client' },
    (m) => m.type === 'state' && !m.room?.spectators?.length);
  assert.equal((await spectatorKicked)[0], 4010);
  await reject({ room: kickRoom, lobby: 'other_new_tab_id', member: 'other_guest_member', intent: 'join' }, 4010);
  const elsewhere = connect({ room, lobby: 'kick_guest_client', member: 'kick_guest_member', intent: 'join' });
  await elsewhere.wait((m) => m.type === 'state');
  assert.equal(moderator.socket.readyState, WebSocket.OPEN, 'shared IPs are not banned');
  console.log('PASS lobby owner binding, exact-session reconnect, private run admission and spectator claim protection');
} finally {
  for (const socket of sockets) socket.terminate();
  const exited = once(relay, 'exit'); relay.kill(); await exited;
}
