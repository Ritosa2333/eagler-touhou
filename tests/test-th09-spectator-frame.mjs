import assert from 'node:assert/strict';
import { isSpectatorFrameForRoom } from '../server/spectator-frame.mjs';

const th09 = Buffer.alloc(46);
th09.set([0x54, 0x39, 0x53, 0x50, 1, 3, 2, 0]);
assert.equal(isSpectatorFrameForRoom('th09mp-1234', th09, 2), true);
assert.equal(isSpectatorFrameForRoom('th06mp-1234', th09, 2), false);
assert.equal(isSpectatorFrameForRoom('th09mp-1234', th09.subarray(0, 45), 2), false);
assert.equal(isSpectatorFrameForRoom('th09mp-1234', th09, 3), false);
const wrong = Buffer.from(th09); wrong[5] = 4;
assert.equal(isSpectatorFrameForRoom('th09mp-1234', wrong, 2), false);

const th07 = Buffer.alloc(24 + 3 * 12);
th07.set([0x45, 0x37, 0x4e, 0x50, 4, 3, 3, 0]);
assert.equal(isSpectatorFrameForRoom('th07mp-1234', th07, 3), true);
assert.equal(isSpectatorFrameForRoom('th09mp-1234', th07, 3), false);
