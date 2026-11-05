import { mkdir, writeFile } from "node:fs/promises";
import { resolve } from "node:path";
// Each generation has a different EM_ASM-like address. Mismatched glue/Wasm
// therefore throws, rather than a same-byte empty Wasm letting a bad test pass.
function unsigned(value) {
  const bytes = [];
  do { let byte = value & 127; value >>>= 7; if (value) byte |= 128; bytes.push(byte); } while (value);
  return bytes;
}
function signed(value) {
  const bytes = [];
  for (;;) {
    let byte = value & 127; value >>= 7;
    const done = (value === 0 && !(byte & 64)) || (value === -1 && (byte & 64));
    bytes.push(done ? byte : byte | 128);
    if (done) return bytes;
  }
}
const section = (id, bytes) => [id, ...unsigned(bytes.length), ...bytes];
const string = value => [value.length, ...Buffer.from(value)];
function program(address) {
  const body = [0, 0x41, ...signed(address), 0x10, 0, 0x0b];
  return new Uint8Array([0, 97, 115, 109, 1, 0, 0, 0,
    ...section(1, [2, 0x60, 1, 0x7f, 1, 0x7f, 0x60, 0, 1, 0x7f]),
    ...section(2, [1, ...string("env"), ...string("emAsm"), 0, 0]),
    ...section(3, [1, 1]), ...section(7, [1, ...string("run"), 0, 1]),
    ...section(10, [1, ...unsigned(body.length), ...body]),
  ]);
}
export async function writeRuntimeFixture(directory, version) {
  await mkdir(directory, { recursive: true });
  const address = 466384 - (version.charCodeAt(0) - 97) * 64;
  await writeFile(resolve(directory, "runtime.html"), '<!doctype html><canvas width="2" height="2"></canvas><script type="module" src="boot.mjs"></script>');
  await writeFile(resolve(directory, "empty.wasm"), program(address));
  await writeFile(resolve(directory, "glue.mjs"), `
const ASM_CONSTS = { ${address}: () => ${version.charCodeAt(0)} };
export async function load() {
  const bytes = await (await fetch(new URL('empty.wasm', import.meta.url))).arrayBuffer();
  const { instance } = await WebAssembly.instantiate(bytes, { env: { emAsm: address => ASM_CONSTS[address]() } });
  return String.fromCharCode(instance.exports.run());
}
`);
  await writeFile(resolve(directory, "late.mjs"), `export const version = '${version}';\n`);
  await writeFile(resolve(directory, "worker.mjs"), `import { load } from './glue.mjs';
onmessage = async () => { try { postMessage(await load()); } catch (error) { postMessage({error: String(error)}); } };\n`);
  await writeFile(resolve(directory, "boot.mjs"), `import { load } from './glue.mjs';
${version === "a" ? `const version = '${version}';` : 'import { version } from "./helper.mjs";'}
const actual = await load();
if (actual !== version) throw new Error('Runtime ABI version mismatch: ' + actual + '/' + version);
document.querySelector('canvas').getContext('2d').fillRect(0, 0, 2, 2);
window.fixtureVersion = version;
window.fixtureLate = async () => (await import('./late.mjs')).version;
window.fixtureWorker = () => new Promise((resolve, reject) => {
  const worker = new Worker('./worker.mjs', {type: 'module'});
  const timer = setTimeout(() => { worker.terminate(); reject(new Error('worker timeout')); }, 10000);
  worker.onmessage = event => { clearTimeout(timer); worker.terminate(); event.data?.error ? reject(new Error(event.data.error)) : resolve(event.data); };
  worker.onerror = event => { clearTimeout(timer); worker.terminate(); reject(new Error(event.message)); };
  worker.postMessage('run');
});
`);
  if (version !== "a") await writeFile(resolve(directory, "helper.mjs"), `export const version = '${version}';\n`);
}
