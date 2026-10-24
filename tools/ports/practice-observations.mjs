import assert from 'node:assert/strict';
import {resolvePracticeAdapter} from '../../tests/adaptation/adapters/registry.mjs';
import {practiceGameplayReady,runPracticeGameplayStage} from '../../tests/adaptation/adapters/practice.mjs';
import {assessLauncherPracticeJourney} from './launcher-practice-journey.mjs';

export {practiceGameplayReady,runPracticeGameplayStage};

export function practiceBuildFeature(manifest,product){
  assert.equal(typeof manifest.features?.thprac,'boolean','Current practice acceptance requires an explicit boolean package feature');
  const enabled=manifest.features.thprac;
  if(enabled)assert.equal(product.features.thprac,true,'Package practice feature exceeds product support');
  return enabled;
}

const practiceLocaleMatrix=[
  {epoch:1,enabled:true,locale:'ja-JP'},
  {epoch:2,enabled:true,locale:'en-US'},
  {epoch:3,enabled:true,locale:'zh-CN'},
  {epoch:4,enabled:false,locale:'en-US'},
];

function boundPracticeCheck({id,game,variant,status,scope,observation}){
  const complete=observation?.complete===true;
  return{id,adaptationIds:[id],status,complete:status==='observed'&&complete,scope,
    observation:{game,variant,adaptationIds:[id],observation}};
}

const basePracticeIds=Object.freeze(['PRACTICE-01','PRACTICE-02','PRACTICE-03','PRACTICE-04','PRACTICE-06','PRACTICE-07','PRACTICE-08']);
/** IDs backed by a real adapter path; product support alone does not declare roundtrip coverage. */
export function practiceAdaptationIds(game,variant='normal'){
  const replay=resolvePracticeAdapter(game,variant)?.replayRoundTrip;
  return Object.freeze(replay
    ?[...basePracticeIds.slice(0,4),'PRACTICE-05',...basePracticeIds.slice(4)]
    :[...basePracticeIds]);
}

function allLocalesRecorded(locales){
  return Array.isArray(locales)&&locales.length===practiceLocaleMatrix.length&&practiceLocaleMatrix.every(expected=>{
    const item=locales.find(candidate=>candidate?.epoch===expected.epoch);
    return item?.status==='passed'&&item.enabled===expected.enabled&&item.locale===expected.locale;
  });
}

function configuredOptionsMatch(item,expected){
  return item?.opened?.options?.thpracEnabled===expected.enabled&&
    item?.opened?.options?.thpracLocale===expected.locale;
}

function menuTransitionObserved(item){
  if(!item||item.status!=='passed'||item.before?.open!==false||item.opened?.open!==true||item.closed?.open!==false)return false;
  const opened=item.opened.events??[],closed=item.closed.events??[];
  return opened.some(event=>event?.open===true)&&closed.slice(opened.length).some(event=>event?.open===false);
}

/** Numbered observations from the practice browser's actual package and menu operations. */
export function practiceAdaptationChecks({game,variant='normal',productSupported,feature,locales=[],finalErrorAudit,practiceReplayRoundTrip,launcherPractice}={}){
  const practiceAdapter=resolvePracticeAdapter(game,variant);
  const auditFailed=finalErrorAudit?.status==='failed';
  const statusWithAudit=status=>auditFailed?'failed':status;
  const unsupported=productSupported===false&&feature===false;
  const launcher=launcherPractice?{...assessLauncherPracticeJourney(launcherPractice),report:launcherPractice}:null;
  const launcherFailed=launcher?.status==='failed';
  const withLauncher=status=>status==='failed'||launcherFailed?'failed':launcher&&!launcher.passed?'incomplete':status;
  // Package OFF is a valid configuration. This row observes its declaration;
  // it does not establish that the Host and visible UI consumed that choice.
  const featureStatus=typeof feature!=='boolean'||typeof productSupported!=='boolean'?'incomplete':
    unsupported?'not-applicable':!productSupported&&feature?'failed':'observed';
  const rows=[boundPracticeCheck({id:'PRACTICE-01',game,variant,status:statusWithAudit(withLauncher(featureStatus)),
    scope:'formal Runtime package practice feature declaration',observation:{feature,productSupported,...(launcher?{launcher}:{} )}})];
  if(unsupported){
    for(const id of practiceAdaptationIds(game,variant).filter(id=>id!=='PRACTICE-01'))rows.push(boundPracticeCheck({id,game,variant,
      status:'not-applicable',scope:'product declares no practice feature',observation:{feature,productSupported}}));
    return rows;
  }

  const exactMatrix=allLocalesRecorded(locales);
  const expectedRows=practiceLocaleMatrix.map(expected=>({expected,item:locales.find(candidate=>candidate?.epoch===expected.epoch)}));
  const anyFailed=locales.some(item=>item?.status==='failed');
  const optionsValid=exactMatrix&&expectedRows.every(({expected,item})=>configuredOptionsMatch(item,expected));
  rows.push(boundPracticeCheck({id:'PRACTICE-02',game,variant,
    status:statusWithAudit(withLauncher(anyFailed?'failed':optionsValid?'observed':'incomplete')),
    scope:'native options consumed for enabled locales and disabled control',
    observation:{...(launcher?{launcher}:{}),locales:expectedRows.map(({expected,item})=>({epoch:expected.epoch,enabled:expected.enabled,locale:expected.locale,
      observedEnabled:item?.opened?.options?.thpracEnabled??null,observedLocale:item?.opened?.options?.thpracLocale??null,
      status:item?.status??'missing'}))}}));

  const enabledRows=expectedRows.filter(({expected})=>expected.enabled);
  const parameterEpoch=locales.find(item=>item?.epoch===1)??null;
  const lifecycleRecord=parameterEpoch?.practiceLifecycle;
  let lifecycle=null;
  if(practiceAdapter?.lifecycle){
    try{lifecycle=practiceAdapter.lifecycle.assess(lifecycleRecord?.evidence??{});}
    catch(error){lifecycle={status:'failed',complete:false,error:String(error.message)};}
  }
  const lifecycleValid=!practiceAdapter?.lifecycle||(lifecycle?.status==='observed'&&lifecycle.complete===true);
  const menuValid=exactMatrix&&enabledRows.every(({item})=>menuTransitionObserved(item))&&
    expectedRows.filter(({expected})=>!expected.enabled).every(({item})=>item?.status==='passed'&&
      item.before?.open===false&&item.opened?.open===false&&item.closed?.open===false);
  rows.push(boundPracticeCheck({id:'PRACTICE-03',game,variant,
    status:statusWithAudit(anyFailed||lifecycle?.status==='failed'?'failed':menuValid&&lifecycleValid?'observed':'incomplete'),
    scope:practiceAdapter?.lifecycle?'native practice menu and same Runtime retry, Replay exit, ordinary start and fresh Practice':'actual gameplay practice menu open and close state',
    observation:{...(practiceAdapter?.lifecycle?{lifecycle,lifecycleEvidence:lifecycleRecord?.evidence??null}:{}),locales:expectedRows.map(({expected,item})=>({epoch:expected.epoch,enabled:expected.enabled,
      beforeOpen:item?.before?.open??null,opened:item?.opened?.open??null,closed:item?.closed?.open??null,
      openEvent:(item?.opened?.events??[]).some(event=>event?.open===true),
      closeEvent:(item?.closed?.events??[]).slice(item?.opened?.events?.length??0).some(event=>event?.open===false),
      status:item?.status??'missing'}))}}));

  const reportedConsumption=parameterEpoch?.parameterConsumption;
  let consumption=reportedConsumption;
  if(typeof practiceAdapter?.assessConsumption==='function'){
    try{consumption=practiceAdapter.assessConsumption(parameterEpoch?.practiceSnapshot,practiceAdapter.parameterCase,practiceAdapter.selectionCase,reportedConsumption??{});}
    catch(error){consumption={status:'failed',complete:false,error:String(error.message)};}
    if(reportedConsumption?.status==='failed'&&consumption.status!=='failed')consumption={...consumption,status:'failed',reportedFailure:reportedConsumption};
  }
  rows.push(boundPracticeCheck({id:'PRACTICE-04',game,variant,
    status:statusWithAudit(consumption?.status==='failed'?'failed':consumption?.status==='observed'?'observed':'incomplete'),
    scope:'native configured block, accepted run block and applied GameplaySession values',
    observation:{epoch:parameterEpoch?.epoch??null,consumption:consumption??null,
      source:parameterEpoch?.practiceSnapshot?.source??null,units:parameterEpoch?.practiceSnapshot?.units??null}}));

  if(practiceAdaptationIds(game,variant).includes('PRACTICE-05')){
    const supported=practiceAdapter?.replayRoundTrip?.supported===true;
    const status=!supported?'incomplete':practiceReplayRoundTrip?.status==='failed'?'failed':
      practiceReplayRoundTrip?.status==='observed'&&practiceReplayRoundTrip.complete===true?'observed':'incomplete';
    rows.push(boundPracticeCheck({id:'PRACTICE-05',game,variant,status:statusWithAudit(status),
      scope:'recorded practice metadata saved, persisted, imported before launch, restored by the native Replay menu and consumed during playback',
      observation:{complete:practiceReplayRoundTrip?.complete===true,
        blocked:!supported,reason:supported?null:practiceAdapter?.replayRoundTrip?.reason??'Title adapter has no Replay roundtrip implementation',
        result:practiceReplayRoundTrip??null}}));
  }

  const pointerResult=parameterEpoch?.p06;
  const pointerValid=pointerResult?.status==='observed'&&pointerResult.keyboard?.key==='F7'&&
    pointerResult.keyboard.before===false&&pointerResult.keyboard.after===true&&
    pointerResult.pointer?.before===true&&pointerResult.pointer?.after===false&&
    pointerResult.menuOpen===true&&
    Number.isFinite(pointerResult.pointer?.mapping?.logical?.x)&&Number.isFinite(pointerResult.pointer?.mapping?.logical?.y);
  rows.push(boundPracticeCheck({id:'PRACTICE-06',game,variant,
    status:statusWithAudit(pointerResult?.status==='failed'?'failed':pointerValid?'observed':'incomplete'),
    scope:'native F7 key and native canvas pointer toggle the same setting in both directions',
    observation:{epoch:parameterEpoch?.epoch??null,result:pointerResult??null}}));

  const localeReviewRows=enabledRows.map(({expected,item})=>({epoch:expected.epoch,locale:expected.locale,
    configuredLocale:item?.opened?.options?.thpracLocale??null,screenshot:item?.screenshot??null,
    localeReview:item?.localeReview??null,status:item?.status??'missing'}));
  const localeEvidenceValid=exactMatrix&&localeReviewRows.every(row=>row.status==='passed'&&
    row.configuredLocale===row.locale&&typeof row.screenshot==='string'&&row.screenshot.length>0);
  rows.push(boundPracticeCheck({id:'PRACTICE-07',game,variant,
    status:statusWithAudit(anyFailed?'failed':localeEvidenceValid?'observed':'incomplete'),
    scope:'configured locale plus screenshots awaiting manual glyph and string review',
    observation:{locales:localeReviewRows,humanReview:'pending'}}));

  const inventory=parameterEpoch?.featureInventory;
  const inventoryValid=Boolean(practiceAdapter)&&inventory?.status==='observed'&&
    Array.isArray(inventory.items)&&inventory.items.length>0&&
    inventory.items.every(item=>typeof item.id==='string'&&item.id.length>0&&typeof item.label==='string'&&
      typeof item.available==='boolean'&&typeof item.exercised==='boolean'&&typeof item.source==='string'&&item.source.length>0)&&
    new Set(inventory.items.map(item=>item.id)).size===inventory.items.length;
  rows.push(boundPracticeCheck({id:'PRACTICE-08',game,variant,
    status:statusWithAudit(inventoryValid?'observed':'incomplete'),
    scope:'explicit feature inventory with native source owner and exercised subset',
    observation:{epoch:parameterEpoch?.epoch??null,inventory:inventory??null}}));
  return rows;
}
