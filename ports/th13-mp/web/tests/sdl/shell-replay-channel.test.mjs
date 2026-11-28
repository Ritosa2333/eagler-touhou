import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createRequire} from 'node:module';
import {runInNewContext} from 'node:vm';

const require = createRequire(new URL('../../../../../launcher/package.json', import.meta.url));
function loadAcorn() {
  try { return require('acorn'); } catch {}
  try {
    return createRequire(import.meta.url)(`${process.env.EMSDK ?? '/opt/emsdk'}/upstream/emscripten/node_modules/acorn`);
  } catch { return null; }
}
const acorn = loadAcorn();
const source = readFileSync(new URL('../../sdl-runtime/shell.mjs', import.meta.url), 'utf8');
const multiplayerStore = /const replayStem = runtimeBuild\.multiplayer/.test(source);
const unavailable = !acorn ? '未找到 acorn：本测试由 th13-dev 镜像的 js 套件运行'
  : multiplayerStore ? '多人版录像库是独立实现（th13mp 命名空间 + 内存/IDB），不适用本测试' : false;
const check = (name, body) => test(name, unavailable ? {skip: unavailable} : {}, body);
const ast = acorn ? acorn.parse(source, {ecmaVersion: 'latest', sourceType: 'module'}) : {body: []};

const functionNames = ['sanitizePath', 'readSaveBytes', 'writeSaveBytes', 'replayIndex',
  'replayWriteIndex', 'replayToBase64', 'replayFromBase64', 'readReplayFile',
  'writeReplayFile', 'removeReplayFile', 'restoreReplaysToCatalog', 'command'];
const constNames = ['replayIndexKey', 'replayBlobPrefix', 'replayNamePattern',
  'replaySingleMax', 'replayTotalMax'];
const parts = [];
for (const node of ast.body) {
  if (node.type === 'FunctionDeclaration' && node.id && functionNames.includes(node.id.name))
    parts.push(source.slice(node.start, node.end));
  if (node.type === 'VariableDeclaration' && node.kind === 'const' &&
      node.declarations.some(d => d.id.type === 'Identifier' && constNames.includes(d.id.name)))
    parts.push(source.slice(node.start, node.end));
}
const constStatements = [];
const matchedConstNames = [];
for (const node of ast.body) {
  if (node.type !== 'VariableDeclaration' || node.kind !== 'const') continue;
  const names = node.declarations
    .filter(d => d.id.type === 'Identifier' && constNames.includes(d.id.name))
    .map(d => d.id.name);
  if (names.length) { constStatements.push(node); matchedConstNames.push(...names); }
}
if (!unavailable) {
  assert.equal(parts.length, functionNames.length + constStatements.length,
    'extraction list must match shell.mjs structure (drift guard)');
  assert.deepEqual([...matchedConstNames].sort(), [...constNames].sort(),
    'all storage constants must be extracted');
}

const localStorageShim = (seed = {}) => {
  const store = new Map(Object.entries(seed));
  return {
    getItem: key => store.has(key) ? store.get(key) : null,
    setItem: (key, value) => store.set(key, String(value)),
    removeItem: key => store.delete(key),
    store,
  };
};
const adapter = (seed = {}, extra = {}) => {
  const storage = localStorageShim(seed);
  const context = {launched: false, core: {}, localStorage: storage,
    btoa: text => Buffer.from(text, 'binary').toString('base64'),
    atob: text => Buffer.from(text, 'base64').toString('binary'),
    ...extra};
  runInNewContext(parts.join('\n') + ';globalThis.adapter = {command, sanitizePath};', context);
  const command = async message => JSON.parse(JSON.stringify(await context.adapter.command(message)));
  return {storage, command, sanitizePath: context.adapter.sanitizePath};
};

check('sanitizePath 放行 replay 命名空间与存档路径、拒绝其余', () => {
  const {sanitizePath} = adapter();
  assert.equal(sanitizePath('replay/th13_15.rpy'), 'replay/th13_15.rpy');
  assert.equal(sanitizePath('replay/th13_07.rpyx'), 'replay/th13_07.rpyx');
  assert.equal(sanitizePath('REPLAY/TH13_udAB12.RPY'), 'replay/th13_udab12.rpy');
  assert.equal(sanitizePath('scoreth13.dat'), 'scoreth13.dat');
  assert.equal(sanitizePath('/savesth13/scoreth13.dat'), 'scoreth13.dat');
  assert.equal(sanitizePath('th13.cfg'), 'th13.cfg');
  for (const bad of ['replay/foo.rpy', 'replay/th13_123.rpy', 'replay/th13_udabc.rpy',
    'replay/sub/th13_01.rpy', 'replay/', '../replay/th13_01.rpy', 'save/th13_01.rpy',
    'replay/th13_udabcd.rpyexe', ''])
    assert.throws(() => sanitizePath(bad), /存档路径无效/, bad);
});

check('list 合成 scoreth13.dat 与 replay 索引', async () => {
  const {command} = adapter({
    'th13.scoredat': Buffer.from('SAVE!').toString('base64'),
    'th13.replays': JSON.stringify([{name: 'th13_15.rpy', size: 1234, ts: 2},
      {name: 'th13_udzz99.rpy', size: 55, ts: 3}]),
    'th13.replay.th13_15.rpy': Buffer.from('REPLAY15').toString('base64'),
  });
  assert.deepEqual(await command({command: 'list'}),
    {files: [{path: 'scoreth13.dat', size: 5},
             {path: 'replay/th13_15.rpy', size: 1234},
             {path: 'replay/th13_udzz99.rpy', size: 55}]});
  assert.deepEqual(await adapter().command({command: 'list'}), {files: []});
});

check('replay write/read roundtrip、覆盖与 remove', async () => {
  const {command, storage} = adapter();
  const bytes = Array.from(Buffer.from('t13r-replay-bytes'));
  await command({command: 'write', path: 'replay/th13_15.rpy', bytes});
  assert.deepEqual((await command({command: 'read', path: 'replay/th13_15.rpy'})).bytes, bytes);
  assert.equal(storage.getItem('th13.replay.th13_15.rpy'), Buffer.from(bytes).toString('base64'));
  const [entry] = JSON.parse(storage.getItem('th13.replays'));
  assert.equal(entry.name, 'th13_15.rpy');
  assert.equal(entry.size, bytes.length);
  assert.equal(typeof entry.ts, 'number');
  const bytes2 = Array.from(Buffer.from('t13r-replay-bytes-v2'));
  await command({command: 'write', path: 'replay/th13_15.rpy', bytes: bytes2});
  assert.equal(JSON.parse(storage.getItem('th13.replays')).length, 1);
  assert.deepEqual((await command({command: 'read', path: 'replay/th13_15.rpy'})).bytes, bytes2);
  await assert.rejects(command({command: 'read', path: 'replay/th13_01.rpy'}), /文件不存在/);
  await command({command: 'remove', path: 'replay/th13_15.rpy'});
  assert.equal(storage.getItem('th13.replay.th13_15.rpy'), null);
  assert.deepEqual(JSON.parse(storage.getItem('th13.replays')), []);
  await assert.rejects(command({command: 'read', path: 'replay/th13_15.rpy'}), /文件不存在/);
  await assert.rejects(command({command: 'remove', path: 'replay/th13_15.rpy'}), /文件不存在/);
});

check('launched 守卫与空/超大文件拒收', async () => {
  const launchedCommand = adapter({}, {launched: true}).command;
  await assert.rejects(launchedCommand({command: 'write', path: 'replay/th13_15.rpy', bytes: [1]}), /请先退出游戏/);
  await assert.rejects(launchedCommand({command: 'write', path: 'scoreth13.dat', bytes: [1]}), /请先退出游戏/);
  await assert.rejects(launchedCommand({command: 'remove', path: 'replay/th13_15.rpy'}), /请先退出游戏/);
  const {command} = adapter();
  await assert.rejects(command({command: 'write', path: 'replay/th13_15.rpy', bytes: []}), /文件大小无效/);
  await assert.rejects(command({command: 'write', path: 'replay/th13_15.rpy',
    bytes: new Array(64 * 1024 * 1024 + 1).fill(0)}), /文件大小无效/);
  await assert.rejects(command({command: 'write', path: 'replay/th13_15.rpy',
    bytes: new Array(1024 * 1024 + 1).fill(0)}), /录像文件过大/);
  await assert.rejects(command({command: 'write', path: 'th13.cfg', bytes: [1]}), /不支持写入此文件/);
  await assert.rejects(command({command: 'read', path: 'th13.cfg'}), /文件不存在/);
});

check('scoreth13.dat 键迁移：写读走 th13.scoredat，旧 th13.save 仅回退读', async () => {
  const {command, storage} = adapter();
  const save = Array.from(Buffer.from('scoredat-new-format'));
  await command({command: 'write', path: 'scoreth13.dat', bytes: save});
  assert.equal(storage.getItem('th13.scoredat'), Buffer.from(save).toString('base64'));
  assert.deepEqual((await command({command: 'read', path: 'scoreth13.dat'})).bytes, save);
  assert.deepEqual(await command({command: 'list'}),
    {files: [{path: 'scoreth13.dat', size: save.length}]});
  const {command: command2, storage: storage2} = adapter({'th13.save': Buffer.from('legacy-th13-save').toString('base64')});
  assert.deepEqual((await command2({command: 'read', path: 'scoreth13.dat'})).bytes,
    Array.from(Buffer.from('legacy-th13-save')));
  assert.deepEqual(await command2({command: 'list'}),
    {files: [{path: 'scoreth13.dat', size: Buffer.from('legacy-th13-save').length}]});
  await command2({command: 'write', path: 'scoreth13.dat', bytes: Array.from(Buffer.from('fresh'))});
  assert.deepEqual((await command2({command: 'read', path: 'scoreth13.dat'})).bytes,
    Array.from(Buffer.from('fresh')));
  await command2({command: 'remove', path: 'scoreth13.dat'});
  assert.equal(storage2.getItem('th13.scoredat'), null);
  assert.equal(storage2.getItem('th13.save'), null);
  await assert.rejects(command2({command: 'read', path: 'scoreth13.dat'}), /存档不存在/);
  assert.deepEqual(await command2({command: 'list'}), {files: []});
  await command2({command: 'remove', path: 'scoreth13.dat'});
});

check('总量护栏：超 5MiB 逐出最旧，正写入条目不逐出', async () => {
  const {command, storage} = adapter();
  const chunk = new Array(900 * 1024).fill(0x41);
  for (const slot of ['01', '02', '03', '04', '05', '06'])
    await command({command: 'write', path: `replay/th13_${slot}.rpy`, bytes: chunk});
  const index = JSON.parse(storage.getItem('th13.replays'));
  assert.equal(index.length, 4);
  assert.deepEqual(index.map(entry => entry.name),
    ['th13_03.rpy', 'th13_04.rpy', 'th13_05.rpy', 'th13_06.rpy']);
  assert.equal(storage.getItem('th13.replay.th13_01.rpy'), null);
  for (const slot of ['03', '04', '05', '06'])
    assert.deepEqual((await command({command: 'read', path: `replay/th13_${slot}.rpy`})).bytes, chunk);
});

check('restoreReplaysToCatalog 按索引回灌 catalog、缺 blob 跳过', () => {
  const calls = [];
  const core = {
    _malloc: size => size,
    allocateUTF8: name => name,
    HEAPU8: {set: (bytes, ptr) => calls.push(['set', ptr, bytes.length])},
    _th13_replay_import: (ptr, size, namePtr) => {
      calls.push(['import', namePtr, size]);
      return 1;
    },
    _th13_replay_import_error: () => 0,
    UTF8ToString: () => '',
    _free: () => {},
  };
  const storage = localStorageShim({
    'th13.replays': JSON.stringify([{name: 'th13_21.rpy', size: 8, ts: 1},
      {name: 'th13_07.rpy', size: 5, ts: 2}]),
    'th13.replay.th13_21.rpy': Buffer.from('replay-A').toString('base64'),
  });
  runInNewContext(parts.join('\n') + ';restoreReplaysToCatalog();',
    {core, localStorage: storage,
     atob: text => Buffer.from(text, 'base64').toString('binary')});
  assert.deepEqual(calls, [['set', 8, 8], ['import', 'th13_21.rpy', 8]]);
});

check('跨面一致性：EM_JS 钩子与两处持久化实现同键注册', () => {
  const gameCpp = readFileSync(new URL('../../cpp/sdl/ApplicationGame.cpp', import.meta.url), 'utf8');
  const emjsStart = gameCpp.indexOf('EM_JS(bool, th13_download_recorded_replay');
  const emjs = gameCpp.slice(emjsStart, gameCpp.indexOf('});', emjsStart));
  assert.ok(emjsStart >= 0, 'EM_JS block must exist');
  assert.match(emjs, /Module\.onReplaySaved\?\.\(/, 'EM_JS must call the persistence hook');
  assert.match(emjs, /'th13_' \+ String\(slot\)\.padStart\(2, '0'\) \+ '\.rpy'/, 'hook name contract');

  const html = readFileSync(new URL('../../sdl-runtime/th13.html', import.meta.url), 'utf8');
  for (const [where, text] of [['th13.html', html], ['shell.mjs', source]]) {
    assert.match(text, /onReplaySaved/, `${where} must register onReplaySaved`);
    assert.match(text, /'th13\.replays'/, `${where} index key`);
    assert.match(text, /'th13\.replay\./, `${where} blob key prefix`);
    assert.match(text, /th13_\(\?:\\d\{2\}\|ud\[a-z0-9\]\{4\}\)\\.rpyx\?\$/, `${where} name pattern`);
  }
  assert.match(html, /__th13_replay_restore_done/);
});
