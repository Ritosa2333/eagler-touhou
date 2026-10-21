import test from 'node:test';
import assert from 'node:assert/strict';
import {SharedNetplay} from '../../sdl-runtime/shared-netplay.mjs';
import {SpectatorTape} from '../../sdl-runtime/spectator-tape.mjs';

// The two entries (Launcher card and the in-game title dialog) share one room
// and one gameplay transport; these tests cover the transport contract itself.
function harness(timeline = false) {
  const previous = {fetch: globalThis.fetch, location: globalThis.location, document: globalThis.document, __eaglerPeerTransport:globalThis.__eaglerPeerTransport, __eaglerNetplayCalibrationSuspended:globalThis.__eaglerNetplayCalibrationSuspended, setInterval: globalThis.setInterval};
  const timers=[];globalThis.setInterval=(...args)=>{const timer=previous.setInterval(...args);timers.push(timer);return timer;};
  const peers = new Map();
  const spectators = new Set();
  const core = () => {
    const buffer = new ArrayBuffer(2048), bytes = new Uint8Array(buffer);
    const calls = {begin: [], modes: [], chargeModes:[], receive: [], pauses: [], timing:[], spectatorOutput: [], spectatorBegin: [], spectatorFeed: [], pumps: 0,ends:0};
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
        self.transport={};globalThis.__eaglerPeerTransport=self.transport;
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
        calls.spectatorOutput.push(bytes.slice(1280, 1280 + length));
        for (const viewer of spectators) viewer.incoming.push(bytes.slice(1280, 1280 + length));
        return 1;
      },
      _th09_peer_close: () => { peers.delete(self.side); spectators.delete(self); },
      _th09_network_info: () => 0,
      _th09_adonis_configure: mode => { calls.modes.push(mode); return 1; },
      _th09_local_charge_priority: mode => {calls.chargeModes.push(mode);return 1;},
      _th09_rollback_begin: (...args) => { calls.begin.push(args.slice(0,9)); return 1; },
      _th09_network_selection_begin:(...args)=>{calls.selection=args;return 1;},
      _th09_spectator_prepare:()=>{calls.preparedForSelection=true;return 1;},
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
    if (timeline) self._th09_spectator_control = () => 1;
    return self;
  };
  const restore = () => {for(const timer of timers)clearInterval(timer);Object.assign(globalThis, previous);};
  return {core, restore};
}

async function connectPair(harness, inputDelay = 0, adonisMode = 0, pending = false, inGameSelection = false) {
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
    ...(inGameSelection?{netplayInGameSelection:true}:{}),
    netplayLoadouts: [{character: 3}, {character: 10}],
  });
  await Promise.all([left.connect(options(0, inputDelay)), right.connect(options(1, inputDelay))]);
  left.pump(); right.pump(); left.pump(); right.pump();
  assert.equal(left.active, !pending);
  assert.equal(right.active, !pending);
  return {left, right, leftCore, rightCore};
}

test('active rollback processes arrivals before waking an already-due native tick',async()=>{
  const box=harness();let pair;
  try{
    pair=await connectPair(box);const {left,leftCore}=pair;
    const events=[];
    leftCore._th09_rollback_pump=()=>{events.push('receive');return 2;};
    leftCore._th09_rollback_wake=()=>events.push('wake');
    const receive=left.startupTransport.onReceive;
    assert.equal(typeof receive,'function','arrival callback survives the ready transition');
    receive();assert.deepEqual(events,['receive','wake']);
    events.length=0;left.pumping=true;receive();assert.deepEqual(events,[]);left.pumping=false;
    left.finished=true;receive();assert.deepEqual(events,['receive'],'terminal ACKs continue without a live wake');
    events.length=0;left.disconnected=true;receive();assert.deepEqual(events,[]);
    left.disconnected=false;left.close();assert.equal(left.startupTransport.onReceive,null);
    receive();assert.deepEqual(events,[],'a stale callback cannot wake a closed session');
  }finally{pair?.left.close();pair?.right.close();box.restore();}
});

test('pure delay receives on arrival without invoking a rollback live tick',async()=>{
  const box=harness();let pair;
  try{
    pair=await connectPair(box,2,1);const {left,leftCore}=pair;
    let wakes=0;leftCore._th09_rollback_wake=()=>++wakes;
    const before=leftCore.calls.pumps;left.startupTransport.onReceive();
    assert.equal(leftCore.calls.pumps,before+1);assert.equal(wakes,0);
  }finally{pair?.left.close();pair?.right.close();box.restore();}
});

test('native Replay compatibility disables local charge before rejoining an eight-digit room',async()=>{
  const box=harness();let pair;
  try{
    pair=await connectPair(box);
    const options={...pair.left.options,netplayUrl:'wss://example.test/netplay?room=th09mp-12345678&run=1&player=0',netplayDisableLocalChargePriority:true};
    pair.left.close();await pair.left.connect(options);
    assert.deepEqual(pair.leftCore.calls.chargeModes,[1,0]);
    assert.equal(pair.leftCore.calls.begin.length,2);
    pair.left.close();await assert.rejects(pair.left.connect({...options,netplayDisableLocalChargePriority:1}),/蓄力进度配置无效/);
  }finally{pair?.left.close();pair?.right.close();box.restore();}
});

test('game selection starts a neutral native menu and leaves room presets outside gameplay identity',async()=>{
  const box=harness();let pair;
  try{
    pair=await connectPair(box,0,0,false,true);
    assert.equal(pair.leftCore.calls.begin.length,0);
    assert.deepEqual(pair.leftCore.calls.selection,[1234,0,0xaaaaaaab,0xaaaaaaaa,(0xaaaaaaaa^0x09020001)>>>0,0,2]);
    assert.notEqual(pair.leftCore.calls.selection[4],(0xaaaaaaaa^0x09010000^(3<<16))>>>0,'Easy Sakuya/Reimu fixed-loadout entry cannot share the menu ABI');
    assert.equal(pair.left.betweenMatches,true);
  }finally{pair?.left.close();pair?.right.close();box.restore();}
});

test('spectator waiting for human selection does not time out or build the room preset world',async()=>{
  const box=harness(true);let pair,viewer;
  try{
    pair=await connectPair(box);const core=box.core();core._th09_spectator_playback_info=()=>1024;
    viewer=new SharedNetplay(core,{onStatus(){},onClose(){},onResult(){}});
    await viewer.connect({netplayUrl:'wss://example.test/?room=th09mp-1234&run=1&spectator=c12345678',
      netplayPlayerCount:2,netplaySpectator:true,netplaySpectatorId:'c12345678',netplaySeed:1234,
      netplayDifficulty:2,netplayLoadouts:[{character:3},{character:10}],netplayInGameSelection:true});
    viewer.spectatorReceiveAt=performance.now()-20000;viewer.pump();
    assert.equal(viewer.connected,true);assert.equal(core.calls.preparedForSelection,true);assert.deepEqual(core.calls.spectatorBegin,[]);
  }finally{viewer?.close();pair?.left.close();pair?.right.close();box.restore();}
});

test('spectator only applies the startup deadline before its first valid match checkpoint',()=>{
  const box=harness(true),performanceDescriptor=Object.getOwnPropertyDescriptor(globalThis,'performance');
  let now=0;Object.defineProperty(globalThis,'performance',{configurable:true,value:{now:()=>now}});
  const makeViewer=started=>{
    const core=box.core();core.peerState=1;let closed='';
    const viewer=new SharedNetplay(core,{onStatus(){},onClose:reason=>{closed=reason;},onResult(){}});
    Object.assign(viewer,{connected:true,spectator:true,timelineExperiment:true,prepared:true,routeReady:true,
      active:true,spectatorTape:new SpectatorTape(),spectatorReceiveAt:0});
    viewer.spectatorTape.started=started;
    return {viewer,get closed(){return closed;}};
  };
  let startup,idle;
  try{
    startup=makeViewer(false);
    now=14999;startup.viewer.pump();assert.equal(startup.viewer.connected,true,'initial admission remains within its deadline');
    now=15000;startup.viewer.pump();assert.equal(startup.viewer.connected,false,'no first checkpoint still fails closed');
    assert.match(startup.closed,/首个对局帧等待超时/);

    idle=makeViewer(true);now=30000;idle.viewer.pump();
    assert.equal(idle.viewer.connected,true,'a started match may intentionally have no new frames while its source is paused');
    idle.viewer.close();
  }finally{
    startup?.viewer.close();idle?.viewer.close();box.restore();
    if(performanceDescriptor)Object.defineProperty(globalThis,'performance',performanceDescriptor);
  }
});

test('launcher and in-game entries share the TH09 room through the common peer transport', async () => {
  const box = harness();
  try {
    const {left, right, leftCore, rightCore} = await connectPair(box);
    assert.deepEqual(leftCore.calls.begin, [[1234, 0, 2, 3, 10, 0xaaaaaaab, 0xaaaaaaaa, (0xaaaaaaaa ^ (3<<16) ^ (10<<20) ^ (2<<24) ^ 0x09010000) >>> 0, 0]]);
    assert.deepEqual(rightCore.calls.begin, [[1234, 1, 2, 3, 10, 0xaaaaaaab, 0xaaaaaaaa, (0xaaaaaaaa ^ (3<<16) ^ (10<<20) ^ (2<<24) ^ 0x09010000) >>> 0, 0]]);
    assert.deepEqual(leftCore.calls.chargeModes,[1]);assert.deepEqual(rightCore.calls.chargeModes,[1]);
    left.input(6, 123, 1, .25, -.5);
    right.pump();
    assert.deepEqual(rightCore.calls.receive, [], "JS must not feed the old lockstep queue");
    assert.ok(leftCore.calls.pumps > 0 && rightCore.calls.pumps > 0);
    left.close(); right.close();
  } finally {
    box.restore();
  }
});

test('stopping optional spectators keeps match statistics and repeated Retry bounded', async () => {
  const box=harness(true);let pair;
  try {
    pair=await connectPair(box);const {left,leftCore}=pair,reports=[];
    left.onMatch=value=>reports.push(value);
    left.stopSpectators('optional stream stopped');
    const at=leftCore._th09_rollback_info()/4;
    leftCore.HEAPU32[at+1]=3600;left.frame();
    assert.deepEqual(reports,[{start:0,frames:3600}]);
    left.endMatch(3700,1);left.endMatch(3700,1);
    assert.deepEqual(reports[1],{start:0,frames:3700,winner:1});
    for(let n=1;n<=1000;n++){
      left.beginMatch(n*4000,new Uint8Array(50));
      left.endMatch(n*4000+3700,n%2);
    }
    assert.equal(reports.length,2002);
    assert.deepEqual(reports[2],{start:4000,frames:0},'new match immediately resets its clock');
    assert.equal(left.spectatorPending.length,0,'stopped stream must retain no new start/end packets');
    assert.equal(left.connected,true);assert.equal(left.active,true);
  } finally {pair?.left.close();pair?.right.close();box.restore();}
});

test('experimental delayed spectator gets retained timing even without initial viewers', async () => {
  const box = harness(true); let pair, viewer;
  try {
    pair = await connectPair(box, 4, 1);
    const {left, leftCore} = pair;
    assert.equal(left.options.netplaySpectatorCount, undefined);
    assert.equal(leftCore.eaglerOptions.netplaySpectatorCount, 1);
    const timing = leftCore.calls.spectatorOutput.find(bytes => bytes.length === 40);
    assert.ok(timing, 'host must upload calibration metadata before anybody joins');
    const viewerCore = box.core();
    viewer = new SharedNetplay(viewerCore, {onStatus() {}, onClose() {}, onResult() {},
      onTiming: value => viewerCore.calls.timing.push(value)});
    await viewer.connect({
      netplayUrl: 'wss://example.test/netplay?room=th09mp-1234&run=1&spectator=c12345678',
      netplayPlayerCount: 2, netplaySpectator: true, netplaySpectatorId: 'c12345678',
      netplaySeed: 1234, netplayDifficulty: 2, netplayAdonisMode: 1, netplayInputDelay: 4,
      netplayLoadouts: [{character: 3}, {character: 10}],
    });
    viewer.pump(); assert.equal(viewer.active, false);
    viewer.receive(timing); assert.equal(viewer.active, true);
    left.publish(0, 1, 2, 0, 0, 0, 0, 0, 0); left.pump(); viewer.pump();
    assert.equal(viewer.connected, true); assert.equal(viewer.spectatorFrame, 1);
    assert.equal(viewerCore.calls.timing.at(-1).inputDelay, 4);
    // A malformed replay batch must close only the viewer.
    viewer.receive(new Uint8Array(47));
    assert.equal(viewer.connected, false); assert.equal(left.connected, true);
  } finally { viewer?.close(); pair?.left.close(); pair?.right.close(); box.restore(); }
});

test('paused viewer drains the previous match before either of two queued same-connection retries', async()=>{
  const box=harness(true);let pair,viewer;
  try{
    pair=await connectPair(box);const core=box.core(),state=core.HEAPU32.subarray(256,267),starts=[];
    core._th09_spectator_playback_info=()=>1024;core._th09_spectator_begin_buffer=()=>1080;
    core._th09_spectator_frame=()=>state[1];
    core._th09_spectator_finish=frame=>{if(frame!==state[2])return 0;state[10]=1;return 1;};
    core._th09_spectator_restart=(frame,size)=>{assert.equal(size,50);starts.push(frame);state.fill(0);state[1]=state[2]=state[8]=frame;return 1;};
    core._th09_spectator_control=action=>{if(action===2)state[7]=1;return 1;};
    const feed=core._th09_spectator_feed;core._th09_spectator_feed=(...args)=>{state[2]=args[0]+1;return feed(...args);};
    viewer=new SharedNetplay(core,{onStatus(){},onClose(){},onResult(){}});
    await viewer.connect({netplayUrl:'wss://example.test/netplay?room=th09mp-1234&run=1&spectator=c12345678',
      netplayPlayerCount:2,netplaySpectator:true,netplaySpectatorId:'c12345678',netplaySeed:1234,
      netplayDifficulty:2,netplayLoadouts:[{character:3},{character:10}]});viewer.pump();
    const packet=(kind,frame)=>{const bytes=new Uint8Array(kind==='begin'?74:kind==='end'?24:46);
      bytes.set(kind==='begin'?[84,57,66,71,1,0,2,0]:kind==='end'?[84,57,83,69,1,0,2,0]:[84,57,83,80,1,3,2,0]);
      new DataView(bytes.buffer).setUint32(8,frame,true);bytes.fill(0xaa,12,24);return bytes;};
    for(let n=0;n<3;n++)viewer.receive(packet('input',n));state[1]=1;state[5]=1;state[7]=1;
    viewer.receive(packet('end',3));viewer.receive(packet('begin',100));
    for(let n=100;n<105;n++)viewer.receive(packet('input',n));viewer.receive(packet('end',105));
    viewer.receive(packet('begin',200));viewer.receive(packet('input',200));
    viewer.frame();assert.deepEqual(starts,[]);assert.equal(state[1],1);assert.equal(state[5],1);
    assert.equal(viewer.spectatorFrame,3);assert.equal(viewer.spectatorTape.begins,2);assert.equal(viewer.connected,true);
    state[5]=0;state[1]=2;viewer.frame();assert.deepEqual(starts,[]);
    state[1]=3;viewer.frame();assert.deepEqual(starts,[100]);assert.equal(state[1],100);assert.equal(state[2],105);
    assert.equal(state[7],1,'follow-live intent survives a complete match transition');
    state[1]=105;state[5]=1;viewer.frame();assert.deepEqual(starts,[100]);assert.equal(viewer.connected,true);
    state[5]=0;viewer.frame();assert.deepEqual(starts,[100,200]);assert.equal(viewer.spectatorFrame,201);
    assert.deepEqual(core.calls.spectatorFeed.map(row=>row[0]),[0,1,2,100,101,102,103,104,200]);
    assert.equal(pair.left.connected,true);assert.equal(pair.right.connected,true);
  }finally{viewer?.close();pair?.left.close();pair?.right.close();box.restore();}
});

test('viewer waits between matches and drains the final tail after the connection ends', async () => {
  const box=harness(true);let pair,viewer,results=0,closed=0;
  try {
    pair=await connectPair(box);const core=box.core(),state=core.HEAPU32.subarray(256,267),starts=[];
    core._th09_spectator_playback_info=()=>1024;core._th09_spectator_begin_buffer=()=>1080;
    core._th09_spectator_frame=()=>state[1];
    core._th09_spectator_finish=frame=>{if(frame!==state[2])return 0;state[10]=1;return 1;};
    core._th09_spectator_restart=(frame,size)=>{assert.equal(size,50);starts.push(frame);state.fill(0);state[1]=state[2]=state[8]=frame;return 1;};
    const feed=core._th09_spectator_feed;core._th09_spectator_feed=(...args)=>{state[2]=args[0]+1;return feed(...args);};
    viewer=new SharedNetplay(core,{onStatus(){},onClose(){++closed;},onResult(){++results;}});
    await viewer.connect({netplayUrl:'wss://example.test/netplay?room=th09mp-1234&run=1&spectator=c12345678',
      netplayPlayerCount:2,netplaySpectator:true,netplaySpectatorId:'c12345678',netplaySeed:1234,
      netplayDifficulty:2,netplayLoadouts:[{character:3},{character:10}]});viewer.pump();
    const packet=(kind,frame)=>{const bytes=new Uint8Array(kind==='begin'?74:kind==='end'?24:46);
      bytes.set(kind==='begin'?[84,57,66,71,1,0,2,0]:kind==='end'?[84,57,83,69,1,0,2,0]:[84,57,83,80,1,3,2,0]);
      new DataView(bytes.buffer).setUint32(8,frame,true);bytes.fill(0xaa,12,24);return bytes;};
    for(let n=0;n<3;n++)viewer.receive(packet('input',n));viewer.receive(packet('end',3));state[1]=3;
    for(let n=0;n<3;n++)viewer.frame();
    assert.equal(viewer.connected,true,'a match end is not a session end');assert.equal(results,0);assert.equal(closed,0);
    viewer.receive(packet('begin',100));viewer.receive(packet('input',100));viewer.receive(packet('end',101));
    assert.deepEqual(starts,[100]);assert.equal(viewer.spectatorFrame,101);
    state[5]=1;viewer.connectionEnded();viewer.frame();
    assert.equal(core.calls.pauses.at(-1),0,'transport loss must not suspend confirmed playback');
    assert.equal(viewer.connected,true,'paused viewers keep their final tail');
    state[5]=0;viewer.frame();assert.equal(viewer.connected,true,'unplayed final input must not be discarded');
    state[1]=101;viewer.frame();viewer.frame();
    assert.equal(viewer.connected,false);assert.equal(results,1);assert.equal(closed,1);
    assert.equal(pair.left.connected,true);assert.equal(pair.right.connected,true);
  } finally {viewer?.close();pair?.left.close();pair?.right.close();box.restore();}
});

for(const transportEnd of [-1,3]) test(`viewer drains buffered final packets across bounded pumps after transport state ${transportEnd}`,async()=>{
  const box=harness(true);let pair,viewer,results=0;
  try {
    pair=await connectPair(box);const core=box.core(),state=core.HEAPU32.subarray(256,267);
    core._th09_spectator_playback_info=()=>1024;core._th09_spectator_frame=()=>state[1];
    core._th09_spectator_finish=frame=>{if(frame!==state[2])return 0;state[10]=1;return 1;};
    const feed=core._th09_spectator_feed;core._th09_spectator_feed=(...args)=>{state[2]=args[0]+1;return feed(...args);};
    viewer=new SharedNetplay(core,{onStatus(){},onClose(){},onResult(){++results;}});
    await viewer.connect({netplayUrl:'wss://example.test/netplay?room=th09mp-1234&run=1&spectator=c12345678',
      netplayPlayerCount:2,netplaySpectator:true,netplaySpectatorId:'c12345678',netplaySeed:1234,
      netplayDifficulty:2,netplayLoadouts:[{character:3},{character:10}]});viewer.pump();
    for(let frame=0;frame<=129;frame++){
      const end=frame===129,bytes=new Uint8Array(end?24:46);
      bytes.set(end?[84,57,83,69,1,0,2,0]:[84,57,83,80,1,3,2,0]);
      new DataView(bytes.buffer).setUint32(8,frame,true);bytes.fill(0xaa,12,24);core.incoming.push(bytes);
    }
    core.peerState=transportEnd;viewer.pump();
    assert.equal(core.incoming.length,2,'one pump remains bounded to 128 packets');
    assert.equal(viewer.connected,true);assert.equal(viewer.disconnected,false);
    viewer.pump();assert.equal(core.incoming.length,0);assert.equal(viewer.disconnected,true);
    assert.equal(viewer.spectatorFrame,129);assert.equal(core.calls.pauses.at(-1),0);
    state[1]=127;viewer.frame();assert.equal(viewer.connected,true);assert.equal(results,0);
    state[1]=129;viewer.frame();assert.equal(viewer.connected,false);assert.equal(results,1);
    assert.equal(pair.left.connected,true);assert.equal(pair.right.connected,true);
  }finally{viewer?.close();pair?.left.close();pair?.right.close();box.restore();}
});

test('TH09 rollback preserves the room manual delay at the native boundary', async () => {
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
      const effectiveMode=mode===2?0:mode;
      assert.deepEqual(leftCore.calls.modes,[effectiveMode]);
      assert.deepEqual(rightCore.calls.modes,[effectiveMode]);
      assert.equal(leftCore.calls.begin[0][8],9);
      assert.equal(rightCore.calls.begin[0][8],9);
      assert.deepEqual(leftCore.calls.receive,[]);
      assert.equal(globalThis.__eaglerNetplayAdonisMode,effectiveMode);
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

test('return-to-character-selection publishes the last confirmed input and end before immediate close', async () => {
  const box = harness(true);
  let pair;
  try {
    pair = await connectPair(box);
    const {left, leftCore} = pair;
    left.publish(0, 8, 0, 0, 0, 0, 0, 0, 0);
    // The native result callback precedes confirmed publication of this tick.
    left.result();
    left.publish(1, 1, 0, 0, 0, 0, 0, 0, 0);
    left.frame();
    const output = leftCore.calls.spectatorOutput;
    assert.deepEqual(output.map(p => [p.length, new DataView(p.buffer).getUint32(8, true)]),
      [[46, 0], [46, 1], [24, 2]]);
    assert.equal(left.spectatorEndSent, true);
    left.frame();
    assert.equal(output.length, 3, 'terminal output is not duplicated');
    left.close();
    assert.equal(output.length, 3);
  } finally { pair?.left.close(); pair?.right.close(); box.restore(); }
});


test('native stop drains a bounded publisher backlog before closing its spectator stream',async()=>{
  const box=harness(true);let pair;
  try{
    pair=await connectPair(box);const {left,leftCore}=pair;
    for(let frame=0;frame<100;frame++)left.publish(frame,0,0,0,0,0,0,0,0);
    left.result();left.frame();
    assert.equal(left.spectatorEndSent,false,'one frame callback keeps its normal 32-packet limit');
    await left.finishSpectatorOutput();
    assert.equal(left.spectatorEndSent,true);
    assert.equal(leftCore.calls.spectatorOutput.length,101);
    assert.equal(new DataView(leftCore.calls.spectatorOutput.at(-1).buffer).getUint32(8,true),100);
  }finally{pair?.left.close();pair?.right.close();box.restore();}
});
