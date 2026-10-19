import {resolve} from 'node:path';
import {preflightFrozenOracle,runFrozenOracle,runOracleMaintenance} from './frozen-oracle.mjs';

const options={},seen=new Set();
for (const argument of process.argv.slice(2)) {
  const match=argument.match(/^--(root|name|mode|task)=(.+)$/);
  if (!match||seen.has(match[1])) throw new Error(`Invalid oracle argument: ${argument}`);
  seen.add(match[1]);
  options[match[1]]=match[2];
}
if (!options.root) throw new Error('Required: --root=PORT_WEB_DIRECTORY');
const root=resolve(options.root);
const mode=options.mode??'candidate';
let result;
if (mode==='candidate') {
  if (options.task) throw new Error('--task is only valid in maintenance mode');
  result=await runFrozenOracle({...options,root,concurrency:Number(process.env.EAGLER_ORACLE_JOBS??2)});
} else if (mode==='preflight') {
  if (options.task||options.name) throw new Error('Preflight mode accepts only --root and --mode');
  result=await preflightFrozenOracle({root});
  result.passed=result.groups.length>0&&result.groups.every(group=>group.status==='ready');
} else if (mode==='maintenance') {
  if (!options.task) throw new Error('Maintenance mode requires --task=TASK_ID');
  if (!options.name) throw new Error('Maintenance mode requires a unique --name=EVIDENCE_ID');
  result=await runOracleMaintenance({root,taskId:options.task,name:options.name});
} else throw new Error(`Unsupported oracle mode: ${options.mode}`);
console.log(JSON.stringify(result,null,2));
if (!result.passed) process.exitCode=1;
