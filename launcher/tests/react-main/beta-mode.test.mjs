import {beforeAll, test} from 'vitest';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
import {fileURLToPath} from 'node:url';

let testFeatures, identity;
beforeAll(async () => {
  const result = await build({stdin: {resolveDir: fileURLToPath(new URL('../../', import.meta.url)), contents: `
    export * from './src/launcher/test-features.mts';
    export * from './src/launcher/multiplayer-identity.mts';
  `}, bundle: true, write: false, format: 'esm', platform: 'node'});
  const loaded = await import('data:text/javascript;base64,' + Buffer.from(result.outputFiles[0].text).toString('base64'));
  testFeatures = loaded;
  identity = loaded;
});

class MemoryStorage {
  values;
  constructor(entries = []) {this.values = new Map(entries);}
  getItem(key) {return this.values.get(key) ?? null;}
  setItem(key, value) {this.values.set(key, String(value));}
  removeItem(key) {this.values.delete(key);}
}

test('the visibility preference reloads the same room URL and keeps multiplayer identity', () => {
  const roomUrl = 'https://example.test/app/lobby.html?lang=zh-CN&room=2468&game=th06mp&run=run-1&member=m0123456789abcdef#room';
  const localStorage = new MemoryStorage([
    ['eagler-touhou-mp-display-name-v1', 'Alice'],
    ['eagler-touhou-mp-member-v1', 'm0123456789abcdef'],
  ]);
  const clientKey = identity.multiplayerLobbyClientStorageKey('th06mp');
  const sessionStorage = new MemoryStorage([[clientKey, 'client_12345678']]);
  const navigations = [];
  const owner = {localStorage, sessionStorage, location: {href: roomUrl, reload() {navigations.push(this.href);}}};
  const before = identity.createMultiplayerIdentityStore({persistentStorage: localStorage, sessionStorage});
  const displayName = before.loadDisplayName(), clientId = before.lobbyClientId('th06mp');
  const memberId = localStorage.getItem('eagler-touhou-mp-member-v1');

  assert.equal(testFeatures.testFeaturesEnabled(owner), false);
  testFeatures.reloadTestFeatures(owner, true);
  assert.equal(testFeatures.testFeaturesEnabled(owner), true);
  assert.equal(owner.location.href, roomUrl);
  assert.deepEqual(navigations, [roomUrl]);
  assert.equal(localStorage.getItem(testFeatures.TEST_FEATURES_KEY), '1');

  const after = identity.createMultiplayerIdentityStore({persistentStorage: localStorage, sessionStorage});
  assert.equal(after.loadDisplayName(), displayName);
  assert.equal(after.lobbyClientId('th06mp'), clientId);
  assert.equal(localStorage.getItem('eagler-touhou-mp-member-v1'), memberId);
  assert.equal(sessionStorage.getItem(clientKey), clientId);

  testFeatures.reloadTestFeatures(owner, false);
  assert.equal(testFeatures.testFeaturesEnabled(owner), false);
  assert.equal(owner.location.href, roomUrl);
  assert.deepEqual(navigations, [roomUrl, roomUrl]);
  assert.equal(localStorage.getItem(testFeatures.TEST_FEATURES_KEY), null);
  assert.equal(after.lobbyClientId('th06mp'), clientId);
});

test('storage denial leaves the visibility preference disabled', () => {
  const owner = {localStorage: {getItem() {throw new Error('blocked');}}};
  assert.equal(testFeatures.testFeaturesEnabled(owner), false);
});
