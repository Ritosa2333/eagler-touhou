import {resolve} from 'node:path';
import {existsSync} from 'node:fs';
import {writeFile} from 'node:fs/promises';
import {randomUUID} from 'node:crypto';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {fileIdentity,dependencySnapshot} from '../testing/dependency-snapshot.mjs';
import {assertLanguageReview,languageReviewMaterials} from '../../tests/adaptation/adapters/language-review-evidence.mjs';

const arg=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3),prepare=process.argv.includes('--prepare');
const requested=arg('output'),occupied=requested&&existsSync(resolve(requested));
const output=resolve(requested&&!occupied?requested:`artifacts/validation/language-review-cli-${randomUUID()}`);
const report=await runWithReport({output,schema:prepare?'eagler-touhou/language-review-materials/1':'eagler-touhou/language-review/1'},async report=>{
  assertCliArguments(process.argv.slice(2),{values:['game','variant','report','review','output'],flags:['prepare']});
  if(occupied)throw Error('Refusing to replace an existing review evidence directory: '+resolve(requested));
  for(const name of ['game','report','output',...(prepare?[]:['review'])])if(!arg(name))throw Error(`Required: --${name}=VALUE`);
  if(prepare&&arg('review'))throw Error('Select preparation or a completed review record');
  report.scope={game:arg('game'),variant:arg('variant')||'normal'};report.newBrowserRun=false;
  report.harnessSources=await dependencySnapshot(process.cwd(),['tools/ports/language-review.mjs']);
  const receipt=async path=>({path:resolve(path),...await fileIdentity(resolve(path))});
  report.evidence={sourceReport:await receipt(arg('report'))};
  if(prepare){
    const prepared=languageReviewMaterials({...report.evidence,product:report.scope});
    report.sourceRun=prepared.sourceRun;report.materials=prepared.materials;
    const template={schema:'eagler-touhou/scene-review/1',...report.scope,reviewer:'',recordedAt:null,
      groups:[{id:'text-presentation',verdict:'pending',materials:prepared.materials}]};
    await writeFile(resolve(output,'review.json'),JSON.stringify(template,null,2)+'\n');
    const lines=[`# ${report.scope.game} | 文字显示场景集`,'',
      '集中检查字形可读性、裁切和遮挡。机器已核对本次语言包、实际源字节、字形与呈现。每组填写一次结论。','',
      `原采集：${prepared.sourceRun.startedAt} | ${prepared.sourceRun.finishedAt}`,''];
    for(const locale of [...new Set(prepared.materials.map(material=>material.locale))]){
      const material=scene=>prepared.materials.find(item=>item.locale===locale&&item.scene===scene);
      const link=item=>'![图片](<'+item.path.replaceAll('\\','/')+'>)';
      lines.push(`## ${locale}`,'','| 统计菜单 | 对白 |','| --- | --- |',`| ${link(material('statistics'))} | ${link(material('dialogue'))} |`,'');
    }
    await writeFile(resolve(output,'review.md'),lines.join('\n')+'\n');
  }else{
    report.evidence.review=await receipt(arg('review'));
    report.checks=[{id:'LANG-05',status:'passed',complete:true,scope:'Exact reviewed native language scene set and original capture',
      observation:assertLanguageReview({...report.evidence,product:report.scope})}];
  }
  report.passed=true;
});
if(!report.passed){process.exitCode=1;console.error(report.failure);}else console.log(prepare?'Language review material prepared | approval pending':'LANG-05 passed | original capture times retained');
