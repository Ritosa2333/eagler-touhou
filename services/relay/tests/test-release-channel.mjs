import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {once} from 'node:events';
import {WebSocket} from 'ws';

for (const channel of ['stable', 'beta']) {
  const relay = spawn(process.execPath, ['src/netplay-relay.mjs'], {
    windowsHide: true, stdio: ['ignore', 'pipe', 'pipe'],
    env: {...process.env, EAGLER_RELEASE_CHANNEL: channel, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1', EAGLER_NETPLAY_RELAY_PORT: '0'},
  });
  const sockets = [];
  let log = '';
  try {
    const port = await new Promise((resolve, reject) => {
      const timer = setTimeout(() => reject(Error('Relay startup timeout: ' + log)), 10000);
      relay.stderr.on('data', bytes => {log += bytes;});
      relay.stdout.on('data', bytes => {
        log += bytes;
        const match = /listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log);
        if (match) {clearTimeout(timer); resolve(Number(match[1]));}
      });
      relay.once('exit', () => {clearTimeout(timer); reject(Error('Relay exited: ' + log));});
    });
    const connect = query => {
      const socket = new WebSocket(`ws://127.0.0.1:${port}/?${query}`);
      sockets.push(socket);
      return socket;
    };
    // Every role is rejected before it can enter a room or the directory.
    for (const role of ['directory=1', 'diagnostic=1', 'room=th08mp-12345678&lobby=channel_test_123', 'room=th08mp-12345678&run=1&signal=1']) {
      const other = channel === 'beta' ? '' : '&channel=beta';
      const socket = connect(role + other), received = [];
      socket.on('message', bytes => received.push(bytes));
      const [code, reason] = await once(socket, 'close');
      assert.equal(code, 1008);
      assert.equal(String(reason), 'release channel mismatch');
      assert.equal(received.length, 0);
    }
    const socket = connect('directory=1&member=channel_member_123' + (channel === 'beta' ? '&channel=beta' : ''));
    const [bytes] = await once(socket, 'message');
    assert.equal(JSON.parse(String(bytes)).type, 'directory');
  } finally {
    for (const socket of sockets) if (socket.readyState < 2) socket.terminate();
    if (relay.exitCode === null) {const ended = once(relay, 'exit'); relay.kill(); await ended;}
  }
}
console.log('Stable and beta Relay channels reject cross-channel roles and accept their own directory PASS');
