#!/usr/bin/env node
// 把 anm-oracle.py 的完整快照（artifacts/th16/oracle/<name>.json，不入库）压成可入库的摘要 golden：
// 每个 (anm, script) 一个 sha256，覆盖全部帧的世界链表 VM {id, script, 0x568 字节} 与 36 条 layer 绘制链。
// tests/anm-vm.test.mjs 对 harness `anm-run` 的输出做同样的摘要再比对。
//
// 用法：node tools/oracle/anm-golden.mjs <oracle-json...>  → 写 tests/golden/anm-vm.json
import {readFileSync, writeFileSync} from 'node:fs';
import {resolve, basename} from 'node:path';
import {fileURLToPath} from 'node:url';

export const SNAPSHOT_BYTES = 0x568;

// 规范化一条脚本运行：[frame, [[id, script, bytesHex], ...], layers]
export function canonicalRun(run) {
  return run.frames.map(f => [f.frame, f.vms.map(v => [v.id, v.script, v.bytes.slice(0, SNAPSHOT_BYTES * 2)]), f.layers]);
}

export async function digestRun(run) {
  const {createHash} = await import('node:crypto');
  return createHash('sha256').update(JSON.stringify(canonicalRun(run))).digest('hex');
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  const out = resolve(fileURLToPath(new URL('../../tests/golden/anm-vm.json', import.meta.url)));
  const files = process.argv.slice(2);
  if (files.length === 0) throw Error('usage: anm-golden.mjs <oracle-json...>');
  const cases = [];
  let frames = null, layer = null;
  for (const file of files) {
    const oracle = JSON.parse(readFileSync(file, 'utf8'));
    if (frames !== null && (oracle.frames !== frames || oracle.layer !== layer)) throw Error('mixed frames/layer in ' + basename(file));
    frames = oracle.frames; layer = oracle.layer;
    const scripts = [];
    for (const run of oracle.runs) scripts.push({script: run.script, spawnedId: run.spawnedId, vmsAtEnd: run.frames.at(-1).vms.length, digest: await digestRun(run)});
    cases.push({anm: oracle.anm, textures: oracle.textures, scripts});
  }
  cases.sort((a, b) => a.anm.localeCompare(b.anm));
  const total = cases.reduce((n, c) => n + c.scripts.length, 0);
  const header = {
    schema: 'th16/anm-vm-golden/v1',
    source: 'th16.exe v1.00a executed under Unicorn (tools/oracle/anm-oracle.py)',
    frames, layer, extra: [{slot: 2, anm: 'ascii.anm'}],
    snapshotBytes: SNAPSHOT_BYTES,
    digest: 'sha256(JSON.stringify(frames.map(f => [f.frame, f.vms.map(v => [v.id, v.script, v.bytes.slice(0, 0x568 * 2)]), f.layers])))',
    totalScripts: total,
  };
  // 每脚本一行：diff 友好且体积小
  const text = '{\n' + Object.entries(header).map(([k, v]) => ` ${JSON.stringify(k)}: ${JSON.stringify(v)},\n`).join('') +
    ' "cases": [\n' + cases.map(c => `  {"anm": ${JSON.stringify(c.anm)}, "textures": ${JSON.stringify(c.textures)}, "scripts": [\n` +
      c.scripts.map(s => '   ' + JSON.stringify(s)).join(',\n') + '\n  ]}').join(',\n') + '\n ]\n}\n';
  JSON.parse(text);
  writeFileSync(out, text);
  console.log(`anm-golden: ${cases.length} anm files, ${total} scripts → ${out}`);
}
