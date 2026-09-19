import fs from 'node:fs';
import path from 'node:path';
import { execFileSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';

const here = path.dirname(fileURLToPath(import.meta.url));
const project = path.resolve(here, '../..');
const workspace = path.resolve(project, '..');
const read = rel => fs.readFileSync(path.join(workspace, rel), 'utf8').replaceAll('\r\n', '\n');

const upstream = read('thprac-reallyportable/thprac/src/thprac/thprac_th07.cpp');
const src = {
    practice: read('th07-eagler/src/PracticeRuntime.cpp'),
    practiceH: read('th07-eagler/src/PracticeRuntime.hpp'),
    mainMenu: read('th07-eagler/src/MainMenu.cpp'),
    game: read('th07-eagler/src/GameManager.cpp'),
    gameH: read('th07-eagler/src/GameManager.hpp'),
    gui: read('th07-eagler/src/Gui.cpp'),
    player: read('th07-eagler/src/Player.cpp'),
    enemy: read('th07-eagler/src/EnemyManager.cpp'),
    ecl: read('th07-eagler/src/EclManager.cpp'),
    sound: read('th07-eagler/src/SoundPlayer.cpp'),
    replay: read('th07-eagler/src/ReplayManager.cpp'),
    controller: read('th07-eagler/src/Controller.cpp'),
    main: read('th07-eagler/src/main.cpp'),
    window: read('th07-eagler/src/GameWindow.cpp'),
    gles: read('th07-eagler/src/graphics/Gles.cpp'),
    adapter: read('thprac-reallyportable/portable/adapters/th07/adapter.cpp'),
    fullPatch: read('thprac-reallyportable/portable/generated/th07_full_patch.inc'),
};

// th07_update @ 0x42fdf8 is mapped inside the tail of Chain::RunCalcChain
// (function 0x42fd60..next function 0x42fe20). Preserve that producer order:
// overlay/advanced-menu hotkeys become visible only after this tick's game
// calc consumers, just like upstream.
const runCalc = src.window.indexOf('chainRes = g_Chain.RunCalcChain();');
const trainerUpdate = src.window.indexOf('PracticeRuntime::UpdateOverlay();');
if (runCalc < 0 || trainerUpdate < runCalc)
    throw new Error('th07_update backend-equivalent must remain post-RunCalcChain');

function requireText(source, needle, label) {
    if (!source.includes(needle))
        throw new Error(`${label}: missing ${JSON.stringify(needle)}`);
}

let mutationCaught = false;
try {
    exactSet(['THAdvOptWnd', 'THGuiPrac', 'THGuiRep', 'THOverlay', 'THMutationWnd'],
             ['THAdvOptWnd', 'THGuiPrac', 'THGuiRep', 'THOverlay'],
             'TH07 mutation');
} catch {
    mutationCaught = true;
}
if (!mutationCaught)
    throw new Error('TH07 Proof Ledger mutation self-test failed');

const hooks = [...upstream.matchAll(/(?:EHOOK|PATCH)_(?:ST|DY|HK)\((th07_[A-Za-z0-9_]+)/g)].map(m => m[1]);
const uniqueHooks = [...new Set(hooks)];
if (uniqueHooks.length !== 34)
    throw new Error(`TH07 upstream named hook inventory drifted: expected 34, got ${uniqueHooks.length}`);

function exactSet(actual, expected, label) {
    const a = [...new Set(actual)].sort();
    const e = [...expected].sort();
    if (JSON.stringify(a) !== JSON.stringify(e))
        throw new Error(`${label} drifted: actual=[${a}] expected=[${e}]`);
}

// Mechanically cover the source surfaces that a named-hook inventory cannot
// see, plus every direct central live-param write and explicit one-shot hook
// lifecycle site. Any upstream change here must force a new review.
exactSet(
    [...upstream.matchAll(/class\s+(TH[A-Za-z0-9_]+)\s*:\s*public\s+Gui::/g)].map(m => m[1]),
    ['THAdvOptWnd', 'THGuiPrac', 'THGuiRep', 'THOverlay'],
    'TH07 formal Gui surface inventory');
exactSet(
    [...upstream.matchAll(/thPracParam\.([A-Za-z_][A-Za-z0-9_]*)\s*=/g)].map(m => m[1]),
    ['_playLock', 'bomb', 'cherry', 'cherryMax', 'cherryPlus', 'dlg', 'frame', 'graze', 'life', 'mode', 'phase', 'point', 'point_stage', 'point_total', 'power', 'rank', 'rankLock', 'score', 'section', 'spellBonus', 'stage'],
    'TH07 central thPracParam write inventory');
exactSet(
    [...upstream.matchAll(/\b(th07_[A-Za-z0-9_]+)\.Enable\(\)/g)].map(m => m[1]),
    ['th07_rb'],
    'TH07 explicit hook Enable inventory');
exactSet(
    [...upstream.matchAll(/\b(th07_[A-Za-z0-9_]+)\.Disable\(\)/g)].map(m => m[1]),
    ['th07_rb'],
    'TH07 explicit hook Disable inventory');

const portableOwnerCategory = new Map(Object.entries({
    g_Config: 'source live THPracParam',
    g_MenuConfig: 'source persistent THGuiPrac widgets',
    g_MenuResult: 'backend THGuiPrac close result handoff',
    g_MenuOpen: 'source THGuiPrac/GameGuiWnd open state',
    g_MenuCursor: 'backend ImGui nav cursor',
    g_MenuDifficulty: 'source THGuiPrac mDiffculty',
    g_MenuSectionIndex: 'source THGuiPrac mSection',
    g_MenuChapter: 'source THGuiPrac mChapter',
    g_MenuVisualState: 'backend GameGuiWnd status/fade',
    g_MenuAlpha: 'backend GameGuiWnd fade',
    g_MenuCloseStep: 'backend GameGuiWnd fade',
    g_MenuPendingResult: 'backend close-callback handoff',
    g_ImGuiMenuFocusPending: 'backend mNavFocus handoff',
    g_ImGuiMenuFocusLabel: 'backend mNavFocus target',
    g_ImGuiMenuFocusRemembered: 'source THGuiPrac sticky mNavFocus',
    g_ReplayCandidate: 'source THGuiRep mRepParam',
    g_ReplayCandidateValid: 'source THGuiRep mParamStatus',
    g_ReplayPlaybackActive: 'source THGuiRep mRepStatus',
    g_ReplayStartupCommitted: 'portable State(3)->GameManager one-shot bridge',
    g_Overlay: 'source THOverlay/Tracker persistent state',
    g_ModMenuToggleRequested: 'backend post-calc->ImGui OnPreUpdate bridge',
    g_AdvancedMenuToggleRequested: 'backend THAdvOpt StaticUpdate ordering bridge',
    g_AdvancedOptions: 'source THAdvOptWnd persistent state',
    g_OverlayKeyDown: 'backend GuiHotKey/GuiHotKeyChord edge history',
    g_TrackerBorderBreaks: 'source THTracker border counter',
    g_EverlastingCurrentSong: 'source THOverlay everlasting-BGM identity',
    g_SupervisorFadeOutScope: 'backend typed scope for source fadeout hook',
    g_ResurrectionButterflySpawnSkipPending: 'source th07_rb one-shot',
}));
const portableOwnerNames = [...src.practice.matchAll(/^static\s+(?![^\n]*\()[^\n;=]+?\b(g_[A-Za-z0-9_]+)\b(?:\s*\[[^\]]+\])?\s*(?:=|;)/gm)].map(m => m[1]);
exactSet(portableOwnerNames, portableOwnerCategory.keys(), 'TH07 portable PracticeRuntime owner inventory');

const playLockOccurrences = [...upstream.matchAll(/_playLock/g)].map(m => m.index);
if (playLockOccurrences.length !== 2 ||
    !upstream.includes('bool _playLock = false;') ||
    !upstream.includes('thPracParam._playLock = true;'))
    throw new Error('TH07 _playLock stopped being declaration+write-only; re-audit required');

requireText(src.practice, 'g_ResurrectionButterflySpawnSkipPending = (g_Config.stage == 5 && g_Config.section == 60);',
            'th07_rb one-shot producer');
requireText(src.practice, 'g_ResurrectionButterflySpawnSkipPending = false;',
            'th07_rb one-shot consume/reset');

// No user-facing upstream surface may be classified optional. Categories only
// describe *how* the exact behavior is represented in portable code.
const category = new Map([
    ['th07_all_clear_bonus_1', 'advopt-core'],
    ['th07_all_clear_bonus_2', 'advopt-core'],
    ['th07_spell_bonus_display_fix1', 'advopt-core'],
    ['th07_spell_bonus_display_fix2', 'advopt-core'],
    ['th07_spell_bonus_display_fix3', 'advopt-core'],
    ['th07_rb', 'practice-core'],
    ['th07_enter', 'overlay-core'],
    ['th07_border_break', 'overlay-core'],
    ['th07_reacquire_input', 'architectural-equivalent'],
    ['th07_soundplayer_queue_command', 'overlay-core'],
    ['th07_prac_menu_1', 'practice-core'],
    ['th07_prac_menu_3', 'practice-core'],
    ['th07_prac_menu_4', 'practice-core'],
    ['th07_rep_menu_1', 'replay-core'],
    ['th07_rep_menu_2', 'replay-core'],
    ['th07_rep_menu_3', 'replay-core'],
    ['th07_unpause_prevent_desync', 'replay-core'],
    ['th07_patch_main', 'practice-core'],
    ['th07_disable_title', 'practice-core'],
    ['th07_fake_shot', 'practice-core'],
    ['th07_bgm', 'practice-core'],
    ['th07_bgm_st6_1', 'practice-core'],
    ['th07_bgm_st6_2', 'practice-core'],
    ['th07_bgm_st6_3', 'practice-core'],
    ['th07_save_replay', 'replay-core'],
    ['th07_disable_prac_menu1', 'practice-core'],
    ['th07_disable_prac_menu2', 'practice-core'],
    ['th07_update', 'backend-equivalent'],
    ['th07_render', 'backend-equivalent'],
    ['th07_disable_dataver', 'architectural-equivalent'],
    ['th07_disable_demo', 'trainer-core'],
    ['th07_disable_mutex', 'architectural-equivalent'],
    ['th07_gui_init_1', 'backend-equivalent'],
    ['th07_gui_init_2', 'backend-equivalent'],
]);
for (const hook of uniqueHooks) {
    if (!category.has(hook))
        throw new Error(`TH07 upstream hook has no mandatory classification: ${hook}`);
}

// Already-established structural equivalents.
requireText(src.practice, 'void OpenPracticeMenu(i32 difficulty, i32 shotType)', 'th07_prac_menu_1');
requireText(src.practice, 'RequestMenuClose(MenuResult::Accepted, true)', 'th07_prac_menu_3');
requireText(src.practice, 'RequestMenuClose(MenuResult::Cancelled, false)', 'th07_prac_menu_4');
requireText(src.practice, 'void ApplyInitialState(GameManager &gameManager, bool applyStats)', 'th07_patch_main');
requireText(src.replay, 'PracticeRuntime::SaveReplayMetadata(filename);', 'th07_save_replay');
requireText(src.mainMenu, 'arg->DrawPracticeMenu();\n        if (PracticeRuntime::Enabled())\n            return CHAIN_CALLBACK_RESULT_CONTINUE;',
            'th07_disable_prac_menu1/2 ImGui draw before vanilla Practice suppression');
requireText(src.mainMenu,
            'Give the newly\n            // opened trainer one real tick before polling its X/Z ownership.',
            'THGuiPrac first-tick input isolation after shot-type selection');
requireText(src.practice, 'if (!g_GameManager.practice)',
            'ordinary Start TH07 live-thprac boundary');
requireText(src.practice, 'g_Config = {};',
            'ordinary Start clears TH07 live thprac parameter block');
if (src.practice.slice(
        src.practice.indexOf('if (!g_GameManager.practice)'),
        src.practice.indexOf('const bool active = EM_ASM_INT', src.practice.indexOf('if (!g_GameManager.practice)')))
    .includes('g_ReplayPlaybackActive = false;')) {
    throw new Error('ordinary Start must not clear sticky THGuiRep::mRepStatus; Replay State(1) owns that reset');
}
requireText(src.practice, 'Module.eaglerOptions.thpracSession = null;',
            'ordinary Start clears TH07 Web live thprac session');
if (src.practice.includes('g_MenuConfig = {};'))
    throw new Error('ordinary Start must preserve TH07 remembered THGuiPrac widget state');
requireText(src.window, 'chainRes = g_Chain.RunCalcChain();',
            'THOverlay current-tick calc consumers');
requireText(src.window, 'PracticeRuntime::UpdateOverlay();',
            'THOverlay post-calc 60 Hz update-loop ownership');
requireText(src.window, 'g_AnmManager->Flush();\n#ifdef TH_ENABLE_THPRAC\n    // THOverlay / Tracker / THAdvOptWnd',
            'THOverlay draw ownership before ImGui frame end');
requireText(src.window, 'PracticeRuntime::DrawOverlay();\n    if (ThpracImGui::IsFrameOpen())\n        ThpracImGui::EndFrame();',
            'THOverlay draw before ImGui EndFrame');
requireText(src.window, 'ThpracImGui::BeginFrame(static_cast<f32>(targetDt));', 'th07_update same-process backend');
requireText(src.gles, 'RenderImGui(const ImDrawData *drawData)', 'th07_render same-process backend');
requireText(src.gameH, 'i32 CheckGameIntegrity()\n    {\n        return 0;', 'th07_disable_dataver anti-tamper structural bypass');

// th07_fake_shot is driven by the mechanically generated ECL PatchContext,
// not by a second hand-maintained Stage4 table.
requireText(src.fullPatch, 'context.fakeShot = 0;', 'th07_fake_shot Lunasa patch context');
requireText(src.fullPatch, 'context.fakeShot = 4;', 'th07_fake_shot Merlin patch context');
requireText(src.fullPatch, 'context.fakeShot = 2;', 'th07_fake_shot Lyrica patch context');
requireText(src.adapter, 'extern "C" std::int32_t ThpracPortableTh07FakeShot()', 'th07_fake_shot adapter bridge');
requireText(src.ecl, 'return PracticeRuntime::EffectivePlayerShot(g_GameManager.shotTypeAndCharacter);',
            'th07_fake_shot ECL_VAR_PLAYER_SHOTTYPE consumer');

// Replay pause ownership and Stage6 BGM hooks are kept at their original
// semantic boundaries rather than being folded into a broad "practice mode"
// special case.
requireText(src.practice, 'if (g_ReplayPlaybackActive)\n        return;', 'th07_unpause_prevent_desync THGuiRep::mRepStatus bypass');
requireText(src.practice, 'g_CurFrameRawInput &= ~TH_BUTTON_SHOOT;', 'th07_unpause_prevent_desync raw shoot clear');
requireText(src.practice, 'g_CurFrameGameInput &= ~TH_BUTTON_SHOOT;', 'th07_unpause_prevent_desync game shoot clear');
requireText(src.game, 'PracticeRuntime::AdvancedSectionActive() ||\n        g_GameManager.currentStage != 6 || g_Gui.frameCounter >= 300',
            'th07_bgm_st6_2 Pause Stage6 advanced-section bypass');
requireText(src.gui, '!PracticeRuntime::AdvancedSectionActive() &&\n        g_GameManager.currentStage == 6 && arg->frameCounter == 300',
            'th07_bgm_st6_1 delayed Stage6 road BGM suppression');
requireText(src.mainMenu, 'if (0x7fffffff < this->demoFramesCount)', 'th07_disable_demo INT_MAX threshold');
requireText(src.practice, 'if (g_Config.stage == 5 && g_Config.section == 60)\n                return 2;',
            'th07_bgm Resurrection Butterfly slot 2');
requireText(src.game, 'g_Supervisor.PlayLoadedAudio(PracticeRuntime::InitialBgmIndex());',
            'th07_bgm initial road/boss/special slot consumer');
requireText(src.gui, 'if (!PracticeRuntime::SuppressStageIntroTitles())\n        g_AnmManager->ExecuteVmsAnms(this->impl->vms1, 2048, 5);',
            'th07_disable_title exact vms1 2048x5 call suppression');

// th07_rb is a one-shot hook armed only by Stage6 Boss10 / Resurrection
// Butterfly. The address-tagged decomp places 0x4157F3 in ECL_SPAWN_ENEMY_ABS;
// upstream sets ECL time=0x1e0, skips that one spawn, then self-disables.
requireText(src.practice,
    'g_ResurrectionButterflySpawnSkipPending = (g_Config.stage == 5 && g_Config.section == 60);',
    'th07_rb Stage6 Boss10 one-shot arm');
requireText(src.ecl, 'if (PracticeRuntime::ConsumeResurrectionButterflySpawnSkip())',
    'th07_rb ECL_SPAWN_ENEMY_ABS consumer');
requireText(src.ecl, 'enemy->currentContext.time.current = 0x1e0;',
    'th07_rb ECL time rewrite');

// THOverlay is mandatory. Each hotkey must reach the same semantic owner as
// the upstream patch rather than merely toggling a UI boolean.
for (const [source, anchor, label] of [
    [src.practice, 'SDL_SCANCODE_BACKSPACE', 'THOverlay Backspace menu'],
    [src.practice, 'SDL_SCANCODE_F1', 'THOverlay F1 hotkey'],
    [src.practice, 'SDL_SCANCODE_F2', 'THOverlay F2 hotkey'],
    [src.practice, 'SDL_SCANCODE_F3', 'THOverlay F3 hotkey'],
    [src.practice, 'SDL_SCANCODE_F4', 'THOverlay F4 hotkey'],
    [src.practice, 'SDL_SCANCODE_F5', 'THOverlay F5 hotkey'],
    [src.practice, 'SDL_SCANCODE_F6', 'THOverlay F6 hotkey'],
    [src.practice, 'SDL_SCANCODE_F7', 'THOverlay F7 hotkey'],
    [src.practice, 'SDL_SCANCODE_TAB', 'THOverlay Tracker hotkey'],
    [src.player, 'PracticeRuntime::OverlayInvincible() ? PLAYER_STATE_INVULNERABLE : PLAYER_STATE_DEAD',
        'THOverlay F1 Player::Die immediate equivalent'],
    [src.player, 'if (!PracticeRuntime::OverlayInfiniteLives())',
        'THOverlay F2 lives decrement equivalent'],
    [src.player, 'if (!PracticeRuntime::OverlayInfiniteBombs())',
        'THOverlay F3 bomb decrement equivalent'],
    [src.player, 'if (!PracticeRuntime::OverlayInfinitePower())',
        'THOverlay F4 power decrement equivalent'],
    [src.ecl, 'if (!PracticeRuntime::OverlayTimeLock() && !enemy->isSurvivalSpellcard)',
        'THOverlay F5 spell bonus decay gate'],
    [src.enemy, 'PracticeRuntime::OverlayTimeLock() && g_GameManager.currentStage == 4',
        'THOverlay F5 Stage4 Lily timeline gate'],
    [src.enemy, '!g_GameManager.isTimeStopped && !PracticeRuntime::OverlayTimeLock()',
        'THOverlay F5 enemy timer gate'],
    [src.player, 'PracticeRuntime::OverlayAutoBomb()\n                                         ? ((g_LastFrameRawInput & TH_BUTTON_BOMB) != 0)',
        'THOverlay F6 last-raw-input bomb test'],
    [src.player, 'g_CurFrameRawInput = TH_BUTTON_BOMB;',
        'THOverlay F6 current-raw-input synthesis'],
    [src.game, 'PracticeRuntime::ResetTracker();', 'th07_enter tracker reset'],
    [src.player, 'PracticeRuntime::RecordBorderBreak();', 'th07_border_break tracker increment'],
    [src.sound, 'PracticeRuntime::FilterAudioCommand(static_cast<i32>(opcode), arg1)',
        'th07_soundplayer_queue_command entry filter'],
    [src.practice, 'if (opcode >= AUDIO_STOP)\n        return true;',
        'THOverlay F7 stop/shutdown/fade/pause suppression'],
]) {
    requireText(source, anchor, label);
}

// Input ownership of these surfaces is also part of the contract. F1..F7 are
// GuiHotKey widgets evaluated only by THOverlay::OnContentUpdate while the Mod
// Menu is open. Backspace's OnPreUpdate refuses to toggle while another ImGui
// item is active. THAdvOptWnd::StaticUpdate follows THOverlay::Update.
const overlayUpdateStart = src.practice.indexOf('void UpdateOverlay()');
const overlayDrawStart = src.practice.indexOf('void DrawOverlay()', overlayUpdateStart);
const overlayUpdateBody = src.practice.slice(overlayUpdateStart, overlayDrawStart);
const overlayDrawEnd = src.practice.indexOf('\nvoid NotifyBorderBreak()', overlayDrawStart);
const overlayDrawBody = src.practice.slice(overlayDrawStart, overlayDrawEnd);
if (!upstream.includes('if (mMenu(false) && !ImGui::IsAnyItemActive())') ||
    !overlayUpdateBody.includes('g_ModMenuToggleRequested = true;') ||
    overlayUpdateBody.includes('g_Overlay.invincible = !g_Overlay.invincible') ||
    !overlayDrawBody.includes('if (!ImGui::IsAnyItemActive())') ||
    !overlayDrawBody.includes('OverlayKeyPressed(1, SDL_SCANCODE_F1)') ||
    !overlayDrawBody.includes('OverlayKeyPressed(7, SDL_SCANCODE_F7)') ||
    !overlayUpdateBody.includes('g_AdvancedMenuToggleRequested = true;') ||
    !overlayDrawBody.includes('g_AdvancedOptions.menuOpen = !g_AdvancedOptions.menuOpen;')) {
    throw new Error('TH07 THOverlay/THAdvOpt input ownership drifted from upstream OnPreUpdate/OnContentUpdate/StaticUpdate order');
}

// THAdvOptWnd is also mandatory. F12 owns the window; GameSpeed preserves the
// upstream no-vpatch/no-OILP unavailable state; Gameplay owns both exact
// all-clear gates and all three spell-bonus display fixes; About remains a
// real visible surface rather than disappearing from the port.
for (const [source, anchor, label] of [
    [src.practice, 'SDL_SCANCODE_F12', 'THAdvOptWnd F12 hotkey'],
    [src.practice, 'TextId::GameSpeed', 'THAdvOptWnd GameSpeed group'],
    [src.practice, 'openinputlagpatch/vpatch', 'THAdvOptWnd upstream unsupported GameSpeed state'],
    [src.practice, 'g_AdvancedOptions.allClearBonus', 'THAdvOptWnd Gameplay all-clear option'],
    [src.practice, 'g_AdvancedOptions.fixSpellBonusDisplay', 'THAdvOptWnd Gameplay spell-display option'],
    [src.practice, 'thprac v2.3.0.3', 'THAdvOptWnd About version'],
    [src.practice, 'github.com/touhouworldcup/thprac', 'THAdvOptWnd About website'],
    [src.practice, 'THPrac::Gui::ShowLicenceInfo();', 'THAdvOptWnd full upstream license surface'],
    [src.gui, '(!g_GameManager.practice || PracticeRuntime::AdvancedAllClearBonus())',
        'th07_all_clear_bonus_1/2 typed gates'],
    [src.gui, 'digitDivisor = fixSpellBonusDisplay ? 100000000 : 10000000;',
        'th07_spell_bonus_display_fix1 divisor'],
    [src.gui, 'for (i = 0; i < (fixSpellBonusDisplay ? 9 : 8); i++)',
        'th07_spell_bonus_display_fix2 digit count'],
    [src.gui, 'this->impl->captureBonusVm.pos.x -= 7.0f;',
        'th07_spell_bonus_display_fix3 x offset'],
]) {
    requireText(source, anchor, label);
}

// THGuiRep ownership is three distinct states: reset when entering Replay,
// inspect selected metadata without touching live thPracParam, then activate
// replay ownership and copy the candidate only on final acceptance.
requireText(src.mainMenu, 'PracticeRuntime::ReplayMenuReset();', 'th07_rep_menu_1 State(1)');
requireText(src.mainMenu, 'PracticeRuntime::ReplayMenuCheck(this->replayFilenames[this->chosenReplay]);',
            'th07_rep_menu_2 State(2)');
requireText(src.mainMenu, 'PracticeRuntime::ReplayMenuActivate();', 'th07_rep_menu_3 State(3)');
requireText(src.practice, 'const Config saved = g_Config;', 'THGuiRep candidate does not mutate live config');
requireText(src.practice, 'g_Config = saved;', 'THGuiRep candidate live-config restoration');
requireText(src.practice, 'g_ReplayPlaybackActive = true;', 'THGuiRep accepted replay ownership');
requireText(src.practice, 'g_ReplayStartupCommitted = true;', 'THGuiRep portable one-shot startup bridge');
requireText(src.game, 'if (arg->replay && !PracticeRuntime::ReplayStartupCommitted())',
            'THGuiRep accepted metadata survives GameManager init without double load');
requireText(src.game, 'PracticeRuntime::FinishReplayStartup();',
            'THGuiRep portable startup bridge one-shot consume');

const ascii = read('th07-eagler/src/AsciiManager.cpp');
requireText(ascii, 'PracticeRuntime::FilterUnpauseInput();', 'th07_unpause_prevent_desync resume boundary');
requireText(ascii, 'if (PracticeRuntime::AdvancedSectionActive() ||\n                g_GameManager.currentStage != 6 || g_Gui.frameCounter >= 300)',
            'th07_bgm_st6_3 UnPause Stage6 advanced-section bypass');

// DirectInput reacquire and Win32 named-mutex hooks have no address-level
// analogue in the SDL3 port. Their original APIs must remain absent rather
// than being reintroduced merely to mimic a binary patch site.
if (src.controller.includes('DirectInput') || src.controller.includes('Acquire()'))
    throw new Error('th07_reacquire_input: portable unexpectedly reintroduced DirectInput ownership');
if (src.main.includes('CreateMutex') || src.main.includes('OpenMutex'))
    throw new Error('th07_disable_mutex: portable unexpectedly reintroduced Win32 single-instance mutex ownership');

// These formal upstream surfaces are intentionally hard-red until a typed
// equivalent exists.  Keeping this list non-empty is a failing audit, not a
// TODO that can be ignored when declaring TH07 thprac complete.
const missing = [];

const counts = {};
for (const value of category.values()) counts[value] = (counts[value] ?? 0) + 1;
if (missing.length) {
    console.error(`TH07 upstream thprac hook audit INCOMPLETE: ${uniqueHooks.length}/${uniqueHooks.length} hooks classified; ` +
        Object.entries(counts).map(([k, v]) => `${k}=${v}`).join(' '));
    console.error('Mandatory implementations still missing:');
    for (const item of missing) console.error(`- ${item}`);
    process.exit(1);
}

// The consumer audit above proves that every upstream named surface still has
// a typed portable owner.  Also require the independent address/decomp audit so
// anonymous F1-F6 PATCH_HK/EHOOK_HK sites cannot fall outside the 34-name
// inventory unnoticed.
execFileSync(process.execPath, [path.join(here, 'thprac-address-map.mjs')], {
    cwd: project,
    stdio: 'inherit',
});

console.log(`TH07 upstream thprac hook audit PASS: ${uniqueHooks.length}/${uniqueHooks.length} hooks classified and implemented; ` +
    Object.entries(counts).map(([k, v]) => `${k}=${v}`).join(' '));
