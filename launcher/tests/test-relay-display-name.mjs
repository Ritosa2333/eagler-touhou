// WebSocket behavior: player admission, spectator admission and renaming all
// remove invisible controls before exposing names in authoritative snapshots.
import assert from 'node:assert/strict';
import { spawn } from 'node:child_process';
import { once } from 'node:events';
import { WebSocket } from 'ws';

const relay = spawn(process.execPath, ['server/netplay-relay.mjs'], {
  windowsHide: true, stdio: ['ignore', 'pipe', 'pipe'],
  env: { ...process.env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1',
    EAGLER_NETPLAY_RELAY_PORT: '0', EAGLER_NETPLAY_STUN_URLS: '' },
});
const sockets = [];
let log = '';
relay.stderr.on('data', data => { log += data; });

function connect(port, id) {
  const socket = new WebSocket(`ws://127.0.0.1:${port}/?${new URLSearchParams({
    room: 'th09mp-6433', lobby: `display_${id}`, member: `private_display_${id}`,
  })}`);
  sockets.push(socket);
  const messages = [], waiters = [];
  socket.on('message', (data, binary) => {
    if (binary) return;
    const message = JSON.parse(String(data));
    messages.push(message);
    for (const waiter of [...waiters]) if (waiter.test(message)) waiter.resolve(message);
  });
  return { socket, messages, wait(test) {
    const found = messages.find(test);
    if (found) return Promise.resolve(found);
    return new Promise((resolve, reject) => {
      const timer = setTimeout(() => {
        waiters.splice(waiters.indexOf(waiter), 1);
        reject(new Error(`relay message timeout: ${log}`));
      }, 5000);
      const waiter = { test, resolve(value) {
        clearTimeout(timer);
        waiters.splice(waiters.indexOf(waiter), 1);
        resolve(value);
      } };
      waiters.push(waiter);
    });
  } };
}

try {
  const port = await new Promise((resolve, reject) => {
    const timer = setTimeout(() => reject(new Error(`relay startup timeout: ${log}`)), 5000);
    relay.stdout.on('data', data => {
      log += data;
      const match = /listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log);
      if (match) { clearTimeout(timer); resolve(Number(match[1])); }
    });
    relay.once('error', error => { clearTimeout(timer); reject(error); });
    relay.once('exit', () => { clearTimeout(timer); reject(new Error(`relay exited: ${log}`)); });
  });
  const player = connect(port, 'player'), spectator = connect(port, 'spectator');
  await player.wait(message => message.type === 'state');
  await spectator.wait(message => message.type === 'state');
  player.socket.send(JSON.stringify({ type: 'take-seat', seat: 0, loadout: 0, name: '\u2067Alice\u2069' }));
  await spectator.wait(message => message.room?.seats[0]?.name === 'Alice');
  spectator.socket.send(JSON.stringify({ type: 'spectate', name: '\u061cViewer\u2068' }));
  await player.wait(message => message.room?.spectators.some(entry => entry.name === 'Viewer'));

  const bidiControls = [0x061c, 0x200e, 0x200f, 0x202a, 0x202b, 0x202c,
    0x202d, 0x202e, 0x2066, 0x2067, 0x2068, 0x2069];
  for (const [index, point] of bidiControls.entries()) {
    const control = String.fromCodePoint(point), expected = `Player${index}`;
    player.socket.send(JSON.stringify({ type: 'set-name', name: `${control}${expected}${control}` }));
    await spectator.wait(message => message.room?.seats[0]?.name === expected);
  }
  const invisible = '\u0000\u001f\u007f\u200b\u200c\u200d\u2060\u2061\u2062\u2063\u2064\ufeff';
  spectator.socket.send(JSON.stringify({ type: 'set-name', name: `${invisible}Watcher${invisible}` }));
  await player.wait(message => message.room?.spectators.some(entry => entry.name === 'Watcher'));
  player.socket.send(JSON.stringify({ type: 'set-name', name: '\u2067'.repeat(12) + '🦊'.repeat(13) }));
  await spectator.wait(message => message.room?.seats[0]?.name === '🦊'.repeat(12));
  console.log('PASS player/spectator display names strip every bidi control and invisible controls before the code-point limit');
} finally {
  for (const socket of sockets) socket.terminate();
  if (relay.exitCode === null && relay.signalCode === null) {
    const exited = once(relay, 'exit');
    relay.kill();
    await exited;
  }
}
