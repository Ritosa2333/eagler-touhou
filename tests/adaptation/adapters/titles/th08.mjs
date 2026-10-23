import {TH08_REPLAY_SEMANTICS,inspectTh08ReplayInputFrames,inspectTh08ReplayMetadata,parseTh08MotionTrailer} from './th08-replay-format.mjs';
import {TH08_PRACTICE_ADAPTER} from './th08-practice.mjs';
import {classifyTh08LanguageDiagnostic} from './th08-language.mjs';
import {TH08_DIALOGUE_SCENE_CONTRACT} from './th08-dialogue.mjs';
import {TH08_MENU_CONTRACT} from './th08-menu.mjs';

/** Title-owned meanings. Operations, assertions and evidence belong to shared drivers. */
const declarations=[{
  schema:'eagler-touhou/adaptation-adapter/1',game:'th08',variant:'normal',
  runtime:{global:'__th08Runtime',core:'core',module:'Module',app:'app'},
  replay:TH08_REPLAY_SEMANTICS,
  practice:TH08_PRACTICE_ADAPTER,
  parseReplayFrames:inspectTh08ReplayInputFrames,
  parseReplayMetadata:inspectTh08ReplayMetadata,
  parseReplayExtension:parseTh08MotionTrailer,
  dialogueTouchScene:TH08_DIALOGUE_SCENE_CONTRACT,
  originalMenu:TH08_MENU_CONTRACT,
  observation:{kind:'legacy-th08-status-trace',sampleBoundary:'after-application-update',
    inputSource:'diagnostics[11] | player_state.input.buttons',inputSemantics:'queued-for-next-movement'},
  flow:{strategy:'confirm',needsControlWarmup:true},
  clock:{tickApp:true,startApp:false,stopExport:'sdl_loop_stop',tickExport:'sdl_loop_tick',
    startExport:'sdl_loop_start',pauseExport:'sdl_loop_pause',
    source:'ports/th08/web/cpp/sdl/GameHost.cpp | sdl_loop_tick'},
  movement:{
    inputBoundary:'queued-for-next-movement',
    inputBoundarySource:'ports/th08/web/cpp/game/GameplayScene.cpp | publish_input | PlayerMotion.cpp | update_player_motion',
    precision:{unitsPerPixel:1,rounding:'float',scalePlacement:'before-limit'},
    source:'ports/th08/web/cpp/game/PlayerMovement.cpp | update',
    keyboardReleasePolicy:{
      semantics:'queued-for-next-movement',
      observationSource:'diagnostics[11] | player_state.input.buttons',
      observationPhase:'after-application-update',directionMasks:{ArrowRight:128,ArrowLeft:64},
      focusMask:4,maxMovementTicks:1,
      keyboardTouchOwnership:{semantics:'direction-key-clears-touch-motion',source:[
        'ports/th08/web/cpp/sdl/GameHost.cpp | poll | touch.sample arrows',
        'ports/th08/portable/input/TouchController.hpp | TouchController::sample | arrows => clear_motion',
      ]},
      touchFocusTransitions:{semantics:'action-queue-then-next-movement',maxMovementTicks:1},
    },
  },
  actions:{schema:'eagler-touhou/stg-actions/1',stateExport:'touch_action_state_v1',wordCount:20,stateApp:true,
    sampleBoundary:'after-player-consumer',inputMasks:{fire:1,focus:4,bomb:2,escape:8},
    escapePulseTicks:3,escapePulseSource:'ports/th08/portable/input/TouchController.hpp | controls and sample | escape_ticks',
    source:['ports/th08/web/cpp/game/PlayerSimulation.cpp | PlayerSimulation::update actual player-frame consumer',
      'ports/th08/web/cpp/game/GameplayScene.cpp | completed player callback snapshot',
      'ports/th08/web/cpp/platform/BrowserExports.cpp | touch_action_state_v1'],
  },
  doubleTapBomb:{firstTapMaxMs:220,interTapMaxMs:320,bombPulseTicks:3,
    source:'ports/th08/portable/input/TouchController.hpp | pointer tap arming and sample bomb_ticks'},
  deathbomb:{schema:'eagler-touhou/stg-deathbomb/1',stateExport:'validation_deathbomb_state_v1',
    hitExport:'validation_deathbomb_hit_v1',wordCount:36,
    lifeStates:{alive:0,deathWindow:2,bombRecovery:3,respawning:1,movementBlocked:[1,2],movementAllowed:[0,3]},
    respawnPositionRule:'initial-player-position',respawnPositionSource:'ports/th08/web/cpp/game/PlayerSetup.cpp and PlayerLife.cpp | extent.x/2, extent.y-64',
    deathbombTypes:[2,3],bombCost:2,pulseTicks:3,
    retiringProjectileStates:[5],projectileRetirementSource:'ports/th08/web/cpp/game/BulletUpdate.cpp | damage collision enters state5 | animation completion retires the slot',
    consumerLatencyTicks:1,consumerLatencySource:'ports/th08/web/cpp/game/GameplayScene.cpp | prepare_frame publishes preceding recording input | player priority9 | recording priority17',
    geometry:{pointerId:9467,start:{x:.5,y:.75},nudge:{x:.52,y:.75},blockedMove:{x:.53,y:.75},recoveryMove:{x:.54,y:.75}},
    source:'ports/th08/web/cpp/game/BulletSystem.cpp | tagged fixture projectile through production collision'},
  runBoundaries:{stateExport:'validation_run_state_v1',armExport:'validation_run_arm_v1',
    transitionExport:'validation_run_transition_v1',gameScene:2,resultScene:6,nonNormalFlags:0x400b,
    touchGeometry:{width:640,height:480,start:{x:.5,y:.75},probeDelta:{x:12,y:4}},
    sources:{runtimeIdentity:'BrowserContext isolated storage | fixture origin | actual Runtime epoch',
      runId:'ports/th08/web/cpp/platform/BrowserRuntime.hpp | begin_motion ordinary initial run sequence',
      runMarker:'ports/th08/web/cpp/platform/MotionTrack.hpp | cheat_movement_used',
      touchTrace:'ports/th08/web/cpp/platform/BrowserRuntime.cpp | completed forward touch trace at a stopped native tick',
      stageTransition:'ports/th08/web/cpp/game/SupervisorFrame.cpp | Scene::NextStage',
      resultTransition:'ports/th08/web/cpp/game/SupervisorFrame.cpp | Scene::GameResults',
      resultLag:'ports/th08/web/cpp/game/ResultKeyboard.cpp | state.hscr.lag',
      replayFinalizer:TH08_REPLAY_SEMANTICS.finalizer.source,
      replayParser:'ReplayMetadata::lag and ReplayMetadata::practice from the independently decoded v6 header'}},
  controlledScene:{
    schema:'eagler-touhou/controlled-input-scene/3',
    armExport:'validation_scene_arm',stateExport:'validation_scene_state',armStateExport:'validation_scene_arm_state',
    writeStateExport:'validation_scene_write_state_v1',
    writePolicy:{initialization:'ram-score-overlay',source:'ports/th08/web/cpp/game/GameplayStart.cpp | initialize_score calls save_score during GameplayScene::load'},
    source:'ports/th08/web/cpp/game/GameApplication.cpp | consume request before GameplayScene::load, then accept gate before the first EnemySimulation update',
  },
  deriveSfxContract:deriveTh08NormalSfxContract,
  language:{schema:'eagler-touhou/language-scenes/1',enableExport:'language_trace_enable_v1',
    sizeExport:'language_trace_json_size_v1',copyExport:'language_trace_json_v1',allocateExport:'allocate',freeExport:'deallocate',
    table:'/thcrap/th08/localization/strings.etl',missingKey:'th07 Menu Replay',
    source:'ports/th08/web/cpp/game/Localization.cpp | JapaneseFonts.cpp | sdl/FontHost.cpp | BrowserRuntime::present',
    browserDiagnostics:{runtimeAsset:'th08-sdl.mjs',
      source:'ports/th08/web/cpp/game/Localization.cpp | TH08_LOCALIZATION_LOG | ports/th08/web/cpp/sdl/FontHost.cpp | SDL_Log'},
    samples:[
      {operation:'string-by-id',key:'th07 Menu Start',source:'ports/th08/web/cpp/game/TitleText.hpp | g_StartMenuHelpTextIds[0] | TitleMenus.cpp Start initialization'},
      {operation:'string-by-id',key:'th07 Menu Replay',source:'ports/th08/web/cpp/game/TitleText.hpp | g_StartMenuHelpTextIds[4] | TitleMenus.cpp Start initialization'},
    ],
  },
  classifyLanguageDiagnostic:classifyTh08LanguageDiagnostic,
  audioLatency:{
    game:'th08',variant:'normal',logicalIndex:10,sampleIndex:8,sample:'se_ok00.wav',handle:11,queueRate:44100,
    probeGlobal:'__th08SfxLatencyProbe',enabledGlobal:'__th08SfxLatencyProbeEnabled',traceGlobal:'__th08SfxLatencyTrace',
    targetHandles:[11],inputFilter:{type:'keydown',code:'KeyZ'},
    sourcePaths:{soundEffectsSource:'ports/th08/web/cpp/game/SoundEffects.cpp',
      audioManagerSource:'ports/th08/web/cpp/platform/GameAudioManager.cpp'},
    scenes:[
      {name:'title-menu-to-difficulty',role:'positive',sameTargetVoice:true,
        before:{activeApp:'Title',appStatus:1,page:0,cursor:0,phase:1,inputReady:1},
        input:'KeyZ',after:{page:4,phase:1}},
      {name:'difficulty-to-character-live-queue-tail',role:'negative-control',sameTargetVoice:true,
        before:{activeApp:'Title',appStatus:1,page:4,phase:1,inputReady:1},
        input:'KeyZ',after:{page:5,phase:1},sameTargetVoice:true,mutation:'measured-queue-tail'},
    ],
    state:{export:'status',inputReadySource:'ports/th08/web/cpp/game/TitleMenus.cpp | title_confirmation_ready | TitleScene::confirmation_ready',
      fields:{activeApp:0,appStatus:1,invalid:2,loading:7,page:8,cursor:9,phase:11,inputReady:25,phaseTimer:26,inputCurrent:27,inputPrevious:28},
      activeApps:{1:'Title'}},
  },
}];

export default declarations;

function fail(reason){throw new Error(`TH08 normal SFX production contract: ${reason}`);}

/** Derive the independent expectation from the actual TH08 production owners. */
export function deriveTh08NormalSfxContract({soundEffectsSource,audioManagerSource}){
  if(typeof soundEffectsSource!=='string'||typeof audioManagerSource!=='string')fail('both production source files are required');
  const definitions=soundEffectsSource.match(/const\s+SoundDefinition\s+sound_definitions\[46\]\s*=\s*\{([\s\S]*?)\};/);
  const samples=soundEffectsSource.match(/const\s+char\s*\*\s*const\s+sound_samples\[36\]\s*=\s*\{([\s\S]*?)\};/);
  if(!definitions||!samples)fail('sound definition or sample table was not found');
  const rows=[...definitions[1].matchAll(/\{\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*\}/g)];
  if(rows.length!==46)fail('sound definition table does not contain all 46 production entries');
  const logicalIndex=declarations[0].audioLatency.logicalIndex;
  const sampleIndex=Number(rows[logicalIndex][1]);
  const sampleRows=[...samples[1].matchAll(/"([^"]+)"/g)].map(row=>row[1]);
  if(sampleRows.length!==36||sampleIndex<0||sampleIndex>=sampleRows.length)fail('sound sample mapping is malformed');
  const sample=sampleRows[sampleIndex];
  const preparation=audioManagerSource.match(/bool\s+GameAudioManager::prepare_samples\s*\(\s*\)\s*\{([\s\S]*?)\n\}/);
  if(!preparation)fail('GameAudioManager::prepare_samples is missing');
  const body=preparation[1];
  const bufferAssignment=body.match(/effects\.buffers\[i\]=i\+(\d+);/);
  if(!bufferAssignment||Number(bufferAssignment[1])!==1)fail('production buffer handle i+1 must be assigned for each uploaded sample');
  const createIndex=body.indexOf('sound_device().create_pcm(i+1,');
  const assignIndex=body.indexOf('effects.buffers[i]=i+1;');
  if(createIndex<0||assignIndex<0||createIndex>=assignIndex)fail('successful create_pcm must precede the production i+1 buffer assignment');
  const guardStart=body.lastIndexOf('if(',createIndex);
  const createCall=body.slice(guardStart>=0?guardStart:createIndex,assignIndex);
  if(!/^if\s*\(\s*sound_device\(\)\.create_pcm\(i\+1,[\s\S]*\)\s*return\s+false\s*;\s*$/.test(createCall))
    fail('buffer assignment must remain conditional on successful create_pcm');
  const process=soundEffectsSource.match(/void\s+SoundEffects::process\s*\(\s*\)\s*\{([\s\S]*?)\n\}/)?.[1]??'';
  const eventObservation=process.indexOf('th08_sfx_latency_event(static_cast<u32>(index),buffer)');
  const playIndex=process.indexOf('output.play(buffer,0,0)');
  const controlsIndex=process.indexOf('output.volume(buffer,adjusted_volume(');
  const consumeIndex=process.indexOf('state.indices[i]=-1');
  if(consumeIndex<0||controlsIndex<=consumeIndex||eventObservation<=controlsIndex||playIndex<=eventObservation||
     /th08_sfx_latency_event\s*\(/.test(audioManagerSource))
    fail('logical sound event must observe the deduplicated production index-to-handle mapping after controls and before play');
  if(sampleIndex!==declarations[0].audioLatency.sampleIndex||sample!==declarations[0].audioLatency.sample)
    fail(`logical index ${logicalIndex} now maps to sample ${sampleIndex} (${sample})`);
  return {...declarations[0].audioLatency,sampleIndex,sample,handle:logicalIndex+1,
    evidence:'sound_definitions → sound_samples → GameAudioManager::prepare_samples buffer i+1 → SoundEffects::process deduplicated play observer'};
}
