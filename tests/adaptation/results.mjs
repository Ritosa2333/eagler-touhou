import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {fileIdentity,identityDigest} from '../../tools/testing/dependency-snapshot.mjs';
import {assessCompleteEvidence} from './complete-evidence.mjs';

const DRIVER_SCHEMAS=new Set(['runtime-browser','runtime-desktop-probes','practice-browser','replay-browser',
  'music-browser','offline-runtime-browser','room-browser','diagnostic-browser','language-browser','build-acceptance',
  'minimal-runtime-browser','adaptation-source-review-result'].map(name=>`eagler-touhou/${name}/1`));
const failed=value=>value?.status==='failed'||value?.passed===false&&
  !['incomplete','partial','blocked','not-run','not-applicable','preflight-only'].includes(value?.status);
const checkIds=entry=>[...new Set([
  ...(/^[A-Z]+(?:-[A-Z]+)?-\d+$/.test(entry.id??'')?[entry.id]:[]),
  ...(Array.isArray(entry.checks)?entry.checks:[]),
  ...(Array.isArray(entry.adaptationIds)?entry.adaptationIds:[]),
])];
const structured=value=>value!==null&&typeof value==='object'&&Object.keys(value).length>0;
const evidenceMetadata=new Set(['game','variant','adaptationIds','status','passed','complete','id','scope','reason','metadata']);

function readEvidencePointer(report,pointer){
  assert.ok(typeof pointer==='string'&&pointer.startsWith('/')&&!/~(?![01])/u.test(pointer),'Evidence reference must be a JSON pointer');
  let value=report;
  const tokens=pointer.slice(1).split('/').map(token=>token.replaceAll('~1','/').replaceAll('~0','~'));
  for(const key of tokens){
    assert.ok(!['checks','scenarios','scenario','modes','locales','coverage','scope','selection'].includes(key),
      `Evidence reference cannot point into result metadata or another claim: ${pointer}`);
    assert.ok(value!==null&&typeof value==='object'&&Object.hasOwn(value,key),`Missing evidence reference ${pointer}`);
    value=value[key];
  }
  assert.ok(structured(value),`Evidence reference must identify structured observations: ${pointer}`);
  return{tokens,value};
}

function hasObservationPayload(value){
  if(Array.isArray(value))return value.length>0;
  return value!==null&&typeof value==='object'&&Object.keys(value).some(key=>!evidenceMetadata.has(key));
}

function checkProof(driver,entry,pointer,product){
  const references=[];
  const ids=checkIds(entry);
  for(const name of ['observation','evidence'])if(structured(entry[name])){
    const value=entry[name];
    if(value.game!==undefined)assert.equal(value.game,product.game,`${name} belongs to another product`);
    if(value.variant!==undefined)assert.equal(value.variant,product.variant,`${name} belongs to another variant`);
    if(value.adaptationIds!==undefined){
      assert.ok(ids.length>0&&Array.isArray(value.adaptationIds)&&ids.every(id=>value.adaptationIds.includes(id)),
        `${name} is not bound to every adaptation ID declared by its check`);
      assert.equal(value.game,product.game,`${name} must identify its product`);
      assert.equal(value.variant,product.variant,`${name} must identify its variant`);
      assert.ok(hasObservationPayload(value.observation),`${name} contains no actual observation payload`);
    }else assert.ok(hasObservationPayload(value),`${name} contains only metadata, not an observation`);
    references.push({pointer:`${pointer}/${name}`,sha256:identityDigest(value)});
  }
  if(entry.evidenceRefs!==undefined){
    assert.ok(Array.isArray(entry.evidenceRefs)&&entry.evidenceRefs.length>0,'evidenceRefs must be a nonempty array');
    for(const reference of entry.evidenceRefs){
      assert.ok(reference!==pointer&&!reference.startsWith(pointer+'/'),'Referenced observations must be outside the claim itself');
      const {tokens,value}=readEvidencePointer(driver,reference);
      assert.ok(value.game===product.game&&value.variant===product.variant,
        `Referenced observation must identify the selected product and variant: ${reference}`);
      assert.ok(Array.isArray(value.adaptationIds)&&ids.length>0&&ids.every(id=>value.adaptationIds.includes(id)),
        `Referenced observation is not bound to every adaptation ID declared by its check: ${reference}`);
      assert.ok(hasObservationPayload(value.observation),`Referenced observation has no actual payload: ${reference}`);
      if(tokens[0]==='targets'){
        const target=driver.targets?.[Number(tokens[1])];
        assert.ok(target&&target.game===product.game&&target.variant===product.variant,
          `Referenced observation belongs to another target: ${reference}`);
      }
      references.push({pointer:reference,sha256:identityDigest(value)});
    }
  }
  return references.length?{source:'driver-report',references}:null;
}

const executedStatuses=new Set(['passed','observed','recorded','partial','incomplete']);

function roomScenarioGaps(testCase,driver,scope){
  const spectator=testCase.group==='spectator';
  if(!['rooms','spectator'].includes(testCase.group))return null;
  const match=(testCase.scenarioIds??[]).map(id=>/^rooms-(2|3)p-(rtc|relay)$/.exec(id)).find(Boolean);
  const expectedPlayers=match?Number(match[1]):null,expectedRoute=match?.[2]??null;
  const gaps=[];
  if(!match)gaps.push('expected room scenario ID');
  if(scope.game!==testCase.game||scope.variant!=='multiplayer'||scope.players!==expectedPlayers||scope.route!==expectedRoute||
    driver.players!==expectedPlayers||driver.route!==expectedRoute)gaps.push('matching game, multiplayer variant, player count and route');
  if(scope.runtimePassed!==true||scope.passed!==true)gaps.push('runtimePassed and target passed');
  const expected=[
    ...Array.from({length:expectedPlayers??0},(_,seat)=>[`P${seat+1}-directory`,`P${seat+1}-room`,`P${seat+1}-ready`]).flat(),
    ...(spectator?['admit-spectator-before-start']:[]),
    'synchronized-start',
    ...(PRODUCT_GAMES[testCase.game]?.multiplayer?.inGameSelection?['native-versus-selection']:[]),
    'actual-native-frame-180',
    ...(spectator?['native-spectator-progress-and-keyboard-isolation']:['native-gamepad-sampling']),
    ...(!spectator?['multiplayer-native-pause-and-resume','multiplayer-native-restart-and-second-game']:[]),
    'capture-gameplay',
    ...(!spectator?['peer-departure-and-freeze']:[]),
  ];
  const stepNames=Array.isArray(scope.steps)?scope.steps.map(step=>typeof step==='string'?step:step?.name):[];
  for(const name of expected){
    const count=stepNames.filter(step=>step===name).length;
    if(count!==1)gaps.push(`${name} step${count===0?' missing':' duplicated'}`);
  }
  if(spectator){
    if(driver.spectatorRequested!==true)gaps.push('spectator mode requested');
  }else{
    if(driver.disconnectRequested!==true)gaps.push('disconnect mode requested');
    if(driver.lifecycleRequested!==true)gaps.push('lifecycle mode requested');
    if(!Array.isArray(scope.gamepad)||scope.gamepad.length!==expectedPlayers||scope.gamepad.some(item=>item?.passed!==true))
      gaps.push('native gamepad result for every player');
    if(scope.lifecycle?.coverage?.complete!==true||
      !['synchronized-pause-resume','synchronized-restart-second-game'].every(id=>
        scope.lifecycle?.checks?.some(check=>check?.id===id&&check.passed===true)))
      gaps.push('complete native pause, resume and restart lifecycle');
  }
  return gaps;
}

function plannedResultGaps(testCase,driver,scopes,checks,scenarios){
  const scenarioIds=testCase.scenarioIds??[];
  if(!scenarioIds.length)return[];

  // The room driver records the full operation protocol rather than shared
  // numbered checks. Validate that driver's declared operation contract.
  if(['rooms','spectator'].includes(testCase.group)){
    const targetScopes=scopes.filter(({scope})=>scope.game===testCase.game&&scope.variant===testCase.variant);
    if(targetScopes.length!==1)return['one matching room target'];
    return roomScenarioGaps(testCase,driver,targetScopes[0].scope);
  }

  const planned=[...new Set(testCase.checksMapped??testCase.checks??[])];
  if(planned.length){
    const missing=planned.filter(id=>!checks.some(check=>check.id===id&&executedStatuses.has(check.status)));
    return missing.length?[`numbered results for ${missing.join(', ')}`]:[];
  }

  // Cases without mapped adaptation IDs must report each declared scenario,
  // not any unrelated passing locale, mode or neighboring scene.
  const missing=scenarioIds.filter(id=>!scenarios.some(scene=>scene.id===id&&executedStatuses.has(scene.status)));
  return missing.length?[`results for scenario ${missing.join(', ')}`]:[];
}

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
  const scopes=[{scope:driver,pointer:''},...targets.map((scope,index)=>({scope,pointer:`/targets/${index}`}))],checks=[],scenarios=[];
  for(const {scope,pointer} of scopes){
    const product={game:scope.game??driver.scope?.game??testCase.game,
      variant:scope.variant??driver.scope?.variant??testCase.variant};
    if(failed(scope)||scope.status==='failed')issues.push(scope.error??scope.failure??'Driver or target failed');
    if(scope.finalErrorAudit?.status==='failed')issues.push('Final browser error audit failed');
    for(const [index,entry] of (scope.checks??[]).entries()){
      let proof=null,completeEvidence=null;
      try{proof=checkProof(driver,entry,`${pointer}/checks/${index}`,product);}
      catch(error){issues.push(`Invalid observation evidence for ${entry.id}: ${error.message}`);}
      if(entry.complete===true||entry.machineComplete===true){
        completeEvidence=assessCompleteEvidence({driver,entry,product});
        if(completeEvidence.available&&!completeEvidence.valid)
          issues.push(`Invalid complete evidence for ${entry.id}: ${completeEvidence.reason}`);
      }
      for(const id of checkIds(entry))checks.push({id,status:entry.status??(entry.passed===true?'passed':'unknown'),
        complete:entry.complete===true&&proof!==null&&completeEvidence?.valid===true,claimedComplete:entry.complete===true,proof,
        ...(entry.machineComplete!==undefined?{machineComplete:entry.machineComplete===true&&proof!==null&&completeEvidence?.valid===true,
          claimedMachineComplete:entry.machineComplete===true}:{}),
        scope:entry.scope??entry.id,error:entry.error??null,
        ...(completeEvidence?{completeEvidence}:{}),
        ...(entry.complete===true&&completeEvidence?.valid!==true?{proofIssue:completeEvidence?.reason??'Complete evidence was not independently assessed'}:{}),
        ...(entry.complete===true&&!proof?{proofIssue:'Complete adaptation evidence requires structured observations'}:{})});
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
  const plannedResultIssues=plannedResultGaps(testCase,driver,scopes,checks,scenarios);
  const plannedResultMissing=plannedResultIssues.length>0;
  const incompleteReasons=plannedResultMissing?[`Driver report omits planned scope: ${plannedResultIssues.join('; ')}`]:[];
  for(const {scope} of scopes){
    if(typeof scope.incompleteReason==='string'&&scope.incompleteReason.trim())incompleteReasons.push(scope.incompleteReason);
    for(const entry of scope.checks??[]){
      const reason=[entry.reason,entry.observation?.reason,entry.observation?.nativeSfxCorrelation?.reason]
        .find(value=>typeof value==='string'&&value.trim());
      if(['blocked','not-run','incomplete'].includes(entry.status)&&reason){
        const ids=checkIds(entry);
        incompleteReasons.push(`${ids.length?ids.join(', '):entry.id}: ${reason}`);
      }
    }
  }
  const selectedIds=new Set(testCase.checksMapped??testCase.checks??[]);
  const incomplete=driver?.status==='incomplete'||targets.some(target=>!completed(target))||
    checks.some(check=>check.claimedComplete&&!check.complete||check.claimedMachineComplete&&!check.machineComplete)||
    checks.some(check=>(!selectedIds.size||selectedIds.has(check.id))&&
      ['unknown','running','blocked','not-run','incomplete'].includes(check.status))||
    scenarios.some(scene=>scene.id!=='none'&&!['passed','observed','recorded'].includes(scene.status))||
    plannedResultMissing||
    (testCase.blockedScenarios?.length??0)>0;
  const nonzero=processResult.code!==0||processResult.status!=='passed';
  let status=processResult.status==='timeout'?'timeout':processResult.status==='cancelled'?'cancelled':
    issues.length?'failed':incomplete?'incomplete':nonzero?'failed':driver?.status==='passed'&&driver.passed===true?'passed':'failed';
  if(status==='failed'&&!issues.length)issues.push(nonzero?`Driver process exited ${processResult.code??processResult.signal}`:'Driver did not report passed=true');
  return{status,issues:[...new Set(issues)],incompleteReasons:[...new Set(incompleteReasons)],checks,scenarios,
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
  if(previous.identityStable!==true||!previous.process)return false;
  if(!['passed','failed','incomplete','blocked','timeout','cancelled'].includes(previous.status))return false;
  try{
    const actual=await fileIdentity(previous.report.path);
    if(actual.bytes!==previous.report.bytes||actual.sha256!==previous.report.sha256)return false;
    const driver=JSON.parse(await readFile(previous.report.path,'utf8'));
    // Replay declares additional native/oracle files chosen at runtime.
    for(const input of driver.decisionInputs??[]){
      const path=resolve(previous.task.cwd,input.path),current=await fileIdentity(path);
      if(current.bytes!==input.bytes||current.sha256!==input.sha256)return false;
    }
    const assessed=assessDriverReport(previous,previous.process,driver);
    if(assessed.status!==previous.status||assessed.driverRunId!==previous.driverRunId)return false;
    // Coverage is derived from the sealed child, including on resume. A parent
    // summary cannot upgrade partial observations into complete evidence.
    const conclusions=value=>({checks:value.checks??[],scenarios:value.scenarios??[],issues:value.issues??[],
      incompleteReasons:value.incompleteReasons??[],driverStatus:value.driverStatus,
      adaptationComplete:value.adaptationComplete===true});
    return identityDigest(conclusions(previous))===identityDigest(conclusions(assessed));
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

/** Explain unfinished evidence from the selected routes without inventing product defects. */
export function adaptationRemainingWork(rows,cases,{humanReviewBundles=[]}={}){
  const byId=new Map(cases.map(item=>[item.id,item])),work=new Map();
  const humanIds=new Set(humanReviewBundles.flatMap(bundle=>bundle.checks??[]));
  const add=(row,item,kind,detail)=>{
    const caseId=item?.id??null,key=JSON.stringify([row.game,row.variant,caseId,kind]);
    let entry=work.get(key);
    if(!entry){entry={game:row.game,variant:row.variant,caseId,group:item?.group??null,kind,checks:[],details:[],
      ...(item?.report?{report:item.report}:{}),execution:item?.execution??'not-run'};work.set(key,entry);}
    if(!entry.checks.includes(row.id))entry.checks.push(row.id);
    if(detail&&!entry.details.includes(detail))entry.details.push(detail);
  };
  for(const row of rows){
    if(row.complete||row.status==='not-applicable')continue;
    if(!row.caseIds.length)add(row,null,'missing-task','No required route is registered for this requirement');
    for(const caseId of [...row.caseIds,...(row.supportingCaseIds??[])]){
      const item=byId.get(caseId),supporting=!row.caseIds.includes(caseId);
      if(!item){if(!supporting)add(row,{id:caseId},'missing-case','Required route has no result record');continue;}
      const checks=(item.checks??[]).filter(check=>check.id===row.id);
      const failures=checks.filter(check=>check.status==='failed');
      if(failures.length){for(const check of failures)add(row,item,'check-failed',check.error??check.scope??row.id);continue;}
      if(supporting)continue;
      if(item.status==='not-applicable')continue;
      const waitingHuman=humanIds.has(row.id)&&checks.some(check=>check.machineComplete===true&&
        check.completeEvidence?.valid===true&&check.proof?.source==='driver-report'&&check.proof.references?.length>0);
      if(item.status!=='passed'&&!(item.status==='incomplete'&&waitingHuman)){
        const kind=item.status==='blocked'?'prerequisite-missing':item.status==='failed'?'route-failed':'execution-incomplete';
        const details=[...(item.issues??[]),...(item.incompleteReasons??[]),...(item.blockedInputs??[]).map(input=>input.reason),item.reason].filter(Boolean);
        add(row,item,kind,details.join(' | ')||`Route status: ${item.status??'missing'}`);
      }
      if(!checks.length){if(item.status==='passed')add(row,item,'missing-observation','Route did not report this numbered requirement');continue;}
      for(const check of checks){
        const proven=(check.status==='passed'||check.status==='observed'&&check.completeEvidence?.valid===true)&&
          (item.status==='passed'||item.status==='incomplete'&&!(item.issues?.length)&&check.completeEvidence?.valid===true)&&
          check.complete===true&&check.proof?.source==='driver-report'&&check.proof.references?.length>0;
        if(proven)continue;
        if(check.claimedComplete===true||check.complete===true||check.claimedMachineComplete===true&&check.machineComplete!==true){
          add(row,item,'evidence-rejected',check.proofIssue??check.completeEvidence?.reason??'Complete claim lacks an accepted observation or independent proof');
        }else if(check.machineComplete===true&&check.completeEvidence?.valid===true&&
          check.proof?.source==='driver-report'&&check.proof.references?.length>0&&humanIds.has(row.id)){
          add(row,item,'human-review-required',check.scope??'Automatic evidence is complete; the registered human scene bundle supplies the visual conclusion');
        }else if(['incomplete','blocked','not-run','unknown','running'].includes(check.status)){
          add(row,item,'observation-incomplete',check.error??check.scope??`Observation status: ${check.status}`);
        }else{
          add(row,item,'coverage-review-required',check.scope??'Observed scenario does not declare full requirement coverage');
        }
      }
    }
  }
  const items=[...work.values()],requirements=new Set(rows.filter(row=>!row.complete&&row.status!=='not-applicable')
    .map(row=>`${row.game}/${row.variant}/${row.id}`));
  const byKind={};
  for(const kind of new Set(items.map(item=>item.kind))){
    const selected=items.filter(item=>item.kind===kind);
    byKind[kind]={requirements:new Set(selected.flatMap(item=>item.checks.map(id=>`${item.game}/${item.variant}/${id}`))).size,
      workItems:selected.length};
  }
  return{requirements:requirements.size,workItems:items.length,byKind,items};
}

export function adaptationCoverage(cases,{checks=[],unmappedChecks=[],targets,humanReviewBundles=[]}={}){
  const rows=[];
  const products=targets??[...new Set(cases.map(item=>`${item.game}/${item.variant}`))].map(target=>{
    const [game,variant]=target.split('/');return{game,variant};
  });
  for(const {game,variant,requirements} of products){
    const selected=cases.filter(item=>item.game===game&&item.variant===variant);
    const expected=requirements??(checks.length?checks:[...new Set(selected.flatMap(item=>item.checksMapped??[]))]).map(id=>({id}));
    for(const {id,reason} of expected){
      const related=selected.filter(item=>item.checksMapped?.includes(id));
      const supporting=selected.filter(item=>!related.includes(item)&&item.supportingChecks?.includes(id));
      const evidence=[...related,...supporting].flatMap(item=>(item.checks??[]).filter(check=>check.id===id)
        .map(check=>({...check,caseId:item.id,execution:item.execution,role:related.includes(item)?'required':'supporting'})));
      const failed=evidence.some(item=>item.status==='failed');
      const applicable=related.filter(item=>item.status!=='not-applicable');
      const complete=!reason&&!failed&&applicable.length>0&&applicable.every(item=>
        (item.checks??[]).some(check=>check.id===id&&
          (item.status==='passed'||item.status==='incomplete'&&!(item.issues?.length)&&check.completeEvidence?.valid===true)&&
          (check.status==='passed'||check.status==='observed'&&check.completeEvidence?.valid===true)&&check.complete===true&&
          check.proof?.source==='driver-report'&&check.proof.references?.length>0));
      rows.push({game,variant,id,status:reason?'not-applicable':failed?'failed':complete?'complete':related.length?'partial':'unmapped',
        complete,evidence,caseIds:related.map(item=>item.id),supportingCaseIds:supporting.map(item=>item.id),...(reason?{reason}: {})});
    }
  }
  return{complete:rows.length>0&&unmappedChecks.length===0&&rows.every(row=>row.complete||row.status==='not-applicable'),rows,unmappedChecks,
    remaining:adaptationRemainingWork(rows,cases,{humanReviewBundles})};
}

function reviewOperations(guides){
  const operations=new Map();
  for(const source of Object.values(guides??{})){
    let fence=null;
    for(const line of source.replace(/<!--[\s\S]*?-->/g,'').split(/\r?\n/)){
      const marker=line.match(/^\s{0,3}(`{3,}|~{3,})/);
      if(marker){
        if(!fence)fence=marker[1];
        else if(marker[1][0]===fence[0]&&marker[1].length>=fence.length)fence=null;
        continue;
      }
      if(fence||!line.trim().startsWith('|')||!line.trim().endsWith('|'))continue;
      const columns=line.trim().slice(1,-1).split(/(?<!\\)\|/u).map(value=>value.trim().replaceAll('\\|','|'));
      const id=columns[0]?.match(/^<a id="[a-z0-9-]+"><\/a>([A-Z][A-Z0-9]*(?:-[A-Z0-9]+)+)\b/)?.[1];
      if(id&&columns.length===3&&columns[1]&&columns[2])operations.set(id,{operation:columns[1],expected:columns[2]});
    }
  }
  return operations;
}

/** Readable machine results and a small, explicit set of human scene bundles. */
export function adaptationReviewMarkdown(report,{checklist,root,guides}={}){
  const cell=value=>String(value??'').replaceAll('|','\\|').replace(/[\r\n]+/g,' ').replaceAll('<','&lt;');
  const statuses={complete:'驱动完整证据',partial:'待完整证据',unmapped:'未关联自动任务',failed:'失败',
    'not-applicable':'不适用',passed:'场景通过',planned:'预览',blocked:'缺前置条件',
    incomplete:'未完成',timeout:'超时',cancelled:'已取消','not-run':'未运行'};
  const status=value=>statuses[value]??cell(value??'未知');
  const workKinds={'missing-task':'未登记任务','missing-case':'缺场景结果','check-failed':'检查失败',
    'route-failed':'所属场景失败','prerequisite-missing':'缺前置条件','execution-incomplete':'场景未完成',
    'missing-observation':'缺编号观察','evidence-rejected':'完整证据未获接受','human-review-required':'等待集中人审',
    'observation-incomplete':'观察未完成','coverage-review-required':'需核对覆盖范围'};
  const link=(label,path)=>`[${cell(label)}](<${String(path).replaceAll('\\','/')}>)`;
  const definitions=new Map((checklist?.checks??[]).map(check=>[check.id,check]));
  const sections=new Map((checklist?.sections??[]).map(section=>[section.id,section]));
  const humanBundleIds=new Set();
  for(const bundle of checklist?.humanReviewBundles??[]){
    assert.ok(typeof bundle.id==='string'&&bundle.id&&!humanBundleIds.has(bundle.id),'Human review bundles require unique IDs');
    humanBundleIds.add(bundle.id);
    assert.ok(Array.isArray(bundle.checks)&&bundle.checks.length&&new Set(bundle.checks).size===bundle.checks.length,
      `Human review bundle ${bundle.id} requires distinct adaptation IDs`);
    if(definitions.size)assert.ok(bundle.checks.every(id=>definitions.has(id)),`Human review bundle ${bundle.id} refers to an unknown adaptation ID`);
    for(const field of ['label','method','judgement','preparation'])
      assert.ok(typeof bundle[field]==='string'&&bundle[field].trim(),`Human review bundle ${bundle.id} is missing ${field}`);
  }
  const operations=reviewOperations(guides);
  const cases=new Map((report.cases??[]).map(item=>[item.id,item]));
  const lines=['# 适配审查表','',`运行：${cell(report.runId)} | 自动队列：${status(report.status)}`,'',
    '自动结果按编号展示，无需逐项人工签字。人工集中审查下列场景集，一次结论可关联多个编号；发现问题时记录对象、时间和现象。自动检查缺失或失败继续由测试处理。',''];
  const rows=report.coverage?.rows??[];
  const products=[...new Set(rows.map(row=>`${row.game}/${row.variant}`))];
  for(const product of products){
    lines.push(`## ${cell(product)}`,'');
    const productRows=rows.filter(row=>`${row.game}/${row.variant}`===product);
    const workItems=(report.coverage?.remaining?.items??[]).filter(item=>`${item.game}/${item.variant}`===product);
    if(workItems.length){
      const pendingIds=new Set(workItems.flatMap(item=>item.checks));
      lines.push('### 待办清单','',`待收口编号 ${pendingIds.size} 项 | 待办 ${workItems.length} 组`,'',
        '编号按要求计数，待办按场景和原因归组。同一场景可关联多个编号；覆盖范围待核对与检查失败分别列出。','',
        '| 场景 | 原因 | 关联编号 | 依据 |','| --- | --- | --- | --- |');
      for(const item of workItems){
        const label=item.group??item.caseId??'未登记任务';
        const source=item.report?.path?link(label,item.report.path):cell(label);
        lines.push(`| ${source} | ${cell(workKinds[item.kind]??item.kind)} | ${item.checks.map(cell).join('、')} | ${item.details.map(cell).join('<br>')} |`);
      }
      lines.push('');
    }
    const humanBundles=(checklist?.humanReviewBundles??[]).map(bundle=>({...bundle,
      selectedChecks:bundle.checks.filter(id=>productRows.some(row=>row.id===id&&row.status!=='not-applicable'))}))
      .filter(bundle=>bundle.selectedChecks.length);
    if(humanBundles.length){
      lines.push('### 人工审查场景集','',
        '机器先准备素材与自动结果，人再集中查看。每组记录一次结论，具体问题附对象和时间；素材尚未准备时由测试补齐。','',
        '| 场景集与方法 | 关联编号 | 人只需判断 | 准备 | 本组结论 |',
        '| --- | --- | --- | --- | --- |');
      for(const bundle of humanBundles){
        const method=root?link(bundle.label,resolve(root,'docs/adaptation',bundle.method)):cell(bundle.label);
        lines.push(`| ${method} | ${bundle.selectedChecks.map(cell).join('、')} | ${cell(bundle.judgement)} | ${cell(bundle.preparation)} | 待审查 |`);
      }
      lines.push('','审查记录保存本组的实际素材、产物身份、审查者与环境；自动结果与人工结论各按自身范围汇总。','');
    }
    const counts=new Map();
    for(const row of productRows)counts.set(row.status,(counts.get(row.status)??0)+1);
    lines.push('### 自动结果与方法','',
      `自动编号 ${productRows.length} 项 | ${[...counts].map(([value,count])=>`${status(value)} ${count}`).join(' | ')}`,'',
      '<details>','<summary>展开全部自动结果与检验方法</summary>','');
    const groups=new Map([...sections.keys()].map(id=>[id,[]]));
    for(const row of productRows){
      const section=definitions.get(row.id)?.section??'other';
      if(!groups.has(section))groups.set(section,[]);
      groups.get(section).push(row);
    }
    for(const [sectionId,group] of groups){
      if(!group.length)continue;
      lines.push(`#### ${cell(sections.get(sectionId)?.title??'其他检查')}`,'',
        '| 编号与方法 | 操作 | 通过时应看到 | 自动结果与证据 |',
        '| --- | --- | --- | --- |');
      for(const row of group){
        const definition=definitions.get(row.id),section=sections.get(definition?.section);
        const method=section&&root?link(`${row.id} | ${definition.label}`,resolve(root,'docs/adaptation',section.file)+`#${row.id.toLowerCase()}`):cell(row.id);
        const evidence=row.caseIds.map(id=>{
          const item=cases.get(id);if(!item)return cell(id);
          const origin=item.execution==='retained'?'沿用旧证据':item.execution==='executed'?'本次执行':'未执行';
          const label=`${item.group??id} | ${origin} | ${status(item.status)}`;
          return item.report?.path?link(label,item.report.path):cell(label);
        });
        const reason=row.reason?` | ${cell(row.reason)}`:'';
        const steps=operations.get(row.id);
        const operation=row.status==='not-applicable'?'不适用':steps?cell(steps.operation):'见方法链接';
        const expected=row.status==='not-applicable'?'—':steps?cell(steps.expected):'见方法链接';
        lines.push(`| ${method} | ${operation} | ${expected} | ${status(row.status)}${reason.replaceAll('|','\\|')}<br>${evidence.join('<br>')||'无执行证据'} |`);
      }
      lines.push('');
    }
    lines.push('</details>','');
  }
  lines.push('## 执行问题','');
  const problems=(report.cases??[]).filter(item=>!['passed','not-applicable'].includes(item.status));
  if(!problems.length)lines.push('所选场景无已报告执行问题。','');
  for(const item of problems){
    const issues=[...(item.issues??[]),...(item.incompleteReasons??[]),...(item.blockedInputs??[]).map(input=>input.reason),...(item.reason?[item.reason]:[])];
    lines.push(`- ${cell(item.game)}/${cell(item.variant)} | ${cell(item.group??item.id)} | ${status(item.status)}${issues.length?' | '+issues.map(cell).join(' | '):''}`);
  }
  lines.push('');
  return lines.join('\n');
}

export function assertPreviousRun(report){
  assert.equal(report?.schema,'eagler-touhou/adaptation-run/1','Resume requires an adaptation-run report');
  assert.ok(Array.isArray(report.cases),'Resume report has no cases');
  assert.equal(new Set(report.cases.map(item=>item.id)).size,report.cases.length,'Resume report has duplicate case IDs');
}
