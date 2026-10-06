import test from 'node:test';
import assert from 'node:assert/strict';
import {SharedNetplay} from '../../sdl-runtime/shared-netplay.mjs';

// The two entries (Launcher card and the in-game title dialog) share one room
// and one gameplay transport; these tests cover the transport contract itself.
function harness() {
  const previous = {fetch: globalThis.fetch, location: globalThis.location, document: globalThis.document, __eaglerPeerTransport:globalThis.__eaglerPeerTransport, __eaglerNetplayCalibrationSuspended:globalThis.__eaglerNetplayCalibrationSuspended, setInterval: globalThis.setInterval};
  const timers=[];globalThis.setInterval=(...args)=>{const timer=previous.setInterval(...args);timers.push(timer);return timer;};
  const peers = new Map();
  const spectators = new Set();
  const core = () => {
    const buffer = new ArrayBuffer(2048), bytes = new Uint8Array(buffer);
    const calls = {begin: [], modes: [], receive: [], pauses: [], timing:[], spectatorBegin: [], spectatorFeed: [], pumps: 0,ends:0};
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
      _th09_peer_state: () => self.peerState ?? (peers.size === 2 ? 1 : 0),
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
      _th09_peer_spectator_state:()=>1,
      _th09_peer_stop_spectators:()=>{},
      _th09_peer_send_spectator: length => {
        for (const viewer of spectators) viewer.incoming.push(bytes.slice(1280, 1280 + length));
        return 1;
      },
      _th09_peer_close: () => { peers.delete(self.side); spectators.delete(self); },
      _th09_network_info: () => 0,
      _th09_adonis_configure: mode => { calls.modes.push(mode); return 1; },
      _th09_rollback_begin: (...args) => { calls.begin.push(args.slice(0,9)); return 1; },
      _th09_measured_begin:(...args)=>{
        calls.begin.push(args);self.HEAPU32.set([3,4,args[8],129,120,5000,6000,0,0,1,args[8],0,0,calls.modes.at(-1),120,120,1000,8000,4000,1000,9000,5000,129,120,16000,1,0],192);return 1;
      },
      _th09_startup_info:()=>768,
      _th09_rollback_pump: () => { ++calls.pumps; return self.pumpState ?? 2; },
      _th09_rollback_info: () => 32,
      _th09_network_receive: (...args) => { calls.receive.push(args); return 1; },
      _th09_network_hash: () => 123,
      _th09_network_end: () => ++calls.ends,
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

async function connectPair(harness, inputDelay = 0, adonisMode = 0, pending = false) {
  globalThis.fetch = async () => ({ok: true, json: async () => ({build: 'a'.repeat(24)})});
  globalThis.location = {href: 'https://example.test/runtime/th09/th09.html'};
  globalThis.document = {hidden: false};
  const leftCore = harness.core(), rightCore = harness.core();
  if(pending)leftCore.pumpState=rightCore.pumpState=1;
  const left = new SharedNetplay(leftCore, {onStatus() {}, onClose() {}, onResult() {},onTiming:v=>leftCore.calls.timing.push(v)});
  const right = new SharedNetplay(rightCore, {onStatus() {}, onClose() {}, onResult() {},onTiming:v=>rightCore.calls.timing.push(v)});
  const options = (side, inputDelay = 0) => ({
    netplayUrl: `wss://example.test/netplay?room=th09mp-1234&run=1&player=${side}`,
    netplayPlayer: side, netplayPlayerCount: 2, netplaySeed: 1234, netplayDifficulty: 2,
    netplayInputDelay: inputDelay,
    netplayAdonisMode: adonisMode,
    netplayLoadouts: [{character: 3}, {character: 10}],
  });
  await Promise.all([left.connect(options(0, inputDelay)), right.connect(options(1, inputDelay))]);
  left.pump(); right.pump(); left.pump(); right.pump();
  assert.equal(left.active, !pending);
  assert.equal(right.active, !pending);
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
      assert.equal(leftCore.calls.begin[0].at(-2),9);
      assert.equal(rightCore.calls.begin[0].at(-2),9);
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
    left.pump();
    viewer.pump();
    assert.deepEqual(viewerCore.calls.spectatorFeed, [[0, 0x101, 0x204,
      2, 350.5, 410.25, 3, 200.75, 150.5]]);
    assert.equal(viewer.connected, true);
    assert.equal(viewer.spectatorFrame, 1);
    assert.equal(viewerCore.calls.receive.length, 0);
    left.publish(1, 0, 0, 4, 0, 0, 0, 0, 0);left.pump();viewer.pump();
    assert.equal(viewer.connected,false,"Unknown spectator motion mode must fail closed");
    viewer.close(); left.close(); right.close();
  } finally { box.restore(); }
});

test('hidden measurement retries in place and exhaustion preserves room return',async()=>{
 const box=harness();let pair;
 try{
  pair=await connectPair(box,0,1,true);const {left,leftCore}=pair;
  leftCore.HEAPU32[193]=2;left.startupNoticeAt=0;globalThis.document.hidden=true;left.pump();
  assert.equal(left.connected,true);assert.equal(globalThis.__eaglerNetplayCalibrationSuspended,true);
  assert.equal(leftCore.calls.timing.at(-1).phase,'suspended');
  globalThis.document.hidden=false;leftCore.HEAPU32[193]=6;leftCore.HEAPU32[217]=2;left.startupNoticeAt=0;left.pump();
  assert.equal(leftCore.calls.timing.at(-1).phase,'retrying');assert.equal(leftCore.calls.begin.length,1);
  leftCore.HEAPU32[193]=7;leftCore.HEAPU32[217]=4;leftCore.HEAPU32[218]=7;left.startupNoticeAt=0;left.pump();
  assert.equal(leftCore.calls.timing.at(-1).phase,'unavailable');assert.equal(left.connected,true);assert.equal(left.disconnected,true);
  assert.equal(leftCore.calls.ends,0);const count=leftCore.calls.pumps;left.pump();assert.equal(leftCore.calls.pumps,count);
 }finally{pair?.left.close();pair?.right.close();box.restore();}
});
test('path recovery resumes the same session and ended connections pause without closing the world',async()=>{
 const box=harness();let pair;
 try{
  pair=await connectPair(box);const {left,leftCore}=pair;
  leftCore.peerState=2;leftCore.pumpState=1;left.pump();
  assert.equal(left.recovering,true);assert.equal(leftCore.calls.pauses.at(-1),1);assert.equal(left.connected,true);
  const pausedCount=leftCore.calls.pauses.length;left.pump();assert.equal(leftCore.calls.pauses.length,pausedCount);
  leftCore.peerState=1;leftCore.pumpState=2;left.pump();assert.equal(leftCore.calls.pauses.at(-1),0);assert.equal(left.recovering,false);
  assert.equal(leftCore.calls.begin.length,1);
  leftCore.peerState=3;left.pump();assert.equal(left.disconnected,true);assert.equal(leftCore.calls.pauses.at(-1),1);
  assert.equal(leftCore.calls.ends,0);assert.equal(left.connected,true);
 }finally{pair?.left.close();pair?.right.close();box.restore();}
});

test('ended connections keep the native replay-save menu usable',async()=>{
 const box=harness();let pair;
 try{
  pair=await connectPair(box);const {left,leftCore}=pair;left.result();
  const pauses=leftCore.calls.pauses.length;leftCore.peerState=3;left.pump();
  assert.equal(left.finished,true);assert.equal(left.disconnected,true);assert.equal(left.connected,true);
  assert.equal(leftCore.calls.pauses.length,pauses);assert.equal(leftCore.calls.ends,0);
 }finally{pair?.left.close();pair?.right.close();box.restore();}
});
