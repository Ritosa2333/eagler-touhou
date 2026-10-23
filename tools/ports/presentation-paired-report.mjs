import {isDeepStrictEqual as same} from 'node:util';
import {readFile,writeFile} from 'node:fs/promises';
import {readFileSync,lstatSync,realpathSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve,dirname,isAbsolute} from 'node:path';
import {pathToFileURL} from 'node:url';
import {compareReplayCadenceRuns,assessReplayReadEvidence} from '../../common/testkit/presentation-lab/replay-pair.mjs';
import {applyPresentationAudioEvidence} from './presentation-audio-evidence.mjs';
import {assessFirstFrameEvidence} from './presentation-first-frame.mjs';
import {assessProductionReplayEvidence} from './presentation-production-replay.mjs';

const SHA=/^[a-f0-9]{64}$/;
/** Read saved production files again; report flags cannot replace their bytes. */
export function readProductionPresentationEvidence(ref){
  const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
  const read=identity=>{
    const path=typeof identity==='string'?resolve(identity):identity?.path;
    if(!isAbsolute(path??'')||!lstatSync(path).isFile()||lstatSync(path).isSymbolicLink()||
      resolve(realpathSync(path)).toLowerCase()!==resolve(path).toLowerCase())throw Error('Production evidence must use a regular canonical file');
    if(lstatSync(path).size>128*1024*1024)throw Error('Production evidence exceeds its byte bound');
    const bytes=readFileSync(path);
    if(typeof identity!=='string'&&(bytes.length!==identity.bytes||hash(bytes)!==identity.sha256))throw Error('Production evidence bytes differ from saved identity');
    return {value:JSON.parse(bytes),identity:{path,bytes:bytes.length,sha256:hash(bytes)}};
  };
  const {value:report,identity}=read(ref);
  if(report.schema!=='eagler-touhou/runtime-browser/1'||report.targets?.length!==1||!report.finishedAt||
    report.failure||!Array.isArray(report.httpFailures)||report.httpFailures.length||!SHA.test(report.harnessSha256??'')||
    report.harnessSha256!==report.harnessPostflightSha256)throw Error('Production browser report is unfinished or failed');
  const target=report.targets[0];
  if(target.variant!=='normal'||target.failure||!Array.isArray(target.checks)||target.checks.some(row=>row.status==='failed')||target.finalErrorAudit?.status!=='passed'||
    ['errors','consoleErrors','httpErrors','protocolErrors'].some(key=>!Array.isArray(target[key])||target[key].length)||
    !same(target.packageIdentity,target.inputPostflight?.packageIdentity)||!same(target.dataIdentity,target.inputPostflight?.dataIdentity))
    throw Error('Production browser identity or error audit failed');
  const rawRefs=[target.presentationProduction,...(target.presentationFirstFrames??[])];
  for(const rawRef of rawRefs)if(dirname(rawRef?.path??'')!==dirname(identity.path))throw Error('Production raw files belong to another browser report directory');
  const run=read(target.presentationProduction).value,firstFrameRuns=(target.presentationFirstFrames??[]).map(item=>read(item).value);
  return {report:identity,run,firstFrameRuns,scope:{game:target.game,variant:target.variant},
    build:target.packageIdentity,data:target.dataIdentity};
}
/** Derive numbered claims from saved data. No capture status alone proves a claim. */
export function buildPresentationPairedReport({runs,requiredTicks,fixture,input,scope={game:'th08',variant:'normal'},
  ratesHz=[60,120,144],expectedAuthorityGroups=17,
  cadenceDifferenceGroups=['host-frame-cadence','synchronous-audio-operations'],productionEvidence=null}={}){
  if(!Array.isArray(runs)||runs.length!==3)throw Error('Three actual cadence captures are required');
  const options={requiredTicks,ratesHz,scope,expectedAuthorityGroups,cadenceDifferenceGroups};
  const logicalComparison=compareReplayCadenceRuns({runs,...options});
  const cadenceComparison=applyPresentationAudioEvidence(logicalComparison,runs,options);
  const rawReads=runs.map(run=>assessReplayReadEvidence(run,{requiredTicks,expectedAuthorityGroups,
    nativeWordLengths:{status:29,trace:68,audioAudit:25}}));
  const production=runs[0]?.identity?.build?.productionBuild,diagnostic=runs[0]?.identity?.build;
  const buildBound=diagnostic?.diagnostic===true&&diagnostic.instrumented===true&&production?.available===true&&
    production.diagnostic===false&&production.sourceMatchesCurrent===true&&production.binaryMatchesDescriptor===true&&
    SHA.test(production.wasmSha256??'')&&production.wasmSha256!==diagnostic.wasm&&
    runs.every(run=>same(run.identity.build.productionBuild,production));
  const productionAssessment=productionEvidence?assessProductionReplayEvidence({productionRun:productionEvidence.run,runs,
    requiredTicks,scope,expectedBuild:productionEvidence.build,expectedData:productionEvidence.data,expectedReplay:input}):null;
  const verify07=rawReads.some(row=>row.status==='failed')||productionAssessment?.status==='failed'?'failed':
    buildBound&&rawReads.every(row=>row.status==='passed')&&productionAssessment?.status==='passed'?'passed':'incomplete';
  const drawEvidence=runs.slice(1).every(run=>run.status==='passed'&&run.samples?.length===requiredTicks&&
    run.capture?.drawAlphas?.join(',')==='0,0.5,1'&&run.samples.every(sample=>sample.drawReceipts?.length===3&&
      sample.drawReceipts.every((receipt,index)=>receipt.alpha===[0,0.5,1][index]&&receipt.status==='drawn'&&
        receipt.unchanged===true&&same(receipt.before,receipt.after)&&receipt.audio?.events?.length===0&&
        receipt.audio?.routes?.length===0&&receipt.audio?.uploads===0&&receipt.audio?.errors===0)));
  const display03=logicalComparison.status==='failed'||rawReads.some(row=>row.status==='failed')?'failed':
    drawEvidence&&rawReads.every(row=>row.status==='passed')?'passed':'incomplete';
  const firstFrameAssessment=productionEvidence?assessFirstFrameEvidence({runs:productionEvidence.firstFrameRuns,scope,
    expectedBuild:productionEvidence.build,expectedData:productionEvidence.data}):null;
  const firstFrameStatus=firstFrameAssessment?.status??'incomplete';
  const display07=firstFrameStatus==='failed'?'failed':firstFrameStatus==='passed'&&firstFrameAssessment.complete===true?'passed':'incomplete';
  const bound=(id,status,complete,evidence)=>({id,status,complete,machineComplete:complete,
    evidence:{game:scope.game,variant:scope.variant,adaptationIds:[id],...evidence}});
  const checks=[...cadenceComparison.checks,
    bound('DISPLAY-02','incomplete',false,{reason:'Physical display refresh has not been measured in this cloud session'}),
    bound('DISPLAY-03',display03,display03==='passed',{extraDrawsPerLogicalTick:3,controlledCadences:[120,144]}),
    bound('DISPLAY-07',display07,display07==='passed',{firstFrameReceipts:runs.map(run=>run.firstFrame??null),
      productionFirstFrame:firstFrameAssessment,
      boundary:'Actual production cold/reload compositor frame, retained image, native present and protocol send',
      compositorSubmissionMeasured:display07==='passed',physicalScanoutMeasured:false}),
    bound('VERIFY-07',verify07,verify07==='passed',{repeatedReadGroups:expectedAuthorityGroups,runtimeEpochs:runs.map(run=>run.epoch),
      actualReadAssessments:rawReads,productionBuild:production??null,productionReplay:productionAssessment,
      diagnosticBuild:{profile:diagnostic?.profile,wasm:diagnostic?.wasm,instrumented:diagnostic?.instrumented},
      extent:'Both actual native reads at every selected boundary and independent ordinary Runtime Replay execution'})];
  const issues=[...logicalComparison.issues];
  if(cadenceComparison.checks.find(row=>row.id==='AUDIO-06')?.status!=='passed')issues.push('Actual output PCM evidence is incomplete or contradicted');
  if(display03!=='passed')issues.push('Controlled extra draw purity did not pass at both high-rate callback schedules');
  if(display07!=='passed')issues.push('Production cold/reload first-frame compositor evidence is incomplete or contradicted');
  if(verify07!=='passed')issues.push('Production/diagnostic actual repeated-read evidence is incomplete or contradicted');
  const required=checks.filter(row=>row.id!=='DISPLAY-02');
  const status=required.some(row=>row.status==='failed')?'failed':required.every(row=>row.status==='passed')?'passed':'incomplete';
  return {schema:'eagler-presentation/replay-pair/1',scope,status,issues,fixture,
    input:{bytes:input?.bytes,sha256:input?.sha256},pairedTicks:requiredTicks,comparedTicks:requiredTicks,
    baselineComparison:null,cadenceComparison,checks,
    runEvidence:runs.map(run=>({role:run.role,epoch:run.epoch,rateHz:run.scheduler?.rateHz,
      identity:run.identity,replayIdentity:run.replayIdentity,firstFrame:run.firstFrame})),
    claims:{productionSchedulerCadenceVerified:logicalComparison.status==='passed',controlledExtraDrawPurity:display03==='passed',
      physicalRefreshVerified:false,browserCompositorSubmissionMeasured:display07==='passed',physicalScanoutMeasured:false,
      replayNaturalTerminalVerified:false,rollbackAudioVerified:false}};
}

if(process.argv[1]&&import.meta.url===pathToFileURL(resolve(process.argv[1])).href){
  const arg=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
  const root=resolve(arg('input-dir')??''),fixture=arg('fixture'),requiredTicks=Number(arg('ticks'));
  if(!root||!/^demo[0-3]$/.test(fixture??'')||!Number.isSafeInteger(requiredTicks)||requiredTicks<1||requiredTicks>3600)
    throw Error('Required: --input-dir=... --fixture=demo0..demo3 --ticks=1..3600');
  const runs=await Promise.all(['baseline','controlled-extra-draw','cadence-144hz'].map(async role=>
    JSON.parse(await readFile(resolve(root,`paired-replay-${fixture}-${role}.json`),'utf8'))));
  const productionEvidence=arg('production-report')?readProductionPresentationEvidence(arg('production-report')):null;
  const report=buildPresentationPairedReport({runs,requiredTicks,fixture,input:runs[0].replayIdentity,productionEvidence});
  await writeFile(resolve(root,`paired-replay-${fixture}-result.json`),JSON.stringify(report,null,2)+'\n');
  console.log(JSON.stringify({status:report.status,checks:report.checks.map(({id,status,complete})=>({id,status,complete}))}));
}
