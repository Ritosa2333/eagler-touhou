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
const upstreamGames = read('thprac-reallyportable/thprac/src/thprac/thprac_games.cpp');
const catalog = read('thprac-reallyportable/portable/generated/section_catalog.hpp');
const portableImGui = read('th06-eagler/src/ThpracImGui.cpp');
const portablePractice = read('th06-eagler/src/PracticeRuntime.cpp');
const portableShell = read('th06-eagler/resources/shell.html');
const portableAttach = read('thprac-reallyportable/portable/cmake/AttachReallyportable.cmake');
const portableAdapter = read('thprac-reallyportable/portable/adapters/th06/adapter.cpp');
const portableSession = read('thprac-reallyportable/portable/src/session.cpp');
const portableResult = read('th06-eagler/src/ResultScreen.cpp');
const portableAscii = read('th06-eagler/src/AsciiManager.cpp');
const portableMainMenu = read('th06-eagler/src/MainMenu.cpp');
const portableReplay = read('th06-eagler/src/ReplayManager.cpp');
const portablePlayer = read('th06-eagler/src/Player.cpp');
const portableGameWindow = read('th06-eagler/src/GameWindow.cpp');
const portableGles = read('th06-eagler/src/graphics/Gles.cpp');

// THOverlay F6 is a cross-tick raw-input protocol, not a direct "if DEAD then
// bomb" helper.  The first two patches make the normal bomb test consume the
// previous frame Bomb bit; the latter two decrement respawnTimer and write
// Bomb into the current input word so it is observed on the following tick.
for (const anchor of [
    'PATCH_HK(0x428989, "EB1D")',
    'PATCH_HK(0x4289B4, "85D2")',
    'PATCH_HK(0x428A94, "FF89")',
    'PATCH_HK(0x428A9D, "66C70504D9690002")',
]) {
    if (!upstreamTh06.includes(anchor))
        throw new Error(`Upstream TH06 F6 AutoBomb contract missing: ${anchor}`);
}
if (!portablePlayer.includes('PracticeRuntime::OverlayAutoBomb()\n                                     ? ((g_LastFrameInput & TH_BUTTON_BOMB) != 0)') ||
    !portablePlayer.includes('if (PracticeRuntime::OverlayAutoBomb())\n                g_CurFrameInput = TH_BUTTON_BOMB;') ||
    portablePlayer.includes('PracticeRuntime::OverlayAutoBomb() && p->playerState == PLAYER_STATE_DEAD')) {
    throw new Error('Portable TH06 F6 AutoBomb must preserve upstream previous-input -> current-input next-tick ownership');
}

// TH06 Replay candidate semantics are intentionally *not* Reset()+overlay.
// THPracParam::ReadJson() lacks Reset(), so omitted fields preserve mRepParam;
// CheckReplay() only Reset()s mRepParam on a failed load and leaves the sticky
// mParamStatus flag unchanged. Direct standalone loads may still start from an
// all-zero Config, but Replay State(2) must use preserveMissing=true.
const th06ReadJson = upstreamTh06.slice(
    upstreamTh06.indexOf('bool ReadJson(std::string& json)'),
    upstreamTh06.indexOf('std::string GetJson()', upstreamTh06.indexOf('bool ReadJson(std::string& json)')));
if (th06ReadJson.includes('Reset();') ||
    !th06ReadJson.includes('GetJsonValue(mode);') ||
    !portablePractice.includes('ParseConfigJson(json, candidate, true)') ||
    !portablePractice.includes('config.mode = static_cast<i32>(JsonNumber(json, "mode", config.mode));') ||
    !portablePractice.includes('config.life = static_cast<i32>(JsonNumber(json, "life", config.life));')) {
    throw new Error('Portable TH06 Replay candidate parser must preserve upstream non-resetting ReadJson semantics');
}
const th06ReplayCheck = portablePractice.slice(
    portablePractice.indexOf('bool ReplayMenuCheck(const char *replayPath)'),
    portablePractice.indexOf('void ReplayMenuActivate()', portablePractice.indexOf('bool ReplayMenuCheck(const char *replayPath)')));
if (!th06ReplayCheck.includes('g_ReplayParamStatus = true;') ||
    !th06ReplayCheck.includes('g_ReplayCandidate = {};') ||
    th06ReplayCheck.includes('g_ReplayParamStatus = false;')) {
    throw new Error('Portable TH06 Replay State(2) must preserve sticky mParamStatus on candidate failure');
}
const th06ReplayReset = portablePractice.slice(
    portablePractice.indexOf('void ReplayMenuReset()'),
    portablePractice.indexOf('bool ReplayMenuCheck(', portablePractice.indexOf('void ReplayMenuReset()')));
if (!th06ReplayReset.includes('g_ReplayParamStatus = false;') ||
    !th06ReplayReset.includes('g_Config = {};') ||
    th06ReplayReset.includes('g_ReplayCandidate = {};')) {
    throw new Error('Portable TH06 Replay State(1) must reset live/status owners without inventing an mRepParam reset');
}
for (const anchor of [
    'i32 life = 0;',
    'i32 bomb = 0;',
    'i32 power = 0;',
    'i32 rank = 0;',
]) {
    if (!read('th06-eagler/src/PracticeRuntime.hpp').includes(anchor))
        throw new Error(`Portable TH06 Config{} must equal live Reset(): ${anchor}`);
}
for (const anchor of ['config.life = 8;', 'config.bomb = 8;', 'config.power = 128;', 'config.rank = 32;']) {
    if (!portablePractice.includes(anchor))
        throw new Error(`Portable TH06 persistent THGuiPrac default missing: ${anchor}`);
}
if (!portableSession.includes('Number(json, "life", 0)') ||
    !portableSession.includes('Number(json, "mode", 0)') ||
    !portableSession.includes('Number(json, "bomb", 0)') ||
    !portableSession.includes('Number(json, "power", 0)') ||
    !portableSession.includes('Number(json, "rank", 0)')) {
    throw new Error('Portable shared session standalone decode must start from zero; TH06 candidate merging belongs to PracticeRuntime State(2)');
}
for (const anchor of [
    'HostNumber("mode", 0)',
    'HostNumber("life", 0)',
    'HostNumber("bomb", 0)',
    'HostNumber("power", 0)',
    'HostNumber("rank", 0)',
]) {
    if (!portablePractice.includes(anchor))
        throw new Error(`Portable TH06 Web host session fallback must match Reset=0: ${anchor}`);
}

// Web thprac must be the same portable trainer core as desktop, not merely the
// ImGui/Backspace shell. The adapter owns ECL patching and re-reads the host
// session on Emscripten, so the host round-trip must retain the complete menu
// identity (including difficulty and shot type).
if (!portableAttach.includes('THPRAC_PORTABLE_ENABLED=1') ||
    !portableAttach.includes('/adapters/th06/adapter.cpp') ||
    !portableAdapter.includes('Module.eaglerOptions?.thpracSession') ||
    !portableAdapter.includes('ApplySection(ecl, g_Context, g_Session, g_Session.section)') ||
    !portablePractice.includes('\\\"difficulty\\\":%d,\\\"shotType\\\":%d}}') ||
    !portablePractice.includes('g_Config.fakeType, RuntimeDifficulty(), RuntimeShotType());')) {
    throw new Error('Portable TH06 Web session/adapter contract is incomplete');
}

// TH06 live-run ownership must be reset before a fresh THGuiPrac selection.
// Upstream gets that reset from th06_restart (State(1) itself intentionally
// does not Reset), so portable must not leave a previous Web/adapter session
// alive while the new menu is only editing persistent widget state.
const th06OpenPractice = portablePractice.slice(
    portablePractice.indexOf('void OpenPracticeMenu(i32 difficulty, i32 shotType)'),
    portablePractice.indexOf('struct CurrentSectionInfo'));
if (!upstreamTh06.includes('EHOOK_DY(th06_restart, 0x435901, 5, {') ||
    !upstreamTh06.includes('thPracParam.Reset();') ||
    !th06OpenPractice.includes('Module.eaglerOptions.thpracSession = null;') ||
    !th06OpenPractice.includes('ThpracPortableTh06SetSessionJson(nullptr);') ||
    !th06OpenPractice.includes('g_Config = g_MenuConfig;') ||
    th06OpenPractice.indexOf('ThpracPortableTh06SetSessionJson(nullptr);') > th06OpenPractice.indexOf('g_Config = g_MenuConfig;')) {
    throw new Error('Portable TH06 fresh Practice entry must atomically end the previous live host/adapter session before editing persistent widgets');
}
if (!upstreamTh06.includes('mDiffculty = GAME_MANAGER->difficulty;') ||
    !upstreamTh06.includes('mShotType = (int)(GAME_MANAGER->character * 2 + GAME_MANAGER->shotType);') ||
    th06OpenPractice.includes('g_MenuSectionIndex = 0;') ||
    th06OpenPractice.includes('g_MenuChapter = 1;')) {
    throw new Error('Portable TH06 State(1) must preserve persistent section/chapter widgets and update only source-owned difficulty/shot inputs');
}
const th06HostAbsent = portablePractice.slice(
    portablePractice.indexOf('if (!active)\n    {', portablePractice.indexOf('void RefreshFromHost()')),
    portablePractice.indexOf('Config config;', portablePractice.indexOf('void RefreshFromHost()')));
if (!th06HostAbsent.includes('g_Config = {};') ||
    !th06HostAbsent.includes('ThpracPortableTh06SetSessionJson(nullptr);')) {
    throw new Error('Portable TH06 missing-host boundary must clear both C++ and adapter live owners');
}
const th06SetConfig = portablePractice.slice(
    portablePractice.indexOf('void SetConfig(const Config &config)'),
    portablePractice.indexOf('\nvoid RefreshFromHost()', portablePractice.indexOf('void SetConfig(const Config &config)')));
if (!th06SetConfig.includes('if (g_Config.active)') ||
    !th06SetConfig.includes('PublishPortableSession();') ||
    !th06SetConfig.includes('ThpracPortableTh06SetSessionJson(nullptr);')) {
    throw new Error('Portable TH06 inactive Config must be encoded as session absence; the wire schema otherwise reactivates it');
}
const th06NativeReset = portablePractice.slice(
    portablePractice.indexOf('#else\n    // Upstream th06_restart resets thPracParam', portablePractice.indexOf('void RefreshFromHost()')),
    portablePractice.indexOf('#endif\n}', portablePractice.indexOf('#else\n    // Upstream th06_restart resets thPracParam')));
if (!th06NativeReset.includes('g_Config = {};') ||
    !th06NativeReset.includes('ThpracPortableTh06SetSessionJson(nullptr);') ||
    th06NativeReset.includes('PublishPortableSession();')) {
    throw new Error('Portable TH06 native ordinary reset must Reset the full live block and clear adapter ownership');
}

// Replay hook-site semantics must remain explicit rather than collapsing
// THGuiRep State(1/2/3) into GameManager startup. These anchors correspond to
// original hook addresses 0x438262 / 0x4385d5 / 0x438974.
if (!portableMainMenu.includes('PracticeRuntime::ReplayMenuReset();') ||
    !portableMainMenu.includes('PracticeRuntime::ReplayMenuCheck(this->replayFilePaths[this->chosenReplay]);') ||
    !portableMainMenu.includes('PracticeRuntime::ReplayMenuActivate();') ||
    !portablePractice.includes('static bool g_ReplayStartupCommitted = false;') ||
    !portablePractice.includes('g_ReplayPlaybackActive = true;') ||
    !portablePractice.includes('g_ReplayStartupCommitted = true;') ||
    !portablePractice.includes('PublishPortableSession();')) {
    throw new Error('Portable TH06 must preserve THGuiRep State(1/2/3), sticky mRepStatus, and the separate one-shot startup bridge');
}
if (!portablePractice.includes('for (i32 round = 0; round < 3 && repeatedPracticeStable; round++)') ||
    !portablePractice.includes('g_Config.section == 4 && g_Config.life == 5 && g_Config.bomb == 4 &&') ||
    !portablePractice.includes('g_Config.power == 96 && g_PreserveConfigOnFreshStart')) {
    throw new Error('Portable TH06 must retain the focused three-round repeated-Practice lifecycle regression');
}

// Warp is a THGuiPrac widget only in TH06: live THPracParam has no warp member.
// State(3/5) commits CalcSection()/frame, not the selector itself.
const upstreamParamStart = upstreamTh06.indexOf('struct THPracParam {');
const upstreamParamEnd = upstreamTh06.indexOf('THPracParam thPracParam', upstreamParamStart);
if (upstreamTh06.slice(upstreamParamStart, upstreamParamEnd).includes('int32_t warp;') ||
    !portablePractice.includes('committed.warp = 0;')) {
    throw new Error('Portable TH06 live state must not acquire a source-less Warp field from persistent menu state');
}

// Cancel preserves widgets but leaves the live owner at the reset state.
const closeStart = portablePractice.indexOf('static void RequestMenuClose(MenuResult result, bool accept)');
const closeEnd = portablePractice.indexOf('\nMenuResult PollPracticeMenu()', closeStart);
const closeBody = portablePractice.slice(closeStart, closeEnd);
if (!closeBody.includes('StoreWorkingMenuConfig();\n        // State(4)') ||
    !closeBody.includes('g_Config = {};')) {
    throw new Error('Portable TH06 State(4) must preserve widgets while restoring all-zero live thPracParam');
}

// THAdvOptWnd is a formal no-hook-name surface. TH06 contains Game Speed and
// About only; GameplayInit/GameplaySet are empty in upstream.
for (const anchor of [
    'class THAdvOptWnd : public Gui::PPGuiWnd',
    'Gui::GetChordPressed(hotkeys.advanced_menu)',
    'if (BeginOptGroup<TH_GAME_SPEED>())',
    'AboutOpt();',
]) {
    if (!upstreamTh06.includes(anchor))
        throw new Error(`Upstream TH06 THAdvOptWnd contract missing: ${anchor}`);
}
for (const anchor of [
    'SDL_SCANCODE_F12',
    'Advanced Options###th06-thprac-advanced',
    'No openinputlagpatch/vpatch backend is loaded',
    'THPrac::Gui::ShowLicenceInfo();',
]) {
    if (!portablePractice.includes(anchor))
        throw new Error(`Portable TH06 THAdvOptWnd surface missing: ${anchor}`);
}
if (portablePractice.includes('g_AdvancedOptions.allClearBonus') ||
    portablePractice.includes('g_AdvancedOptions.fixSpellBonusDisplay')) {
    throw new Error('Portable TH06 THAdvOptWnd must not import TH07-only Gameplay controls');
}

// THOverlay input ownership: Backspace OnPreUpdate refuses to toggle while an
// earlier ImGui item is active; F1..F7 GuiHotKey operators are called only by
// OnContentUpdate while the Mod Menu is open. Tracker/F12 have separate owners.
const overlayUpdateStart = portablePractice.indexOf('void UpdateOverlay()');
const overlayDrawStart = portablePractice.indexOf('void DrawOverlay()', overlayUpdateStart);
const overlayUpdateBody = portablePractice.slice(overlayUpdateStart, overlayDrawStart);
const overlayDrawEnd = portablePractice.indexOf('\nvoid NotifyBorderBreak()', overlayDrawStart);
const overlayDrawBody = portablePractice.slice(overlayDrawStart, overlayDrawEnd);
if (!upstreamTh06.includes('if (mMenu(false) && !ImGui::IsAnyItemActive())') ||
    !overlayUpdateBody.includes('g_ModMenuToggleRequested = true;') ||
    overlayUpdateBody.includes('g_Overlay.invincible = !g_Overlay.invincible') ||
    !overlayDrawBody.includes('if (!ImGui::IsAnyItemActive())') ||
    !overlayDrawBody.includes('if (g_Overlay.menuOpen)') ||
    !overlayDrawBody.includes('OverlayKeyPressed(1, SDL_SCANCODE_F1)') ||
    !overlayDrawBody.includes('OverlayKeyPressed(7, SDL_SCANCODE_F7)')) {
    throw new Error('Portable TH06 THOverlay must preserve Backspace item-owner guard and F1-F7 open-window sampling');
}

// Common GameGuiEnd has two formal no-hook-name surfaces: Alt+1/2/3 locale
// switching, gated by !IsAnyItemActive(), and Home screenshot in th06_render.
for (const anchor of [
    'if (!ImGui::IsAnyItemActive())',
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
    'SDL_SCANCODE_LALT',
    'OverlayKeyPressed(10, SDL_SCANCODE_1)',
    'RequestLocale(ThpracImGui::Locale::JaJP)',
    'OverlayKeyPressed(11, SDL_SCANCODE_2)',
    'RequestLocale(ThpracImGui::Locale::ZhCN)',
    'OverlayKeyPressed(12, SDL_SCANCODE_3)',
    'RequestLocale(ThpracImGui::Locale::EnUS)',
]) {
    if (!portablePractice.includes(anchor))
        throw new Error(`Portable TH06 GameGuiEnd locale surface missing: ${anchor}`);
}
if (!portableImGui.includes('void RequestLocale(Locale locale)') ||
    !portableImGui.includes('if (g_LocaleChangePending)') ||
    !portableImGui.includes('BuildLocaleFont(io, g_Locale)') ||
    !portableGles.includes('this->imguiFontTexture != 0 && io.Fonts->TexID == nullptr')) {
    throw new Error('Portable TH06 locale change must defer atlas rebuild to next BeginFrame and invalidate the GLES font texture');
}
if (!upstreamTh06.includes('GameGuiBegin(IMPL_WIN32_DX8, !THAdvOptWnd::singleton().IsOpen());') ||
    !portableGameWindow.includes('ThpracImGui::SetGameNavEnabled(!PracticeRuntime::AdvancedOptionsOpen());') ||
    !portableImGui.includes('g_GameNavEnabled && (g_GameButtons & TH_BUTTON_UP)') ||
    !portablePractice.includes('bool AdvancedOptionsOpen()')) {
    throw new Error('Portable TH06 GameGuiBegin must disable background game navigation while THAdvOptWnd is open');
}
if (!upstreamGames.includes('if (draw_cursor && Gui::ImplWin32CheckFullScreen())') ||
    !upstreamGames.includes('io.MouseDrawCursor = true;') ||
    !upstreamTh06.includes('GameGuiEnd(THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen() || THPauseMenu::singleton().IsOpen());') ||
    !portablePractice.includes('ImGui::GetIO().MouseDrawCursor = fullscreen &&') ||
    !portablePractice.includes('(g_AdvancedOptions.menuOpen || g_MenuOpen || pauseCursor);')) {
    throw new Error('Portable TH06 GameGuiEnd must preserve fullscreen software-cursor ownership for AdvOpt/Practice/Pause');
}
if (!upstreamTh06.includes('Gui::GetChordPressed(hotkeys.screenshot)') ||
    !upstreamTh06.includes('THSnapshot::Snapshot(SUPERVISOR->d3dDevice);') ||
    !portablePractice.includes('OverlayKeyPressed(13, SDL_SCANCODE_HOME)') ||
    !portablePractice.includes('bool ConsumeScreenshotRequest()') ||
    !portableGameWindow.includes('SaveThpracSnapshot()') ||
    !portableGameWindow.includes('SDL_SaveBMP(surface, FileSystem::GetPrefPath(relativePath).c_str())') ||
    !portableGameWindow.includes('if (PracticeRuntime::ConsumeScreenshotRequest())')) {
    throw new Error('Portable TH06 must implement the formal Home full-backbuffer screenshot surface');
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
    !portableReplay.includes('const size_t baseSize = ReplayExtension::BaseFileSize(bytes, static_cast<size_t>(fileSize));') ||
    !portableReplay.includes('std::memcmp(bytes + baseSize - 4, "PRAC", 4)') ||
    portablePractice.includes('std::memcmp(bytes + size - 4, "CARP", 4)') ||
    portablePractice.includes('std::memcpy(bytes.data() + sizeOffset + 4, "CARP", 4)') ||
    portableReplay.includes('std::memcmp(bytes + baseSize - 4, "CARP", 4)')) {
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
const pauseUpdateStart = portablePractice.indexOf('bool UpdatePauseMenu()');
const pauseUpdateEnd = portablePractice.indexOf('\nvoid DrawPauseMenuPanel()', pauseUpdateStart);
const pauseUpdateBody = portablePractice.slice(pauseUpdateStart, pauseUpdateEnd);
const restartFrameOne = pauseUpdateBody.indexOf('if (g_PauseFrameCounter == 1)');
const restartFrameTen = pauseUpdateBody.indexOf('if (g_PauseFrameCounter != 10)', restartFrameOne);
if (restartFrameOne < 0 || restartFrameTen < 0 ||
    !pauseUpdateBody.slice(restartFrameOne, restartFrameTen).includes('CommitRestartWithBgmPolicy();') ||
    !pauseUpdateBody.includes('g_PauseFrameCounter = 0;') ||
    portablePractice.includes('g_PauseActionFrames')) {
    throw new Error('Portable TH06 Restart must use the persistent upstream mFrameCounter: State(5) at frame 1, signal at frame 10, no second action counter');
}
// THPauseMenu::Update() runs from th06_update at the RunCalcChain return
// boundary. Its OnPreUpdate counter continues while the window is closed;
// entering Pause must not initialize a fresh six-frame delay.
const runChain = portableGameWindow.indexOf('const i32 res = g_Chain.RunCalcChain();');
const trainerUpdate = portableGameWindow.indexOf('PracticeRuntime::UpdateOverlay();', runChain);
if (runChain < 0 || trainerUpdate < runChain ||
    !portablePractice.includes('if (g_PauseFrameCounter < 0xffffffffu)\n        ++g_PauseFrameCounter;') ||
    pauseUpdateBody.slice(0, pauseUpdateBody.indexOf('if (g_Config.mode == 0)')).includes('g_PauseFrameCounter = 0;') ||
    pauseUpdateBody.slice(pauseUpdateBody.indexOf('if (!g_PauseWasOpen)'), pauseUpdateBody.indexOf('#ifdef TH_ENABLE_THPRAC', pauseUpdateBody.indexOf('if (!g_PauseWasOpen)')) + 250).includes('g_PauseFrameCounter = 0;')) {
    throw new Error('Portable TH06 THPauseMenu counter/hotkey producer must live at the post-RunCalcChain th06_update boundary and persist across closed/non-paused ticks');
}
const openPracticeStart = portablePractice.indexOf('void OpenPracticeMenu(i32 difficulty, i32 shotType)');
const openPracticeEnd = portablePractice.indexOf('\nstruct CurrentSectionInfo', openPracticeStart);
const openPracticeBody = portablePractice.slice(openPracticeStart, openPracticeEnd);
if (openPracticeBody.includes('RefreshFromHost();') ||
    openPracticeBody.includes('g_MenuConfig = g_Config;')) {
    throw new Error('Portable TH06 State(1) must not import live/replay Web session state into persistent THGuiPrac widgets');
}
const applyInitialStart = portablePractice.indexOf('void ApplyInitialState(GameManager &gameManager, bool applyStats)');
const applyInitialEnd = portablePractice.indexOf('\nstatic double JsonNumber', applyInitialStart);
const applyInitialBody = portablePractice.slice(applyInitialStart, applyInitialEnd);
if (applyInitialBody.includes('ResolveWarpFrame(g_Config.stage, g_Config.warp)')) {
    throw new Error('Portable TH06 must not reinterpret Warp selector Mid/End/Nonspell/Spell/Frame as a chapter portion');
}

// Original TH06 patch_main is at 0x41c17a: after ReplayManager registration,
// immediately before initial BGM, and before the vanilla end-of-callback score
// reset at 0x41c1c1. Preserve that transient ordering; it affects what the
// replay recorder and first gameplay tick observe.
const portableGame = read('th06-eagler/src/GameManager.cpp');
const portableGui = read('th06-eagler/src/Gui.cpp');
const gameAddedStart = portableGame.indexOf('ZunResult GameManager::AddedCallback(GameManager *mgr)');
const gameAddedEnd = portableGame.indexOf('\nZunResult GameManager::DeletedCallback', gameAddedStart);
const gameAdded = portableGame.slice(gameAddedStart, gameAddedEnd);
const recorderPos = gameAdded.indexOf('ReplayManager::RegisterChain(0, "replay/th6_00.rpy")');
const applyPos = gameAdded.indexOf('PracticeRuntime::ApplyInitialState(*mgr, true);');
const bgmPos = gameAdded.indexOf('g_Supervisor.PlayAudio(initialBgm);');
const scoreResetPos = gameAdded.indexOf('mgr->score = 0;', applyPos);
if (recorderPos < 0 || applyPos < recorderPos || bgmPos < applyPos || scoreResetPos < bgmPos ||
    gameAdded.includes('if (!PracticeRuntime::Active())\n        mgr->score = 0;')) {
    throw new Error('Portable TH06 patch_main timing drifted from verified 0x41c17a -> BGM -> 0x41c1c1 score-reset lifecycle');
}

// th06_preplay_2's load/store is already vanilla code; its trainer semantic
// is the EIP jump that bypasses the isInPracticeMode test for live advanced
// practice. Extra advanced practice intentionally has isInPracticeMode=false,
// so losing this jump sends StageEnd down the normal Extra flow.
for (const anchor of [
    'if (thPracParam.mode && !THGuiRep::singleton().mRepStatus)',
    'pCtx->Eip = 0x418f0e;',
]) {
    if (!upstreamTh06.includes(anchor))
        throw new Error(`Upstream TH06 preplay control-flow contract missing: ${anchor}`);
}
if (!portableGui.includes('(PracticeRuntime::AdvancedActive() && !PracticeRuntime::ReplayPlaybackActive())'))
    throw new Error('Portable TH06 StageEnd no longer mirrors th06_preplay_2 live-advanced-practice branch override');

// th06_preplay_1 is an unconditional permanent byte patch on the vanilla
// Practice branch: EXIT (0x11) -> WRITING_HIGHSCORE_NAME (0x09). It therefore
// applies to Mode=Original/vanilla Practice as well as advanced runs.
if (!upstreamTh06.includes('PATCH_DY(th06_preplay_1, 0x42d835, "09")') ||
    !portableResult.includes('return RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME;') ||
    portableResult.includes('ResolveFromGameResultState(false, true, false, false) == RESULT_SCREEN_STATE_EXIT')) {
    throw new Error('Portable TH06 ResultScreen must preserve unconditional th06_preplay_1 Practice routing');
}
if (!upstreamTh06.includes('if (thPracParam.mode)\n            THSaveReplay(rep_name);') ||
    !portablePractice.includes('if (!AdvancedActive() || !replayPath || !*replayPath)')) {
    throw new Error('Portable TH06 replay metadata save must be mode-gated, not merely Active()-gated');
}
if (portablePractice.includes('ReplayUnsafeAssistUsedThisRun') ||
    portableResult.includes('ReplayUnsafeAssistUsedThisRun')) {
    throw new Error('Portable TH06 must not add an assist-based replay-save ban absent from upstream thprac');
}

// TH06_ST6_MID2 selects its health branch from the *current game* character
// and shot type. It is not a THGuiPrac widget field and is not serialized by
// upstream Replay metadata, so adapter/host state must not use g_MenuShotType
// here (that would replay the previous menu's character).
for (const anchor of [
    'shot = GAME_MANAGER->character * 2 + GAME_MANAGER->shotType;',
]) {
    if (!upstreamTh06.includes(anchor))
        throw new Error(`Upstream TH06 runtime-shot contract missing: ${anchor}`);
}
if (!portablePractice.includes('static i32 RuntimeShotType()') ||
    !portablePractice.includes('return g_GameManager.CharacterShotType();') ||
    !portablePractice.includes('static i32 RuntimeDifficulty()') ||
    !portablePractice.includes('return g_GameManager.difficulty;') ||
    /"shotType\\":%d[^\n]*g_MenuShotType/.test(portablePractice)) {
    throw new Error('Portable TH06 adapter/host session shotType must follow current GameManager, not persistent Practice-menu state');
}
const replayRegisterPos = gameAdded.indexOf('ReplayManager::RegisterChain(1, (char *)g_GameManager.replayFile)');
const runtimeSyncPos = gameAdded.indexOf('PracticeRuntime::SyncRuntimeDerivedSession();', replayRegisterPos);
const stageRegisterPos = gameAdded.indexOf('Stage::RegisterChain(mgr->currentStage)', replayRegisterPos);
if (replayRegisterPos < 0 || runtimeSyncPos < replayRegisterPos || stageRegisterPos < runtimeSyncPos) {
    throw new Error('Portable TH06 Replay must republish GameManager-owned shot/difficulty after replay header load and before Stage/ECL load');
}

// Mode ownership is hook-specific in TH06. th06_patch_main calls
// THSectionPatch only inside mode==1, while th06_fake_shot_type and
// th06_patchouli gate on fakeType alone. Hidden widget state makes this
// observable, so do not collapse both rules into AdvancedActive().
for (const anchor of [
    'if (thPracParam.mode == 1)',
    'THSectionPatch();',
    'if (thPracParam.fakeType)',
    '*PLAYER_SHOT = thPracParam.fakeType - 1;',
]) {
    if (!upstreamTh06.includes(anchor))
        throw new Error(`Upstream TH06 mode/fakeType contract missing: ${anchor}`);
}
const effectiveShotStart = portablePractice.indexOf('i32 EffectivePlayerShot(i32 vanillaShot)');
const effectiveShotEnd = portablePractice.indexOf('\nbool ForceFlandreFinalRage()', effectiveShotStart);
const effectiveShotBody = portablePractice.slice(effectiveShotStart, effectiveShotEnd);
if (!effectiveShotBody.includes('if (Active() && g_Config.fakeType != 0)') ||
    effectiveShotBody.includes('AdvancedActive()')) {
    throw new Error('Portable TH06 fakeType hooks must follow live fakeType independently of mode');
}
if (!portableAdapter.includes('g_Session.mode != 1 || g_Session.section == 0') ||
    (portableAdapter.match(/g_Context = \{\};/g) || []).length < 3) {
    throw new Error('Portable TH06 section adapter must gate ECL/runtime section effects on mode==1 and clear stale context otherwise');
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
    !portableResult.includes('(void)isInPracticeMode;') ||
    !portableResult.includes('(void)thpracActive;') ||
    !portableResult.includes('(void)isInReplay;') ||
    !portableResult.includes('return RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME;') ||
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

// The detailed contract above must always be accompanied by the independent
// mechanical upstream hook/surface/state inventory. Never rely on somebody
// remembering to run the second audit manually.
await import('./audit-thprac-upstream-hooks.mjs');

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
