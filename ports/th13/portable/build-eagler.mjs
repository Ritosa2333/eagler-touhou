import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const args = process.argv.slice(2);
const printInputs = args.includes('--print-inputs');
for (const script of printInputs ? ['build.mjs'] : ['build.mjs', 'package-eagler.mjs']) {
  const childArgs = [fileURLToPath(new URL(script, import.meta.url)),
    ...(script === 'build.mjs' ? args : [])];
  const result = spawnSync(process.execPath, childArgs, {
    stdio: 'inherit', env: process.env,
  });
  if (result.error) throw result.error;
  if (result.signal || result.status !== 0) throw Error(`${script} failed: ${result.signal || result.status}`);
}
