import {WASI} from 'node:wasi';
import {readFileSync, mkdirSync, statSync} from 'node:fs';
import {resolve, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {harnessPaths, validateHarnessOutput} from './harness-paths.mjs';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const workspace = resolve(port, '../../..');
const wasmPath = process.env.TH13_HARNESS_WASM
  ? resolve(process.env.TH13_HARNESS_WASM)
  : resolve(port, 'web/artifacts/wasi/th13-harness.wasm');
const bytes = readFileSync(wasmPath);

const {args, outputs} = harnessPaths(workspace, process.argv.slice(2));
const before = new Map();
const snapshot = path => {
  try { return statSync(path, {bigint: true}); }
  catch (error) {
    if (error.code === 'ENOENT') return null;
    throw error;
  }
};
for (const output of outputs) {
  mkdirSync(dirname(output.host), {recursive: true});
  before.set(output.host, snapshot(output.host));
}
const env = {};
for (const key of ['TH13_DUMPCHECK', 'TH13_SHOT_PROBE', 'TH13_LIFE_PROBE', 'TH13_STATE_LOG',
                   'TH13_FREEZE_AT', 'TH13_NO_ECL_SLOT_ANM', 'TH13_RNG_TRACE',
                   'TH13_SPIRIT_PROBE', 'TH13_DAMAGE_PROBE'])
  if (process.env[key]) env[key] = process.env[key];
const wasi = new WASI({version: 'preview1', args, env, preopens: {'/': workspace}, returnOnExit: true});
const module = await WebAssembly.compile(bytes);
const instance = await WebAssembly.instantiate(module, wasi.getImportObject());
const code = wasi.start(instance) ?? 0;
if (code !== 0) process.exit(code);
for (const output of outputs) {
  validateHarnessOutput(output, before.get(output.host), snapshot(output.host));
  console.log(`harness-output ${output.host}`);
}
process.exit(0);
