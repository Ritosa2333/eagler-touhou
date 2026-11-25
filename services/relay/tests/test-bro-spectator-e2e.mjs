// BRO 观战阶段 A 端到端（relay 层）：验证 player-0 data socket 经 0xe8 车道
// 上传 packSpecInput 帧后，relay 开闸并让观众 socket 收到历史回放 + 实时帧。
// 与 test-spectator-session-relay.mjs 同款自写 harness（spawn relay on :0）。
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
  const id = 'bromp-spec' + (Date.now() % 1e8).toString(36);
  const prefix = 'bro_spec_';
  const base = `room=${id}&run=1&players=2`;

  // 1. 建局：host/guest 占座、ready、start
  const host = await open(`room=${id}&lobby=${prefix}host&member=${prefix}m_host`);
  const guest = await open(`room=${id}&lobby=${prefix}guest&member=${prefix}m_guest`);
  send(host, { type: 'take-seat', seat: 0, loadout: 0 });
  send(guest, { type: 'take-seat', seat: 1, loadout: 1 });
  await wait(() => roomOf(host)?.seats?.[0] && roomOf(host)?.seats?.[1], 'seats');
  send(host, { type: 'set-ready', ready: true });
  send(guest, { type: 'set-ready', ready: true });
  await wait(() => roomOf(host)?.seats.slice(0, 2).every((s) => s?.ready), 'ready');
  send(host, { type: 'start' });
  await wait(() => host.json.some((m) => m.type === 'start'), 'start');

  // 2. player-0 data socket（主播发布端）
  const p0 = await open(base + '&player=0&member=' + prefix + 'm_host');

  // 3. 观众 lobby 申请观战（此时尚无观战帧 → 还在等待）
  const viewerLobby = await open(`room=${id}&lobby=${prefix}viewer&member=${prefix}m_viewer`);
  send(viewerLobby, { type: 'spectate' });
  await new Promise((r) => setTimeout(r, 60));
  assert.equal(viewerLobby.json.some((m) => m.type === 'spectator-start'), false, '首帧前不应开闸');

  // 4. 首个 BRO 观战帧 → relay 开闸 + 给 lobby 观众发 spectator-start
  upload(p0, broFrame(2, 0, 12345));
  await wait(() => viewerLobby.json.some((m) => m.type === 'spectator-start'), 'spectator-start');
  const snap = viewerLobby.json.findLast((m) => m.type === 'spectator-start');
  assert.ok(snap.room, 'spectator-start 带房间快照');

  // 5. 观众 data socket → 收到历史回放（含刚上传的 frame 0）
  const viewer = await open(base + '&spectator=' + prefix + 'viewer&member=' + prefix + 'm_viewer');
  await wait(() => viewer.binary.length >= 1, 'history replay');
  const first = viewer.binary[0];
  assert.equal(first[0], 0x45, '回放帧首字节 E（BRO 帧原样转发）');
  assert.equal(first[1], 0x52, '回放帧次字节 R marker');
  assert.equal(first.readUInt32LE(8), 0, '回放帧 frame=0');

  // 6. 实时帧：再上传 frame 1，观众立刻收到
  const before = viewer.binary.length;
  upload(p0, broFrame(2, 1, 12345));
  await wait(() => viewer.binary.length > before, 'live frame');
  assert.equal(viewer.binary.at(-1).readUInt32LE(8), 1, '实时帧 frame=1');
  assert.equal(viewer.closed, false, '观众收到实时帧后仍在线');

  // 7. 普通 gameplay 帧（非 0xe8）不应被当成观战帧污染回放
  const before2 = viewer.binary.length;
  p0.ws.send(Buffer.from([1, 2, 3]));
  await new Promise((r) => setTimeout(r, 80));
  assert.equal(viewer.binary.length, before2, '普通帧不进观战回放');

  console.log('PASS BRO 观战阶段 A relay e2e：player-0 上传 → 开闸 → 历史回放 + 实时帧 + 普通帧隔离');
} finally {
  for (const s of sockets) s.close();
  child.kill();
}
