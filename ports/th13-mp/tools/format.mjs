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

const unguarded = unguardedInlineJs(port, names);
if (unguarded.length) {
  console.error('内联 JS 必须放在 // clang-format off 与 // clang-format on 之间：\n  ' + unguarded.join('\n  '));
  process.exit(1);
}

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
