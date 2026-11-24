import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile, writeFile, mkdir} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {dirname, join, resolve, extname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '../../..');
const workspace = resolve(port, '../../..');
const runtime = join(port, 'web/sdl-runtime');
const build = process.env.TH13_SDL3_BUILD_DIR ?? join(port, 'web/artifacts');
const evidence = process.env.TH13_MUSIC_ROOM_EVIDENCE;
const require = createRequire(import.meta.url);
let chromium;
try { ({chromium} = require('playwright-core')); }
catch { ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core'))); }

const sha256 = data => createHash('sha256').update(data).digest('hex');

function parseMusiccmt(bytes) {
  const lines = [];
  let start = 0;
  for (let i = 0; i <= bytes.length; ++i) {
    if (i !== bytes.length && bytes[i] !== 0x0a) continue;
    let end = i;
    if (end > start && bytes[end - 1] === 0x0d) --end;
    lines.push(Buffer.from(bytes.subarray(start, end)));
    start = i + 1;
  }
  const tracks = [];
  for (let i = 0; i < lines.length; ++i) {
    const line = lines[i];
    if (line.length < 4 || line[0] !== 0x40) continue;
    const slash = line.indexOf(0x2f);
    if (slash < 0) continue;
    const track = {name: line.subarray(slash + 1), title: null, comments: []};
    for (let k = 0; k < 9; ++k) {
      if (i + 1 >= lines.length) break;
      const next = lines[i + 1];
      if (next.length && next[0] === 0x40) break;
      ++i;
      if (k === 0) track.title = lines[i];
      else track.comments.push(lines[i]);
    }
    tracks.push(track);
  }
  return tracks;
}

function hasInk(cp932) {
  for (let i = 0; i < cp932.length; ++i) {
    const byte = cp932[i];
    if (byte === 0x20 || byte === 0x09) continue;
    if (byte === 0x81 && cp932[i + 1] === 0x40) { ++i; continue; }
    return true;
  }
  return false;
}

const f32 = bits => {
  const buffer = Buffer.alloc(4);
  buffer.writeUInt32LE(bits >>> 0);
  return buffer.readFloatLE(0);
};

function parseAnm(bytes, label) {
  const data = Buffer.from(bytes);
  const size = data.length;
  const entries = [], sprites = [], scripts = [];
  let base = 0;
  for (;;) {
    if (base + 64 > size) throw Error(`${label}: truncated entry header`);
    if (data.readUInt32LE(base) !== 8) throw Error(`${label}: version != 8`);
    const ns = data.readUInt16LE(base + 4), nc = data.readUInt16LE(base + 6);
    const width = data.readUInt16LE(base + 10), height = data.readUInt16LE(base + 12);
    const stride = data.readUInt32LE(base + 36);
    const chunk = stride || size - base;
    if (chunk < 64 || base + chunk > size) throw Error(`${label}: bad chunk`);
    const nameOffset = data.readUInt32LE(base + 16);
    const nameEnd = data.indexOf(0, base + nameOffset);
    if (nameOffset >= chunk || nameEnd < 0 || nameEnd >= base + chunk)
      throw Error(`${label}: bad texture name`);
    const name = data.subarray(base + nameOffset, nameEnd).toString('latin1');
    const tableEnd = 64 + ns * 4 + nc * 8;
    if (tableEnd > chunk) throw Error(`${label}: bad table`);
    const entryIndex = entries.length;
    for (let i = 0; i < ns; ++i) {
      const offset = data.readUInt32LE(base + 64 + i * 4);
      if (offset < tableEnd || offset + 20 > chunk) throw Error(`${label}: bad sprite`);
      sprites.push({
        sourceId: data.readUInt32LE(base + offset),
        x: data.readFloatLE(base + offset + 4),
        y: data.readFloatLE(base + offset + 8),
        width: data.readFloatLE(base + offset + 12),
        height: data.readFloatLE(base + offset + 16),
        textureIndex: entryIndex,
      });
    }
    for (let i = 0; i < nc; ++i) {
      const record = base + 64 + ns * 4 + i * 8;
      const sourceId = data.readInt32LE(record);
      const offset = data.readUInt32LE(record + 4);
      if (offset < tableEnd || offset + 8 > chunk) throw Error(`${label}: bad script`);
      const ops = [];
      let cursor = base + offset;
      const limit = base + chunk;
      for (;;) {
        if (cursor + 8 > limit) throw Error(`${label}: unterminated script`);
        const opcode = data.readInt16LE(cursor);
        const length = data.readUInt16LE(cursor + 2);
        const time = data.readInt16LE(cursor + 4);
        if (opcode === -1) break;
        if (length < 8 || length % 4 || cursor + length > limit)
          throw Error(`${label}: bad instruction`);
        const args = [];
        for (let a = 0; a < (length - 8) / 4; ++a)
          args.push(data.readUInt32LE(cursor + 8 + a * 4));
        ops.push({opcode, time, args});
        cursor += length;
      }
      scripts.push({sourceId, ops, entryIndex});
    }
    entries.push({name, width, height, ns, nc, textureIndex: entryIndex});
    if (!stride) break;
    base += stride;
  }
  return {entries, sprites, scripts};
}

function scriptIdentity(anm, index) {
  const script = anm.scripts[index];
  if (!script) throw Error(`missing script ${index}`);
  const spriteOp = script.ops.find(op => op.opcode === 300);
  const positionOp = script.ops.find(op => op.opcode === 400);
  if (!spriteOp || !positionOp) throw Error(`script ${index}: sprite/position missing`);
  const spriteIndex = spriteOp.args[0] | 0;
  const sprite = anm.sprites[spriteIndex];
  if (!sprite) throw Error(`script ${index}: sprite ${spriteIndex} missing`);
  const texture = anm.entries[sprite.textureIndex];
  return {
    script: index,
    spriteIndex,
    rect: [sprite.x, sprite.y, sprite.width, sprite.height],
    position: [f32(positionOp.args[0]), f32(positionOp.args[1]), f32(positionOp.args[2])],
    textureIndex: sprite.textureIndex,
    textureName: texture.name,
  };
}

function layerRecord(text, script, size, origin) {
  const lines = text.split('\n');
  for (let i = 0; i + 1 < lines.length; ++i) {
    const head = /^\s+script=(-?\d+) sprite=(-?\d+) pos=\(([^,]+),([^)]+)\) flags=0x([0-9a-f]+) tex=(.*)$/
      .exec(lines[i]);
    if (!head || Number(head[1]) !== script) continue;
    const visual = /^\s+visual=([0-9a-f]+) color=([0-9a-f]+) mode=(\d+) scale=\(([^,]+),([^)]+)\) size=\(([^,]+),([^)]+)\) origin=\(([^,]+),([^,]+),([^)]+)\) pc=(-?\d+)$/
      .exec(lines[i + 1]);
    if (!visual) continue;
    const record = {
      script: Number(head[1]), sprite: Number(head[2]),
      pos: [Number(head[3]), Number(head[4])], flags: parseInt(head[5], 16),
      textureField: head[6],
      visual: parseInt(visual[1], 16), color: parseInt(visual[2], 16),
      mode: Number(visual[3]), scale: [Number(visual[4]), Number(visual[5])],
      size: [Number(visual[6]), Number(visual[7])],
      origin: [Number(visual[8]), Number(visual[9]), Number(visual[10])],
      pc: Number(visual[11]),
    };
    if (size && (record.size[0] !== size[0] || record.size[1] !== size[1])) continue;
    if (origin && (record.origin[0] !== origin[0] || record.origin[1] !== origin[1]))
      continue;
    return record;
  }
  return null;
}

function vmByBankScript(state, bank, script) {
  return (state.vms ?? []).filter(vm => vm.bank === bank && vm.script === script);
}

function decodePosition(vm) {
  return (vm.position_bits ?? []).map(f32);
}

function assertVmIdentity(vm, expected, bank, label, {position = true} = {}) {
  assert.ok(vm, `${label}: VM missing (bank ${bank} script ${expected.script})`);
  assert.equal(vm.bank, bank, `${label}: bank`);
  assert.equal(vm.script, expected.script, `${label}: script`);
  assert.equal(vm.sprite, expected.spriteIndex, `${label}: sprite index`);
  assert.equal(vm.texture_index, expected.textureIndex, `${label}: texture index`);
  assert.equal(vm.texture_name, expected.textureName, `${label}: texture name`);
  if (position) {
    const decoded = decodePosition(vm);
    for (let axis = 0; axis < 3; ++axis)
      assert.ok(Math.abs(decoded[axis] - expected.position[axis]) < 1e-4,
        `${label}: position[${axis}] ${decoded[axis]} != ${expected.position[axis]}`);
  }
  assert.ok((vm.color >>> 24) !== 0, `${label}: zero-alpha text VM`);
}

function commentPasses(updates, expected) {
  const key = region => region.join(',');
  const keys = expected.map(item => [
    item.rect[0], item.rect[1], item.rect[0] + item.rect[2], item.rect[1] + item.rect[3],
  ].join(','));
  const passes = [];
  let current = null;
  for (const update of updates) {
    const line = keys.indexOf(key(update.region));
    if (line < 0) continue;
    if (line === 0 || !current || current.length === expected.length) {
      current = [];
      passes.push(current);
    }
    current.push({...update, line});
  }
  return passes.filter(pass =>
    pass.length === expected.length && pass.every((update, line) => update.line === line));
}

function assertHash(value, label) {
  assert.match(value ?? '', /^[0-9a-f]{64}$/, `${label}: content hash is not sha256 hex`);
}

test('production Music Room text identity: original ANM scripts/textures, ' +
     'submission ink, paging identity, zero console errors',
     {timeout: 300000}, async () => {
  const manifestPath = join(build, 'sdl3/build.json');
  const wasmPath = join(build, 'sdl3/th13-sdl.wasm');
  const manifest = JSON.parse(await readFile(manifestPath, 'utf8'));
  const wasm = await readFile(wasmPath);
  assert.equal(wasm.length, manifest.bytes, 'build manifest byte count');
  assert.equal(sha256(wasm), manifest.sha256, 'build manifest wasm sha256');
  const inventory = manifest.sourceFiles ?? {};
  const currentHash = async relative => {
    const absolute = join(port, relative);
    return sha256(await readFile(absolute));
  };
  const musicPath = [
    'web/cpp/game/TitleMusicRoomView.cpp', 'web/cpp/game/TitleMusicRoomView.hpp',
    'web/cpp/game/TitleMusicRoomMenu.cpp', 'web/cpp/game/TitleMusicRoomMenu.hpp',
    'web/cpp/game/TitleScene.cpp', 'web/cpp/game/TitleScene.hpp',
    'web/cpp/game/AnmText.cpp', 'web/cpp/game/AnmText.hpp',
    'web/cpp/game/AnmResource.cpp', 'web/cpp/game/AnmResource.hpp',
    'web/cpp/game/AnmInterpreter.cpp', 'web/cpp/game/AnmManager.cpp',
    'web/cpp/game/AnmVm.hpp', 'web/cpp/game/TextureImage.cpp',
    'web/cpp/game/AnmRenderer.cpp', 'web/cpp/game/RenderSubmission.hpp',
    'web/cpp/game/SubmissionTrace.hpp',
    'web/cpp/sdl/ApplicationGame.cpp', 'web/cpp/sdl/ApplicationTitle.cpp',
    'web/cpp/sdl/Application.cpp', 'web/cpp/sdl/ApplicationState.hpp',
    'web/cpp/sdl/BrowserRuntime.cpp', 'web/cpp/sdl/GraphicsDevice.cpp',
    'web/cpp/sdl/GraphicsDevice.hpp', 'web/cpp/sdl/FontDevice.cpp',
    'web/cpp/sdl/FontDevice.hpp',
  ];
  const drift = [];
  const musicDrift = [];
  const portPrefix = port.replaceAll('\\', '/') + '/';
  const portRelative = absolute => {
    const normalized = absolute.replaceAll('\\', '/');
    return normalized.startsWith(portPrefix) ? normalized.slice(portPrefix.length) : null;
  };
  for (const [absolute, recorded] of Object.entries(inventory)) {
    const suffix = portRelative(absolute);
    if (!suffix) continue;
    let actual;
    try { actual = await currentHash(suffix); } catch { actual = null; }
    if (actual !== recorded) drift.push({file: suffix, recorded, actual});
  }
  for (const relative of musicPath) {
    const hit = drift.find(item => item.file === relative);
    if (hit) musicDrift.push(hit);
  }
  assert.deepEqual(musicDrift, [],
    'BUILD_DRIFT (build artifact predates current music-room sources; not a product failure): ' +
    JSON.stringify(musicDrift));
  const tracked = new Set(Object.keys(inventory).map(portRelative).filter(Boolean));
  assert.ok(tracked.size > 0, 'build manifest lists this port\'s sources');
  assert.deepEqual(musicPath.filter(relative => !tracked.has(relative)), [],
    'music-room path source missing from build manifest (cannot certify build identity)');

  const assetNames = ['musiccmt.txt', 'title.anm', 'text.anm', 'ascii.anm'];
  const assetIdentity = {};
  const rawDir = join(workspace, 'artifacts/reference/raw');
  for (const name of assetNames) {
    let raw, runtimeAsset;
    try { raw = await readFile(join(rawDir, name)); }
    catch (error) { throw Error(`ASSET_MISSING: ${join(rawDir, name)}: ${error.message}`); }
    try { runtimeAsset = await readFile(join(runtime, 'assets', name)); }
    catch (error) { throw Error(`ASSET_MISSING: runtime assets/${name}: ${error.message}`); }
    assert.equal(sha256(runtimeAsset), sha256(raw),
      `ASSET_DRIFT: runtime assets/${name} differs from original raw data`);
    assetIdentity[name] = {sha256: sha256(raw), bytes: raw.length};
  }
  const musiccmt = parseMusiccmt(await readFile(join(rawDir, 'musiccmt.txt')));
  assert.equal(musiccmt.length, 17, 'original musiccmt track count');
  for (const [index, track] of musiccmt.entries())
    assert.equal(track.comments.length, 8, `track ${index} comment line count`);
  assert.ok(hasInk(musiccmt[0].comments[0]), 'track 0 line 0 expected ink');
  assert.ok(!hasInk(musiccmt[0].comments[1]) && !hasInk(musiccmt[0].comments[3]),
    'track 0 lines 1/3 expected blank (full-width space)');
  assert.notDeepEqual(musiccmt[0].comments[0], musiccmt[10].comments[0],
    'track 0 and track 10 first comments must differ');

  const textAnm = parseAnm(await readFile(join(rawDir, 'text.anm')), 'text.anm');
  const titleAnm = parseAnm(await readFile(join(rawDir, 'title.anm')), 'title.anm');
  const asciiAnm = parseAnm(await readFile(join(rawDir, 'ascii.anm')), 'ascii.anm');
  const commentIdentity = [];
  for (let line = 0; line < 8; ++line) {
    const identity = scriptIdentity(textAnm, 41 + line);
    assert.equal(identity.spriteIndex, 25 + line, `text.anm script ${41 + line} sprite`);
    assert.deepEqual(identity.rect, [0, 18 * line, 512, 17],
      `text.anm script ${41 + line} rect`);
    assert.deepEqual(identity.position, [64, 320 + 18 * line, 0],
      `text.anm script ${41 + line} position`);
    assert.equal(identity.textureIndex, 0, `text.anm script ${41 + line} texture index`);
    assert.equal(identity.textureName, '@', `text.anm script ${41 + line} texture name`);
    commentIdentity.push(identity);
  }
  const rowIdentity = [];
  for (let row = 0; row < musiccmt.length; ++row) {
    const identity = scriptIdentity(titleAnm, 160 + row);
    assert.equal(identity.spriteIndex, 132 + row, `title.anm script ${160 + row} sprite`);
    assert.deepEqual(identity.rect, [0, 18 * row, 512, 18],
      `title.anm script ${160 + row} rect`);
    assert.deepEqual(identity.position, [64, 96 + 20 * row, 0],
      `title.anm script ${160 + row} position`);
    assert.equal(identity.textureIndex, 21, `title.anm script ${160 + row} texture index`);
    assert.equal(identity.textureName, '@', `title.anm script ${160 + row} texture name`);
    rowIdentity.push(identity);
  }
  const badgeIdentity = scriptIdentity(titleAnm, 106);
  assert.equal(badgeIdentity.spriteIndex, 84, 'title.anm script 106 sprite');
  assert.equal(badgeIdentity.textureName, 'title/select01.png', 'title.anm script 106 texture');
  assert.ok(asciiAnm.scripts[19], 'ascii.anm script 19 (particles) exists');
  const commentTexture = `anm:${commentIdentity[0].textureName}:entry:${commentIdentity[0].textureIndex}`;
  const rowTexture = `anm:${rowIdentity[0].textureName}:entry:${rowIdentity[0].textureIndex}`;
  assert.equal(commentTexture, 'anm:@:entry:0');
  assert.equal(rowTexture, 'anm:@:entry:21');
  const blankTextureHash = sha256(Buffer.alloc(512 * 512 * 2));
  const expectedCommentRegions = commentIdentity.map(item => [
    item.rect[0], item.rect[1], item.rect[0] + item.rect[2], item.rect[1] + item.rect[3],
  ]);
  const expectedRowRegions = rowIdentity.map(item => [
    item.rect[0], item.rect[1], item.rect[0] + item.rect[2], item.rect[1] + item.rect[3],
  ]);

  const consoleErrors = [], consoleLines = [], pageErrors = [], requestFailures = [];
  const missingRequests = [];
  const server = createServer(async (request, response) => {
    try {
      const path = new URL(request.url, 'http://localhost').pathname;
      if (path === '/favicon.ico') { response.writeHead(204).end(); return; }
      const file = path.startsWith('/artifacts/') ? join(build, path.slice(11))
        : path.startsWith('/assets/') ? join(runtime, 'assets', path.slice(8))
        : path.startsWith('/sdl-runtime/') ? join(runtime, path.replace(/^\/sdl-runtime\//, ''))
        : join(runtime, path.replace(/^\//, ''));
      const data = await readFile(file);
      const type = ({'.mjs': 'text/javascript', '.js': 'text/javascript', '.wasm': 'application/wasm',
        '.html': 'text/html', '.png': 'image/png', '.json': 'application/json'})[extname(file)]
        ?? 'application/octet-stream';
      response.writeHead(200, {'Content-Type': type}).end(data);
    } catch (error) {
      missingRequests.push(request.url);
      response.writeHead(404).end(String(error));
    }
  });
  await new Promise(done => server.listen(0, '127.0.0.1', done));
  let browser;
  let result;
  try {
    browser = await chromium.launch({executablePath: process.env.CHROMIUM_PATH, headless: true,
      args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
        '--enable-unsafe-swiftshader', '--autoplay-policy=no-user-gesture-required']});
    const page = await browser.newPage({viewport: {width: 640, height: 480}});
    page.on('console', message => {
      consoleLines.push(message.type() + ' ' + message.text());
      if (message.type() === 'error') consoleErrors.push(message.text());
    });
    page.on('pageerror', error => pageErrors.push(String(error)));
    page.on('requestfailed', request =>
      requestFailures.push(request.url() + ' ' + (request.failure()?.errorText ?? '')));
    await page.goto(`http://127.0.0.1:${server.address().port}/sdl-runtime/th13.html`,
      {waitUntil: 'domcontentloaded'});
    await page.waitForFunction(() => window.mod?._th13_frame() > 10, null, {timeout: 180000});
    result = await page.evaluate(({commentTexture, rowTexture, trackCount}) => {
      const mod = window.mod;
      mod._th13_loop_stop();
      const state = () => [...mod.HEAP32.subarray(mod._th13_probe_state() / 4,
        mod._th13_probe_state() / 4 + 16)];
      const frame = (count = 1) => {
        for (let i = 0; i < count; ++i)
          if (!mod._th13_debug_frame()) throw Error(mod.UTF8ToString(mod._th13_error()));
      };
      const tap = scan => {
        mod._th13_key(scan, 1); frame();
        mod._th13_key(scan, 0); frame();
      };
      const report = () => mod.UTF8ToString(mod._th13_debug_layer_report());
      const anmState = () => JSON.parse(mod.UTF8ToString(mod._th13_debug_anm_state()));
      const capture = name => {
        const pointer = mod.allocateUTF8(name);
        if (!pointer) throw Error('capture begin: path allocation failed');
        try {
          if (!mod._th13_debug_capture_begin(pointer))
            throw Error('capture begin: ' + mod.UTF8ToString(mod._th13_error()));
        } finally {
          mod._free(pointer);
        }
      };
      const collect = (text, identities) => {
        const out = Object.fromEntries(identities.map(identity => [identity, []]));
        for (const line of text.split('\n')) {
          if (!line) continue;
          let event;
          try { event = JSON.parse(line); } catch { continue; }
          if (event.kind !== 'update') continue;
          const resource = (event.resources ?? []).find(item => item.handle === event.resource);
          if (!resource || !(resource.identity in out)) continue;
          out[resource.identity].push({frame: event.frame, seq: event.seq,
            region: event.region, content: resource.content, handle: event.resource});
        }
        return out;
      };
      const finish = (name, identities) => {
        const frames = mod._th13_debug_capture_end();
        if (!(frames > 0 && frames < 100000))
          throw Error('capture end ' + frames + ': ' + mod.UTF8ToString(mod._th13_error()));
        const text = new TextDecoder().decode(mod.FS.readFile(name));
        const tail = text.trimEnd().split('\n').at(-1);
        let complete = false;
        try { complete = JSON.parse(tail).complete === true; } catch { complete = false; }
        return {frames, complete, updates: collect(text, identities)};
      };
      frame(440);
      for (let guard = 0; (state()[8] !== 1 || state()[9] !== 2) && guard < 240; ++guard)
        frame();
      if (state()[8] !== 1 || state()[9] !== 2)
        throw Error('Main menu not interactive: ' + JSON.stringify(state()));
      for (let i = 0; state()[10] !== 6 && i < 12; ++i) tap(208);
      if (state()[10] !== 6) throw Error('Cannot select main-menu row 6: ' + JSON.stringify(state()));

      capture('/music-room-entry.trace');
      tap(44);
      frame(120);
      const entry = {state: state(), anm: anmState(), layer: report()};
      const entryTrace = finish('/music-room-entry.trace', [commentTexture, rowTexture]);

      for (let i = 0; i < 10; ++i) tap(208);
      frame(30);
      const paged = {state: state(), anm: anmState(), layer: report()};

      capture('/music-room-page.trace');
      tap(44);
      frame(120);
      const locked = {state: state(), anm: anmState(), layer: report()};
      tap(44);
      frame(120);
      const real = {state: state(), anm: anmState(), layer: report()};
      const pageTrace = finish('/music-room-page.trace', [commentTexture, rowTexture]);
      return {entry, paged, locked, real, entryTrace, pageTrace,
        error: mod.UTF8ToString(mod._th13_error())};
    }, {commentTexture, rowTexture, trackCount: musiccmt.length});

    assert.equal(result.error, '', 'production runtime error');
    assert.equal(result.entry.state[8], 13, 'music room screen value');
    assert.equal(result.locked.state[8], 13, 'locked response left the music room');
    assert.equal(result.real.state[8], 13, 'real comment pass left the music room');
    assert.equal(result.real.state[9], 2, 'real comment pass left the interactive substate');
    assert.equal(result.entryTrace.complete, true, 'entry submission trace complete');
    assert.equal(result.pageTrace.complete, true, 'paging submission trace complete');

    for (const [line, expected] of commentIdentity.entries()) {
      const vms = vmByBankScript(result.entry.anm, 0, expected.script);
      assert.equal(vms.length, 1, `entry comment line ${line}: VM count`);
      assertVmIdentity(vms[0], expected, 0, `entry comment line ${line}`);
      const record = layerRecord(result.entry.layer, expected.script, [512, 17],
        [64, 320 + 18 * line]);
      assert.ok(record, `entry comment line ${line}: layer record missing`);
      assert.ok((record.color >>> 24) !== 0, `entry comment line ${line}: color alpha`);
      assert.equal(record.textureField, '@/@', `entry comment line ${line}: layer texture`);
    }
    for (const [row, expected] of rowIdentity.entries()) {
      const vms = vmByBankScript(result.entry.anm, 16, expected.script);
      assert.equal(vms.length, 1, `entry row ${row}: VM count`);
      assertVmIdentity(vms[0], expected, 16, `entry row ${row}`, {position: false});
      const record = layerRecord(result.entry.layer, expected.script, [512, 18]);
      assert.ok(record, `entry row ${row}: layer record missing`);
      assert.ok((record.color >>> 24) !== 0, `entry row ${row}: color alpha`);
    }
    const commentVms = (result.entry.anm.vms ?? []).filter(vm =>
      vm.bank === 0 && vm.script >= 41 && vm.script <= 48);
    const rowVms = (result.entry.anm.vms ?? []).filter(vm =>
      vm.bank === 16 && vm.script >= 160 && vm.script <= 176);
    assert.equal(commentVms.length, 8, 'exactly eight comment VMs');
    assert.equal(rowVms.length, musiccmt.length, 'one row VM per original track');
    assert.equal(vmByBankScript(result.entry.anm, 16, 177).length, 0,
      'no row VM beyond the original track count');
    assert.equal(vmByBankScript(result.entry.anm, 0, 49).length, 0,
      'no comment VM beyond the original eight lines');
    const badge = vmByBankScript(result.entry.anm, 16, 106);
    assert.equal(badge.length, 1, 'music room badge VM count');
    assertVmIdentity(badge[0], badgeIdentity, 16, 'music room badge', {position: false});
    assert.equal(vmByBankScript(result.entry.anm, 2, 19).length, 1,
      'music room particle VM (ascii.anm script 19)');

    const entryComments = result.entryTrace.updates[commentTexture];
    const entryPasses = commentPasses(entryComments, commentIdentity);
    assert.equal(entryComments.length, 8,
      'entry trace must contain exactly one Update per comment line; updates=' +
      JSON.stringify(entryComments));
    assert.equal(entryPasses.length, 1,
      'no single full 8-line comment rasterization pass in entry submission trace; updates=' +
      JSON.stringify(entryComments));
    const entryPass = entryPasses[0];
    for (const [line, update] of entryPass.entries()) {
      assert.deepEqual(update.region, expectedCommentRegions[line],
        `entry comment line ${line}: submitted region`);
      assertHash(update.content, `entry comment line ${line}`);
    }
    const prior = entryComments.filter(update => update.seq < entryPass[0].seq);
    assert.equal(prior.length, 0, 'unexpected text updates before the comment pass');
    let previousContent = blankTextureHash;
    for (const [line, update] of entryPass.entries()) {
      if (hasInk(musiccmt[0].comments[line]))
        assert.notEqual(update.content, previousContent,
          `entry comment line ${line}: ink line did not change texture content hash`);
      else
        assert.equal(update.content, previousContent,
          `entry comment line ${line}: blank line added texture content`);
      previousContent = update.content;
    }
    assert.notEqual(entryPass[7].content, blankTextureHash,
      'entry comment texture still blank after all eight lines');
    const entryRows = result.entryTrace.updates[rowTexture];
    const rowPasses = commentPasses(entryRows, rowIdentity);
    assert.equal(entryRows.length, 17,
      'entry trace must contain exactly one Update per track row; updates=' +
      JSON.stringify(entryRows));
    assert.equal(rowPasses.length, 1,
      'no single full 17-row rasterization pass in entry submission trace; updates=' +
      JSON.stringify(entryRows));
    const rowPass = rowPasses[0];
    let previousRowContent = blankTextureHash;
    for (const [row, update] of rowPass.entries()) {
      assert.deepEqual(update.region, expectedRowRegions[row], `entry row ${row}: submitted region`);
      assertHash(update.content, `entry row ${row}`);
      assert.notEqual(update.content, previousRowContent,
        `entry row ${row}: title line did not change texture content hash`);
      previousRowContent = update.content;
    }
    assert.notEqual(rowPass[16].content, blankTextureHash,
      'row title texture still blank after all tracks');

    const entryCommentHandles = commentIdentity.map(expected =>
      vmByBankScript(result.entry.anm, 0, expected.script)[0].handle);
    const entryRowHandles = rowIdentity.map(expected =>
      vmByBankScript(result.entry.anm, 16, expected.script)[0].handle);
    assert.equal(result.paged.state[8], 13, 'paging left the music room');
    for (const [line, expected] of commentIdentity.entries()) {
      const after = vmByBankScript(result.paged.anm, 0, expected.script);
      assert.equal(after.length, 1, `paged comment line ${line}: VM count`);
      assert.equal(after[0].handle, entryCommentHandles[line],
        `paged comment line ${line}: handle identity changed`);
      assertVmIdentity(after[0], expected, 0, `paged comment line ${line}`);
    }
    const visibleRows = [];
    for (const [row, expected] of rowIdentity.entries()) {
      const after = vmByBankScript(result.paged.anm, 16, expected.script);
      assert.equal(after.length, 1, `paged row ${row}: VM count`);
      assert.equal(after[0].handle, entryRowHandles[row],
        `paged row ${row}: handle identity changed`);
      assertVmIdentity(after[0], expected, 16, `paged row ${row}`, {position: false});
      if ((after[0].flags & 2) !== 0) visibleRows.push(row);
    }
    assert.deepEqual(visibleRows, Array.from({length: 10}, (_, index) => index + 1),
      'paged 10-row window visibility');
    const selectedPosition = decodePosition(vmByBankScript(result.paged.anm, 16, 170)[0]);
    const neighbourPosition = decodePosition(vmByBankScript(result.paged.anm, 16, 161)[0]);
    assert.ok(Math.abs(selectedPosition[0] - 60) < 1e-4,
      `paged selected row x ${selectedPosition[0]} != 60`);
    assert.ok(Math.abs(neighbourPosition[0] - 64) < 1e-4,
      `paged neighbour row x ${neighbourPosition[0]} != 64`);

    const pageComments = result.pageTrace.updates[commentTexture];
    assert.equal(pageComments.length, 16,
      'page trace must contain exactly two 8-line Update passes; updates=' +
      JSON.stringify(pageComments));
    const pagePasses = commentPasses(pageComments, commentIdentity);
    assert.equal(pagePasses.length, 2,
      'expected placeholder + real comment rasterization passes, got ' +
      JSON.stringify(pagePasses.map(pass => pass.map(update => update.region))));
    for (const [passIndex, pass] of pagePasses.entries()) {
      for (const [line, update] of pass.entries()) {
        assert.deepEqual(update.region, expectedCommentRegions[line],
          `page pass ${passIndex} line ${line}: submitted region`);
        assertHash(update.content, `page pass ${passIndex} line ${line}`);
      }
    }
    const [lockedPass, realPass] = pagePasses;
    assert.notEqual(lockedPass[7].content, entryPass[7].content,
      'locked placeholder did not replace track 0 comment texture content');
    assert.notEqual(realPass[7].content, lockedPass[7].content,
      'real track 10 comments did not replace locked placeholder content');
    assert.notEqual(realPass[7].content, entryPass[7].content,
      'track 10 comments produced the same texture content as track 0');
    for (const [line, expected] of commentIdentity.entries()) {
      const vms = vmByBankScript(result.real.anm, 0, expected.script);
      assert.equal(vms.length, 1, `real comment line ${line}: VM count`);
      assert.equal(vms[0].handle, entryCommentHandles[line],
        `real comment line ${line}: handle identity changed after Z`);
      assertVmIdentity(vms[0], expected, 0, `real comment line ${line}`);
    }
    for (const [row, expected] of rowIdentity.entries()) {
      const vms = vmByBankScript(result.real.anm, 16, expected.script);
      assert.equal(vms.length, 1, `real row ${row}: VM count`);
      assert.equal(vms[0].handle, entryRowHandles[row],
        `real row ${row}: handle identity changed after Z`);
      assertVmIdentity(vms[0], expected, 16, `real row ${row}`, {position: false});
    }

    assert.deepEqual(pageErrors, [], 'pageerror must stay empty');
    assert.deepEqual(consoleErrors, [],
      'console error must stay empty; missing requests: ' + JSON.stringify(missingRequests));
    assert.deepEqual(requestFailures, [], 'failed requests must stay empty');
    assert.deepEqual(missingRequests, [], 'all requested HTTP resources must exist');

    if (evidence) {
      await mkdir(evidence, {recursive: true});
      const summarize = pass => pass.map(update => ({
        line: update.line, region: update.region, content: update.content,
        frame: update.frame, seq: update.seq,
      }));
      await writeFile(join(evidence, 'music-room-text-identity.json'),
        JSON.stringify({
          schema: 'th13-music-room-text-identity-v1',
          build: {builtAt: manifest.builtAt, sha256: manifest.sha256,
            bytes: manifest.bytes, drift},
          assets: assetIdentity,
          musiccmt: {tracks: musiccmt.length,
            comments: musiccmt.map(track => track.comments.length)},
          expected: {commentTexture, rowTexture,
            commentIdentity, rowIdentity, badgeIdentity,
            commentRegions: expectedCommentRegions, rowRegions: expectedRowRegions},
          entry: {state: result.entry.state,
            commentUpdates: entryComments,
            commentPass: summarize(entryPass),
            rowPass: summarize(rowPass)},
          paged: {state: result.paged.state},
          locked: {state: result.locked.state,
            commentPass: summarize(lockedPass)},
          real: {state: result.real.state,
            commentPass: summarize(realPass)},
          console: {errors: consoleErrors, lines: consoleLines.slice(-20)},
          pageErrors, requestFailures, missingRequests,
          exactGlyphStatus: 'BLOCKED: no export exposes CP932 bytes or per-glyph pixels; ' +
            'resource identity + submission/ink only (see test header)',
        }, null, 2) + '\n');
    }
  } finally {
    await browser?.close();
    await new Promise(done => server.close(done));
  }
});
