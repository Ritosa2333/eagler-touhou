import {spawn} from 'node:child_process';
import {fileURLToPath} from 'node:url';
import {TURN_NODE_TESTS} from './test-plan.mjs';

const root = fileURLToPath(new URL('../', import.meta.url));
const child = spawn(process.execPath, ['--test', ...TURN_NODE_TESTS],
  {cwd:root, stdio:'inherit', windowsHide:true});
child.once('error', error => {
  console.error(`Unable to start TURN tests: ${error.message}`);
  process.exitCode = 1;
});
child.once('close', code => {process.exitCode = code ?? 1;});
