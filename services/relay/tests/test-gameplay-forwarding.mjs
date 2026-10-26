import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {fileURLToPath} from 'node:url';

function relayPort() {
  const listener = net.createServer();
  return new Promise(resolve => listener.listen(0, '127.0.0.1', () => {
    const port = listener.address().port;
    listener.close(() => resolve(port));
  }));
}
async function startRelay(port, env = {}) {
  const child = spawn(process.execPath, [fileURLToPath(new URL('../src/netplay-relay.mjs', import.meta.url))], {
    env: {...process.env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1', EAGLER_NETPLAY_RELAY_PORT: String(port),
      EAGLER_NETPLAY_PUBLIC_NICKNAMES: '0', ...env},
    stdio: ['ignore', 'pipe', 'pipe'],
    windowsHide: true,
  });
  let log = '';
  child.stdout.on('data', bytes => log += bytes);
  child.stderr.on('data', bytes => log += bytes);
  await new Promise((resolve, reject) => {
    const end = Date.now() + 8000;
    const tick = () => log.includes('listening') ? resolve() : Date.now() > end
      ? reject(new Error(`relay ready\n${log}`)) : setTimeout(tick, 5);
    tick();
  });
  return {child, log: () => log};
}

// --- 未知房间：玩家与信令连接不得隐式建房 ---
{
  const port = await relayPort();
  const {child, log} = await startRelay(port, {
    EAGLER_NETPLAY_MAX_ROOMS: '2', EAGLER_NETPLAY_MAX_ROOMS_PER_IP: '16',
    EAGLER_NETPLAY_MAX_CONNECTIONS_PER_IP: '64', EAGLER_NETPLAY_CONNECTIONS_PER_MINUTE: '120',
  });
  const sockets = [];
  const open = query => {
    const socket = {ws: new WebSocket(`ws://127.0.0.1:${port}/?${query}`), closed: null};
    sockets.push(socket.ws);
    socket.ws.addEventListener('close', event => socket.closed = {code: event.code, reason: event.reason});
    return new Promise((resolve, reject) => {
      socket.ws.addEventListener('open', () => resolve(socket), {once: true});
      socket.ws.addEventListener('error', () => resolve(socket), {once: true});
      setTimeout(() => reject(new Error(`open ${query}\n${log()}`)), 8000);
    });
  };
  try {
    for (let index = 0; index < 3; index++) {
      const gameplay = await open(`room=th06mp-9901${index}&run=1&player=0&players=2&member=unknown_member_${index}`);
      await new Promise(resolve => setTimeout(resolve, 50));
      assert.ok(gameplay.closed, 'unknown-room gameplay socket must close');
      assert.equal(gameplay.closed.code, 4007);
      assert.equal(gameplay.closed.reason, 'room unavailable');
      const signal = await open(`room=th06mp-9902${index}&run=1&player=0&players=2&member=unknown_signal_${index}&signal=1`);
      await new Promise(resolve => setTimeout(resolve, 50));
      assert.ok(signal.closed, 'unknown-room signal socket must close');
      assert.equal(signal.closed.code, 4007);
      assert.equal(signal.closed.reason, 'room unavailable');
    }
    const lobby = await open('room=th06mp-990300&lobby=lobby990300&member=m990300_host');
    await new Promise(resolve => setTimeout(resolve, 150));
    assert.equal(lobby.closed, null, 'room creation must stay available after unknown-room sockets');
    assert.ok(lobby.ws.readyState === WebSocket.OPEN);
  } finally {
    for (const ws of sockets) ws.close();
    child.kill();
  }
}

// --- 对局帧席位改写：E?NP / ADS / ADP 的发送者一律取发送连接 ---
{
  const port = await relayPort();
  const {child, log} = await startRelay(port, {
    EAGLER_NETPLAY_MAX_CONNECTIONS_PER_IP: '32', EAGLER_NETPLAY_CONNECTIONS_PER_MINUTE: '120',
  });
  const sockets = [];
  const open = query => {
    const socket = {ws: new WebSocket(`ws://127.0.0.1:${port}/?${query}`), json: [], frames: []};
    sockets.push(socket.ws);
    socket.ws.addEventListener('message', event => {
      if (typeof event.data === 'string') socket.json.push(JSON.parse(event.data));
      else event.data.arrayBuffer().then(buffer => socket.frames.push(Buffer.from(buffer)));
    });
    return new Promise((resolve, reject) => {
      socket.ws.addEventListener('open', () => resolve(socket), {once: true});
      socket.ws.addEventListener('error', () => reject(new Error(`open ${query}`)), {once: true});
      setTimeout(() => reject(new Error(`open ${query}\n${log()}`)), 8000);
    });
  };
  const send = (socket, message) => socket.ws.send(JSON.stringify(message));
  const latest = socket => [...socket.json].reverse().find(message => message.room && typeof message.room === 'object')?.room;
  const wait = async (test, label) => {
    const end = Date.now() + 8000;
    while (!test()) {
      if (Date.now() > end) throw new Error(`${label}\n${log()}`);
      await new Promise(resolve => setTimeout(resolve, 5));
    }
  };

  function inputFrame(sender) {
    const frame = Buffer.alloc(44);
    frame.set([0x45, 0x54, 0x4e, 0x50, 4, 1, sender, 2]); // 'ETNP' v4, input, sender, playerCount
    frame.writeBigUInt64LE(77n, 8);
    frame.writeUInt32LE(0xffffffff, 24); // latestFrame
    frame.writeUInt32LE(0xffffffff, 32); // firstInputFrame (ACK-only)
    return frame;
  }
  function sessionFrame(sender) {
    const frame = Buffer.alloc(32);
    frame.set([0x45, 0x54, 0x4e, 0x50, 4, 2, sender, 2]);
    frame.writeBigUInt64LE(77n, 8);
    frame[28] = 1; // Hello
    return frame;
  }
  function adsFrame(peer) {
    const frame = Buffer.alloc(64);
    frame.set([0x41, 0x44, 0x53, 4, 1, peer, 1, 2]); // 'ADS' v4, Hello, peer, mode
    frame.writeBigUInt64LE(77n, 8);
    frame[56] = 2; // playerCount
    frame[57] = 255;
    return frame;
  }
  function adpFrame(sender) {
    const frame = Buffer.alloc(28);
    frame.set([0x41, 0x44, 0x50, 1]);
    frame.writeBigUInt64LE(77n, 4);
    frame.writeUInt32LE(16, 12);
    frame[16] = sender; frame[17] = sender ^ 1;
    frame[18] = 16; frame[19] = 0;
    return frame;
  }

  try {
    const host = await open('room=th06mp-990400&lobby=lobby9904host&member=m990400_host');
    const guest = await open('room=th06mp-990400&lobby=lobby9904guest&member=m990400_guest');
    send(host, {type: 'take-seat', seat: 0, loadout: 0});
    send(guest, {type: 'take-seat', seat: 1, loadout: 0});
    await wait(() => latest(host)?.seats?.slice(0, 2).every(Boolean), 'both seats taken');
    send(host, {type: 'set-ready', ready: true});
    send(guest, {type: 'set-ready', ready: true});
    await wait(() => latest(host)?.seats?.slice(0, 2).every(seat => seat.ready), 'both seats ready');
    send(host, {type: 'start'});
    await wait(() => host.json.some(message => message.type === 'start'), 'start');

    const p0 = await open('room=th06mp-990400&run=1&player=0&players=2&member=m990400_host');
    const p1 = await open('room=th06mp-990400&run=1&player=1&players=2&member=m990400_guest');
    const received = () => p1.frames.length;
    const targeted = payload => Buffer.concat([Buffer.from([0xe7, 0x01]), payload]);

    // 广播帧：伪造 sender=1（接收者自己的席位）
    p0.ws.send(inputFrame(1));
    await wait(() => received() === 1, 'broadcast input forwarded');
    assert.equal(p1.frames[0][6], 0, 'input sender is rewritten to the sending connection');
    assert.equal(p1.frames[0].subarray(0, 6).equals(inputFrame(1).subarray(0, 6)), true, 'only the sender byte changes');

    // 定向帧：伪造 sender=2（不存在的第三席位）
    p0.ws.send(targeted(inputFrame(2)));
    await wait(() => received() === 2, 'targeted input forwarded');
    assert.equal(p1.frames[1][6], 0);

    // 会话包（HELLO/READY）与控制族同样改写
    p0.ws.send(sessionFrame(2));
    await wait(() => received() === 3, 'session forwarded');
    assert.equal(p1.frames[2][6], 0);
    p0.ws.send(adsFrame(7));
    await wait(() => received() === 4, 'ADS forwarded');
    assert.equal(p1.frames[3][5], 0, 'ADS peer is rewritten to the sending connection');
    p0.ws.send(adpFrame(7));
    await wait(() => received() === 5, 'ADP forwarded');
    assert.equal(p1.frames[4][16], 0, 'ADP sender is rewritten to the sending connection');

    // 诚实帧原样到达
    const honest = inputFrame(0);
    p0.ws.send(honest);
    await wait(() => received() === 6, 'honest input forwarded');
    assert.equal(p1.frames[5].equals(honest), true, 'a frame stamped with the real seat arrives byte-identical');
  } finally {
    for (const ws of sockets) ws.close();
    child.kill();
  }
}
