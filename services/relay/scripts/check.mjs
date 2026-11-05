import {spawn} from 'node:child_process';
import {fileURLToPath} from 'node:url';
import {RELAY_NODE_TESTS} from './test-plan.mjs';
const root=fileURLToPath(new URL('../',import.meta.url));
const child=spawn(process.execPath,['--test','--test-concurrency=4',...RELAY_NODE_TESTS],{cwd:root,stdio:'inherit',windowsHide:true});
child.once('error',error=>{throw error;});
child.once('exit',code=>{process.exitCode=code??1;});
