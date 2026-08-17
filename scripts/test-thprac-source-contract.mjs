import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const here = path.dirname(fileURLToPath(import.meta.url));
const project = path.resolve(here, '..');
const workspace = path.resolve(project, '..');

function read(rel) {
    return fs.readFileSync(path.join(workspace, rel), 'utf8').replaceAll('\r\n', '\n');
}

const upstreamLocale = read('thprac-reallyportable/thprac/src/thprac/thprac_gui_locale.cpp');
const upstreamConfig = read('thprac-reallyportable/thprac/src/thprac/thprac_cfg.h');
const upstreamTh06 = read('thprac-reallyportable/thprac/src/thprac/thprac_th06.cpp');
const upstreamGui = read('thprac-reallyportable/thprac/src/thprac/thprac_gui_components.cpp');
const catalog = read('thprac-reallyportable/portable/generated/section_catalog.hpp');
const portableImGui = read('th06-eagler/src/ThpracImGui.cpp');
const portablePractice = read('th06-eagler/src/PracticeRuntime.cpp');
const portableShell = read('th06-eagler/resources/shell.html');
const portableAttach = read('thprac-reallyportable/portable/cmake/AttachReallyportable.cmake');
const portableAdapter = read('thprac-reallyportable/portable/adapters/th06/adapter.cpp');
const portableResult = read('th06-eagler/src/ResultScreen.cpp');
const portableAscii = read('th06-eagler/src/AsciiManager.cpp');
const portableMainMenu = read('th06-eagler/src/MainMenu.cpp');
const portableReplay = read('th06-eagler/src/ReplayManager.cpp');

// Web thprac must be the same portable trainer core as desktop, not merely the
// ImGui/Backspace shell. The adapter owns ECL patching and re-reads the host
// session on Emscripten, so the host round-trip must retain the complete menu
// identity (including difficulty and shot type).
if (!portableAttach.includes('THPRAC_PORTABLE_ENABLED=1') ||
    !portableAttach.includes('/adapters/th06/adapter.cpp') ||
    !portableAdapter.includes('Module.eaglerOptions?.thpracSession') ||
    !portableAdapter.includes('ApplySection(ecl, g_Context, g_Session, g_Session.section)') ||
    !portablePractice.includes('\\\"difficulty\\\":%d,\\\"shotType\\\":%d}}') ||
    !portablePractice.includes('g_Config.fakeType, g_MenuDifficulty, g_MenuShotType);')) {
    throw new Error('Portable TH06 Web session/adapter contract is incomplete');
}
if (!portableShell.includes('thpracLocale: options.thpracLocale || "en-US"') ||
    !portableImGui.includes("Module.eaglerOptions?.thpracLocale") ||
    !portableImGui.includes("value === 'zh-CN'") ||
    !portableImGui.includes("value === 'ja-JP'")) {
    throw new Error('Portable TH06 Web thprac locale must be explicitly owned by the host language selection');
}

// ReplaySaveParam writes the multi-character constant 'CARP'. On little-endian
// x86 that DWORD is stored as the byte sequence "PRAC". Portable must match
// the actual file bytes, not the source spelling of the integer literal.
if (!upstreamTh06.includes('ReplaySaveParam(mb_to_utf16(rep_name, 932).c_str(), thPracParam.GetJson());') ||
    !read('thprac-reallyportable/thprac/src/thprac/thprac_games.cpp').includes("*(int32_t*)((int)paramBuf + paramSize + 4) = 'CARP';") ||
    !portablePractice.includes('std::memcmp(bytes + size - 4, "PRAC", 4)') ||
    !portablePractice.includes('std::memcpy(bytes.data() + sizeOffset + 4, "PRAC", 4)') ||
    !portableReplay.includes('std::memcmp(bytes + fileSize - 4, "PRAC", 4)') ||
    portablePractice.includes('std::memcmp(bytes + size - 4, "CARP", 4)') ||
    portablePractice.includes('std::memcpy(bytes.data() + sizeOffset + 4, "CARP", 4)') ||
    portableReplay.includes('std::memcmp(bytes + fileSize - 4, "CARP", 4)')) {
    throw new Error('Portable TH06 replay metadata trailer must use upstream on-disk PRAC bytes');
}

// THGuiPrac's Practice entry hook is unconditional once TH06 reaches the
// Practice stage-selection callsite.  GAME_MANAGER->isInReplay can remain set
// after returning from Replay until a new game actually starts, so using that
// stale flag to gate the Practice menu causes the first post-Replay Practice
// attempt to fall through to vanilla stage select. Replay ownership belongs to
// gameplay Pause/Result hooks, not to PracticeRuntime::Enabled().
if (!upstreamTh06.includes('EHOOK_DY(th06_prac_menu_1, 0x437179, 7, {') ||
    !upstreamTh06.includes('THGuiPrac::singleton().State(1);') ||
    !portablePractice.includes('return g_GameManager.isInPracticeMode != 0;') ||
    portablePractice.includes('return g_GameManager.isInPracticeMode != 0 && g_GameManager.isInReplay == 0;')) {
    throw new Error('Portable TH06 Practice entry must not be gated by stale Replay state');
}

if (!upstreamConfig.includes('bool render_only_used_glyphs = false;') ||
    !upstreamLocale.includes('if (gSettings.render_only_used_glyphs)') ||
    !upstreamLocale.includes('io.Fonts->GetGlyphRangesChineseFull();')) {
    throw new Error('Upstream thprac v2.3.0.3 default Chinese glyph-range contract drifted');
}
if (!portableImGui.includes('glyphRange = io.Fonts->GetGlyphRangesChineseFull();') ||
    portableImGui.includes('glyphRange = io.Fonts->GetGlyphRangesChineseSimplifiedCommon();') ||
    !portableImGui.includes('font->FindGlyphNoFallback(static_cast<ImWchar>(codepoint))') ||
    !portableImGui.includes('for (const auto &entry : thprac::portable::generated::th06SectionLabels)')) {
    throw new Error('Portable TH06 must use the full Chinese atlas and self-test every shipped Chinese section label');
}

// Original TH06 thprac selects section IDs from stage/warp arrays directly;
// mDiffculty is only used to select the visible string table.
for (const anchor of [
    'th_sections_cba[*mStage + st][*mWarp - 2]',
    'th_sections_cbt[*mStage + st][*mWarp - 4]',
    'th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]',
]) {
    if (!upstreamTh06.includes(anchor))
        throw new Error(`Upstream TH06 section-selection contract missing: ${anchor}`);
}

if (!portablePractice.includes('BuildSectionMatchesFor(i32 stage, i32 warp, i32 fakeType, i32 menuDifficulty') ||
    !portablePractice.includes('if (entry.stage != sectionStage)') ||
    !portablePractice.includes('if (candidate == nullptr)') ||
    !portablePractice.includes('else if (!(candidate->difficultyMask & difficultyBit) && (entry.difficultyMask & difficultyBit))') ||
    !portablePractice.includes('keep the first row so the section ID remains available') ||
    portablePractice.includes('entry.stage != sectionStage || !(entry.difficultyMask & difficultyBit)')) {
    throw new Error('Portable TH06 section matcher regressed to difficulty-as-availability filtering');
}

const th06Start = catalog.indexOf('inline constexpr SectionLabel th06SectionLabels[] = {');
const th06End = catalog.indexOf('inline constexpr std::size_t th06SectionLabelCount', th06Start);
if (th06Start < 0 || th06End < 0)
    throw new Error('Generated TH06 section catalogue boundary is missing');
const th06Catalog = catalog.slice(th06Start, th06End);
const rowPattern = /^\s*\{(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(0x[0-9a-f]+),/gmi;
const rows = [];
for (const match of th06Catalog.matchAll(rowPattern)) {
    rows.push({
        id: Number(match[1]),
        patchId: Number(match[2]),
        stage: Number(match[3]),
        bgm: Number(match[4]),
        spell: Number(match[5]),
        difficultyMask: Number.parseInt(match[6], 16),
    });
}
const uniquePatchIds = entries => [...new Set(entries.map(entry => entry.patchId))];
const extra = rows.filter(entry => entry.stage === 6);
const extraIds = uniquePatchIds(extra);
if (extraIds.length !== 21 || extraIds[0] !== 50 || extraIds.at(-1) !== 70 ||
    extra.some(entry => entry.difficultyMask !== 0x10)) {
    throw new Error('Generated TH06 Extra catalogue must contain patch IDs 50..70 at Extra difficulty');
}
const counts = {
    midboss: uniquePatchIds(extra.filter(entry => entry.bgm === 0)).length,
    boss: uniquePatchIds(extra.filter(entry => entry.bgm === 1)).length,
    nonspell: uniquePatchIds(extra.filter(entry => entry.spell === 0)).length,
    spell: uniquePatchIds(extra.filter(entry => entry.spell === 1)).length,
};
if (counts.midboss !== 3 || counts.boss !== 18 || counts.nonspell !== 8 || counts.spell !== 13)
    throw new Error(`Generated TH06 Extra section cardinality drifted: ${JSON.stringify(counts)}`);

const patchNine = rows.filter(entry => entry.stage === 1 && entry.patchId === 9);
if (patchNine.length !== 2 || patchNine[0].difficultyMask !== 0x3 || patchNine[1].difficultyMask !== 0xc)
    throw new Error('TH06 stage-2 patch 9 difficulty-label fixture drifted');

if (!portablePractice.includes('{2, 3, 50, 52}') ||
    !portablePractice.includes('{3, 18, 53, 70}') ||
    !portablePractice.includes('{4, 8, 53, 67}') ||
    !portablePractice.includes('{5, 13, 50, 70}') ||
    !portablePractice.includes('TH06 thprac Extra section UI: entryDifficulty=%d warp=%d count=%d first=%d last=%d')) {
    throw new Error('Portable TH06 Extra runtime/self-test evidence anchors drifted');
}

// GameGuiWnd owns the item-width stack inside the actual ImGui window.  The
// portable bridge once popped it after End(), where GetCurrentWindow() became
// Debug##Default and marked that implicit fallback WriteAccessed every frame.
// Because THGuiPrac uses NoBringToFrontOnFocus, the active fallback could then
// stay above the visible practice panel and steal every mouse hover/click.
const upstreamBegin = upstreamGui.indexOf('ImGui::Begin(mTitle.c_str(), nullptr, mWndFlag);');
const upstreamPushWidth = upstreamGui.indexOf('ImGui::PushItemWidth(mItemWidth);', upstreamBegin);
const upstreamContent = upstreamGui.indexOf('OnContentUpdate();', upstreamPushWidth);
const upstreamPopWidth = upstreamGui.indexOf('ImGui::PopItemWidth();', upstreamContent);
const upstreamEnd = upstreamGui.indexOf('ImGui::End();', upstreamPopWidth);
if ([upstreamBegin, upstreamPushWidth, upstreamContent, upstreamPopWidth, upstreamEnd].some(index => index < 0) ||
    !(upstreamBegin < upstreamPushWidth && upstreamPushWidth < upstreamContent &&
      upstreamContent < upstreamPopWidth && upstreamPopWidth < upstreamEnd)) {
    throw new Error('Upstream GameGuiWnd Begin/PushItemWidth/content/PopItemWidth/End order drifted');
}

function assertPortableItemWidthScope(functionName, beginAnchor) {
    const start = portablePractice.indexOf(functionName);
    const nextFunction = portablePractice.indexOf('\nvoid ', start + functionName.length);
    if (start < 0)
        throw new Error(`Portable function missing: ${functionName}`);
    const body = portablePractice.slice(start, nextFunction < 0 ? portablePractice.length : nextFunction);
    const begin = body.indexOf(beginAnchor);
    const push = body.indexOf('ImGui::PushItemWidth(', begin);
    const pop = body.indexOf('ImGui::PopItemWidth();', push);
    const end = body.indexOf('ImGui::End();', pop);
    if ([begin, push, pop, end].some(index => index < 0) || !(begin < push && push < pop && pop < end))
        throw new Error(`${functionName} must keep Push/PopItemWidth inside Begin/End`);
}
assertPortableItemWidthScope('void DrawPracticeMenu()', 'ImGui::Begin(ThpracImGui::Text(ThpracImGui::TextId::Menu)');
assertPortableItemWidthScope('void DrawPauseMenuPanel()', 'ImGui::Begin("Pause Menu###thprac-pause"');

// THGuiPrac keeps hidden advanced settings alive when Mode toggles, but has
// narrower reset rules for Stage/Warp/Section.  In particular, Warp resets
// section/chapter/phase/frame; section changes reset phase; Fake Shot does not
// reset mSection; and hiding the dialogue checkbox does not clear mDlg.
const upstreamPracticeMenuStart = upstreamTh06.indexOf('void PracticeMenu(Gui::GuiNavFocus& nav_focus)');
const upstreamPracticeMenuEnd = upstreamTh06.indexOf('\n    protected:', upstreamPracticeMenuStart);
if (upstreamPracticeMenuStart < 0 || upstreamPracticeMenuEnd < 0)
    throw new Error('Upstream TH06 PracticeMenu boundary missing');
const upstreamPracticeMenu = upstreamTh06.slice(upstreamPracticeMenuStart, upstreamPracticeMenuEnd);
for (const anchor of [
    'mMode();',
    'if (mStage())\n                *mSection = *mChapter = 0;',
    'if (mWarp())\n                    *mSection = *mChapter = *mPhase = *mFrame = 0;',
    'mFakeShot();',
]) {
    if (!upstreamPracticeMenu.includes(anchor))
        throw new Error(`Upstream TH06 menu-state lifecycle anchor drifted: ${anchor}`);
}
if (!upstreamTh06.includes('mSection(TH_WARP_SELECT_FRAME[*mWarp]') ||
    !upstreamTh06.includes('*mPhase = 0;')) {
    throw new Error('Upstream TH06 SectionWidget section-change phase reset drifted');
}

const portableControlsStart = portablePractice.indexOf('static void DrawPracticeControls(bool includeActions)');
const portableControlsEnd = portablePractice.indexOf('\nvoid DrawPracticeMenu()', portableControlsStart);
const portableControls = portablePractice.slice(portableControlsStart, portableControlsEnd);
if (!portableControls.includes('GuiCombo(modeLabel, &g_Config.mode, modeSelector, modeItems);') ||
    portableControls.includes('if (modeChanged)') ||
    !portableControls.includes('ResetWarpDependentMenuState();') ||
    !portableControls.includes('CurrentSection(0, false);') ||
    !portableControls.includes('g_Config.phase = 0;') ||
    portablePractice.includes('g_Config.dialogue = selected->dialogue && g_Config.dialogue;')) {
    throw new Error('Portable TH06 hidden-state lifecycle diverged from THGuiPrac');
}

// Chapter remains a normal visible SliderInt.  Its encoded section may seed
// the widget when opening/restoring the menu, but must never overwrite a
// freshly edited g_MenuChapter in the same frame.
const chapterStateStart = portablePractice.indexOf('if (g_Config.warp == 1)');
const chapterStateEnd = portablePractice.indexOf('\n#if defined(THPRAC_PORTABLE_ENABLED)', chapterStateStart);
const chapterState = portablePractice.slice(chapterStateStart, chapterStateEnd);
if (!chapterState.includes('else if (syncFromConfig && g_Config.section >= 10000') ||
    !portableControls.includes('GuiSliderInt(sectionLabel, &g_MenuChapter, 1, limit, chapterStep, format)') ||
    !portableControls.includes('CurrentSection(0, false);') ||
    !portablePractice.includes('bool changed = ImGui::SliderInt(label, value, minimum, maximum, format);') ||
    portablePractice.includes('relativeMouse')) {
    throw new Error('Portable TH06 Chapter slider/input ownership regressed');
}
const warpResetStart = portablePractice.indexOf('static void ResetWarpDependentMenuState()');
const warpResetEnd = portablePractice.indexOf('\nstatic void DrawPracticeControls', warpResetStart);
const warpReset = portablePractice.slice(warpResetStart, warpResetEnd);
for (const anchor of ['g_MenuSectionIndex = 0;', 'g_MenuChapter = 1;', 'g_Config.phase = 0;', 'g_Config.frame = 0;']) {
    if (!warpReset.includes(anchor))
        throw new Error(`Portable TH06 Warp reset missing: ${anchor}`);
}

// THGuiPrac's widgets persist independently from thPracParam. State(3) commits
// after the ordinary restart hook has reset thPracParam; State(4) only closes
// the widget window; State(5) commits over the current runtime and therefore
// preserves conditional dlg/fakeType fields when the newly selected section
// doesn't assign them. Portable must keep the same menu/runtime split rather
// than letting unconfirmed Pause settings mutate the live run.
for (const anchor of [
    'case 3:',
    'thPracParam.bomb = (float)*mBomb;',
    'case 4:',
    'case 5:',
    'if (SectionHasDlg(thPracParam.section))',
    'if (thPracParam.section >= TH06_ST4_BOSS1 && thPracParam.section <= TH06_ST4_BOSS7)',
]) {
    if (!upstreamTh06.includes(anchor))
        throw new Error(`Upstream TH06 State(3/4/5) parameter contract missing: ${anchor}`);
}
for (const anchor of [
    'static Config g_MenuConfig = []',
    'static void StoreWorkingMenuConfig()',
    'static void CommitMenuConfigToRuntime(bool preserveConditionalFields)',
    'const Config previousRuntime = g_Config;',
    'committed.dialogue = preserveConditionalFields ? previousRuntime.dialogue : false;',
    'committed.fakeType = preserveConditionalFields ? previousRuntime.fakeType : 0;',
    'g_PreserveConfigOnFreshStart = true;',
    'g_PreserveConfigOnRestart = true;',
]) {
    if (!portablePractice.includes(anchor))
        throw new Error(`Portable TH06 menu/runtime parameter boundary missing: ${anchor}`);
}
const pauseSettingsDraw = portablePractice.slice(
    portablePractice.indexOf('void DrawPauseMenuPanel()'),
    portablePractice.indexOf('void PrepareStart(', portablePractice.indexOf('void DrawPauseMenuPanel()'))
);
for (const anchor of [
    'const Config runtimeConfig = g_Config;',
    'g_Config = g_MenuConfig;',
    'DrawPracticeControls(false);',
    'StoreWorkingMenuConfig();',
    'g_Config = runtimeConfig;',
]) {
    if (!pauseSettingsDraw.includes(anchor))
        throw new Error(`Portable TH06 Pause settings must edit the remembered menu model only: ${anchor}`);
}

// TH06 upstream contains one internally inconsistent patch_main line assigning
// bombsRemaining from thPracParam.life. The surrounding TH06 State(3/5), JSON
// round-trip, and every adjacent-generation implementation all treat bomb as
// an independent field. Keep the portable runtime faithful to that coherent
// contract instead of reproducing this isolated source typo.
if (!upstreamTh06.includes('GAME_MANAGER->bombsRemaining = (int8_t)thPracParam.life;') ||
    !upstreamTh06.includes('AddJsonValueEx(bomb, (int)bomb);') ||
    !portablePractice.includes('gameManager.bombsRemaining = static_cast<i8>(g_Config.bomb);')) {
    throw new Error('TH06 bomb parameter anomaly/portable independent-bomb contract drifted');
}

// th06_result_screen_create is a disabled-by-default ST hook in upstream.
// Only advanced-practice Pause->Exit enables it, and the hook immediately
// disables itself on the next result-screen construction.  Replay playback
// can restore thPracParam metadata, but that alone must never offer to save a
// replay of the replay.
const resultEnableAnchor = 'th06_result_screen_create.Enable();';
const firstResultEnable = upstreamTh06.indexOf(resultEnableAnchor);
if (firstResultEnable < 0 || upstreamTh06.indexOf(resultEnableAnchor, firstResultEnable + resultEnableAnchor.length) >= 0)
    throw new Error('Upstream TH06 result-screen save hook must have exactly one Enable site');
const upstreamPauseExit = upstreamTh06.slice(Math.max(0, firstResultEnable - 500), firstResultEnable + 200);
if (!upstreamPauseExit.includes('SIGNAL_EXIT') || !upstreamPauseExit.includes('SUPERVISOR_STATE_RESULTSCREEN_FROMGAME'))
    throw new Error('Upstream TH06 result-screen save hook is no longer tied to Pause->Exit');
for (const anchor of [
    'static bool g_ResultReplaySaveRequested = false;',
    'g_ResultReplaySaveRequested = true;',
    'bool ConsumeResultReplaySaveRequest()',
]) {
    if (!portablePractice.includes(anchor))
        throw new Error(`Portable TH06 one-shot result-save contract missing: ${anchor}`);
}
if (!portableResult.includes('PracticeRuntime::ConsumeResultReplaySaveRequest()') ||
    portableResult.includes('PracticeRuntime::Active() && PracticeRuntime::GetConfig().mode == 1')) {
    throw new Error('Portable TH06 ResultScreen regressed to metadata-derived replay-save activation');
}

// A thprac replay restores advanced-practice metadata into the live runtime,
// but it is still a replay.  Upstream THGuiRep marks replay playback
// independently from thPracParam; the thprac pause replacement must therefore
// not steal the vanilla replay ESC menu merely because mode remains advanced.
// Keep update and draw ownership symmetric.
if (!upstreamTh06.includes('if (thPracParam.mode && (GAME_MANAGER->isInReplay == 0))') ||
    !portablePractice.includes('!g_GameManager.isInGameMenu || g_GameManager.isInReplay') ||
    !portablePractice.includes('!g_GameManager.isInGameMenu || g_GameManager.isInReplay)') ||
    !portableAscii.includes('g_GameManager.isInGameMenu && !g_GameManager.isInReplay')) {
    throw new Error('Portable TH06 replay Pause ownership must stay with the vanilla replay menu');
}

// Cancelling THGuiPrac is State(4): close only, then let TH06's own
// PRACTICE_LVL_SELECT return path restore SHOT_SELECT.  That vanilla path
// sends interrupt 13 globally and then explicitly suppresses both VMs for the
// non-selected character in the character (86..) and shot (92..) groups.  A
// previous portable shortcut omitted those two suppression loops, causing the
// two character-selection groups to be drawn on top of each other after
// closing the ImGui practice window.
const cancelStart = portableMainMenu.indexOf('case PracticeRuntime::MenuResult::Cancelled:');
const cancelEnd = portableMainMenu.indexOf('break;', cancelStart);
if (cancelStart < 0 || cancelEnd < 0)
    throw new Error('Portable TH06 Practice cancel branch missing');
const cancelBranch = portableMainMenu.slice(cancelStart, cancelEnd);
for (const anchor of [
    'menu->gameState = STATE_SHOT_SELECT;',
    'menu->vm[i].pendingInterrupt = 13;',
    'vmList = &menu->vm[86];',
    'vmList = &menu->vm[92];',
    'if (i != g_GameManager.character)',
    'vmList[0].pendingInterrupt = 0;',
    'vmList[1].pendingInterrupt = 0;',
]) {
    if (!cancelBranch.includes(anchor))
        throw new Error(`Portable TH06 Practice cancel VM cleanup missing: ${anchor}`);
}

// Natural Practice completion/death uses a second upstream path. The permanent
// th06_preplay_1 byte patch changes vanilla Practice's initial ResultScreen
// state from EXIT (0x11) to WRITING_HIGHSCORE_NAME (0x09), after which the
// stock result flow reaches Stats -> SAVE_REPLAY_QUESTION. th06_preplay_2
// forces a live advanced run into the result path while excluding THGuiRep
// playback. Portable code expresses both branches directly in source.
if (!upstreamTh06.includes('PATCH_DY(th06_preplay_1, 0x42d835, "09")') ||
    !upstreamTh06.includes('thPracParam.mode && !THGuiRep::singleton().mRepStatus') ||
    !upstreamTh06.includes('pCtx->Eip = 0x418f0e;')) {
    throw new Error('Upstream TH06 natural Practice replay-save/preplay contract drifted');
}
if (!portableResult.includes('static ResultScreenState ResolveFromGameResultState(') ||
    !portableResult.includes('if (!isInPracticeMode || (thpracActive && !isInReplay))') ||
    !portableResult.includes('natural Practice enters replay-save result flow')) {
    throw new Error('Portable TH06 natural Practice result no longer enters the replay-save result flow');
}
for (const anchor of [
    'if (g_GameManager.isInPracticeMode)',
    'g_GameManager.guiScore = g_GameManager.score;',
    'g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;',
    'if (g_GameManager.isInReplay)',
    'g_Supervisor.curState = SUPERVISOR_STATE_MAINMENU_REPLAY;',
]) {
    if (!portableAscii.includes(anchor))
        throw new Error(`Portable TH06 retry/preplay boundary missing: ${anchor}`);
}

// Mode=Original does not use THPauseMenu upstream. It leaves the vanilla
// StageMenu Pause UI in control and installs only the escR quick-restart path.
for (const anchor of [
    'if (thPracParam.mode && (GAME_MANAGER->isInReplay == 0))',
    'if (!thPracParam.mode && GAME_MANAGER->isInReplay == 0)',
    "Gui::KeyboardInputGetRaw('R') || (key & 0x124) == 0x124",
    'thRestartFlag_normalGame = true;',
]) {
    if (!upstreamTh06.includes(anchor))
        throw new Error(`Upstream TH06 mode-0 Pause/restart contract missing: ${anchor}`);
}
for (const anchor of [
    'if (g_Config.mode == 0)',
    'IS_PRESSED(TH_BUTTON_R)',
    'IS_PRESSED(TH_BUTTON_SKIP) && IS_PRESSED(TH_BUTTON_FOCUS) && IS_PRESSED(TH_BUTTON_DOWN)',
    'SetConfig(Config {});',
]) {
    if (!portablePractice.includes(anchor))
        throw new Error(`Portable TH06 mode-0 Pause/restart contract missing: ${anchor}`);
}
if (!portableAscii.includes('PracticeRuntime::Active() && PracticeRuntime::GetConfig().mode != 0 &&') ||
    !portableAscii.includes('g_GameManager.isInGameMenu && !g_GameManager.isInReplay'))
    throw new Error('Portable TH06 mode-0 Pause drawing must remain owned by vanilla StageMenu');

// Pause buttons are different from the Practice menu itself: GameGuiWnd still
// exposes only D-pad navigation to ImGui, while GuiButton manually accepts Z
// or Return when the item is focused.  StateOpen's shortcut is physical Esc,
// not the generic TH06 RETURNMENU mask (Esc|X/Bomb), and Esc resumes even from
// the Settings page.
for (const anchor of [
    'bool GuiButton::operator()()',
    'ImGui::IsItemFocused() && InGameInputGetConfirm()',
]) {
    if (!upstreamGui.includes(anchor))
        throw new Error(`Upstream TH06 GuiButton confirm contract missing: ${anchor}`);
}
const upstreamPauseStateStart = upstreamTh06.indexOf('signal StateOpen()');
const upstreamPauseStateEnd = upstreamTh06.indexOf('virtual void OnPreUpdate()', upstreamPauseStateStart);
const upstreamPauseState = upstreamTh06.slice(upstreamPauseStateStart, upstreamPauseStateEnd);
if (!upstreamPauseState.includes('Gui::KeyboardInputGetSingle(VK_ESCAPE)') ||
    !upstreamPauseState.includes('StateResume();')) {
    throw new Error('Upstream TH06 Pause Esc->Resume contract drifted');
}
for (const anchor of [
    'static bool GuiPauseButton(const char *label, const ImVec2 &size)',
    'ImGui::IsItemFocused() && ThpracImGui::InputPressed(TH_BUTTON_SELECTMENU)',
    'g_PauseFrameCounter > 10 && WAS_PRESSED(TH_BUTTON_MENU)',
]) {
    if (!portablePractice.includes(anchor))
        throw new Error(`Portable TH06 Pause input contract missing: ${anchor}`);
}
if (portablePractice.includes('g_PauseFrameCounter > 10 && WAS_PRESSED(TH_BUTTON_RETURNMENU)'))
    throw new Error('Portable TH06 Pause must not treat X/Bomb as the upstream Esc shortcut');

// Directional navigation is sampled on TH06's 60 Hz logic tick.  A held key
// may repeat only on the original eighth-frame cadence; select/cancel/focus
// remain rising-edge inputs.  This prevents one physical Down press from being
// re-applied on every high-refresh presentation frame.
for (const anchor of [
    'io.NavInputs[ImGuiNavInput_DpadUp] = InGameInputGet(VK_UP);',
    'io.NavInputs[ImGuiNavInput_DpadDown] = InGameInputGet(VK_DOWN);',
    'io.NavInputs[ImGuiNavInput_DpadLeft] = InGameInputGet(VK_LEFT);',
    'io.NavInputs[ImGuiNavInput_DpadRight] = InGameInputGet(VK_RIGHT);',
]) {
    if (!upstreamGui.includes(anchor))
        throw new Error(`Upstream GameGuiWnd directional-input anchor missing: ${anchor}`);
}
if (upstreamGui.includes('io.NavInputs[ImGuiNavInput_Activate]') ||
    upstreamGui.includes('io.NavInputs[ImGuiNavInput_Cancel]')) {
    throw new Error('Upstream GameGuiWnd unexpectedly owns Z/X Activate/Cancel');
}
for (const anchor of [
    'BuildGameInputPulse(std::uint16_t buttons, std::uint16_t previousButtons,',
    'TH_BUTTON_UP, TH_BUTTON_DOWN, TH_BUTTON_LEFT, TH_BUTTON_RIGHT',
    '((previousButtons & button) == 0) || eighthFrameRepeat',
    'TH_BUTTON_SELECTMENU, TH_BUTTON_RETURNMENU, TH_BUTTON_FOCUS',
    'io.NavInputs[ImGuiNavInput_Activate] = 0.0f;',
    'io.NavInputs[ImGuiNavInput_Cancel] = 0.0f;',
    'inputOwnershipContract',
    'BuildGameInputPulse(direction | accept, direction | accept, false, true) == 0',
    'afterPress == 1 && afterRelease == 1',
]) {
    if (!portableImGui.includes(anchor))
        throw new Error(`Portable TH06 60 Hz input-pulse contract missing: ${anchor}`);
}

console.log(
    'TH06 thprac source contract PASS: Zh-CN default -> ChineseFull + shipped-glyph coverage; ' +
    'section IDs -> original stage/warp availability, difficulty only selects duplicate label variant; ' +
    'Extra stage -> patch 50..70, Mid/Boss/Nonspell/Spell counts 3/18/8/13; ' +
    'GameGuiWnd item-width scope -> no active Debug##Default hover blocker; ' +
    'Mode/Warp/Section/FakeShot/Dialog hidden-state lifecycle matches THGuiPrac; ' +
    'menu widget state stays separate from committed State(3/5) runtime params; ' +
    'ImGui owns directions only while vanilla Practice owns Z/X; Pause focused Z/Enter + Esc shortcut match GuiButton/StateOpen; ' +
    'Pause->Exit Replay-save hook is one-shot while natural Practice follows preplay Stats->save flow; ' +
    'Mode=Original keeps vanilla Pause plus upstream escR restart; ' +
    '60 Hz directional pulse contract retained'
);
