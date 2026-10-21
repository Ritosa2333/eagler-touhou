import {defineStateAuthority} from '../../../../common/testkit/presentation-lab/state-authority.mjs';

// TH08 owns these semantics. The common lab consumes normalized labels and
// policy evidence; numeric IDs and the state group order are the native ABI.
export const OWNERS=['未标记','标题菜单','菜单说明','暂停/重试','重试','敌弹','激光','特效','敌人/使魔','道具','HUD','3D背景','符卡UI','自机','自机弹','ASCII','自机Bomb','自机Option','ASCII文本','分数弹字','时间弹字','画面特效'];

// One digest per explicit source domain, in audit_state() order.
export const STATE_GROUPS=[
  'determinism-and-game-values','player-and-player-input','bullet-pool','laser-pool',
  'enemy-execution-and-validation-gate','effect-pool','item-pool','player-shots-and-bombs',
  'title-and-scene-control','background-script-and-camera','screen-effects',
  'replay-consumption','save-write-observation','synchronous-audio-operations',
  'host-frame-cadence','host-keyboard-input','hud-ascii-dialogue-and-spell',
];

// These names describe the fields hashed by the native audit, not whole types.
export const STATE_FIELD_MAP=Object.freeze({
  'determinism-and-game-values':['GameplaySession::random.{seed,backup,calls}','GameGlobals::{display_score,graze_stage,score,score_increment,high_score,high_score_retries,captured_spells,gauge_copy,gauge,point_value,clock_time,retries,points_stage,points,point_extends,next_point_extend,time_orbs,last_spell_requirement,total_time_orbs,rng1..rng8,deaths,lives,bombs,power,integrity_value,integrity_checksum}','GameplaySession::{config,display_config,rank,thresholds,history,records[0..221],previous_records[0..221],clears[0..12],practices[0..11],statistics,last_words,last_name,stall_frames,stage_copy,replay_seed,total_clock,values.expected_checksum}','PracticeState::{mode flags,configured,run,replay_candidate,cheats,assisted,THPrac::PracticeInput,PracticeKeyMonitor vectors/deque,PracticeSpeed,tracker counters}','TitleContext::{save/history copies,flags,replayFilename,selection fields}'],
  'player-and-player-input':['PlayerSimulationState.motion movement/history/form/options/gauge','PlayerLifeState state/timer/death counters','PlayerBombState active/type/timer/cooldown/sequence','PlayerMotionInput bounds/buttons/character/GUI flags','PlayerLifeContext game/HUD flags and counters'],
  'bullet-pool':['BulletManagerState active_count/cancel_frames/timer/unknown_counter','each BulletState state/position/velocity/speed/angle/change parameters/timers/flags/extra state'],
  'laser-pool':['each LaserState in_use/state/position/angle/geometry/timer/flags','both laser ANM state snapshots'],
  'enemy-execution-and-validation-gate':['EnemySimulationState timers/frame counters/timeline cursors','each active EclVm main and asynchronous contexts, relative bytecode offsets, locals, call frames, generations, emitters, motion/health/ANM state and pool relationships','ValidationSceneGate enabled/withheld_updates and BrowserRuntime validation_scene_used latch','EclGlobals stage/flags/events, script scratch arrays, service bindings and active script snapshots'],
  'effect-pool':['each active EffectState ANM/spatial/geometry/age/lifecycle fields','EffectPoolState active_count/frames'],
  'item-pool':['ItemPoolState next_index/count','each live linked ItemState position/velocity/target/timer/type/ANM state'],
  'player-shots-and-bombs':['all PlayerShot active state/position/velocity/timer/kind/ANM state','three PlayerLaserSlot timers','all active PlayerBombObject state/position/history/timer/ANM state'],
  'title-and-scene-control':['Supervisor state/target','TitleContext screen/menu/input selection','GameplayControlState frames/load/replay/stage fields','scene/gameplay flags and logical frame counters'],
  'background-script-and-camera':['BackgroundState script/timer/fog/tint/effect fields and camera interpolation endpoints','SceneCamera authored coordinates'],
  'screen-effects':['GameplayScene.screen and GameApplication.screen active ScreenEffectState/context fingerprints'],
  'replay-consumption':['BrowserRuntime::status(12..21): selected stream, frame, input/timing cursors and sizes, stride, flags, in-game and stage','GameplayScene::playback (ReplayPlayback)::{stage_mask,metadata,input,stream,file.decoded()[0..size)}','GameplayScene::recording (ReplayRecording)::{ready,current,metadata,input,frame_state,stages[0..8].snapshot,present,stream.input[0..size),stream.timing[0..size),input_end,timing_end,stride}'],
  'save-write-observation':['BrowserRuntime::status(22..24): write attempts, successful saves and successful bytes','BrowserRuntime::audit_saved_payloads[normalized_path]={payload_bytes,FNV1a32} for every successful Runtime user save/replay write'],
  'synchronous-audio-operations':['AudioAudit schema/total and per-operation counters; asynchronous device queue depth is excluded'],
  'host-frame-cadence':['frames/loop_epoch/running/suspended','FrameCadence.debt','PresentationCadence fast/slow/high_refresh','PresentationGate.primed','PracticeCadence debt/period'],
  'host-keyboard-input':['BrowserRuntime::keys[0..255]','ControllerSnapshot::{available,direct,x,y,legacy_buttons,x_min,x_max,y_min,y_max,buttons[0..127]} and InputController::focus_conflict plus TitleContext::controller_state[0..127]','TouchController::{menu,tap,DialogueGesture::points,directions,fingers,primary,instance,context,dragging,motion_blocked,tap/capture state,action counters,controls}','MotionTrack::{stages[0..8].Entry bytes,touch_stages[0..8].TouchEntry bytes,ticks,cursors,touch_cursors,record_points,playback_points,playback_stage,mode flags,target_x,target_y}','BrowserRuntime::{touch_sensitivity,touch_trace_callbacks,pending_touch_trace[all fields]}'],
  'hud-ascii-dialogue-and-spell':['active ASCII text and popups, HUD counters/segments, live GUI and dialogue ANM states, spell presentation values and ANM state'],
});

const domainGroups=Object.freeze({
  'simulation-world':STATE_GROUPS.slice(0,11),
  'gameplay-session':['determinism-and-game-values'],
  'practice-state':['determinism-and-game-values'],
  'replay-state':['replay-consumption'],
  'input-transients':['player-and-player-input','host-keyboard-input'],
  'user-payload-buffers':['replay-consumption','host-keyboard-input'],
  'persistent-storage':['save-write-observation'],
  'synchronous-audio':['synchronous-audio-operations'],
  'host-cadence':['host-frame-cadence'],
  'presentation-owners':['effect-pool','player-shots-and-bombs','hud-ascii-dialogue-and-spell','screen-effects'],
});
export const STATE_AUTHORITY=defineStateAuthority({groupFields:STATE_FIELD_MAP,domains:domainGroups});

// Pools whose generation is observed at their real allocation/retirement sites.
export const LIFECYCLE_TRACKED_OWNER_IDS=new Set([5,6,7,8,9,14,16,18,19,20,21]);
export const WORLD_OWNER_IDS=new Set([5,6,7,8,9,10,11,13,14,15,16,17,19,20,21]);
export const MOTION_OWNER_IDS=new Set([5,6,8,9,13,14,16,17,19,20]);
export const SOURCE_PATHS={1:'TitleSupport.cpp / TitleView.cpp',2:'TitleView.cpp',3:'UiMenus.cpp',4:'UiMenus.cpp',5:'BulletCreation.cpp / BulletUpdate.cpp / BulletDrawing.cpp',6:'LaserRuntime.cpp / BulletDrawing.cpp',7:'EffectSystem.cpp',8:'EnemyPopulation.cpp / EnemyRetirement.cpp / EnemyDeath.cpp / EnemyDrawing.cpp',9:'ItemPool.cpp',10:'GuiController.cpp / GuiView.cpp',11:'BackgroundObjects.cpp / BackgroundView.cpp',12:'SpellDrawing.cpp',13:'PlayerSimulation.cpp / PlayerMotion.cpp',14:'PlayerShots.cpp / PlayerShotUpdate.cpp / PlayerShotDraw.cpp',15:'AsciiManager.cpp',16:'PlayerBombPatterns.cpp / PlayerBomb*.cpp',17:'PlayerMotion.cpp',18:'AsciiManager.cpp',19:'AsciiManager.cpp',20:'AsciiManager.cpp',21:'ScreenEffects.cpp'};

// Owner metadata is a source map; it does not upgrade an owner to passed.
export const OWNER_REGISTRY=OWNERS.map((label,id)=>({
  id,label,source:SOURCE_PATHS[id]||null,
  identityPolicy:LIFECYCLE_TRACKED_OWNER_IDS.has(id)?'native-generation-events':'session-pointer-and-authored-discrete-state',
  reviewStatus:id?'source-mapped':'unscoped',
  authoritativeWriter:null,endpointPublisher:null,presentationConsumer:null,
  finalSubmission:null,identity:null,continuity:null,discontinuity:null,
  restoration:null,fingerprints:[],evidence:[],
}));
