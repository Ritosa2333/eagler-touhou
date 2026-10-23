import assert from 'node:assert/strict';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {validateTitleFlow} from './flow-steps.mjs';
import th08 from './titles/th08.mjs';
import th10 from './titles/th10.mjs';

const SCHEMA='eagler-touhou/adaptation-adapter/1';
const identifier=/^[A-Za-z_$][\w$]*$/;
const capabilities=new Set(['runtime','observation','flow','clock','movement','actions','doubleTapBomb','deathbomb','runBoundaries','dialogueTouchScene','originalMenu','controlledScene','audioLatency','deriveSfxContract','language','classifyLanguageDiagnostic','replay','parseReplayFrames','parseReplayMetadata','parseReplayExtension','practice']);
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
    const {deriveSfxContract,classifyLanguageDiagnostic,parseReplayFrames,parseReplayMetadata,parseReplayExtension,practice,...declaration}=original;
    const adapter=structuredClone(declaration);
    for(const [key,value] of Object.entries({deriveSfxContract,classifyLanguageDiagnostic,parseReplayFrames,parseReplayMetadata,parseReplayExtension})){
      if(value===undefined)continue;
      assert.equal(typeof value,'function',`${key} must be a title-owned derivation function`);
      adapter[key]=value;
    }
    if(practice!==undefined){
      assert.equal(typeof practice,'object','Practice capability must be a title-owned declaration');
      adapter.practice=cloneWithFunctions(practice);
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
