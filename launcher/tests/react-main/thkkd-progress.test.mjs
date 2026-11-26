import assert from 'node:assert/strict';
import {test} from 'vitest';
import {IDBFactory} from 'fake-indexeddb';
import {parseThkkdProgress, readThkkdProgress} from '../../app/services/thkkd-progress';

test('THKKD progress reads final stages and distinguishes continued and one-credit clears', () => {
  const bytes = new Uint8Array(1316), words = new DataView(bytes.buffer);
  words.setUint32(56 + 5 * 4, 1, true);
  words.setUint32(56 + (63 + 21 + 14 + 5) * 4, 2, true);
  words.setUint32(56 + (252 + 42 + 14 + 6) * 4, 2, true);
  words.setUint32(56 + (126 + 5) * 4, 17, true);
  const progress = parseThkkdProgress(bytes);
  assert.equal(progress.length, 45);
  const get = (difficulty, character, shot) => progress.find(row => row.difficulty === difficulty && row.character === character && row.shot === shot);
  assert.equal(get('easy', 0, 0).cleared, true);assert.equal(get('easy', 0, 0).oneCreditClear, false);
  assert.equal(get('normal', 1, 2).oneCreditClear, true);
  assert.equal(get('extra', 2, 2).oneCreditClear, true);
  assert.equal(get('extra', 0, 0).cleared, false);
  assert.equal(get('hard', 0, 0).cleared, null);
  assert.ok(progress.every(row => row.playCount === null && row.bestScore === null));
});

test('THKKD parser observes byte offsets, ignores native trailing bytes and rejects short saves', () => {
  const buffer = new Uint8Array(1400);new DataView(buffer.buffer).setUint32(8 + 56 + 20, 2, true);
  assert.equal(parseThkkdProgress(buffer.subarray(8)).find(row => row.difficulty === 'easy' && row.character === 0 && row.shot === 0).oneCreditClear, true);
  assert.throws(() => parseThkkdProgress(buffer.subarray(0, 1315)), /Incomplete/);
});

test('THKKD local progress reads the native committed database without creating or writing saves', async () => {
  const factory = new IDBFactory();assert.equal(await readThkkdProgress(factory), undefined);assert.deepEqual(await factory.databases(), []);
  const bytes = new Uint8Array(1316);new DataView(bytes.buffer).setUint32(76, 2, true);
  await new Promise((resolve, reject) => {
    const request = factory.open('/savesthkkd', 21);
    request.onupgradeneeded = () => request.result.createObjectStore('FILE_DATA');
    request.onerror = () => reject(request.error);
    request.onsuccess = () => {
      const db = request.result, transaction = db.transaction('FILE_DATA', 'readwrite');
      transaction.objectStore('FILE_DATA').put({contents: bytes, mode: 0o100666, timestamp: new Date(1000)}, '/savesthkkd/th_kaikeidou01.dat');
      transaction.oncomplete = () => {db.close();resolve();};transaction.onerror = () => reject(transaction.error);
    };
  });
  const result = await readThkkdProgress(factory);assert.equal(result.modifiedAt, 1000);assert.equal(result.progress[0].oneCreditClear, true);
  assert.deepEqual(await readThkkdProgress(factory), result);
});
