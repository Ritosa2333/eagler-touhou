import {spawn} from 'node:child_process';
import {fileURLToPath} from 'node:url';
import {discoverNodeTests} from '../../../tools/testing/discovery.mjs';

const root = fileURLToPath(new URL('../', import.meta.url));
const tests = (await discoverNodeTests(root + '/tests')).map(name => 'tests/' + name);
if (!tests.length) throw Error('No TURN tests found');
const child = spawn(process.execPath, ['--test', ...tests], {cwd: root, stdio: 'inherit', windowsHide: true});
child.once('error', error => {console.error(error); process.exitCode = 1;});
child.once('close', code => {process.exitCode = code ?? 1;});
