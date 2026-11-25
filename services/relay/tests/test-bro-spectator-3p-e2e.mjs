// BRO 观战 relay e2e（3 人房）：3 人对局下 player-0 经 0xe8 车道上包，
// relay 按 players=3 校验帧、开闸，观众收到历史 + 实时帧。
// 与 test-bro-spectator-e2e.mjs 同款自写 harness（spawn relay on :0），
// 区别仅在人数：eagler 目前只支持 2~3 人联机，这里补上 3 人房覆盖。
import assert from 'node:assert/strict';
import { spawn } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import { WebSocket } from 'ws';

const child = spawn(process.execPath, [fileURLToPath(new URL('../src/netplay-relay.mjs', import.meta.url))], {
  windowsHide: true, stdio: ['ignore', 'pipe', 'pipe'],
  env: {
    ...process.env,
    EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK: '1',
    EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1',
    EAGLER_NETPLAY_RELAY_PORT: '0',
    EAGLER_NETPLAY_PUBLIC_NICKNAMES: '0',
    EAGLER_NETPLAY_STUN_URLS: '',
    EAGLER_NETPLAY_CONNECTIONS_PER_MINUTE: '240',
    EAGLER_NETPLAY_MAX_ROOMS_PER_IP: '8',
  },
});
let log = '', port;
child.stdout.on('data', (b) => {
  log += b;
  const m = String(b).match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);
  if (m) port = Number(m[1]);
});
child.stderr.on('data', (b) => { log += b; });
const sockets = [];
async function wait(check, label) {
  const end = Date.now() + 8000;
  while (!check()) {
    if (Date.now() > end) throw Error(label + ' timeout\n' + log);
    await new Promise((r) => setTimeout(r, 10));
  }
}
async function open(query, { allowClosed = false } = {}) {
  const ws = new WebSocket(`ws://127.0.0.1:${port}/?${query}`);
  const r = { ws, json: [], binary: [], closed: false, closeCode: null };
  sockets.push(ws);
  ws.on('message', (b, binary) => binary ? r.binary.push(Buffer.from(b)) : r.json.push(JSON.parse(String(b))));
  ws.on('close', (code, reason) => { r.closed = true; r.closeCode = code + ' ' + reason; });
  await wait(() => ws.readyState === 1 || r.closed, 'open ' + query);
  if (!allowClosed) assert.equal(r.closed, false, query + ' ' + r.closeCode + '\n' + log);
  return r;
}
const send = (s, v) => s.ws.send(JSON.stringify(v));
const roomOf = (s) => s.json.findLast((m) => m.room && typeof m.room === 'object')?.room;
function broFrame(playerCount, frame, epoch) {
  const b = Buffer.alloc(24 + playerCount * 12);
  b.set([0x45, 0x52, 0x4e, 0x50, 4, 3, playerCount, 0]);
  b.writeUInt32LE(frame, 8);
  b.writeUInt32LE(epoch, 12);
  b.writeUInt32LE(1, 16);
  return b;
}
const upload = (s, b) => s.ws.send(Buffer.concat([Buffer.from([0xe8]), b]));

try {
  await wait(() => port, 'startup');
  const id = 'bromp-spec3p' + (Date.now() % 1e8).toString(36);
  const prefix = 'bro3p_';
  const base = `room=${id}&run=1&players=3`;

  // 1. 三席占座 + ready + start
  // bromp 房间默认 playerCount=2，必须由 P1 先发 settings 把人数改成 3，
  // 否则第三席 take-seat 会被拒绝（seat >= playerCount）。
  const p = [];
  for (let i = 0; i < 3; i++) {
    p.push(await open(`room=${id}&lobby=${prefix}l${i}&member=${prefix}m${i}`));
  }
  send(p[0], { type: 'take-seat', seat: 0, loadout: 0 });
  await wait(() => roomOf(p[0])?.seats?.[0], 'host seat 0');
  send(p[0], { type: 'settings', playerCount: 3 });
  await wait(() => roomOf(p[0])?.playerCount === 3 || roomOf(p[0])?.seats?.length === 3, 'settings playerCount=3');
  send(p[1], { type: 'take-seat', seat: 1, loadout: 1 });
  send(p[2], { type: 'take-seat', seat: 2, loadout: 2 });
  await wait(() => roomOf(p[0])?.seats?.slice(0, 3).every((s) => s), '3 seats');
  for (const s of p) send(s, { type: 'set-ready', ready: true });
  await wait(() => roomOf(p[0])?.seats.slice(0, 3).every((s) => s?.ready), '3 ready');
  send(p[0], { type: 'start' });
  await wait(() => p[0].json.some((m) => m.type === 'start'), 'start');

  // 2. player-0 data socket（主播发布端），players=3
  const p0 = await open(base + '&player=0&member=' + prefix + 'm0');

  // 3. 观众 lobby 申请观战（首帧前不开闸）
  const viewerLobby = await open(`room=${id}&lobby=${prefix}viewer&member=${prefix}mviewer`);
  send(viewerLobby, { type: 'spectate' });
  await new Promise((r) => setTimeout(r, 60));
  assert.equal(viewerLobby.json.some((m) => m.type === 'spectator-start'), false, '首帧前不应开闸');

  // 4. 首个 3 人 BRO 观战帧 → 开闸
  upload(p0, broFrame(3, 0, 12345));
  await wait(() => viewerLobby.json.some((m) => m.type === 'spectator-start'), 'spectator-start');
  assert.ok(roomOf(viewerLobby), 'spectator-start 带房间快照');

  // 5. 观众 data socket → 收到 3 人帧（60 字节）回放
  const viewer = await open(base + '&spectator=' + prefix + 'viewer&member=' + prefix + 'mviewer');
  await wait(() => viewer.binary.length >= 1, 'history replay');
  const first = viewer.binary[0];
  assert.equal(first.length, 24 + 3 * 12, '3 人帧应为 60 字节');
  assert.equal(first[0], 0x45);
  assert.equal(first[1], 0x52);
  assert.equal(first[6], 3, '帧内 playerCount=3');
  assert.equal(first.readUInt32LE(8), 0, '回放帧 frame=0');

  // 6. 实时帧
  const before = viewer.binary.length;
  upload(p0, broFrame(3, 1, 12345));
  await wait(() => viewer.binary.length > before, 'live frame');
  assert.equal(viewer.binary.at(-1).readUInt32LE(8), 1, '实时帧 frame=1');

  // 7. 人数不符的帧（2 人帧）不得污染 3 人回放
  const before2 = viewer.binary.length;
  upload(p0, broFrame(2, 2, 12345));
  await new Promise((r) => setTimeout(r, 80));
  assert.equal(viewer.binary.length, before2, '2 人帧不应进 3 人回放');

  console.log('PASS BRO 观战 relay e2e（3 人房）：3 席占座 → 0xe8 上包 → 开闸 → 60 字节帧回放 + 实时帧 + 人数隔离');
} finally {
  for (const s of sockets) s.close();
  child.kill();
}
