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
// 每个 port 读自己的文件：测试在 ports/th13/tests/，MP 在 ../../th13-mp/。
const runtimeDir = port => port === 'th13' ? '../web/sdl-runtime' : '../../th13-mp/web/sdl-runtime';

for (const port of ports) {
  test(`${port}: th13.html 在 Module.default 之前装好低延迟画布`, () => {
    const source = read(`${runtimeDir(port)}/th13.html`);
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
    const shell = read(`${runtimeDir(port)}/shell.mjs`);
    const page = read(`${runtimeDir(port)}/th13.html`);
    // 防路径别名回归：MP 迭代必须真读到 MP 侧文件（两侧 shell 内容必然不同）。
    if (port === 'th13-mp') assert.notEqual(shell, read('../web/sdl-runtime/shell.mjs'),
      'th13-mp 迭代读到了 SP 的 shell.mjs（路径解析回归）');
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

function listedRuntimeNames(source) {
  const match = /const runtimeNames = \[([\s\S]*?)\];/.exec(source);
  assert.ok(match, '打包清单缺 runtimeNames');
  return [...match[1].matchAll(/'([^']+)'/g)].map(item => item[1]);
}

test('两份打包清单与产品目录都包含共享模块', () => {
  const moduleName = 'low-latency-canvas.mjs';
  for (const rel of ['../portable/package-eagler.mjs', '../../th13-mp/portable/package-eagler.mjs']) {
    assert.ok(listedRuntimeNames(read(rel)).includes(moduleName), `${rel} 未打包 ${moduleName}`);
  }
  const catalog = read('../../../common/contracts/product-catalog.mjs');
  const th13 = /th13: Object\.freeze\(\{([\s\S]*?)\n    \}\),\n    th14:/.exec(catalog);
  assert.ok(th13, '产品目录缺 th13');
  assert.match(th13[1], /"low-latency-canvas\.mjs"/, 'th13 runtimeAssets 必须包含共享模块');
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

// input kick 生产回退开关的形状：launcher 内嵌时不透传页面 query，shell 必须在
// 同源下回退读父窗 query；默认开、仅精确 '0' 关闭；跨源不可读时保持默认。
for (const port of ports) {
  test(`${port}: shell.mjs inputKick 回退读父窗 query 且仅 '0' 关闭`, () => {
    const shell = read(`${runtimeDir(port)}/shell.mjs`);
    assert.match(shell, /parent !== window/, '回退必须先判非顶层窗');
    assert.match(shell, /URLSearchParams\(parent\.location\.search\)/, '回退必须读父窗 query');
    assert.match(shell, /catch\s*\{\s*\}/, '跨源读取必须 try 包裹');
    assert.match(shell, /inputKickParam\(\) !== '0'/, "默认开，仅精确 '0' 关闭");
    assert.doesNotMatch(shell, /inputKick.*=== '1'/, '不得要求显式 =1 才开启');
  });
}
