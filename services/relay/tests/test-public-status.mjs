import test from 'node:test';
import assert from 'node:assert/strict';
import {once} from 'node:events';
import {createServer} from 'node:net';
import {spawn} from 'node:child_process';
import {mkdtemp, rm, symlink} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {setTimeout as delay} from 'node:timers/promises';
import {WebSocket} from 'ws';
import {PRODUCT_IDS, isMultiplayerProductId} from '../../../common/contracts/product-catalog.mjs';
import {roomStatus, startRoomStatus} from '../src/public-status.mjs';
import {packageRelay} from '../scripts/package.mjs';

async function freePort() {
  const server = createServer().listen(0, '127.0.0.1');
  await once(server, 'listening');
  const port = server.address().port;
  await new Promise(resolve => server.close(resolve));
  return port;
}

test('room counts cover the current catalog and exclude disconnected visitors and private details', () => {
  const rooms = new Map(), connected = {readyState: 1}, disconnected = {readyState: 3};
  for (const product of PRODUCT_IDS.filter(isMultiplayerProductId)) {
    rooms.set(product + '-private-room', {
      lobby: {phase: 'lobby', seats: [{clientId: 'host', name: 'private-name'}, {clientId: 'gone'}],
        spectators: new Map([['viewer', {}], ['gone-viewer', {}]])},
      lobbyClients: new Map([['host', connected], ['gone', disconnected], ['viewer', connected], ['gone-viewer', disconnected]]),
    });
  }
  rooms.set('unknownmp-secret', rooms.values().next().value);
  rooms.set('th12mp-unseated', {lobby: {seats: [], spectators: new Map()}, lobbyClients: new Map([['visitor', connected]])});
  rooms.set('th12mp-retired', {lobby: {seats: [{clientId: 'gone'}]}, lobbyClients: new Map([['gone', disconnected]])});
  rooms.set('th12mp-spectator-only', {lobby: {phase: 'running', seats: [], spectators: new Map([['viewer', {}]])},
    lobbyClients: new Map([['viewer', connected]])});
  const result = roomStatus(rooms, 0);
  assert.equal(result.checkedAt, '1970-01-01T00:00:00.000Z');
  for (const product of PRODUCT_IDS.filter(isMultiplayerProductId)) {
    assert.deepEqual(result.products[product], product === 'th12mp'
      ? {rooms: 2, waiting: 1, playing: 1, players: 1, spectators: 2}
      : {rooms: 1, waiting: 1, playing: 0, players: 1, spectators: 1}, product);
  }
  assert.equal(Object.keys(result.products).length, PRODUCT_IDS.filter(isMultiplayerProductId).length);
  assert.doesNotMatch(JSON.stringify(result), /private|visitor|host|gone/);
});

test('status HTTP is loopback-only, uncached and read-only', async t => {
  for (const port of [0, -1, 65536, NaN, 1.5]) assert.throws(() => startRoomStatus(new Map(), port), /Invalid/);
  const server = startRoomStatus(new Map(), await freePort());
  t.after(() => new Promise(resolve => server.close(resolve)));
  await once(server, 'listening');
  assert.equal(server.address().address, '127.0.0.1');
  const url = `http://127.0.0.1:${server.address().port}`;
  const response = await fetch(url + '/status');
  assert.equal(response.headers.get('cache-control'), 'no-store');
  assert.match(response.headers.get('content-type'), /application\/json/);
  assert.deepEqual((await response.json()).products, {});
  for (const [path, method] of [['/', 'GET'], ['/status?secret=1', 'GET'], ['/status', 'POST']]) {
    assert.equal((await fetch(url + path, {method})).status, 404);
  }
});

test('Relay fails startup for invalid or occupied status ports', {timeout: 10000}, async t => {
  const occupied = createServer().listen(0, '127.0.0.1');
  await once(occupied, 'listening');
  t.after(() => new Promise(resolve => occupied.close(resolve)));
  for (const port of ['invalid', String(occupied.address().port)]) {
    const env = Object.fromEntries(Object.entries(process.env).filter(([key]) => !/^(EAGLER_NETPLAY_|TH07_)/.test(key)));
    const child = spawn(process.execPath, ['src/netplay-relay.mjs'], {
      cwd: new URL('../', import.meta.url), windowsHide: true, stdio: ['ignore', 'pipe', 'pipe'],
      env: {...env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1', EAGLER_NETPLAY_RELAY_PORT: '0', EAGLER_NETPLAY_STATUS_PORT: port},
    });
    let log = '';
    child.stderr.on('data', bytes => log += bytes);
    child.stdout.resume();
    t.after(() => {if (child.exitCode === null) child.kill();});
    const [code] = await once(child, 'exit');
    assert.equal(code, 1);
    assert.match(log, port === 'invalid' ? /Invalid relay status port/ : /Room status failed: EADDRINUSE/);
  }
});

for (const packaged of [false, true]) test(`${packaged ? 'packaged' : 'source'} Relay publishes room lifecycle without deployment injection`, {timeout: 30000}, async () => {
  const source = resolve(import.meta.dirname, '..');
  const work = await mkdtemp(resolve(tmpdir(), 'eagler-room-status-'));
  const sockets = [];
  let child, log = '';
  try {
    let cwd = source, entry = 'src/netplay-relay.mjs';
    if (packaged) {
      cwd = resolve(work, 'package');
      await packageRelay(cwd);
      await symlink(resolve(source, 'node_modules'), resolve(cwd, 'node_modules'), process.platform === 'win32' ? 'junction' : 'dir');
      entry = 'server/netplay-relay.mjs';
    }
    const statusPort = await freePort();
    const env = Object.fromEntries(Object.entries(process.env).filter(([key]) => !/^(EAGLER_NETPLAY_|TH07_)/.test(key)));
    child = spawn(process.execPath, [entry], {cwd, windowsHide: true, stdio: ['ignore', 'pipe', 'pipe'], env: {
      ...env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1', EAGLER_NETPLAY_RELAY_PORT: '0',
      EAGLER_NETPLAY_STATUS_PORT: String(statusPort), EAGLER_NETPLAY_PUBLIC_NICKNAMES: '0', EAGLER_NETPLAY_STUN_URLS: '',
    }});
    child.stdout.on('data', bytes => log += bytes);
    child.stderr.on('data', bytes => log += bytes);
    async function wait(condition) {
      const deadline = Date.now() + 6000;
      while (!await condition()) {
        if (child.exitCode !== null || Date.now() > deadline) throw Error('Relay wait failed: ' + log);
        await delay(10);
      }
    }
    await wait(() => /listening ws:\/\/127\.0\.0\.1:\d+/.test(log));
    const relayPort = /listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log)[1];
    const status = async () => (await fetch(`http://127.0.0.1:${statusPort}/status`)).json();
    assert.deepEqual((await status()).products, {});
    async function connect(game, index) {
      const socket = new WebSocket(`ws://127.0.0.1:${relayPort}/?` + new URLSearchParams({
        room: game + '-status-test', lobby: 'client_' + game + index, member: 'member_' + game + index,
      }));
      sockets.push(socket);
      const messages = [];
      socket.on('message', bytes => messages.push(JSON.parse(String(bytes))));
      socket.on('error', () => {});
      await wait(() => messages.some(m => m.type === 'state'));
      return {socket, messages};
    }
    for (const game of ['th12mp', 'th15mp']) {
      const host = await connect(game, 0), guest = await connect(game, 1), viewer = await connect(game, 2);
      assert.equal((await status()).products[game], undefined, 'unseated visitors are excluded');
      for (const [peer, seat] of [[host, 0], [guest, 1]]) peer.socket.send(JSON.stringify({type: 'take-seat', seat, loadout: seat}));
      viewer.socket.send(JSON.stringify({type: 'spectate'}));
      await wait(async () => (await status()).products[game]?.players === 2 && (await status()).products[game]?.spectators === 1);
      assert.deepEqual((await status()).products[game], {rooms: 1, waiting: 1, playing: 0, players: 2, spectators: 1});
      for (const peer of [host, guest]) peer.socket.send(JSON.stringify({type: 'set-ready', ready: true}));
      await wait(() => host.messages.some(m => m.room?.seats?.slice(0, 2).every(seat => seat?.ready)));
      host.socket.send(JSON.stringify({type: 'start'}));
      await wait(async () => (await status()).products[game]?.playing === 1);
      assert.equal((await status()).products[game].waiting, 0);
      for (const peer of [host, guest, viewer]) peer.socket.close();
      await wait(async () => !(await status()).products[game]);
    }
  } finally {
    for (const socket of sockets) socket.terminate();
    if (child && child.exitCode === null) {const closed = once(child, 'exit'); child.kill(); await closed;}
    await rm(work, {recursive: true, force: true});
  }
});
