import {execFileSync} from 'node:child_process';
import {existsSync, mkdirSync, statSync} from 'node:fs';
import {resolve} from 'node:path';

const root = resolve(import.meta.dirname, '..');
const args = process.argv.slice(2);
let mode = 'all';
if (args.length > 1 || (args.length === 1 && !args[0].startsWith('--mode='))) {
  console.error('Usage: node test-purple-portable.mjs [--mode=unit|retail|all]');
  process.exitCode = 2;
} else if (args.length === 1) {
  mode = args[0].slice('--mode='.length);
  if (!['unit', 'retail', 'all'].includes(mode)) {
    console.error('Mode must be unit, retail, or all');
    process.exitCode = 2;
  }
}

if (process.exitCode !== 2) {
  const retailDat = process.env.TH07_RETAIL_DAT ? resolve(process.env.TH07_RETAIL_DAT) : null;
  const hasRetailDat = retailDat && existsSync(retailDat) && statSync(retailDat).isFile();
  if (mode !== 'unit' && !hasRetailDat) {
    console.error('BLOCKED: retail ECL verification requires TH07_RETAIL_DAT pointing to the original th07.dat. Use --mode=unit for the portable unit checks.');
    process.exitCode = 2;
  } else {
    const sdk = process.env.EMSDK;
    if (!sdk) {
      console.error('BLOCKED: Set EMSDK explicitly before compiling the selected TH07 portable checks.');
      process.exitCode = 2;
    } else {
      const out = resolve(root, 'build/purple-portable');
      mkdirSync(out, {recursive: true});
      const env = {...process.env, EM_CONFIG: process.env.EM_CONFIG ?? resolve(sdk, '.emscripten')};
      const cc = resolve(sdk, 'install/emscripten/emcc.py');
      const run = (name, sources, extra = []) => {
        const target = resolve(out, name + '.js');
        execFileSync('python', [cc, ...sources.map(file => resolve(root, file)),
          '-I' + resolve(root, 'portable/include'), '-I' + resolve(root, 'portable/generated'), '-I' + resolve(root, 'src'),
          '-O1', '-std=c++17', '-sDEFAULT_TO_CXX=1', '-sENVIRONMENT=node', '-sALLOW_MEMORY_GROWTH=1',
          ...extra, '-o', target], {cwd: root, env, windowsHide: true, stdio: 'inherit'});
        execFileSync(process.execPath, [target], {cwd: root, env, windowsHide: true, stdio: 'inherit'});
      };

      if (mode !== 'retail') {
        run('session', ['portable/tests/session_test.cpp', 'portable/src/session.cpp']);
        run('transaction', ['portable/tests/ecl_patch_test.cpp', 'portable/src/ecl_patch.cpp']);
        run('stage1', ['portable/tests/th07_patch_test.cpp', 'portable/src/th07/patches.cpp', 'portable/src/ecl_patch.cpp']);
        console.log('UNIT MODE PASS: Replay decode, CRC/expected-byte/transaction failure and stage1 patch checks');
      }
      if (mode !== 'unit') {
        run('retail-ecl', ['tests/th07-purple-ecl.cpp', 'src/pbg4/Lzss.cpp', 'portable/src/session.cpp', 'portable/src/ecl_patch.cpp'],
          ['--use-port=sdl3', '--embed-file', retailDat + '@/fixture.dat', '-sINITIAL_MEMORY=67108864',
            "-sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=['$stringToNewUTF8']"]);
        console.log('RETAIL MODE PASS: original th07.dat ECL verification');
      }
      if (mode === 'all') console.log('ALL MODE PASS: unit and retail checks');
    }
  }
}
