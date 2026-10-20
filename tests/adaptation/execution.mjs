import assert from 'node:assert/strict';
import {randomUUID} from 'node:crypto';
import {mkdir,readFile,readdir} from 'node:fs/promises';
import {isAbsolute,relative,resolve} from 'node:path';
import {runTasks} from '../../tools/testing/runner.mjs';
import {runWithReport} from '../../tools/testing/cli-report.mjs';
import {acquireOutputLock} from '../../tools/testing/output-lock.mjs';
import {fileIdentity} from '../../tools/testing/dependency-snapshot.mjs';
import {buildAdaptationPlan} from './catalog.mjs';
import {createCaseIdentityReader} from './identity.mjs';
import {adaptationEnvironment,INPUT_ENVIRONMENT} from './inputs.mjs';
import {adaptationCoverage,assertPreviousRun,readCaseReport,reusableResult,summarizeAdaptationCases} from './results.mjs';

export const freshAdaptationOutput=root=>resolve(root,'artifacts/validation',`adaptation-${new Date().toISOString().replace(/[:.]/g,'-')}-${randomUUID().slice(0,8)}`);
const inputNames=Object.values(INPUT_ENVIRONMENT);
const list=value=>Array.isArray(value)?value:value?value.split(','):[];

async function loadPrevious(path){
  if(!path)return null;
  let absolute=resolve(path);if(!absolute.endsWith('.json'))absolute=resolve(absolute,'summary.json');
  const report=JSON.parse(await readFile(absolute,'utf8'));assertPreviousRun(report);
  return{path:absolute,identity:await fileIdentity(absolute),report};
}

/** Plans and executes case boundaries through the repository's existing process queue. */
export async function runAdaptationVerification(options,dependencies={}){
  const root=resolve(options.root),output=resolve(options.output??freshAdaptationOutput(root));
  assert.ok(!(options.resume&&options.rerunFailed),'Select either resume or rerun-failed');
  const previous=await loadPrevious(options.resume??options.rerunFailed);
  const selection={games:options.games??previous?.report.selection.games,
    variant:options.variant??previous?.report.selection.variant??'both',
    checks:list(options.checks??previous?.report.selection.checks),groups:list(options.groups??previous?.report.selection.groups)};
  let environment={...options.environment??process.env};
  for(const name of inputNames)if(!environment[name]&&previous?.report.inputs?.[name])environment[name]=previous.report.inputs[name];
  environment=await adaptationEnvironment(options.inputs,environment);
  const jobs=options.jobs??1;assert.ok(Number.isInteger(jobs)&&jobs>=1&&jobs<=32,'jobs must be an integer from 1 to 32');
  await mkdir(output,{recursive:true});
  const release=await acquireOutputLock(output);
  try{
    assert.deepEqual((await readdir(output)).filter(name=>name!=='.running.json'),[],`Use a new evidence directory: ${output}`);
    return await runWithReport({output,filename:'summary.json',schema:'eagler-touhou/adaptation-run/1'},async(report,save)=>{
      Object.assign(report,{output,selection,inputs:Object.fromEntries(inputNames.filter(name=>environment[name]).map(name=>[name,environment[name]])),
        mode:options.planOnly?'plan':options.rerunFailed?'rerun-failed':options.resume?'resume':'run',
        ...(previous?{parent:{path:previous.path,...previous.identity,runId:previous.report.runId}}:{}),cases:[]});
      const plan=(dependencies.buildPlan??buildAdaptationPlan)({root,output,environment,...selection});
      report.unmappedChecks=plan.unmappedChecks??[];
      for(const item of plan.cases){
        assert.ok(/^[a-zA-Z0-9][a-zA-Z0-9._-]*$/.test(item.id),'Invalid adaptation case ID');
        const within=relative(output,item.reportPath);
        assert.ok(within&&!isAbsolute(within)&&within!=='..'&&!within.startsWith('..\\')&&!within.startsWith('../'),'Case evidence must stay inside this run');
        report.cases.push({...item,checksMapped:item.checks,checks:[],status:item.status==='ready'?'not-run':item.status,execution:'not-run'});
      }
      assert.equal(new Set(report.cases.map(item=>item.id)).size,report.cases.length,'Duplicate adaptation case IDs');
      if(options.planOnly){
        report.status='planned';report.passed=false;await save();return;
      }
      assert.ok(report.cases.length||report.unmappedChecks.length,'No adaptation scenarios selected');
      const identityReader=(dependencies.createIdentityReader??createCaseIdentityReader)({root,environment});
      const pending=[];
      for(const item of report.cases){
        if(item.status!=='not-run')continue;
        try{Object.assign(item,await identityReader({...item,checks:item.checksMapped}));}
        catch(error){item.status='blocked';item.issues=[`Preflight: ${error.message}`];item.finishedAt=new Date().toISOString();continue;}
        const prior=previous?.report.cases.find(old=>old.id===item.id);
        const rerun=options.rerunFailed&&['failed','timeout','cancelled','not-run'].includes(prior?.status);
        if(!rerun&&await reusableResult(prior,item.fingerprint)){
          const {status,issues,checks,scenarios,driverRunId,driverStatus,adaptationComplete,process:processResult,finishedAt,report:childReport}=prior;
          Object.assign(item,{status,issues,checks,scenarios,driverRunId,driverStatus,adaptationComplete,process:processResult,finishedAt,
            report:childReport,execution:'retained',source:{runId:previous.report.runId,path:previous.path,finishedAt}});
        }else pending.push({...item.task,id:item.id});
        await save();
      }
      const controller=new AbortController(),cancel=()=>controller.abort();
      process.once('SIGINT',cancel);process.once('SIGTERM',cancel);
      try{
        // Build the shared Launcher once. Per-title packaging reuses its content cache.
        const needsLauncher=pending.some(task=>report.cases.find(item=>item.id===task.id).requiresLauncherBuild);
        if(needsLauncher&&!dependencies.skipPreparation){
          const prepared=await runTasks([{id:'launcher',command:process.execPath,args:['scripts/build-launcher.mjs'],
            cwd:resolve(root,'launcher'),timeoutMs:300000,resources:['launcher-build']}],
          {output:resolve(output,'prepare'),concurrency:1,environment,signal:controller.signal});
          report.preparation={status:prepared.status,path:resolve(output,'prepare/report.json')};
          if(prepared.status!=='passed')for(let index=pending.length-1;index>=0;index--){
            const item=report.cases.find(item=>item.id===pending[index].id);
            if(!item.requiresLauncherBuild)continue;
            pending.splice(index,1);item.status='blocked';item.issues=['Shared Launcher preparation failed'];
            item.finishedAt=new Date().toISOString();
          }
        }
        if(pending.length)await runTasks(pending,{output:resolve(output,'processes'),concurrency:jobs,environment,
          signal:controller.signal,failFast:false,metadata:{parentRunId:report.runId},
          assessResult:async result=>{
            const item=report.cases.find(item=>item.id===result.id);
            Object.assign(item,await readCaseReport(item,result),{process:{...result},execution:'executed',finishedAt:new Date().toISOString()});
            // Preserve the process result alongside the driver's semantic outcome.
            result.processStatus=result.status;result.status=item.status;
          },onResult:async result=>{await save();options.onResult?.(report.cases.find(item=>item.id===result.id));}});
      }finally{process.removeListener('SIGINT',cancel);process.removeListener('SIGTERM',cancel);}
      const postflight=(dependencies.createIdentityReader??createCaseIdentityReader)({root,environment});
      for(const item of report.cases.filter(item=>['executed','retained'].includes(item.execution))){
        try{
          const current=await postflight({...item,checks:item.checksMapped});
          item.identityStable=current.fingerprint===item.fingerprint;
          assert.ok(item.identityStable,'Selected driver, Runtime, input or browser changed during verification');
        }catch(error){item.identityStable=false;item.status='failed';(item.issues??=[]).push(`Postflight: ${error.message}`);}
      }
      Object.assign(report,summarizeAdaptationCases(report.cases));
      if(report.preparation&&report.preparation.status!=='passed'){report.status='failed';report.passed=false;}
      report.coverage=adaptationCoverage(report.cases,{checks:selection.checks,unmappedChecks:report.unmappedChecks});
      if((report.unmappedChecks.length||selection.checks.length&&!report.coverage.complete)&&report.status==='passed'){
        report.status='incomplete';report.passed=false;
      }
      await save();
    });
  }finally{await release();}
}
