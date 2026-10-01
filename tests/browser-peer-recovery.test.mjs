import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';

// Execute the actual Emscripten transport body with deterministic browser
// stand-ins. Native C++ tests cannot exercise RTC's JavaScript health checks.
const source = readFileSync(new URL('../src/netplay/BrowserPeerTransport.cpp', import.meta.url), 'utf8');
const start = source.indexOf('EM_JS(int, eagler_peer_connect');
const bodyStart = source.indexOf('{', start);
const bodyEnd = source.indexOf('\n});\n\nEM_JS(int, eagler_spectator_connect', bodyStart);
assert(start >= 0 && bodyEnd > bodyStart);
const connect = new Function('relayUrlPtr', 'tagPtr', 'localPlayer', 'playerCount',
    source.slice(bodyStart + 1, bodyEnd));

const timers = [];
globalThis.setTimeout = (fn, delay) => { timers.push({ fn, delay }); return timers.length; };
globalThis.clearTimeout = () => {};
globalThis.setInterval = () => 1;
globalThis.clearInterval = () => {};
globalThis.location = { href: 'https://example.test/' };
globalThis.Module = { eaglerOptions: {} };
globalThis.HEAPU8 = new Uint8Array(256);
HEAPU8.set(new TextEncoder().encode('wss://example.test/room'), 0);
HEAPU8.set(new TextEncoder().encode('th10'), 100);
class WebSocketStub {
    static OPEN = 1;
    static CONNECTING = 0;
    static CLOSING = 2;
    constructor() { this.readyState = WebSocketStub.OPEN; }
    send() {}
    close() { this.readyState = 3; }
}
globalThis.WebSocket = WebSocketStub;

assert.equal(connect(0, 100, 0, 2), 1);
const state = globalThis.__eaglerPeerTransport;
let bytesSent = 100;
let bytesReceived = 100;
const peer = {
    pc: {
        connectionState: 'connected', iceConnectionState: 'connected',
        async getStats() {
            return new Map([
                ['transport', { type: 'transport', selectedCandidatePairId: 'pair' }],
                ['pair', { type: 'candidate-pair', state: 'succeeded', nominated: true,
                    bytesSent, bytesReceived }],
            ]);
        },
    },
    inputDc: { bufferedAmount: 40000 }, controlDc: { bufferedAmount: 0 },
    lastReceivedAt: Date.now() - 6000, lastInputSentAt: Date.now() - 1000,
    restartInFlight: false, restartAttempts: 0, recoveryTimer: null,
};
state.peers.set(1, peer);
state.setRoute('rtc');
let restarts = 0;
state.requestPeerIceRestart = () => { ++restarts; };

await state.schedulePeerRecovery(1, false, true);
assert.equal(timers.length, 1, 'a connected but backed-up peer must be probed');
bytesSent += 5000;
await timers.shift().fn();
assert.equal(restarts, 1, 'outbound bytes alone must not mask a silent peer');

peer.lastReceivedAt = Date.now() - 6000;
await state.schedulePeerRecovery(1, false, true);
assert.equal(timers.length, 1);
peer.lastReceivedAt = Date.now();
await timers.shift().fn();
assert.equal(restarts, 1, 'received data cancels the recovery');

peer.lastReceivedAt = Date.now() - 6000;
peer.inputDc.bufferedAmount = 0;
await state.schedulePeerRecovery(1, false, true);
assert.equal(timers.length, 0, 'idle connected peers must not restart');

peer.pc.connectionState = 'disconnected';
await state.schedulePeerRecovery(1);
assert.equal(timers.length, 1);
bytesSent += 5000;
await timers.shift().fn();
assert.equal(restarts, 2, 'disconnected recovery must also ignore outbound-only traffic');

state.requestPeerIceRestart = async () => { throw new Error('unexpected restart request'); };
peer.pc.connectionState = 'connected';
peer.pc.restartIce = () => {};
peer.pc.createOffer = async () => ({ type: 'offer', sdp: 'test' });
peer.pc.setLocalDescription = async offer => { peer.pc.localDescription = offer; };
state.setupChannel(1, peer.inputDc, 'input');
await state.restartPeerIce(1);
assert.equal(peer.restartInFlight, true);
assert.equal(peer.restartAttempts, 1);
assert.equal(timers.length, 1, 'a restart waits for actual inbound data');
state.handlePeerConnectionState(1);
assert.equal(peer.restartInFlight, true, 'connected ICE alone cannot finish recovery');
peer.inputDc.onmessage({ data: new Uint8Array([1]).buffer });
assert.equal(peer.restartInFlight, false);
assert.equal(peer.restartAttempts, 0, 'inbound data finishes recovery');

state.close();
console.log('browser peer silent recovery: PASS');
