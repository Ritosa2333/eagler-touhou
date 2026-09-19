import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const here = path.dirname(fileURLToPath(import.meta.url));
const project = path.resolve(here, '../..');
const workspace = path.resolve(project, '..');

function read(rel) {
    return fs.readFileSync(path.join(workspace, rel), 'utf8').replaceAll('\r\n', '\n');
}

const upstream = read('thprac-reallyportable/thprac/src/thprac/thprac_th07.cpp');
const upstreamGames = read('thprac-reallyportable/thprac/src/thprac/thprac_games.cpp');
const catalog = read('thprac-reallyportable/portable/generated/section_catalog.hpp');
const portable = read('th07-eagler/src/PracticeRuntime.cpp');
const portableMenu = read('th07-eagler/src/MainMenu.cpp');
const portableAscii = read('th07-eagler/src/AsciiManager.cpp');
const portableMain = read('th07-eagler/src/main.cpp');
const portableWindow = read('th07-eagler/src/GameWindow.cpp');
const portableGles = read('th07-eagler/src/graphics/Gles.cpp');
const portableGui = read('th07-eagler/src/Gui.cpp');
const portableSupervisor = read('th07-eagler/src/Supervisor.cpp');
const portableCmake = read('th07-eagler/CMakeLists.txt');
const portableReplay = read('th07-eagler/src/ReplayManager.cpp');
const portableResult = read('th07-eagler/src/ResultScreen.cpp');
const portableImGui = read('th07-eagler/src/ThpracImGui.cpp');
const portableSession = read('thprac-reallyportable/portable/src/session.cpp');
const portableAdapter = read('thprac-reallyportable/portable/adapters/th07/adapter.cpp');

const upstreamPracStateStart = upstream.indexOf('__declspec(noinline) void State(int state)');
const upstreamPracStateEnd = upstream.indexOf('\n    protected:', upstreamPracStateStart);
const upstreamPracState = upstream.slice(upstreamPracStateStart, upstreamPracStateEnd);
const upstreamState3Start = upstreamPracState.indexOf('case 3:');
const upstreamState4Start = upstreamPracState.indexOf('case 4:');
const upstreamState3 = upstreamPracState.slice(upstreamState3Start, upstreamState4Start);
const upstreamState1Start = upstreamPracState.indexOf('case 1:');
const upstreamState2Start = upstreamPracState.indexOf('case 2:');
const upstreamState1 = upstreamPracState.slice(upstreamState1Start, upstreamState2Start);
const upstreamState4 = upstreamPracState.slice(upstreamState4Start);
if (upstreamState3.includes('*mNavFocus = 0;') || !upstreamState4.includes('*mNavFocus = 0;'))
    throw new Error('Upstream TH07 nav-focus ownership drifted: State(3) preserves focus and State(4) clears it');

// th07_prac_menu_3 is not merely a State(3) notification. It commits the
// selected stage to GameManager and forces Extra/Phantasm to their dedicated
// difficulty IDs. Hook inventory without these state writes is incomplete.
for (const anchor of [
    'GAME_MANAGER->stage = thPracParam.stage;',
    'if (thPracParam.stage == 6)',
    'GAME_MANAGER->difficulty = 4;',
    'else if (thPracParam.stage == 7)',
    'GAME_MANAGER->difficulty = 5;',
]) {
    if (!upstream.includes(anchor))
        throw new Error(`Upstream TH07 Practice accept-state write missing: ${anchor}`);
}
for (const anchor of [
    'g_GameManager.currentStage = PracticeRuntime::GetConfig().stage;',
    'if (g_GameManager.currentStage == 6)',
    'g_GameManager.difficulty = 4;',
    'else if (g_GameManager.currentStage == 7)',
    'g_GameManager.difficulty = 5;',
]) {
    if (!portableMenu.includes(anchor))
        throw new Error(`Portable TH07 Practice accept-state write missing: ${anchor}`);
}

// TH07 has its own menu/runtime lifetime. State(1) clears live thPracParam but
// does not recreate THGuiPrac's widget members; State(3) is the only menu
// commit path. Unlike TH06 v2.3.0.3, TH07 defines no THPauseMenu/State(5).
for (const anchor of [
    'class THGuiPrac : public Gui::GameGuiWnd',
    'case 1:',
    'THReset();',
    'case 3:',
    'thPracParam.mode = *mMode;',
    'thPracParam.stage = *mStage;',
    'thPracParam.section = CalcSection();',
    'thPracParam.phase = SpellPhase() ? *mPhase : 0;',
    'thPracParam.point_total = *mPointTotal;',
    'thPracParam.point_stage = *mPointStage;',
    'thPracParam.cherryMax = *mCherryMax;',
    'thPracParam.rankLock = *mRankLock;',
]) {
    if (!upstream.includes(anchor))
        throw new Error(`Upstream TH07 Practice state contract missing: ${anchor}`);
}
if (upstream.includes('class THPauseMenu') || upstream.includes('THPauseMenu::singleton()'))
    throw new Error('Upstream TH07 unexpectedly acquired a TH06-style THPauseMenu');

// THSectionPatch has a state write outside the generated ECL patch itself:
// any non-zero section forces the already-created player to ALIVE.  This is a
// lifecycle semantic, not an x86 implementation detail, and must survive the
// portable rewrite.
for (const anchor of [
    'if (thPracParam.section)',
    'PLAYER->playerState = PLAYER_STATE_ALIVE;',
]) {
    if (!upstream.includes(anchor))
        throw new Error(`Upstream TH07 section player-state contract missing: ${anchor}`);
}
if (!portable.includes('if (g_Config.section != 0)') ||
    !portable.includes('g_Player.playerState = PLAYER_STATE_ALIVE;'))
    throw new Error('Portable TH07 direct-section start no longer mirrors THSectionPatch PLAYER_STATE_ALIVE');

// THStageWarp is not purely a timeline jump. Three chapter starts patch ECL
// data as well, and those writes must stay in the adapter even though the
// actual timeline time is represented separately by the portable runtime.
for (const anchor of [
    'ecl << pair{0xbb3c, (int16_t)1559}',
    '<< pair{0xbb5c, (int16_t)1560};',
    'ecl << pair{0xee3e, (int16_t)0x68};',
    'ecl << pair{0x1265a, (int16_t)0x68};',
]) {
    if (!upstream.includes(anchor))
        throw new Error(`Upstream TH07 chapter ECL mutation missing: ${anchor}`);
}
for (const anchor of [
    'stage == 3 && portion == 3',
    'pair{0xbb3c, static_cast<std::int16_t>(1559)}',
    'pair{0xbb5c, static_cast<std::int16_t>(1560)}',
    'stage == 7 && portion == 6',
    'pair{0xee3e, static_cast<std::int16_t>(0x68)}',
    'stage == 8 && portion == 6',
    'pair{0x1265a, static_cast<std::int16_t>(0x68)}',
]) {
    if (!portableAdapter.includes(anchor))
        throw new Error(`Portable TH07 chapter ECL mutation missing: ${anchor}`);
}
const applyInitialStart = portable.indexOf('void ApplyInitialState(GameManager &gameManager, bool applyStats)');
const applyInitialEnd = portable.indexOf('\nstatic double JsonNumber', applyInitialStart);
const applyInitialBody = portable.slice(applyInitialStart, applyInitialEnd);
if (applyInitialBody.includes('ResolveWarpFrame(g_Config.stage, g_Config.warp)')) {
    throw new Error('Portable TH07 must not reinterpret Warp selector Mid/End/Nonspell/Spell/Frame as a chapter portion');
}
// Direct Frame and Chapter are separate upstream paths. patch_main always
// runs ECLST3BG + ECLTimeWarp(2, frame), whereas THStageWarp alone owns the
// Stage-2 three-timeline special case and Stage-3-only ECLST3BG chapter call.
if (!upstream.includes('if (thPracParam.frame) {') ||
    !upstream.includes('ECLST3BG();') ||
    !upstream.includes('ECLTimeWarp(2, thPracParam.frame);') ||
    !upstream.includes('ECLTimeWarp(3, 390);') ||
    !applyInitialBody.includes('if (g_Config.frame > 0)') ||
    !applyInitialBody.includes('for (i32 index = 0; index < 2; index++)') ||
    !applyInitialBody.includes('const i32 count = chapterStage == 1 ? 3 : 2;') ||
    !applyInitialBody.includes('if (chapterStage == 2)\n                applyStage3BackgroundPatch();') ||
    applyInitialBody.includes('const i32 count = g_Config.stage == 1 ? 3 : 2;')) {
    throw new Error('Portable TH07 must not merge direct Frame and THStageWarp chapter timeline/background semantics');
}

// th07_patch_main lives at the very end of GameManager::AddedCallback. Do not
// move trainer state earlier and then suppress later vanilla writes to make the
// final screenshot look right; intermediate Replay/BGM/init state is part of
// the observable contract too.
const portableGame = read('th07-eagler/src/GameManager.cpp');
const gameAddedStart = portableGame.indexOf('ZunResult GameManager::AddedCallback(GameManager *arg)');
const gameAddedEnd = portableGame.indexOf('\nZunResult GameManager::DeletedCallback', gameAddedStart);
const gameAdded = portableGame.slice(gameAddedStart, gameAddedEnd);
const vanillaScoreReset = gameAdded.indexOf('arg->globals->score = 0;');
const applyAtEnd = gameAdded.indexOf('PracticeRuntime::ApplyInitialState(*arg, true);');
const runtimeAtEnd = gameAdded.indexOf('RuntimeExtension::OnGameStarted(arg);', applyAtEnd);
if (vanillaScoreReset < 0 || applyAtEnd < vanillaScoreReset || runtimeAtEnd < applyAtEnd ||
    gameAdded.includes('if (!PracticeRuntime::Active())\n        arg->globals->score = 0;')) {
    throw new Error('Portable TH07 patch_main timing must remain after vanilla AddedCallback resets, with section runtime side effects last');
}

// TH07 stage list is 1..6 + Extra + Phantasm, with stage-specific chapter
// cardinalities. Keep these independent from TH06.
for (const anchor of [
    'Gui::GuiCombo mStage { TH_STAGE, TH07_STAGE_SELECT };',
    '{ 2, 1 },',
    '{ 1, 1 },',
    '{ 4, 4 },',
    '{ 5, 3 }',
]) {
    if (!upstream.includes(anchor))
        throw new Error(`Upstream TH07 stage/chapter contract missing: ${anchor}`);
}

// Original TH07 chooses section IDs from th_sections_cba/cbt by stage+warp.
// mDiffculty is only the visible string-table selector; it is not an
// availability filter. This is especially important for Extra/Phantasm,
// whose generated rows carry only the 0x10 source mask.
for (const anchor of [
    'th_sections_cba[*mStage][*mWarp - 2]',
    'th_sections_cbt[*mStage][*mWarp - 4]',
    'th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]',
]) {
    if (!upstream.includes(anchor))
        throw new Error(`Upstream TH07 section-selection contract missing: ${anchor}`);
}

const start = catalog.indexOf('inline constexpr SectionLabel th07SectionLabels[] = {');
const end = catalog.indexOf('inline constexpr std::size_t th07SectionLabelCount', start);
if (start < 0 || end < 0)
    throw new Error('Generated TH07 section catalogue boundary is missing');
const rows = [...catalog.slice(start, end).matchAll(/^\s*\{(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(0x[0-9a-f]+),/gmi)]
    .map(match => ({
        id: Number(match[1]), patchId: Number(match[2]), stage: Number(match[3]),
        bgm: Number(match[4]), spell: Number(match[5]), mask: Number.parseInt(match[6], 16),
    }));
if (rows.length === 0 || rows.some(row => row.id !== row.patchId))
    throw new Error('TH07 generated catalogue unexpectedly changed id/patchId identity');
const unique = values => [...new Set(values)];
const extraIds = unique(rows.filter(row => row.stage === 6).map(row => row.patchId));
const phantasmIds = unique(rows.filter(row => row.stage === 7).map(row => row.patchId));
if (extraIds.length !== 21 || extraIds[0] !== 61 || extraIds.at(-1) !== 81 ||
    phantasmIds.length !== 22 || phantasmIds[0] !== 82 || phantasmIds.at(-1) !== 103 ||
    rows.filter(row => row.stage >= 6).some(row => row.mask !== 0x10)) {
    throw new Error('TH07 Extra/Phantasm generated catalogue cardinality/mask drifted');
}

// Portable target contracts. These intentionally fail against the historical
// half-port until the corresponding audit items are fixed.
for (const anchor of [
    'static Config g_MenuConfig',
    'BuildSectionMatchesFor(',
    'keep the first row so the section ID remains available',
    'g_Config = g_MenuConfig;',
    'MenuVisualState::Opening',
    'g_MenuCloseStep = 0.8f;',
    'RequestMenuClose(MenuResult::Cancelled, false)',
    'RequestMenuClose(MenuResult::Accepted, true)',
    'ImGui::SetNextWindowSize(chinese ? ImVec2(330.0f, 390.0f) : ImVec2(400.0f, 390.0f)',
    'locale == ThpracImGui::Locale::EnUS ? -80.0f',
    'locale == ThpracImGui::Locale::JaJP ? -65.0f : -60.0f',
    'ThpracImGui::TextId::PointTotal',
    'ThpracImGui::TextId::PointStage',
    'ThpracImGui::TextId::Cherry',
    'ThpracImGui::TextId::CherryMax',
    'ThpracImGui::TextId::CherryPlus',
    'ThpracImGui::TextId::SpellBonus',
]) {
    if (!portable.includes(anchor))
        throw new Error(`Portable TH07 audited contract missing: ${anchor}`);
}
if (portable.includes('entry.stage != g_Config.stage || !(entry.difficultyMask & difficultyBit)') ||
    portable.includes('SDL_IOFromFile("thprac-session.json", "rb")') ||
    portable.includes('LoadDesktopSession();')) {
    throw new Error('Portable TH07 still contains launcher/session-era desktop gating or difficulty-as-availability filtering');
}
if (!portable.includes('return g_GameManager.practice != 0;') ||
    portable.includes('return g_GameManager.practice != 0 && g_GameManager.replay == 0;'))
    throw new Error('Portable TH07 Practice entry must follow upstream th07_prac_menu_1 and ignore stale replay ownership');

// THGuiRep::mRepStatus is an independent state owner. State(1) clears it and
// State(3) sets it for every accepted replay; th07_unpause_prevent_desync reads
// that flag directly. It must not be collapsed into GameManager::replay or the
// Practice flag just because those often overlap during playback.
for (const anchor of [
    'mRepStatus = false;',
    'mRepStatus = true;',
    'if (THGuiRep::singleton().mRepStatus) return;',
]) {
    if (!upstream.includes(anchor))
        throw new Error(`Upstream TH07 Replay ownership contract missing: ${anchor}`);
}
const filterStart = portable.indexOf('void FilterUnpauseInput()');
const filterEnd = portable.indexOf('\nbool Enabled()', filterStart);
const filterBody = portable.slice(filterStart, filterEnd);
if (!filterBody.includes('if (g_ReplayPlaybackActive)') || filterBody.includes('if (g_GameManager.replay)'))
    throw new Error('Portable TH07 unpause filtering must use THGuiRep-equivalent replay ownership, not GameManager::replay');
const refreshStart = portable.indexOf('void RefreshFromHost()');
const refreshEnd = portable.indexOf('\nconst Config &GetConfig()', refreshStart);
const refreshBody = portable.slice(refreshStart, refreshEnd);
if ((refreshBody.match(/if \(g_ReplayStartupCommitted && g_GameManager\.replay\)/g) || []).length < 2 ||
    !refreshBody.includes('if (g_GameManager.practice && g_Config.active)') ||
    refreshBody.includes('g_ReplayPlaybackActive = false;') ||
    !refreshBody.includes('ThpracPortableTh07SetSessionJson(nullptr);')) {
    throw new Error('Portable TH07 startup must use a one-shot Replay payload bridge without clearing sticky upstream THGuiRep::mRepStatus');
}
const hostAbsentStart = refreshBody.indexOf('if (!active)\n    {');
const hostAbsentEnd = refreshBody.indexOf('Config config;', hostAbsentStart);
const hostAbsentBody = refreshBody.slice(hostAbsentStart, hostAbsentEnd);
if (!hostAbsentBody.includes('g_Config = {};') ||
    !hostAbsentBody.includes('ThpracPortableTh07SetSessionJson(nullptr);')) {
    throw new Error('Portable TH07 missing-host boundary must clear both C++ and adapter live owners');
}

// State(1) is a live-state reset, not a restore-from-last-run operation.
// Persistent THGuiPrac widgets survive independently; host/adapter live state
// must therefore be cleared rather than copied back into g_MenuConfig.
const openPracticeStart = portable.indexOf('void OpenPracticeMenu(i32 difficulty, i32 shotType)');
const openPracticeEnd = portable.indexOf('\nstatic i32 ChapterLimit', openPracticeStart);
const openPractice = portable.slice(openPracticeStart, openPracticeEnd);
for (const anchor of [
    'int oldRank = mDiffculty;',
    'if (mDiffculty != oldRank)',
    'if (oldRank == 0 && mDiffculty > 0)',
    '*mRank = 32;',
    'else if (oldRank > 0 && mDiffculty == 0)',
    '*mRank = 20;',
]) {
    if (!upstreamPracState.includes(anchor))
        throw new Error(`Upstream TH07 State(1) difficulty/rank transition missing: ${anchor}`);
}
for (const anchor of [
    'const i32 oldDifficulty = g_MenuDifficulty;',
    'if (g_MenuDifficulty != oldDifficulty)',
    'if (oldDifficulty == 0 && g_MenuDifficulty > 0)',
    'g_Config.rank = 32;',
    'else if (oldDifficulty > 0 && g_MenuDifficulty == 0)',
    'g_Config.rank = 20;',
]) {
    if (!openPractice.includes(anchor))
        throw new Error(`Portable TH07 State(1) difficulty/rank transition drift: ${anchor}`);
}
if (!openPractice.includes('Module.eaglerOptions.thpracSession = null;') ||
    !openPractice.includes('ThpracPortableTh07SetSessionJson(nullptr);') ||
    openPractice.includes('RefreshFromHost();') ||
    openPractice.includes('g_MenuConfig = g_Config;')) {
    throw new Error('Portable TH07 State(1) must clear live host/adapter state without overwriting persistent menu widgets');
}

// MainMenu::AddedCallback deliberately clears GameManager.practice after
// remembering a returned Practice route in isPracticeMode.  Vanilla
// STATE_SELECT_PRACTICE_STAGE re-arms GameManager.practice at stateTimer==0.
// The portable thprac branch intercepts before the vanilla block, so it must
// preserve that write before opening THGuiPrac; otherwise the second returned
// Practice run reaches GameManager::AddedCallback with practice==0, which
// clears the just-committed session (power/section collapse), and the third
// return no longer routes back to Practice at all.
const practiceStageStart = portableMenu.indexOf('u32 MainMenu::OnUpdateSelectPracticeStage()');
const practiceStageEnd = portableMenu.indexOf('\nu32 MainMenu::OnUpdateSelectReplay()', practiceStageStart);
const practiceStageBody = portableMenu.slice(practiceStageStart, practiceStageEnd);
const thpracBranchStart = practiceStageBody.indexOf('if (PracticeRuntime::Enabled())');
const thpracOpen = practiceStageBody.indexOf('PracticeRuntime::OpenPracticeMenu(', thpracBranchStart);
const thpracPracticeOwner = practiceStageBody.indexOf('g_GameManager.practice = 1;', thpracBranchStart);
const vanillaPracticeOwner = practiceStageBody.indexOf('g_GameManager.practice = 1;', thpracOpen + 1);
if (practiceStageStart < 0 || practiceStageEnd < 0 || thpracBranchStart < 0 || thpracOpen < 0 ||
    thpracPracticeOwner < thpracBranchStart || thpracPracticeOwner > thpracOpen ||
    vanillaPracticeOwner < thpracOpen) {
    throw new Error('Portable TH07 thprac stage-select interception must preserve the vanilla practice=1 owner before opening THGuiPrac');
}
const requestCloseStart = portable.indexOf('static void RequestMenuClose(MenuResult result, bool accept)');
const requestCloseEnd = portable.indexOf('\nMenuResult PollPracticeMenu()', requestCloseStart);
const requestCloseBody = portable.slice(requestCloseStart, requestCloseEnd);
if (!portable.includes('static bool PublishPortableSession()') ||
    !requestCloseBody.includes('PublishPortableSession()') ||
    !requestCloseBody.includes('CommitMenuConfigToRuntime();')) {
    throw new Error('Portable TH07 State(3) must commit both PracticeRuntime live params and the portable adapter session');
}
if (!requestCloseBody.includes('g_Config = {};')) {
    throw new Error('Portable TH07 State(4) must preserve widgets while leaving live THPracParam at THReset() zero');
}
const commitStart = portable.indexOf('static void CommitMenuConfigToRuntime()');
const commitEnd = portable.indexOf('\nstatic void AdjustMenuValue', commitStart);
const commitBody = portable.slice(commitStart, commitEnd);
if (!commitBody.includes('g_Config.warp = 0;') ||
    !upstreamState3.includes('thPracParam.section = CalcSection();') ||
    upstreamState3.includes('thPracParam.warp =')) {
    throw new Error('Portable TH07 State(3) must keep Warp as persistent widget state and leave live THPracParam.warp at Reset value zero');
}
const replayResetStart = portable.indexOf('void ReplayMenuReset()');
const replayCheckStart = portable.indexOf('\nbool ReplayMenuCheck(', replayResetStart);
const replayResetBody = portable.slice(replayResetStart, replayCheckStart);
const replayCheckEnd = portable.indexOf('\nvoid ReplayMenuActivate()', replayCheckStart);
const replayCheckBody = portable.slice(replayCheckStart, replayCheckEnd);
const replayActivateStart = portable.indexOf('void ReplayMenuActivate()');
const replayActivateEnd = portable.indexOf('\nbool ReplayPlaybackActive()', replayActivateStart);
const replayActivateBody = portable.slice(replayActivateStart, replayActivateEnd);
if (!replayResetBody.includes('Module.eaglerOptions.thpracSession = null;') ||
    !replayResetBody.includes('ThpracPortableTh07SetSessionJson(nullptr);') ||
    !replayResetBody.includes('g_ReplayCandidateValid = false;') ||
    replayResetBody.includes('g_ReplayCandidate = {};') ||
    !replayCheckBody.includes('g_ReplayCandidateValid = true;') ||
    !replayCheckBody.includes('g_ReplayCandidate = {};') ||
    replayCheckBody.includes('g_ReplayCandidateValid = false;') ||
    !replayActivateBody.includes('PublishConfigToHost();') ||
    !replayActivateBody.includes('PublishPortableSession()') ||
    !replayActivateBody.includes('g_ReplayPlaybackActive = true;') ||
    !replayActivateBody.includes('g_ReplayStartupCommitted = true;') ||
    !replayActivateBody.includes('Module.eaglerOptions.thpracSession = null;')) {
    throw new Error('Portable TH07 Replay State(1/2/3) must preserve sticky mParamStatus/mRepStatus while switching live adapter/host state from the resolved candidate');
}

// Config{} is the live THPracParam::Reset() representation. THGuiPrac's
// constructor defaults belong only to persistent g_MenuConfig.
for (const anchor of [
    'i32 life = 0;', 'i32 bomb = 0;', 'i32 power = 0;',
    'i32 cherryMax = 0;', 'i32 rank = 0;',
]) {
    if (!read('th07-eagler/src/PracticeRuntime.hpp').includes(anchor))
        throw new Error(`Portable TH07 Config{} must equal live Reset(): ${anchor}`);
}
for (const anchor of [
    'config.life = 8;', 'config.bomb = 8;', 'config.power = 128;',
    'config.cherryMax = 200000;', 'config.rank = 16;',
]) {
    if (!portable.includes(anchor))
        throw new Error(`Portable TH07 persistent THGuiPrac default missing: ${anchor}`);
}

// THGuiPrac has no mShotType state in TH07; adding a persistent portable shot
// owner creates a source-less pollution path.
if (portable.includes('static i32 g_MenuShotType') || !openPractice.includes('(void)shotType;'))
    throw new Error('Portable TH07 must not invent a THGuiPrac shot-type owner absent from upstream');

// th07_update is a tail hook inside Chain::RunCalcChain. Trainer hotkey/UI
// producers must stay after this tick's game calc consumers.
const th07RunCalc = portableWindow.indexOf('chainRes = g_Chain.RunCalcChain();');
const th07TrainerUpdate = portableWindow.indexOf('PracticeRuntime::UpdateOverlay();');
if (th07RunCalc < 0 || th07TrainerUpdate < th07RunCalc)
    throw new Error('Portable TH07 trainer update must remain at the upstream post-RunCalcChain boundary');

// THOverlay input ownership is not global. Backspace is evaluated from
// THOverlay::OnPreUpdate only when no earlier ImGui item is active; F1..F7
// GuiHotKey operators are called only by OnContentUpdate while the Mod Menu is
// open. Tracker and Advanced Options have their own independent owners.
const overlayUpdateStart = portable.indexOf('void UpdateOverlay()');
const overlayDrawStart = portable.indexOf('void DrawOverlay()', overlayUpdateStart);
const overlayUpdateBody = portable.slice(overlayUpdateStart, overlayDrawStart);
const overlayDrawEnd = portable.indexOf('\nvoid NotifyBorderBreak()', overlayDrawStart);
const overlayDrawBody = portable.slice(overlayDrawStart, overlayDrawEnd);
if (!upstream.includes('if (mMenu(false) && !ImGui::IsAnyItemActive())') ||
    !overlayUpdateBody.includes('g_ModMenuToggleRequested = true;') ||
    overlayUpdateBody.includes('g_Overlay.invincible = !g_Overlay.invincible') ||
    !overlayDrawBody.includes('if (!ImGui::IsAnyItemActive())') ||
    !overlayDrawBody.includes('if (g_Overlay.menuOpen)') ||
    !overlayDrawBody.includes('OverlayKeyPressed(1, SDL_SCANCODE_F1)') ||
    !overlayDrawBody.includes('OverlayKeyPressed(7, SDL_SCANCODE_F7)') ||
    !overlayUpdateBody.includes('g_AdvancedMenuToggleRequested = true;') ||
    !overlayDrawBody.includes('g_AdvancedOptions.menuOpen = !g_AdvancedOptions.menuOpen;')) {
    throw new Error('Portable TH07 must preserve THOverlay Backspace item-owner guard, F1-F7 open-window sampling, and post-overlay F12 ownership');
}

// Common GameGuiEnd locale surface: Alt+1/2/3 is evaluated after all current
// ImGui content and only when no item is active. Font-atlas replacement is a
// backend concern, but must happen before the next frame uses translated text.
for (const anchor of [
    'Gui::GetChordPressedDuration(hotkeys.language)',
    "Gui::KeyboardInputUpdate('1') == 1",
    'Gui::LocaleSet(LOCALE_JA_JP);',
    "Gui::KeyboardInputUpdate('2') == 1",
    'Gui::LocaleSet(LOCALE_ZH_CN);',
    "Gui::KeyboardInputUpdate('3') == 1",
    'Gui::LocaleSet(LOCALE_EN_US);',
]) {
    if (!upstreamGames.includes(anchor))
        throw new Error(`Upstream common GameGuiEnd locale contract missing: ${anchor}`);
}
for (const anchor of [
    'OverlayKeyPressed(10, SDL_SCANCODE_1)',
    'RequestLocale(ThpracImGui::Locale::JaJP)',
    'OverlayKeyPressed(11, SDL_SCANCODE_2)',
    'RequestLocale(ThpracImGui::Locale::ZhCN)',
    'OverlayKeyPressed(12, SDL_SCANCODE_3)',
    'RequestLocale(ThpracImGui::Locale::EnUS)',
]) {
    if (!portable.includes(anchor))
        throw new Error(`Portable TH07 GameGuiEnd locale surface missing: ${anchor}`);
}
if (!portableImGui.includes('void RequestLocale(Locale locale)') ||
    !portableImGui.includes('if (g_LocaleChangePending)') ||
    !portableImGui.includes('BuildLocaleFont(io, g_Locale)') ||
    !portableGles.includes('this->imguiFontTexture != 0 && io.Fonts->TexID == nullptr')) {
    throw new Error('Portable TH07 locale change must defer atlas rebuild and replace the GLES font texture');
}
if (!upstream.includes('GameGuiBegin(IMPL_WIN32_DX8, !THAdvOptWnd::singleton().IsOpen());') ||
    !portableWindow.includes('ThpracImGui::SetGameNavEnabled(!PracticeRuntime::AdvancedOptionsOpen());') ||
    !portableImGui.includes('g_GameNavEnabled && (g_GameButtons & TH_BUTTON_UP)') ||
    !portable.includes('bool AdvancedOptionsOpen()')) {
    throw new Error('Portable TH07 GameGuiBegin must disable background game navigation while THAdvOptWnd is open');
}
if (!upstreamGames.includes('if (draw_cursor && Gui::ImplWin32CheckFullScreen())') ||
    !upstreamGames.includes('io.MouseDrawCursor = true;') ||
    !upstream.includes('bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen();') ||
    !upstream.includes('GameGuiEnd(drawCursor);') ||
    !portable.includes('ImGui::GetIO().MouseDrawCursor = fullscreen &&') ||
    !portable.includes('(g_AdvancedOptions.menuOpen || g_MenuOpen);')) {
    throw new Error('Portable TH07 GameGuiEnd must preserve fullscreen software-cursor ownership for AdvOpt/THGuiPrac only');
}

// TH07's two gui_update_frames=2 writes exist only to force static GUI
// redraw for a couple of frames. Portable unconditionally sets
// redrawEveryFrame=1 at Supervisor init, making that need structurally always
// satisfied. This is an architectural N/A only while that invariant holds.
if ((upstream.match(/SUPERVISOR->gui_update_frames = 2;/g) || []).length !== 2 ||
    !portableSupervisor.includes('this->cfg.redrawEveryFrame = 1;') ||
    !portableGui.includes('g_Supervisor.cfg.redrawEveryFrame || vm->currentInstruction ||') ||
    !portableGui.includes('g_Supervisor.renderSkipFrames != 0')) {
    throw new Error('TH07 gui_update_frames architectural-N/A proof failed: portable must structurally redraw static GUI every frame');
}

// th07 already has a typed full-backbuffer Home screenshot path in the game
// backend. That is the architectural equivalent of THSnapshot; do not add a
// second thprac screenshot owner just to mimic the old DX8 helper shape.
if (!upstream.includes('Gui::GetChordPressed(hotkeys.screenshot)') ||
    !upstream.includes('THSnapshot::Snapshot(SUPERVISOR->d3d_device);') ||
    !portableWindow.includes('if (WAS_PRESSED_RAW(TH_BUTTON_HOME))') ||
    !portableWindow.includes('g_Supervisor.SnapshotScreen(snapshotPath);')) {
    throw new Error('Portable TH07 Home screenshot backend-equivalent is missing');
}

// THSetPoint continues from the current point_extends owner; it never resets
// that value before recreating the vanilla next-extend threshold loop.
const upstreamSetPointStart = upstream.indexOf('void THSetPoint()');
const upstreamSetPointEnd = upstream.indexOf('\n    bool THBGMTest()', upstreamSetPointStart);
const upstreamSetPoint = upstream.slice(upstreamSetPointStart, upstreamSetPointEnd);
const portableSetPointStart = portable.indexOf('static void SetPointItems(GameManager &gameManager)');
const portableSetPointEnd = portable.indexOf('\nvoid ApplyInitialState', portableSetPointStart);
const portableSetPoint = portable.slice(portableSetPointStart, portableSetPointEnd);
if (!upstreamSetPoint.includes('++(globals->point_extends);') ||
    !portableSetPoint.includes('++globals.extendsFromPointItems;') ||
    portableSetPoint.includes('globals.extendsFromPointItems = 0;')) {
    throw new Error('Portable TH07 THSetPoint must preserve current point_extends and only increment it through the upstream threshold loop');
}
if (!portable.includes('static bool g_ImGuiMenuFocusRemembered = false;') ||
    !portable.includes('g_ImGuiMenuFocusRemembered = true;') ||
    !portable.includes('State(4) alone clears mNavFocus') ||
    !portable.includes('g_ImGuiMenuFocusRemembered = false;')) {
    throw new Error('Portable TH07 must preserve Practice nav focus after State(3) and clear it only on State(4)');
}
if (!portable.includes('for (i32 round = 0; round < 3 && repeatedPracticeStable; round++)') ||
    !portable.includes('g_Config.section == 4 && g_Config.life == 5 && g_Config.bomb == 4 && g_Config.power == 96') ||
    !portable.includes('!g_Config.active && g_MenuConfig.section == 4 && g_MenuConfig.power == 96')) {
    throw new Error('Portable TH07 must retain the focused three-round repeated-Practice lifecycle regression');
}
if (!upstream.includes('thFakeShot = -1;'))
    throw new Error('Upstream TH07 THReset fake-shot ownership disappeared');
if (!portableAdapter.includes('g_Context = {};') ||
    !portableAdapter.includes('Session replacement is also a patch-context ownership boundary')) {
    throw new Error('Portable TH07 adapter no longer clears section-derived PatchContext at session reset boundaries');
}
if (!upstream.includes('thPracParam.mode = *mMode;') ||
    !upstream.includes('thPracParam.section = CalcSection();') ||
    !upstream.includes('if (thPracParam.mode == 1) {') ||
    !upstream.includes('THSectionPatch();') ||
    !portableAdapter.includes('g_Session.mode != 1 || g_Session.section == 0')) {
    throw new Error('Portable TH07 must keep hidden Mode=Original section state inert; ECL patch ownership belongs to mode==1');
}

// TH07 rank has a zero sentinel independent of the GUI slider bounds.
// Reset()/ReadJson() can leave it at zero and patch_main then deliberately
// skips the rank write.  A portable clamp to the GUI minimum would corrupt
// old/missing-field PRAC Replay metadata.
if (!upstream.includes('if (thPracParam.rank) {') ||
    !portable.includes('g_Config.rank = Clamp(g_Config.rank, 0, 99);') ||
    !portable.includes('if (g_Config.rank != 0)') ||
    portable.includes('g_Config.rank = Clamp(g_Config.rank, 10, 99);')) {
    throw new Error('Portable TH07 must preserve upstream rank=0 no-override sentinel');
}
if (!portable.includes('JsonNumber(json, "mode", 0)') ||
    !portable.includes('JsonNumber(json, "life", 0)') ||
    !portable.includes('JsonNumber(json, "bomb", 0)') ||
    !portable.includes('JsonNumber(json, "power", 0)') ||
    !portable.includes('JsonNumber(json, "cherryMax", 0)') ||
    !portable.includes('JsonNumber(json, "rank", 0)') ||
    !portableSession.includes('s.rank = Clamp(s.rank, 0, 99);') ||
    !portableSession.includes('Number(json, "mode", 0)') ||
    !portableSession.includes('Number(json, "cherryMax", 0)') ||
    !portableSession.includes('Number(json, "rank", 0)')) {
    throw new Error('Portable TH07 replay/session parser must preserve Reset=0 fields and rank 0..99 wire semantics');
}
for (const anchor of [
    'HostNumber("mode", 0)',
    'HostNumber("life", 0)',
    'HostNumber("bomb", 0)',
    'HostNumber("power", 0)',
    'HostNumber("cherryMax", 0)',
    'HostNumber("rank", 0)',
]) {
    if (!portable.includes(anchor))
        throw new Error(`Portable TH07 Web host session fallback must match Reset=0: ${anchor}`);
}

// Chapter is a normal visible SliderInt. The encoded section is only allowed
// to seed the widget on an explicit restore/sync path; live mouse/left/right
// edits must flow g_MenuChapter -> section without being rolled back.
const chapterStateStart = portable.indexOf('if (g_Config.warp == 1)');
const chapterStateEnd = portable.indexOf('\n    if (g_Config.warp == 6)', chapterStateStart);
const chapterState = portable.slice(chapterStateStart, chapterStateEnd);
if (!chapterState.includes('else if (syncFromConfig && g_Config.section >= 10000') ||
    !portable.includes('GuiSliderInt(sectionLabel, &g_MenuChapter, 1, limit, chapterStep, format)') ||
    !portable.includes('CurrentSection(0, false);') ||
    !portable.includes('bool changed = ImGui::SliderInt(label, value, minimum, maximum, format);') ||
    portable.includes('relativeMouse')) {
    throw new Error('Portable TH07 Chapter slider/input ownership regressed');
}

// THGuiPrac itself is a pinned Dear ImGui 1.82 same-process window, not the
// historical ASCII placeholder.  The bridge samples game directions on the
// fixed tick, feeds SDL mouse events, draws after the game into the 640x480
// game FBO, and restores GL state before normal presentation.
for (const [source, anchor, label] of [
    [portableCmake, 'src/ThpracImGui.cpp', 'TH07 CMake thprac ImGui source'],
    [portableCmake, '${THPRAC_IMGUI_ROOT}/imgui.cpp', 'TH07 pinned ImGui core'],
    [portableCmake, 'IMGUI_DISABLE_WIN32_FUNCTIONS', 'TH07 no Win32 ImGui backend'],
    [portableCmake, 'Freetype::Freetype', 'TH07 pinned ImGui FreeType atlas'],
    [portableMain, 'ThpracImGui::ProcessEvent(*event);', 'TH07 SDL event bridge'],
    [portableMain, '#if defined(TH_DEV_TOOLS) && !defined(TH_ENABLE_THPRAC)', 'TH07 F5 thprac ownership'],
    [portableWindow, 'ThpracImGui::SetGameInput(g_CurFrameRawInput, updated);', 'TH07 fixed-tick game-input bridge'],
    [portableWindow, 'ThpracImGui::BeginFrame(static_cast<f32>(targetDt));', 'TH07 fixed-tick ImGui frame'],
    [portableWindow, 'RenderImGui(ThpracImGui::GetDrawData())', 'TH07 same-FBO ImGui render'],
    [portableGles, 'glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);', 'TH07 ImGui game-FBO target'],
    [portableGles, 'glViewport(0, 0, 640, 480);', 'TH07 ImGui logical surface'],
    [portableGles, 'this->stateCache.Invalidate();', 'TH07 ImGui GL-state restoration boundary'],
    [portableImGui, 'GetGlyphRangesChineseFull()', 'TH07 Full CJK trainer atlas'],
    [portableImGui, 'g_LastFrameRawInput', 'TH07 Gen1 input previous sample'],
    [portableImGui, 'g_IsEighthFrameOfHeldInput', 'TH07 Gen1 8-frame repeat'],
]) {
    if (!source.includes(anchor))
        throw new Error(`${label} missing: ${anchor}`);
}
if (portable.includes('thprac - Advanced Practice') ||
    portable.includes('ScreenEffect::DrawSquare(&panel, 0xff181820)'))
    throw new Error('Portable TH07 still contains the historical ASCII Practice placeholder');

// Exact TH07 final-spell phase sets. Extra is Normal/Full/Rage while
// Phantasm is Normal/Rage; treating both as a boolean phase loses upstream
// functionality.
for (const anchor of [
    'if (section == 81)',
    'return 3;',
    'if (section == 102)',
    'return 2;',
    'phaseRagefulSelector[] = {1, 2, 3, 0}',
    'ThpracImGui::TextId::Full',
    'phaseSimpleSelector[] = {1, 2, 0}',
]) {
    if (!portable.includes(anchor))
        throw new Error(`Portable TH07 Extra/Phantasm phase contract missing: ${anchor}`);
}

// TH07 replay metadata is embedded in the actual T7RP replay.  The previous
// .thprac.json sidecar was an undeployed portable invention and must not be
// retained as a compatibility branch.  Upstream's multichar 'CARP' lands as
// PRAC bytes on little-endian x86.
for (const anchor of [
    'std::memcmp(bytes.data(), "T7RP", 4)',
    'std::memcpy(bytes.data() + oldSize + payloadSize + 4, "PRAC", 4)',
    'json.find("\\\"version\\\":")',
    '"{\\\"version\\\":\\\"2.3.0.3\\\",\\\"game\\\":\\\"th07\\\",\\\"mode\\\":%d,\\\"stage\\\":%d"',
    'std::vector<u8> checksumBytes(bytes.begin() + 13, bytes.end());',
    'for (std::size_t i = 0; i < bytes.size() - 16; ++i)',
    'checksumBytes[3 + i] = static_cast<u8>(checksumBytes[3 + i] - key);',
    'key = static_cast<u8>(key + 7);',
    'u32 checksum = 0x3f000318;',
    'WriteLe32(bytes.data() + 8, checksum);',
]) {
    if (!portable.includes(anchor))
        throw new Error(`Portable TH07 upstream replay contract missing: ${anchor}`);
}
if (portable.includes('.thprac.json') || portable.includes('"CARP"'))
    throw new Error('Portable TH07 still contains undeployed sidecar/CARP replay compatibility');
if (!portableReplay.includes('PracticeRuntime::SaveReplayMetadata('))
    throw new Error('TH07 ReplayManager no longer appends upstream thprac metadata after saving');
if (!upstream.includes('if (thPracParam.mode)\n            THSaveReplay(rep_name);') ||
    !portable.includes('if (!AdvancedActive() || !replayPath || !*replayPath)'))
    throw new Error('Portable TH07 replay metadata save must follow upstream thPracParam.mode gate');
if (portable.includes('ReplayUnsafeAssistUsedThisRun') ||
    portableResult.includes('ReplayUnsafeAssistUsedThisRun'))
    throw new Error('Portable TH07 must not add an assist-based replay-save ban absent from upstream thprac');
if (!portableSession.includes('const bool upstreamSchema = json.find("\\\"version\\\":") != std::string::npos;') ||
    !portableSession.includes('(!portableSchema && !upstreamSchema)'))
    throw new Error('TH07 adapter no longer accepts upstream version/game PRAC metadata JSON');

// TH07 has no TH06 custom Pause/Restart/State(5) parameter lifecycle.
for (const forbidden of [
    'g_PreserveConfigOnRestart',
    'DebugRestartPreservesConfig',
    '--thprac-restart-selftest',
    'class THPauseMenu',
]) {
    if (portable.includes(forbidden) || portableMain.includes(forbidden))
        throw new Error(`Portable TH07 retained TH06-only restart/pause concept: ${forbidden}`);
}

// TH07 upstream does not replace the game Pause UI. The portable adapter must
// therefore not suppress AsciiManager's vanilla pause draw/update path.
if (portableAscii.includes('PracticeRuntime::DrawPauseMenuPanel()') ||
    portableAscii.includes('if (!PracticeRuntime::UpdatePauseMenu())')) {
    throw new Error('Portable TH07 still installs the non-upstream TH06-style Pause overlay');
}

// Always accompany the detailed state assertions with the independent
// mechanically enumerated upstream hook/surface/state inventory.
await import('./thprac-upstream-hooks.mjs');

console.log(
    'TH07 thprac source contract PASS: TH07-specific Stage1..6/Extra/Phantasm menu; ' +
    'section availability follows cba/cbt while difficulty selects duplicate labels only; ' +
    'persistent THGuiPrac widget state is separate from State(3) live params; real pinned ImGui/GLES bridge; ' +
    'Extra=Normal/Full/Rage and Phantasm=Normal/Rage; in-file T7RP/PRAC replay metadata; ' +
    'desktop entry is stale-replay-safe real Practice flow; vanilla TH07 owns Pause'
);
