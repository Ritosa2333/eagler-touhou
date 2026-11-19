// 低延迟画布源级形状测试（node 内置 test runner，零外部依赖，不碰 acorn）。
// 断言三件事：
//   1. 两侧 th13.html 的 installLowLatencyCanvas 调用先于 Module.default
//      （上下文创建前必须装好，源码顺序即装载顺序——顶层 await 顺序）；
//      两侧 shell.mjs 同理先于 createModule。
//   2. shell.mjs 与 th13.html 引同一共享模块路径 ./low-latency-canvas.mjs，
//      且 SP/MP 两份共享模块逐字节相同（纯属性合并，不允许分叉）。
//   3. 共享模块源码无授予状态分支：if 条件里出现 desynchronized 即失败
//      （presentation()/rendererReport() 只读上报；desynchronized 的
//      三元只改 renderer 报告文案，不触游戏路径——用 if 扫描限定）。
import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';

const read = rel => readFileSync(new URL(rel, import.meta.url), 'utf8');
const ports = ['th13', 'th13-mp'];
const SPECIFIER = './low-latency-canvas.mjs';

for (const port of ports) {
  test(`${port}: th13.html 在 Module.default 之前装好低延迟画布`, () => {
    const source = read(`../web/sdl-runtime/th13.html`);
    assert.match(source, new RegExp(
      `import\\s*\\{installLowLatencyCanvas\\}\\s*from\\s*['"]${'\\./low-latency-canvas\\.mjs'}['"]`));
    const install = source.indexOf('installLowLatencyCanvas(canvas)');
    const module = source.indexOf('Module.default(');
    assert.ok(install >= 0, 'th13.html 缺 installLowLatencyCanvas(canvas) 调用');
    assert.ok(module >= 0, 'th13.html 缺 Module.default 调用');
    assert.ok(install < module, 'installLowLatencyCanvas 必须先于 Module.default（上下文创建前装好）');
    const expose = source.indexOf('window.__th13_canvas_presentation');
    assert.ok(expose > install, 'th13.html 必须暴露 window.__th13_canvas_presentation');
  });

  test(`${port}: shell.mjs 与 th13.html 引同一共享模块且 install 先于 createModule`, () => {
    const shell = read(`../web/sdl-runtime/shell.mjs`);
    const page = read(`../web/sdl-runtime/th13.html`);
    const importRe = /import\s*\{installLowLatencyCanvas\}\s*from\s*['"]([^'"]+)['"]/;
    const shellSpecifier = importRe.exec(shell)?.[1];
    const pageSpecifier = importRe.exec(page)?.[1];
    assert.equal(shellSpecifier, SPECIFIER);
    assert.equal(pageSpecifier, SPECIFIER);
    const install = shell.indexOf('installLowLatencyCanvas(canvas)');
    const create = shell.indexOf('createModule(');
    assert.ok(install >= 0 && create >= 0, 'shell.mjs 缺 install/createModule');
    assert.ok(install < create, 'installLowLatencyCanvas 必须先于 createModule（上下文创建前装好）');
  });
}

test('SP/MP 共享模块逐字节相同', () => {
  const sp = readFileSync(new URL('../web/sdl-runtime/low-latency-canvas.mjs', import.meta.url));
  const mp = readFileSync(new URL('../../th13-mp/web/sdl-runtime/low-latency-canvas.mjs', import.meta.url));
  assert.ok(sp.equals(mp), 'low-latency-canvas.mjs 两份必须逐字节相同');
});

// 把注释与字符串字面量内容替换为空白（保留换行与代码字符），再扫描所有
// if 语句的条件括号组：出现 desynchronized 即视为授予状态分支。
function codeOnly(source) {
  let out = '';
  let state = 'code';  // code | line | block | single | double | template
  for (let i = 0; i < source.length; ++i) {
    const c = source[i], next = source[i + 1];
    if (state === 'code') {
      if (c === '/' && next === '/') { state = 'line'; out += '  '; ++i; continue; }
      if (c === '/' && next === '*') { state = 'block'; out += '  '; ++i; continue; }
      if (c === "'") { state = 'single'; out += ' '; continue; }
      if (c === '"') { state = 'double'; out += ' '; continue; }
      if (c === '`') { state = 'template'; out += ' '; continue; }
      out += c;
    } else {
      if (state === 'line' && c === '\n') { state = 'code'; out += '\n'; continue; }
      if (state === 'block' && c === '*' && next === '/') { state = 'code'; out += '  '; ++i; continue; }
      if (state === 'single' && c === "'") { state = 'code'; out += ' '; continue; }
      if (state === 'double' && c === '"') { state = 'code'; out += ' '; continue; }
      if (state === 'template' && c === '`') { state = 'code'; out += ' '; continue; }
      out += c === '\n' ? '\n' : ' ';
    }
  }
  return state === 'code' ? out : null;  // 未闭合的字面量视为源码异常
}

function ifConditions(code) {
  const conditions = [];
  for (let i = 0; i < code.length - 1; ++i) {
    if (code.slice(i, i + 2) !== 'if') continue;
    const before = i === 0 ? ' ' : code[i - 1], after = code[i + 2] ?? '';
    if (/\w/.test(before) || /\w/.test(after)) continue;  // 仅独立单词 if
    let j = i + 2;
    while (j < code.length && /\s/.test(code[j])) ++j;
    if (code[j] !== '(') continue;
    let depth = 0;
    for (let k = j; k < code.length; ++k) {
      if (code[k] === '(') ++depth;
      else if (code[k] === ')') {
        if (--depth === 0) { conditions.push(code.slice(j + 1, k)); break; }
      }
    }
  }
  return conditions;
}

test('共享模块无授予状态分支（if 条件不含 desynchronized）', () => {
  const source = read('../web/sdl-runtime/low-latency-canvas.mjs');
  const code = codeOnly(source);
  assert.ok(code, '低延迟画布模块源码应无未闭合字面量');
  const offending = ifConditions(code).filter(condition => /desynchronized/.test(condition));
  assert.deepEqual(offending, [],
    'desynchronized 只允许出现在上报路径（presentation/rendererReport），不得出现在 if 分支改游戏行为');
});
