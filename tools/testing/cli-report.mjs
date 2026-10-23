import {randomUUID} from 'node:crypto';
import {mkdir,writeFile,rename} from 'node:fs/promises';
import {resolve} from 'node:path';

/** Invalidate the previous result before parsing inputs, and finalize every initialized run. */
export async function runWithReport({output,schema,filename='report.json'},operation){
  if(!/^[A-Za-z0-9][A-Za-z0-9._-]*\.json$/.test(filename))throw new Error('Report filename must be a JSON basename');
  const report={schema,runId:randomUUID(),startedAt:new Date().toISOString(),status:'running',passed:false,targets:[]};
  await mkdir(output,{recursive:true});
  const pending=resolve(output,`${filename}.${report.runId}.pending.json`),destination=resolve(output,filename);
  const save=async()=>{await writeFile(pending,JSON.stringify(report,null,2)+'\n');await rename(pending,destination);};
  await save();
  try{
    await operation(report,save);
    if(report.status==='running')report.status=report.passed===true?'passed':'failed';
    if(report.status==='passed'&&report.passed!==true)throw new Error('Report status passed requires passed=true');
    report.passed=report.status==='passed'&&report.passed===true;
  }catch(error){report.status='failed';report.passed=false;report.failure=String(error.stack||error);}
  finally{report.finishedAt=new Date().toISOString();await save();}
  return report;
}
