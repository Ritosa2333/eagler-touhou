// 运行 WASI harness（wasm32-wasip1），输出捕获供 oracle 比对。
// 用法：node --experimental-wasi-unstable-preview1 tools/run-harness.mjs [args...]
//   无参数：RNG 冒烟；stage1：装载 st01.ecl 跑 3600 帧，
//   事件流写 web/artifacts/wasi/stage1-events.jsonl（契约见 docs/plans/event-contract.md）；
//   replay <rpy> [资源目录] [事件输出] [帧数] [stage 索引]：解码原版录像并逐帧注入
//   （rpy 用工作区根相对路径，如 games/replays/th13_udSr01.rpy）；
//   draws [资源目录] [绘制输出] [帧数] [stage.ecl] / draws-replay <rpy> [资源目录]
//   [绘制输出] [帧数] [stage 索引]：语义绘制清单导出（draw-contract.md v1
//   移植侧；默认输出 artifacts/port-draws/ 下，无输入窗 3600 帧、回放窗帧数显式传）；
//   title [资源目录] [绘制输出] / title-b [资源目录] [绘制输出]：标题窗导出
//   （oracle/title_oracle.py 同窗同规则；窗A 600 帧主菜单→难度、窗B 至开局
//   转场请求帧，boot 装载幕 + 输入时刻表内置）。
import {WASI} from 'node:wasi';
import {readFileSync, mkdirSync, statSync} from 'node:fs';
import {resolve, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {harnessPaths, validateHarnessOutput} from './harness-paths.mjs';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const workspace = resolve(port, '../../..');
// TH13_HARNESS_WASM（可选）：wasm 产物路径覆盖——lockstep 门禁
// （artifacts/verification/r3/bin/build-native.sh）把 fresh 构建产物放在
// 私有目录并经此注入，绝不复用/覆写 web/artifacts/wasi 旧产物。
const wasmPath = process.env.TH13_HARNESS_WASM
  ? resolve(process.env.TH13_HARNESS_WASM)
  : resolve(port, 'web/artifacts/wasi/th13-harness.wasm');
const bytes = readFileSync(wasmPath);

// WASI 的 '/' 对应外层工作区；容器宿主路径 /work/... 需去掉 /work。
const {args, outputs} = harnessPaths(workspace, process.argv.slice(2));
const before = new Map();
const snapshot = path => {
  try { return statSync(path, {bigint: true}); }
  catch (error) {
    if (error.code === 'ENOENT') return null;
    throw error;
  }
};
for (const output of outputs) {
  mkdirSync(dirname(output.host), {recursive: true});
  before.set(output.host, snapshot(output.host));
}
// TH13_DUMPCHECK / TH13_SHOT_PROBE / TH13_LIFE_PROBE / TH13_STATE_LOG
// （可选宿主环境变量）：透传给 wasm——前者 harness 侧 dump 层 2 插值通道字段，
// 中者 StageBattle 出膛/命中探针，再次者敌机生命轨迹对拍探针，末者 replay
// 模式对局状态流输出路径（guest 路径，oracle battle_oracle TH13_STATE_LOG
// 对等，取证脚手架，见 tests/harness/harness.cpp 与 oracle/state_compare.py）。
// TH13_FREEZE_AT / TH13_NO_ECL_SLOT_ANM（r4/harness）：replay 同源路径的
// freeze 门/ANM sync 诊断自证开关（只观察，不改评分字段）。
const env = {};
for (const key of ['TH13_DUMPCHECK', 'TH13_SHOT_PROBE', 'TH13_LIFE_PROBE', 'TH13_STATE_LOG',
                   'TH13_FREEZE_AT', 'TH13_NO_ECL_SLOT_ANM'])
  if (process.env[key]) env[key] = process.env[key];
const wasi = new WASI({version: 'preview1', args, env, preopens: {'/': workspace}, returnOnExit: true});
const module = await WebAssembly.compile(bytes);
const instance = await WebAssembly.instantiate(module, wasi.getImportObject());
const code = wasi.start(instance) ?? 0;
if (code !== 0) process.exit(code);
for (const output of outputs) {
  validateHarnessOutput(output, before.get(output.host), snapshot(output.host));
  console.log(`harness-output ${output.host}`);
}
process.exit(0);
