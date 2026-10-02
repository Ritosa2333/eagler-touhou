import test from 'node:test';
import assert from 'node:assert/strict';
import {SharedNetplay} from '../../sdl-runtime/shared-netplay.mjs';

// The two entries (Launcher card and the in-game title dialog) share one room
// and one gameplay transport; these tests cover the transport contract itself.
function harness() {
  const previous = {fetch: globalThis.fetch, location: globalThis.location, document: globalThis.document, setInterval: globalThis.setInterval};
  const timers=[];globalThis.setInterval=(...args)=>{const timer=previous.setInterval(...args);timers.push(timer);return timer;};
  const peers = new Map();
  const spectators = new Set();
  const core = () => {
    const buffer = new ArrayBuffer(2048), bytes = new Uint8Array(buffer);
    const calls = {begin: [], modes: [], receive: [], pauses: [], spectatorBegin: [], spectatorFeed: [], pumps: 0};
    const self = {
      HEAPU8: bytes, HEAPU32: new Uint32Array(buffer), calls, incoming: [], side: -1,
      _th09_peer_url_buffer: () => 256,
      _th09_peer_spectator_id_buffer: () => 512,
      _th09_peer_packet_buffer: () => 1280,
      _th09_peer_connect: side => {
        self.side = side;
        const end = bytes.indexOf(0, 256);
        const url = new URL(new TextDecoder().decode(bytes.subarray(256, end)));
        assert.equal(Number(url.searchParams.get('player')), side);
        peers.set(side, self);
        return 1;
      },
      _th09_peer_state: () => peers.size === 2 ? 1 : 0,
      _th09_peer_connect_spectator: () => { spectators.add(self); return 1; },
      _th09_peer_send: length => {
        peers.get(1 - self.side)?.incoming.push(bytes.slice(1280, 1280 + length));
        return 1;
      },
      _th09_peer_poll: () => {
        const packet = self.incoming.shift();
        if (!packet) return 0;
        bytes.set(packet, 1280);
        return packet.length;
      },
      _th09_peer_has_spectators: () => spectators.size > 0 ? 1 : 0,
      _th09_peer_send_spectator: length => {
        for (const viewer of spectators) viewer.incoming.push(bytes.slice(1280, 1280 + length));
        return 1;
      },
      _th09_peer_close: () => { peers.delete(self.side); spectators.delete(self); },
      _th09_network_info: () => 0,
      _th09_adonis_configure: mode => { calls.modes.push(mode); return 1; },
      _th09_rollback_begin: (...args) => { calls.begin.push(args); return 1; },
      _th09_rollback_pump: () => { ++calls.pumps; return 2; },
      _th09_rollback_info: () => 32,
      _th09_network_receive: (...args) => { calls.receive.push(args); return 1; },
      _th09_network_hash: () => 123,
      _th09_network_end: () => {},
      _th09_spectator_begin: (...args) => { calls.spectatorBegin.push(args); return 1; },
      // Match Application.cpp's exported ABI: both key words precede motion.
      _th09_spectator_feed: (frame, left, right, leftMode, leftX, leftY, rightMode, rightX, rightY) => {
        calls.spectatorFeed.push([frame, left, right, leftMode, leftX, leftY, rightMode, rightX, rightY]);
        return +([leftMode, rightMode].every(mode => Number.isInteger(mode) && mode >= 0 && mode <= 3));
      },
      _th09_spectator_frame: () => calls.spectatorFeed.length,
      _th09_spectator_end: () => {},
      _th09_loop_pause: value => calls.pauses.push(value),
    };
    return self;
  };
  const restore = () => {for(const timer of timers)clearInterval(timer);Object.assign(globalThis, previous);};
  return {core, restore};
}

async function connectPair(harness, inputDelay = 0, adonisMode = 0) {
  globalThis.fetch = async () => ({ok: true, json: async () => ({build: 'a'.repeat(24)})});
  globalThis.location = {href: 'https://example.test/runtime/th09/th09.html'};
  globalThis.document = {hidden: false};
  const leftCore = harness.core(), rightCore = harness.core();
  const left = new SharedNetplay(leftCore, {onStatus() {}, onClose() {}, onResult() {}});
  const right = new SharedNetplay(rightCore, {onStatus() {}, onClose() {}, onResult() {}});
  const options = (side, inputDelay = 0) => ({
    netplayUrl: `wss://example.test/netplay?room=th09mp-1234&run=1&player=${side}`,
    netplayPlayer: side, netplayPlayerCount: 2, netplaySeed: 1234, netplayDifficulty: 2,
    netplayInputDelay: inputDelay,
    netplayAdonisMode: adonisMode,
    netplayLoadouts: [{character: 3}, {character: 10}],
  });
  await Promise.all([left.connect(options(0, inputDelay)), right.connect(options(1, inputDelay))]);
  left.pump(); right.pump(); left.pump(); right.pump();
  assert.equal(left.active, true);
  assert.equal(right.active, true);
  return {left, right, leftCore, rightCore};
}

test('launcher and in-game entries share the TH09 room through the common peer transport', async () => {
  const box = harness();
  try {
    const {left, right, leftCore, rightCore} = await connectPair(box);
    assert.deepEqual(leftCore.calls.begin, [[1234, 0, 2, 3, 10, 0xaaaaaaab, 0xaaaaaaaa, (0xaaaaaaaa ^ (3<<16) ^ (10<<20) ^ (2<<24) ^ 0x09010000) >>> 0, 0]]);
    assert.deepEqual(rightCore.calls.begin, [[1234, 1, 2, 3, 10, 0xaaaaaaab, 0xaaaaaaaa, (0xaaaaaaaa ^ (3<<16) ^ (10<<20) ^ (2<<24) ^ 0x09010000) >>> 0, 0]]);
    left.input(6, 123, 1, .25, -.5);
    right.pump();
    assert.deepEqual(rightCore.calls.receive, [], "JS must not feed the old lockstep queue");
    assert.ok(leftCore.calls.pumps > 0 && rightCore.calls.pumps > 0);
    left.close(); right.close();
  } finally {
    box.restore();
  }
});

test('launcher input delay reaches the native rollback session', async () => {
  const box = harness();
  try {
    const {left, right, leftCore, rightCore} = await connectPair(box, 3);
    const baseAbi = (0xaaaaaaaa ^ (3<<16) ^ (10<<20) ^ (2<<24) ^ 0x09010000) >>> 0;
    assert.deepEqual(leftCore.calls.begin, [[1234, 0, 2, 3, 10, 0xaaaaaaab, 0xaaaaaaaa, baseAbi, 3]]);
    assert.deepEqual(rightCore.calls.begin, [[1234, 1, 2, 3, 10, 0xaaaaaaab, 0xaaaaaaaa, baseAbi, 3]]);
    assert.equal(globalThis.__eaglerNetplayInputDelayFrames, 3);
    left.close(); right.close();
  } finally { box.restore(); }
});

test('Adonis mode and all nine delay frames reach the native session without JS prediction', async () => {
  for (const mode of [1, 2]) {
    const box = harness();
    try {
      const {left,right,leftCore,rightCore} = await connectPair(box,9,mode);
      assert.deepEqual(leftCore.calls.modes,[mode]);
      assert.deepEqual(rightCore.calls.modes,[mode]);
      assert.equal(leftCore.calls.begin[0].at(-1),9);
      assert.equal(rightCore.calls.begin[0].at(-1),9);
      assert.deepEqual(leftCore.calls.receive,[]);
      assert.equal(globalThis.__eaglerNetplayAdonisMode,mode);
      left.close();right.close();
    } finally { box.restore(); }
  }
});

test('diagnostics report native rollback counters and result keeps pumping terminal ACKs', async () => {
  const box = harness();
  try {
    const {left, right, leftCore} = await connectPair(box);
    leftCore.HEAPU32.set([100,98,7,19,100,3,1,98],8);
    leftCore.HEAPU32[3]=100;left.frame();
    assert.equal(globalThis.__eaglerNetplayLanRollback,7);
    assert.equal(globalThis.__eaglerNetplayLanResimulated,19);
    assert.equal(globalThis.__eaglerNetplayLanConfirmed,98);
    const before=leftCore.calls.pumps;left.result();left.pump();
    assert.equal(leftCore.calls.pumps,before+1);
    assert.equal(left.active,false);
    left.close();right.close();
  } finally {box.restore();}
});

test('each peer keeps its transport open until its own native replay-save menu closes', async () => {
  const box = harness();
  try {
    const {left, right, leftCore} = await connectPair(box);
    let results = 0;
    left.onResult = () => ++results;
    left.result();
    assert.equal(left.finished, true);
    assert.equal(left.active, false);
    assert.equal(left.connected, true);
    assert.equal(right.connected, true);
    assert.equal(right.active, true);
    assert.equal(leftCore.calls.begin.length, 1);
    left.result();
    assert.equal(results, 1);
    right.result();
    assert.equal(right.finished, true);
    assert.equal(right.connected, true);
    left.close();
    assert.equal(right.connected, true);
    right.close();
  } finally {
    box.restore();
  }
});

test('an admitted spectator receives both players\' ordered keys and motion without contributing input', async () => {
  const box = harness();
  try {
    const {left, right} = await connectPair(box);
    const viewerCore = box.core();
    const viewer = new SharedNetplay(viewerCore, {onStatus() {}, onClose() {}, onResult() {}});
    await viewer.connect({
      netplayUrl: 'wss://example.test/netplay?room=th09mp-1234&run=1&spectator=c12345678',
      netplayPlayer: 0, netplayPlayerCount: 2, netplaySpectator: true,
      netplaySpectatorId: 'c12345678', netplaySeed: 1234, netplayDifficulty: 2,
      netplayLoadouts: [{character: 3}, {character: 10}],
    });
    viewer.pump();
    assert.equal(viewer.spectator, true);
    assert.deepEqual(viewerCore.calls.spectatorBegin, [[1234, 2, 3, 10]]);
    left.options.netplaySpectatorCount = 1;
    left.publish(0, 0x101, 0x204, 2, 350.5, 410.25, 3, 200.75, 150.5);
    viewer.pump();
    assert.deepEqual(viewerCore.calls.spectatorFeed, [[0, 0x101, 0x204,
      2, 350.5, 410.25, 3, 200.75, 150.5]]);
    assert.equal(viewer.connected, true);
    assert.equal(viewer.spectatorFrame, 1);
    assert.equal(viewerCore.calls.receive.length, 0);
    left.publish(1, 0, 0, 4, 0, 0, 0, 0, 0);viewer.pump();
    assert.equal(viewer.connected,false,"Unknown spectator motion mode must fail closed");
    viewer.close(); left.close(); right.close();
  } finally { box.restore(); }
});
