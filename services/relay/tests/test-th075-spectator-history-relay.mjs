import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {fileURLToPath} from 'node:url';

const port = await new Promise(resolve => {
  const server = net.createServer().listen(0, '127.0.0.1', () => {
    const {port} = server.address();
    server.close(() => resolve(port));
  });
});
const child = spawn(process.execPath, [fileURLToPath(new URL('../src/netplay-relay.mjs', import.meta.url))], {
  windowsHide: true, stdio: ['ignore', 'pipe', 'pipe'],
  env: {...process.env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1', EAGLER_NETPLAY_RELAY_PORT: String(port),
    EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK: '1', EAGLER_NETPLAY_PUBLIC_NICKNAMES: '0'},
});
let log = '';
child.stdout.on('data', data => log += data);
child.stderr.on('data', data => log += data);
const sockets = [];
async function waitFor(test, label) {
  const deadline = Date.now() + 8000;
  while (!test()) {
    if (Date.now() >= deadline) throw Error(`${label} timed out\n${log}`);
    await new Promise(resolve => setTimeout(resolve, 5));
  }
}
const roomId = 'th075mp-8192';
async function open(query) {
  const socket = new WebSocket(`ws://127.0.0.1:${port}/?room=${roomId}&${query}`);
  const client = {socket, messages: [], packets: []};
  sockets.push(socket);
  socket.binaryType = 'arraybuffer';
  socket.addEventListener('message', ({data}) => {
    if (typeof data === 'string') {
      const message = JSON.parse(data);
      client.messages.push(message);
      if (message.type === 'error') log += `\n${query}: ${message.error}`;
    }
    else client.packets.push(Buffer.from(data));
  });
  await waitFor(() => socket.readyState !== WebSocket.CONNECTING, 'socket');
  assert.equal(socket.readyState, WebSocket.OPEN);
  return client;
}
const send = (client, message) => client.socket.send(JSON.stringify(message));
const upload = (client, packet) => client.socket.send(Buffer.concat([Buffer.of(0xe8), packet]));
const snapshot = client => client.messages.findLast(message => message.room)?.room;
const lobby = id => open(`lobby=th075_${id}&member=th075_member_${id}`);
const runtime = (role, id) => open(`run=1&players=2&${role}&member=th075_member_${id}`);
const control = type => Buffer.of(69, 80, 84, 83, 1, type);
const timing = Buffer.alloc(40);
timing.set([69, 80, 84, 77, 1, 1, 2]);
timing.writeUInt32LE(1, 8);
timing.writeBigUInt64LE(10n, 16);
timing.writeUInt32LE(10000, 24);
timing.writeUInt32LE(96, 36);
function frame(index) {
  const packet = Buffer.alloc(48);
  packet.set([69, 80, 78, 80, 4, 3, 2]);
  packet.writeBigUInt64LE(10n, 8);
  packet.writeUInt32LE(index, 16);
  packet.writeUInt32LE(123, 20);
  return packet;
}

try {
  await waitFor(() => log.includes('listening'), 'relay startup');
  const host = await lobby('host'), guest = await lobby('guest');
  send(host, {type: 'take-seat', seat: 0, loadout: 0, movementMode: 'joystick'});
  send(guest, {type: 'take-seat', seat: 1, loadout: 18, movementMode: 'joystick'});
  await waitFor(() => snapshot(host)?.seats.slice(0, 2).every(Boolean), 'player seats');
  for (const client of [host, guest]) send(client, {type: 'set-ready', ready: true, movementMode: 'joystick'});
  await waitFor(() => snapshot(host)?.seats.slice(0, 2).every(seat => seat?.ready), 'player ready');
  const opening = await lobby('opening'), early = await lobby('early');
  send(opening, {type: 'spectate'});
  send(early, {type: 'spectate'});
  await waitFor(() => snapshot(host)?.spectatorCount === 2, 'opening viewers registered');
  send(host, {type: 'start', adonisMode: 1, inputDelay: 2, inputDelayAuto: false});
  await waitFor(() => host.messages.some(message => message.type === 'start'), 'room start');
  assert.equal(snapshot(host).spectatorCount, 2);
  const hostRuntime = await runtime('player=0', 'host');
  await runtime('player=1', 'guest');

  const openingViewer = await runtime('spectator=th075_opening', 'opening');
  const waiting = await lobby('waiting');
  send(waiting, {type: 'spectate'});
  await waitFor(() => waiting.messages.some(message => message.type === 'spectator-waiting'), 'waiting without a stream');
  upload(hostRuntime, control(1));
  upload(hostRuntime, timing);
  await waitFor(() => openingViewer.packets[0]?.equals(timing), 'live opening timing');
  const earlyViewer = await runtime('spectator=th075_early', 'early');
  await waitFor(() => earlyViewer.packets[0]?.equals(timing), 'buffered opening timing');
  for (let i = 0; i < 160; ++i) upload(hostRuntime, frame(i));
  await waitFor(() => waiting.messages.some(message => message.type === 'spectator-start'), 'admission after the first input');
  const firstViewer = await runtime('spectator=th075_waiting', 'waiting');
  await waitFor(() => firstViewer.packets.length >= 160, 'queued viewer history');
  assert.ok(firstViewer.packets[0]?.equals(timing), 'late history starts with timing');
  await waitFor(() => firstViewer.packets.length === 161, 'complete queued viewer history');

  const late = await lobby('late');
  send(late, {type: 'spectate'});
  await waitFor(() => late.messages.some(message => message.type === 'spectator-start'), 'mid-match admission');
  const lateViewer = await runtime('spectator=th075_late', 'late');
  for (let i = 160; i < 200; ++i) upload(hostRuntime, frame(i));
  upload(hostRuntime, control(2));
  const viewers = [openingViewer, earlyViewer, firstViewer, lateViewer];
  await waitFor(() => viewers.every(viewer => viewer.packets.length === 202), 'history to live handoff');
  const expected = [timing, ...Array.from({length: 200}, (_, i) => frame(i)), control(2)];
  for (const viewer of viewers) assert.deepEqual(viewer.packets, expected);
  const afterEnd = await lobby('after_end');
  send(afterEnd, {type: 'spectate'});
  await waitFor(() => afterEnd.messages.some(message => message.type === 'spectator-waiting'), 'admission closed at the end');
} finally {
  for (const socket of sockets) socket.close();
  child.kill();
}
