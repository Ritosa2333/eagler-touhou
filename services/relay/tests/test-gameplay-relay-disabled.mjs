import test from 'node:test';
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {WebSocket} from 'ws';

test('RTC failure ends the run without selecting WebSocket gameplay', {timeout: 15000}, async () => {
  const child = spawn(process.execPath, ['src/netplay-relay.mjs'], {
    cwd: new URL('../', import.meta.url), windowsHide: true, stdio: ['ignore', 'pipe', 'pipe'],
    env: {...process.env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1', EAGLER_NETPLAY_RELAY_PORT: '0',
      EAGLER_NETPLAY_STUN_URLS: '', EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK: '0'},
  });
  let log = '';
  const sockets = [];
  const wait = async (condition, label) => {
    const until = Date.now() + 6000;
    while (!condition()) {
      if (Date.now() > until) throw Error(`${label} timed out\n${log}`);
      await new Promise(resolve => setTimeout(resolve, 5));
    }
  };
  try {
    child.stdout.on('data', bytes => log += bytes);
    child.stderr.on('data', bytes => log += bytes);
    await wait(() => /listening ws:\/\/127\.0\.0\.1:\d+/.test(log), 'relay start');
    const port = /listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log)[1];
    async function open(query) {
      const socket = new WebSocket(`ws://127.0.0.1:${port}/?${query}`);
      const peer = {socket, messages: [], binary: [], closed: false};
      sockets.push(socket);
      socket.on('message', (bytes, isBinary) => {
        if (isBinary) {peer.binary.push(Buffer.from(bytes)); return;}
        try {peer.messages.push(JSON.parse(String(bytes)));} catch {}
      });
      socket.on('close', () => peer.closed = true);
      socket.on('error', () => {});
      await wait(() => socket.readyState === WebSocket.OPEN || peer.closed, 'socket open');
      assert.equal(socket.readyState, WebSocket.OPEN);
      return peer;
    }
    const send = (peer, value) => peer.socket.send(JSON.stringify(value));
    const room = 'th09mp-no-ws-fallback';
    const host = await open(`room=${room}&lobby=host_lobby&member=member_host`);
    const guest = await open(`room=${room}&lobby=guest_lobby&member=member_guest`);
    const viewer = await open(`room=${room}&lobby=viewer_lobby&member=member_viewer`);
    send(host, {type: 'take-seat', seat: 0, loadout: 0});
    send(guest, {type: 'take-seat', seat: 1, loadout: 1});
    send(viewer, {type: 'spectate'});
    await wait(() => host.messages.some(m => m.room?.seats?.[0] && m.room?.seats?.[1]), 'seats');
    send(host, {type: 'set-ready', ready: true});
    send(guest, {type: 'set-ready', ready: true});
    await wait(() => host.messages.some(m => m.room?.seats?.slice(0, 2).every(s => s?.ready)), 'ready');
    send(host, {type: 'start'});
    await wait(() => host.messages.some(m => m.type === 'start'), 'start');
    const base = `room=${room}&run=1&players=2`;
    const spectator = await open(`${base}&spectator=viewer_lobby&member=member_viewer`);
    const r0 = await open(`${base}&player=0&member=member_host`);
    const r1 = await open(`${base}&player=1&member=member_guest`);
    const s0 = await open(`${base}&player=0&signal=1&member=member_host`);
    const s1 = await open(`${base}&player=1&signal=1&member=member_guest`);
    await wait(() => s0.messages.some(m => m.type === 'peers') && s1.messages.some(m => m.type === 'peers'), 'signaling');
    r0.socket.send(Buffer.from([0xe7, 1, 11]));
    await new Promise(resolve => setTimeout(resolve, 50));
    assert.equal(r1.binary.length, 0, 'Disabled gameplay relay does not forward input');
    send(s0, {type: 'rtc-failed'});
    await wait(() => [r0, r1, s0, s1].every(p => p.messages.some(m => m.type === 'route' && m.mode === 'failed')), 'failure broadcast');
    await wait(() => [r0, r1, s0, s1, spectator].every(p => p.closed), 'failed run close');
    await wait(() => host.messages.at(-1)?.room?.phase === 'lobby', 'lobby recovery');
    assert.equal(host.messages.some(m => m.type === 'route' && m.mode === 'relay'), false);
    assert.equal(log.includes('mode=relay'), false);

    send(host, {type: 'set-ready', ready: true});
    send(guest, {type: 'set-ready', ready: true});
    await wait(() => host.messages.at(-1)?.room?.seats?.slice(0, 2).every(s => s?.ready), 'ready after failure');
    send(host, {type: 'start'});
    await wait(() => host.messages.some(m => m.type === 'start' && m.serial === 2), 'second start');
    const retry = `room=${room}&run=2&players=2`;
    const next0 = await open(`${retry}&player=0&signal=1&member=member_host`);
    const next1 = await open(`${retry}&player=1&signal=1&member=member_guest`);
    await wait(() => next0.messages.some(m => m.type === 'peers') && next1.messages.some(m => m.type === 'peers'), 'second signaling');
    send(next0, {type: 'rtc-ready'});
    send(next1, {type: 'rtc-ready'});
    await wait(() => [next0, next1].every(p => p.messages.some(m => m.type === 'route' && m.mode === 'rtc')), 'RTC route');
    assert.equal(next0.closed, false);
    assert.equal(next1.closed, false);
  } finally {
    for (const socket of sockets) if (socket.readyState === WebSocket.OPEN) socket.close();
    child.kill();
  }
});
