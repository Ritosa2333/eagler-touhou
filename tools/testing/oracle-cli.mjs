import {resolve} from 'node:path';
import {runFrozenOracle} from './frozen-oracle.mjs';

const options={};
for (const argument of process.argv.slice(2)) {
  const match=argument.match(/^--(root|name)=(.+)$/);
  if (!match||Object.hasOwn(options,match[1])) throw new Error(`Invalid oracle argument: ${argument}`);
  options[match[1]]=match[2];
}
if (!options.root) throw new Error('Required: --root=PORT_WEB_DIRECTORY');
const result=await runFrozenOracle({...options,root:resolve(options.root),concurrency:Number(process.env.EAGLER_ORACLE_JOBS??2)});
console.log(JSON.stringify(result,null,2));
if (!result.passed) process.exitCode=1;
