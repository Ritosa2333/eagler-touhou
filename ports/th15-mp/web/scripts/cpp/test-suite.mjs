import {fileURLToPath} from 'node:url';
import {runFrozenOracle} from '../../../../../tools/testing/frozen-oracle.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
const result=await runFrozenOracle({root,name:process.argv[2]??'full-suite',concurrency:Number(process.env.TH15_TEST_CONCURRENCY??2)});
console.log(JSON.stringify(result,null,2));
if(!result.passed)process.exitCode=1;
