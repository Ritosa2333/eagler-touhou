// shell.mjs replay 文件通道 + scoreth13.dat 键迁移单测（node --test，无浏览器）。
// 覆盖（managed 壳 postMessage 协议 list/read/write/remove 的存储语义）：
//   1. sanitizePath：replay/th13_NN.rpy(x) / th13_udXXXX 命名空间放行、
//      其余拒绝（launcher Replay 工作台的路径唯一入口）。
//   2. list：scoreth13.dat + replay 索引合成 [{path,size}]。
//   3. write/read roundtrip、覆盖、remove；launched 守卫；空/超大拒收。
//   4. scoreth13.dat 键迁移：写读走 'th13.scoredat'；旧 'th13.save' 只在
//      新键缺席时回退读（过渡兼容，见 shell.mjs readSaveBytes 注释）；
//      remove 两键齐清（防回退读复活旧档）。
//   5. 总量护栏：>5MiB（base64 账面）逐出最旧条目，正写入条目不逐出。
//   6. restoreReplaysToCatalog 按索引回灌 catalog、缺 blob 跳过。
//   7. 跨面一致性（静态）：EM_JS 钩子 onReplaySaved 存在，th13.html /
//      shell.mjs 注册且持久化键名一致（两处内联同款实现的漂移栅栏）。
// 运行：make exec-port CMD='node --test web/tests/sdl/shell-replay-channel.test.mjs'
import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createRequire} from 'node:module';
import {runInNewContext} from 'node:vm';

const require = createRequire(new URL('../../../../../launcher/package.json', import.meta.url));
const {parse} = require('acorn');
const source = readFileSync(new URL('../../sdl-runtime/shell.mjs', import.meta.url), 'utf8');
const ast = parse(source, {ecmaVersion: 'latest', sourceType: 'module'});

// 提取被测函数与常量声明（managed-shell.test.mjs 同款 VM 隔离范式；这里
// 的被测面横跨 command 与存储助手，按名单显式提取）。
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
assert.equal(parts.length, functionNames.length + constStatements.length,
  'extraction list must match shell.mjs structure (drift guard)');
assert.deepEqual([...matchedConstNames].sort(), [...constNames].sort(),
  'all storage constants must be extracted');

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
    // vm 上下文无浏览器全局：注入 base64 对（shell.mjs 持久化实现依赖）。
    btoa: text => Buffer.from(text, 'binary').toString('base64'),
    atob: text => Buffer.from(text, 'base64').toString('binary'),
    ...extra};
  runInNewContext(parts.join('\n') + ';globalThis.adapter = {command, sanitizePath};', context);
  // vm 域的返回对象原型与本域不同（assert deepEqual 比原型）：过 JSON 归一。
  const command = async message => JSON.parse(JSON.stringify(await context.adapter.command(message)));
  return {storage, command, sanitizePath: context.adapter.sanitizePath};
};

test('sanitizePath 放行 replay 命名空间与存档路径、拒绝其余', () => {
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

test('list 合成 scoreth13.dat 与 replay 索引', async () => {
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
  // 空库 → 空清单（launcher 侧 invalidFileList/空目录语义）。
  assert.deepEqual(await adapter().command({command: 'list'}), {files: []});
});

test('replay write/read roundtrip、覆盖与 remove', async () => {
  const {command, storage} = adapter();
  const bytes = Array.from(Buffer.from('t13r-replay-bytes'));
  await command({command: 'write', path: 'replay/th13_15.rpy', bytes});
  assert.deepEqual((await command({command: 'read', path: 'replay/th13_15.rpy'})).bytes, bytes);
  assert.equal(storage.getItem('th13.replay.th13_15.rpy'), Buffer.from(bytes).toString('base64'));
  const [entry] = JSON.parse(storage.getItem('th13.replays'));
  assert.equal(entry.name, 'th13_15.rpy');
  assert.equal(entry.size, bytes.length);
  assert.equal(typeof entry.ts, 'number');
  // 覆盖同名：单条索引、尺寸更新。
  const bytes2 = Array.from(Buffer.from('t13r-replay-bytes-v2'));
  await command({command: 'write', path: 'replay/th13_15.rpy', bytes: bytes2});
  assert.equal(JSON.parse(storage.getItem('th13.replays')).length, 1);
  assert.deepEqual((await command({command: 'read', path: 'replay/th13_15.rpy'})).bytes, bytes2);
  // 读不存在 → 现有错误约定；remove 后读亦然。
  await assert.rejects(command({command: 'read', path: 'replay/th13_01.rpy'}), /文件不存在/);
  await command({command: 'remove', path: 'replay/th13_15.rpy'});
  assert.equal(storage.getItem('th13.replay.th13_15.rpy'), null);
  assert.deepEqual(JSON.parse(storage.getItem('th13.replays')), []);
  await assert.rejects(command({command: 'read', path: 'replay/th13_15.rpy'}), /文件不存在/);
  await assert.rejects(command({command: 'remove', path: 'replay/th13_15.rpy'}), /文件不存在/);
});

test('launched 守卫与空/超大文件拒收', async () => {
  const launchedCommand = adapter({}, {launched: true}).command;
  await assert.rejects(launchedCommand({command: 'write', path: 'replay/th13_15.rpy', bytes: [1]}), /请先退出游戏/);
  await assert.rejects(launchedCommand({command: 'write', path: 'scoreth13.dat', bytes: [1]}), /请先退出游戏/);
  await assert.rejects(launchedCommand({command: 'remove', path: 'replay/th13_15.rpy'}), /请先退出游戏/);
  const {command} = adapter();
  await assert.rejects(command({command: 'write', path: 'replay/th13_15.rpy', bytes: []}), /文件大小无效/);
  await assert.rejects(command({command: 'write', path: 'replay/th13_15.rpy',
    bytes: new Array(64 * 1024 * 1024 + 1).fill(0)}), /文件大小无效/);
  // replay 命名空间内部的单条上限（1MiB）。
  await assert.rejects(command({command: 'write', path: 'replay/th13_15.rpy',
    bytes: new Array(1024 * 1024 + 1).fill(0)}), /录像文件过大/);
  // sanitizePath 放行但无存储分支的路径（th13.cfg）保持原语义。
  await assert.rejects(command({command: 'write', path: 'th13.cfg', bytes: [1]}), /不支持写入此文件/);
  await assert.rejects(command({command: 'read', path: 'th13.cfg'}), /文件不存在/);
});

test('scoreth13.dat 键迁移：写读走 th13.scoredat，旧 th13.save 仅回退读', async () => {
  // 1) 新键写入 + 读回。
  const {command, storage} = adapter();
  const save = Array.from(Buffer.from('scoredat-new-format'));
  await command({command: 'write', path: 'scoreth13.dat', bytes: save});
  assert.equal(storage.getItem('th13.scoredat'), Buffer.from(save).toString('base64'));
  assert.deepEqual((await command({command: 'read', path: 'scoreth13.dat'})).bytes, save);
  assert.deepEqual(await command({command: 'list'}),
    {files: [{path: 'scoreth13.dat', size: save.length}]});
  // 2) 仅旧键：回退读生效（过渡兼容），list 仍呈现 scoreth13.dat。
  const {command: command2, storage: storage2} = adapter({'th13.save': Buffer.from('legacy-th13-save').toString('base64')});
  assert.deepEqual((await command2({command: 'read', path: 'scoreth13.dat'})).bytes,
    Array.from(Buffer.from('legacy-th13-save')));
  assert.deepEqual(await command2({command: 'list'}),
    {files: [{path: 'scoreth13.dat', size: Buffer.from('legacy-th13-save').length}]});
  // 3) 写入新键后新键恒优先（回退只兜底缺席）。
  await command2({command: 'write', path: 'scoreth13.dat', bytes: Array.from(Buffer.from('fresh'))});
  assert.deepEqual((await command2({command: 'read', path: 'scoreth13.dat'})).bytes,
    Array.from(Buffer.from('fresh')));
  // 4) remove：两键齐清（否则回退读会复活旧档）。
  await command2({command: 'remove', path: 'scoreth13.dat'});
  assert.equal(storage2.getItem('th13.scoredat'), null);
  assert.equal(storage2.getItem('th13.save'), null);
  await assert.rejects(command2({command: 'read', path: 'scoreth13.dat'}), /存档不存在/);
  assert.deepEqual(await command2({command: 'list'}), {files: []});
  // 5) 不存在的存档 remove 不报错（幂等，保持既有语义）。
  await command2({command: 'remove', path: 'scoreth13.dat'});
});

test('总量护栏：超 5MiB 逐出最旧，正写入条目不逐出', async () => {
  const {command, storage} = adapter();
  const chunk = new Array(900 * 1024).fill(0x41);  // ~0.9MiB/条，base64 账面 ~1.2MiB
  for (const slot of ['01', '02', '03', '04', '05', '06'])
    await command({command: 'write', path: `replay/th13_${slot}.rpy`, bytes: chunk});
  const index = JSON.parse(storage.getItem('th13.replays'));
  // 6×1.2MiB > 5MiB：最旧逐出至 ≤5MiB（4 条留存），th13_01/02 必然被逐。
  assert.equal(index.length, 4);
  assert.deepEqual(index.map(entry => entry.name),
    ['th13_03.rpy', 'th13_04.rpy', 'th13_05.rpy', 'th13_06.rpy']);
  assert.equal(storage.getItem('th13.replay.th13_01.rpy'), null);
  for (const slot of ['03', '04', '05', '06'])
    assert.deepEqual((await command({command: 'read', path: `replay/th13_${slot}.rpy`})).bytes, chunk);
});

test('restoreReplaysToCatalog 按索引回灌 catalog、缺 blob 跳过', () => {
  const calls = [];
  const core = {
    _malloc: size => size,  // 尺寸作指针，便于断言
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
      {name: 'th13_07.rpy', size: 5, ts: 2}]),  // 无 blob → 跳过
    'th13.replay.th13_21.rpy': Buffer.from('replay-A').toString('base64'),
  });
  runInNewContext(parts.join('\n') + ';restoreReplaysToCatalog();',
    {core, localStorage: storage,
     atob: text => Buffer.from(text, 'base64').toString('binary')});
  assert.deepEqual(calls, [['set', 8, 8], ['import', 'th13_21.rpy', 8]]);
});

test('跨面一致性：EM_JS 钩子与两处持久化实现同键注册', () => {
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
  // th13.html 恢复栅栏旗标（smoke 测试依赖）。
  assert.match(html, /__th13_replay_restore_done/);
});
