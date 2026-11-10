import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const args = process.argv.slice(2);
const printInputs = args.includes('--print-inputs');
// --print-inputs is an inventory-only build.mjs mode: it exits before producing
// a Runtime, so package-eagler.mjs must not run. Forward the flag and every
// other argument so build.mjs can see the same command line as the caller.
for (const script of printInputs ? ['build.mjs'] : ['build.mjs', 'package-eagler.mjs']) {
  // package-eagler 需要与 build.mjs 相同的变体参数（--multiplayer）：launcher
  // runtime-builds 的 sourceBuild.args 传到本入口；丢掉会让多人变体静默产出
  // 单席 manifest（桥因此永不激活）。
  const childArgs = [fileURLToPath(new URL(script, import.meta.url)), ...args];
  const result = spawnSync(process.execPath, childArgs, {
    stdio: 'inherit', env: process.env,
  });
  if (result.error) throw result.error;
  if (result.signal || result.status !== 0) throw Error(`${script} failed: ${result.signal || result.status}`);
}
