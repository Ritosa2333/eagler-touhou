const assert = require('node:assert/strict');
const {readFileSync} = require('node:fs');
const {join} = require('node:path');
const vm = require('node:vm');

// Execute the production embedded JS, with channel objects in place of a real
// browser. This proves lane selection/bounds, not SCTP or live TURN delivery.
const source = readFileSync(join(__dirname, '../src/netplay/BrowserPeerTransport.cpp'), 'utf8');
function embedded(declaration, parameters, context) {
  const start = source.indexOf(declaration);
  assert.ok(start >= 0);
  const bodyStart = source.indexOf('{', start) + 1;
  const end = source.indexOf('\n});', bodyStart);
  assert.ok(end > bodyStart);
  return vm.runInNewContext('(function(' + parameters + '){' + source.slice(bodyStart, end) + '\n})', context);
}
const deliveries = [];
const control = {readyState: 'open', bufferedAmount: 0, send(bytes) { deliveries.push(Array.from(bytes)); }};
const state = {route: 'rtc', failed: false, closed: false, localPlayer: 0, playerCount: 2,
  peers: new Map([[1, {inputDc: {readyState: 'open', bufferedAmount: 300000}, controlDc: control}]])};
const heap = new Uint8Array(1024);heap.set([0, 11, 22, 33, 44]);
const context = {__eaglerPeerTransport: state, HEAPU8: heap};
const send = embedded('EM_JS(int, eagler_peer_send_repair_to,', 'peerId,data,size', context);
const buffered = embedded('EM_JS(double, eagler_peer_buffered_amount,', 'lane', context);

assert.equal(send(1, 1, 3), 1, 'healthy control repair works with a queued fast input lane');
assert.deepEqual(deliveries, [[11, 22, 33]]);
assert.equal(state.inputRepairSent, 1);
assert.equal(buffered(0), 300000);
assert.equal(buffered(1), 300000);
assert.equal(buffered(2), 0);
control.bufferedAmount = 32769;
assert.equal(buffered(0), 332769);
assert.equal(buffered(2), 32769);
assert.equal(send(1, 1, 3), 0, 'backpressured control lane still refuses repair');
assert.equal(deliveries.length, 1);
control.bufferedAmount = 0;
control.readyState = 'closed';
assert.equal(send(1, 1, 3), 0);
control.readyState = 'open';
control.send = () => { throw Error('simulated blocked association'); };
assert.equal(send(1, 1, 3), 0);
assert.equal(state.failed, false, 'repair does not take ownership of transport health');
state.route = 'relay';
state.relay = {bufferedAmount: 700};
assert.equal(send(1, 1, 3), 0, 'there is no separate reliable repair lane in relay mode');
assert.equal(buffered(0), 700);assert.equal(buffered(1), 0);assert.equal(buffered(2), 0);
assert.equal(state.inputRepairSent, 1);
state.route = 'rtc';
let accepted = 0;
control.bufferedAmount = 0;
control.send = bytes => { ++accepted;control.bufferedAmount += bytes.byteLength; };
for (let i = 0; i < 10000; ++i) send(1, 0, 1024);
assert.equal(accepted, 33, 'sustained blocked control does not append unbounded repairs');
assert.equal(control.bufferedAmount, 32768 + 1024, 'at most one maximum SessionChannel packet over its lane threshold');
assert.equal(state.failed, false);
console.log('eagler-common production RTC repair lane/backpressure: PASS');
