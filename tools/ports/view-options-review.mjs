import {resolve} from 'node:path';
import {existsSync} from 'node:fs';
import {writeFile} from 'node:fs/promises';
import {randomUUID} from 'node:crypto';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {fileIdentity,dependencySnapshot} from '../testing/dependency-snapshot.mjs';
import {assertViewOptionsReview,viewOptionsReviewMaterials} from '../../tests/adaptation/adapters/view-options-review-evidence.mjs';

const arg=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3),prepare=process.argv.includes('--prepare');
const requested=arg('output'),occupied=requested&&existsSync(resolve(requested));
const output=resolve(requested&&!occupied?requested:`artifacts/validation/view-options-review-cli-${randomUUID()}`);
const report=await runWithReport({output,schema:prepare?'eagler-touhou/view-options-review-materials/1':'eagler-touhou/view-options-review/1'},async report=>{
  assertCliArguments(process.argv.slice(2),{values:['game','variant','report','review','output'],flags:['prepare']});
  if(occupied)throw Error('Refusing to replace an existing review evidence directory: '+resolve(requested));
  for(const name of ['game','report','output',...(prepare?[]:['review'])])if(!arg(name))throw Error(`Required: --${name}=VALUE`);
  if(prepare&&arg('review'))throw Error('Select preparation or a completed review record');
  report.scope={game:arg('game'),variant:arg('variant')||'normal'};report.newBrowserRun=false;
  report.harnessSources=await dependencySnapshot(process.cwd(),['tools/ports/view-options-review.mjs']);
  const receipt=async path=>({path:resolve(path),...await fileIdentity(resolve(path))});
  report.evidence={sourceReport:await receipt(arg('report'))};
  if(prepare){
    const prepared=viewOptionsReviewMaterials({...report.evidence,product:report.scope});
    report.sourceRun=prepared.sourceRun;report.materials=prepared.materials;
    await writeFile(resolve(output,'review.json'),JSON.stringify({schema:'eagler-touhou/scene-review/1',...report.scope,reviewer:'',recordedAt:null,
      groups:[{id:'player-marker',verdict:'pending',materials:prepared.materials}]},null,2)+'\n');
    const lines=[`# ${report.scope.game} | 判定点场景集`,'',
      '集中查看判定点是否贴合角色、低速与常显设置对应的画面是否正确。整组填写一次结论。机器另行核对输入、位置、碰撞盒及重开。','',
      `原采集：${prepared.sourceRun.startedAt} | ${prepared.sourceRun.finishedAt}`,''];
    for(const image of prepared.materials)lines.push(`## ${image.name}`,'','![判定点](<'+image.path.replaceAll('\\','/')+'>)','');
    await writeFile(resolve(output,'review.md'),lines.join('\n')+'\n');
  }else{
    report.evidence.review=await receipt(arg('review'));
    report.checks=[{id:'RUNTIME-06',status:'passed',complete:true,scope:'Exact reviewed native player-marker scenes and original machine capture',
      observation:assertViewOptionsReview({...report.evidence,product:report.scope})}];
  }
  report.passed=true;
});
if(!report.passed){process.exitCode=1;console.error(report.failure);}else console.log(prepare?'Player-marker review material prepared | approval pending':'RUNTIME-06 passed | original capture times retained');
