import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';
import {cp, mkdir, mkdtemp, readFile, rm, writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import vm from 'node:vm';
import {exportReplayName, importReplayName} from '../../sdl-runtime/motion-replay.mjs';

// Execute the generated shells, with persistence keyed by the actual IDBFS
// mount point. This checks file ownership and reload ordering, not game logic.
async function openRuntime(directory, databases) {
  const html = await readFile(join(directory, 'th09.html'), 'utf8');
  const variant = /name="eagler-runtime-variant" content="([^"]+)"/.exec(html)?.[1];
  const files = new Map(), links = new Map();
  let mount;
  const normalize = path => path.startsWith('/save/') ? links.get('/save') + path.slice(5) : path;
  const FS = {
    mkdirTree() {},
    mount(_type, _options, root) { mount = root; },
    symlink(target, path) { links.set(path, target); },
    syncfs(populate, done) {
      if (populate) {
        for (const [path, bytes] of databases.get(mount) || []) files.set(path, bytes.slice());
      } else {
        databases.set(mount, new Map([...files].filter(([path]) => path.startsWith(mount + '/'))
          .map(([path, bytes]) => [path, bytes.slice()])));
      }
      done();
    },
    writeFile(path, bytes) { files.set(normalize(path), Uint8Array.from(bytes)); },
    readFile(path) {
      const value = files.get(normalize(path));
      if (!value) throw Error('ENOENT: ' + path);
      return value;
    },
    unlink(path) { assert.ok(files.delete(normalize(path)), path); },
    readdir(path) {
      return [...new Set([...files.keys()].filter(key => key.startsWith(path + '/'))
        .map(key => key.slice(path.length + 1).split('/')[0]))];
    },
    stat(path) { return files.has(path) ? {mode: 1, size: files.get(path).length} : {mode: 0}; },
    isFile(mode) { return mode === 1; },
    analyzePath(path) { return {exists: files.has(normalize(path))}; },
  };
  const core = {FS, IDBFS: {}, HEAPU8: new Uint8Array(2048),
    _th09_file_buffer: () => 8, _th09_file_valid: () => 1};
  const element = {addEventListener() {}, focus() {}};
  const context = {
    console, URLSearchParams, Uint8Array, ArrayBuffer, TextDecoder, performance,
    exportReplayName, importReplayName, scanCodes: {},
    createModule: async () => core,
    location: {search: '?managedData=1&runtimeEpoch=1', origin: 'https://test.invalid'},
    window: {addEventListener() {}},
    document: {querySelector: selector => selector.startsWith('meta[') ? {content: variant} : element,
      body: element, addEventListener() {}},
    parent: {postMessage() {}, __eaglerPrepareManagedRuntimeDataV1: async () => ({buffer: new ArrayBuffer(1000000)})},
    fetch: async () => ({ok: true, arrayBuffer: async () => new ArrayBuffer(1)}),
  };
  const source = (await readFile(join(directory, 'shell.mjs'), 'utf8')).replace(/^import .*;\r?\n/gm, '');
  const command = await vm.runInNewContext('(async()=>{\n' + source + '\nawait initialized; return command;})()', context);
  return {command, FS, mount, links};
}

test('packaged normal and multiplayer shells isolate saves and Replay across reloads', async () => {
  const temporary = await mkdtemp(join(tmpdir(), 'th09-storage-'));
  try {
    await mkdir(join(temporary, 'scripts'));
    await cp(new URL('../../scripts/build-eagler.mjs', import.meta.url), join(temporary, 'scripts/build-eagler.mjs'));
    await cp(new URL('../../sdl-runtime/', import.meta.url), join(temporary, 'sdl-runtime'), {recursive: true});
    const compiled = join(temporary, 'artifacts/sdl-release'), fonts = join(temporary, 'assets/sdl-native');
    await mkdir(compiled, {recursive: true}); await mkdir(fonts, {recursive: true});
    // Packaging validates the attested bytes; these are not executable game fixtures.
    const wasm = Buffer.from('storage packaging fixture');
    await writeFile(join(compiled, 'th09.wasm'), wasm);
    await writeFile(join(compiled, 'th09.mjs'), 'export default function() {}');
    await writeFile(join(compiled, 'build.json'), JSON.stringify({kind: 'th09-native-web-release-candidate',
      sha256: createHash('sha256').update(wasm).digest('hex')}));
    for (const name of ['cp932.bin', 'blend.bin']) await writeFile(join(fonts, name), 'fixture');
    for (const args of [[], ['--multiplayer']]) {
      const env = {...process.env}; delete env.TH09_OUTPUT;
      const result = spawnSync(process.execPath, [join(temporary, 'scripts/build-eagler.mjs'), ...args],
        {encoding: 'utf8', windowsHide: true, env});
      assert.equal(result.status, 0, result.stderr);
    }
    const normalPath = join(temporary, 'build-eagler'), multiplayerPath = join(temporary, 'build-eagler-multiplayer');
    const databases = new Map();
    const normal = await openRuntime(normalPath, databases), multiplayer = await openRuntime(multiplayerPath, databases);
    assert.equal(normal.mount, '/savesth09'); assert.equal(multiplayer.mount, '/savesth09mp');
    assert.equal(normal.links.get('/save'), normal.mount);
    assert.equal(multiplayer.links.get('/save'), multiplayer.mount);
    const names = ['score.dat', 'th09.cfg', 'replay/th9_01.rpy'];
    for (const [runtime, marker] of [[normal, 1], [multiplayer, 2]]) {
      for (const path of names) await runtime.command({command: 'write', path, bytes: [marker]});
      // Native C++ uses /save, while Launcher file commands use the mounted root.
      assert.equal(runtime.FS.readFile('/save/score.dat')[0], marker);
    }
    for (const [directory, marker] of [[normalPath, 1], [multiplayerPath, 2]]) {
      const runtime = await openRuntime(directory, databases);
      const listed = await runtime.command({command: 'list'});
      assert.deepEqual(Array.from(listed.files, file => file.path).sort(), [...names].sort());
      for (const path of names) {
        const result = await runtime.command({command: 'read', path: runtime.mount + '/' + path});
        assert.deepEqual(Array.from(result.bytes), [marker]);
      }
    }
    await assert.rejects(multiplayer.command({command: 'read', path: '/savesth09/score.dat'}), /存档路径无效/);
    await multiplayer.command({command: 'remove', path: 'replay/th9_01.rpy'});
    await multiplayer.command({command: 'sync'});
    const reloadedNormal = await openRuntime(normalPath, databases);
    assert.equal((await reloadedNormal.command({command: 'read', path: 'replay/th9_01.rpy'})).bytes[0], 1);
    const reloadedMultiplayer = await openRuntime(multiplayerPath, databases);
    await assert.rejects(reloadedMultiplayer.command({command: 'read', path: 'replay/th9_01.rpy'}), /ENOENT/);
  } finally { await rm(temporary, {recursive: true, force: true}); }
});
