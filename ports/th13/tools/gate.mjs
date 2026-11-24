#!/usr/bin/env node
// th13 测试套件编排：在容器内并行运行各套件，把 TAP 结果汇总为 results.json，
// 并可与基线逐测试比对（只比对本次分组与 --only 选中的套件）。带 --compare 时，
// 退出码非零当且仅当出现基线之外的新失败；不带时，退出码非零当且仅当有套件失败。
// 用法（容器内，于 ports/th13 或 ports/th13-mp 目录执行）：
//   node tools/gate.mjs [--group native|js|all] [--jobs N] [--only a,b]
//                       [--out <目录>] [--compare <results.json>]
// 日志与结果默认写 <外层工作区>/artifacts/gate/<端口>/<分组>/。
import {spawn, spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {existsSync, mkdirSync, readdirSync, readFileSync, writeFileSync} from 'node:fs';
import {basename, dirname, join, resolve} from 'node:path';

const portDir = process.cwd();
const portName = basename(portDir);
const outer = resolve(portDir, '..', '..', '..');
const suiteTimeoutMs = 3 * 60 * 60 * 1000;
const nativeSuites = ['anm', 'ecl', 'ui', 'replay', 'items', 'score', 'combat', 'bomb', 'audio', 'e2e', 'contracts'];
// 这些 .test.mjs 只是 native 分组对应套件的包装，不重复运行。
const nativeWrappers = new Set(['anm', 'ecl', 'replay', 'ui']);
const browserless = ['shell-replay-channel.test.mjs'];

function parseArguments(argv) {
  const options = {group: 'native', jobs: 6, only: null, out: null, compare: null};
  for (let i = 0; i < argv.length; i++) {
    const value = () => {
      if (i + 1 >= argv.length) throw Error(`${argv[i]} 需要一个值`);
      return argv[++i];
    };
    switch (argv[i]) {
      case '--group': options.group = value(); break;
      case '--jobs': options.jobs = Number(value()); break;
      case '--only': options.only = new Set(value().split(',').filter(Boolean)); break;
      case '--out': options.out = resolve(value()); break;
      case '--compare': options.compare = resolve(value()); break;
      default: throw Error(`未知参数：${argv[i]}`);
    }
  }
  if (!['native', 'js', 'all'].includes(options.group)) throw Error('--group 必须是 native|js|all');
  if (!(options.jobs >= 1)) throw Error('--jobs 必须是正整数');
  return options;
}

function headCommit() {
  const result = spawnSync('git', ['rev-parse', 'HEAD'], {cwd: portDir, encoding: 'utf8'});
  return result.status === 0 ? result.stdout.trim() : null;
}

function collectSuites(group, outDir) {
  const suites = [];
  if (group !== 'js') {
    for (const name of nativeSuites) {
      if (existsSync(join(portDir, 'tests', name, 'run.sh')))
        suites.push({name, command: `bash tests/${name}/run.sh`});
    }
    // 多人版 M1–M9 累计原生工作流；逐 case 结果写在报告 JSON 里。来源摘要取
    // 当前 HEAD 的 sha256（运行器要求 64 位十六进制）。
    if (existsSync(join(portDir, 'tests/multiplayer/run-native.sh'))) {
      const cases = join(outDir, 'multiplayer-report.json');
      const digest = createHash('sha256').update(headCommit() ?? '').digest('hex');
      suites.push({name: 'multiplayer', cases, command: 'bash tests/multiplayer/run-native.sh --through M9 ' +
        `--scope cumulative --source-digest ${digest} --report ${JSON.stringify(cases)}`});
    }
  }
  if (group !== 'native') {
    const tests = join(portDir, 'tests');
    const files = existsSync(tests) ? readdirSync(tests) : [];
    const scripts = files.filter(file => file.endsWith('.test.mjs') && !nativeWrappers.has(basename(file, '.test.mjs')))
      .map(file => `tests/${file}`);
    const contracts = join(tests, 'contracts');
    if (existsSync(contracts))
      scripts.push(...readdirSync(contracts).filter(file => file.endsWith('.test.mjs')).map(file => `tests/contracts/${file}`));
    // web/tests/sdl 里不需要浏览器的测试同样归 js 套件：浏览器套件所在的 th13-shot 镜像没有 acorn。
    // 清单按普通版 shell 的录像库书写；多人版那套是独立实现，文件自行跳过，不排期。
    for (const file of portName === 'th13-mp' ? [] : browserless)
      if (existsSync(join(portDir, 'web/tests/sdl', file))) scripts.push(`web/tests/sdl/${file}`);
    if (scripts.length) suites.push({name: 'js', command: `node --test ${scripts.join(' ')}`});
  }
  return suites;
}

// 不输出 TAP 的套件只看退出码，记为一条占位失败以便与基线比对。
function parseTap(text, exitCode) {
  const passed = [], failed = [];
  let assertions = 0;
  for (const line of text.split('\n')) {
    const result = /^(not )?ok(?: \d+)? - (.*)$/.exec(line);
    if (result) (result[1] ? failed : passed).push(result[2].trim());
    const count = /^# assertions: (\d+)$/.exec(line);
    if (count) assertions += Number(count[1]);
  }
  if (exitCode !== 0 && !failed.length) failed.push(`退出码 ${exitCode}`);
  return {passed, failed, assertions};
}

function runSuite(suite, logPath) {
  return new Promise(done => {
    const startedAt = Date.now();
    // 独立进程组：超时时连同编译器与测试进程一并结束。
    const child = spawn('bash', ['-c', suite.command], {cwd: portDir, env: process.env, detached: true});
    const chunks = [];
    const collect = chunk => chunks.push(chunk);
    child.stdout.on('data', collect);
    child.stderr.on('data', collect);
    let timedOut = false;
    const timer = setTimeout(() => {
      timedOut = true;
      try { process.kill(-child.pid, 'SIGKILL'); } catch { child.kill('SIGKILL'); }
    }, suiteTimeoutMs);
    child.on('error', error => {
      clearTimeout(timer);
      chunks.push(Buffer.from(String(error)));
      child.emit('close', -1, null);
    });
    child.on('close', (code, signal) => {
      clearTimeout(timer);
      const output = Buffer.concat(chunks);
      mkdirSync(dirname(logPath), {recursive: true});
      writeFileSync(logPath, output);
      const result = {...suite, exitCode: code, signal, timedOut, seconds: (Date.now() - startedAt) / 1000, log: logPath,
        ...parseTap(output.toString('utf8'), timedOut ? -9 : code)};
      // 逐 case 报告（th13/mp-workflow-test/v1）补充 TAP 之外的测试名。
      if (suite.cases && existsSync(suite.cases)) {
        delete result.cases;
        for (const item of JSON.parse(readFileSync(suite.cases, 'utf8')).cases ?? []) {
          (item.passed ? result.passed : result.failed).push(item.id);
          result.assertions += Number(item.assertions) || 0;
        }
      }
      done(result);
    });
  });
}

async function runAll(suites, jobs, outDir) {
  const results = [];
  let next = 0;
  await Promise.all(Array.from({length: Math.min(jobs, suites.length)}, async () => {
    while (next < suites.length) {
      const index = next++;
      console.log(`[gate] 开始 ${suites[index].name}`);
      results[index] = await runSuite(suites[index], join(outDir, `${suites[index].name}.log`));
      const {name, exitCode, passed, failed, seconds} = results[index];
      console.log(`[gate] 完成 ${name}：退出 ${exitCode}，通过 ${passed.length}，失败 ${failed.length}，${seconds.toFixed(0)} 秒`);
    }
  }));
  return results;
}

// 只比对本次分组与 --only 选中的套件；其中基线里有而本次没跑的套件算回归。
function compareWith(baseline, results, group, skipped) {
  const regressions = [], improvements = [];
  const inGroup = name => group === 'all' || (name === 'js') === (group === 'js');
  const before = new Map((baseline.suites ?? [])
    .filter(suite => inGroup(suite.name) && !skipped.has(suite.name)).map(suite => [suite.name, suite]));
  for (const suite of results) {
    const prior = before.get(suite.name);
    if (!prior) {
      if (suite.exitCode !== 0) regressions.push(`${suite.name}：基线之外的新失败套件`);
      continue;
    }
    const failedBefore = new Set(prior.failed);
    for (const test of suite.failed) if (!failedBefore.has(test)) regressions.push(`${suite.name}：${test}`);
    for (const test of prior.failed) if (!suite.failed.includes(test)) improvements.push(`${suite.name}：${test}`);
  }
  for (const name of before.keys())
    if (!results.some(suite => suite.name === name)) regressions.push(`${name}：基线中有、本次未运行`);
  return {regressions, improvements};
}

const options = parseArguments(process.argv.slice(2));
const outDir = options.out ?? join(outer, 'artifacts', 'gate', portName, options.group);
mkdirSync(outDir, {recursive: true});
const collected = collectSuites(options.group, outDir);
const suites = collected.filter(suite => !options.only || options.only.has(suite.name));
const skipped = new Set(collected.filter(suite => !suites.includes(suite)).map(suite => suite.name));
if (!suites.length) {
  console.error('gate: 没有可运行的套件');
  process.exit(2);
}
const startedAt = new Date().toISOString();
const results = await runAll(suites, options.jobs, outDir);
const report = {schema: 'th13/gate-results/v1', port: portName, group: options.group, commit: headCommit(),
  startedAt, finishedAt: new Date().toISOString(), suites: results};
const reportPath = join(outDir, 'results.json');
writeFileSync(reportPath, JSON.stringify(report, null, 2) + '\n');
for (const {name, exitCode, passed, failed, assertions} of results)
  console.log(`${exitCode === 0 ? '通过' : '失败'}  ${name}  (${passed.length} 通过 / ${failed.length} 失败 / ${assertions} 断言)`);
console.log(`[gate] 结果：${reportPath}`);

if (options.compare) {
  const {regressions, improvements} = compareWith(JSON.parse(readFileSync(options.compare, 'utf8')), results, options.group, skipped);
  for (const line of regressions) console.log(`回归  ${line}`);
  for (const line of improvements) console.log(`修复  ${line}`);
  console.log(regressions.length ? `[gate] 相对基线新增失败 ${regressions.length} 项` : '[gate] 相对基线无新增失败');
  process.exitCode = regressions.length ? 1 : 0;
} else {
  process.exitCode = results.some(suite => suite.exitCode !== 0) ? 1 : 0;
}
