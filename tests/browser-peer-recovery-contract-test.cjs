const { readFileSync } = require('node:fs');
const path = require('node:path');

const source = readFileSync(path.resolve(__dirname, '../src/netplay/BrowserPeerTransport.cpp'), 'utf8');

function requireText(text, label) {
  if (!source.includes(text)) throw new Error(`missing ${label}: ${text}`);
}

requireText("setTimeout(async () => {", 'delayed disconnected observation');
requireText("}, 2500);", '2.5 second recovery grace');
requireText("this.peerProgressed(before, after)", 'traffic and confirmed-frame progress guard');
requireText("peer.pc.connectionState === 'failed' || peer.pc.iceConnectionState === 'failed'", 'immediate failed-state handling');
requireText("peer.pc.restartIce()", 'browser ICE restart');
requireText("createOffer({ iceRestart: true })", 'restart offer generation');
requireText("type: 'ice-restart-request'", 'designated offerer restart request');
requireText("this.scheduleSignalReconnect()", 'persistent recovery signaling');
requireText("restartAttempts >= 2", 'bounded recovery attempts');
requireText("const family = address.includes(':') ? 'IPv6'", 'privacy-preserving selected address family');
requireText("th06_peer_send_to", 'peer-addressed input send path');
requireText("state.peers.get(peerId)?.inputDc", 'targeted RTC input channel');
requireText("envelope[0] = 0xe7", 'targeted relay transport envelope');
requireText("receivedHead", 'O(1) receive queue head');
requireText("state.receivedHead = head + 1", 'O(1) receive dequeue');
requireText("if (!this.route || this.route === 'rtc') this.receiveBinary(event.data)", 'RTC route-race buffering');
requireText("if (!state.route || state.route === 'relay') state.receiveBinary(event.data)", 'relay route-race buffering');
requireText("'th06-control'", 'TH06 reliable control channel identity');
requireText("'th06-input'", 'TH06 fast input channel identity');

if (source.includes('state.received.shift()'))
  throw new Error('gameplay receive queue must not use per-packet Array.shift()');

if (source.includes("this.signal?.close(1000, 'route selected')"))
  throw new Error('RTC route must retain signaling for ICE restart');

console.log('Browser peer recovery contract: PASS');
