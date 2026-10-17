// Test-process-only observer. Imported before the shared relay; does not
// change relay policy, wire bytes, delivery order, or production services.
import { createRequire } from 'node:module';
import { performance } from 'node:perf_hooks';
const require = createRequire(new URL('../../eagler-touhou/package.json', import.meta.url));
const { WebSocket } = require('ws');
let received = 0, forwarded = 0, previousTime = performance.now();
const originalEmit = WebSocket.prototype.emit;
const originalSend = WebSocket.prototype.send;
const originalSetSocket = WebSocket.prototype.setSocket;
const sockets = new Set();
WebSocket.prototype.setSocket = function (...args) {
  const result = originalSetSocket.apply(this, args);
  const ws = this;
  const state = {ws, lastRaw: performance.now(), maxRawGap: 0, rawChunks: 0};
  sockets.add(state);
  ws._socket.on('data', () => {
    const now = performance.now();
    state.maxRawGap = Math.max(state.maxRawGap, now - state.lastRaw);
    state.lastRaw = now;
    ++state.rawChunks;
  });
  ws.once('close', () => sockets.delete(state));
  return result;
};
function isInput(data) {
  if (!Buffer.isBuffer(data) && !(data instanceof Uint8Array)) return false;
  const offset = data[0] === 0xe7 ? 2 : 0;
  return data.length >= offset + 32 && data[offset] === 0x45 && data[offset + 5] === 1;
}
WebSocket.prototype.emit = function (name, ...args) {
  if (name === 'message' && isInput(args[0])) received++;
  return originalEmit.call(this, name, ...args);
};
WebSocket.prototype.send = function (data, ...args) {
  if (isInput(data)) forwarded++;
  return originalSend.call(this, data, ...args);
};
setInterval(() => {
  const now = performance.now();
  console.log(JSON.stringify({ probe: 'relay-timing', intervalMs: now - previousTime,
    received, forwarded, sockets: [...sockets].map(s => ({
      read: s.ws._socket?.bytesRead, written: s.ws._socket?.bytesWritten,
      rawAgeMs: now - s.lastRaw, maxRawGapMs: s.maxRawGap,
      chunks: s.rawChunks, paused: s.ws._socket?.isPaused(),
      receiverBuffered: s.ws._receiver?._bufferedBytes,
      receiverState: s.ws._receiver?._state,
      pendingWrite: s.ws._socket?.writableLength
    })) }));
  previousTime = now;
}, 1000).unref();
