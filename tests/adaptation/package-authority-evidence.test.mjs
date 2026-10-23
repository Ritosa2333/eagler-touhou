import assert from 'node:assert/strict';
import test from 'node:test';
import {assessPackageInstallationAuthority,INSTALLATION_AUTHORITY_JOURNAL_SCHEMA} from '../../tools/ports/offline-installation-authority.mjs';
import {assessCompleteEvidence} from './complete-evidence.mjs';
import {assessDriverReport} from './results.mjs';

const game='th08',variant='normal',product={game,variant};
const DATA={bytes:123,sha256:'a'.repeat(64)};
const clone=value=>structuredClone(value);
const envelope=(observation,adaptationIds=['PACKAGE-06'])=>({game,variant,adaptationIds,observation});

function storeRead(sequence,phase,store,key,result){
  return{sequence,phase,store,key,mode:'readonly',result,error:null};
}
function generation(game,generationId,revision){
  return{id:generationId,game,revision,gameVersion:'original',dataFile:'game-data',dataObjectId:`${generationId}-object`};
}
function installation(game,currentGeneration){
  return{game,source:game==='th08'?'local':null,currentGeneration,pendingGeneration:null};
}
function authorityJournal(){
  const reads=[];let sequence=0;
  const phase=(name,generationId,revision)=>{
    reads.push(storeRead(++sequence,name,'installations',[game,'original'],installation(game,generationId)));
    reads.push(storeRead(++sequence,name,'generations',[game,generationId],generation(game,generationId,revision)));
  };
  phase('post-import-card-reopen','base-g1','rev-1');
  reads.push(storeRead(++sequence,'post-import-card-reopen','installations',['th10','original'],installation('th10',null)));
  phase('resource-preparation-and-update-prompt','base-g1','rev-1');
  phase('update-ready-checkpoint','updated-g2','rev-2');
  phase('published-update-commit','updated-g2','rev-2');
  phase('offline-reopen','updated-g2','rev-2');
  phase('offline-resource-preparation','updated-g2','rev-2');
  return{schema:INSTALLATION_AUTHORITY_JOURNAL_SCHEMA,overflow:false,pending:0,reads};
}
function runtime(generationId,pageOnline,epoch=2){
  const query={hosted:'1',managedData:'1',runtimeVariant:'normal',gameGeneration:generationId,runtimeEpoch:String(epoch)};
  const dataRead={game,epoch,generation:generationId,fileId:'game-data',...DATA};
  return{frameUrl:`https://127.0.0.1/runtime/${game}/game.html?${new URLSearchParams(query)}`,
    query,ready:{protocol:'eagler-touhou/1',game,epoch,event:'ready'},
    first:{protocol:'eagler-touhou/1',game,epoch,event:'first-frame'},
    pageOnline,workerControlled:true,dataRead,dataReads:[clone(dataRead)]};
}
function packageAuthorityFixture(){
  const prompt={observed:true,confirmed:true,title:'更新可用',
    message:'新版已下载并校验完成。立即重启会先保存并结束当前游戏，再使用新版启动。也可以继续游戏，下次启动时使用新版。',
    confirm:'立即重启',cancel:'下次启动使用新版',publishedRevision:'rev-2',
    currentGenerationAtPrompt:'updated-g2',currentRevisionAtPrompt:'rev-2',sourceAtPrompt:'local',
    pinnedRuntimeGenerationAtPrompt:'base-g1',pinnedRuntimeEpoch:1,pendingGenerationAtPrompt:null};
  const journal=authorityJournal();
  const authorityInput={schema:'eagler-touhou/package-installation-authority-input/1',journal,game,
    nonSelectedGame:'th10',baseGenerationId:'base-g1',baseRevision:'rev-1',
    updatedGenerationId:'updated-g2',updatedRevision:'rev-2',finalRuntimeGenerationId:'updated-g2',
    offlineRuntimeGenerationId:'updated-g2',updatePrompt:prompt};
  const productionReads=assessPackageInstallationAuthority(authorityInput);
  assert.equal(productionReads.valid,true,productionReads.reason);
  const card={source:'Launcher game card after a real page reopen',launchLabel:'启动游戏',importLabel:'导入游戏包'};
  const fixture={game,revision:'rev-2',descriptor:{path:'packages/th08-rev-2.package.json',bytes:300,sha256:'b'.repeat(64)},
    catalog:{path:'release-catalog.json',bytes:200,sha256:'c'.repeat(64)},sourceArchive:{bytes:500,sha256:'d'.repeat(64)},
    files:[{source:'games/th08/th08.data',bytes:DATA.bytes,sha256:DATA.sha256}]};
  const online=runtime('updated-g2',true),offline=runtime('updated-g2',false);
  const observation={status:'passed',coverageComplete:true,fixture,gameCard:card,initialRuntime:runtime('base-g1',true,1),
    update:{status:'committed',prompt,base:{generationId:'base-g1',revision:'rev-1'},
      current:{generationId:'updated-g2',revision:'rev-2',pendingGeneration:null},sourceArchive:fixture.sourceArchive},
    authorityInput,productionReads,
    uiResults:{gameCard:card,updatePrompt:prompt,
      onlineRuntime:{generationId:'updated-g2',dataRead:online.dataRead},
      offlineRuntime:{generationId:'updated-g2',dataRead:offline.dataRead}}};
  const refs={
    '/packageAuthority':envelope(observation),
    '/uiImport':envelope({path:'C:/private/th08.zip',source:'Launcher #gamePackageImport UI',store:'IndexedDB Package Store',
      generation:{generationId:'base-g1',revision:'rev-1',gameVersion:'original',fileId:'game-data',target:'/th08.data',
        ...DATA,declaredBytes:DATA.bytes,declaredSha256:DATA.sha256}}),
    '/onlineRuntime':envelope(online),'/offlineRuntime':envelope(offline),
  };
  const entry={id:'PACKAGE-06',status:'passed',complete:true,scope:'selected normal game',evidenceRefs:Object.keys(refs)};
  const driver={schema:'eagler-touhou/offline-runtime-browser/1',runId:'offline-run',
    startedAt:'2026-10-21T00:00:00.000Z',finishedAt:'2026-10-21T00:00:01.000Z',
    status:'passed',passed:true,scope:{game,variant},checks:[entry],
    packageAuthority:refs['/packageAuthority'],uiImport:refs['/uiImport'],
    onlineRuntime:refs['/onlineRuntime'],offlineRuntime:refs['/offlineRuntime']};
  return{driver,entry,refs,observation,authorityInput};
}

test('PACKAGE-06 recomputes a local publish-to-install authority chain and normalizes complete',()=>{
  const fixture=packageAuthorityFixture();
  assert.deepEqual(assessCompleteEvidence({driver:fixture.driver,entry:fixture.entry,product}),
    {available:true,valid:true,reason:null,kind:'package-installation-authority'});
  const result=assessDriverReport({game,variant,group:'offline',checksMapped:['PACKAGE-06'],scenarioIds:[]},
    {code:0,status:'passed',startedAt:'2026-10-20T23:59:59.000Z'},fixture.driver);
  assert.equal(result.status,'passed');
  assert.equal(result.checks.length,1);
  assert.equal(result.checks[0].complete,true);
  assert.equal(result.checks[0].completeEvidence.valid,true);
});

test('PACKAGE-06 rejects a complete label without the retained prompt, generation chain, native DATA or current journal',()=>{
  const placeholder=packageAuthorityFixture();
  placeholder.entry.evidenceRefs=[];
  assert.equal(assessCompleteEvidence({driver:placeholder.driver,entry:placeholder.entry,product}).valid,false,
    'complete:true cannot replace actual structured evidence');

  const missingPrompt=packageAuthorityFixture();
  delete missingPrompt.observation.authorityInput.updatePrompt;
  assert.equal(assessCompleteEvidence({driver:missingPrompt.driver,entry:missingPrompt.entry,product}).valid,false);

  const wrongGeneration=packageAuthorityFixture();
  wrongGeneration.refs['/onlineRuntime'].observation.query.gameGeneration='base-g1';
  wrongGeneration.refs['/onlineRuntime'].observation.ready.epoch=2;
  assert.equal(assessCompleteEvidence({driver:wrongGeneration.driver,entry:wrongGeneration.entry,product}).valid,false);

  const noRestart=packageAuthorityFixture();
  noRestart.observation.authorityInput.updatePrompt.pinnedRuntimeEpoch=2;
  assert.equal(assessCompleteEvidence({driver:noRestart.driver,entry:noRestart.entry,product}).valid,false);

  const wrongEventEpoch=packageAuthorityFixture();
  wrongEventEpoch.driver.onlineRuntime.observation.query.runtimeEpoch='3';
  assert.equal(assessCompleteEvidence({driver:wrongEventEpoch.driver,entry:wrongEventEpoch.entry,product}).valid,false);
  const wrongUrl=packageAuthorityFixture();
  wrongUrl.driver.onlineRuntime.observation.frameUrl=wrongUrl.driver.onlineRuntime.observation.frameUrl.replace('runtimeEpoch=2','runtimeEpoch=3');
  assert.equal(assessCompleteEvidence({driver:wrongUrl.driver,entry:wrongUrl.entry,product}).valid,false);

  const missingNativeData=packageAuthorityFixture();
  missingNativeData.refs['/offlineRuntime'].observation.dataRead=null;
  assert.equal(assessCompleteEvidence({driver:missingNativeData.driver,entry:missingNativeData.entry,product}).valid,false);

  const oldJournal=packageAuthorityFixture();
  oldJournal.observation.authorityInput.journal.reads=oldJournal.observation.authorityInput.journal.reads
    .map(read=>({...read,phase:'legacy-package-store-observation'}));
  oldJournal.observation.productionReads.valid=true;
  assert.equal(assessCompleteEvidence({driver:oldJournal.driver,entry:oldJournal.entry,product}).valid,false);
});

test('PACKAGE-06 rejects mismatched producer summary, prompt identity and evidence product',()=>{
  const badSummary=packageAuthorityFixture();
  badSummary.observation.productionReads.reads=[];
  assert.equal(assessCompleteEvidence({driver:badSummary.driver,entry:badSummary.entry,product}).valid,false);

  const badPrompt=packageAuthorityFixture();
  badPrompt.observation.authorityInput.updatePrompt.confirmed=false;
  assert.equal(assessCompleteEvidence({driver:badPrompt.driver,entry:badPrompt.entry,product}).valid,false);

  const wrongProduct=packageAuthorityFixture();
  wrongProduct.refs['/packageAuthority'].game='th10';
  assert.equal(assessCompleteEvidence({driver:wrongProduct.driver,entry:wrongProduct.entry,product}).valid,false);
});
