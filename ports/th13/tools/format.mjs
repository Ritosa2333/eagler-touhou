// 统一 clang-format 入口：按 .clang-format 排版本端口的 C/C++ 源码。
// 用法（容器内执行，用镜像自带的 clang-format）：
//   node tools/format.mjs           格式化
//   node tools/format.mjs --check   只检查，有未排版文件时非零退出
//   node tools/format.mjs --list    列出范围内的文件
// 宿主入口：tools/container.sh [--mp] bash -lc 'node tools/format.mjs [--check]'
// 范围与内联 JS 保护规则见 tools/source-layout.mjs。
import {spawnSync} from 'node:child_process';
import {readFileSync} from 'node:fs';
import {relative, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {formatTargets, unguardedInlineJs} from './source-layout.mjs';

const port = resolve(fileURLToPath(new URL('../', import.meta.url)));
const options = new Set(process.argv.slice(2));
const names = formatTargets(port);

if (options.has('--list')) {
  console.log(names.join('\n'));
  process.exit(0);
}

// 先查后排版：未受保护的内联 JS 停在“少一对注释”，而不是被改坏之后再来收拾。
const unguarded = unguardedInlineJs(port, names);
if (unguarded.length) {
  console.error('内联 JS 必须放在 // clang-format off 与 // clang-format on 之间：\n  ' + unguarded.join('\n  '));
  process.exit(1);
}

// CLANG_FORMAT：clang-format 可执行文件路径；默认取镜像里 wasi-sdk 自带的一份，
// 保证与全量排版时同一个版本。
const binary = process.env.CLANG_FORMAT ?? '/opt/wasi-sdk/bin/clang-format';
const files = names.map(name => resolve(port, name));
const check = options.has('--check');
const run = spawnSync(binary, [...(check ? ['--dry-run', '--Werror'] : ['-i']), ...files],
  check ? {encoding: 'utf8'} : {stdio: 'inherit'});
if (run.error) {
  console.error(`无法运行 ${binary}：${run.error.message}（用 CLANG_FORMAT 指定路径）`);
  process.exit(1);
}
if (check && run.status !== 0) {
  const offenders = [...new Set([...run.stderr.matchAll(/^(.+?):\d+:\d+: (?:warning|error):/gm)]
    .map(match => relative(port, match[1])))];
  console.error(`未按 .clang-format 排版的文件 ${offenders.length} 个（共检查 ${files.length} 个）：`);
  for (const name of offenders) console.error('  ' + name);
  console.error("运行 node tools/format.mjs 修复；内联 JS（EM_JS / EM_ASM）需先放进 clang-format off/on 区间。");
}
const summary = check ? (run.status === 0 ? `排版检查通过：${files.length} 个文件` : null)
  : `已排版 ${files.length} 个文件`;
if (summary) console.log(summary);
process.exit(run.status ?? 1);
