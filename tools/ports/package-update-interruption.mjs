import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');

/** The old generation remains complete after an actual in-flight HTTP body abort. */
export function assertInterruptedPackageDownload(e){
  const p=e.probe,game=p.game;assert.equal(p.schema,'eagler-touhou/interrupted-package-download/1');
  assert.equal(p.unexpectedCommit,undefined);assert.equal(p.error?.name,'AbortError');assert.ok(p.error?.message);
  const before=p.before,after=p.after,stage=p.staged;assert.ok(stage?.pending&&stage.current);
  const old=before.installation.currentGeneration;assert.equal(before.generation.id,old);
  assert.deepEqual(after.generation,before.generation,'Interrupted download changed the committed package bytes or references');
  assert.equal(after.installation.currentGeneration,old);assert.equal(after.installation.pendingGeneration,null);
  assert.equal(stage.current.installation.currentGeneration,old);assert.equal(stage.current.generation.id,old);
  assert.equal(stage.current.installation.pendingGeneration,stage.pending.id);assert.notEqual(stage.pending.id,old);
  assert.deepEqual(stage.pending.descriptor,p.descriptor);assert.equal(stage.pending.game,game);
  assert.equal(stage.pending.files[stage.fileId].objectId,stage.objectId);
  const declaration=p.descriptor.files[stage.fileId];assert.equal(stage.bytes,declaration.bytes);assert.equal(stage.sha256,declaration.sha256);
  assert.ok(!Object.values(before.generation.files).some(ref=>ref.objectId===stage.objectId),'The staged object was already committed in the base generation');
  assert.ok(p.progress.some(row=>row.fileId===stage.fileId&&row.found===true&&row.reused!==true));
  const aborted=p.abortedRequest;assert.ok(p.requests.includes(aborted)||p.requests.some(row=>JSON.stringify(row)===JSON.stringify(aborted)));
  assert.equal(aborted.status,200);assert.equal(aborted.signalBefore,false);assert.equal(aborted.signalAfter,true);
  assert.ok(aborted.contentLength>0);assert.equal(aborted.bodyError.name,'AbortError');assert.equal(aborted.bodyCompleted,undefined);
  assert.ok(p.requests.length>=2);assert.notEqual(p.requests[0].url,aborted.url);
  const transfer=e.transport,remote=new URL(aborted.url),descriptorUrl=new URL(p.descriptorUrl);
  const abortedFile=Object.entries(p.descriptor.files).find(([,file])=>new URL(file.source,descriptorUrl).href===remote.href);
  assert.ok(abortedFile,'Aborted HTTP response is not a declared package file');assert.notEqual(abortedFile[0],stage.fileId);
  assert.equal(transfer.url,aborted.url);assert.equal(transfer.bytes,abortedFile[1].bytes);assert.equal(transfer.sha256,abortedFile[1].sha256);
  assert.equal(aborted.contentLength,transfer.bytes);assert.ok(transfer.holdAfterBytes>0&&transfer.holdAfterBytes<transfer.bytes);
  assert.ok(transfer.holdMs>0&&transfer.holdMs<=30_000);
  assert.equal(aborted.prefix.bytes,transfer.holdAfterBytes);assert.equal(aborted.prefix.sha256,transfer.prefixSha256,
    'The browser must actually receive the real package prefix before aborting');
  assert.equal(p.pendingAfter,null,'Pending generation was not cancelled');assert.equal(p.stagedObjectAfter,null,'Unreferenced staged object was not collected');
  assert.deepEqual(e.pageErrors,[]);assert.deepEqual(e.httpErrors,[]);
  assert.equal(e.requestFailures.length,1);assert.equal(e.requestFailures[0].url,aborted.url);assert.match(e.requestFailures[0].error,/ERR_ABORTED|ABORT/i);
  assert.ok(e.consoleErrors.every(row=>row.url===aborted.url&&/ERR_ABORTED|ABORT/i.test(row.text)));
}

/** A separate real Launcher page shares the installation, not the Runtime document. */
export async function runInterruptedPackageDownload({context,base,game,site,fixture,expectedGenerationId,baseDescriptor,responseOverrides}){
  const absoluteDescriptor=resolve(site,fixture.descriptor.path),descriptorPath=pathToFileURL(absoluteDescriptor),
    descriptor=JSON.parse(readFileSync(descriptorPath)),descriptorUrl=new URL(fixture.descriptor.path,base).href;
  const newFiles=Object.entries(descriptor.files).filter(([,file])=>!Object.values(baseDescriptor.files).some(old=>old.sha256===file.sha256&&old.bytes===file.bytes));
  assert.ok(newFiles.length>=2,'Interrupted update needs two new actual package files');
  const [,second]=newFiles[1],url=new URL(second.source,descriptorUrl),body=readFileSync(new URL(second.source,descriptorPath));
  assert.equal(body.length,second.bytes);assert.equal(hash(body),second.sha256);
  const control={kind:'controlled-resource-response',status:200,body,bytes:body.length,sha256:hash(body),contentType:'application/octet-stream',
    holdAfterBytes:Math.min(4096,body.length-1),holdMs:30_000};
  const evidence={descriptor:{...fixture.descriptor,path:absoluteDescriptor},
    transport:{url:url.href,bytes:control.bytes,sha256:control.sha256,holdAfterBytes:control.holdAfterBytes,holdMs:control.holdMs,
      prefixSha256:hash(body.subarray(0,control.holdAfterBytes))},
    probe:null,pageErrors:[],httpErrors:[],consoleErrors:[],requestFailures:[]};
  let page;
  try{
    responseOverrides.set(url.pathname+url.search,control);page=await context.newPage();
    page.on('pageerror',error=>evidence.pageErrors.push(String(error.stack||error)));
    page.on('console',message=>{if(message.type()==='error')evidence.consoleErrors.push({text:message.text(),url:message.location().url});});
    page.on('response',response=>{if(response.status()>=400)evidence.httpErrors.push({url:response.url(),status:response.status()});});
    page.on('requestfailed',request=>evidence.requestFailures.push({url:request.url(),error:request.failure()?.errorText??null}));
    await page.goto(new URL(`?game=${game}&acceptanceUpdateProbe=1`,base).href,{waitUntil:'load',timeout:60_000});
    await page.waitForFunction(()=>window.__eaglerBoot?.done===true,null,{timeout:60_000});
    await page.addScriptTag({url:new URL('acceptance-package-update-probe.js',base).href});
    evidence.probe=await page.evaluate(input=>window.EaglerPackageUpdateProbe.run(input),{descriptor,descriptorUrl,expectedGenerationId,interruptAtBytes:control.holdAfterBytes});
    await page.waitForTimeout(100);
  }catch(error){evidence.error=String(error.stack||error);}
  finally{responseOverrides.delete(url.pathname+url.search);if(page)try{await page.close();}catch(error){evidence.pageErrors.push(String(error.stack||error));}}
  return evidence;
}

export function assessPackageUpdateInterruptionEvidence({driver,entry,product}={}){
  if(driver?.schema!=='eagler-touhou/offline-runtime-browser/1'||entry?.id!=='PACKAGE-03')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'staged-http-interruption-and-package-retry',checks:['PACKAGE-03']};
  try{
    assert.equal(entry.complete,true);assert.equal(entry.status,'passed');assert.equal(driver.scope.game,product.game);assert.equal(product.variant,'normal');
    assert.ok(driver.checks.includes(entry)&&driver.checks.filter(row=>row.id===entry.id).length===1);
    assert.deepEqual(entry.evidenceRefs,['/interruptedRemoteUpdate']);
    const envelope=driver.interruptedRemoteUpdate;assert.equal(envelope.game,product.game);assert.equal(envelope.variant,product.variant);assert.ok(envelope.adaptationIds.includes(entry.id));
    const e=envelope.observation;assertInterruptedPackageDownload(e);
    const receipt=e.descriptor,bytes=readFileSync(receipt.path);assert.equal(bytes.length,receipt.bytes);assert.equal(hash(bytes),receipt.sha256);
    assert.deepEqual(JSON.parse(bytes),e.probe.descriptor);
    const old=e.probe.before.generation,opened=e.oldReopen;assert.equal(opened.query.gameGeneration,old.id);
    assert.equal(opened.ready.game,product.game);assert.equal(opened.first.event,'first-frame');
    assert.equal(opened.first.epoch,Number(opened.query.runtimeEpoch));assert.equal(opened.dataRead.sha256,driver.runtimePackages.data.sha256);
    assert.equal(opened.dataRead.bytes,driver.runtimePackages.data.bytes);
    const retry=driver.packageUpdate.retry;assert.equal(retry.status,'committed');assert.equal(retry.oldGeneration,old.id);
    assert.notEqual(retry.atomicCurrentGeneration,old.id);assert.equal(retry.generation.revision,e.probe.descriptor.revision);
    assert.equal(retry.generation.pendingGeneration,null);assert.deepEqual(retry.generation.data,driver.runtimePackages.data);
    assert.equal(driver.onlineRuntimeAfterUpdate.query.gameGeneration,retry.atomicCurrentGeneration);
    assert.equal(driver.onlineRuntimeAfterUpdate.first.event,'first-frame');result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
