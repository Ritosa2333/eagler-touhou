import test from 'node:test';
import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from './registry.mjs';
import {assessRuntimeInputEvidence} from './input-evidence.mjs';

const product={game:'th08',variant:'normal'};
const adapter=resolveAdaptationAdapter(product.game,product.variant);
const clone=value=>structuredClone(value);
function report(id,adaptationId,observation){
  const entry={id,adaptationIds:[adaptationId],status:'passed',complete:true,observation};
  return{driver:{schema:'eagler-touhou/runtime-desktop-probes/1',targets:[{...product,checks:[entry]}]},entry};
}
function launcherBase(id,adaptationId,schema){
  const observation={schema,...product,target:{...product,epoch:3},transition:{epoch:3},clock:{mode:'native-manual'},
    contractEvidence:{inputMasks:clone(adapter.actions.inputMasks),movement:clone(adapter.movement)}};
  return report(id,adaptationId,observation);
}
function testAssessment(id,adaptationId,observation){
  const value=report(id,adaptationId,observation);
  return assessRuntimeInputEvidence({...value,product});
}
function gamepadEvidence(){
  const value=launcherBase('shared-launcher-gamepad-mapping','INPUT-03','eagler-touhou/launcher-gamepad-scenes/1');
  const e=value.entry.observation,first={key:'pad-a',index:0},second={key:'pad-b',index:1};
  e.mappingSeat=0;e.nativeSeat=0;e.identities=[first,second];e.deviceFixture={devices:[first,second]};e.reloaded=true;
  const labels=['first-selected-neutral','nonselected-device-button','nonselected-device-release','nonselected-device-axis',
    'first-device-shoot-held','first-device-shoot-released','first-device-right-axis','second-selected-neutral',
    'second-device-shoot-held','second-device-shoot-released','first-device-unselected-after-switch',
    'first-device-release-after-switch','first-device-unselected-axis-after-switch','second-device-right-axis',
    'reordered-selected-neutral','reordered-device-shoot-held','reordered-device-shoot-released',
    'reloaded-selected-neutral','reloaded-device-shoot-held','reloaded-device-shoot-released'];
  const selectedFor=label=>label.startsWith('first-device-unselected')?second:
    label.startsWith('nonselected-device')||label.startsWith('first-device-')?first:second;
  const activeLabels=new Set(['first-device-shoot-held','second-device-shoot-held','reordered-device-shoot-held','reloaded-device-shoot-held']);
  const ticks=labels.map((label,index)=>{
    const selected=selectedFor(label),sequence=index+1;
    return{label,before:{sequence:sequence-1},after:{sequence,consumed:{fire:activeLabels.has(label)}},
      samples:[{seat:0,bits:activeLabels.has(label)?1:0,selectedKey:selected.key,selectedIndex:selected.index,devices:[first,second]}]};
  });
  e.ticks=ticks;
  const tick=name=>ticks.find(row=>row.label===name);
  const button=(label,key,index,held,released)=>({kind:'button',label,key,index,seat:0,held:tick(held).after.sequence,
    released:tick(released).after.sequence,sample:{seat:0,bits:1,selectedKey:key,selectedIndex:index}});
  const axis=(label,key,index,sequence)=>({kind:'axis',label,key,index,seat:0,before:{x:10,y:20},after:{x:12,y:20},sequence,
    sample:{seat:0,bits:128,selectedKey:key,selectedIndex:index}});
  e.operations=[
    {kind:'unselected-axis',label:'nonselected-device-axis',selectedKey:first.key,otherKey:second.key,before:{x:10,y:20},after:{x:10,y:20},
      sequence:tick('nonselected-device-axis').after.sequence,samples:tick('nonselected-device-axis').samples},
    button('first-device-shoot',first.key,0,'first-device-shoot-held','first-device-shoot-released'),
    axis('first-device-right-axis',first.key,0,tick('first-device-right-axis').after.sequence),
    button('second-device-shoot',second.key,1,'second-device-shoot-held','second-device-shoot-released'),
    {kind:'unselected-axis',label:'first-device-unselected-axis-after-switch',selectedKey:second.key,otherKey:first.key,
      before:{x:12,y:20},after:{x:12,y:20},sequence:tick('first-device-unselected-axis-after-switch').after.sequence,
      samples:tick('first-device-unselected-axis-after-switch').samples},
    axis('second-device-right-axis',second.key,1,tick('second-device-right-axis').after.sequence),
    {kind:'index-reorder',seat:0,selectedKey:second.key,before:1,after:0,otherKey:first.key,otherBefore:0,otherAfter:1},
    button('reordered-device-shoot',second.key,0,'reordered-device-shoot-held','reordered-device-shoot-released'),
    button('reloaded-device-shoot',second.key,0,'reloaded-device-shoot-held','reloaded-device-shoot-released'),
  ];
  e.stimuli=[
    {label:'nonselected-device-button:nonselected-active',key:second.key,control:{kind:'button',index:0},active:true,value:true,
      receipt:{key:second.key,index:0,pressed:true},nativeConsumerSequence:tick('nonselected-device-button').after.sequence},
    {label:'nonselected-device-axis:nonselected-active',key:second.key,control:{kind:'axis',index:0},active:true,value:1,
      receipt:{key:second.key,index:0,value:1},nativeConsumerSequence:tick('nonselected-device-axis').after.sequence},
    {label:'first-device-unselected-after-switch:nonselected-active',key:first.key,control:{kind:'button',index:0},active:true,value:true,
      receipt:{key:first.key,index:0,pressed:true},nativeConsumerSequence:tick('first-device-unselected-after-switch').after.sequence},
    {label:'first-device-unselected-axis-after-switch:nonselected-active',key:first.key,control:{kind:'axis',index:0},active:true,value:1,
      receipt:{key:first.key,index:0,value:1},nativeConsumerSequence:tick('first-device-unselected-axis-after-switch').after.sequence},
  ];
  e.persisted={selectedKey:second.key,index:0};
  return value;
}
function layoutEvidence(){
  const value=launcherBase('shared-launcher-touch-layout','TOUCH-17','eagler-touhou/launcher-touch-layout-scenes/1');
  const e=value.entry.observation;e.control='fire';
  let profiles={};
  e.orientations=['landscape','portrait'].map((orientation,index)=>{
    const placement=index===0?{x:.3,y:.6,scale:1.2}:{x:.7,y:.4,scale:1.6};
    profiles={...profiles,[orientation]:{controls:{fire:placement}}};
    const stored={key:'th08-touch-layout',value:{profiles:clone(profiles)}};
    const safeRect={left:0,top:0,width:100,height:100};
    const controlRect={left:placement.x*100-5,top:placement.y*100-5,width:10,height:10};
    return{orientation,stored,applied:{available:true,target:'player',orientation,storageKey:stored.key,placement:clone(placement),safeRect,
      controlRect,renderedScale:placement.scale}};
  });
  e.complete=false;
  return value;
}
function touchEvents(pointerId,clientX,clientY){
  return['pointerdown','pointermove','pointerup'].map(type=>({type,pointerType:'touch',pointerId,isTrusted:true,clientX,clientY,
    surfaceOwner:'player',surfaceTarget:'touchDirectSurface'}));
}
function magnifierEvidence(){
  const value=launcherBase('shared-launcher-touch-magnifier','TOUCH-19','eagler-touhou/launcher-touch-magnifier-scenes/1');
  const e=value.entry.observation;
  const gesture=(label,offset)=>{
    const before={source:'native-touch-trace-v2',sequence:10+offset,raw:[1],target:{x:10,y:20}};
    const armed={source:before.source,sequence:before.sequence+1,raw:[2],target:{x:11,y:21}};
    const moved={source:before.source,sequence:before.sequence+2,raw:[3],target:{x:13,y:24}};
    return{label,rect:{left:offset,top:offset,width:200,height:200},screen:{move:{clientX:50+offset,clientY:60+offset}},
      native:{before,armed,moved},delta:{x:2,y:3},events:touchEvents(41+offset,50+offset,60+offset)};
  };
  const plain=gesture('magnifier-off',0),zoomed=gesture('magnifier-on',20);
  const pinchEvents=['pointerdown','pointermove','pointerup'].flatMap(type=>[
    {type,pointerType:'touch',pointerId:71,isTrusted:true,surfaceOwner:'player',surfaceTarget:'touchDirectSurface'},
    {type,pointerType:'touch',pointerId:72,isTrusted:true,surfaceOwner:'player',surfaceTarget:'touchDirectSurface'},
  ]);
  e.gestures=[plain,zoomed];e.comparison={pinch:{zoom:{active:true,target:'player',scale:1.5},events:pinchEvents},
    plainDelta:clone(plain.delta),magnifiedDelta:clone(zoomed.delta)};
  e.browserEvidence={trustedCdpTouch:true};e.complete=false;
  return value;
}
function touchMenuTrial({omitStability=false}={}){
  const contract=adapter.originalMenu,epoch=1,runtime={generation:1,pointer:4097,epoch};
  const title=(screen,cursor,current=0,previous=0)=>({screen,cursor,phase:1,stateTimer2:20,confirmationReady:true,
    input:{current,previous}});
  const row=(label,screen,cursor,current=0,previous=0)=>({label,value:{available:true,runtime:clone(runtime),scene:'title',
    title:title(screen,cursor,current,previous)}});
  const evidence={schema:'eagler-touhou/original-menu-input-scenes/1',ingress:'touch',expected:clone(contract.expected),
    inputMasks:{...contract.inputMasks,cancel:contract.touchCancelMask},sources:clone(contract.sources),geometry:clone(contract.geometry),
    totalBudgetTicks:181,difficultyCursorBefore:0,difficultyCursorAfter:1,cleanup:{released:true},events:[],observations:[
      row('title-menu-ready',0,0),row('select-start-menu-entry-consumer',4,0,contract.inputMasks.confirm),
      row('single-menu-swipe-held-consumer',4,1,contract.inputMasks.down),
    ]};
  for(let tick=1;tick<contract.geometry.swipeHoldTicks;tick++)if(!omitStability)
    evidence.observations.push(row(`single-menu-swipe-held-stability-${tick}`,4,1,contract.inputMasks.down));
  evidence.observations.push(row('return-from-difficulty-menu-consumer',4,1,contract.touchCancelMask),row('wait-title-menu-return',0,0));
  const g=contract.geometry;
  const touch=(label,type,id)=>({label,operation:{type,id},receipt:{acknowledged:true,source:'touch',type,id,
    startedAtMs:10,completedAtMs:11}});
  evidence.events=[touch('select-start-menu-entry-down','down',g.pointerId),touch('select-start-menu-entry-up','up',g.pointerId),
    touch('single-menu-swipe-down','down',g.pointerId),touch('single-menu-swipe-move','move',g.pointerId),
    touch('single-menu-swipe-up','up',g.pointerId),touch('return-from-difficulty-menu-first-down','down',g.pointerId),
    touch('return-from-difficulty-menu-second-down','down',g.cancelPointerId),touch('return-from-difficulty-menu-first-up','up',g.pointerId),
    touch('return-from-difficulty-menu-second-up','up',g.cancelPointerId)];
  const frameBinding={url:`https://localhost/runtime/th08/game.html?runtimeEpoch=${epoch}`,hostEpoch:epoch,nativeRuntime:runtime};
  return{checkStatus:'passed',ingress:'touch',epoch,frameBinding,frameBindingAfter:clone(frameBinding),transition:{epoch,state:{game:product.game,variant:product.variant}},
    clock:{mode:'native-manual'},evidence};
}

test('known input assessors recompute raw Launcher evidence instead of trusting completion flags',()=>{
  const gamepad=gamepadEvidence();
  gamepad.entry.observation.complete=false;
  assert.deepEqual(assessRuntimeInputEvidence({...gamepad,product}),
    {available:true,valid:true,reason:null,kind:'launcher-gamepad-mapping',checks:['INPUT-03']});
  assert.deepEqual(assessRuntimeInputEvidence({...layoutEvidence(),product}),
    {available:true,valid:true,reason:null,kind:'launcher-touch-layout',checks:['TOUCH-17']});
  assert.deepEqual(assessRuntimeInputEvidence({...magnifierEvidence(),product}),
    {available:true,valid:true,reason:null,kind:'launcher-touch-magnifier',checks:['TOUCH-19']});
});

test('gamepad negative rows require actual other-device stimulus and same-tick neutral native consumer',()=>{
  const fixture=gamepadEvidence();
  fixture.entry.observation.stimuli[0].receipt.pressed=false;
  const result=assessRuntimeInputEvidence({...fixture,product});
  assert.equal(result.available,true);assert.equal(result.valid,false);
  assert.match(result.reason,/other-device button was not actually pressed/);
  const wrongSequence=gamepadEvidence();wrongSequence.entry.observation.stimuli[0].nativeConsumerSequence++;
  assert.match(assessRuntimeInputEvidence({...wrongSequence,product}).reason,/same native consumer sample/);
});

test('layout and magnifier reject storage aliasing, untrusted touches, and incorrect native attribution',()=>{
  const layout=layoutEvidence();layout.entry.observation.orientations[1].stored.value.profiles.landscape.controls.fire.x=.9;
  assert.equal(assessRuntimeInputEvidence({...layout,product}).valid,false);
  const magnifier=magnifierEvidence();magnifier.entry.observation.gestures[1].events[1].isTrusted=false;
  assert.match(assessRuntimeInputEvidence({...magnifier,product}).reason,/untrusted/);
  const foreign=magnifierEvidence();foreign.entry.observation.gestures[0].native.moved.sequence++;
  assert.equal(assessRuntimeInputEvidence({...foreign,product}).valid,false);
});

test('magnifier evidence rejects trusted coordinates that were observed outside the actual Player touch surface',()=>{
  const foreign=magnifierEvidence();foreign.entry.observation.gestures[0].events[1].surfaceOwner='outside-player';
  assert.match(assessRuntimeInputEvidence({...foreign,product}).reason,/direct-touch surface inside the launched Player/);
  const wrongTarget=magnifierEvidence();wrongTarget.entry.observation.comparison.pinch.events[0].surfaceTarget='body';
  assert.match(assessRuntimeInputEvidence({...wrongTarget,product}).reason,/direct-touch surface inside the launched Player/);
});

test('menu and Dialogue groups fail closed when any raw ingress or final gameplay restoration is absent',()=>{
  for(const [id,adaptationId] of [['shared-original-menu-dialogue-input','INPUT-07'],
    ['shared-touch-menu-dialogue-gestures','TOUCH-12']]){
    const value=report(id,adaptationId,{complete:true,trials:[]});
    const result=assessRuntimeInputEvidence({...value,product});
    assert.equal(result.available,true);assert.equal(result.valid,false);
    assert.match(result.reason,/trials and final restore are incomplete/);
  }
});

test('TOUCH-12 requires the registered held-swipe stability window, not a single movement tick',()=>{
  const make=menu=>{
    const trial=menu;
    const placeholder=(epoch,kind)=>({kind,ingress:'touch',evidence:{schema:''},checkStatus:'passed',epoch,
      transition:{epoch,state:{game:'th08',variant:'normal'}},clock:{mode:'native-manual'},
      frameBinding:{url:`https://localhost/runtime/th08/game.html?runtimeEpoch=${epoch}`,hostEpoch:epoch,nativeRuntime:{generation:1,pointer:4097,epoch:2}},
      frameBindingAfter:{url:`https://localhost/runtime/th08/game.html?runtimeEpoch=${epoch}`,hostEpoch:epoch,nativeRuntime:{generation:1,pointer:4097,epoch:2}}});
    const restore={...placeholder(4,'restore-gameplay'),transition:{epoch:4,state:{game:'th08',variant:'normal'}}};
    const value=report('shared-touch-menu-dialogue-gestures','TOUCH-12',{trials:[trial,placeholder(2,'short-confirm'),
      placeholder(3,'hold-skip'),restore]});
    return assessRuntimeInputEvidence({...value,product});
  };
  const missing=make(touchMenuTrial({omitStability:true}));
  assert.equal(missing.available,true);assert.equal(missing.valid,false);
  assert.match(missing.reason,/held swipe stability observation 1\/39 is missing/);
  const held=make(touchMenuTrial());
  assert.equal(held.valid,false);
  assert.match(held.reason,/touch Dialogue confirm evidence schema is absent/);
  const independentEpochs=touchMenuTrial();
  independentEpochs.frameBinding.nativeRuntime.epoch=2;
  independentEpochs.frameBindingAfter.nativeRuntime.epoch=2;
  for(const row of independentEpochs.evidence.observations)row.value.runtime.epoch=2;
  assert.match(make(independentEpochs).reason,/touch Dialogue confirm evidence schema is absent/,
    'A stable native loop epoch must not be confused with the Host epoch');
  independentEpochs.evidence.observations[3].value.runtime.epoch=3;
  assert.match(make(independentEpochs).reason,/another Runtime instance or loop epoch/);
});

test('known check rejects a foreign/duplicate Runtime target while unknown checks remain unavailable',()=>{
  const fixture=layoutEvidence();
  fixture.driver.targets.push(clone(fixture.driver.targets[0]));
  assert.equal(assessRuntimeInputEvidence({...fixture,product}).valid,false);
  assert.deepEqual(assessRuntimeInputEvidence({driver:{},entry:{id:'shared-not-registered'},product}),
    {available:false,valid:false,reason:'No Runtime input assessor for shared-not-registered',kind:null});
});

test('fresh confirm standalone uses the same native trial gate and cannot pass from a summary flag',()=>{
  const value=report('shared-touch-dialogue-confirm','TOUCH-13',{complete:true,trials:[]});
  assert.match(assessRuntimeInputEvidence({...value,product}).reason,/native trial and final gameplay restoration/);
  value.entry.observation.trials=[{checkStatus:'passed',evidence:{}},{checkStatus:'passed'}];
  const result=assessRuntimeInputEvidence({...value,product});
  assert.equal(result.available,true);assert.equal(result.valid,false);
});

test('menu evidence accepts its declared keyboard support without granting that support completion',()=>{
  const value=report('shared-original-menu-dialogue-input','INPUT-07',{});
  value.entry.adaptationIds.unshift('INPUT-01');
  const verdict=assessRuntimeInputEvidence({...value,product});
  assert.deepEqual(verdict.checks,['INPUT-07']);
  assert.doesNotMatch(verdict.reason,/declared supporting IDs/);
  for(const ids of [['INPUT-01'],['INPUT-07','INPUT-02'],['INPUT-07','INPUT-07']]){
    value.entry.adaptationIds=ids;
    assert.match(assessRuntimeInputEvidence({...value,product}).reason,/declared supporting IDs/);
  }
});

function th10ControlledReceipt(frame=100){
  const contract=resolveAdaptationAdapter('th10','normal').controlledScene;
  return{available:true,accepted:true,armStatus:2,requestGeneration:1,requestPending:false,
    acceptedBeforeFirstUpdate:true,firstTimelineWithheld:true,updateStarted:true,sessionWriteLocked:true,
    active:true,eligible:true,ready:true,busy:false,replay:false,assisted:true,stage:1,
    enemies:1,bullets:0,lasers:0,timelineCount:1,game:'th10',variant:'normal',
    controlFrame:frame,enemyFrame:frame-2,withheldTimelineUpdates:frame-80,
    identityWords:[0x54493130,1,8,1,0,1,4096,7],runtime:{generation:1,pointer:4096,epoch:7},
    writePolicy:clone(contract.writePolicy),controllerPolicy:clone(contract.controller),
    writeReceipt:{locked:true,loadScope:false,attempts:0,ramOverrides:0,discardedBytes:0,committedBytes:0,rejectedWrites:0},
    controller:{schema:'th10/controlled-main/1',raw:[1,1,1,200,300,400,400,frame-80,0,0,1,1,200,200,1,0,0,0,0,400,1,frame-2,frame,1]}};
}
function forTh10(value){
  const adapter=resolveAdaptationAdapter('th10','normal');
  value.driver.targets[0].game='th10';
  const e=value.entry.observation;e.game='th10';e.target.game='th10';
  e.contractEvidence={inputMasks:clone(adapter.actions.inputMasks),movement:clone(adapter.movement)};
  e.controlledScene={acceptance:th10ControlledReceipt(),receipts:[th10ControlledReceipt(101),th10ControlledReceipt(102)]};
  return value;
}
test('TH10 offline input assessment requires the real inert main identity on every captured gameplay tick',()=>{
  const product={game:'th10',variant:'normal'};
  for(const fixture of [gamepadEvidence,layoutEvidence,magnifierEvidence])
    assert.equal(assessRuntimeInputEvidence({...forTh10(fixture()),product}).valid,true);
  for(const mutate of [
    e=>delete e.controlledScene,
    e=>delete e.controlledScene.receipts[0].controller,
    e=>e.controlledScene.receipts[0].enemies=2,
    e=>e.controlledScene.receipts[0].controller.raw[8]=1,
    e=>e.controlledScene.receipts[0].controller.raw[6]++,
    e=>{e.controlledScene.receipts[0].identityWords[7]++;e.controlledScene.receipts[0].runtime.epoch++;},
    e=>e.controlledScene.receipts[0].writeReceipt.committedBytes=1,
  ]){
    const value=forTh10(gamepadEvidence());mutate(value.entry.observation);
    const result=assessRuntimeInputEvidence({...value,product});
    assert.equal(result.available,true);assert.equal(result.valid,false);
  }
});
