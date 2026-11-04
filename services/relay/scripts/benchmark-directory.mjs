import { EventEmitter } from 'node:events';
import { performance } from 'node:perf_hooks';
import { createRoomDirectory } from '../src/room-directory.mjs';

// Synthetic local sockets exercise publication work without platform requests
// or production traffic. RSS includes the fixture and is not a server budget.
class Socket extends EventEmitter {
  readyState = 1;
  bufferedAmount = 0;
  send() {}
  close() { this.readyState = 3; this.emit('close'); }
  ping() { this.emit('pong'); }
}

const roomCount = 200, viewerCount = 200, publications = 5;
let summaryVisits = 0;
const rooms = new Map();
for (let i = 0; i < roomCount; i++) {
  const id = `${i % 2 ? 'th09mp' : 'th10mp'}-${1000 + i}`;
  const clientId = `host-${i}`, socket = new Socket();
  const seats = [{ clientId, name: 'H', ready: false }, null];
  rooms.set(id, {
    id, multiplayer: {}, createdAt: Date.now(), lastActivityAt: Date.now(),
    lobby: {
      visibility: 'public', playerCount: 2, phase: 'lobby', startSerial: 0,
      spectators: new Map(),
      get seats() { summaryVisits++; return seats; },
    },
    lobbyClients: new Map([[clientId, socket]]), lobbyDisconnectTimers: new Map(),
    clients: new Map(), runs: new Map(),
  });
}
const directory = createRoomDirectory({
  rooms, clearSeat: () => false, invalidateReady() {}, broadcast() {}, maybeDelete() {},
});
const sleep = ms => new Promise(resolve => setTimeout(resolve, ms));
try {
  for (let i = 0; i < viewerCount; i++) directory.connect(new Socket(), `viewer-${i}`);
  await sleep(200);
  summaryVisits = 0;
  const started = performance.now(), cpu = process.cpuUsage();
  for (let i = 0; i < publications; i++) {
    directory.changed();
    await sleep(150);
  }
  const elapsed = process.cpuUsage(cpu);
  console.log(JSON.stringify({
    rooms: roomCount, viewers: viewerCount, publications, summaryVisits,
    cpuMs: (elapsed.user + elapsed.system) / 1000,
    wallMs: performance.now() - started,
    rssMiB: process.memoryUsage().rss / 1048576,
  }, null, 2));
} finally {
  directory.close();
}
