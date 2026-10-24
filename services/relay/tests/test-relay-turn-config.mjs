import test from 'node:test';
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createHmac} from 'node:crypto';
import {once} from 'node:events';
import {mkdtemp, writeFile, rename, rm, symlink} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {setTimeout as delay} from 'node:timers/promises';
import {WebSocket} from 'ws';
import {packageRelay} from '../scripts/package.mjs';

for (const packaged of [false, true]) test(`${packaged ? 'packaged' : 'source'} Relay switches TURN on room, signaling and diagnostic paths`, {timeout: 30000}, async () => {
  const source = resolve(import.meta.dirname, '..');
  const work = await mkdtemp(resolve(tmpdir(), 'eagler-turn-config-'));
  const config = resolve(work, 'active-turn.json');
  let child, log = '';
  const sockets = [];
  try {
    let cwd = source, entry = 'src/netplay-relay.mjs';
    if (packaged) {
      cwd = resolve(work, 'package');
      await packageRelay(cwd);
      // Use the locked production dependency already installed by npm ci.
      await symlink(resolve(source, 'node_modules'), resolve(cwd, 'node_modules'), process.platform === 'win32' ? 'junction' : 'dir');
      entry = 'server/netplay-relay.mjs';
    }
    const env = Object.fromEntries(Object.entries(process.env).filter(([key]) => !/^(EAGLER_NETPLAY_|TH07_)/.test(key)));
    child = spawn(process.execPath, [entry], {cwd, windowsHide: true, stdio: ['ignore', 'pipe', 'pipe'], env: {
      ...env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1', EAGLER_NETPLAY_RELAY_PORT: '0',
      EAGLER_NETPLAY_PUBLIC_NICKNAMES: '0', EAGLER_NETPLAY_STUN_URLS: '',
      EAGLER_NETPLAY_TURN_URLS: 'turn:fallback.example:3478',
      EAGLER_NETPLAY_TURN_SHARED_SECRET: 'fallback-secret', EAGLER_NETPLAY_TURN_CONFIG_FILE: config,
    }});
    const port = await new Promise((yes, no) => {
      const timer = setTimeout(() => no(Error('Relay startup timeout: ' + log)), 10000);
      child.stderr.on('data', bytes => log += bytes);
      child.stdout.on('data', bytes => {
        log += bytes;
        const match = /listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log);
        if (match) {clearTimeout(timer); yes(Number(match[1]));}
      });
      child.once('error', error => {clearTimeout(timer); no(error);});
      child.once('exit', code => {clearTimeout(timer); no(Error(`Relay exited ${code}: ${log}`));});
    });
    function connect(params) {
      const socket = new WebSocket(`ws://127.0.0.1:${port}/?${new URLSearchParams(params)}`);
      sockets.push(socket);
      const messages = [], waiters = [];
      socket.on('error', () => {});
      socket.on('message', bytes => {
        const message = JSON.parse(String(bytes));
        messages.push(message);
        for (const waiter of [...waiters]) if (waiter.match(message)) waiter.resolve(message);
      });
      return {socket, messages, wait(match) {
        const found = messages.find(match);
        if (found) return Promise.resolve(found);
        return new Promise((yes, no) => {
          const timer = setTimeout(() => no(Error('Relay message timeout: ' + log)), 4000);
          const waiter = {match, resolve(value) {
            clearTimeout(timer); waiters.splice(waiters.indexOf(waiter), 1); yes(value);
          }};
          waiters.push(waiter);
        });
      }};
    }
    async function close(client) {
      const closed = once(client.socket, 'close');
      client.socket.close();
      await closed;
    }
    async function send(client, payload, match) {
      client.messages.length = 0;
      const reply = client.wait(match);
      client.socket.send(JSON.stringify(payload));
      return reply;
    }
    const room = 'th07mp-turnconfig';
    const host = connect({room, lobby: 'turn_host_client', member: 'turn_host_member'});
    const guest = connect({room, lobby: 'turn_guest_client', member: 'turn_guest_member'});
    await Promise.all([host.wait(m => m.type === 'state'), guest.wait(m => m.type === 'state')]);
    await send(host, {type: 'take-seat', seat: 0, loadout: 0}, m => m.room?.seats[0]);
    await send(guest, {type: 'take-seat', seat: 1, loadout: 1}, m => m.room?.seats[1]);
    await send(host, {type: 'set-ready', ready: true}, m => m.room?.seats[0]?.ready);
    await send(guest, {type: 'set-ready', ready: true}, m => m.room?.seats.slice(0, 2).every(s => s?.ready));
    const started = await send(host, {type: 'start'}, m => m.type === 'start');
    const guestSignal = connect({room, run: String(started.serial), player: '1', players: '2', signal: '1', member: 'turn_guest_member'});
    await guestSignal.wait(m => m.type === 'peers');

    async function check(urls, secret) {
      const lobby = connect({room, lobby: 'turn_probe_client', member: 'turn_probe_member'});
      const state = await lobby.wait(m => m.type === 'state');
      const signal = connect({room, run: String(started.serial), player: '0', players: '2', signal: '1', member: 'turn_host_member'});
      const peers = await signal.wait(m => m.type === 'peers');
      const diagnostic = connect({diagnostic: '1'});
      const ready = await diagnostic.wait(m => m.type === 'diagnostic-ready');
      for (const servers of [state.roomProbe.iceServers, peers.iceServers, ready.iceServers]) {
        assert.equal(servers.length, 1);
        const turn = servers[0];
        assert.deepEqual(turn.urls, urls);
        assert.equal(turn.credential, createHmac('sha1', secret).update(turn.username).digest('base64'));
        assert.equal('sharedSecret' in turn, false);
        const expiry = Number(turn.username.split(':')[0]);
        assert.ok(expiry > Date.now() / 1000 && expiry <= Date.now() / 1000 + 3600);
      }
      await Promise.all([close(lobby), close(signal), close(diagnostic)]);
      assert.equal(child.exitCode, null, 'configuration changes preserve the Relay process');
    }
    async function publish(value) {
      const temporary = config + '.next';
      await writeFile(temporary, JSON.stringify(value));
      await rename(temporary, config);
      await delay(1100);
    }
    await check(['turn:fallback.example:3478'], 'fallback-secret');
    await publish({urls: ['turn:first.example:3478?transport=udp'], sharedSecret: 'first-secret'});
    await check(['turn:first.example:3478?transport=udp'], 'first-secret');
    const urls = ['turn:second.example:3478?transport=udp', 'turns:second.example:5349'];
    await publish({urls, sharedSecret: 'second-secret'});
    await check(urls, 'second-secret');
    await publish({urls: ['https://invalid.example'], sharedSecret: 'invalid-secret'});
    await check(urls, 'second-secret');
    await rm(config);
    await delay(1100);
    await check(urls, 'second-secret');
  } finally {
    for (const socket of sockets) if (socket.readyState < WebSocket.CLOSING) socket.terminate();
    if (child && child.exitCode === null) {const exited = once(child, 'exit'); child.kill(); await exited;}
    await rm(work, {recursive: true, force: true});
  }
});
