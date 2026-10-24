import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {existsSync,lstatSync,readFileSync,realpathSync,statSync} from 'node:fs';
import {basename,dirname,isAbsolute,resolve} from 'node:path';
import {isDeepStrictEqual} from 'node:util';
import {assertNativeVisualEvidence} from './presentation-native-visual-evidence.mjs';
import {assertContinuousVisualEvidence} from '../../../common/testkit/presentation-lab/continuous-fields.mjs';
import {assertPoolReuseBoundary} from '../../../common/testkit/presentation-lab/pool-boundary.mjs';

// Optional title sources may be absent in another title's sparse checkout.
// Existing but broken modules still fail normally; only absence is optional.
const th08Url=new URL('../../../ports/th08/portable/presentation-lab/visual-contract.mjs',import.meta.url);
const th08=existsSync(th08Url)?(await import(th08Url.href)).TH08_VISUAL_CONTRACT:null;
const IDS=new Set(['DISPLAY-04','DISPLAY-05','DISPLAY-06']);
const SHA=/^[a-f0-9]{64}$/u;
const requireValue=(value,message)=>assert.ok(value,message);
const same=(left,right)=>isDeepStrictEqual(left,right);
const record=value=>value!==null&&typeof value==='object'&&!Array.isArray(value);
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const NAMES=Object.freeze({observations:'presentation-observations.json',scan:'display05-pool-reuse-scan.json',
  titleMaterial:'material-display05-title-before.json',poolMaterial:'material-display05-pool-reuse-boundary.json',
  continuousMaterial:'material-continuous-motion.json',pauseMaterial:'material-pause-menu.json',
  resumedMaterial:'material-pause-resumed.json',materials:'visual-materials.json',capture:'browser-captures.json'});
const MATERIALS=Object.freeze([
  ['titleMaterial','display05-title-before'],['poolMaterial','display05-pool-reuse-boundary'],
  ['continuousMaterial','stage1-motion'],['pauseMaterial','pause-menu'],['resumedMaterial','pause-resumed'],
]);

function readFile(identity,name,directory){
  requireValue(record(identity)&&isAbsolute(identity.path)&&basename(identity.path)===name&&
    dirname(resolve(identity.path))===directory&&Number.isSafeInteger(identity.bytes)&&identity.bytes>0&&SHA.test(identity.sha256??''),
  `Visual artifact ${name} is not bound to its exact evidence path, size and SHA-256`);
  const path=resolve(identity.path),before=lstatSync(path);
  requireValue(before.isFile()&&!before.isSymbolicLink()&&realpathSync(path)===path,
    `Visual artifact ${name} is not a regular file at its declared path`);
  const bytes=readFileSync(path),after=statSync(path);
  requireValue(bytes.length===identity.bytes&&hash(bytes)===identity.sha256,`Visual artifact ${name} differs from its producer identity`);
  requireValue(after.size===before.size&&after.mtimeMs===before.mtimeMs&&after.ctimeMs===before.ctimeMs,
    `Visual artifact ${name} changed while being read`);
  return {value:JSON.parse(bytes.toString('utf8')),identity:{path,bytes:bytes.length,sha256:identity.sha256}};
}

function exactRow(rows,id,label){
  requireValue(Array.isArray(rows),`${label} has no check rows`);
  const found=rows.filter(row=>row?.id===id);
  requireValue(found.length===1,`${label} has a missing or duplicate ${id} row`);
  return found[0];
}

function matchIndexIdentity(saved,actual,label){
  requireValue(record(saved)&&saved.path===basename(actual.path)&&saved.bytes===actual.bytes&&saved.sha256===actual.sha256,
    `${label} differs from the producer's sealed file identity`);
}

function validateBuild(row,runs,label){
  const expected=runs[0]?.identity;
  requireValue(expected?.build?.game==='th08'&&expected.build.diagnostic===true&&expected.build.instrumented===true&&
    SHA.test(expected.build.wasm??'')&&SHA.test(expected.build.sourceDigest??'')&&SHA.test(expected.build.toolDigest??''),
  'Visual evidence has no independently registered diagnostic build');
  requireValue(row?.build?.game==='th08'&&row.build.instrumented===true&&row.build.nativeAbi===th08.nativeAbi&&
    row.build.wasm===expected.build.wasm&&row.build.sourceDigest===expected.build.sourceDigest&&
    row.build.toolDigest===expected.build.toolDigest&&row.build.inputs?.data?.bytes===expected.data?.bytes&&
    row.build.inputs.data.sha256===expected.data?.sha256&&row.build.inputs.replay===null,
  `${label} belongs to another diagnostic Runtime, DATA or Replay session`);
}

/** Reassess visual machine evidence; this never grants a human or device verdict. */
export function assessPresentationVisualArtifacts({id,refs,acceptance,observation,entry,capability,runs}={}){
  requireValue(IDS.has(id)&&entry?.id===id&&observation?.check?.id===id,`Unsupported or borrowed visual check ${id}`);
  requireValue(th08&&same(observation.scope,{game:'th08',variant:'normal'}),
    'No registered native visual contract for this selected product');
  requireValue(capability?.boundary?.reportSchema==='th08/presentation-audit/1'&&
    Array.isArray(runs)&&runs.length===3,'Visual evidence lacks its registered raw capture contract');
  const acceptancePath=observation.artifacts?.acceptance?.path;
  requireValue(typeof acceptancePath==='string'&&isAbsolute(acceptancePath)&&basename(acceptancePath)==='presentation-acceptance.json',
    'Visual evidence has no producer-bound acceptance path');
  const directory=dirname(resolve(acceptancePath));
  const read=(key)=>readFile(refs?.[key],NAMES[key],directory);
  const capture=read('capture'),index=read('materials'),observations=read('observations'),scan=read('scan');
  requireValue(acceptance?.schema==='eagler-presentation/acceptance/1'&&same(acceptance.scope,observation.scope)&&
    acceptance.humanReview?.status==='pending'&&observation.humanReview?.status==='pending',
  'Visual machine evidence cannot replace the pending human review');
  const accepted=exactRow(acceptance.checks,id,'Acceptance'),display=exactRow(observations.value.checks,id,'Display observation');
  for(const [label,row] of [['entry',entry],['acceptance',accepted],['display',display]])
    requireValue(row.status==='passed'&&row.complete===false&&row.machineComplete===true,`${id} ${label} does not retain a separate machine-only conclusion`);
  requireValue(same(accepted,observation.check)&&accepted.evidence?.game==='th08'&&accepted.evidence.variant==='normal'&&
    same(accepted.evidence.adaptationIds,[id])&&display.evidence?.game==='th08'&&display.evidence.variant==='normal'&&
    same(display.evidence.adaptationIds,[id])&&
    same(accepted.evidence.sources,[{schema:display.schema??null,...display.evidence}]),
  `${id} acceptance differs from its product-specific display source`);
  const raw=observations.value,collection=capture.value,inventory=index.value;
  requireValue(raw.schema==='eagler-presentation/display-observations/1'&&same(raw.scope,observation.scope)&&
    raw.humanReview?.status==='pending'&&raw.claims?.humanVisualApproval===false&&raw.claims.physicalRefreshVerified===false,
  'Display observation scope or human/device claims are invalid');
  requireValue(Array.isArray(collection.reports)&&Array.isArray(collection.errors)&&collection.errors.length===0&&
    collection.captureInterrupted!==true,'Visual browser capture is incomplete or contains browser errors');
  requireValue(inventory.schema==='eagler-presentation/visual-materials/1'&&same(inventory.scope,observation.scope)&&
    inventory.status==='recorded'&&Array.isArray(inventory.issues)&&inventory.issues.length===0&&
    inventory.humanReview?.status==='pending'&&inventory.humanReview.accepted===null&&
    inventory.claims?.humanVisualApproval===false&&inventory.claims.physicalRefreshVerified===false,
  'Visual material index is incomplete or manufactures an approval');
  matchIndexIdentity(inventory.browserCaptures,capture.identity,'Visual capture index');
  const errorsRef=inventory.browserErrors;
  requireValue(errorsRef?.path==='browser-errors.json','Visual index omits the final browser error channels');
  const errors=readFile({...errorsRef,path:resolve(directory,errorsRef.path)},'browser-errors.json',directory);
  requireValue(errors.value.schema==='eagler-presentation/browser-errors/1'&&errors.value.status==='passed'&&
    Array.isArray(errors.value.errors)&&errors.value.errors.length===0&&
    same(Object.keys(errors.value.channels??{}).sort(),['console','http','pageerror','requestfailed'])&&
    Object.values(errors.value.channels).every(values=>Array.isArray(values)&&values.length===0),
  'Final browser errors were present or not fully recorded');
  const byLabel=new Map();
  for(const row of collection.reports){
    requireValue(typeof row?.label==='string'&&!byLabel.has(row.label),'Browser capture has duplicate or missing visual labels');
    byLabel.set(row.label,row);validateBuild(row,runs,row.label);
    const native=assertNativeVisualEvidence(row,th08,row.label);
    requireValue(native.recomputed.valid&&Array.isArray(row.traceBefore)&&row.traceBefore.length===68&&
      row.traceBefore.every(word=>Number.isSafeInteger(word)&&word>=0&&word<=0xffffffff)&&same(row.traceBefore,row.traceAfter),
    `${row.label}: visual window or Runtime trace changed or is missing`);
    if(row.negativeControl){
      requireValue(['negative-control','stage1-negative'].includes(row.label)&&native.recomputed.purityStatus==='fail'&&
        native.recomputed.stateChanges.some(change=>change.group==='synchronous-audio-operations'),
      `${row.label}: actual draw-side mutation was not rejected`);
    }else requireValue(native.recomputed.purityStatus==='pass',`${row.label}: draw or observation changed authority state`);
  }
  for(const label of ['title-idle','title-animate','menu-motion','negative-control','negative-recovered',
    'stage1-negative','stage1-recovered',...MATERIALS.map(([,label])=>label)])
    requireValue(byLabel.has(label),`Visual browser capture omits ${label}`);
  const before=th08.readNativeLifecycle(raw.lifecycleBefore),after=th08.readNativeLifecycle(raw.lifecycleAfter);
  requireValue(before.epoch===after.epoch&&before.runtimeEpoch===after.runtimeEpoch&&before.count<=after.count&&
    same(after.native.words.slice(0,before.count*5),before.native.words),'Visual scene lifecycle prefix changed or was dropped');
  const ordinaryEpoch=byLabel.get('stage1-motion').build.runtimeEpoch;
  requireValue(after.runtimeEpoch===ordinaryEpoch&&runs.every(run=>run.epoch!==ordinaryEpoch),
    'Ordinary visual windows must be bound to their own Runtime epoch');
  requireValue(same(raw.poolReuseCapture,scan.value)&&same(byLabel.get('display05-pool-reuse-boundary'),scan.value.report),
    'Pool raw capture differs from the observation or browser envelope');
  const materialEvidence=[];
  requireValue(Array.isArray(inventory.materials)&&inventory.materials.length===5&&
    new Set(inventory.materials.map(item=>item.path)).size===5,'Visual index must retain all five unique materials');
  for(const [key,label] of MATERIALS){
    const source=byLabel.get(label);requireValue(source.build.runtimeEpoch===ordinaryEpoch,`${label}: visual windows cross Runtime sessions`);
    // Pause/recovery can add events after lifecycleAfter, which seals the pool scan.
    const retained=label==='pause-menu'||label==='pause-resumed'?null:after;
    assertNativeVisualEvidence(source,th08,label,retained);
    const material=read(key),saved=inventory.materials.find(item=>item.path===NAMES[key]);
    matchIndexIdentity(saved,material.identity,`${label} material index`);
    const checked=th08.assertVisualMaterialEvidence(material.value,{sourceReport:source,retainedLifecycle:retained});
    requireValue(same(saved.nativeValidation,checked)&&saved.runtimeEpoch===checked.runtimeEpoch&&saved.tick===checked.tick&&
      saved.label===checked.label&&Array.isArray(saved.images)&&saved.images.length===5,
    `${label}: material identity or raw validation differs`);
    for(let imageIndex=0;imageIndex<5;imageIndex++){
      const image=material.value.images[imageIndex],savedImage=saved.images[imageIndex];
      const text=image.png.slice('data:image/png;base64,'.length),png=Buffer.from(text,'base64');
      requireValue(png.subarray(0,8).equals(Buffer.from([137,80,78,71,13,10,26,10]))&&
        png.toString('base64')===text&&savedImage?.alpha===image.alpha&&savedImage.bytes===png.length&&
        savedImage.sha256===hash(png)&&savedImage.boxCount===image.boxes.length,
      `${label}: PNG ${imageIndex} bytes differ from the sealed producer material`);
    }
    materialEvidence.push({key,...material.identity,runtimeEpoch:checked.runtimeEpoch,tick:checked.tick,images:5});
  }
  const motion=byLabel.get('stage1-motion'),pause=byLabel.get('pause-menu'),resumed=byLabel.get('pause-resumed');
  const continuous={motion:assertContinuousVisualEvidence(motion,th08),pause:assertContinuousVisualEvidence(pause,th08),
    resumed:assertContinuousVisualEvidence(resumed,th08)};
  const positionOwners=new Set(motion.objects.filter(object=>object.fields.some(field=>field.id==='position'&&field.status==='interpolated')).map(object=>object.ownerId));
  requireValue(positionOwners.has(13)&&[5,6,7,8,9,11,14,16,17,19,20,21].some(owner=>positionOwners.has(owner)),
    'DISPLAY-04 lacks actual player and other world-object motion');
  const title=byLabel.get('display05-title-before'),scene=capability.boundary;
  for(const [row,expected] of [[title,scene.titleScene],[motion,scene.gameScene]])
    requireValue(row.scene?.diagnostics?.[scene.sceneSlot]===expected&&row.scene.diagnostics[scene.loadingSlot]===0&&
      row.scene.status?.[scene.statusSceneSlot]===expected&&row.scene.status[scene.errorSlot]===0&&
      Array.isArray(row.scene.nativeScene)&&row.scene.nativeScene.length===16&&row.scene.nativeScene[0]===expected,
    'DISPLAY-05 lacks the actual native title-to-game scene boundary');
  const pool=assertPoolReuseBoundary(scan.value,th08),recovery=th08.assertPauseRecoveryEvidence(pause,resumed);
  requireValue(same(scan.value.nativeVerification,pool),'Saved pool verification differs from the raw boundary');
  requireValue(same(resumed.pauseRecoveryValidation,recovery),'Saved pause recovery differs from native clocks and raw player submissions');
  return {status:'passed',complete:false,machineComplete:true,evidence:{game:'th08',variant:'normal',adaptationIds:[id],
    kind:'native-visual-artifacts',runtimeEpoch:ordinaryEpoch,capture:capture.identity,observations:observations.identity,
    scan:scan.identity,materialsIndex:index.identity,browserErrors:errors.identity,materials:materialEvidence,
    continuous,pool,recovery,humanReview:'pending',physicalRefreshVerified:false}};
}
