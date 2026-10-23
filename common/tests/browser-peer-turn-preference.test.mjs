import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';

// Execute the production EM_JS body, including signaling and route fallback.
const source = readFileSync(new URL('../src/netplay/BrowserPeerTransport.cpp', import.meta.url), 'utf8');
const start = source.indexOf('EM_JS(int, eagler_peer_connect,');
const body = source.indexOf('{', start), end = source.indexOf('\n});', body);
assert.ok(start >= 0 && end > body);

function fixture(preference, tag, seat, players) {
  class Socket {
    static OPEN = 1; static CONNECTING = 0; static CLOSING = 2;
    readyState = 1; sent = [];
    send(message) {this.sent.push(JSON.parse(message));}
    close() {this.readyState = 3;}
  }
  class Peer {
    constructor(config) {this.config = config; this.channels = [];}
    createDataChannel(label, options) {const dc = {label, options, close() {}}; this.channels.push(dc); return dc;}
    async createOffer(options) {this.offerOptions = options; return {type:'offer', sdp:'fixture'};}
    async setLocalDescription(description) {this.localDescription = description;}
    async getStats() {return new Map();}
    close() {this.closed = true;}
  }
  const heap = new Uint8Array(512), global = {};
  heap.set(new TextEncoder().encode('wss://example.test/room'), 0);
  heap.set(new TextEncoder().encode(tag), 100);
  const context = {globalThis:global, Module:{eaglerOptions:{netplayPreferTurn:preference}},
    location:{href:'https://example.test/'}, HEAPU8:heap, TextDecoder, URL,
    WebSocket:Socket, RTCPeerConnection:Peer, Uint8Array, ArrayBuffer, Blob,
    setTimeout:()=>1, clearTimeout(){}, setInterval:()=>1, clearInterval(){}};
  const connect = runInNewContext('(relayUrlPtr,tagPtr,localPlayer,playerCount,stageSpectatorHistory)=>' + source.slice(body,end+2), context);
  assert.equal(connect(0, 100, seat, players, 0), 1);
  return global.__eaglerPeerTransport;
}

for (const tag of ['th06','th07','th075','th08','th09','th10','th11','th15']) {
  for (const preference of [undefined, false, true]) for (const seat of [0, 1]) {
    const state = fixture(preference, tag, seat, 3);
    const iceServers = [{urls:'turn:example.test:3478', username:'fixture', credential:'fixture'}];
    state.signal.onmessage({data:JSON.stringify({type:'peers', peers:[0,1,2].filter(n=>n!==seat), iceServers})});
    // ensurePeer registers synchronously and sends offers after its awaits.
    await Promise.resolve(); await Promise.resolve();
    assert.equal(state.peers.size, 2);
    for (const peer of state.peers.values()) {
      assert.equal(peer.pc.config.iceTransportPolicy, preference === true ? 'relay' : 'all', `${tag} | P${seat+1}`);
      assert.equal(peer.pc.config.iceServers, state.iceServers, 'Use fresh credentials from the gameplay signaling service');
      if (peer.pc.channels.length) {
        assert.equal(peer.pc.channels[0].options.ordered, true);
        assert.equal(peer.pc.channels[1].options.ordered, false);
        assert.equal(peer.pc.channels[1].options.maxRetransmits, 0);
      }
    }
    // Failed TURN setup follows the existing server route decision.
    const [peerId, peer] = state.peers.entries().next().value;
    peer.pc.connectionState = 'failed';
    state.handlePeerConnectionState(peerId);
    assert.equal(state.signal.sent.at(-1).type, 'rtc-failed');
    state.signal.onmessage({data:JSON.stringify({type:'route', mode:'relay'})});
    assert.equal(state.route, 'relay');
    assert.equal(peer.pc.closed, true);
    state.relay.onmessage({data:new Uint8Array([7]).buffer});
    assert.equal(state.received[0][0], 7, 'WebSocket fallback still receives gameplay');
    state.close();
  }
}

const state = fixture(true, 'th09', 0, 2);
await state.ensurePeer(1);
state.setRoute('rtc');
await state.restartPeerIce(1);
assert.equal(state.peers.get(1).pc.config.iceTransportPolicy, 'relay', 'Recovery retains TURN preference');
assert.equal(state.peers.get(1).pc.offerOptions.iceRestart, true);
state.close();
console.log('browser peer TURN preference: PASS | 8 titles | 2 and 3 players | both roles | recovery and WebSocket fallback');
