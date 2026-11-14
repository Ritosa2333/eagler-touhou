// Real relay sockets and exact synthetic TH13 wire inputs. Native world
// restoration is covered separately by spectator-prefix-replay.py.
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {once} from 'node:events';
import {fileURLToPath} from 'node:url';
import {WebSocket} from 'ws';

const child = spawn(process.execPath, [fileURLToPath(new URL('../src/netplay-relay.mjs', import.meta.url))], {
  stdio: ['ignore', 'pipe', 'pipe'], env: {...process.env,
    EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1', EAGLER_NETPLAY_RELAY_PORT: '0',
    EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK: '1', EAGLER_NETPLAY_PUBLIC_NICKNAMES: '0',
    EAGLER_NETPLAY_STUN_URLS: '', EAGLER_NETPLAY_MAX_ROOMS_PER_IP: '2'},
});
const sockets = [];
let log = '';
child.stdout.on('data', data => {log += data;});
child.stderr.on('data', data => {log += data;});
async function wait(test, label) {
  const end = Date.now() + 5000;
  while (!test()) {
    if (Date.now() >= end) throw Error(`${label} timeout\n${log}`);
    await new Promise(resolve => setTimeout(resolve, 5));
  }
}
function event(count, frame, stage, serial) {
  const bytes = Buffer.alloc(44);
  bytes.set([69, 0x4d, 83, 86, 2, 1, count, 0]);
  bytes.writeBigUInt64LE(9n, 8); bytes.writeUInt32LE(7, 16);
  bytes.writeUInt32LE(frame, 20); bytes.writeUInt32LE(stage, 24); bytes.writeUInt32LE(serial, 28);
  bytes.writeUInt32LE(2, 32); bytes.writeUInt32LE(4, 36); bytes.set([stage, 3, 2, 1], 40);
  return bytes;
}
function input(count, frame) {
  const bytes = Buffer.alloc(24 + count * 12);
  bytes.set([69, 0x4d, 78, 80, 4, 3, count, 0]);
  bytes.writeBigUInt64LE(9n, 8); bytes.writeUInt32LE(frame, 16); bytes.writeUInt32LE(7, 20);
  for (let seat = 0; seat < count; ++seat) bytes.writeUInt16LE((frame + seat) % 32, 24 + seat * 12);
  return bytes;
}
try {
  await wait(() => /listening ws:\/\/127\.0\.0\.1:(\d+)/.test(log), 'relay startup');
  const port = Number(/listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log)[1]);
  async function open(params) {
    const ws = new WebSocket(`ws://127.0.0.1:${port}/?${new URLSearchParams(params)}`);
    const client = {ws, json: [], binary: []}; sockets.push(ws);
    ws.on('message', (data, binary) => binary ? client.binary.push(Buffer.from(data)) : client.json.push(JSON.parse(String(data))));
    await once(ws, 'open'); return client;
  }
  const send = (client, message) => client.ws.send(JSON.stringify(message));
  const roomState = client => client.json.findLast(message => message.room?.seats)?.room;
  for (const count of [2, 3]) {
    const room = `th13mp-${7950 + count}`, players = [], gameplay = [], signals = [];
    for (let seat = 0; seat < count; ++seat) {
      const player = await open({room, lobby: `prefix_${count}_${seat}`, member: `prefix_member_${count}_${seat}`});
      await wait(() => roomState(player), 'lobby'); players.push(player);
      send(player, {type: 'take-seat', seat, loadout: 0});
      if (seat === 0) send(player, {type: 'settings', playerCount: count});
      await wait(() => roomState(players[0])?.seats[seat]?.clientId === `prefix_${count}_${seat}`, 'seat');
    }
    for (const player of players) send(player, {type: 'set-ready', ready: true});
    await wait(() => roomState(players[0])?.seats.slice(0, count).every(seat => seat?.ready), 'ready');
    send(players[0], {type: 'start'});
    await wait(() => players[0].json.some(message => message.type === 'start'), 'start');
    for (let seat = 0; seat < count; ++seat) {
      const params = {room, run: '1', players: String(count), player: String(seat), member: `prefix_member_${count}_${seat}`};
      gameplay.push(await open(params)); signals.push(await open({...params, signal: '1'}));
    }
    for (const signal of signals) send(signal, {type: 'rtc-failed'});
    await wait(() => signals[0].json.some(message => message.type === 'route'), 'relay route');
    const tape = [event(count, 0, 1, 1)];
    for (let frame = 0; frame < 180; ++frame) {
      if (frame === 9) tape.push(event(count, frame, 2, 2));
      tape.push(input(count, frame));
    }
    for (const packet of tape) gameplay[0].ws.send(Buffer.concat([Buffer.from([0xe8]), packet]));
    for (let attempt = 0; attempt < 2; ++attempt) {
      const lobby = await open({room, lobby: `prefix_view_${count}_${attempt}`, member: `prefix_view_member_${count}_${attempt}`});
      await wait(() => roomState(lobby), 'observer lobby'); send(lobby, {type: 'spectate'});
      await wait(() => lobby.json.some(message => message.type === 'spectator-start'), 'observer admission');
      const viewer = await open({room, run: '1', players: String(count), spectator: `prefix_view_${count}_${attempt}`, member: `prefix_view_member_${count}_${attempt}`});
      await wait(() => viewer.binary.length >= tape.length + 1, 'full-prefix bootstrap');
      assert.equal(viewer.binary.length, tape.length + 1);
      assert.deepEqual(viewer.binary.slice(0, -1), tape);
      const bootstrap = viewer.binary.at(-1);
      assert.equal(bootstrap[5], 4); assert.equal(bootstrap.readUInt32LE(20), 180);
      assert.equal(bootstrap.readUInt32LE(24), 2);
      assert.equal(viewer.binary.filter(packet => packet[2] === 78).length, 180);
      viewer.ws.close(); send(lobby, {type: 'leave-spectator'});
    }
    for (const player of gameplay) assert.equal(player.ws.readyState, WebSocket.OPEN);
  }
  console.log('PASS TH13 two/three-seat real Relay late join and rejoin retain frame0 root, stage bookmarks and every exact input packet');
} finally {
  for (const socket of sockets) socket.terminate();
  child.kill(); await once(child, 'exit');
}
