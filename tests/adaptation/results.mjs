import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {fileIdentity} from '../../tools/testing/dependency-snapshot.mjs';

const DRIVER_SCHEMAS=new Set(['runtime-browser','runtime-desktop-probes','practice-browser','replay-browser',
  'music-browser','offline-runtime-browser','room-browser'].map(name=>`eagler-touhou/${name}/1`));
const failed=value=>value?.status==='failed'||value?.passed===false&&
  !['incomplete','partial','blocked','not-run','not-applicable','preflight-only'].includes(value?.status);
const checkIds=entry=>/^[A-Z]+(?:-[A-Z]+)?-\d+$/.test(entry.id??'')?[entry.id]:
  Array.isArray(entry.checks)?entry.checks:Array.isArray(entry.adaptationIds)?entry.adaptationIds:[];

/** Preserve the driver's exact scope; a zero process exit cannot hide failed assertions. */
export function assessDriverReport(testCase,processResult,driver){
  const issues=[];
  if(!driver||!DRIVER_SCHEMAS.has(driver.schema))issues.push('Unknown or missing driver report schema');
  if(!driver?.runId||!driver.finishedAt||driver.status==='running')issues.push('Driver report is not sealed');
  const started=Date.parse(driver?.startedAt),finished=Date.parse(driver?.finishedAt);
  if(!Number.isFinite(started)||!Number.isFinite(finished)||finished<started)issues.push('Invalid driver report times');
  if(processResult.startedAt&&started<Date.parse(processResult.startedAt)-2000)issues.push('Driver report predates this invocation');
  const targets=driver?.targets??[];
  if(targets.length){
    if(targets.length!==1||targets[0].game!==testCase.game||
      targets[0].variant!==testCase.variant)issues.push('Driver report belongs to another product or selection');
  }else if(driver?.scope?.game!==testCase.game||driver?.scope?.variant!==testCase.variant)
    issues.push('Driver report does not identify the selected product');
  const scopes=[driver,...targets],checks=[],scenarios=[];
  for(const scope of scopes){
    if(failed(scope)||scope.status==='failed')issues.push(scope.error??scope.failure??'Driver or target failed');
    if(scope.finalErrorAudit?.status==='failed')issues.push('Final browser error audit failed');
    for(const entry of scope.checks??[]){
      for(const id of checkIds(entry))checks.push({id,status:entry.status??(entry.passed===true?'passed':'unknown'),
        complete:entry.complete===true,scope:entry.scope??entry.id,error:entry.error??null});
      if(failed(entry)||entry.status==='failed')issues.push(entry.error??`Failed driver check: ${entry.id}`);
    }
    for(const entry of [...(scope.scenarios??[]),...(scope.scenario?[scope.scenario]:[]),
      ...(scope.locales??[]),...(scope.modes??[])]){
      scenarios.push({id:entry.id??entry.locale??entry.mode??null,status:entry.status,passed:entry.passed});
      // Music's NONE mode is a diagnostic control; the required OGG scene has its own result.
      if(entry.status==='failed'||entry.mode!=='none'&&failed(entry))issues.push(entry.error??`Failed driver scene: ${entry.id??entry.locale??entry.mode}`);
      if(entry.finalErrorAudit?.status==='failed')issues.push('Scene final browser error audit failed');
    }
  }
  const completed=value=>value?.status==='passed'||value?.status===undefined&&value?.passed===true;
  const incomplete=driver?.status==='incomplete'||targets.some(target=>!completed(target))||
    scenarios.some(scene=>scene.id!=='none'&&!['passed','observed','recorded'].includes(scene.status))||
    (testCase.blockedScenarios?.length??0)>0;
  const nonzero=processResult.code!==0||processResult.status!=='passed';
  let status=processResult.status==='timeout'?'timeout':processResult.status==='cancelled'?'cancelled':
    issues.length?'failed':incomplete?'incomplete':nonzero?'failed':driver?.status==='passed'&&driver.passed===true?'passed':'failed';
  if(status==='failed'&&!issues.length)issues.push(nonzero?`Driver process exited ${processResult.code??processResult.signal}`:'Driver did not report passed=true');
  return{status,issues:[...new Set(issues)],checks,scenarios,
    driverStatus:driver?.status??'missing',driverRunId:driver?.runId??null,
    adaptationComplete:driver?.coverage?.complete===true};
}

export async function readCaseReport(testCase,processResult){
  try{
    const identity=await fileIdentity(testCase.reportPath),driver=JSON.parse(await readFile(testCase.reportPath,'utf8'));
    return{...assessDriverReport(testCase,processResult,driver),report:{path:testCase.reportPath,...identity}};
  }catch(error){return{status:processResult.status==='timeout'?'timeout':processResult.status==='cancelled'?'cancelled':'failed',
    issues:[`Cannot verify sealed driver report: ${error.message}`],checks:[],scenarios:[],report:null};}
}

/** Reusing a result retains its original evidence instead of claiming another execution. */
export async function reusableResult(previous,currentFingerprint){
  if(!previous||previous.fingerprint!==currentFingerprint||!previous.finishedAt||!previous.report)return false;
  if(previous.identityStable===false||!previous.process)return false;
  if(!['passed','failed','incomplete','blocked','timeout','cancelled'].includes(previous.status))return false;
  try{
    const actual=await fileIdentity(previous.report.path);
    if(actual.bytes!==previous.report.bytes||actual.sha256!==previous.report.sha256)return false;
    const driver=JSON.parse(await readFile(previous.report.path,'utf8'));
    const assessed=assessDriverReport(previous,previous.process,driver);
    return assessed.status===previous.status&&assessed.driverRunId===previous.driverRunId;
  }catch{return false;}
}

export function summarizeAdaptationCases(cases){
  const count=status=>cases.filter(item=>item.status===status).length;
  const failure=cases.some(item=>['failed','timeout'].includes(item.status));
  const passed=cases.length>0&&cases.every(item=>['passed','not-applicable'].includes(item.status));
  return{status:failure?'failed':passed?'passed':'incomplete',passed,
    counts:Object.fromEntries(['passed','failed','incomplete','blocked','timeout','cancelled','not-run','not-applicable'].map(status=>[status,count(status)])),
    executed:cases.filter(item=>item.execution==='executed').length,retained:cases.filter(item=>item.execution==='retained').length};
}

export function adaptationCoverage(cases,{checks=[],unmappedChecks=[]}={}){
  const rows=[];
  for(const target of new Set(cases.map(item=>`${item.game}/${item.variant}`))){
    const [game,variant]=target.split('/'),selected=cases.filter(item=>item.game===game&&item.variant===variant);
    const ids=checks.length?checks:[...new Set(selected.flatMap(item=>item.checksMapped??[]))];
    for(const id of ids){
      const related=selected.filter(item=>item.checksMapped?.includes(id));
      const evidence=related.flatMap(item=>(item.checks??[]).filter(check=>check.id===id).map(check=>({...check,caseId:item.id,execution:item.execution})));
      const failed=evidence.some(item=>item.status==='failed');
      const complete=evidence.some(item=>item.status==='passed'&&item.complete)&&related.every(item=>item.status==='passed');
      rows.push({game,variant,id,status:failed?'failed':complete?'complete':related.length?'partial':'unmapped',
        complete,evidence,caseIds:related.map(item=>item.id)});
    }
  }
  return{complete:rows.length>0&&unmappedChecks.length===0&&rows.every(row=>row.complete),rows,unmappedChecks};
}

export function assertPreviousRun(report){
  assert.equal(report?.schema,'eagler-touhou/adaptation-run/1','Resume requires an adaptation-run report');
  assert.ok(Array.isArray(report.cases),'Resume report has no cases');
  assert.equal(new Set(report.cases.map(item=>item.id)).size,report.cases.length,'Resume report has duplicate case IDs');
}
