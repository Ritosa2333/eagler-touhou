import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const compiler = process.env.TH128_EMCC ?? path.resolve(root, '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');
const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
  EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
  EMSDK_PYTHON: python, PYTHONUTF8: '1', EMCC_CORES: '2', BINARYEN_CORES: '2'};
const buildDirectory = path.join(root, 'tests/ecl-build');
const sanitize = process.env.TH128_ECL_SANITIZE === '1';

function build(name, sources, flags = []) {
  assert.ok(fs.existsSync(compiler), `required Emscripten compiler not found: ${compiler}`);
  assert.ok(fs.existsSync(python), `required bundled Python not found: ${python}`);
  fs.mkdirSync(buildDirectory, {recursive: true});
  const output = path.join(buildDirectory, `${name}${sanitize ? '-ubsan' : ''}.cjs`);
  const args = [compiler, ...sources, '-Isrc', '-std=c++17', '-O1', '-Wall', '-Wextra', '-Werror',
    '-fexceptions', '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-sNODERAWFS=1',
    '-sALLOW_MEMORY_GROWTH=1', '-sASSERTIONS=2', '-sSTACK_SIZE=2097152', '-o', output];
  args.push(...flags);
  if (sanitize) args.push('-fsanitize=undefined', '-fno-sanitize-recover=all');
  const result = spawnSync(python, args, {cwd: root, encoding: 'utf8', env: environment, timeout: 600000});
  assert.equal(result.status, 0, `${result.error ?? ''}\n${result.stdout}\n${result.stderr}`);
  return output;
}

function run(output, args = []) {
  const result = spawnSync(process.execPath, [output, ...args], {cwd: root, encoding: 'utf8', timeout: 120000});
  assert.equal(result.status, 0, `${result.error ?? ''}\n${result.stdout}\n${result.stderr}`);
  return result.stdout;
}

function actualFiles() {
  const catalogFile = path.join(root, 'local/catalog.json');
  assert.ok(fs.existsSync(catalogFile), 'local/catalog.json is required; import original assets first');
  const catalog = JSON.parse(fs.readFileSync(catalogFile, 'utf8'));
  const manifestFile = path.resolve(path.dirname(catalogFile), catalog.variants.ja.manifest);
  const manifest = JSON.parse(fs.readFileSync(manifestFile, 'utf8'));
  const names = manifest.files.filter(entry => entry.name.endsWith('.ecl')).map(entry => entry.name);
  assert.equal(names.length, 33);
  return names.map(name => path.resolve(path.dirname(manifestFile), 'raw', name));
}

test('bounded C++ ECL reader: synthetic corrupt inputs and all actual resources', () => {
  const output = build('program', ['tests/native/ecl-program.cpp', 'src/ecl/EclProgram.cpp']);
  assert.match(run(output), /PASS ECL reader synthetic/);
  assert.match(run(output, actualFiles()), /33 files, 1459 subroutines, 49097 instructions/);
});

test('bounded C++ ECL VM: numeric/control/host faults and actual main fail-stop', () => {
  const output = build('vm', ['tests/native/ecl-vm.cpp', 'src/ecl/EclProgram.cpp', 'src/ecl/EclVm.cpp']);
  assert.match(run(output), /PASS ECL explicit host acceptance/);
  assert.match(run(output, actualFiles()), /16 actual mains stop explicitly at opcode 402 offset 20, before 22/);
});

test('C++ ECL API with an exception-disabled nonthrowing host', () => {
  const output = build('noexceptions', ['tests/native/ecl-noexceptions.cpp', 'src/ecl/EclProgram.cpp', 'src/ecl/EclVm.cpp'], ['-fno-exceptions']);
  assert.match(run(output), /PASS ECL exception-disabled host contract/);
});

test('ECL include linkage and explicit typed game reference providers',()=>{
  const output=build('linked',['tests/native/ecl-link.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp']);
  assert.match(run(output,actualFiles()),/all 16 actual stages link/);
});
