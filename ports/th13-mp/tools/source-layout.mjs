// 代码排版的范围与内联 JS 保护规则。tools/format.mjs 执行排版，
// tests/shared-sources.test.mjs 在两个目录上做门禁，两边共用这一份规则。
import {existsSync, readdirSync, readFileSync} from 'node:fs';
import {basename, join, relative} from 'node:path';

const roots = ['web/cpp', 'portable', 'tests'];
const sourceExtension = /\.(cpp|hpp|inc|h)$/;
const skippedDirectories = new Set(['third_party', 'build', 'node_modules']);
// 生成文件由脚本原样写出，不经过 clang-format；排进范围只会在重新生成时来回变。
const generated = new Set(['MsgPlayerCp932.hpp', 'deflate_golden.hpp', 'color01_expected.hpp', 'rgba64_png.hpp']);
// 多人版目录 → 普通版目录。多人版只排版与普通版同路径的文件；联机目录等
// 多人版独有代码不在范围内。
const singleOf = {'th13-mp': 'th13'};

// 返回端口目录下参与排版的文件（端口目录相对路径，已排序）。
export function formatTargets(portDir) {
  const counterpart = singleOf[basename(portDir)];
  const walk = dir => readdirSync(dir, {withFileTypes: true}).flatMap(entry => {
    const path = join(dir, entry.name);
    if (entry.isDirectory()) return skippedDirectories.has(entry.name) ? [] : walk(path);
    if (!sourceExtension.test(entry.name) || entry.name.endsWith('.gen.hpp') || generated.has(entry.name)) return [];
    const name = relative(portDir, path);
    if (counterpart && !existsSync(join(portDir, '..', counterpart, name))) return [];
    return [name];
  });
  return roots.flatMap(root => existsSync(join(portDir, root)) ? walk(join(portDir, root)) : []).sort();
}

export const inlineJs = /\b(?:MAIN_THREAD_)?EM_ASM(?:_INT|_DOUBLE|_FLOAT)?\s*\(|\bEM_JS(?:_DEPS)?\s*\(/;

// EM_JS / EM_ASM 的函数体在预处理里是裸 token，靠字符串化拼回 JS；clang-format
// 按 C++ 规则重排会把 `??` 排成 `? ?`、`=>` 排成 `= >`。所以这类代码必须放在
// `// clang-format off` 与 `// clang-format on` 之间。
export function inlineJsLines(text) {
  const lines = [];
  text.split('\n').forEach((line, index) => {
    if (inlineJs.test(line.replace(/\/\/.*$/, ''))) lines.push(index + 1);
  });
  return lines;
}

// 返回不在 clang-format off/on 区间内的内联 JS 所在行号（1 起）。
export function unguardedInlineJsLines(text) {
  const guarded = new Set();
  let active = false;
  text.split('\n').forEach((line, index) => {
    if (line.includes('// clang-format off')) active = true;
    else if (line.includes('// clang-format on')) active = false;
    if (active) guarded.add(index + 1);
  });
  return inlineJsLines(text).filter(line => !guarded.has(line));
}

// 端口目录内所有未受保护的内联 JS：['相对路径:行号', ...]。
export function unguardedInlineJs(portDir, files = formatTargets(portDir)) {
  return files.flatMap(name => unguardedInlineJsLines(readFileSync(join(portDir, name), 'utf8'))
    .map(line => `${name}:${line}`));
}
