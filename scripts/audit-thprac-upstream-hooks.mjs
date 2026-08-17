import fs from 'node:fs';
import path from 'node:path';

const repo = process.cwd();
const root = path.resolve(repo, '..');
const upstreamPath = path.join(root, 'thprac-reallyportable', 'thprac', 'src', 'thprac', 'thprac_th06.cpp');
const read = (p) => fs.readFileSync(path.join(repo, p), 'utf8');
const upstream = fs.readFileSync(upstreamPath, 'utf8');

const sources = {
  practice: read('src/PracticeRuntime.cpp'),
  practiceH: read('src/PracticeRuntime.hpp'),
  mainMenu: read('src/MainMenu.cpp'),
  ascii: read('src/AsciiManager.cpp'),
  game: read('src/GameManager.cpp'),
  player: read('src/Player.cpp'),
  bullet: read('src/BulletManager.cpp'),
  enemy: read('src/EnemyManager.cpp'),
  ecl: read('src/EnemyEclInstr.cpp'),
  gui: read('src/Gui.cpp'),
  replay: read('src/ReplayManager.cpp'),
  result: read('src/ResultScreen.cpp'),
  screen: read('src/ScreenEffect.cpp'),
  window: read('src/GameWindow.cpp'),
  imgui: read('src/ThpracImGui.cpp'),
  gameWindow: read('src/GameWindow.cpp'),
  main: read('src/main.cpp'),
};

function has(source, needle, label) {
  if (!source.includes(needle)) throw new Error(`${label}: missing ${JSON.stringify(needle)}`);
}

const hookNames = [...upstream.matchAll(/(?:EHOOK|PATCH)_(?:ST|DY)\((th06_[A-Za-z0-9_]+)/g)].map((m) => m[1]);
const uniqueHooks = [...new Set(hookNames)];

// Every upstream hook/patch must be named here. The point of this audit is to
// make omissions impossible to hide behind a successful end-to-end run.
const classifications = new Map([
  ['th06_result_screen_create', 'core-equivalent'],
  ['th06_sfx_fix', 'core-equivalent'],
  ['th06_bomb_esc_r_prevent_desyncs', 'core-equivalent'],
  ['th06_white_screen', 'architectural-na'],
  ['th06_enter_game', 'overlay-core'],
  ['th06_track_miss', 'overlay-core'],
  ['th06_reacquire_input', 'architectural-na'],
  ['th06_activateapp', 'architectural-na'],
  ['th06_bgm_play', 'overlay-core'],
  ['th06_bgm_stop', 'overlay-core'],
  ['th06_prac_menu_1', 'core-equivalent'],
  ['th06_prac_menu_3', 'core-equivalent'],
  ['th06_prac_menu_4', 'core-equivalent'],
  ['th06_prac_menu_enter', 'core-equivalent'],
  ['th06_pause_menu', 'core-equivalent'],
  ['th06_unpause_prevent_desyncs', 'core-equivalent'],
  ['th06_patch_main', 'core-equivalent'],
  ['th06_restart', 'core-equivalent'],
  ['th06_title', 'core-equivalent'],
  ['th06_preplay_1', 'core-equivalent'],
  ['th06_preplay_2', 'core-equivalent'],
  ['th06_save_replay', 'core-equivalent'],
  ['th06_rep_menu_1', 'core-equivalent'],
  ['th06_rep_menu_2', 'core-equivalent'],
  ['th06_rep_menu_3', 'core-equivalent'],
  ['th06_fake_shot_type', 'core-equivalent'],
  ['th06_patchouli', 'core-equivalent'],
  ['th06_cancel_muteki', 'core-equivalent'],
  ['th06_set_deathbomb_timer', 'core-equivalent'],
  ['th06_hamon_rage', 'core-equivalent'],
  ['th06_disable_menu', 'core-equivalent'],
  ['th06_update', 'backend-equivalent'],
  ['th06_render', 'backend-equivalent'],
  ['th06_gui_init_1', 'backend-equivalent'],
  ['th06_gui_init_2', 'backend-equivalent'],
]);

const missing = uniqueHooks.filter((name) => !classifications.has(name));
const stale = [...classifications.keys()].filter((name) => !uniqueHooks.includes(name));
if (missing.length || stale.length) {
  throw new Error(`hook classification incomplete: missing=[${missing}] stale=[${stale}]`);
}

// Newly source-audited dynamic semantics outside THSectionPatch.
has(sources.practice, 'EffectivePlayerShot', 'fake-shot helper');
has(sources.ecl, 'PracticeRuntime::EffectivePlayerShot(g_GameManager.CharacterShotType())', 'ECL PLAYER_SHOT fake-shot');
has(sources.ecl, 'PracticeRuntime::ForceFlandreFinalRage()', 'QED phase-1 rage');
has(sources.player, 'PracticeRuntime::AdvancedActive() ? PLAYER_STATE_ALIVE : PLAYER_STATE_SPAWNING', 'cancel_muteki');
has(sources.player, 'PracticeRuntime::AdvancedActive() ? 6 : 8', 'deathbomb timer');
has(sources.practice, 'case 34: // TH06_ST5_BOSS2', 'boss-section SFX table');
has(sources.bullet, 'PracticeRuntime::ApplyPendingBossSectionSfxFix();', 'SFX one-shot call site');
has(sources.game, 'songPaths[PracticeRuntime::InitialBgmIndex()]', 'THBGMTest initial BGM');
has(sources.practice, 'g_CurFrameInput &= ~TH_BUTTON_BOMB;', 'unpause bomb clear');
has(sources.practice, 'g_CurFrameInput &= ~TH_BUTTON_SHOOT;', 'dialogue unpause shoot clear');
has(sources.ascii, 'PracticeRuntime::FilterUnpauseInput();', 'vanilla pause unpause hook');
has(sources.screen, 'g_CancelNextShakeForRestart', 'restart shake one-shot');
has(sources.practice, 'ScreenEffect::RequestShakeCancelForRestart();', 'restart shake request');
has(sources.practice, 'g_PauseVisualState == MenuVisualState::Closed', 'THPauseMenu STATE_CLOSE delay');
has(sources.practice, 'g_PauseFrameCounter > 5', 'THPauseMenu >5-frame pre-open gate');
has(sources.practice, 'if (g_Config.section != 0)', 'Replay conditional section serialization');
has(sources.practice, 'if (g_Config.phase != 0)', 'Replay conditional phase serialization');
has(sources.practice, 'if (g_Config.frame != 0)', 'Replay conditional frame serialization');
has(sources.practice, 'if (g_Config.dialogue)', 'Replay conditional dlg serialization');

// th06_white_screen patches DrawFadeIn only while upstream routes Restart back
// through MainMenu::BeginStartup. The portable Restart goes directly to
// GAMEMANAGER_REINIT, and every portable fade-in registration lives in
// MainMenu startup. Therefore no corresponding fade-in is created on the
// portable restart path; this hook is architectural N/A rather than omitted.
has(upstream, 'PATCH_ST(th06_white_screen, 0x42fee0, "c3")', 'upstream white-screen patch');
has(sources.practice, 'SUPERVISOR_STATE_GAMEMANAGER_REINIT', 'portable direct restart');
const srcDir = path.join(repo, 'src');
const cppFiles = fs.readdirSync(srcDir).filter((name) => name.endsWith('.cpp'));
const fadeRegistrations = [];
for (const name of cppFiles) {
  const text = fs.readFileSync(path.join(srcDir, name), 'utf8');
  if (text.includes('RegisterChain(SCREEN_EFFECT_FADE_IN')) fadeRegistrations.push(name);
}
if (fadeRegistrations.length !== 1 || fadeRegistrations[0] !== 'MainMenu.cpp') {
  throw new Error(`white-screen N/A assumption changed: fade registrations=${fadeRegistrations.join(',')}`);
}

// Replay State(1/2/3) is integrated at actual run start: normal Replay first
// clears live practice state, then metadata (if present) is loaded before
// PrepareStart/ApplyInitialState. No separate visible THGuiRep window exists.
has(sources.game, 'PracticeRuntime::RefreshFromHost();', 'Replay State(1) reset equivalent');
has(sources.game, 'PracticeRuntime::LoadReplayMetadata', 'Replay State(2/3) load equivalent');
has(sources.practice, 'g_Config = {};', 'Replay metadata reset');
has(sources.practice, 'if (!g_GameManager.isInPracticeMode)', 'ordinary Start live-thprac ownership boundary');
has(sources.practice, 'Module.eaglerOptions.thpracSession = null;', 'ordinary Start clears Web live thprac session');
if (sources.practice.includes('g_MenuConfig = {};')) {
  throw new Error('ordinary Start must preserve TH06 remembered THGuiPrac widget state');
}

// Practice menu replacement must suppress vanilla stage-list drawing without
// binary-NOP patching.
has(sources.mainMenu, 'PracticeRuntime::DrawPracticeMenu();', 'disable vanilla practice menu equivalent');
has(sources.mainMenu, 'return ZUN_SUCCESS;', 'practice draw early return');

// Backend-only hooks are represented by the SDL/ImGui frame bridge rather than
// Win32 DirectInput/DX8 detours.
has(sources.window, 'ThpracImGui::BeginFrame', 'th06_update backend equivalent');
has(sources.window, 'RenderImGui', 'th06_render backend equivalent');

// THGuiPrac data tables are part of the trainer contract even though they do
// not have individual th06_* hook names.
has(upstream, 'int mChapterSetup[7][2] {\r\n            { 4, 2 },\r\n            { 2, 2 },\r\n            { 4, 3 },\r\n            { 4, 5 },\r\n            { 3, 2 },\r\n            { 2, 0 },\r\n            { 4, 3 }', 'upstream chapter setup');
has(sources.practice, '{4, 2}, {2, 2}, {4, 3}, {4, 5}, {3, 2}, {2, 0}, {4, 3}', 'portable chapter setup');

const warpRows = [
  [68, 580, 1160, 1540, 2348, 4438, 0, 0, 0],
  [270, 924, 3528, 4563, 0, 0, 0, 0, 0],
  [340, 1050, 1670, 2762, 3807, 4118, 5274, 0, 0],
  [380, 1454, 2328, 0x0d40, 4872, 5712, 7434, 8354, 9784],
  [350, 1352, 2292, 3814, 6774, 0, 0, 0, 0],
  [380, 1484, 0, 0, 0, 0, 0, 0, 0],
  [380, 1300, 2600, 3680, 4803, 5933, 7733, 0, 0],
];
for (const row of warpRows) {
  const rendered = `{${row.map((n) => n === 0x0d40 ? '0x0d40' : n).join(', ')}}`;
  has(sources.practice, rendered, `portable THStageWarp row ${rendered}`);
}

// THOverlay is a separate Backspace "Mod Menu" living beside THGuiPrac, but
// it is still shipped user-facing thprac functionality.  F1-F7 and Tracker
// are therefore mandatory trainer surface, never an optional/audit-only set.
const overlayFeatures = [
  ['mMuteki', 'F1'],
  ['mInfLives', 'F2'],
  ['mInfBombs', 'F3'],
  ['mInfPower', 'F4'],
  ['mTimeLock', 'F5'],
  ['mAutoBomb', 'F6'],
  ['mElBgm', 'F7'],
  ['THTrackerUpdate()', 'Tracker'],
];
for (const [needle, label] of overlayFeatures) has(upstream, needle, `THOverlay ${label}`);

// Mandatory portable THOverlay contract.  Checking only the upstream names is
// insufficient: every checkbox/hotkey must have a typed gameplay consumer.
has(sources.practice, 'SDL_SCANCODE_BACKSPACE', 'THOverlay Backspace hotkey');
has(sources.practice, 'SDL_SCANCODE_F1', 'THOverlay F1 hotkey');
has(sources.practice, 'SDL_SCANCODE_F2', 'THOverlay F2 hotkey');
has(sources.practice, 'SDL_SCANCODE_F3', 'THOverlay F3 hotkey');
has(sources.practice, 'SDL_SCANCODE_F4', 'THOverlay F4 hotkey');
has(sources.practice, 'SDL_SCANCODE_F5', 'THOverlay F5 hotkey');
has(sources.practice, 'SDL_SCANCODE_F6', 'THOverlay F6 hotkey');
has(sources.practice, 'SDL_SCANCODE_F7', 'THOverlay F7 hotkey');
has(sources.practice, 'SDL_SCANCODE_TAB', 'Tracker default Tab hotkey');
has(sources.gameWindow, 'PracticeRuntime::UpdateOverlay();', 'THOverlay 60 Hz update bridge');
has(sources.gameWindow, 'PracticeRuntime::DrawOverlay();', 'THOverlay ImGui draw bridge');

has(sources.player, 'PracticeRuntime::OverlayInvincible()', 'F1 invincible death-state consumer');
has(sources.player, 'PracticeRuntime::OverlayInfiniteLives()', 'F2 infinite-lives consumer');
has(sources.player, 'PracticeRuntime::OverlayInfiniteBombs()', 'F3 infinite-bombs consumer');
has(sources.player, 'PracticeRuntime::OverlayInfinitePower()', 'F4 infinite-power consumer');
has(sources.enemy, 'PracticeRuntime::OverlayTimeLock()', 'F5 time-lock consumer');
has(sources.enemy, 'midStart[5] = {2008, 2588, 0, 4132, 3374}', 'F5 upstream midboss start table');
has(sources.enemy, 'midLength[5] = {(24 + 24) * 60, 32 * 60, 0, 40 * 60, (40 + 30) * 60}', 'F5 upstream midboss length table');
has(sources.enemy, 'midExtraWait[5] = {4 * 60, 15 * 60, 0, 12 * 60, 5 * 60}', 'F5 upstream midboss wait table');
has(sources.player, 'PracticeRuntime::OverlayAutoBomb()', 'F6 auto-bomb consumer');

has(sources.practice, 'CommitRestartWithBgmPolicy()', 'F7 restart decision');
has(sources.practice, 'OverlayEverlastingBgm() && !g_BgmChangedSinceStart && sameBgmIdentity', 'F7 exact identity gate');
has(sources.game, '!PracticeRuntime::PreserveBgmOnRestart()', 'F7 stop/play lifecycle gate');
has(sources.gui, 'PracticeRuntime::NotifyBgmPlay(path);', 'F7 in-run BGM-change tracking');

has(sources.game, 'PracticeRuntime::ResetTracker();', 'Tracker run reset');
has(sources.player, 'PracticeRuntime::RecordTrackerMiss();', 'Tracker deathbomb-safe miss count');
has(sources.practice, 'g_GameManager.bombsUsed', 'Tracker upstream bomb-count source');
has(sources.practice, 'ImGui::Begin("Tracker###thprac-tracker"', 'Tracker window');

// F5 is reserved by thprac in thprac-enabled builds. The developer-only speed
// shortcut may exist only when TH_ENABLE_THPRAC is absent.
has(sources.main, '#if defined(TH_DEV_TOOLS) && !defined(TH_ENABLE_THPRAC)', 'F5 thprac/dev ownership split');

const counts = {};
for (const kind of classifications.values()) counts[kind] = (counts[kind] ?? 0) + 1;
console.log(`TH06 upstream thprac hook audit PASS: ${uniqueHooks.length}/${uniqueHooks.length} hooks classified; ` +
  Object.entries(counts).map(([k, v]) => `${k}=${v}`).join(' ') +
  `; mandatory THOverlay surface enumerated=${overlayFeatures.length}`);
