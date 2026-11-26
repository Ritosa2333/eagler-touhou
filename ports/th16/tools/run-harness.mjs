// 运行 WASI harness（wasm32-wasip1）：与 tests/run.sh 的本机 harness 同源同参数。
// 用法：node tools/run-harness.mjs <command> [args...]（路径相对工作区根，如 games/th16/original/th16.dat）
import {WASI} from 'node:wasi';
import {readFileSync, writeSync} from 'node:fs';
import {resolve, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const workspace = resolve(port, '../..');
const wasmPath = process.env.TH16_HARNESS_WASM ? resolve(process.env.TH16_HARNESS_WASM) : resolve(port, 'web/artifacts/wasi/th16-harness.wasm');
const wasi = new WASI({version: 'preview1', args: ['harness', ...process.argv.slice(2)], env: {}, preopens: {'/': workspace}, returnOnExit: true});
const module = await WebAssembly.compile(readFileSync(wasmPath));
const imports = wasi.getImportObject();
// Node 22 的 uvwasi fd_write 在 stdout/stderr 为文件或 /dev/null 时会让进程段错误（anm-run 这类大输出必现；
// wasmtime 下同一 wasm 正常）。stdout/stderr 改由 JS 侧同步写出，其余 fd 仍走 uvwasi。
// Node 22 can crash in a direct wasm-to-uvwasi path_open call after reading the DAT.
// Forward through JS to avoid that host-specific call path without changing WASI rights.
const uvwasiPathOpen = imports.wasi_snapshot_preview1.path_open;
imports.wasi_snapshot_preview1.path_open = (...args) => uvwasiPathOpen(...args);
let memory = null;
const uvwasiWrite = imports.wasi_snapshot_preview1.fd_write;
imports.wasi_snapshot_preview1.fd_write = (fd, iovs, iovsLen, nwritten) => {
  if (fd !== 1 && fd !== 2) return uvwasiWrite(fd, iovs, iovsLen, nwritten);
  const view = new DataView(memory.buffer);
  let total = 0;
  for (let i = 0; i < iovsLen; ++i) {
    const ptr = view.getUint32(iovs + i * 8, true), len = view.getUint32(iovs + i * 8 + 4, true);
    let off = 0;
    while (off < len) off += writeSync(fd, new Uint8Array(memory.buffer, ptr + off, len - off));
    total += len;
  }
  view.setUint32(nwritten, total, true);
  return 0;
};
const instance = await WebAssembly.instantiate(module, imports);
memory = instance.exports.memory;
process.exitCode = wasi.start(instance) ?? 0;
