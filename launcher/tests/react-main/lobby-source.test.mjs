import {expect,test} from 'vitest';
import {createLobbySource, normalizeLobbySource} from '../../app/services/lobby-source';

test('accepts local WebSocket addresses on HTTPS launchers and rejects insecure public endpoints', () => {
  expect(normalizeLobbySource(' wss://relay.example.invalid:8443/play ', 'https:'))
    .toBe('wss://relay.example.invalid:8443/play');
  expect(normalizeLobbySource('ws://relay.example.invalid:8080', 'http:'))
    .toBe('ws://relay.example.invalid:8080/');
  expect(normalizeLobbySource('ws://relay.example.invalid:8080', 'https:')).toBeNull();
  for (const host of ['10.44.3.7', '172.16.5.6', '172.31.5.6', '192.168.1.4', '169.254.11.1',
    '127.0.0.2', 'localhost', 'relay.local', '[::1]', '[fd00::1]', '[fe80::1]']) {
    expect(normalizeLobbySource(`ws://${host}:8080`, 'https:')).toBe(`ws://${host}:8080/`);
  }
  for (const host of ['172.15.1.1', '172.32.1.1', '192.168.1.4.example.invalid', 'relay.local.example.invalid', '[2001:db8::1]']) {
    expect(normalizeLobbySource(`ws://${host}:8080`, 'https:')).toBeNull();
  }
  expect(normalizeLobbySource('wss://a:b@relay.example.invalid', 'https:')).toBeNull();
  expect(normalizeLobbySource('wss://relay.example.invalid/?room=1234', 'https:')).toBeNull();
});

test('switching lobby sources separates saved rooms and keeps the public settings', () => {
  const values = new Map([['room','public']]);
  const storage = {
    getItem: key => values.get(key) ?? null,
    setItem: (key,value) => values.set(key,value),
    removeItem: key => values.delete(key),
  };
  const initial = createLobbySource(storage,'https:');
  expect(initial.relay).toBeNull();
  expect(initial.save('wss://relay.example.invalid')).toBe(true);
  const first = createLobbySource(storage,'https:');
  expect(first.scopedStorage.getItem('room')).toBeNull();
  first.scopedStorage.setItem('room','private');
  const session = first.memberId();
  expect(createLobbySource(storage,'https:').memberId()).toBe(session);
  expect(first.save('wss://other.example.invalid')).toBe(true);
  const second = createLobbySource(storage,'https:');
  expect(second.scopedStorage.getItem('room')).toBeNull();
  expect(second.memberId()).not.toBe(session);
  expect(second.save('')).toBe(true);
  expect(createLobbySource(storage,'https:').scopedStorage.getItem('room')).toBe('public');
});
