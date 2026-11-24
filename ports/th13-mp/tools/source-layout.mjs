import {existsSync, readdirSync, readFileSync} from 'node:fs';
import {basename, join, relative} from 'node:path';

const roots = ['web/cpp', 'portable', 'tests'];
const sourceExtension = /\.(cpp|hpp|inc|h)$/;
const skippedDirectories = new Set(['third_party', 'build', 'node_modules']);
const generated = new Set(['MsgPlayerCp932.hpp', 'deflate_golden.hpp', 'color01_expected.hpp', 'rgba64_png.hpp']);
const singleOf = {'th13-mp': 'th13'};

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

export function inlineJsLines(text) {
  const lines = [];
  text.split('\n').forEach((line, index) => {
    if (inlineJs.test(line.replace(/\/\/.*$/, ''))) lines.push(index + 1);
  });
  return lines;
}

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

export function unguardedInlineJs(portDir, files = formatTargets(portDir)) {
  return files.flatMap(name => unguardedInlineJsLines(readFileSync(join(portDir, name), 'utf8'))
    .map(line => `${name}:${line}`));
}
