import test from 'node:test';
import assert from 'node:assert/strict';
import {createTurnConfigReader} from '../src/turn-config.mjs';

test('TURN reader caches paired URLs and secrets and retains the last valid publication', () => {
  const fallback = {urls: ['turn:default.example:3478'], sharedSecret: 'fallback-secret'};
  let now = 0, reads = 0, content;
  const read = createTurnConfigReader('active-turn.json', fallback, () => {
    reads++;
    if (content === undefined) throw Error('file unavailable');
    return content;
  }, () => now);
  assert.deepEqual(read(), fallback);
  const first = {urls: ['turn:first.example:3478', 'turns:first.example:5349'], sharedSecret: 'first-secret'};
  content = JSON.stringify(first);
  now = 999;
  assert.deepEqual(read(), fallback);
  assert.equal(reads, 1);
  now = 1000;
  assert.deepEqual(read(), first);
  const invalid = [undefined, '{', 'null', '[]', JSON.stringify({urls: [], sharedSecret: 'bad'}),
    JSON.stringify({urls: ['https://invalid'], sharedSecret: 'bad'}),
    JSON.stringify({urls: ['turn:bad host'], sharedSecret: 'bad'}),
    JSON.stringify({urls: [123], sharedSecret: 'bad'}),
    JSON.stringify({urls: Array(9).fill('turn:too-many'), sharedSecret: 'bad'}),
    JSON.stringify({urls: ['turn:missing-secret']}),
    JSON.stringify({urls: ['turn:empty-secret'], sharedSecret: ''}),
    JSON.stringify({urls: ['turn:oversized'], sharedSecret: '密'.repeat(6000)})];
  for (const value of invalid) {
    content = value;
    now += 1000;
    assert.deepEqual(read(), first);
  }
  const second = {urls: ['turn:second.example:3478?transport=udp'], sharedSecret: 'second-secret'};
  content = JSON.stringify(second);
  now += 1000;
  assert.deepEqual(read(), second);
});

test('TURN reader preserves static configuration when no file is configured', () => {
  const fallback = {urls: ['turn:static.example:3478'], sharedSecret: ''};
  const read = createTurnConfigReader('', fallback, () => assert.fail('must not read a file'));
  assert.equal(read(), fallback);
  assert.equal(read(), fallback);
});
