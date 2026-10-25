import {resolve} from 'node:path';
import {existsSync} from 'node:fs';
import {randomUUID} from 'node:crypto';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {fileIdentity,dependencySnapshot} from '../testing/dependency-snapshot.mjs';
import {reviewDiagnosticReference} from '../../tests/adaptation/adapters/diagnostic-reference-evidence.mjs';

const arg=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
const requested=arg('output'),occupied=requested&&existsSync(resolve(requested));
const output=resolve(requested&&!occupied?requested:`artifacts/validation/diagnostic-reference-review-${randomUUID()}`);
const result=await runWithReport({output,schema:'eagler-touhou/diagnostic-reference-review/1'},async report=>{
  assertCliArguments(process.argv.slice(2),{values:['game','variant','report','output']});
  if(occupied)throw Error('Refusing to replace existing evidence: '+resolve(requested));
  for(const name of ['game','report','output'])if(!arg(name))throw Error(`Required: --${name}=VALUE`);
  report.scope={game:arg('game'),variant:arg('variant')||'normal'};report.newBrowserRun=false;
  report.harnessSources=await dependencySnapshot(process.cwd(),['tools/ports/diagnostic-reference-review.mjs']);
  const path=resolve(arg('report'));report.evidence={sourceReport:{path,...await fileIdentity(path)}};
  report.checks=[{id:'VERIFY-04',status:'passed',complete:true,scope:'Original Quick and Daily reference/input inventories and executed candidate queues',
    observation:reviewDiagnosticReference({...report.evidence,product:report.scope})}];report.passed=true;
});
if(!result.passed){process.exitCode=1;console.error(result.failure);}else console.log('VERIFY-04 passed | original capture and comparison verdicts retained');
