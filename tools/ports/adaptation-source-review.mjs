import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {lstatSync,readFileSync} from 'node:fs';
import {dirname,resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';

const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const sha=/^[a-f0-9]{64}$/;
const text=value=>typeof value==='string'&&value.trim().length>0;
const object=value=>value&&typeof value==='object'&&!Array.isArray(value);
const sourceReviewChecks=new Set(['BUILD-06','VERIFY-05']);

function explicitBlockers(product,checks){
  if(product?.status!=='blocked')return null;
  const blockers=product.blockers;
  assert.ok(Array.isArray(blockers)&&blockers.length>0&&blockers.every(blocker=>
    object(blocker)&&sourceReviewChecks.has(blocker.check)&&text(blocker.reason)),
  'Explicit blocked source review needs check-specific human-readable blockers');
  const reasonsFor=id=>blockers.filter(blocker=>blocker.check===id).map(blocker=>blocker.reason);
  assert.ok(checks.length&&checks.every(id=>sourceReviewChecks.has(id)&&reasonsFor(id).length),
    'Every requested check needs an explicit source review blocker');
  return Object.fromEntries(checks.map(id=>[id,reasonsFor(id)]));
}

/** Inspect an explicit reviewed record. Candidate runs never create a review or accept a golden. */
export function inspectSourceReviewNow({manifest,game,variant,checks=['BUILD-06','VERIFY-05']}={}){
  assert.ok(text(game)&&['normal','multiplayer'].includes(variant),'Select a product and variant');
  assert.ok(checks.length&&checks.every(id=>['BUILD-06','VERIFY-05'].includes(id)),'Unknown source review check');
  const path=resolve(manifest),bytes=readFileSync(path),input=JSON.parse(bytes);
  assert.equal(input.schema,'eagler-touhou/adaptation-source-review/1','Unsupported source review manifest');
  const product=input.products?.[game]?.[variant];
  assert.ok(object(product),`Review manifest has no ${game}/${variant}`);
  const blocked=explicitBlockers(product,checks);
  if(blocked){
    const reasons=checks.flatMap(id=>blocked[id].map(reason=>`${id}: ${reason}`));
    throw new Error(`Source review blocked: ${reasons.join(' | ')}`);
  }
  const files=new Map();
  const inspect=(record,label)=>{
    assert.ok(object(record)&&text(record.path)&&sha.test(record.sha256??''),`${label} requires a path and SHA-256`);
    const absolute=resolve(dirname(path),record.path),info=lstatSync(absolute);
    assert.ok(info.isFile()&&!info.isSymbolicLink(),`${label} must be a regular file`);
    const data=readFileSync(absolute),identity={path:absolute,bytes:data.length,sha256:digest(data)};
    assert.equal(identity.sha256,record.sha256,`${label} has changed since review`);
    if(record.bytes!==undefined)assert.equal(identity.bytes,record.bytes,`${label} byte count differs`);
    files.set(absolute,identity);return identity;
  };
  const reviewed=(record,label)=>{
    assert.ok(object(record)&&record.decision==='accepted'&&text(record.reviewer)&&text(record.reviewedAt),
      `${label} needs an explicit accepted review, reviewer and date`);
    assert.ok(Array.isArray(record.evidence)&&record.evidence.length,`${label} has no reviewed evidence files`);
    const evidence=[];for(const [index,item] of record.evidence.entries())evidence.push(inspect(item,`${label} evidence ${index}`));
    return{decision:record.decision,reviewer:record.reviewer,reviewedAt:record.reviewedAt,evidence};
  };
  const results=[];
  if(checks.includes('BUILD-06')){
    const value=product.source;
    assert.ok(object(value)&&text(value.upstream)&&/^[a-f0-9]{40,64}$/.test(value.revision??''),
      'Source record requires an upstream and immutable revision');
    assert.ok(text(value.author)&&text(value.license),'Source record requires attribution and license declaration');
    const notice=inspect(value.notice,'Source license/attribution notice');
    assert.ok(Array.isArray(value.externalInputs)&&value.externalInputs.length,'Declare external game and font inputs');
    const roles=new Set(),externalInputs=[];
    for(const item of value.externalInputs){
      assert.ok(text(item.role)&&text(item.origin)&&text(item.distribution),'External input origin and distribution are required');
      assert.ok(!roles.has(item.role),`Duplicate external role ${item.role}`);roles.add(item.role);
      externalInputs.push({...inspect(item,`External ${item.role}`),role:item.role,origin:item.origin,distribution:item.distribution});
    }
    assert.ok(roles.has('game-data')&&roles.has('font'),'Both game-data and font provenance are required');
    const review=reviewed(value.review,'Source provenance');
    results.push({id:'BUILD-06',status:'passed',complete:true,scope:`${game}/${variant} | reviewed source and external-input identities`,
      evidence:{upstream:value.upstream,revision:value.revision,author:value.author,license:value.license,notice,externalInputs,review}});
  }
  if(checks.includes('VERIFY-05')){
    const value=product.reference;
    assert.ok(object(value),'Declare the original reference maintenance record');
    assert.equal(value.candidateMayUpdateGolden,false,'Candidate checks must not accept or overwrite expected data');
    assert.equal(value.maintenanceWritesAcceptedReference,false,'Maintenance must write staging separately from accepted references');
    assert.ok(text(value.provider)&&text(value.captureMethod)&&text(value.acceptanceMethod),
      'Reference record requires independent provider, capture and separate acceptance methods');
    const producer=inspect(value.producer,'Original capture producer');
    const originalExecutable=inspect(value.originalExecutable,'Original executable');
    const originalData=inspect(value.originalData,'Original DATA');
    const acceptedManifest=inspect(value.acceptedManifest,'Accepted golden manifest');
    const golden=JSON.parse(readFileSync(acceptedManifest.path,'utf8'));
    assert.equal(golden.game,game,'Golden belongs to another game');
    assert.equal(golden.policy?.candidateMayUpdateGolden,false,'Golden permits candidate replacement');
    assert.equal(golden.identity?.executableSha256,originalExecutable.sha256,'Golden has another original executable');
    assert.equal(golden.identity?.resourceSha256,originalData.sha256,'Golden has another original DATA');
    assert.ok(object(golden.assets)&&Object.keys(golden.assets).length,'Golden set has no accepted traces');
    const assets=[];
    for(const [id,asset] of Object.entries(golden.assets)){
      assert.ok(text(asset.path)&&sha.test(asset.sha256??'')&&sha.test(asset.replaySha256??''),`Golden ${id} has no trace/Replay identity`);
      assert.ok(Number.isSafeInteger(asset.comparedTicks)&&asset.comparedTicks>0,`Golden ${id} has no comparison extent`);
      assets.push({id,...inspect({...asset,path:resolve(dirname(acceptedManifest.path),asset.path)},`Golden ${id}`)});
    }
    const review=reviewed(value.review,'Original reference acceptance');
    results.push({id:'VERIFY-05',status:'passed',complete:true,scope:`${game}/${variant} | accepted original-reference maintenance provenance`,
      evidence:{provider:value.provider,captureMethod:value.captureMethod,acceptanceMethod:value.acceptanceMethod,
        producer,originalExecutable,originalData,acceptedManifest,assets,review,recapturedThisRun:false}});
  }
  for(const before of files.values())assert.equal(digest(readFileSync(before.path)),before.sha256,'Reviewed input changed during inspection');
  assert.equal(digest(readFileSync(path)),digest(bytes),'Review manifest changed during inspection');
  const inputs={manifest:{path,bytes:bytes.length,sha256:digest(bytes)},files:[...files.values()]};
  for(const result of results){
    result.evidence={game,variant,adaptationIds:[result.id],observation:{...result.evidence,inputIdentity:inputs}};
  }
  return{checks:results,inputs};
}

/** Preserve the async public interface used by planning and input inspection. */
export async function inspectSourceReview(options){
  return inspectSourceReviewNow(options);
}

if(process.argv[1]&&resolve(process.argv[1])===fileURLToPath(import.meta.url)){
  assertCliArguments(process.argv.slice(2),{values:['manifest','game','variant','checks','output'],flags:[]});
  const arg=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
  const output=arg('output');assert.ok(output,'Required: --output');
  const report=await runWithReport({output:resolve(output),schema:'eagler-touhou/adaptation-source-review-result/1'},async(report,save)=>{
    report.scope={game:arg('game'),variant:arg('variant')};
    if(!arg('manifest')){report.status='blocked';report.passed=false;report.reason='Provide the reviewed source/maintenance input manifest';return;}
    const manifestPath=resolve(arg('manifest')),manifestBytes=readFileSync(manifestPath);
    const manifestInput=JSON.parse(manifestBytes);
    const selectedChecks=arg('checks')?.split(',')??['BUILD-06','VERIFY-05'];
    const selectedProduct=manifestInput.products?.[report.scope.game]?.[report.scope.variant];
    assert.equal(manifestInput.schema,'eagler-touhou/adaptation-source-review/1','Unsupported source review manifest');
    const blocked=explicitBlockers(selectedProduct,selectedChecks);
    if(blocked){
      report.blocked=true;
      report.blockers=selectedChecks.flatMap(id=>blocked[id].map(reason=>({check:id,reason})));
      report.checks=selectedChecks.map(id=>({id,status:'blocked',complete:false,
        scope:`${report.scope.game}/${report.scope.variant} | source review pending`,reason:blocked[id].join(' | ')}));
      report.inputs={manifest:{path:manifestPath,bytes:manifestBytes.length,sha256:digest(manifestBytes)},files:[]};
      report.status='blocked';report.passed=false;report.reason='Source and original-reference review inputs are pending';
      return;
    }
    const result=await inspectSourceReview({manifest:arg('manifest'),...report.scope,checks:arg('checks')?.split(',')});
    Object.assign(report,result,{status:'passed',passed:true});await save();
  });
  if(report.status!=='passed')process.exitCode=1;
}
