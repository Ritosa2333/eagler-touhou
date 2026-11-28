import {execFileSync} from 'node:child_process';
import {existsSync, readFileSync, writeFileSync} from 'node:fs';
import {basename, extname, join, resolve} from 'node:path';
import {createRequire} from 'node:module';
import {fileURLToPath} from 'node:url';

const roots = ['container', 'portable', 'tests', 'tools', 'web'];
const skipped = new Set(['__pycache__', 'artifacts', 'assets', 'build', 'node_modules', 'third_party']);
const generated = new Set(['MsgPlayerCp932.hpp', 'color01_expected.hpp', 'deflate_golden.hpp', 'rgba64_png.hpp']);
const kinds = {'.cpp': 'c', '.h': 'c', '.hpp': 'c', '.inc': 'c', '.java': 'c', '.css': 'css',
  '.mjs': 'js', '.js': 'js', '.html': 'html', '.sh': 'sh', '.py': 'py'};
const clangFormat = /^\/\/\s*clang-format\s+(?:off|on)\b/;
const copiedBlock = [/^\/\/ BEGIN GENERATED SPECTATOR PLAYBACK[ \t]*$/, /^\/\/ END GENERATED SPECTATOR PLAYBACK[ \t]*$/];
const javaScript = /^(?:module|(?:text|application)\/(?:java|ecma)script|text\/(?:jscript|livescript))$/;
const hereOperator = /^<<(?!<|>|&|=)(-?)[ \t]*(?:\\?(['"]?))([A-Za-z_][A-Za-z0-9_]*)\2/;
const pythonRanges = `
import ast, io, json, sys, tokenize
text = sys.stdin.read()
lines = text.split(chr(10))
offsets = [0]
for line in lines:
    offsets.append(offsets[-1] + len(line) + 1)
def offset(position):
    return offsets[position[0] - 1] + position[1]
ranges = []
tokens = list(tokenize.generate_tokens(io.StringIO(text).readline))
for token in tokens:
    if token.type == tokenize.COMMENT:
        ranges.append([offset(token.start), offset(token.end)])
strings = {token.start: index for index, token in enumerate(tokens) if token.type == tokenize.STRING}
documents = (ast.Module, ast.ClassDef, ast.FunctionDef, ast.AsyncFunctionDef)
for node in ast.walk(ast.parse(text)):
    if not isinstance(node, documents) or not node.body:
        continue
    first = node.body[0]
    literal = isinstance(first, ast.Expr) and isinstance(first.value, ast.Constant) and isinstance(first.value.value, str)
    index = strings.get((first.lineno, first.col_offset)) if literal else None
    if index is None:
        continue
    start, end = tokens[index].start, tokens[index].end
    while index + 1 < len(tokens) and tokens[index + 1].type in (tokenize.STRING, tokenize.NL):
        index += 1
        if tokens[index].type == tokenize.STRING:
            end = tokens[index].end
    ranges.append([offset(start), offset(end)])
print(json.dumps(ranges))
`;

let parser;
const acorn = () => parser ??= (() => {
  for (const at of ['../../../launcher/package.json', import.meta.url]) {
    try { return createRequire(new URL(at, import.meta.url))('acorn'); } catch {}
  }
  return createRequire(import.meta.url)(`${process.env.EMSDK ?? '/opt/emsdk'}/upstream/emscripten/node_modules/acorn`);
})();

const kindOf = path => {
  const name = basename(path);
  if (generated.has(name) || name.endsWith('.gen.hpp')) return null;
  if (['Dockerfile', '.clang-format', '.gitignore'].includes(name)) return 'hash';
  return kinds[extname(name)] ?? null;
};

export function commentTargets(portDir) {
  return execFileSync('git', ['-C', portDir, 'ls-files', '-z'], {encoding: 'utf8', maxBuffer: 1 << 28})
    .split('\0').filter(path => kindOf(path) && (!path.includes('/') || roots.includes(path.split('/')[0]))
      && path.split('/').every(part => !skipped.has(part)) && existsSync(join(portDir, path))).sort();
}

const quotedEnd = (text, start) => {
  let at = start + 1;
  while (at < text.length && text[at] !== text[start] && text[at] !== '\n') at += text[at] === '\\' ? 2 : 1;
  return Math.min(at + 1, text.length);
};

const rawStringEnd = (text, start) => {
  const open = text.indexOf('(', start + 1);
  const delimiter = text.slice(start + 1, open);
  const close = text.indexOf(')' + delimiter + '"', open + 1);
  return close < 0 ? text.length : close + delimiter.length + 2;
};

const cLikeComments = (text, lineComments) => {
  const ranges = [];
  let at = 0;
  while (at < text.length) {
    const start = at;
    if (lineComments && text.startsWith('//', at)) {
      for (at += 2; at < text.length && (text[at] !== '\n' || text[at - 1] === '\\'); at++);
      if (!clangFormat.test(text.slice(start, at))) ranges.push([start, at]);
    } else if (text.startsWith('/*', at)) {
      const close = text.indexOf('*/', at + 2);
      at = close < 0 ? text.length : close + 2;
      ranges.push([start, at]);
    } else if (text[at] === '"' && text[at - 1] === 'R') at = rawStringEnd(text, at);
    else if (text[at] === '"' || text[at] === "'") at = quotedEnd(text, at);
    else at++;
  }
  return ranges;
};

const parseScript = (text, module) => {
  const ranges = [];
  const parse = sourceType => acorn().parse(text, {ecmaVersion: 'latest', sourceType,
    onComment: (block, value, start, end) => ranges.push([start, end])});
  try { parse(module ? 'module' : 'script'); } catch (first) {
    if (!module) throw first;
    ranges.length = 0;
    try { parse('script'); } catch { throw first; }
  }
  return ranges;
};

const htmlComments = text => {
  const ranges = [], bodies = [];
  const tags = /<script\b([^>]*)>([\s\S]*?)<\/script>/gi;
  for (let tag = tags.exec(text); tag; tag = tags.exec(text)) {
    const type = (tag[1].match(/\btype\s*=\s*["']?([^"'\s>]+)/i) ?? [])[1]?.toLowerCase() ?? '';
    if (type && !javaScript.test(type)) continue;
    const body = tag.index + tag[0].length - tag[2].length - 9;
    bodies.push([body, body + tag[2].length]);
    ranges.push(...parseScript(tag[2], type === 'module').map(([from, to]) => [from + body, to + body]));
  }
  for (let at = text.indexOf('<!--'); at >= 0; at = text.indexOf('<!--', at + 4)) {
    if (bodies.some(([from, to]) => from <= at && at < to)) continue;
    const close = text.indexOf('-->', at + 4);
    ranges.push([at, close < 0 ? text.length : close + 3]);
  }
  return ranges.sort((from, to) => from[0] - to[0]);
};

const shellComments = text => {
  const ranges = [], stack = [], documents = [];
  const unquoted = (start, mark) => {
    for (let end = start + 1; end < text.length; end++) {
      if (text[end] === '\\') { end++; continue; }
      if (text[end] === mark) return end + 1;
    }
    return -1;
  };
  let state = 'code', at = 0;
  while (at < text.length) {
    const c = text[at];
    if (state === 'document') {
      const tag = documents[0].tag;
      const first = at + (documents[0].tabs ? text.slice(at).match(/^\t*/)[0].length : 0);
      const ends = text.startsWith(tag, first) && ['\n', undefined].includes(text[first + tag.length]);
      if (ends) documents.shift();
      if (ends && !documents.length) state = 'code';
      at = ends ? first + tag.length + 1 : (text.indexOf('\n', at) < 0 ? text.length : text.indexOf('\n', at) + 1);
      continue;
    }
    if (c === '\n') {
      if (state === 'code' && documents.length) state = 'document';
      at++;
      continue;
    }
    if (state === 'single') {
      if (c === "'") state = 'code';
      at++;
    } else if (state === 'ansi' || state === 'backtick') {
      if ((state === 'ansi' && c === "'") || (state === 'backtick' && c === '`')) state = 'code';
      at += c === '\\' ? 2 : 1;
    } else if (state === 'double') {
      if (c === '"') { state = 'code'; at++; }
      else if (c === '`') {
        const end = unquoted(at, '`');
        if (end < 0) break;
        at = end;
      } else if (c === '$' && ['(', '{'].includes(text[at + 1])) {
        stack.push({close: text[at + 1] === '(' ? ')' : '}', resume: 'double'});
        state = 'code';
        at += 2;
      } else at += c === '\\' ? 2 : 1;
    } else if (c === '\\') at += 2;
    else if (c === '#' && at > 0 && /[\s;|&({]/.test(text[at - 1]) && !(text[at - 1] === '{' && text[at - 2] === '$')
      && stack.at(-1)?.close !== '}') {
      const end = text.indexOf('\n', at);
      ranges.push([at, end < 0 ? text.length : end]);
      at = end < 0 ? text.length : end;
    } else if (c === '$' && text[at + 1] === "'") { state = 'ansi'; at += 2; }
    else if (c === "'") { state = 'single'; at++; }
    else if (c === '"') { state = 'double'; at++; }
    else if (c === '`') { state = 'backtick'; at++; }
    else if (c === '(' || (c === '{' && text[at - 1] === '$')) {
      stack.push({close: c === '(' ? ')' : '}', resume: null});
      at++;
    } else if (c === stack.at(-1)?.close) {
      state = stack.pop().resume ?? 'code';
      at++;
    } else if (c === '<' && hereOperator.test(text.slice(at, at + 80))) {
      const operator = hereOperator.exec(text.slice(at, at + 80));
      documents.push({tag: operator[3], tabs: operator[1] === '-'});
      at += operator[0].length;
    } else at++;
  }
  return ranges;
};

const codePointRanges = (text, ranges) => {
  const wide = [];
  let point = 0;
  for (const ch of text) { if (ch.length === 2) wide.push(point); point++; }
  const utf16 = offset => offset + wide.filter(at => at < offset).length;
  return wide.length ? ranges.map(([from, to]) => [utf16(from), utf16(to)]) : ranges;
};

const hashComments = text => [...text.matchAll(/^[ \t]*#.*$/gm)].map(match => [match.index, match.index + match[0].length]);

const lineStarts = text => {
  const starts = [0];
  for (let at = text.indexOf('\n'); at >= 0; at = text.indexOf('\n', at + 1)) starts.push(at + 1);
  return starts;
};

const lineOf = (starts, offset) => {
  let low = 0, high = starts.length - 1;
  while (low < high) {
    const mid = (low + high + 1) >> 1;
    if (starts[mid] <= offset) low = mid; else high = mid - 1;
  }
  return low;
};

export const analyze = (name, text) => {
  const ranges = {
    c: () => cLikeComments(text, true),
    css: () => cLikeComments(text, false),
    js: () => parseScript(text, true),
    html: () => htmlComments(text),
    sh: () => shellComments(text),
    hash: () => hashComments(text),
    py: () => codePointRanges(text, JSON.parse(execFileSync('python3', ['-X', 'utf8', '-c', pythonRanges], {input: text, encoding: 'utf8'}))),
  }[kindOf(name)]();
  const lines = text.split('\n'), starts = [0], keep = new Set();
  let copied = false;
  lines.forEach((line, index) => {
    starts.push(starts.at(-1) + line.length + 1);
    if (copiedBlock[0].test(line)) copied = true;
    if (copied) keep.add(index);
    if (copiedBlock[1].test(line)) copied = false;
  });
  const shielded = from => keep.has(lineOf(starts, from)) || (from === 0 && text.startsWith('#!'));
  return {ranges: ranges.filter(([from]) => !shielded(from)).sort((from, to) => from[0] - to[0]), keep};
};

export function erase(text, ranges, keep = new Set()) {
  const lines = text.split('\n'), starts = lineStarts(text), out = [];
  let next = 0, dropped = false, blank = false;
  lines.forEach((line, index) => {
    const from = starts[index], to = from + line.length;
    while (next < ranges.length && ranges[next][1] <= from) next++;
    if (keep.has(index)) { out.push(line); dropped = blank = false; return; }
    let kept = '', at = from, touched = false;
    for (let r = next; r < ranges.length && ranges[r][0] < to; r++) {
      const [open, close] = ranges[r];
      kept += text.slice(at, Math.max(at, open));
      at = Math.max(at, close);
      touched = true;
      const before = open > from ? text[open - 1] : '', after = close < to ? text[close] : '';
      if (/[ \t]/.test(before)) {
        if (/[ \t]/.test(after) || /^[,;:)\]}]/.test(after)) {
          kept = kept.replace(/[ \t]+$/, '');
          const closer = {'{': '}', '(': ')', '[': ']'}[kept.at(-1)];
          if (closer && text.slice(close, to).match(/^[ \t]*(.)/)?.[1] === closer)
            while (at < to && /[ \t]/.test(text[at])) at++;
        }
      } else if (before && after) kept += ' ';
    }
    kept += text.slice(at, to);
    if (!touched) {
      if (dropped && blank && !line.trim()) return;
      out.push(line);
      blank = !line.trim();
      return;
    }
    if (!kept.trim()) { dropped = true; return; }
    out.push(kept.replace(/[ \t]+$/, ''));
    dropped = blank = false;
  });
  while (out.length > 1 && out.at(-1) === '') out.pop();
  const result = out.join('\n');
  return result.trim() ? result + '\n' : '';
}

export const commentsIn = (name, text) => {
  const starts = lineStarts(text);
  return analyze(name, text).ranges.map(([from, to]) => ({
    line: lineOf(starts, from), text: text.slice(from, Math.min(to, from + 100)).split('\n')[0]}));
};

export function scan(portDir) {
  return commentTargets(portDir).flatMap(name => commentsIn(name, readFileSync(join(portDir, name), 'utf8'))
    .map(finding => `${name}:${finding.line + 1}: ${finding.text}`));
}

const strip = port => commentTargets(port).reduce((changed, name) => {
  const path = join(port, name), text = readFileSync(path, 'utf8');
  const next = erase(text, ...Object.values(analyze(name, text)));
  if (next !== text) writeFileSync(path, next);
  return changed + (next !== text);
}, 0);

if (resolve(process.argv[1] ?? '') === fileURLToPath(import.meta.url)) {
  const port = resolve(fileURLToPath(new URL('../', import.meta.url)));
  const options = process.argv.slice(2);
  if (options.includes('--list')) {
    console.log(commentTargets(port).join('\n'));
  } else if (options.includes('--strip')) {
    const count = commentTargets(port).length;
    console.log(`已删除注释：${strip(port)}/${count} 个文件`);
  } else {
    const findings = scan(port);
    for (const finding of findings.slice(0, 200)) console.error('  ' + finding);
    console.error(findings.length ? `含注释的源码位置 ${findings.length} 处（共检查 ${commentTargets(port).length} 个文件）`
      : `无注释检查通过：${commentTargets(port).length} 个文件`);
    process.exit(findings.length ? 1 : 0);
  }
}
