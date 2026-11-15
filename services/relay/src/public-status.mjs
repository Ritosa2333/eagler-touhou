import {createServer} from 'node:http';
import {isMultiplayerProductId} from '../../../common/contracts/product-catalog.mjs';

// Aggregate connected lobby participants only. Transport connections and
// unseated visitors are not a measure of active rooms or safe shutdown.
export function roomStatus(rooms, now = Date.now()) {
  const products = {}, online = socket => socket?.readyState === 1;
  for (const [id, room] of rooms) {
    const product = id.split('-')[0];
    if (!isMultiplayerProductId(product)) continue;
    const players = (room.lobby?.seats ?? []).filter(seat =>
      seat && online(room.lobbyClients?.get(seat.clientId))).length;
    const spectators = [...room.lobby?.spectators?.keys() ?? []].filter(client =>
      online(room.lobbyClients?.get(client))).length;
    if (!players && !spectators) continue;
    const row = products[product] ??= {rooms: 0, waiting: 0, playing: 0, players: 0, spectators: 0};
    row.rooms++;
    row.players += players;
    row.spectators += spectators;
    row[room.lobby.phase === 'lobby' ? 'waiting' : 'playing']++;
  }
  return {checkedAt: new Date(now).toISOString(), products};
}

export function startRoomStatus(rooms, port) {
  if (!Number.isSafeInteger(port) || port < 1 || port > 65535) throw Error('Invalid relay status port');
  const server = createServer((req, res) => {
    if (req.method !== 'GET' || req.url !== '/status') {
      res.writeHead(404).end();
      return;
    }
    res.setHeader('Content-Type', 'application/json');
    res.setHeader('Cache-Control', 'no-store');
    res.end(JSON.stringify(roomStatus(rooms)));
  });
  server.listen(port, '127.0.0.1');
  return server;
}
