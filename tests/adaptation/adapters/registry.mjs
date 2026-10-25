import assert from 'node:assert/strict';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {validateTitleFlow,validateObservationFlow} from './flow-steps.mjs';
import {assertBombResource} from '../../../tools/ports/bomb-resource-contract.mjs';
import th08 from './titles/th08.mjs';
import th10 from './titles/th10.mjs';

const SCHEMA='eagler-touhou/adaptation-adapter/1';
const identifier=/^[A-Za-z_$][\w$]*$/;
const capabilities=new Set(['runtime','observation','flow','clock','movement','actions','doubleTapBomb','deathbomb','runBoundaries','dialogueTouchScene','originalMenu','controlledScene','audioLatency','deriveSfxContract','language','classifyLanguageDiagnostic','parseDialogueText','replay','parseReplayFrames','parseReplayMetadata','parseReplayExtension','inspectOriginalReplay','practice','storage','music','inputLifecycle','viewOptions']);
const name=(value,label)=>assert.ok(typeof value==='string'&&identifier.test(value),`${label} must be a JavaScript/native identifier`);
function freeze(value){
  if(value&&typeof value==='object'){
    for(const child of Object.values(value))freeze(child);
    Object.freeze(value);
  }
  return value;
}
function cloneWithFunctions(value){
  if(Array.isArray(value))return value.map(cloneWithFunctions);
  if(value&&typeof value==='object')return Object.fromEntries(Object.entries(value).map(([key,child])=>[key,cloneWithFunctions(child)]));
  return value;
}

/** Validate serializable declarations and title-owned source derivation before execution. */
export function createAdapterRegistry(declarations,{products=PRODUCT_GAMES}={}){
  assert.ok(Array.isArray(declarations),'Adapter declarations must be an array');
  const entries=new Map();
  for(const original of declarations){
    const {deriveSfxContract,classifyLanguageDiagnostic,parseDialogueText,parseReplayFrames,parseReplayMetadata,parseReplayExtension,inspectOriginalReplay,practice,storage,music,...declaration}=original;
    const adapter=structuredClone(declaration);
    if(adapter.viewOptions){
      const view=adapter.viewOptions;assert.equal(view.schema,'eagler-touhou/view-options-adapter/1');assert.ok(view.source?.trim());
      name(view.snapshot?.export,'View option snapshot');assert.equal(view.snapshot.wordCount,32);assert.equal(view.snapshot.wordType,'f64');assert.equal(view.snapshot.app,true);
      assert.ok(Number.isFinite(view.markerOffset?.x)&&Number.isFinite(view.markerOffset?.y),'View option consumers need the declared native-to-canvas origin');
    }
    if(adapter.inputLifecycle){
      const declaration=adapter.inputLifecycle;
      assert.equal(declaration.schema,'eagler-touhou/input-lifecycle-adapter/1');
      assert.ok(declaration.source?.trim()&&declaration.audioRole?.trim(),'Input lifecycle needs source-owned mixer identity');
      assert.ok(Array.isArray(declaration.audioContextPath)&&declaration.audioContextPath.length>0);
      for(const part of declaration.audioContextPath)name(part,'Input lifecycle audio context path');
    }
    for(const [key,value] of Object.entries({deriveSfxContract,classifyLanguageDiagnostic,parseDialogueText,parseReplayFrames,parseReplayMetadata,parseReplayExtension,inspectOriginalReplay})){
      if(value===undefined)continue;
      assert.equal(typeof value,'function',`${key} must be a title-owned derivation function`);
      adapter[key]=value;
    }
    if(practice!==undefined){
      assert.equal(typeof practice,'object','Practice capability must be a title-owned declaration');
      adapter.practice=cloneWithFunctions(practice);
    }
    if(storage!==undefined){
      assert.equal(storage.schema,'eagler-touhou/native-storage/1');
      assert.ok(storage.source?.trim()&&Array.isArray(storage.files)&&storage.files.length>0,'Native storage needs source-backed file declarations');
      assert.equal(new Set(storage.files.map(file=>file.path)).size,storage.files.length,'Native storage paths must be unique');
      for(const file of storage.files){
        assert.ok(typeof file.path==='string'&&file.path&&!file.path.includes('..')&&!/^[\\/]|:/.test(file.path),'Invalid native storage path');
        assert.ok(['configuration','score'].includes(file.kind)&&typeof file.inspect==='function','Native storage requires an independent file reader');
      }
      adapter.storage=cloneWithFunctions(storage);
    }
    if(music!==undefined){
      assert.equal(music.schema,'eagler-touhou/native-music/1');
      assert.equal(typeof music.parseCatalog,'function','Music needs an independent title catalog reader');
      for(const key of ['titleFile','stageFile','titleMidi','stageMidi','source'])assert.ok(music[key]?.trim(),`Music needs ${key}`);
      assert.ok(Number.isSafeInteger(music.stageSong)&&music.stageSong>=0,'Music needs the native first-stage song index');
      if(music.gameplayRestart!==undefined){
        assert.equal(typeof music.normalizeRestart,'function','Native music restart needs a title observation normalizer');
        const restart=music.gameplayRestart;
        assert.ok(Number.isSafeInteger(restart.minimumFrame)&&restart.minimumFrame>0&&restart.minimumFrame<=3600&&
          Number.isSafeInteger(restart.stage)&&restart.stage>=0&&restart.source?.trim(),'Native music restart needs bounded frame and stage expectations');
        for(const key of ['pauseKey','restartKey'])assert.match(restart[key],/^[A-Za-z][A-Za-z0-9]{0,31}$/);
      }
      if(music.parseCues!==undefined||music.buildCueRecipe!==undefined){
        assert.equal(typeof music.parseCues,'function');assert.equal(typeof music.buildCueRecipe,'function');
      }
      adapter.music=cloneWithFunctions(music);
    }
    assert.equal(adapter.schema,SCHEMA,'Unsupported adaptation adapter schema');
    const product=products[adapter.game];
    assert.ok(product,`Register product ${adapter.game} before its adaptation adapter`);
    assert.ok(['normal','multiplayer'].includes(adapter.variant),'Adapter variant must be explicit');
    if(adapter.variant==='multiplayer')assert.ok(product.multiplayerRuntime,`${adapter.game} has no multiplayer Runtime`);
    for(const key of Object.keys(adapter))assert.ok(['schema','game','variant'].includes(key)||capabilities.has(key),`Unknown adapter capability ${key}`);
    assert.ok(adapter.runtime,'An adapter needs its real Runtime mounting declaration');
    for(const key of ['global','core','module','app'])name(adapter.runtime[key],`runtime.${key}`);
    if(adapter.doubleTapBomb){
      const g=adapter.doubleTapBomb;
      for(const key of ['firstTapMaxMs','interTapMaxMs'])assert.ok(Number.isFinite(g[key])&&g[key]>0,`Double-tap ${key} must be positive`);
      assert.ok(Number.isSafeInteger(g.bombPulseTicks)&&g.bombPulseTicks>0,'Double-tap pulse must name its native tick budget');
      assert.ok(g.source?.trim(),'Double-tap timing requires its production source');
    }
    if(adapter.deathbomb){
      const d=adapter.deathbomb;
      assert.equal(d.schema,'eagler-touhou/stg-deathbomb/1');assert.equal(d.wordCount,36);
      for(const key of ['stateExport','hitExport'])name(d[key],`deathbomb.${key}`);
      assert.ok(d.source?.trim(),'Deathbomb fixture requires its production collision source');
    }
    if(adapter.runBoundaries){
      const r=adapter.runBoundaries;
      for(const key of ['stateExport','armExport','transitionExport'])name(r[key],`runBoundaries.${key}`);
      assert.ok(Number.isSafeInteger(r.gameScene)&&Number.isSafeInteger(r.resultScene)&&r.gameScene!==r.resultScene,
        'Ordinary-run scene meanings must be distinct');
      assert.ok(Number.isSafeInteger(r.nonNormalFlags)&&r.nonNormalFlags>0,'Declare excluded ordinary-run flags');
      for(const key of ['runtimeIdentity','runId','runMarker','touchTrace','stageTransition','resultTransition','resultLag','replayFinalizer','replayParser'])
        assert.ok(r.sources?.[key]?.trim(),`Ordinary-run ${key} needs its production source`);
    }
    if(adapter.dialogueTouchScene){
      const d=adapter.dialogueTouchScene;
      assert.equal(d.schema,'eagler-touhou/dialogue-touch-scene/1');
      assert.equal(d.game,adapter.game,'Dialogue contract must name the registered product');
      assert.equal(d.variant,adapter.variant,'Dialogue contract must name the registered variant');
      for(const key of ['manifest','observation','request'])name(d[key]?.export,`dialogueTouchScene.${key}.export`);
      assert.ok(Number.isSafeInteger(d.stage)&&d.stage>=0,'Dialogue fixture needs its source stage');
      assert.ok(Number.isSafeInteger(d.waitOpcode)&&d.waitOpcode>=0,'Declare the script confirmation-wait meaning');
    }
    if(adapter.originalMenu){
      const m=adapter.originalMenu;name(m.statusExport,'originalMenu.statusExport');name(m.identity?.export,'originalMenu.identity.export');
      assert.ok(Number.isSafeInteger(m.identity.wordCount)&&m.identity.wordCount>=3&&m.identity.wordCount<=4096,'Declare the bounded title identity ABI');
      for(const key of ['generation','pointer','epoch'])assert.ok(Number.isSafeInteger(m.identity.fields?.[key])&&m.identity.fields[key]>=3&&m.identity.fields[key]<m.identity.wordCount,`Declare originalMenu identity ${key}`);
      for(const key of ['scene','screen','cursor','phase','stateTimer2','confirmationReady','currentInput','previousInput'])
        assert.ok(Number.isSafeInteger(m.fields?.[key])&&m.fields[key]>=0,`Declare native title ${key} field`);
      for(const key of ['titleState','navigation','cancel'])assert.ok(m.sources?.[key]?.trim(),`Declare native menu ${key} source`);
      if(m.signedFields)for(const key of m.signedFields)assert.ok(['scene','screen','cursor','phase','stateTimer2','startupElapsed'].includes(key),'Only native signed title fields may be signed');
      if(m.startup){
        assert.equal(m.startup.schema,'eagler-touhou/title-startup-navigation/1');
        assert.ok(Number.isInteger(m.startup.ticks)&&m.startup.ticks>0&&m.startup.ticks<=256,'Declare a bounded native startup sequence');
        assert.ok(m.startup.source?.trim()&&m.fields.startupElapsed!==undefined,'Startup needs its actual drawing counter and source');
      }
    }
    const observation=adapter.observation;
    assert.ok(observation&&typeof observation==='object','Declare how native observations are read');
    assert.ok(['native-v1','legacy-th08-status-trace','unsupported'].includes(observation.kind),
      'Observation kind must be native-v1, legacy-th08-status-trace, or unsupported');
    if(observation.kind==='native-v1'){
      name(observation.export,'observation.export');
      assert.ok(observation.source?.trim(),'Native observation needs its production source anchor');
      assert.ok(observation.sampleBoundary?.trim(),'Native observation needs its sampling boundary');
      assert.ok(['consumed','queued-for-next-movement'].includes(observation.inputSemantics),
        'Declare native input consumption semantics');
    }else if(observation.kind==='legacy-th08-status-trace'){
      assert.equal(adapter.game,'th08','The implemented legacy status trace is TH08-specific');
      assert.equal(adapter.variant,'normal','The implemented legacy status trace is normal-Runtime-only');
      assert.equal(observation.sampleBoundary,'after-application-update','Declare the actual TH08 legacy snapshot boundary');
      assert.equal(observation.inputSource,'diagnostics[11] | player_state.input.buttons',
        'Declare the actual TH08 queued input source');
      assert.equal(observation.inputSemantics,'queued-for-next-movement',
        'TH08 diagnostics[11] is sampled before the queued movement consumes input');
    }else{
      assert.ok(typeof observation.reason==='string'&&observation.reason.trim(),
        'Unsupported observation must state why no interface is available');
      assert.equal(observation.export,undefined,'Unsupported observations cannot claim an export');
    }
    if(adapter.flow){
      const result=validateTitleFlow(adapter.flow);
      assert.equal(result.status,'passed',result.issues.map(({path,reason})=>`${path}: ${reason}`).join('; '));
    }
    if(adapter.clock){
      const c=adapter.clock;
      for(const key of ['tickApp','startApp'])assert.equal(typeof c[key],'boolean',`clock.${key} must declare its native signature`);
      for(const key of ['stopExport','tickExport','startExport'])name(c[key],`clock.${key}`);
      if(c.pauseExport!==undefined)name(c.pauseExport,'clock.pauseExport');
      assert.ok(c.source?.trim(),'Manual clock must identify its production loop owner');
    }
    if(adapter.actions){
      const a=adapter.actions;
      if(a.bombResource)assertBombResource(a.bombResource);
      assert.equal(a.schema,'eagler-touhou/stg-actions/1','Unknown STG action observation schema');
      name(a.stateExport,'actions.stateExport');
      assert.equal(a.wordCount,20,'STG action observation v1 has 20 words');
      assert.equal(typeof a.stateApp,'boolean','Declare the action observation app signature');
      assert.equal(a.sampleBoundary,'after-player-consumer','Action state must identify the actual player consumer boundary');
      assert.ok(Number.isSafeInteger(a.escapePulseTicks)&&a.escapePulseTicks>0&&a.escapePulseTicks<=60&&a.escapePulseSource?.trim(),
        'Declare the native Escape pulse lifetime and its production source');
      assert.ok(Array.isArray(a.source)&&a.source.length&&a.source.every(source=>typeof source==='string'&&source.trim()),
        'Action observation needs its production consumer source anchors');
      const masks=['fire','focus','bomb','escape'].map(action=>a.inputMasks?.[action]);
      assert.ok(masks.every(mask=>Number.isSafeInteger(mask)&&mask>0&&mask<=0x80000000&&Number.isInteger(Math.log2(mask)))&&
        new Set(masks).size===masks.length,'Declare distinct native action input bits');
    }
    if(adapter.movement){
      if(adapter.movement.directionMasks){
        const masks=['ArrowUp','ArrowDown','ArrowLeft','ArrowRight'].map(key=>adapter.movement.directionMasks[key]);
        assert.ok(masks.every(value=>Number.isSafeInteger(value)&&value>0&&value<=0xffff&&(value&(value-1))===0)&&new Set(masks).size===4,
          'Direction input masks need four distinct native bits');
        assert.ok(adapter.movement.directionSource?.trim(),'Direction input masks need their production source');
      }
      assert.ok(adapter.movement.source?.trim(),'Native movement source is required');
      assert.ok(['queued-for-next-movement','synchronous-consumption'].includes(adapter.movement.inputBoundary),
        'Declare whether native movement input is queued or consumed synchronously');
      assert.ok(adapter.movement.inputBoundarySource?.trim(),'Native movement input boundary needs its production source anchor');
      const p=adapter.movement.precision;
      assert.ok(Number.isInteger(p?.unitsPerPixel)&&p.unitsPerPixel>0,'Native position units must be a positive integer');
      assert.ok(['float','toward-zero','nearest'].includes(p.rounding),'Unknown native movement rounding');
      assert.ok(['before-limit','after-limit'].includes(p.scalePlacement),'Unknown native movement scale boundary');
      assert.ok(p.limitedQuantization===undefined||p.limitedQuantization==='nearest-within-speed',
        'Unknown limited movement quantizer');
      if(adapter.movement.inputBoundary==='queued-for-next-movement'){
        const policy=adapter.movement.keyboardReleasePolicy;
        assert.ok(policy&&policy.semantics==='queued-for-next-movement'&&policy.observationSource?.trim()&&
          policy.observationPhase?.trim()&&Number.isInteger(policy.maxMovementTicks)&&policy.maxMovementTicks>0,
        'Queued movement needs a source-anchored keyboard release policy');
      }
    }
    if(adapter.controlledScene){
      const c=adapter.controlledScene;
      assert.equal(c.schema,'eagler-touhou/controlled-input-scene/3','Unknown controlled scene contract');
      name(c.armExport,'controlledScene.armExport');name(c.stateExport,'controlledScene.stateExport');
      name(c.armStateExport,'controlledScene.armStateExport');
      name(c.writeStateExport,'controlledScene.writeStateExport');
      assert.ok(c.source?.trim(),'Controlled scene must identify the production observation owner');
      assert.ok(c.writePolicy&&['none','ram-score-overlay','ram-session-overlay'].includes(c.writePolicy.initialization),
        'Controlled scene must declare its initialization write policy');
      assert.ok(c.writePolicy.source?.trim(),'Controlled scene write policy needs a production source anchor');
      if(c.controller){
        assert.equal(adapter.game,'th10');assert.equal(adapter.variant,'normal');
        assert.equal(c.controller.schema,'th10/controlled-main/1');assert.equal(c.controller.wordCount,24);
        name(c.controller.stateExport,'controlledScene.controller.stateExport');
        assert.ok(c.controller.source?.trim(),'The real inert controller needs its production source');
        assert.equal(c.writePolicy.initialization,'ram-session-overlay');
        assert.equal(c.identity?.wordCount,8);assert.equal(c.identity?.version,1);assert.equal(c.identity?.magic,0x54493130);
        name(c.identity.export,'controlledScene.identity.export');
        assert.deepEqual(c.requestEffects?.dialogue&&Object.fromEntries(['controlFrame','enemyFrame','withheldTimelineUpdates'].map(key=>[key,c.requestEffects.dialogue[key]])),
          {controlFrame:0,enemyFrame:2,withheldTimelineUpdates:0},'TH10 dialogue retains both real clear-enemy clock effects');
        assert.ok(c.requestEffects.dialogue.source?.trim());
      }
    }
    if(adapter.language){
      const language=adapter.language;
      assert.equal(language.schema,'eagler-touhou/language-scenes/1','Unknown language scene contract');
      for(const key of ['enableExport','sizeExport','copyExport','allocateExport','freeExport'])name(language[key],`language.${key}`);
      assert.ok(language.source?.trim(),'Language observation needs its production source anchor');
      assert.ok(typeof language.table==='string'&&language.table.startsWith(`/thcrap/${adapter.game}/`)&&
        !language.table.includes('..')&&!language.table.includes('\\'),'Language table must belong to the selected game');
      assert.ok(Array.isArray(language.samples)&&language.samples.length>=2,'Language needs independent translated and missing-key samples');
      assert.equal(new Set(language.samples.map(sample=>sample.key)).size,language.samples.length,'Language samples repeat a key');
      for(const sample of language.samples){
        assert.ok(typeof sample.key==='string'&&sample.key.trim()&&sample.operation==='string-by-id',
          'Declare a supported production language lookup key');
        assert.ok(sample.source?.trim(),'Language sample needs its production consumer anchor');
      }
      assert.ok(language.samples.some(sample=>sample.key===language.missingKey),'Missing language control must select a declared key');
      if(language.consumers){
        const spec=language.uiSnapshot;name(spec?.export,'language.uiSnapshot.export');assert.equal(spec.wordType,'f64');assert.equal(typeof spec.app,'boolean');
        assert.ok(Number.isSafeInteger(spec.wordCount)&&spec.wordCount>=2&&spec.wordCount<=4096&&spec.source?.trim());
        const indexes=Object.entries(spec.fields);assert.ok(indexes.length>0&&new Set(indexes.map(([,index])=>index)).size===indexes.length);
        for(const [field,index]of indexes){name(field,'language consumer field');assert.ok(Number.isSafeInteger(index)&&index>=2&&index<spec.wordCount);}
        assert.ok(Array.isArray(spec.booleans)&&new Set(spec.booleans).size===spec.booleans.length&&spec.booleans.every(field=>Object.hasOwn(spec.fields,field)));
        for(const phase of ['enter','exit'])assert.equal(validateObservationFlow(language.consumers.statistics[phase]).status,'passed','Language consumer navigation must use finite observed steps');
        const navigation=language.navigation;assert.ok(navigation&&Object.hasOwn(spec.fields,navigation.inputField));
        assert.ok(Array.isArray(navigation.transitionFields)&&navigation.transitionFields.length>0&&navigation.transitionFields.every(field=>Object.hasOwn(spec.fields,field)));
        assert.ok(Object.keys(navigation.masks).length>0&&Object.values(navigation.masks).every(mask=>Number.isSafeInteger(mask)&&mask>0&&mask<=0x80000000&&Number.isInteger(Math.log2(mask))));
        assert.equal(typeof adapter.parseDialogueText,'function','Dialogue text needs its independent source codec');
        assert.equal(language.consumers.dialogue.archiveKind,'pbgz');
        const dialogue=language.consumers.dialogue;assert.match(dialogue.mountRoot,/^\/[A-Za-z0-9_/-]+\/$/);assert.ok(!dialogue.mountRoot.includes('..'));
        assert.ok(Number.isSafeInteger(dialogue.captureWaitTicks)&&dialogue.captureWaitTicks>0&&dialogue.captureWaitTicks<=60);
        assert.ok(Number.isSafeInteger(dialogue.minimumTextAlpha)&&dialogue.minimumTextAlpha>0&&dialogue.minimumTextAlpha<=255);
        assert.ok(Array.isArray(dialogue.textAlphaFields)&&dialogue.textAlphaFields.length>0&&dialogue.textAlphaFields.every(field=>Object.hasOwn(spec.fields,field)));
        for(const path of language.consumers.dialogue.assets)assert.match(path,/^[A-Za-z0-9_.-]+$/);
      }
      if(language.browserDiagnostics!==undefined||adapter.classifyLanguageDiagnostic!==undefined){
        const diagnostics=language.browserDiagnostics;
        assert.equal(typeof adapter.classifyLanguageDiagnostic,'function',
          'Browser language diagnostics need a title-owned host classifier');
        assert.ok(diagnostics&&typeof diagnostics==='object'&&!Array.isArray(diagnostics),
          'Browser language diagnostics need a serializable Runtime source declaration');
        const asset=diagnostics.runtimeAsset;
        assert.ok(typeof asset==='string'&&asset.trim()&&!asset.startsWith('/')&&!asset.includes('\\')&&
          asset.split('/').every(part=>part&&part!=='.'&&part!=='..'),
          'Browser diagnostic Runtime asset must be a safe Runtime-relative path');
        assert.ok(typeof diagnostics.source==='string'&&diagnostics.source.trim(),
          'Browser diagnostic Runtime asset needs its production source anchor');
      }
    }
    if(!adapter.language)assert.equal(adapter.classifyLanguageDiagnostic,undefined,
      'Browser language diagnostic classifiers require a language capability');
    if(adapter.audioLatency){
      const a=adapter.audioLatency;
      assert.equal(typeof adapter.deriveSfxContract,'function','Audio scene needs independent production sound mapping derivation');
      assert.equal(a.game,adapter.game,'Audio scene belongs to another game');
      assert.equal(a.variant,adapter.variant,'Audio scene belongs to another variant');
      assert.ok(a.sourcePaths&&Object.values(a.sourcePaths).length>0,'Audio scene needs production source anchors');
      assert.equal(a.scenes?.length,2,'Audio scene needs exactly one positive and one queue-tail negative operation');
      const positive=a.scenes.filter(scene=>scene?.role==='positive');
      const negative=a.scenes.filter(scene=>scene?.role==='negative-control');
      assert.equal(positive.length,1,'Audio scene needs exactly one positive role');
      assert.equal(negative.length,1,'Audio scene needs exactly one negative-control role');
      assert.ok(a.scenes.every(scene=>scene.sameTargetVoice===true),
        'Positive and negative audio scenes must address the same production target voice');
      assert.equal(positive[0].mutation,undefined,'Positive audio scene cannot apply a queue mutation');
      assert.equal(negative[0].mutation,'measured-queue-tail',
        'Negative audio scene must apply the measured queue-tail mutation');
      for(const scene of a.scenes){
        assert.ok(typeof scene.name==='string'&&scene.name.trim(),'Audio scene needs a readable name');
        assert.ok(typeof scene.input==='string'&&scene.input.trim(),'Audio scene needs its production input');
        assert.ok(scene.before&&typeof scene.before==='object'&&scene.after&&typeof scene.after==='object',
          'Audio scene needs explicit native before and after states');
      }
      name(a.state?.export,'audioLatency.state.export');
      assert.ok(typeof a.state.inputReadySource==='string'&&a.state.inputReadySource.trim(),
        'Audio readiness needs its production input gate source');
      assert.ok(a.scenes.every(scene=>scene.before.inputReady===1),
        'Every audio scene must wait for the native input gate');
      for(const field of ['activeApp','appStatus','invalid','loading','page','cursor','phase','inputReady'])
        assert.ok(Number.isSafeInteger(a.state.fields?.[field])&&a.state.fields[field]>=0,`Missing native audio state field ${field}`);
    }
    if(adapter.replay){
      const r=adapter.replay;
      assert.equal(r.schema,'eagler-touhou/replay-title-semantics/1','Unknown Replay title semantics');
      assert.equal(r.game,adapter.game,'Replay semantics belong to another game');
      assert.equal(r.variant,adapter.variant,'Replay semantics belong to another variant');
      assert.equal(typeof adapter.parseReplayFrames,'function','Replay needs an independent title-owned file parser');
      assert.ok(r.format?.id?.trim()&&r.format.source?.trim(),'Replay format needs its identity and production source');
      for(const key of ['recording','playback','isolation'])assert.ok(r[key]?.source?.trim(),`Replay ${key} needs its production source`);
      if(r.pointerExtension)assert.equal(typeof adapter.parseReplayExtension,'function','Replay pointer extension needs an independent parser');
      const t=r.nativeTrace;
      assert.ok(t&&Number.isSafeInteger(t.schema)&&t.schema>0&&Number.isSafeInteger(t.wordCount)&&t.wordCount>0,
        'Replay trace needs its explicit schema and word count');
      for(const key of ['controlExport','readExport'])name(t[key],`replay.nativeTrace.${key}`);
      assert.ok(t.source?.trim(),'Replay trace needs its production source');
      for(const [key,value] of Object.entries(t.statusFields??{}))assert.ok(Number.isSafeInteger(value)&&value>=0,`Replay trace status field ${key} is invalid`);
      for(const [key,value] of Object.entries(t.sample??{}))assert.ok(Number.isSafeInteger(value)&&value>=0&&value<t.wordCount,`Replay trace sample field ${key} is invalid`);
    }else assert.ok(parseReplayFrames===undefined&&parseReplayExtension===undefined,'Replay parsers require explicit title semantics');
    if(adapter.practice){
      const p=adapter.practice;
      if(p.bossEffects){const b=p.bossEffects;assert.ok(b.source?.trim());assert.equal(typeof b.snapshot?.decode,'function');
        assert.ok(Number.isInteger(b.stage)&&Number.isInteger(b.slot)&&b.slot>=0&&b.slot<2);
        for(const field of ['protectedMask','vulnerableMask','unlockedMask'])assert.ok(Number.isInteger(b[field])&&b[field]>=0);
        assert.ok(new Set([b.protectedMask,b.vulnerableMask,b.unlockedMask]).size===3);
        for(const key of ['playerGuard','enemyGuard','timeLock','fire'])assert.ok(typeof b.keys?.[key]==='string');
      }
      if(p.options){const o=p.options;name(o.snapshot?.export,'practice.options.snapshot');assert.equal(o.snapshot.wordCount,1024);assert.equal(o.snapshot.wordType,'f64');assert.equal(o.snapshot.app,true);
        assert.ok(o.source?.trim()&&o.width>0&&o.height>0);assert.ok(Number.isInteger(o.consumerLatencyTicks)&&o.consumerLatencyTicks>=0&&o.consumerLatencyTicks<=8);
        if(o.lifecycle){const l=o.lifecycle;assert.ok(l.source?.trim());
          for(const value of Object.values(l.fields))assert.ok(Number.isInteger(value)&&value>=16&&value<128);
          assert.ok(l.cases.length>0&&l.cases.every(c=>['fast-retry','one-key-death'].includes(c.kind)));}
        for(const key of ['inGame','frame','sequence','current','life'])assert.ok(Number.isInteger(o.consumerFields?.[key])&&o.consumerFields[key]>=16&&o.consumerFields[key]<128);
        assert.ok(o.filters?.length>0&&new Set(o.filters.map(row=>row.id)).size===o.filters.length);
        for(const row of o.filters){assert.ok(Number.isInteger(row.semantic)&&row.semantic>0&&row.semantic<64);assert.ok(Number.isInteger(row.stateWord)&&row.stateWord>=16&&row.stateWord<128);
          assert.ok(Number.isInteger(row.vk)&&row.vk>=0&&row.vk<=255);assert.ok(Number.isInteger(row.mask)&&row.mask>0);}
        if(o.keyMonitor){const k=o.keyMonitor;assert.ok(k.source?.trim());assert.equal(k.window,60);assert.equal(k.inputMasks.length,10);
          for(const value of Object.values(k.fields))assert.ok(Number.isInteger(value)&&value>=16&&value<128);
          for(const id of [k.toggle,k.record,k.download])assert.ok(Number.isInteger(id)&&id>0&&id<64);
          assert.ok(k.pulses.length>0&&k.pulses.every(p=>p.keys.length>0&&Number.isInteger(p.ticks)&&p.ticks>=4&&Number.isInteger(p.release)&&p.release>=3));
        }
      }
      if(p.trainer){
        const t=p.trainer;assert.ok(t.source?.length&&t.cases?.length&&t.cases.length<=32,'Trainer cases need bounded source-owned consumers');
        assert.equal(new Set(t.cases.map(row=>row.id)).size,t.cases.length,'Trainer cases need unique IDs');
        for(const key of ['snapshot','tracker']){const s=t[key];name(s?.export,`practice.trainer.${key}`);
          assert.ok(Number.isSafeInteger(s.wordCount)&&s.wordCount>0&&s.wordCount<=4096);assert.ok(['f64','u32','i32'].includes(s.wordType));assert.equal(s.app,true);}
        assert.equal(typeof t.snapshot.decode,'function');
        for(const row of t.cases){assert.match(row.id,/^[a-z][a-z0-9-]*$/,'Trainer case ID must be portable');assert.ok(Array.isArray(row.keys)&&row.keys.every(key=>typeof key==='string'));
          assert.ok(Number.isSafeInteger(row.mask)&&row.mask>=0);assert.ok(['hit','bomb'].includes(row.stimulus));
          assert.ok(['miss','invincible','bomb','hit-bomb','late-hit-bomb','auto-bomb'].includes(row.kind));}
      }
      if(p.uiSnapshot?.keyboard!==undefined)assert.equal(p.uiSnapshot.keyboard,'vk256/1','Unknown native Practice UI keyboard contract');
      if(p.localeReview){
        const review=p.localeReview;
        assert.ok(Array.isArray(review.locales)&&review.locales.length>0&&review.locales.every(value=>typeof value==='string'&&value.trim())&&
          new Set(review.locales).size===review.locales.length,'Practice review needs unique locales');
        assert.ok(typeof review.font==='string'&&review.font.startsWith('/shared/')&&review.source?.trim(),
          'Practice review needs the actual shared font and native locale source');
      }
      assert.equal(p.schema,'eagler-touhou/practice-title/1','Unknown title practice contract');
      assert.ok(p.source?.trim(),'Practice capability needs a production source anchor');
      assert.ok(p.parameterCase&&typeof p.parameterCase==='object'&&!Array.isArray(p.parameterCase),
        'Practice title needs an independent parameter case');
      assert.ok(p.selectionCase&&typeof p.selectionCase==='object'&&!Array.isArray(p.selectionCase)&&p.selectionCase.source?.trim(),
        'Practice title needs independently sourced menu selections');
      for(const [key,value] of Object.entries(p.selectionCase))if(key!=='source')
        assert.ok(value!==null&&['boolean','string','number'].includes(typeof value)&&
          (typeof value!=='number'||Number.isFinite(value)),`Practice selection ${key} must be a normalized scalar`);
      for(const key of ['snapshot','initialSnapshot']){
        const snapshot=p[key];
        assert.ok(snapshot&&Number.isSafeInteger(snapshot.wordCount)&&snapshot.wordCount>0,
          `Practice ${key} needs its ABI word count`);
        name(snapshot.export,`practice.${key}.export`);
        assert.ok(['f64','u32','i32'].includes(snapshot.wordType),`Practice ${key} has an unsupported ABI word type`);
        assert.equal(typeof snapshot.app,'boolean',`Practice ${key} must declare whether its export takes the Runtime app argument`);
        assert.equal(typeof snapshot.decode,'function',`Practice ${key} needs its title-owned decoder`);
      }
      for(const key of ['enterMenu','exerciseControls','assessConsumption','featureInventory'])
        assert.equal(typeof p[key],'function',`Practice capability needs ${key}`);
      assert.ok(p.replayRoundTrip&&typeof p.replayRoundTrip==='object'&&!Array.isArray(p.replayRoundTrip),
        'Practice Replay roundtrip must be declared as supported or explicitly blocked');
      if(p.replayRoundTrip?.supported===true){
        const r=p.replayRoundTrip;
        assert.ok(r.format?.trim()&&r.savePath?.trim(),'Practice Replay support needs a format identity and production save path');
        assert.ok(r.selectionCase&&Number.isSafeInteger(r.selectionCase.difficulty)&&
          Number.isSafeInteger(r.selectionCase.character),'Practice Replay needs independent title-selection expectations');
        assert.equal(typeof r.finalizer,'function','Practice Replay support needs its production finalizer');
        for(const key of ['arm','wait','restore'])assert.equal(typeof r.sync?.[key],'function',`Practice Replay support needs its IDBFS ${key} observer`);
        for(const key of ['parse','inputExpectation','importPath','isUserImportPath','recordingFrame','recordingStage','inputCapacityFrames','enterMenu','assess'])
          assert.equal(typeof r[key],'function',`Practice Replay support needs its ${key} title codec`);
        assert.ok(r.freshConfig&&typeof r.freshConfig==='object','Practice Replay support needs the source-owned fresh defaults');
      }else if(p.replayRoundTrip!==undefined){
        assert.equal(p.replayRoundTrip.supported,false,'Optional practice Replay support must be explicitly disabled or complete');
        assert.ok(p.replayRoundTrip.reason?.trim(),'Disabled practice Replay support needs a reason');
      }
    }
    const key=`${adapter.game}/${adapter.variant}`;
    assert.ok(!entries.has(key),`Duplicate adaptation adapter ${key}`);
    entries.set(key,freeze(adapter));
  }
  return Object.freeze({
    products:freeze([...entries.values()].map(({game,variant})=>({game,variant}))),
    resolve(game,variant){
      assert.ok(['normal','multiplayer'].includes(variant),'Select normal or multiplayer explicitly');
      return entries.get(`${game}/${variant}`)??null;
    },
  });
}

export const ADAPTATION_ADAPTERS=createAdapterRegistry([...th08,...th10]);
export const resolveAdaptationAdapter=(game,variant)=>ADAPTATION_ADAPTERS.resolve(game,variant);
export const resolvePracticeAdapter=(game,variant)=>ADAPTATION_ADAPTERS.resolve(game,variant)?.practice??null;
