import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtempSync,writeFileSync,readFileSync,rmSync,symlinkSync,renameSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join,basename} from 'node:path';
import {assessPresentationVisualArtifacts} from './presentation-visual-artifacts.mjs';
import {analyzeWindow,keyOf,compactReport} from '../../../ports/th08/portable/presentation-lab/analyzer.mjs';
import {TH08_VISUAL_CONTRACT as contract,decodeNativeVisualRecord} from '../../../ports/th08/portable/presentation-lab/visual-contract.mjs';
import {STATE_GROUPS,STATE_FIELD_MAP,STATE_AUTHORITY} from '../../../ports/th08/portable/presentation-lab/owners.mjs';
import {assertPoolReuseBoundary} from '../../../common/testkit/presentation-lab/pool-boundary.mjs';

const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const png='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR4nGP4z8DwHwAFAAH/iZk9HQAAAABJRU5ErkJggg==';
const otherPNG='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR4nGNg+M/wHwAEAQH/cetH5QAAAABJRU5ErkJggg==';
const scope={game:'th08',variant:'normal'},alphas=[0,.25,.5,.75,1,.5,.5,1];
const data={bytes:23,sha256:hash('synthetic DATA')};
const build={game:'th08',instrumented:true,diagnostic:true,wasm:hash('synthetic WASM'),
  sourceDigest:hash('synthetic source'),toolDigest:hash('synthetic tooling'),nativeAbi:contract.nativeAbi};
const birth={owner:5,object:100,generation:1,tick:100,operation:1};
const retired={...birth,tick:139,operation:2},born={...birth,tick:139,generation:2};
const events=[birth,retired,born];
const state=()=>({coverageVersion:'th08/state/3',groups:STATE_GROUPS.map(id=>({id,digest:1,includedFields:STATE_FIELD_MAP[id]})),missingGroups:[],authority:STATE_AUTHORITY});

function nativeRecord(owner,x,generation=0){
  const buffer=new ArrayBuffer(760),meta=new Uint32Array(buffer,0,14),values=new Float32Array(buffer,56,32),vertices=new Float32Array(buffer,184,144);
  meta.set([owner,owner===5?100:owner*100,0,0,0,1,1,1,10,4,4,1<<8,generation,generation?3:0]);
  values[0]=x;values[6]=values[7]=1;values[16]=values[20]=values[30]=values[31]=1;
  values.set([x-2,98,x+2,102,x,100],21);
  let at=0;for(const [dx,dy] of [[-2,-2],[2,-2],[-2,2],[2,2]]){vertices.set([x+dx,100+dy,0,0,0,1,1,1,1],at);at+=9;}
  return Array.from(new Uint32Array(buffer));
}
function lifecycle(count=3,epoch=2){return {available:true,capacity:16384,count,next:count,dropped:0,epoch:1,runtimeEpoch:epoch,
  events:events.slice(0,count),native:{schema:'th08/presentation-lifecycle-buffer/1',stride:20,
    words:events.slice(0,count).flatMap(event=>[event.owner,event.object,event.generation,event.tick,event.operation])}};}
function frame(records,tick,count=3,epoch=2,alpha){return {tick,dropped:0,...(alpha===undefined?{}:{alpha}),
  records:records.map(decodeNativeVisualRecord),native:{schema:'th08/presentation-record-buffer/1',stride:760,count:records.length,
    runtimeEpoch:epoch,words:records.flat(),lifecycle:{count,next:count,dropped:0,epoch:1}}};}
function report({label,tick=400,epoch=2,count=3,previousCount=count,worldFrozen=false,negativeControl=false,
  previous,current,sample,scene=2}){
  const nativeFrames={schema:'th08/presentation-native-frames/1',previous:frame(previous,tick-1,previousCount,epoch),
    current:frame(current,tick,count,epoch),samples:alphas.map(alpha=>frame(sample(alpha),tick,count,epoch,alpha))};
  const readVerification={schema:'eagler-presentation/sweep-reads/1',receipts:[null,...alphas].map((alpha,index)=>{
    const first=state();if(negativeControl&&index)first.groups.find(group=>group.id==='synchronous-audio-operations').digest++;
    return {phase:alpha===null?'before':'after-draw',alpha,reads:[first,structuredClone(first)]};
  })};
  const reads=readVerification.receipts.map(item=>item.reads),nativeLifecycle=lifecycle(count,epoch);
  const value=analyzeWindow({...nativeFrames,stateBefore:reads[0][0],statesAfter:[reads[0][1],...reads.slice(1).flat()],
    worldFrozen,gate:true,build:{...build,runtimeEpoch:epoch,inputs:{data,replay:null}},label,negativeControl,lifecycle:nativeLifecycle});
  return {...value,nativeFrames,nativeLifecycle,readVerification,traceBefore:Array(68).fill(1),traceAfter:Array(68).fill(1),
    scene:{diagnostics:[scene,scene,0,0,...Array(12).fill(0)],status:[scene,1,0,3,...Array(6).fill(0)],
      nativeScene:[scene,1,0,0,0,20,100,...Array(9).fill(0)]}};
}

function fixture(t){
  const directory=mkdtempSync(join(tmpdir(),'native-visual-artifacts-'));t.after(()=>rmSync(directory,{recursive:true,force:true}));
  const title=(label,epoch=1,tick=100)=>report({label,epoch,tick,count:0,scene:1,
    previous:[nativeRecord(1,20)],current:[nativeRecord(1,20)],sample:()=>[nativeRecord(1,20)],negativeControl:label==='negative-control'});
  const reports=['title-idle','title-animate','menu-motion','negative-control','negative-recovered'].map(label=>title(label));
  reports.push(title('display05-title-before',2,110));
  const pool=report({label:'display05-pool-reuse-boundary',tick:140,previousCount:1,
    previous:[nativeRecord(5,10,1)],current:[nativeRecord(5,14,2)],sample:()=>[nativeRecord(5,14,2)]});
  const scan={event:{retired,born},report:pool};scan.nativeVerification=assertPoolReuseBoundary(scan,contract);reports.push(pool);
  for(const label of ['stage1-motion','stage1-negative','stage1-recovered'])reports.push(report({label,negativeControl:label==='stage1-negative',
    previous:[nativeRecord(13,10),nativeRecord(5,20,2)],current:[nativeRecord(13,14),nativeRecord(5,24,2)],
    sample:alpha=>[nativeRecord(13,10+4*alpha),nativeRecord(5,20+4*alpha,2)]}));
  const pause=report({label:'pause-menu',tick:413,worldFrozen:true,
    previous:[nativeRecord(13,14),nativeRecord(3,20),nativeRecord(5,24,2)],
    current:[nativeRecord(13,14),nativeRecord(3,20),nativeRecord(5,24,2)],
    sample:()=>[nativeRecord(13,14),nativeRecord(3,20),nativeRecord(5,24,2)]});
  const scene=(world,control,frozen)=>({nativeScene:[2,1,0,0,0,world,control,0,0,0,0,0,0,frozen?1:0,0,0]});
  pause.pauseProgress={schema:'th08/pause-progress/1',runtimeEpoch:2,waited:1,steps:12,complete:true,
    before:{tick:401,worldFrozen:true,scene:scene(20,100,true),reference:frame([nativeRecord(13,14),nativeRecord(3,20),nativeRecord(5,24,2)],401)},
    after:{tick:413,worldFrozen:true,scene:scene(20,112,true),reference:pause.nativeFrames.current},
    samples:Array.from({length:12},(_,index)=>({step:index+1,tick:402+index,worldFrozen:true,nativeScene:scene(20,101+index,true).nativeScene}))};
  const resumed=report({label:'pause-resumed',tick:422,previous:[nativeRecord(13,18),nativeRecord(5,28,2)],
    current:[nativeRecord(13,22),nativeRecord(5,32,2)],sample:alpha=>[nativeRecord(13,18+4*alpha),nativeRecord(5,28+4*alpha,2)]});
  resumed.pauseRecovery={schema:'th08/pause-recovery/1',runtimeEpoch:2,before:structuredClone(pause.pauseProgress.after),
    movementBefore:{tick:417,worldFrozen:false,scene:scene(21,116,false),reference:frame([nativeRecord(13,14),nativeRecord(5,24,2)],417)},
    after:{tick:422,worldFrozen:false,scene:scene(26,121,false),reference:resumed.nativeFrames.current},
    samples:[{step:1,tick:417,worldFrozen:false,nativeScene:scene(21,116,false).nativeScene}],
    input:[{code:'Escape',down:true,ticks:3},{code:'Escape',down:false},{code:'ArrowRight',down:true,ticks:4},{code:'ArrowRight',down:false}],
    keysReleased:true,playerMoved:true,complete:true};
  resumed.pauseRecoveryValidation=contract.assertPauseRecoveryEvidence(pause,resumed);reports.push(pause,resumed);
  const byLabel=new Map(reports.map(row=>[row.label,row])),refs={},values={};
  const save=(key,name,value)=>{const path=join(directory,name),bytes=Buffer.from(JSON.stringify(value));writeFileSync(path,bytes);
    refs[key]={path,bytes:bytes.length,sha256:hash(bytes)};values[key]=value;return refs[key];};
  const relative=identity=>({...identity,path:basename(identity.path)});
  save('capture','browser-captures.json',{reports,errors:[],browser:'synthetic component fixture'});
  const errors=save('browserErrors','browser-errors.json',{schema:'eagler-presentation/browser-errors/1',status:'passed',errors:[],
    channels:{console:[],http:[],pageerror:[],requestfailed:[]}});
  save('scan','display05-pool-reuse-scan.json',scan);
  const checks=['DISPLAY-04','DISPLAY-05','DISPLAY-06'].map(id=>({id,status:'passed',complete:false,machineComplete:true,
    evidence:{...scope,adaptationIds:[id]}}));
  const observations={schema:'eagler-presentation/display-observations/1',scope,checks,lifecycleBefore:lifecycle(1),lifecycleAfter:lifecycle(),
    poolReuseCapture:scan,humanReview:{status:'pending'},claims:{humanVisualApproval:false,physicalRefreshVerified:false}};
  save('observations','presentation-observations.json',observations);
  const materials=[];
  for(const [key,name,label] of [
    ['titleMaterial','material-display05-title-before.json','display05-title-before'],
    ['poolMaterial','material-display05-pool-reuse-boundary.json','display05-pool-reuse-boundary'],
    ['continuousMaterial','material-continuous-motion.json','stage1-motion'],
    ['pauseMaterial','material-pause-menu.json','pause-menu'],['resumedMaterial','material-pause-resumed.json','pause-resumed'],
  ]){
    const source=byLabel.get(label),material=structuredClone(source);material.label=`${label}/visual-window`;
    material.images=material.nativeFrames.samples.slice(0,5).map(sample=>({alpha:sample.alpha,png,
      boxes:sample.records.map(record=>({key:keyOf(record),bounds:record.values.slice(21,25)}))}));
    const checked=contract.assertVisualMaterialEvidence(material,{sourceReport:source});
    const identity=save(key,name,compactReport(material));
    materials.push({...relative(identity),label:material.label,tick:material.tick,runtimeEpoch:2,nativeValidation:checked,
      images:material.images.map(image=>{const bytes=Buffer.from(image.png.split(',')[1],'base64');
        return {alpha:image.alpha,bytes:bytes.length,sha256:hash(bytes),boxCount:image.boxes.length};})});
  }
  save('materials','visual-materials.json',{schema:'eagler-presentation/visual-materials/1',scope,status:'recorded',issues:[],
    browserCaptures:relative(refs.capture),browserErrors:relative(errors),materials,
    humanReview:{status:'pending',accepted:null},claims:{humanVisualApproval:false,physicalRefreshVerified:false}});
  const acceptance={schema:'eagler-presentation/acceptance/1',scope,humanReview:{status:'pending'},checks:checks.map(row=>({
    ...row,evidence:{...scope,adaptationIds:[row.id],sources:[row.evidence]}}))};
  const acceptanceRef=save('acceptance','presentation-acceptance.json',acceptance);
  const args=id=>({id,refs,acceptance,observation:{scope,check:acceptance.checks.find(row=>row.id===id),
    humanReview:{status:'pending'},artifacts:{acceptance:acceptanceRef}},entry:{id,status:'passed',complete:false,machineComplete:true},
    capability:{boundary:{reportSchema:'th08/presentation-audit/1',titleScene:1,gameScene:2,sceneSlot:0,loadingSlot:3,statusSceneSlot:0,errorSlot:2}},
    runs:[10,11,12].map(epoch=>({epoch,identity:{build,data}}))});
  return {directory,refs,values,args,save,byLabel,relative};
}

test('all three visual IDs re-evaluate native words, five materials and 25 sealed PNGs while human remains pending',t=>{
  const f=fixture(t);
  for(const id of ['DISPLAY-04','DISPLAY-05','DISPLAY-06']){
    const result=assessPresentationVisualArtifacts(f.args(id));
    assert.equal(result.status,'passed');assert.equal(result.machineComplete,true);assert.equal(result.complete,false);
    assert.deepEqual(result.evidence.adaptationIds,[id]);assert.equal(result.evidence.materials.length,5);
    assert.equal(result.evidence.recovery.pausedSteps,12);assert.equal(result.evidence.pool.newSubmissions,1);
  }
});

test('missing raw, changed raw fields, wrong lifecycle and a changed second read cannot borrow summary pass',t=>{
  const f=fixture(t),original=readFileSync(f.refs.capture.path),originalRef={...f.refs.capture};
  for(const change of [
    row=>{delete row.nativeFrames;},row=>{delete row.nativeLifecycle.native;},
    row=>{row.nativeFrames.samples[0].records[0].values[0]++;},
    row=>{row.nativeFrames.current.native.lifecycle.epoch++;},
    row=>{row.readVerification.receipts[1].reads[1].groups[0].digest++;},
    row=>{row.objects[0].fields[0].status='passed';},
  ]){
    const capture=JSON.parse(original);change(capture.reports.find(row=>row.label==='stage1-motion'));
    f.save('capture',basename(originalRef.path),capture);
    const index=structuredClone(f.values.materials);index.browserCaptures=f.relative(f.refs.capture);
    f.save('materials','visual-materials.json',index);
    assert.throws(()=>assessPresentationVisualArtifacts(f.args('DISPLAY-04')));
  }
});

test('a replacement PNG, forged boxes or changed producer index is rejected even with a freshly hashed material',t=>{
  const f=fixture(t),original=readFileSync(f.refs.continuousMaterial.path);
  const material=JSON.parse(original);material.images[0].png=otherPNG;
  f.save('continuousMaterial','material-continuous-motion.json',material);
  assert.throws(()=>assessPresentationVisualArtifacts(f.args('DISPLAY-04')),/sealed file identity/);
  material.images[0].png=png;material.images[0].boxes[0].bounds[0]++;
  f.save('continuousMaterial','material-continuous-motion.json',material);
  const index=structuredClone(f.values.materials),item=index.materials.find(row=>row.path==='material-continuous-motion.json');
  Object.assign(item,f.relative(f.refs.continuousMaterial));f.save('materials','visual-materials.json',index);
  assert.throws(()=>assessPresentationVisualArtifacts(f.args('DISPLAY-04')),/boxes/);
  const bytes=readFileSync(f.refs.materials.path);writeFileSync(f.refs.materials.path,Buffer.concat([bytes,Buffer.from(' ')]));
  assert.throws(()=>assessPresentationVisualArtifacts(f.args('DISPLAY-05')),/producer identity/);
});

test('foreign paths, symlink artifacts, a failed browser channel and a manufactured human approval fail closed',t=>{
  const f=fixture(t),args=f.args('DISPLAY-06');
  const path=f.refs.pauseMaterial.path,link=join(f.directory,'foreign.json');symlinkSync(path,link);
  args.refs={...f.refs,pauseMaterial:{...f.refs.pauseMaterial,path:link}};
  assert.throws(()=>assessPresentationVisualArtifacts(args),/exact evidence path/);
  rmSync(link);renameSync(path,link);symlinkSync(link,path);
  assert.throws(()=>assessPresentationVisualArtifacts(f.args('DISPLAY-06')),/regular file/);
  rmSync(path);renameSync(link,path);
  const index=structuredClone(f.values.materials);index.humanReview.accepted=true;
  f.save('materials','visual-materials.json',index);
  assert.throws(()=>assessPresentationVisualArtifacts(f.args('DISPLAY-06')),/manufactures an approval/);
  index.humanReview.accepted=null;
  const errors=structuredClone(f.values.browserErrors);errors.channels.requestfailed.push('actual fixture request failure');
  f.save('browserErrors','browser-errors.json',errors);index.browserErrors=f.relative(f.refs.browserErrors);
  f.save('materials','visual-materials.json',index);
  assert.throws(()=>assessPresentationVisualArtifacts(f.args('DISPLAY-06')),/Final browser errors/);
});

test('another product or duplicated acceptance claim cannot use TH08 visual artifacts',t=>{
  const f=fixture(t),args=f.args('DISPLAY-05');args.observation.scope={game:'th10',variant:'normal'};
  assert.throws(()=>assessPresentationVisualArtifacts(args),/registered native visual contract/);
  const duplicate=f.args('DISPLAY-05');duplicate.acceptance.checks.push(duplicate.acceptance.checks.find(row=>row.id==='DISPLAY-05'));
  assert.throws(()=>assessPresentationVisualArtifacts(duplicate),/duplicate DISPLAY-05/);
});
