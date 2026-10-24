import test from 'node:test';
import assert from 'node:assert/strict';
import {practiceAdaptationChecks,practiceBuildFeature,practiceGameplayReady,runPracticeGameplayStage} from './practice-observations.mjs';
import {createStageRecorder} from './runtime-desktop-stage.mjs';

test('practice readiness uses the title world clock and rejects app/network clocks or another seat',()=>{
  const th08={game:'th08',localSeat:0,phase:{name:'stage'},pause:{active:false},frames:{simulation:80,application:800}};
  assert.equal(practiceGameplayReady(th08),true);
  const th08Legacy={game:'th08',localSeat:0,phase:{name:'stage'},pause:{active:false},frames:{enemy:80,control:78,application:800}};
  assert.equal(practiceGameplayReady(th08Legacy),true,'legacy TH08 trace exposes its native world frame as frames.enemy');
  assert.equal(practiceGameplayReady({...th08,frames:{application:800}}),false);
  assert.equal(practiceGameplayReady({...th08,localSeat:1}),false);
  assert.equal(practiceGameplayReady({...th08,pause:{active:true}}),false);
  const th11={game:'th11',localSeat:0,phase:{name:'stage'},pause:{active:false},frames:{simulation:80}};
  assert.equal(practiceGameplayReady(th11),true);
  assert.equal(practiceGameplayReady({...th11,frames:{multiplayer:80,application:90}}),false);
  assert.equal(practiceGameplayReady({...th11,phase:{name:'title'}}),false);
});

test('native practice selection stage binds its gameplay snapshot to both recorded fields',async()=>{
  const record={},stage=createStageRecorder(record,async()=>{}, {now:()=>new Date(0),monotonic:()=>1});
  const snapshot={game:'th08',localSeat:0,phase:{name:'stage'},pause:{active:false},
    frames:{simulation:27},worldProgress:{counters:{enemySceneFrame:27}}};
  const result=await runPracticeGameplayStage({record,stage,epoch:1,ready:practiceGameplayReady,
    perform:async()=>({titleSelection:{screen:11,cursor:3},snapshot,practice:{state:{active:true}}}),
    assess:practice=>({status:practice?.state?.active===true?'observed':'incomplete'})});
  assert.equal(record.stages[0].stage,'native-practice-selection');
  assert.equal(record.stages[0].status,'passed');
  assert.equal(result.snapshot,snapshot);
  assert.equal(record.nativePracticeMenu,result);
  assert.equal(record.gameplay,snapshot);
});

test('native practice selection stage records and rejects a missing gameplay-ready snapshot',async()=>{
  const record={},stage=createStageRecorder(record,async()=>{}, {now:()=>new Date(0),monotonic:()=>1});
  await assert.rejects(runPracticeGameplayStage({record,stage,epoch:1,ready:practiceGameplayReady,
    perform:async()=>({titleSelection:{screen:11,cursor:3},snapshot:undefined}),assess:()=>({status:'observed'})}),/gameplay-ready snapshot/);
  assert.equal(record.stages[0].stage,'native-practice-selection');
  assert.equal(record.stages[0].status,'failed');
  assert.equal(record.gameplay,undefined);
});

test('unknown or malformed practice declarations cannot become an unsupported-product N/A',()=>{
  const unsupported={features:{thprac:false}},supported={features:{thprac:true}};
  for(const thprac of [undefined,null,0,1,'true','false'])
    assert.throws(()=>practiceBuildFeature({features:{thprac}},unsupported),/explicit boolean/);
  assert.throws(()=>practiceBuildFeature({features:{thprac:true}},unsupported),/exceeds product support/);
  assert.equal(practiceBuildFeature({features:{thprac:false}},unsupported),false);
  assert.equal(practiceBuildFeature({features:{thprac:true}},supported),true);
  assert.equal(practiceBuildFeature({features:{thprac:false}},supported),false);
});

test('a supported product may publish a disabled package without claiming unobserved Host and UI behavior',()=>{
  const input={game:'th08',productSupported:true,feature:false};
  const declaration=practiceAdaptationChecks(input).find(check=>check.id==='PRACTICE-01');
  assert.equal(declaration.status,'observed');assert.equal(declaration.complete,false);
  assert.deepEqual(declaration.observation.observation,{feature:false,productSupported:true});
  // Native enable/locale observations are not the missing Host/publication/UI capability matrix.
  const withLocales=practiceAdaptationChecks({...input,locales:practiceLocaleRows()}).find(check=>check.id==='PRACTICE-01');
  assert.equal(withLocales.status,'observed');assert.equal(withLocales.complete,false);
  const audited=practiceAdaptationChecks({...input,finalErrorAudit:{status:'failed'}}).find(check=>check.id==='PRACTICE-01');
  assert.equal(audited.status,'failed');assert.equal(audited.complete,false);
});

test('unknown support and unsupported enabled declarations cannot become a valid package observation',()=>{
  for(const input of [{productSupported:undefined,feature:false},{productSupported:true,feature:undefined}]){
    const row=practiceAdaptationChecks({game:'th08',...input}).find(check=>check.id==='PRACTICE-01');
    assert.equal(row.status,'incomplete');assert.equal(row.complete,false);
  }
  const invalid=practiceAdaptationChecks({game:'th20',productSupported:false,feature:true}).find(check=>check.id==='PRACTICE-01');
  assert.equal(invalid.status,'failed');assert.equal(invalid.complete,false);
});

function practiceLocaleRows(){
  return[
    {epoch:1,enabled:true,locale:'ja-JP',status:'passed',before:{open:false},
      opened:{open:true,options:{thpracEnabled:true,thpracLocale:'ja-JP'},events:[{open:true}]},
      closed:{open:false,events:[{open:true},{open:false}]},screenshot:'th06-ja-JP-practice.png',localeReview:'pending'},
    {epoch:2,enabled:true,locale:'en-US',status:'passed',before:{open:false},
      opened:{open:true,options:{thpracEnabled:true,thpracLocale:'en-US'},events:[{open:true}]},
      closed:{open:false,events:[{open:true},{open:false}]},screenshot:'th06-en-US-practice.png',localeReview:'pending'},
    {epoch:3,enabled:true,locale:'zh-CN',status:'passed',before:{open:false},
      opened:{open:true,options:{thpracEnabled:true,thpracLocale:'zh-CN'},events:[{open:true}]},
      closed:{open:false,events:[{open:true},{open:false}]},screenshot:'th06-zh-CN-practice.png',localeReview:'pending'},
    {epoch:4,enabled:false,locale:'en-US',status:'passed',before:{open:false},
      opened:{open:false,options:{thpracEnabled:false,thpracLocale:'en-US'},events:[]},closed:{open:false,events:[]}},
  ];
}

test('practice numbered results bind only package, consumed options, menu toggles and locale-review evidence',()=>{
  const checks=practiceAdaptationChecks({game:'th06',variant:'normal',productSupported:true,feature:true,locales:practiceLocaleRows()});
  assert.deepEqual(checks.map(({id,status,complete})=>({id,status,complete})),[
    {id:'PRACTICE-01',status:'observed',complete:false},
    {id:'PRACTICE-02',status:'observed',complete:false},
    {id:'PRACTICE-03',status:'observed',complete:false},
    {id:'PRACTICE-04',status:'incomplete',complete:false},
    {id:'PRACTICE-06',status:'incomplete',complete:false},
    {id:'PRACTICE-07',status:'observed',complete:false},
    {id:'PRACTICE-08',status:'incomplete',complete:false},
  ]);
  assert.ok(checks.every(check=>check.observation.game==='th06'&&check.observation.variant==='normal'&&
    check.observation.adaptationIds.includes(check.id)));
  assert.equal(checks[2].scope,'actual gameplay practice menu open and close state');
  const localeCheck=checks.find(check=>check.id==='PRACTICE-07');
  assert.equal(localeCheck.observation.observation.humanReview,'pending');
  assert.deepEqual(localeCheck.observation.observation.locales.map(item=>item.locale),['ja-JP','en-US','zh-CN']);
});

test('practice result rows become incomplete for missing locales, bad consumed options or missing menu transitions',()=>{
  const locales=practiceLocaleRows();
  const missing=practiceAdaptationChecks({game:'th06',productSupported:true,feature:true,locales:locales.slice(0,3)});
  assert.equal(missing.find(check=>check.id==='PRACTICE-02').status,'incomplete');
  assert.equal(missing.find(check=>check.id==='PRACTICE-03').status,'incomplete');
  assert.equal(missing.find(check=>check.id==='PRACTICE-07').status,'incomplete');

  const wrongOptions=practiceLocaleRows();wrongOptions[1].opened.options.thpracLocale='ja-JP';
  const options=practiceAdaptationChecks({game:'th06',productSupported:true,feature:true,locales:wrongOptions});
  assert.equal(options.find(check=>check.id==='PRACTICE-02').status,'incomplete');
  assert.equal(options.find(check=>check.id==='PRACTICE-07').status,'incomplete');

  const noCloseEvent=practiceLocaleRows();noCloseEvent[0].closed.events=[{open:true}];
  const menu=practiceAdaptationChecks({game:'th06',productSupported:true,feature:true,locales:noCloseEvent});
  assert.equal(menu.find(check=>check.id==='PRACTICE-03').status,'incomplete');
  assert.equal(menu.find(check=>check.id==='PRACTICE-02').status,'observed');
});

test('practice audit failures and unsupported products do not report behavior as observed',()=>{
  const failed=practiceAdaptationChecks({game:'th06',productSupported:true,feature:true,locales:practiceLocaleRows(),
    finalErrorAudit:{status:'failed'}});
  assert.ok(failed.every(check=>check.status==='failed'&&check.complete===false));
  const unsupported=practiceAdaptationChecks({game:'th20',productSupported:false,feature:false});
  assert.ok(unsupported.every(check=>check.status==='not-applicable'&&check.complete===false));
});

test('th08 practice rows require actual parameter consumption, both input paths and a source-backed inventory',()=>{
  const locales=practiceLocaleRows();
  locales[0].parameterConsumption={status:'observed',complete:false,requested:{score:1234560},applied:{score:123456}};
  locales[0].practiceSnapshot={source:{applied:'PracticeRuntime::apply_practice'},units:{score:'native points'}};
  locales[0].p06={status:'observed',keyboard:{key:'F7',before:false,after:true},
    pointer:{before:true,after:false,mapping:{logical:{x:50,y:141}}},menuOpen:true};
  locales[0].featureInventory={status:'observed',complete:false,items:[
    {id:'custom',label:'Custom settings',available:true,exercised:true,source:'PracticeRuntime::apply_practice'},
    {id:'pointer',label:'Native pointer',available:true,exercised:true,source:'ThpracUi::mouse'},
  ]};
  const checks=practiceAdaptationChecks({game:'th08',productSupported:true,feature:true,locales});
  assert.equal(checks.find(check=>check.id==='PRACTICE-04').status,'incomplete','cached observed without retained initial/live evidence cannot pass');
  assert.equal(checks.find(check=>check.id==='PRACTICE-06').status,'observed');
  assert.equal(checks.find(check=>check.id==='PRACTICE-08').status,'observed');

  locales[0].parameterConsumption={status:'failed',mismatches:[{field:'rank'}]};
  locales[0].p06.pointer.after=true;
  locales[0].featureInventory.items[1].source='';
  const rejected=practiceAdaptationChecks({game:'th08',productSupported:true,feature:true,locales});
  assert.equal(rejected.find(check=>check.id==='PRACTICE-04').status,'failed');
  assert.equal(rejected.find(check=>check.id==='PRACTICE-06').status,'incomplete');
  assert.equal(rejected.find(check=>check.id==='PRACTICE-08').status,'incomplete');
});

test('th08 lifecycle numbered result recomputes retained evidence rather than trusting its status',()=>{
  const locales=practiceLocaleRows();
  locales[0].practiceLifecycle={status:'observed',complete:true};
  const row=practiceAdaptationChecks({game:'th08',productSupported:true,feature:true,locales}).find(check=>check.id==='PRACTICE-03');
  assert.equal(row.status,'incomplete');assert.equal(row.complete,false);
  assert.equal(row.observation.observation.lifecycle.status,'incomplete');
  assert.ok(row.observation.observation.lifecycle.missing.includes('retried'));
  assert.equal(row.observation.observation.lifecycleEvidence,null);
});

test('missing real Launcher settings evidence remains incomplete and cannot hide an existing failure',()=>{
  const launcherPractice={status:'observed',passed:true,scenes:[],cleanupErrors:[]};
  const missing=practiceAdaptationChecks({game:'th08',productSupported:true,feature:true,locales:practiceLocaleRows(),launcherPractice});
  for(const id of ['PRACTICE-01','PRACTICE-02'])assert.equal(missing.find(row=>row.id===id).status,'incomplete');
  const locales=practiceLocaleRows();locales[0].status='failed';
  const failed=practiceAdaptationChecks({game:'th08',productSupported:true,feature:true,locales,launcherPractice});
  assert.equal(failed.find(row=>row.id==='PRACTICE-02').status,'failed');
});
