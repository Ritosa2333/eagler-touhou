import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync, mkdirSync, rmSync, writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {dirname, join} from 'node:path';
import {formatTargets, inlineJsLines, unguardedInlineJsLines} from '../tools/source-layout.mjs';

function fixture(t, name) {
  const dir = mkdtempSync(join(tmpdir(), 'th13-layout-'));
  t.after(() => rmSync(dir, {recursive: true, force: true}));
  const port = join(dir, 'ports', name);
  const put = (root, file) => {
    mkdirSync(dirname(join(root, file)), {recursive: true});
    writeFileSync(join(root, file), '// synthetic\n');
  };
  return {dir, port, put};
}

test('format targets cover the source roots and skip generated or third-party files', t => {
  const f = fixture(t, 'th13');
  const included = ['web/cpp/game/A.cpp', 'web/cpp/sdl/B.hpp', 'portable/sdl/Renderer.cpp',
    'portable/input/KeyboardMap.inc', 'tests/ui/a.hpp', 'tests/render/helper.h',
    'tests/e2e/native/include/SDL3/SDL.h'];
  const excluded = ['web/cpp/game/third_party/stb_image.h', 'tests/items/build/x.cpp',
    'web/cpp/game/A.gen.hpp', 'web/cpp/game/MsgPlayerCp932.hpp', 'tests/replay/deflate_golden.hpp',
    'tests/render/color01_expected.hpp', 'tests/anm/rgba64_png.hpp', 'tools/gate.mjs',
    'web/sdl-runtime/shell.mjs', 'docs/a.md', 'web/cpp/game/a.js'];
  for (const file of [...included, ...excluded]) f.put(f.port, file);
  assert.deepEqual(formatTargets(f.port), included.sort());
});

test('multiplayer formats only files that also exist in the single-player port', t => {
  const f = fixture(t, 'th13-mp');
  const single = join(f.dir, 'ports', 'th13');
  for (const file of ['web/cpp/game/A.cpp', 'web/cpp/sdl/B.cpp']) {
    f.put(f.port, file);
    f.put(single, file);
  }
  for (const file of ['web/cpp/multiplayer/Session.cpp', 'tests/multiplayer/native/case.cpp', 'web/cpp/sdl/OnlyMp.cpp'])
    f.put(f.port, file);
  assert.deepEqual(formatTargets(f.port), ['web/cpp/game/A.cpp', 'web/cpp/sdl/B.cpp']);
});

test('inline JS is found only where it is not inside a clang-format off/on region', () => {
  const guarded = '// clang-format off\nEM_JS(void, f, (), { return a ?? b; });\n// clang-format on\n';
  assert.deepEqual(unguardedInlineJsLines(guarded), []);
  assert.deepEqual(unguardedInlineJsLines(guarded + 'EM_JS(void, g, (), { return a ?? b; });'), [4]);
  assert.deepEqual(inlineJsLines('// EM_JS 不是调用\nconst int x = EM_ASM_INT({ return 1; });\n' +
    'MAIN_THREAD_EM_ASM(0);\nEM_ASM_DOUBLE({ return 1; });\nEM_ASM_FLOAT({ return 1; });\nEM_ASM({ x; });\n'),
  [2, 3, 4, 5, 6]);
  assert.deepEqual(unguardedInlineJsLines('    // clang-format off\n    const int x = EM_ASM_INT({ return 1; });\n' +
    '    // clang-format on\n    MAIN_THREAD_EM_ASM(0);\n'), [4]);
});
