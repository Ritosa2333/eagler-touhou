import assert from 'node:assert/strict';
import { spawn } from 'node:child_process';
import { once } from 'node:events';
import { WebSocket } from 'ws';

// Isolated loopback service: simulate an exact trusted proxy, never production.
const relay = spawn(process.execPath, ['netplay-relay.mjs'], {
  cwd: new URL('../src/', import.meta.url), windowsHide: true, stdio: ['ignore', 'pipe', 'pipe'],
  env: { ...process.env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1',
    EAGLER_NETPLAY_RELAY_PORT: '0', EAGLER_NETPLAY_STUN_URLS: '',
    EAGLER_NETPLAY_TRUSTED_PROXIES: '127.0.0.1', EAGLER_NETPLAY_ROOM_LOOKUPS_PER_MINUTE: '2',
    EAGLER_NETPLAY_CONNECTIONS_PER_MINUTE: '100' },
});
const sockets = [];
let log = '';
try {
  const port = await new Promise((resolve, reject) => {
    const timer = setTimeout(() => reject(new Error(`relay startup timeout: ${log}`)), 5000);
    relay.stderr.on('data', (data) => { log += data; });
    relay.stdout.on('data', (data) => {
      log += data;
      const match = /listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log);
      if (match) { clearTimeout(timer); resolve(Number(match[1])); }
    });
    relay.once('exit', () => { clearTimeout(timer); reject(new Error(`relay exited: ${log}`)); });
  });
  let serial = 0;
  async function first(params, address = '203.0.113.1') {
    const socket = new WebSocket(`ws://127.0.0.1:${port}/?${new URLSearchParams(params)}`, {
      headers: { 'X-Real-IP': address },
    });
    sockets.push(socket);
    const timer = setTimeout(() => socket.terminate(), 3000);
    try { return JSON.parse(String((await once(socket, 'message'))[0])); }
    finally { clearTimeout(timer); socket.close(); }
  }
  const lobby = (code, intent = 'join') => ({
    room: `th09mp-${code}`, intent, lobby: `lookup_client_${++serial}`, member: `lookup_member_${serial}`,
  });
  for (const code of ['11111111', '22222222']) {
    const missing = await first(lobby(code));
    assert.equal(missing.type, 'error');
    assert.notEqual(missing.code, 'rate-limited');
  }
  assert.equal((await first(lobby('33333333'))).code, 'rate-limited', 'failed joins consume target lookup quota');
  assert.equal((await first(lobby('33333333', 'create'))).code, 'rate-limited', 'create intent cannot bypass lookup limits');
  for (const signal of [undefined, '1'])
    assert.equal((await first({room:'th09mp-33333333', run:'1', player:'0', players:'2', signal})).code, 'rate-limited');
  assert.notEqual((await first(lobby('11111111'))).code, 'rate-limited', 'revisiting the same target is permitted');
  assert.notEqual((await first(lobby('33333333'), '203.0.113.2')).code, 'rate-limited', 'proxy-attributed IP quotas are isolated');
  assert.equal((await first({directory:'1', member:'lookup_directory_member'})).type, 'directory', 'room quota does not block directory recovery');
  console.log('PASS distinct-room enumeration limits across lobby/create/game/signaling and per-client IP isolation');
} finally {
  for (const socket of sockets) socket.terminate();
  const exited = once(relay, 'exit');
  relay.kill();
  await exited;
}
