// BRO 观战帧（bromp）：relay 只认「自身声明协议」的帧，所以这条判据必须是双向的 —
// 认得自己的、绝不吞别人的。帧布局见 ports/bossrush web/src/spectator_wire.mjs。
import test from 'node:test';
import assert from 'node:assert/strict';
import { isSpectatorFrameForRoom, isBroSpectatorFrame, BRO_SPECTATOR_MARKER } from '../src/spectator-frame.mjs';

function broFrame(playerCount, { frame = 0, keysMask = 0 } = {}) {
  const bytes = Buffer.alloc(24 + playerCount * 12);
  bytes[0] = 0x45; // 'E'
  bytes[1] = BRO_SPECTATOR_MARKER;
  bytes[2] = 0x4e; // 'N'
  bytes[3] = 0x50; // 'P'
  bytes[4] = 4;
  bytes[5] = 3;
  bytes[6] = playerCount;
  bytes[7] = 0;
  bytes.writeUInt32LE(frame, 8);
  bytes.writeUInt32LE(1, 12);
  bytes.writeUInt32LE(keysMask, 16);
  return bytes;
}

test('bromp 的每个合法人数都被接受', () => {
  for (const count of [2, 3, 4]) {
    const payload = broFrame(count);
    assert.equal(isBroSpectatorFrame(count, payload), true);
    assert.equal(isSpectatorFrameForRoom(`bromp-${'a'.repeat(8)}`, payload, count), true);
  }
});

test('人数与声明不符的帧被拒（防止把三人的帧塞进两人的局）', () => {
  const payload = broFrame(3);
  assert.equal(isBroSpectatorFrame(2, payload), false);
  assert.equal(isSpectatorFrameForRoom(`bromp-${'a'.repeat(8)}`, payload, 2), false);
});

test('长度、协议字节、kind、保留位任一不符都拒', () => {
  const good = broFrame(2);
  const cases = [];
  const short = Buffer.from(good.subarray(0, good.length - 1)); cases.push(short);
  const marker = Buffer.from(good); marker[1] = 0x36; cases.push(marker);
  const magic = Buffer.from(good); magic[0] = 0x54; cases.push(magic);
  const np = Buffer.from(good); np[3] = 0x42; cases.push(np);
  const ver = Buffer.from(good); ver[4] = 5; cases.push(ver);
  const kind = Buffer.from(good); kind[5] = 1; cases.push(kind);
  const reserved = Buffer.from(good); reserved[7] = 1; cases.push(reserved);
  const padded = Buffer.concat([good, Buffer.alloc(4)]); cases.push(padded);
  for (const payload of cases) assert.equal(isBroSpectatorFrame(2, payload), false);
  assert.equal(isBroSpectatorFrame(2, null), false);
  assert.equal(isBroSpectatorFrame(2, new Uint8Array(0)), false);
});

test('其它作品的房间不会因为 BRO 分支而接受 BRO 帧', () => {
  const payload = broFrame(2);
  for (const room of ['th06mp-aaaaaaaa', 'th09mp-aaaaaaaa', 'th075mp-aaaaaaaa', 'th19mp-aaaaaaaa']) {
    assert.equal(isSpectatorFrameForRoom(room, payload, 2), false, room);
  }
});

test('BRO 房间不接受协作阶段帧/时序帧', () => {
  const room = `bromp-${'a'.repeat(8)}`;
  const stage = Buffer.alloc(32);
  stage.set([69, 0x36, 83, 86], 0); // E 6 S V
  stage[6] = 2;
  assert.equal(isSpectatorFrameForRoom(room, stage, 2), false);
  const timing = Buffer.alloc(40);
  timing.set([69, BRO_SPECTATOR_MARKER, 84, 77], 0); // E R T M
  timing[6] = 2;
  assert.equal(isSpectatorFrameForRoom(room, timing, 2), false);
});
