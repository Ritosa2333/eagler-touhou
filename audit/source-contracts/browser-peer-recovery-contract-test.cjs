const { readFileSync } = require('node:fs');
const path = require('node:path');

const source = readFileSync(path.resolve(__dirname, '../../third_party/eagler-common/src/netplay/BrowserPeerTransport.cpp'), 'utf8');
const config = readFileSync(path.resolve(__dirname, '../../src/netplay/NetplayTransportConfig.hpp'), 'utf8');

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
requireText("eagler_peer_send_to", 'peer-addressed input send path');
requireText("state.peers.get(peerId)?.inputDc", 'targeted RTC input channel');
requireText("envelope[0] = 0xe7", 'targeted relay transport envelope');
requireText("controlLabel: `${tag}-control`", 'title-derived reliable control channel identity');
requireText("inputLabel: `${tag}-input`", 'title-derived fast input channel identity');
if (!config.includes('Tag[] = "th07"'))
  throw new Error('TH07 transport tag changed');

if (source.includes("this.signal?.close(1000, 'route selected')"))
  throw new Error('RTC route must retain signaling for ICE restart');

console.log('Browser peer recovery contract: PASS');
