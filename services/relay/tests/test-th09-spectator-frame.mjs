import assert from 'node:assert/strict';
import { isSpectatorFrameForRoom } from '../src/spectator-frame.mjs';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';

const th09 = Buffer.alloc(46);
th09.set([0x54, 0x39, 0x53, 0x50, 1, 3, 2, 0]);
assert.equal(isSpectatorFrameForRoom('th09mp-1234', th09, 2), true);
assert.equal(isSpectatorFrameForRoom('th06mp-1234', th09, 2), false);
assert.equal(isSpectatorFrameForRoom('th09mp-1234', th09.subarray(0, 45), 2), false);
assert.equal(isSpectatorFrameForRoom('th09mp-1234', th09, 3), false);
const wrong = Buffer.from(th09); wrong[5] = 4;
assert.equal(isSpectatorFrameForRoom('th09mp-1234', wrong, 2), false);
const priority=Buffer.alloc(126);th09.copy(priority);priority[4]=2;
for(const at of [46,86]){priority.writeFloatLE(-1,at+28);priority.writeUInt32LE(1,at+36);}
assert.equal(isSpectatorFrameForRoom('th09mp-1234',priority,2),true);
assert.equal(isSpectatorFrameForRoom('th07mp-1234',priority,2),false);
for(const [offset,value] of [[46,401],[86,NaN],[74,500]]){
  const invalid=Buffer.from(priority);invalid.writeFloatLE(value,offset);assert.equal(isSpectatorFrameForRoom('th09mp-1234',invalid,2),false);
}
const badEvent=Buffer.from(priority);badEvent.writeUInt32LE(2,78);
assert.equal(isSpectatorFrameForRoom('th09mp-1234',badEvent,2),false);

const th07 = Buffer.alloc(24 + 3 * 12);
th07.set([0x45, 0x37, 0x4e, 0x50, 4, 3, 3, 0]);
assert.equal(isSpectatorFrameForRoom('th07mp-1234', th07, 3), true);
assert.equal(isSpectatorFrameForRoom('th09mp-1234', th07, 3), false);
for (const [product, protocol] of [['th06mp', 0x36], ['th07mp', 0x37], ['th08mp', 0x38], ['th10mp', 0x41]]) {
  for (const count of [2, 3]) {
    const frame = Buffer.alloc(24 + count * 12);
    frame.set([0x45, protocol, 0x4e, 0x50, 4, 3, count, 0]);
    assert.equal(isSpectatorFrameForRoom(`${product}-1234`, frame, count), true);
    assert.equal(isSpectatorFrameForRoom(`${product}-1234`, frame, count === 2 ? 3 : 2), false);
    assert.equal(isSpectatorFrameForRoom(`${product}-1234`, frame.subarray(0, frame.length - 1), count), false);
    for (const other of ['th06mp', 'th07mp', 'th08mp', 'th09mp', 'th10mp', 'unknown'])
      if (other !== product) assert.equal(isSpectatorFrameForRoom(`${other}-1234`, frame, count), false);
  }
}

// th075 shares the confirmed-input envelope while retaining its native
// whole-match spectator owner and its own wire identity.
const th075=Buffer.alloc(48);th075.set([0x45,0x50,0x4e,0x50,4,3,2,0]);
assert.equal(isSpectatorFrameForRoom('th075mp-1234',th075,2),true);
assert.equal(isSpectatorFrameForRoom('th075mp-1234',th075,3),false);
assert.equal(isSpectatorFrameForRoom('th075mp-1234',th075.subarray(0,47),2),false);
for(const product of [...Object.entries(PRODUCT_GAMES).filter(([game,p])=>game!=='th075'&&p.multiplayer).map(([game])=>game+'mp'),'unknown'])
 assert.equal(isSpectatorFrameForRoom(product+'-1234',th075,2),false);
for(const [mode,delay,reserve] of [[1,2,0],[0,0,2],[0,0,8]]){
 const timing=Buffer.alloc(40);timing.set([0x45,0x50,0x54,0x4d,1,mode,delay,reserve]);
 timing.writeUInt32LE(2,8);timing.writeUInt32LE(50000,24);timing.writeUInt32LE(120,36);
 assert.equal(isSpectatorFrameForRoom('th075mp-1234',timing,2),true);
 assert.equal(isSpectatorFrameForRoom('th075mp-1234',timing,3),false);
 assert.equal(isSpectatorFrameForRoom('th09mp-1234',timing,2),false);
 const malformed=Buffer.from(timing);malformed.writeUInt32LE(95,36);
 assert.equal(isSpectatorFrameForRoom('th075mp-1234',malformed,2),false);
}
