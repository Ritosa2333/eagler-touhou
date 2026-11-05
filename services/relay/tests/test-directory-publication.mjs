import test from 'node:test';
import assert from 'node:assert/strict';
import { EventEmitter } from 'node:events';
import { createRoomDirectory } from '../src/room-directory.mjs';

class Socket extends EventEmitter {
  readyState = 1;
  bufferedAmount = 0;
  messages = [];
  send(value) { this.messages.push(JSON.parse(value)); }
  close() { this.readyState = 3; this.emit('close'); }
}

test('directory shares public work but not membership, and refreshes each publication', t => {
  t.mock.timers.enable({ apis: ['setTimeout'] });
  let visits = 0;
  const room = (id, clientId) => ({
    id, multiplayer: {}, createdAt: 1, lastActivityAt: Date.now(),
    lobby: {
      visibility: 'public', playerCount: 2, phase: 'lobby', startSerial: 0,
      spectators: new Map(),
      get seats() { visits++; return [{ clientId, name: clientId }, null]; },
    },
    lobbyClients: new Map([[clientId, new Socket()]]), lobbyDisconnectTimers: new Map(),
    clients: new Map(), runs: new Map(),
  });
  const rooms = new Map([
    ['th10mp-1111', room('th10mp-1111', 'client-a')],
    ['th09mp-2222', room('th09mp-2222', 'client-b')],
  ]);
  const directory = createRoomDirectory({
    rooms, clearSeat: () => false, invalidateReady() {}, broadcast() {}, maybeDelete() {},
  });
  try {
    directory.admit(rooms.get('th10mp-1111').lobbyClients.get('client-a'), 'th10mp-1111', 'client-a', 'member-a');
    directory.admit(rooms.get('th09mp-2222').lobbyClients.get('client-b'), 'th09mp-2222', 'client-b', 'member-b');
    const a = new Socket(), b = new Socket(), anonymous = new Socket();
    directory.connect(a, 'member-a');
    directory.connect(b, 'member-b');
    directory.connect(anonymous, 'unbound-member');
    const filtered = new Socket(), filteredAgain = new Socket();
    directory.connect(filtered, 'filtered-viewer');
    directory.connect(filteredAgain, 'filtered-viewer-again');
    const refresh = Buffer.from(JSON.stringify({ type: 'refresh', product: 'th09mp' }));
    filtered.emit('message', refresh, false);
    filteredAgain.emit('message', refresh, false);
    for (let i = 0; i < 20; i++) directory.connect(new Socket(), 'viewer-' + i);
    visits = 0;
    t.mock.timers.tick(100);
    assert.equal(visits, 2, 'summarize each room once for every publication');
    assert.equal(a.messages.at(-1).mine.code, '1111');
    assert.equal(b.messages.at(-1).mine.code, '2222');
    assert.notEqual(a.messages.at(-1).mine.recoveryToken, b.messages.at(-1).mine.recoveryToken);
    assert.equal(anonymous.messages.at(-1).mine, null);
    assert.deepEqual(filtered.messages.at(-1).rooms.map(row => row.code), ['2222']);
    assert.deepEqual(filteredAgain.messages.at(-1), filtered.messages.at(-1));
    const before = anonymous.messages.length;
    directory.changed();
    t.mock.timers.tick(100);
    assert.equal(anonymous.messages.length, before, 'unchanged rows are not sent again');
    anonymous.bufferedAmount = 128 * 1024;
    rooms.get('th10mp-1111').lobby.visibility = 'private';
    directory.changed();
    t.mock.timers.tick(100);
    assert.equal(anonymous.messages.length, before, 'congested viewers wait without advancing their snapshot');
    anonymous.bufferedAmount = 0;
    directory.changed();
    t.mock.timers.tick(100);
    assert.deepEqual(anonymous.messages.at(-1).rooms.map(row => row.code), ['2222']);
    assert.equal(a.messages.at(-1).mine.code, '1111', 'private membership is independent of public rows');
  } finally {
    directory.close();
  }
});
