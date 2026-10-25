import {resolve} from 'node:path';
import {existsSync} from 'node:fs';
import {randomUUID} from 'node:crypto';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {fileIdentity,dependencySnapshot} from '../testing/dependency-snapshot.mjs';
import {assertPracticeLanguageReview} from '../../tests/adaptation/adapters/practice-language-evidence.mjs';

const arg=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
const requested=arg('output'),occupied=requested&&existsSync(resolve(requested));
const output=resolve(requested&&!occupied?requested:`artifacts/validation/practice-language-review-cli-${randomUUID()}`);
const report=await runWithReport({output,schema:'eagler-touhou/practice-language-review/1'},async report=>{
  assertCliArguments(process.argv.slice(2),{values:['game','variant','report','review','output']});
  if(occupied)throw Error('Refusing to replace an existing review evidence directory: '+resolve(requested));
  for(const key of ['game','report','review','output'])if(!arg(key))throw Error(`Required: --${key}=VALUE`);
  report.scope={game:arg('game'),variant:arg('variant')||'normal'};report.newBrowserRun=false;
  report.harnessSources=await dependencySnapshot(process.cwd(),['tools/ports/practice-language-review.mjs']);
  const receipt=async path=>({path:resolve(path),...await fileIdentity(resolve(path))});
  report.evidence={sourceReport:await receipt(arg('report')),review:await receipt(arg('review'))};
  const observation=assertPracticeLanguageReview({...report.evidence,product:report.scope});
  report.checks=[{id:'PRACTICE-07',status:'passed',complete:true,scope:'Exact reviewed native locale screenshots and their original execution',observation}];
  report.passed=true;
});
if(!report.passed){process.exitCode=1;console.error(report.failure);}else console.log('PRACTICE-07 passed | evidence recomputed | original capture times retained');
