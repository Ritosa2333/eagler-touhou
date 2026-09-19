const { spawn } = require('node:child_process');
const net = require('node:net');
const { requireHostFile, requireHostNodeModule } = require('./integration-support.cjs');
const WebSocket = requireHostNodeModule('ws');
const relayPath = requireHostFile('server/netplay-relay.mjs', 'netplay relay');

const port = 19000 + Math.floor(Math.random() * 20000);
const room = `spectator-${Date.now()}`;
const ids = {
  p1: 'player_one_0001', p2: 'player_two_0002', spectator: 'spectator_0003',
  late: 'late_join_0004', tooLate: 'too_late_0005'
};
const relay = spawn(process.execPath, [relayPath], {
  env: {
    ...process.env,
    TH07_RELAY_HOST: '127.0.0.1',
    TH07_RELAY_PORT: String(port),
    TH07_STUN_URLS: '',
    TH07_SPECTATOR_CONNECT_GRACE_MS: '1500',
  },
  stdio: ['ignore', 'pipe', 'pipe'],
});

const wait = ms => new Promise(resolve => setTimeout(resolve, ms));
const open = url => new Promise((resolve, reject) => {
  const ws = new WebSocket(url);
  ws.once('open', () => resolve(ws));
  ws.once('error', reject);
});
const openWithFirstBinary = url => new Promise((resolve, reject) => {
  const ws = new WebSocket(url);
  let firstResolve;
  const first = new Promise(done => { firstResolve = done; });
  ws.once('message', data => firstResolve(Buffer.from(data)));
  ws.once('open', () => resolve({ ws, first }));
  ws.once('error', reject);
});
const nextJson = ws => new Promise((resolve, reject) => {
  const onMessage = data => { cleanup(); resolve(JSON.parse(String(data))); };
  const onError = error => { cleanup(); reject(error); };
  const cleanup = () => { ws.off('message', onMessage); ws.off('error', onError); };
  ws.on('message', onMessage); ws.on('error', onError);
});
const nextType = async (ws, type) => {
  for (;;) {
    const message = await nextJson(ws);
    if (message.type === type) return message;
  }
};
const waitType = (ws, type) => new Promise((resolve, reject) => {
  const onMessage = data => {
    const message = JSON.parse(String(data));
    if (message.type !== type) return;
    cleanup(); resolve(message);
  };
  const onError = error => { cleanup(); reject(error); };
  const cleanup = () => { ws.off('message', onMessage); ws.off('error', onError); };
  ws.on('message', onMessage); ws.on('error', onError);
});
const nextBinary = ws => new Promise((resolve, reject) => {
  const onMessage = data => { cleanup(); resolve(Buffer.from(data)); };
  const onError = error => { cleanup(); reject(error); };
  const cleanup = () => { ws.off('message', onMessage); ws.off('error', onError); };
  ws.on('message', onMessage); ws.on('error', onError);
});
const closed = ws => new Promise(resolve => ws.once('close', (code, reason) => resolve({ code, reason: String(reason) })));

async function waitRelay() {
  for (let attempt = 0; attempt < 50; attempt++) {
    const ready = await new Promise(resolve => {
      const socket = net.connect(port, '127.0.0.1');
      socket.once('connect', () => { socket.destroy(); resolve(true); });
      socket.once('error', () => resolve(false));
    });
    if (ready) return;
    await wait(50);
  }
  throw new Error('relay did not start');
}

async function main() {
  await waitRelay();
  const base = `ws://127.0.0.1:${port}/?room=${room}`;
  const p1 = await open(`${base}&lobby=${ids.p1}`); await nextJson(p1);
  const p2 = await open(`${base}&lobby=${ids.p2}`); await nextJson(p2);
  const watcher = await open(`${base}&lobby=${ids.spectator}`); await nextJson(watcher);
  p1.send(JSON.stringify({ type: 'take-seat', seat: 0, loadout: 0, ready: true }));
  p2.send(JSON.stringify({ type: 'take-seat', seat: 1, loadout: 2, ready: true }));
  watcher.send(JSON.stringify({ type: 'spectate' }));
  await wait(50);
  const startPromise = nextType(watcher, 'start');
  p1.send(JSON.stringify({ type: 'start' }));
  const start = await startPromise;
  if (start.room?.spectatorCount !== 1 || start.serial !== 1)
    throw new Error(`bad explicit spectator start snapshot: ${JSON.stringify(start)}`);

  const spectator = await open(`${base}&run=1&players=2&spectator=${ids.spectator}`);
  const player = await open(`${base}&run=1&players=2&player=0`);
  const frame = Buffer.alloc(48);
  frame.set([0x45, 0x37, 0x4e, 0x50, 0x04, 0x03, 0x02, 0x00]);
  const received = nextBinary(spectator);
  player.send(Buffer.concat([Buffer.from([0xe8]), frame]));
  const payload = await received;
  if (payload.length !== 48 || payload[5] !== 3) throw new Error('spectator stream was not isolated');

  const readonlyClose = closed(spectator);
  spectator.send(Buffer.from([1]));
  const readonly = await readonlyClose;
  if (readonly.code !== 1008) throw new Error(`spectator write was not rejected: ${JSON.stringify(readonly)}`);

  const reconnect = new WebSocket(`${base}&run=1&players=2&spectator=${ids.spectator}`);
  const reconnectResult = await closed(reconnect);
  if (reconnectResult.code !== 1008) throw new Error(`midgame spectator reconnect was admitted: ${JSON.stringify(reconnectResult)}`);

  // A user who was merely unseated at start is not an implicit spectator.
  // Volunteering during the finite join window grants this run and replays history from frame zero.
  const lateLobby = await open(`${base}&lobby=${ids.late}`); await nextJson(lateLobby);
  const lateStartPromise = waitType(lateLobby, 'spectator-start');
  lateLobby.send(JSON.stringify({ type: 'spectate' }));
  const lateStart = await lateStartPromise;
  if (lateStart.serial !== 1 || lateStart.room?.spectatorCount < 1)
    throw new Error(`late spectator was not admitted to current run: ${JSON.stringify(lateStart)}`);
  const lateOpened = await openWithFirstBinary(`${base}&run=1&players=2&spectator=${ids.late}`);
  const lateSpectator = lateOpened.ws;
  const lateHistory = await lateOpened.first;
  if (!lateHistory.equals(frame)) throw new Error('late spectator did not receive retained frame-zero history');

  // A separate run starts with no spectator at all. P1 still publishes history during
  // the bounded admission window, proving late join does not depend on a start-time watcher.
  const expireRoom = `${room}-expire`;
  const expireBase = `ws://127.0.0.1:${port}/?room=${expireRoom}`;
  const e1 = await open(`${expireBase}&lobby=expire_p1_0001`); await nextJson(e1);
  const e2 = await open(`${expireBase}&lobby=expire_p2_0002`); await nextJson(e2);
  e1.send(JSON.stringify({ type: 'take-seat', seat: 0, loadout: 0, ready: true }));
  e2.send(JSON.stringify({ type: 'take-seat', seat: 1, loadout: 1, ready: true }));
  await wait(50);
  const expireStartPromise = nextType(e1, 'start');
  e1.send(JSON.stringify({ type: 'start' }));
  const expireStart = await expireStartPromise;
  if (expireStart.room?.spectatorCount !== 0 || expireStart.serial !== 1)
    throw new Error(`unseated users were implicitly admitted: ${JSON.stringify(expireStart)}`);
  const expirePlayer = await open(`${expireBase}&run=1&players=2&player=0`);
  expirePlayer.send(Buffer.concat([Buffer.from([0xe8]), frame]));

  await wait(1750);
  const tooLateLobby = await open(`${expireBase}&lobby=${ids.tooLate}`); await nextJson(tooLateLobby);
  const tooLateErrorPromise = waitType(tooLateLobby, 'error');
  tooLateLobby.send(JSON.stringify({ type: 'spectate' }));
  const tooLateError = await tooLateErrorPromise;
  if (!/窗口已关闭/.test(String(tooLateError.error || '')))
    throw new Error(`late-window spectator did not get a clear rejection: ${JSON.stringify(tooLateError)}`);
  const expired = new WebSocket(`${expireBase}&run=1&players=2&spectator=${ids.tooLate}`);
  const expiredResult = await closed(expired);
  if (expiredResult.code !== 1008)
    throw new Error(`expired spectator admission was still accepted: ${JSON.stringify(expiredResult)}`);

  for (const ws of [p1, p2, watcher, lateLobby, lateSpectator, player, e1, e2, expirePlayer, tooLateLobby]) {
    try { ws.close(1000); } catch {}
  }
  console.log('Spectator relay contract: PASS explicit=1 late-window=1 unseated-not-spectator=1 readonly=1 reconnect-rejected=1 expiry=1');
}

main().finally(() => relay.kill()).catch(error => { console.error(error); process.exitCode = 1; });
