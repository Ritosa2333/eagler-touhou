import assert from 'node:assert/strict';
import {test} from 'vitest';
import {IDBFactory, IDBKeyRange} from 'fake-indexeddb';
import {collectLocalSaveFiles, readLocalSaveFiles} from '../../app/services/local-save-storage';
import {createReplayStorageReader} from '../../app/services/replay-storage';

function seededDb(factory, name, entries, version = 21, storeName = 'FILE_DATA', keyRoot = name) {
  return new Promise((resolveDb, reject) => {
    const request = factory.open(name, version);
    request.onupgradeneeded = () => request.result.createObjectStore(storeName);
    request.onerror = () => reject(request.error);
    request.onsuccess = () => {
      const database = request.result;
      const transaction = database.transaction(storeName, 'readwrite');
      const store = transaction.objectStore(storeName);
      for (const [path, contents] of entries) {
        store.put({contents, mode: 0o100666, timestamp: 1}, `${keyRoot}/${path}`);
      }
      transaction.oncomplete = () => { database.close(); resolveDb(); };
      transaction.onabort = () => { database.close(); reject(transaction.error); };
    };
  });
}

function signedBytes(bytes) {
  const backing = new ArrayBuffer(bytes.length + 9);
  new Uint8Array(backing, 4, bytes.length).set(bytes);
  return new Int8Array(backing, 4, bytes.length);
}

function fileBytes(file) { return file.arrayBuffer().then(buffer => new Uint8Array(buffer)); }
function countingFactory(factory) {
  let opens = 0;
  return {factory: new Proxy(factory, {get(target, key) {
    if (key === 'open') return (...args) => { opens++; return target.open(...args); };
    const value = Reflect.get(target, key, target);
    return typeof value === 'function' ? value.bind(target) : value;
  }}), opened: () => opens};
}

test('missing databases stay absent; old and unknown schemas are rejected', async () => {
  const factory = new IDBFactory();
  assert.deepEqual(await readLocalSaveFiles('th06', ['score.dat'], 'ja', factory), []);
  assert.deepEqual(await factory.databases(), []);

  await seededDb(factory, '/savesth06', [], 20);
  await assert.rejects(readLocalSaveFiles('th06', ['score.dat'], 'ja', factory), /无法识别本地存储版本/);
  assert.equal((await factory.databases()).find(db => db.name === '/savesth06').version, 20);

  await seededDb(factory, '/savesth07', [], 21, 'NOT_FILE_DATA');
  await assert.rejects(readLocalSaveFiles('th07', ['score.dat'], 'ja', factory), /无法识别本地存储版本/);

  await seededDb(factory, '/savesth08', [], 22);
  await assert.rejects(readLocalSaveFiles('th08', ['score.dat'], 'ja', factory), /无法识别本地存储版本/);
  assert.equal((await factory.databases()).find(db => db.name === '/savesth08').version, 22);
});

test('save names reject traversal and duplicates before opening IndexedDB', async () => {
  const factory = countingFactory(new IDBFactory());
  for (const names of [
    ['../score.dat'], ['/score.dat'], ['a/../b'], ['a//b'], ['a\\b'], [''],
    ['score.dat', 'score.dat'], Array.from({length: 13}, (_, i) => `f${i}.dat`), ['x'.repeat(129)],
  ]) {
    await assert.rejects(readLocalSaveFiles('th06', names, 'ja', factory.factory));
  }
  assert.equal(factory.opened(), 0);
});

test('signed IDBFS bytes preserve the exact typed-array slice; optional files can be missing', async () => {
  const factory = new IDBFactory();
  await seededDb(factory, '/savesth06', [
    ['score.dat', signedBytes(new Uint8Array([0, 127, 128, 255]))],
    ['score06.dat', signedBytes(new Uint8Array([22, 33]))],
  ]);
  const files = await readLocalSaveFiles('th06', ['score.dat', 'score06.dat', 'spell_capture.dat'], 'ja', factory);
  assert.deepEqual(files.map(file => file.name), ['score.dat', 'score06.dat']);
  assert.deepEqual([...await fileBytes(files[0])], [0, 127, 128, 255]);
  assert.deepEqual([...await fileBytes(files[1])], [22, 33]);
});

test('per-file and aggregate byte limits accept 4/8 MiB and reject overflow', async () => {
  const exact = new IDBFactory();
  const four = new Uint8Array(4 * 1024 * 1024);
  await seededDb(exact, '/savesth06', [['score.dat', four], ['score06.dat', four]]);
  const accepted = await readLocalSaveFiles('th06', ['score.dat', 'score06.dat'], 'ja', exact);
  assert.equal(accepted.length, 2);
  assert.equal(accepted.reduce((sum, file) => sum + file.size, 0), 8 * 1024 * 1024);

  const fileTooLarge = new IDBFactory();
  await seededDb(fileTooLarge, '/savesth06', [['score.dat', new Uint8Array(4 * 1024 * 1024 + 1)]]);
  await assert.rejects(readLocalSaveFiles('th06', ['score.dat'], 'ja', fileTooLarge), /upload limit/);

  const totalTooLarge = new IDBFactory();
  const three = new Uint8Array(3 * 1024 * 1024);
  await seededDb(totalTooLarge, '/savesth06', [['a.dat', three], ['b.dat', three], ['c.dat', three]]);
  await assert.rejects(readLocalSaveFiles('th06', ['a.dat', 'b.dat', 'c.dat'], 'ja', totalTooLarge), /upload limit/);
});

test('TH10 language selects only its preferred score alias and preserves an available fallback', async () => {
  const factory = new IDBFactory();
  await seededDb(factory, '/savesth10', [
    ['jp/scoreth10.dat', new Uint8Array([10])], ['jp/scoreth10c.dat', new Uint8Array([11])],
    ['chs/scoreth10.dat', new Uint8Array([20])], ['chs/scoreth10c.dat', new Uint8Array([21])],
  ]);
  const jp = await readLocalSaveFiles('th10', ['scoreth10.dat', 'scoreth10c.dat'], 'ja', factory);
  const chs = await readLocalSaveFiles('th10', ['scoreth10.dat', 'scoreth10c.dat'], 'lang_zh-hans', factory);
  assert.deepEqual(jp.map(file => file.name), ['scoreth10.dat']);
  assert.deepEqual([...await fileBytes(jp[0])], [10]);
  assert.deepEqual(chs.map(file => file.name), ['scoreth10c.dat']);
  assert.deepEqual([...await fileBytes(chs[0])], [21]);

  const fallback = new IDBFactory();
  await seededDb(fallback, '/savesth10', [['chs/scoreth10.dat', new Uint8Array([30])]]);
  const onlyAvailable = await readLocalSaveFiles('th10', ['scoreth10.dat', 'scoreth10c.dat'], 'lang_zh-hans', fallback);
  assert.deepEqual(onlyAvailable.map(file => file.name), ['scoreth10.dat']);
});

test('normal and multiplayer saves use separate stores; TH12 MP uses its own DB with shared root policy', async () => {
  const factory = new IDBFactory();
  await seededDb(factory, '/savesth06', [['score.dat', new Uint8Array([6])]]);
  await seededDb(factory, '/savesth06-multiplayer', [['score.dat', new Uint8Array([60])]]);
  assert.deepEqual([...await fileBytes((await readLocalSaveFiles('th06', ['score.dat'], 'ja', factory))[0])], [6]);
  assert.deepEqual([...await fileBytes((await readLocalSaveFiles('th06mp', ['score.dat'], 'ja', factory))[0])], [60]);

  await seededDb(factory, '/savesth12', [['scoreth12.dat', new Uint8Array([12])]]);
  await seededDb(factory, '/savesth12mp', [['scoreth12.dat', new Uint8Array([120])]], 21, 'FILE_DATA', '/savesth12');
  assert.deepEqual([...await fileBytes((await readLocalSaveFiles('th12', ['scoreth12.dat'], 'ja', factory))[0])], [12]);
  assert.deepEqual([...await fileBytes((await readLocalSaveFiles('th12mp', ['scoreth12.dat'], 'ja', factory))[0])], [120]);
});

test('test features read saves from the shared IDBFS database', async () => {
  const factory = new IDBFactory();
  await seededDb(factory, '/savesth06', [['score.dat', new Uint8Array([6])]]);
  assert.deepEqual([...await fileBytes((await readLocalSaveFiles('th06', ['score.dat'], 'ja', factory))[0])], [6]);
  const collected = await collectLocalSaveFiles({
    product: 'th06', names: ['score.dat'], language: 'ja', indexedDB: factory, runtime: null,
  });
  assert.deepEqual([...await fileBytes(collected[0])], [6]);
  assert.deepEqual((await factory.databases()).map(database => database.name), ['/savesth06']);
});

function runtimeFor({game = 'th06', variant = 'normal', epoch = 7, session} = {}) {
  return {
    getSnapshot: () => ({ready: true, game, runtimeVariant: variant, epoch}),
    withFileSession: async (receivedGame, operation, options) => session(receivedGame, operation, options),
  };
}

test('a matching ready runtime is synced first, read-only, and receives unsigned bytes', async () => {
  const factory = countingFactory(new IDBFactory()), order = [];
  const runtime = runtimeFor({session: async (game, operation, options) => {
    assert.equal(game, 'th06');
    assert.deepEqual(options, {readOnly: true, runtimeVariant: 'normal', epoch: 7});
    return operation({
      sync: async () => {order.push('sync');},
      send: async (_type, {path}) => {order.push(path); return {bytes: [0, 127, 128, 255]};},
    });
  }});
  const files = await collectLocalSaveFiles({product: 'th06', names: ['score.dat', 'score06.dat'], language: 'ja', indexedDB: factory.factory, runtime});
  assert.deepEqual(order, ['sync', 'score.dat', 'score06.dat']);
  assert.deepEqual([...await fileBytes(files[0])], [0, 127, 128, 255]);
  assert.equal(factory.opened(), 0);
});

test('runtime sync errors, non-missing read errors, and epoch supersession never fall back to stale IDBFS', async () => {
  const indexedDB = new IDBFactory();
  await seededDb(indexedDB, '/savesth06', [['score.dat', new Uint8Array([99])]]);
  const factory = countingFactory(indexedDB);

  const syncFailure = runtimeFor({session: async (_game, operation) => operation({
    sync: async () => {throw new Error('sync failed');}, send: async () => ({bytes: [1]}),
  })});
  await assert.rejects(collectLocalSaveFiles({product: 'th06', names: ['score.dat'], language: 'ja', indexedDB: factory.factory, runtime: syncFailure}), /sync failed/);
  assert.equal(factory.opened(), 0);

  const readFailure = runtimeFor({session: async (_game, operation) => operation({
    sync: async () => {}, send: async () => {throw Object.assign(new Error('permission denied'), {errno: 13});},
  })});
  await assert.rejects(collectLocalSaveFiles({product: 'th06', names: ['score.dat'], language: 'ja', indexedDB: factory.factory, runtime: readFailure}), /permission denied/);
  assert.equal(factory.opened(), 0);

  const changedGeneration = runtimeFor({session: async (_game, _operation, options) => {
    assert.equal(options.epoch, 7);
    throw new Error('runtime generation changed');
  }});
  await assert.rejects(collectLocalSaveFiles({product: 'th06', names: ['score.dat'], language: 'ja', indexedDB: factory.factory, runtime: changedGeneration}), /generation changed/);
  assert.equal(factory.opened(), 0);

  const supersededDuringSync = runtimeFor({session: async (_game, operation) => operation({
    sync: async () => {throw new Error('runtime epoch changed during sync');}, send: async () => ({bytes: [1]}),
  })});
  await assert.rejects(collectLocalSaveFiles({product: 'th06', names: ['score.dat'], language: 'ja', indexedDB: factory.factory, runtime: supersededDuringSync}), /epoch changed/);
  assert.equal(factory.opened(), 0);
});

test('runtime skips only errno 44 and applies the same per-file/aggregate limits', async () => {
  const factory = countingFactory(new IDBFactory());
  const missingOptional = runtimeFor({session: async (_game, operation) => operation({
    sync: async () => {}, send: async (_type, {path}) => {
      if (path === 'score06.dat') throw Object.assign(new Error('missing'), {errno: 44});
      return {bytes: [5]};
    },
  })});
  const files = await collectLocalSaveFiles({product: 'th06', names: ['score.dat', 'score06.dat'], language: 'ja', indexedDB: factory.factory, runtime: missingOptional});
  assert.deepEqual(files.map(file => file.name), ['score.dat']);
  assert.equal(factory.opened(), 0);

  const tooLarge = runtimeFor({session: async (_game, operation) => operation({
    sync: async () => {}, send: async () => ({bytes: Array(4 * 1024 * 1024 + 1).fill(1)}),
  })});
  await assert.rejects(collectLocalSaveFiles({product: 'th06', names: ['score.dat'], language: 'ja', indexedDB: factory.factory, runtime: tooLarge}), /Invalid Runtime save contents/);
  assert.equal(factory.opened(), 0);

  const sparse = new Array(2);
  sparse[1] = 7;
  const sparseBytes = runtimeFor({session: async (_game, operation) => operation({
    sync: async () => {}, send: async () => ({bytes: sparse}),
  })});
  await assert.rejects(collectLocalSaveFiles({product: 'th06', names: ['score.dat'], language: 'ja', indexedDB: factory.factory, runtime: sparseBytes}), /Invalid Runtime save contents/);
  assert.equal(factory.opened(), 0);

  const four = new Array(4 * 1024 * 1024).fill(1);
  const exactLimit = runtimeFor({session: async (_game, operation) => operation({
    sync: async () => {}, send: async () => ({bytes: four}),
  })});
  const exactFiles = await collectLocalSaveFiles({product: 'th06', names: ['a.dat', 'b.dat'], language: 'ja', indexedDB: factory.factory, runtime: exactLimit});
  assert.equal(exactFiles.length, 2);
  assert.equal(exactFiles.reduce((sum, file) => sum + file.size, 0), 8 * 1024 * 1024);
  assert.equal(factory.opened(), 0);

  const three = new Array(3 * 1024 * 1024).fill(1);
  const aggregateOverflow = runtimeFor({session: async (_game, operation) => operation({
    sync: async () => {}, send: async () => ({bytes: three}),
  })});
  await assert.rejects(collectLocalSaveFiles({product: 'th06', names: ['a.dat', 'b.dat', 'c.dat'], language: 'ja', indexedDB: factory.factory, runtime: aggregateOverflow}), /upload limit/);
  assert.equal(factory.opened(), 0);
});

test('ReplayStorageReader still shares the same safe IDBFS reader', async () => {
  const factory = new IDBFactory();
  const reader = createReplayStorageReader(() => ({indexedDB: factory, keyRange: IDBKeyRange}));
  assert.deepEqual(await reader.read('th09'), []);
  assert.deepEqual(await factory.databases(), []);
  await seededDb(factory, '/savesth09mp', [
    ['replay/th9_ud0000.rpy', signedBytes(new Uint8Array([128, 255, 84, 57]))],
    ['score.dat', new Uint8Array([9])],
  ]);
  const rows = await reader.read('th09mp');
  assert.equal(rows.length, 1);
  assert.deepEqual([...rows[0].bytes], [128, 255, 84, 57]);
  await assert.rejects(reader.read('th09mp', {path: 'replay/../../score.dat'}));
});
