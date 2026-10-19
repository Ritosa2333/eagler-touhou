import fs from "node:fs";
import path from "node:path";
import process from "node:process";
import { createHash } from "node:crypto";

const root = path.resolve(import.meta.dirname, "..", "..", "..");
const read = rel => fs.readFileSync(path.join(root, rel), "utf8");
const requireText = (haystack, needle, label) => {
  if (!haystack.includes(needle)) throw new Error(`missing ${label}: ${needle}`);
};

const globalPatch = read("dependencies/upstream-thcrap-tsa/base_tsa/global.js");
const th07Patch = read("dependencies/upstream-thcrap-tsa/base_tsa/th07.js");
const th07Version = read("dependencies/upstream-thcrap-tsa/base_tsa/th07.v1.00b.js");
const layout = read("dependencies/upstream-thcrap/thcrap_tsa/src/layout.cpp");
const upstreamAscii = read("dependencies/upstream-thcrap/thcrap_tsa/src/ascii.cpp");
const upstreamAsciiHeader = read("dependencies/upstream-thcrap/thcrap_tsa/src/ascii.hpp");
const upstreamAnm = read("dependencies/upstream-thcrap/thcrap_tsa/src/anm.cpp");
const upstreamMusic = read("dependencies/upstream-thcrap/thcrap_tsa/src/music.cpp");
const upstreamStrings = read("dependencies/upstream-thcrap/thcrap/src/strings.cpp");
const upstreamFileBp = read("dependencies/upstream-thcrap/thcrap/src/bp_file.cpp");
const compiler = read("eagler-touhou/server/thcrap-compiler.mjs");
const anm = read("th07-eagler/src/AnmManager.cpp");
const anmHeader = read("th07-eagler/src/AnmManager.hpp");
const asciiManager = read("th07-eagler/src/AsciiManager.cpp");
const controller = read("th07-eagler/src/Controller.cpp");
const helper = read("th07-eagler/src/TextHelper.cpp");
const thpracImGui = read("th07-eagler/src/ThpracImGui.cpp");
const cmake = read("th07-eagler/CMakeLists.txt");
const fileSystem = read("th07-eagler/src/FileSystem.cpp");
const localization = read("th07-eagler/src/Localization.cpp");
const mainMenu = read("th07-eagler/src/MainMenu.cpp");
const bombData = read("th07-eagler/src/BombData.cpp");
const eclManager = read("th07-eagler/src/EclManager.cpp");
const resultScreen = read("th07-eagler/src/ResultScreen.cpp");
const ending = read("th07-eagler/src/Ending.cpp");
const musicRoom = read("th07-eagler/src/MusicRoom.cpp");
const gui = read("th07-eagler/src/Gui.cpp");
const gameWindow = read("th07-eagler/src/GameWindow.cpp");
const gameManagerHeader = read("th07-eagler/src/GameManager.hpp");
const originalGameManagerHeader = read("th07/src/th07/GameManager.hpp");
const supervisor = read("th07-eagler/src/Supervisor.cpp");
const gameError = read("th07-eagler/src/GameErrorContext.cpp");
const originalGui = read("th07/src/th07/Gui.cpp");
const originalEnding = read("th07/src/th07/Ending.cpp");
const originalAsciiManager = read("th07/src/th07/AsciiManager.cpp");
const originalAnmVmHeader = read("th07/src/th07/AnmVm.hpp");
const portableAsciiManagerHeader = read("th07-eagler/src/AsciiManager.hpp");
const unifontBytes = fs.readFileSync(path.join(root, "dependencies/unifont-15.1.05/unifont-15.1.05.otf"));
const unifontSha256 = createHash("sha256").update(unifontBytes).digest("hex");

if (unifontSha256 !== "7b62b50acbb186689dc30c446ce4367b87d79489e9907b83255f9fbe0dcfb9e1") {
  throw new Error(`GNU Unifont 15.1.05 OTF bytes drifted: ${unifontSha256}`);
}
requireText(cmake, "TH_UNIFONT_FILE", "TH07 pinned Unifont build input");
requireText(cmake, "unifont-15.1.05.otf", "TH07 pinned Unifont version");
requireText(cmake, "/unifont.otf", "TH07 runtime Unifont staging name");
requireText(cmake, "msgothic.ttc", "TH07 vanilla MS Gothic resource");
requireText(helper, 'Localization::Active() ? "unifont.otf" : "msgothic.ttc"', "TH07 vanilla/localized font split");
if (helper.includes("NotoSans")) {
  throw new Error("TH07 must not introduce a Noto Sans game-text fallback");
}
const thpracFontBuilder = thpracImGui.slice(
  thpracImGui.indexOf("bool BuildLocaleFont"),
  thpracImGui.indexOf("bool BuildLocaleFont") + 1800);
if (!thpracFontBuilder.includes('FileSystem::GetBasePath("unifont.otf")') ||
    !thpracFontBuilder.includes("AddFontFromFileTTF") || thpracFontBuilder.includes("AddSystemFont")) {
  throw new Error("TH07 thprac ImGui must load the same GNU Unifont OTF instead of a system-font candidate");
}
const originalGameError = read("th07/src/th07/GameErrorContext.cpp");
const main = read("th07-eagler/src/main.cpp");
const stringContract = read("eagler-touhou/server/thcrap-string-contract.mjs");
const screenshot = read("dependencies/upstream-thcrap/thcrap_tsa/src/screenshot.cpp");

requireText(th07Version, '"menu_desc_align": {\n\t\t\t"addr": "Rx546e4"', "TH07 menu_desc_align address");
requireText(th07Version, '"boss_title_align": {\n\t\t\t"addr": "0x4544eb"', "TH07 boss_title_align address");
requireText(globalPatch,
  '"code": "52 ff75ac ff75b0 e8[GetTextExtentForFontID] 8d440010 50 db0424 58 59 89ca 90909090"',
  "menu_desc_align patch code");
requireText(th07Patch,
  '"code": "ff75ac ff75b0 e8[GetTextExtentForFontID] 83c004 50 db0424 58 9090909090"',
  "boss_title_align patch code");
requireText(layout, "return text_extent_full_for_font(str, font) / 2;", "extent game-coordinate division");
requireText(layout,
  "(id * 2) - ((game_id == TH07 || game_id == TH08) ? 2 : 0)",
  "TH07 font ID to CreateFont height formula");

requireText(helper, "const i32 targetCellHeight = fontId * 2 - 2;", "portable font-ID height formula");
requireText(helper, "return width / 2.0f;", "portable measured width coordinate conversion");
requireText(anm, "const f32 measuredWidth = TextHelper::MeasureTextWidth(buf, fontWidth);", "portable measured extent");
requireText(anm, "(measuredWidth + 4.0f)", "portable right-align +4 contract");
requireText(anm, "(measuredWidth + 8.0f) * vm->sprite->cols / 2.0f", "portable center-align +8 contract");
requireText(anm, "if (Localization::Active())", "localization-only alignment branch");
requireText(anm, "TextHelper::GetLogicalStringWidth(buf) * (f32)fontWidth * vm->sprite->cols / 2.0f", "translation-off right baseline");
requireText(anm, "TextHelper::GetLogicalStringWidth(buf) * fontWidth * vm->sprite->cols / 4.0f", "translation-off center baseline");

// thcrap image patches for embedded TH07 ANM sheets are coordinate-space,
// sprite-level patches. A replacement PNG can be smaller than the original
// THTX: sprites outside it remain original and partially covered sprites copy
// only the intersection. Opaque destinations use upstream's additive-alpha
// blend; transparent/mixed destinations use overwrite.
requireText(upstreamAnm, "if(sp.rep_x >= image.img.width || sp.rep_y >= image.img.height)", "upstream outside-patch fallback");
requireText(upstreamAnm, "sp.copy_w = MIN(sprite.w, (image.img.width - sp.rep_x));", "upstream partial-width contract");
requireText(upstreamAnm, "sp.copy_h = MIN(sprite.h, (image.img.height - sp.rep_y));", "upstream partial-height contract");
requireText(upstreamAnm, "if(dst_alpha == SPRITE_ALPHA_OPAQUE)", "upstream opaque-destination blend choice");
requireText(upstreamAnm, "func = blit_blend;", "upstream blend function selection");
requireText(anm, "if (left >= patch->w || top >= patch->h)", "portable outside-patch fallback");
requireText(anm, "const i32 copyWidth = std::min(width, patch->w - left);", "portable partial-width contract");
requireText(anm, "const i32 copyHeight = std::min(height, patch->h - top);", "portable partial-height contract");
requireText(anm, "replacementAlpha == RgbaAlphaState::Empty", "transparent replacement fallback");
requireText(anm, "fallbackSprites++;", "transparent sprite fallback accounting");
requireText(anm, "destinationAlpha == RgbaAlphaState::Opaque", "portable opaque-destination blend choice");
requireText(anm, "BlendRgbaOverOpaque(destinationRow, sourceRow, copyWidth);", "portable upstream-equivalent RGBA blend");
requireText(anm, "destination[3] = static_cast<u8>(std::min<i32>(destination[3] + sourceAlpha, 255));", "portable additive/clamped alpha");
if (anm.includes("if (runtimePatch->w == data->width && runtimePatch->h == data->height)")) {
  throw new Error("embedded ANM patches must not require replacement PNG dimensions to match the original THTX");
}
requireText(anm, "runtimeTextureUsesOwnBounds && data->numSprites == 1", "replacement-only bounds adjustment");

// TH_ENABLE_THCRAP=OFF is the strict Japanese/default baseline even if a
// stale thcrap directory is adjacent to the executable.
requireText(fileSystem, "#ifndef TH_ENABLE_THCRAP", "runtime-override compile gate");
requireText(fileSystem, "(void)filepath;", "THCRAP-off unused filepath handling");
requireText(fileSystem, "#else", "THCRAP-on runtime-override branch");

// base_tsa also installs the TH06-style P-key PNG capture in TH07. This is
// distinct from TH07's original HOME-key BMP snapshot and both must coexist.
requireText(th07Version, '"th06_screenshot": {', "TH07 common P-key snapshot hook");
requireText(th07Version, '"pD3DDevice": "[0x575958]"', "TH07 screenshot D3D device slot");
requireText(th07Version, '"addr": "Rx2feb3"', "TH07 screenshot breakpoint address");
requireText(screenshot, "TH_EXPORT size_t BP_th06_screenshot", "common screenshot handler");
requireText(screenshot, "if ((GetKeyState('P') & 0x8000))", "common screenshot physical P key");
requireText(screenshot, 'wchar_t dir[] = L"snapshot/th000.png";', "common screenshot PNG numbering");
requireText(screenshot, "constexpr size_t width = 640;", "common screenshot width");
requireText(screenshot, "constexpr size_t height = 480;", "common screenshot height");
requireText(gameWindow, "static i32 g_ThcrapSnapshotRequests = 0;", "TH07 pending P snapshot count");
requireText(gameWindow, "keyboard[SDL_SCANCODE_P]", "TH07 60Hz P-key sampling");
requireText(gameWindow, 'sprintf(snapshotPath, "snapshot/th%.3d.png", i);', "TH07 PNG snapshot filename");
requireText(gameWindow, "g_Supervisor.SnapshotPng(snapshotPath);", "TH07 P-key PNG capture call");
requireText(supervisor, "i32 Supervisor::SnapshotPng(const char *param_1)", "TH07 PNG snapshot implementation");
requireText(supervisor, "this->gfxDevice->ReadPixels(0, 0, 640, 480, pixels);", "TH07 PNG 640x480 backbuffer read");
requireText(supervisor, "IMG_SavePNG(surf, outputPath.c_str())", "TH07 PNG encoder");
if (!/chainRes = g_Chain\.RunCalcChain\(\);[^]*?#ifdef TH_ENABLE_THCRAP[^]*?keyboard\[SDL_SCANCODE_P\][^]*?#endif\s*g_SoundPlayer\.ProcessQueues\(\);/m.test(gameWindow)) {
  throw new Error("TH07 P-key snapshot sampling must remain at the fixed 60Hz calc boundary");
}
if (!/#ifdef TH_ENABLE_THCRAP[^]*?g_Supervisor\.SnapshotPng\(snapshotPath\);[^]*?#endif\s*g_Supervisor\.gfxDevice->SwapBuffers\(\);/m.test(gameWindow)) {
  throw new Error("TH07 P-key PNG must capture the completed backbuffer before SwapBuffers");
}
requireText(gameWindow, "if (WAS_PRESSED_RAW(TH_BUTTON_HOME))", "original TH07 HOME screenshot trigger retained");
requireText(gameWindow, 'sprintf(snapshotPath, "snapshot/th%.3d.bmp", i);', "original TH07 HOME BMP numbering retained");
requireText(gameWindow, "g_Supervisor.SnapshotScreen(snapshotPath);", "original TH07 HOME BMP call retained");

// sprintf_call_ebp-208 @ Rx2077 is the original AsciiManager::AddFormatText
// stack formatter. It formats into the function's 0x208-byte local buffer and
// then immediately forwards the result to AddString; ascii_patch_1/2 replace
// that same wrapper with ascii_vpatchf_th07_th08. Portable therefore must not
// grow a second generic strings_vsprintf layer here: AddFormatText already
// routes the original fmt + va_list through the typed EAS1 formatter, including
// recursive %s lookup and the thcrap alignment contract.
requireText(th07Version, '"sprintf_call_ebp-208": {\n\t\t\t"addr": "Rx2077"', "TH07 AddFormatText sprintf hook address");
requireText(globalPatch, '"sprintf_call_ebp-208": {', "global AddFormatText safe sprintf hook");
requireText(globalPatch, '"title": "Safe sprintf (ebp-208)"', "global AddFormatText safe sprintf intent");
requireText(th07Version, '"ascii_patch_1": { "addr": "Rx206c" }', "TH07 ascii patch #1 address");
requireText(th07Version, '"ascii_patch_2": { "addr": "Rx2099" }', "TH07 ascii patch #2 address");
requireText(asciiManager, "void AsciiManager::AddFormatText(AsciiManager *manager, ZunVec3 *pos, const char *fmt, ...)", "portable AddFormatText entry");
requireText(asciiManager, "if (FormatLocalizedAscii(manager, *pos, localizedPos, str, sizeof(str), fmt, args))", "portable typed EAS1 formatter routing");
requireText(asciiManager, "const bool known = active && Localization::LookupAscii(format, entry);", "portable EAS1 fmt lookup");
requireText(asciiManager, "Localization::AsciiString", "portable EAS1 recursive string argument lookup");

// reacquire_input is a DirectInput device-loss recovery fix, not a text hook.
// Original TH07 compares GetDeviceState()'s HRESULT specifically against
// DIERR_INPUTLOST, then calls IDirectInputDevice::Acquire and retries. The
// global patch changes that compare to zero and flips JNE->JE so every failed
// HRESULT enters recovery. Portable has no Acquire/device-loss state machine:
// SDL exposes current keyboard/gamepad state directly on each input poll.
requireText(th07Version, '"reacquire_input": {\n\t\t\t"addr": "Rx30f03"', "TH07 DirectInput reacquire hook address");
requireText(globalPatch, '"title": "Fix input glitching out in TH06/TH07"', "common reacquire-input intent");
requireText(globalPatch, '"code": "00000000 74"', "common reacquire-input compare/jump rewrite");
requireText(controller, "const bool *keys = SDL_GetKeyboardState(NULL);", "portable keyboard state polling");
requireText(controller, "SDL_GetGamepadButton", "portable gamepad button polling");
requireText(controller, "SDL_GetGamepadAxis", "portable gamepad axis polling");
if (/\bAcquire\s*\(/.test(controller) || controller.includes("DIERR_INPUTLOST")) {
  throw new Error("TH07 portable input unexpectedly regained a legacy DirectInput Acquire state machine");
}

// hud_force_redraw @ Rx2b677 replaces the original JNE guarding the HUD
// border/background redraw with an unconditional JMP. Portable keeps the same
// typed gate but forces cfg.redrawEveryFrame=1 during Supervisor setup, so the
// guarded redraw body is always taken without needing an address-level patch.
requireText(th07Version, '"hud_force_redraw": {\n\t\t\t"addr": "Rx2b677"', "TH07 HUD force-redraw hook address");
requireText(globalPatch, '"title": "Redraw the HUD every frame, because we might be drawing TL notes there"', "HUD force-redraw upstream intent");
requireText(globalPatch, '"code": "eb"', "HUD force-redraw unconditional jump");
requireText(gui, "if (g_Supervisor.cfg.redrawEveryFrame || vm->currentInstruction ||", "portable TH07 HUD redraw gate");
requireText(gui, "g_Supervisor.renderSkipFrames != 0)", "portable TH07 HUD redraw fallback conditions");
requireText(supervisor, "this->cfg.redrawEveryFrame = 1;", "portable TH07 unconditional HUD redraw configuration");

// force_disable_vsync @ Rx38887 changes the original cfg.enableVsync JNE into
// an unconditional near jump to CheckVSync's return-0 epilogue. Therefore a
// THCRAP build must skip the whole legacy refresh/timing probe, not merely
// choose a different swap interval after the probe. Strict OFF retains the
// portable SDL probe and its display-refresh fallback.
requireText(th07Version, '"force_disable_vsync": {\n\t\t\t"addr": "Rx38887"', "TH07 force-disable-vsync hook address");
requireText(th07Version, '"code": "e9 f6000000 90"', "TH07 force-disable-vsync jump bytes");
requireText(supervisor, "i32 Supervisor::CheckVSync()", "portable TH07 CheckVSync entry");
if (!/i32 Supervisor::CheckVSync\(\)\s*\{\s*#ifdef TH_ENABLE_THCRAP[^]*?return 0;\s*#endif\s*#ifdef __EMSCRIPTEN__/m.test(supervisor)) {
  throw new Error("TH07 THCRAP CheckVSync must early-return before the legacy/SDL timing probe");
}
requireText(supervisor, "SDL_GL_GetSwapInterval(&swapInterval);", "strict-OFF SDL VSync probe retained");
requireText(supervisor, "g_Supervisor.vsyncEnabled = 1;", "strict-OFF software VSync fallback retained");

// file_name/file_size/file_load/file_loaded are the old generic contiguous-
// file replacement lifecycle. TH07 exposes the basename at Rx313a0, archive
// size at Rx313ab, destination buffer at 0x431420, and post-load completion at
// 0x45fa9b. thcrap uses those four moments to resolve replacement bytes,
// enlarge the game allocation, copy/patch them, then run post-load hooks.
// Portable owns that lifecycle one layer earlier: OpenFile first asks the
// strict THCRAP-gated runtime override namespace for already-materialized
// bytes; the override loader allocates the exact final size and publishes it
// through g_LastFileSize. Message/ending jdiffs are compiled to final game
// bytes by ThcrapRuntimeCompiler before mounting. Therefore reproducing the
// four mutation breakpoints inside PBG4 decompression would double-patch data.
requireText(th07Version, '"file_name": {\n\t\t\t"addr": "Rx313a0"', "TH07 file_name breakpoint address");
requireText(th07Version, '"file_size": {\n\t\t\t"addr": "Rx313ab"', "TH07 file_size breakpoint address");
requireText(th07Version, '"file_load": {\n\t\t\t"addr": "0x431420"', "TH07 file_load breakpoint address");
requireText(th07Version, '"file_loaded": {\n\t\t\t"addr": "0x45fa9b"', "TH07 file_loaded breakpoint address");
requireText(th07Patch, '"file_name": "eax"', "TH07 file_name register contract");
requireText(th07Patch, '"file_size": "eax"', "TH07 file_size register contract");
requireText(th07Patch, '"file_buffer": "eax"', "TH07 file_load buffer contract");
requireText(upstreamFileBp, "fr->rep_buffer = stack_game_file_resolve(fr->name, &fr->pre_json_size);", "upstream replacement resolution");
requireText(upstreamFileBp, "*file_size = POST_JSON_SIZE(fr);", "upstream replacement allocation sizing");
requireText(upstreamFileBp, "memcpy(fr->game_buffer, fr->rep_buffer, fr->pre_json_size);", "upstream replacement copy");
requireText(upstreamFileBp, "file_rep_hooks_run(fr);", "upstream post-load patch hook");
requireText(fileSystem, "if (u8 *overrideData = OpenRuntimeOverride(filepath))", "portable pre-archive override gate");
requireText(fileSystem, "g_LastFileSize = static_cast<u32>(size);", "portable exact override size publish");
requireText(fileSystem, "g_LastFileWasRuntimeOverride = true;", "portable override provenance publish");
const overrideGate = fileSystem.indexOf("if (u8 *overrideData = OpenRuntimeOverride(filepath))");
const archiveGate = fileSystem.indexOf("if (!isExternalResource)");
if (!(overrideGate >= 0 && archiveGate > overrideGate)) {
  throw new Error("TH07 runtime override must precede original PBG4/external file loading");
}
requireText(compiler, "const patched = patchThmsgDump(dumped, parsed);", "portable message jdiff materialization");
requireText(compiler, "const bytes = await this.runner.compileMessage(patched, version);", "portable final message bytes compilation");
requireText(compiler, "bytes: patchEnding(base, parsed)", "portable final ending bytes materialization");

// log_restore replaces two original no-op variadic debug log stubs (Supervisor
// DebugPrint2 at Rx37903 and the old DirectSound helper at Rx5e4f0) with
// thcrap's log_printf. This is diagnostic plumbing, not a localization hook.
// Portable already makes its surviving typed debug logger functional through
// SDL_LogMessageV, while the obsolete DirectSound helper no longer exists.
requireText(th07Version, '"log_restore": {\n\t\t\t"addr": [\n\t\t\t\t"Rx37903",\n\t\t\t\t"Rx5e4f0"', "TH07 two log_restore sites");
requireText(globalPatch, '"code": "e9[log_printf]"', "upstream log_restore trampoline");
requireText(globalPatch, '"title": "Restore the game\'s built-in logging"', "upstream log_restore intent");
requireText(supervisor, "void Supervisor::DebugPrint(const char *fmt, ...)", "portable TH07 debug logger");
requireText(supervisor, "SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, args);", "portable SDL debug log sink");

// antitamper_remove_check @ Rx04fe0 is GameManager::CheckGameIntegrity, the
// runtime checksum predicate called periodically and before protected gameplay
// mutations. On mismatch callers either return an exit code or nuke Supervisor
// state. base_tsa replaces the entire predicate with xor eax,eax; ret. The
// portable/nonmatching code already expresses exactly that semantic result as
// an unconditional return 0, while still retaining checksum regeneration for
// replay/game state bookkeeping.
requireText(th07Version, '"antitamper_remove_check": {\n\t\t\t"addr": "Rx04fe0"', "TH07 anti-tamper hook address");
requireText(globalPatch, '"title": "Don\'t quit the game on an invalid anti-tampering checksum"', "anti-tamper upstream intent");
requireText(globalPatch, '"code": "33c0 c3"', "anti-tamper always-zero return bytes");
requireText(originalGameManagerHeader, "// FUNCTION: TH07 0x00404fe0", "original CheckGameIntegrity function address");
requireText(originalGameManagerHeader, "i32 CheckGameIntegrity()", "original CheckGameIntegrity entry");
requireText(originalGameManagerHeader, "#ifdef NON_MATCHING", "original decomp nonmatching anti-tamper bypass");
requireText(gameManagerHeader, "i32 CheckGameIntegrity()\r\n    {\r\n        return 0;\r\n    }", "portable always-zero game-integrity predicate");
requireText(gameManagerHeader, "this->globals->csumAsSum = ComputeGameIntegrityCsum();", "portable integrity checksum bookkeeping retained");

// The three sprintf_call_ebp-50 hooks are the three Anm text formatter
// variants at 0x4542f1/0x4543ee/0x4545ee. Upstream strings_vsprintf has two
// independent responsibilities: (1) string/vararg translation lookup and (2)
// replacing the original 0x50-byte stack destination with dynamically-sized
// storage. Portable decomposes (1) at typed callers (EST1/EAS1, compiled MSG,
// spell/bomb tables, Music Room tables and Ending bytes), and must still keep
// (2). THCRAP builds therefore measure with vsnprintf(nullptr, 0) and allocate
// exact output storage; strict OFF retains the existing portable 256-byte
// formatter baseline.
requireText(th07Version, '"sprintf_call_ebp-50": {\n\t\t\t"addr": [\n\t\t\t\t"0x4542f1",\n\t\t\t\t"0x4543ee",\n\t\t\t\t"0x4545ee"', "TH07 three Anm safe-sprintf sites");
requireText(globalPatch, '"sprintf_call_ebp-50": {', "global Anm safe-sprintf hook");
requireText(globalPatch, '"code": "50e8[strings_vsprintf]8945b0"', "global Anm strings_vsprintf call");
requireText(upstreamStrings, "format = strings_lookup(format, NULL);", "upstream formatter format lookup");
requireText(upstreamStrings, "strings_va_lookup(va2, format);", "upstream formatter vararg lookup");
requireText(upstreamStrings, "int str_len = vsnprintf(NULL, 0, format, va2);", "upstream formatter dynamic length measurement");
requireText(upstreamStrings, "strings_storage_resize(ret, str_len);", "upstream formatter dynamic storage growth");
requireText(anm, "static std::vector<char> FormatThcrapAnmText(const char *format, va_list args)", "portable dynamic Anm formatter helper");
requireText(anm, "const int length = vsnprintf(nullptr, 0, format, measureArgs);", "portable dynamic Anm length measurement");
requireText(anm, "std::vector<char> output(static_cast<size_t>(length) + 1u);", "portable exact Anm output allocation");
requireText(anm, "std::vector<char> text = FormatThcrapAnmText(str, args);", "DrawVmTextFmt dynamic THCRAP path");
const dynamicFormatterUses = [...anm.matchAll(/FormatThcrapAnmText\((?:str|text), args\)/g)].length;
if (dynamicFormatterUses !== 3) throw new Error(`expected all 3 TH07 Anm formatter variants to use dynamic THCRAP storage, got ${dynamicFormatterUses}`);
const fixedFormatterBuffers = [...anm.matchAll(/char (?:text|buf)\[256\];/g)].length;
if (fixedFormatterBuffers !== 3) throw new Error(`expected 3 strict-OFF 256-byte Anm formatter baselines, got ${fixedFormatterBuffers}`);

// Typed caller decomposition for strings_vsprintf's lookup half. Raw dialogue
// text is already materialized in compiled MSG bytes; all remaining player-
// visible dynamic sources are looked up before reaching the generic renderer.
requireText(gui, "args->dialogue.text);", "compiled MSG dialogue reaches Anm formatter directly");
requireText(bombData, "g_Gui.ShowBombNamePortrait", "bomb display producer before Anm formatter");
requireText(bombData, "Localization::StringById", "bomb name typed lookup before Gui/Anm formatter");
requireText(eclManager, "const char *displaySpellName = Localization::SpellName", "spell name typed lookup before Gui/Anm formatter");
requireText(mainMenu, '"%s", Localization::StringById(g_MainMenuStringIds[i]', "main-menu lookup before centered Anm formatter");
requireText(mainMenu, 'Localization::StringById(g_OptionsStringIds[i]', "options lookup before centered Anm formatter");
requireText(mainMenu, 'Localization::StringById(g_KeyConfigStringIds[i]', "key-config lookup before centered Anm formatter");
requireText(musicRoom, "Localization::MusicComment", "Music Room comment typed lookup");
requireText(musicRoom, "Localization::MusicTitle", "Music Room title typed lookup");
requireText(resultScreen, "Localization::SpellName", "Result spell typed lookup before Anm formatter");
requireText(resultScreen, "Localization::FormatStringById", "Result formatted EST1 lookup before Anm formatter");
requireText(ending, "FindTranslatedEndingLine", "Ending translated-byte direct source before Anm formatter");
requireText(compiler, "const patched = patchThmsgDump(dumped, parsed);", "dialogue translation pre-materialization before runtime formatter");

// General strings_lookup: EST1 remains atomic. Stats formats additionally go
// through a runtime printf-signature check before reaching C varargs.
requireText(localization, 'std::memcmp(data, "EST1", 4) == 0', "EST1 magic validation");
requireText(localization, "count > 4096", "EST1 count bound");
requireText(localization, "(flags & ~0x1u) != 0 || reserved != 0", "EST1 flags/reserved validation");
requireText(localization, "offset != size", "EST1 exact EOF validation");
requireText(localization, "entries.swap(parsed);", "EST1 atomic publish");
requireText(localization, "Localization::StringById", "EST1 lookup API");
requireText(localization, "ParsePrintfSignature", "runtime printf signature parser");
requireText(localization, "Localization::FormatStringById", "safe formatted EST1 lookup");
requireText(localization, "fallbackSignature != translatedSignature", "format-signature fallback gate");
requireText(mainMenu, "g_MainMenuStringIds[8]", "main menu stringloc mapping");
requireText(mainMenu, "g_OptionsStringIds[9]", "options stringloc mapping");
requireText(mainMenu, "g_KeyConfigStringIds[12]", "key config stringloc mapping");
requireText(mainMenu, '"%s", Localization::StringById(g_MainMenuStringIds[i]', "main menu fixed-format lookup");
requireText(mainMenu, '"%s",\r\n                                            Localization::StringById(g_OptionsStringIds[i]', "options fixed-format lookup");
requireText(mainMenu, '"%s", Localization::StringById(g_KeyConfigStringIds[i]', "key config fixed-format lookup");

const bombMappings = [
  ["th07 Bomb Reimu A unfocused", "霊符「夢想封印　散」"],
  ["th07 Bomb Reimu A focused", "霊符「夢想封印　集」"],
  ["th06 Bomb Reimu B", "夢符「封魔陣」"],
  ["th07 Bomb Reimu B focused", "夢符「二重結界」"],
  ["th06 Bomb Marisa A", "魔符「スターダストレヴァリエ」"],
  ["th07 Bomb Marisa A focused", "魔符「ミルキーウェイ」"],
  ["th07 Bomb Marisa B unfocused", "恋符「ノンディレクショナルレーザー」"],
  ["th06 Bomb Marisa B", "恋符「マスタースパーク」"],
  ["th07 Bomb Sakuya A unfocused", "幻符「インディスクリミネイト」"],
  ["th07 Bomb Sakuya A focused", "幻符「殺人ドール」"],
  ["th07 Bomb Sakuya B unfocused", "時符「パーフェクトスクウェア」"],
  ["th07 Bomb Sakuya B focused", "時符「プライベートスクウェア」"],
];
for (const [id, fallback] of bombMappings) {
  requireText(stringContract, `{ id: "${id}" }`, `${id} EST1 contract`);
  requireText(bombData, `Localization::StringById("${id}", "${fallback}")`, `${id} bomb consumer`);
}

const th07ContractBlock = stringContract.match(/th07:\s*Object\.freeze\(\[([\s\S]*?)\]\),/);
if (!th07ContractBlock) throw new Error("TH07 string contract block not found");
const generalStringContractIds = [...th07ContractBlock[1].matchAll(/\{ id: "(th(?:06|07) (?:Key|Option|Menu|Bomb)[^"]+)" \}/g)]
  .map(match => match[1]);
if (generalStringContractIds.length !== 41 || new Set(generalStringContractIds).size !== 41) {
  throw new Error(`expected 41 unique TH07 general EST1 records, got ${generalStringContractIds.length}/${new Set(generalStringContractIds).size}`);
}

const allStringContractIds = [...th07ContractBlock[1].matchAll(/\{ id: "([^"]+)"/g)].map(match => match[1]);
if (allStringContractIds.length !== 98 || new Set(allStringContractIds).size !== 98) {
  throw new Error(`expected 98 unique TH07 EST1 records, got ${allStringContractIds.length}/${new Set(allStringContractIds).size}`);
}
const logStringContractIds = allStringContractIds.filter(id => /^(?:th06|th07)_(?:log|error)_/.test(id));
if (logStringContractIds.length !== 35 || new Set(logStringContractIds).size !== 35) {
  throw new Error(`expected 35 unique TH07 Log/Fatal EST1 records, got ${logStringContractIds.length}/${new Set(logStringContractIds).size}`);
}

// strings_lookup#cavesize_5/6 are not generic UI/ASCII hooks: their exact
// addresses are +3 bytes into GameErrorContext::Log and ::Fatal, where the
// fmt pointer lives at [ebp+0x0c]. Upstream mutates that pointer with
// strings_lookup() before vsprintf. Portable performs the same lookup by exact
// fallback literal, but routes through FormatStringById so translated printf
// signatures are checked before C varargs see the format.
requireText(th07Version, '"strings_lookup#cavesize_5": {\n            "addr": "Rx315f3"', "TH07 Log strings_lookup breakpoint");
requireText(th07Version, '"strings_lookup#cavesize_6": {\n            "addr": "Rx31733"', "TH07 Fatal strings_lookup breakpoint");
requireText(globalPatch, '"strings_lookup#cavesize_5": {\n            "str": "[ebp+0x0c]"', "global Log strings_lookup fmt source");
requireText(globalPatch, '"strings_lookup#cavesize_6": {\n            "str": "[ebp+0x0c]"', "global Fatal strings_lookup fmt source");
requireText(originalGameError, "// FUNCTION: TH07 0x004315f0", "original GameErrorContext::Log address");
requireText(originalGameError, "// FUNCTION: TH07 0x00431730", "original GameErrorContext::Fatal address");
requireText(upstreamStrings, "*string = strings_lookup(*string, NULL);", "upstream strings_lookup pointer replacement");
requireText(localization, "const char *Localization::LogString(const char *fallback)", "portable Log/Fatal resolver");
requireText(localization, "return FormatStringById(entry.id, fallback);", "portable Log/Fatal printf-safe lookup");
requireText(gameError, "const char *localizedFmt = Localization::LogString(fmt);", "portable GameErrorContext localization call");
requireText(gameError, "vsnprintf(tmp, sizeof(tmp), localizedFmt, args);", "portable localized Log/Fatal formatter");
requireText(stringContract, '{ id: "th06_log_unable_to_read_file", formatFallback: "%sが読み込めないです。\\r\\n" }', "TH07 log %s contract");
requireText(stringContract, '{ id: "th06_log_export_failure", formatFallback: "ファイルが書き出せません %s\\r\\n" }', "TH07 fatal %s contract");
requireText(stringContract, '{ id: "th06_error_two_instances", formatFallback: "二つは起動できません\\r\\n" }', "TH07 fatal no-arg contract");
requireText(stringContract, '{ id: "th06_log_color_composition", formatFallback: "テクスチャの色合成を抑制しますn" }', "TH07 original trailing-n log contract");

// thcrap layout.cpp owns one module-global Layout_Tabs array. Each independent
// TextOut/layout_process call resets cur_tab but reuses that global table.
requireText(layout, "Layout_Tabs = json_array();", "upstream persistent Layout_Tabs init");
requireText(layout, "json_array_set_new_expand(Layout_Tabs, lay->cur_tab, json_integer(tab_end));", "upstream tabstop persistence");
for (const command of ["case 's':", "case 't':", "case 'l':", "case 'c':", "case 'r':"])
  requireText(layout, command, `upstream layout command ${command}`);
requireText(layout, "tab_end = lay->bitmap_width;", "upstream full-bitmap tab end");
requireText(layout, "lay->cur_tab++;", "upstream per-call tab cursor");
requireText(layout, "lay->cur_w = tab_end - lay->cur_x;", "upstream tab advance");

requireText(helper, "static std::vector<i32> g_LayoutTabsFull;", "portable persistent full-pixel tabs");
requireText(helper, "BuildLayoutFragments(string, 1024, g_LayoutTabsFull", "portable 1024px layout bitmap");
requireText(helper, "if (ch != '\\t')", "portable GDI zero-width tab semantics");
for (const command of ["case 's':", "case 't':", "case 'l':", "case 'c':", "case 'r':"])
  requireText(helper, command, `portable layout command ${command}`);
requireText(helper, "tabsFull[curTab] = tabEnd;", "portable persistent tabstop write");
requireText(helper, "DebugLayoutSelfTest", "portable layout self-test");

const statsIds = [
  "th07 Stats Retries", "th07 Stats Retries +Phantasm",
  "th07 Stats Practice", "th07 Stats Practice +Phantasm",
  "th07 Stats Continue", "th07 Stats Continue +Phantasm",
  "th07 Stats Clear Count", "th07 Stats Clear Count +Phantasm",
  "th07 Stats Character Format", "th07 Stats Character Format +Phantasm",
  "th07 Stats Play Count", "th07 Stats Play Count +Phantasm",
  "th07 Stats Total Playtime", "th07 Stats Time Since Startup",
  "th07 Spell Result Character Select", "th07 Stats Total (All Characters)",
  "th07 Stats SakuyaB", "th07 Stats SakuyaA",
  "th06 Stats MarisaB", "th06 Stats MarisaA", "th06 Stats ReimuB", "th06 Stats ReimuA",
];
for (const id of statsIds) {
  requireText(stringContract, `{ id: "${id}"`, `${id} EST1 contract`);
  requireText(resultScreen, `"${id}"`, `${id} Result consumer`);
}
requireText(resultScreen, "LocalizedStatsCharacterName", "Stats %s argument localization");
requireText(main, '"--result-stats"', "real Result Stats Dev entry");
requireText(main, '"--thcrap-layout-selftest"', "layout self-test Dev entry");

// TH07's three sprintf_replay binhacks wrap replay FILE PATH construction,
// not player-facing labels. Portable intentionally supersedes these with its
// per-user pref-path abstraction; translating the path would break replay I/O.
requireText(th07Version, '"sprintf_replay_1": {\n\t\t\t"addr": "Rx475ab"', "sprintf_replay_1 address");
requireText(th07Version, '"sprintf_replay_2": {\n\t\t\t"addr": "Rx47c41"', "sprintf_replay_2 address");
requireText(th07Version, '"sprintf_replay_3": {\n\t\t\t"addr": "Rx5aa3a"', "sprintf_replay_3 address");
requireText(th07Patch, '"title": "Replay name sprintf 1"', "sprintf_replay_1 upstream hook");
requireText(resultScreen, 'FileSystem::GetPrefPath("replay") + "/" + filename', "Result replay pref-path construction");
requireText(mainMenu, 'FileSystem::GetPrefPath("replay") + "/" + filename', "MainMenu replay pref-path construction");
requireText(resultScreen, 'snprintf(filename, sizeof(filename), "th7_%.2d.rpy"', "Result replay basename contract");
requireText(mainMenu, 'snprintf(filename, sizeof(filename), "th7_%.2d.rpy"', "MainMenu replay basename contract");

// TH07 spell localization is a three-part contract. spell_loop rewrites the
// fixed 48-byte ECL XOR pass; spell_name only swaps the display pointer after
// that pass; spell_name#result performs the same lookup from CATK/history.
// unpatch_result_spell explicitly removes a legacy English-patch lookup, so
// localized display text must never replace the Japanese CATK name persisted
// by the game.
requireText(th07Version, '"spell_loop": {\n\t\t\t"addr": "0x40fcb1"', "TH07 spell_loop address");
requireText(th07Patch, '"title": "Rewrite spell card name decryption loop"', "TH07 spell_loop intent");
requireText(th07Patch, "83fe307315", "TH07 spell_loop 48-byte bound");
requireText(th07Patch, "81f2aa000000", "TH07 spell_loop XOR key");
requireText(eclManager, "for (i = 0; (u32)i < 48; i++)", "portable 48-byte spell decryption loop");
requireText(eclManager, "spellcardName[i] = (u8)spellcardName[i] ^ 0xaa;", "portable spell XOR key");
requireText(th07Version, '"spell_name": {\n\t\t\t"addr": "0x40fce1"', "TH07 spell_name address");
requireText(eclManager, "const u32 spellcardId = instr->args[0].us[1];", "portable spell ID source");
requireText(eclManager, "const char *displaySpellName = Localization::SpellName(spellcardId, spellcardName);", "portable game spell display lookup");
requireText(eclManager, "g_Gui.ShowSpellcard(instr->args[0].s[0], displaySpellName);", "portable localized spell display consumer");
requireText(eclManager, "strcpy(catk->name, spellcardName);", "portable Japanese CATK persistence");
requireText(th07Version, '"unpatch_result_spell": {\n\t\t\t"addr": "Rx467ea"', "TH07 unpatch_result_spell address");
requireText(th07Patch, '"title": "Remove English patch spell translation lookup in the Result screen"', "TH07 result unpatch intent");
requireText(th07Version, '"spell_name#result": {\n\t\t\t"addr": "0x4467ed"', "TH07 result spell breakpoint address");
requireText(resultScreen, "const char *spellName = Localization::SpellName(", "portable Result spell lookup");
requireText(resultScreen, "static_cast<u32>(vmIdx), g_GameManager.catk[vmIdx].name", "portable Result CATK fallback");
requireText(resultScreen, "0xffffff, 0, spellName);", "portable Result localized spell consumer");

// Music Room patches keep a (track,line) state around the original
// musiccmt.txt parser. TH07's vanilla parser contains an impossible
// `while (newline && carriage-return)` immediately after the track title.
// That leaves the terminator in place and creates an empty description slot 0.
// The line-number breakpoint at 0x43b313 is first reached while advancing from
// that empty slot to VM slot 1, publishing line_num=0. Therefore thcrap
// musiccmt.js line N maps to portable description/VM slot N+1. The numbered
// title (`@`, line 0) must render in slot 1 at y=336, inside music.jpg's black
// comment box, exactly as the real original-exe thcrap reference does.
requireText(th07Version, '"music_cmt_prepare": {\n\t\t\t"addr": "0x43aa32"', "TH07 music_cmt_prepare address");
requireText(th07Version, '"music_cmt_prepare_first": {\n\t\t\t"addr": "0x43b30a"', "TH07 music_cmt_prepare_first address");
requireText(th07Patch, '"code": "31c0488945f8"', "TH07 Music Room comment-state prepare patch");
requireText(th07Patch, '"code": "31c0488945fc"', "TH07 Music Room first-comment prepare patch");
requireText(th07Version, '"music_title#track": {', "TH07 music_title track breakpoint");
requireText(th07Version, '"0x43b1c4"', "TH07 music_title track address #1");
requireText(th07Version, '"0x43b393"', "TH07 music_title track address #2");
requireText(th07Version, '"music_title": {\n\t\t\t"addr": "0x43b23b"', "TH07 music_title address");
requireText(th07Patch, '"music_title#track": {\n\t\t\t"track": "eax"', "TH07 Music Room title track source");
requireText(th07Version, '"music_cmt#track": {\n\t\t\t"addr": "0x43a9f4"', "TH07 music_cmt track address");
requireText(th07Version, '"music_cmt#line_num": {', "TH07 music_cmt line-number breakpoint");
requireText(th07Version, '"0x43aa3b"', "TH07 music_cmt line address #1");
requireText(th07Version, '"0x43b313"', "TH07 music_cmt line address #2");
requireText(th07Version, '"music_cmt": {', "TH07 music_cmt breakpoint");
requireText(th07Version, '"0x43aab2"', "TH07 music_cmt address #1");
requireText(th07Version, '"0x43b403"', "TH07 music_cmt address #2");
requireText(th07Patch, '"music_cmt#track": {\n\t\t\t"track": "ecx"', "TH07 Music Room comment track source");
requireText(th07Patch, '"music_cmt#line_num": {\n\t\t\t"line_num": "eax"', "TH07 Music Room comment line source");
requireText(th07Patch, '"format_id": "Music Room Numbered Title"', "TH07 Music Room numbered-title format selector");
requireText(upstreamMusic, "*str = str_rep;", "upstream Music Room direct comment pointer replacement");
requireText(upstreamMusic, "*str = strings_sprintf(0, format, track_id_displayed, title);", "upstream Music Room dynamic numbered-title pointer replacement");
requireText(upstreamMusic, "*str = title;", "upstream Music Room direct title pointer replacement");
requireText(musicRoom, "MusicRoomTitleForDisplay", "portable render-time Music Room title lookup");
requireText(musicRoom, "MusicRoomCommentForDisplay", "portable render-time Music Room comment lookup");
requireText(musicRoom, "Localization::MusicTitle(static_cast<std::uint32_t>(track), descriptor.title)", "portable direct Music Room title pointer lookup");
const musicPathWrite = 'arg->trackDescriptors[offset].path[charIdx] = *curChar;';
const musicTitleWrite = 'arg->trackDescriptors[offset].title[charIdx] = *curChar;';
const musicPathPos = musicRoom.indexOf(musicPathWrite);
const musicTitlePos = musicRoom.indexOf(musicTitleWrite, musicPathPos);
const musicPathTerminator = musicRoom.indexOf("while (*curChar == '\\n' || *curChar == '\\r')", musicPathPos);
const musicTitleTerminator = musicRoom.indexOf("while (*curChar == '\\n' && *curChar == '\\r')", musicTitlePos);
if (!(musicPathPos >= 0 && musicPathTerminator > musicPathPos && musicPathTerminator < musicTitlePos &&
      musicTitlePos >= 0 && musicTitleTerminator > musicTitlePos)) {
  throw new Error('TH07 Music Room parser contract drifted: CR/LF OR belongs after path, impossible AND belongs after title');
}
const misplacedMusicAnd = musicRoom.indexOf("while (*curChar == '\\n' && *curChar == '\\r')", musicPathPos);
if (misplacedMusicAnd >= 0 && misplacedMusicAnd < musicTitlePos) {
  throw new Error('TH07 Music Room parser contract regression: impossible AND moved before title parsing');
}
requireText(musicRoom, "Localization::MusicComment(", "portable Music Room comment lookup");
requireText(musicRoom, "static_cast<std::uint16_t>(slot - 1)", "portable thcrap line to VM slot mapping");
requireText(musicRoom, "if (!Localization::Active() || slot == 0)", "portable Music Room blank slot ownership");
requireText(musicRoom, 'std::strcmp(comment, "@")', "portable Music Room numbered-title marker");
requireText(musicRoom, '"No. %2u  ", static_cast<unsigned>(track)', "portable Music Room numbered-title prefix");
requireText(musicRoom, "formatted += MusicRoomTitleForDisplay(descriptor, track);", "portable dynamic Music Room numbered-title output");
if (musicRoom.includes("Localization::CopyText(descriptor.description") ||
    musicRoom.includes("std::snprintf(descriptor.description") ||
    musicRoom.includes("Localization::CopyText(descriptor.title")) {
  throw new Error("TH07 Music Room must not pre-materialize translated UTF-8 into vanilla fixed-size TrackDescriptor buffers");
}

// Boss title/name textimages are one logical two-slot group. Upstream owns
// logical slots 0x702/0x703 and 384x64 sprite rows, with Stage 4 selecting the
// Prismriver row from the right portrait sprite. Portable may use private GPU
// texture slots, but must keep the logical VM slots, all-or-nothing activation,
// row mapping, and Japanese text fallback when the image group is unavailable.
requireText(th07Version, '"textimage_set": {\n\t\t\t"addr": "Rx2a2d5"', "TH07 textimage_set address");
requireText(th07Version, '"0x702": "[Rx22f85c] - 1', "TH07 boss-title textimage row expression");
requireText(th07Version, '"0x703": "[Rx22f85c] - 1', "TH07 boss-name textimage row expression");
requireText(th07Version, "0x1fe0c+0x1d4", "TH07 Stage4 right-portrait sprite source");
requireText(th07Version, "0x4ad", "TH07 Prismriver portrait base");
requireText(th07Version, '"textimage_is_active": {\n\t\t\t"addr": "Rx2a339"', "TH07 textimage_is_active address");
requireText(th07Patch, '"slots": ["0x702", "0x703"]', "TH07 textimage active slots");
requireText(th07Version, '"textimage_init": {\n\t\t\t"addr": "Rx3414c"', "TH07 textimage_init address");
requireText(th07Patch, '"filename": "ti_bosstitle.png"', "TH07 boss-title image filename");
requireText(th07Patch, '"filename": "ti_bossname.png"', "TH07 boss-name image filename");
requireText(th07Patch, '"texture_slot": 50', "TH07 boss-title private texture slot");
requireText(th07Patch, '"texture_slot": 51', "TH07 boss-name private texture slot");
requireText(th07Patch, '"sprite_w": 384', "TH07 boss textimage width");
requireText(th07Patch, '"sprite_h": 64', "TH07 boss textimage row height");
requireText(localization, 'LoadTextImage(50, "ti_bosstitle.png"', "portable boss-title image load");
requireText(localization, 'LoadTextImage(51, "ti_bossname.png"', "portable boss-name image load");
requireText(localization, "all-or-nothing activation rule", "portable boss image group activation");
requireText(localization, "bool ResolveBossImageRow", "portable boss image row resolver");
requireText(localization, "const i32 faceDelta = rightPortraitSprite - 0x4ad;", "portable Prismriver portrait base");
requireText(localization, "row = 3 + face;", "portable Prismriver row mapping");
requireText(localization, "row = stage <= 3 ? static_cast<i32>(stage - 1)", "portable non-Prismriver row mapping");
requireText(localization, "return ApplyTextImage(vm, spriteSlot, textureSlot, row, 384, 64);", "portable boss textimage dimensions");
requireText(localization, 'g_BossTitleImageReady, 0x702', "portable boss-title logical sprite slot");
requireText(localization, 'g_BossNameImageReady, 0x703', "portable boss-name logical sprite slot");
requireText(gui, "if (!bossImageApplied)", "portable Japanese/text fallback when image inactive");
requireText(gui, "g_AnmManager->DrawStringFormat", "portable boss intro fallback renderer");
requireText(main, '"--thcrap-textimage-selftest"', "boss textimage row self-test entry");

// ascii_patch_1/2 replace ZUN's variadic ASCII formatter as one unit; the
// ascii_params breakpoint only points the helper at the live AsciiManager
// state (class pointer, Scale and TH07's 14px character advance). Portable
// receives the manager as a typed argument and reads those fields directly.
requireText(th07Version, '"ascii_patch_1": { "addr": "Rx206c" }', "TH07 ascii_patch_1 address");
requireText(th07Version, '"ascii_patch_2": { "addr": "Rx2099" }', "TH07 ascii_patch_2 address");
requireText(th07Patch, "Hook ZUN's variadic ASCII printing function", "TH07 ASCII variadic hook intent");
requireText(th07Patch, "e8[ascii_vpatchf]", "TH07 ASCII helper call");
requireText(upstreamAscii, "int ascii_vpatchf_th07_th08(", "upstream TH07/TH08 ASCII helper");
requireText(upstreamAscii, "auto id = strings_id(fmt);", "upstream ASCII format stringloc lookup");
requireText(upstreamAscii, "auto single_str = strings_vsprintf", "upstream ASCII safe/localized printf path");
requireText(upstreamAscii, '{ "th07 Full Power", "Full Power Mode!", game_id == TH07 ? 8.5f : 15.5f }', "upstream TH07 Full Power alignment");
requireText(upstreamAscii, '{ "th06_ascii_centered_spell_bonus", "Spell Card Bonus!", game_id == TH07 ? 17.5f : 8.5f }', "upstream TH07 spell bonus alignment");
requireText(upstreamAscii, '{ "th07 Supernatural Border", "Supernatural Border!!", -11.5f }', "upstream TH07 border alignment");
requireText(upstreamAscii, '{ "th07 CherryPoint Max", "CherryPoint Max!", 8.5f }', "upstream TH07 CherryPoint alignment");
requireText(upstreamAscii, '{ "th07 Border Bonus Format", "Border Bonus 1234567", -7.5f }', "upstream TH07 border bonus alignment");
requireText(th07Version, '"ascii_params": {\n\t\t\t"addr": "Rx38c5f"', "TH07 ascii_params address");
requireText(th07Version, '"ClassPtr": "0x134ce18"', "TH07 ascii_params class pointer");
requireText(th07Patch, '".Scale": "0x74c4"', "TH07 ascii_params scale offset");
requireText(th07Patch, '"CharWidth": 14.0', "TH07 ascii_params character advance");
requireText(upstreamAsciiHeader, "Pointer to the beginning of the game's ASCII manager class.", "upstream ascii ClassPtr semantics");
requireText(asciiManager, "void AsciiManager::AddFormatText", "portable variadic ASCII entry");
requireText(asciiManager, "if (FormatLocalizedAscii(manager, *pos, localizedPos, str, sizeof(str), fmt, args))", "portable localized ASCII formatter call");
requireText(asciiManager, "manager->AddString(&localizedPos, str);", "portable ASCII final draw queue");
requireText(asciiManager, "const bool known = active && Localization::LookupAscii(format, entry);", "portable EAS1 format lookup");
requireText(asciiManager, "Localization::AsciiString(value)", "portable printf %s secondary lookup");
requireText(asciiManager, "static_cast<float>(manager->fontSpacing) * manager->scale.x", "portable ASCII live char-width state");
requireText(asciiManager, "this->fontSpacing = 14;", "portable TH07 default 14px character advance");
requireText(asciiManager, "this->scale.x = 1.0f;", "portable ASCII live scale state");
requireText(asciiManager, "const float center = sourcePos.x + baselineExtent * 0.5f + entry.extraX;", "portable EAS1 alignment center formula");
requireText(asciiManager, "std::vsnprintf(output, outputSize, format, fallbackArgs)", "portable unknown/unsafe ASCII fallback");

// ending_copy_rem/rep remove the original 68-byte CP932 temporary copy. The
// compiled translation endings carry UTF-8 lines terminated by NUL before LF;
// only those lines take the direct-pointer path. Japanese/plain fallback lines
// retain the original two-byte copy loop.
requireText(th07Version, '"ending_copy_rem" : {\n\t\t\t"addr": "Rx1e2e9"', "ending_copy_rem address");
requireText(th07Version, '"ending_copy_rep" : {\n\t\t\t"addr": "Rx1e11d"', "ending_copy_rep address");
requireText(globalPatch, "Remove the useless string copy and buffer overflow in ending messages, #1", "ending_copy_rem upstream intent");
requireText(globalPatch, "Remove the useless string copy and buffer overflow in ending messages, #2", "ending_copy_rep upstream intent");
requireText(ending, "char local_54[68];", "Japanese ending temporary baseline");
requireText(ending, "FindTranslatedEndingLine", "translated ending line detector");
requireText(ending, "if (*scan != '\\0')", "translated ending NUL-before-newline gate");
requireText(ending, "translatedLine != nullptr ? translatedLine : local_54", "ending direct-pointer/fallback draw split");
requireText(ending, "local_54[local_58 + 1] = this->endFileDataPtr[1];", "Japanese ending two-byte fallback copy");
requireText(main, '"--ending-reimu-a"', "real Ending Dev entry");
requireText(main, '"--thcrap-ending-selftest"', "ending direct-line self-test entry");

// Original TH07 Ending does not own a presentation interpolation layer. It
// draws the current background position and current ANM VMs directly, then
// draws the current fade color. Portable advances the fade state from the
// fixed 60Hz calc callback (rather than mutating it from high-refresh draw),
// but rendering must remain current-state/direct or translated text can be
// presented through a second lifetime not present in the original game.
requireText(originalEnding, "g_AnmManager->Draw(&arg->sprites[i]);", "original Ending direct VM draw");
requireText(originalEnding, "ScreenEffect::DrawSquare(&rect, this->endingFadeRectColor.color);", "original Ending direct fade draw");
requireText(ending, "g_AnmManager->DrawCurrent(&arg->sprites[i]);", "portable Ending current-state VM draw");
requireText(ending, "ScreenEffect::DrawSquare(&rect, this->endingFadeRectColor.color);", "portable Ending direct fade draw");
requireText(anmHeader, "const f32 savedRenderAlpha = g_RenderAlpha;", "portable current-state draw saves presentation alpha");
requireText(anmHeader, "g_RenderAlpha = 1.0f;", "portable current-state draw selects current VM state");
requireText(anmHeader, "g_RenderAlpha = savedRenderAlpha;", "portable current-state draw restores presentation alpha");
if (ending.includes("DrawInterp(&arg->sprites[i])") || ending.includes("prevEndingFadeRectColor") ||
    ending.includes("prevBackgroundPos")) {
  throw new Error("TH07 Ending must not reintroduce a second presentation-interpolation lifetime absent from original TH07");
}

// DrawEndingRect is an immediate surface draw followed in the same Ending
// callback by normal ANM text/sprite draws. Portable's renderer caches the
// currently bound ANM texture, so this direct path must flush queued sprites
// and update currentTexture before binding the surface. Otherwise DrawInner can
// falsely skip the next ANM BindTexture while the GPU still owns the Ending
// background texture, producing alternating wrong-texture/flicker frames.
const endingRectStart = anm.indexOf("void AnmManager::DrawEndingRect(");
const screenshotStart = anm.indexOf("void AnmManager::TakeScreenshot(", endingRectStart);
if (endingRectStart < 0 || screenshotStart < 0) throw new Error("TH07 DrawEndingRect bounds missing");
const endingRect = anm.slice(endingRectStart, screenshotStart);
for (const [needle, label] of [
  ["this->Flush();", "Ending direct draw flush"],
  ["this->currentTexture = this->surfaceTextures[surfaceIdx];", "Ending surface texture cache sync"],
  ["g_Supervisor.gfxDevice->BindTexture(this->currentTexture);", "Ending synchronized surface bind"],
]) requireText(endingRect, needle, label);
const endingFlush = endingRect.indexOf("this->Flush();");
const endingCache = endingRect.indexOf("this->currentTexture = this->surfaceTextures[surfaceIdx];");
const endingBind = endingRect.indexOf("g_Supervisor.gfxDevice->BindTexture(this->currentTexture);");
const endingDraw = endingRect.indexOf("DrawPrimitiveUP(PRIM_TRIANGLE_STRIP");
if (!(endingFlush < endingCache && endingCache < endingBind && endingBind < endingDraw)) {
  throw new Error("TH07 DrawEndingRect direct-draw/cache-sync order drifted");
}

// The original Demonstration label intentionally runs ANM script 7, whose
// opcode 34 is ANM_INTERP_ALPHA. Keep that slow source animation. Portable
// DrawInner additionally interpolates prevColor->color at presentation rate,
// so the Demonstration VM must publish its previous state at every fixed 60Hz
// update; otherwise prevColor stays stale and creates frame-level sawtooth
// flicker unrelated to the original script.
requireText(originalAsciiManager, "SetAnmIdxAndExecuteScript(&arg->vm, 7);", "original Demonstration script 7");
requireText(originalAsciiManager, "g_AnmManager->ExecuteScript(&arg->vm);", "original Demonstration VM update");
requireText(originalAnmVmHeader, "ANM_INTERP_ALPHA = 34", "original Demonstration alpha-interpolation opcode");
requireText(portableAsciiManagerHeader, "this->vm.UpdatePrev();", "portable Demonstration previous-state publication");

// boss-title line order: original DrawDialogue draws introLines[0] then [1].
// base_tsa rewrites the two ADD immediate VM offsets so localized runs draw
// Name (introLines[1]) before Title (introLines[0]).
requireText(th07Version, '"bosstitle_line_order#1": {\n\t\t\t"addr": "Rx2acfc"', "boss-title line-order #1 address");
requireText(th07Version, '"bosstitle_line_order#2": {\n\t\t\t"addr": "Rx2ad13"', "boss-title line-order #2 address");
requireText(th07Patch, '"code": "3c070200"', "boss-title #1 patched offset 0x2073c");
requireText(th07Patch, '"code": "f0040200"', "boss-title #2 patched offset 0x204f0");
requireText(th07Patch, "Render the two boss title lines in the opposite order, #1: Name", "boss-title #1 upstream intent");
requireText(th07Patch, "Render the two boss title lines in the opposite order, #2: Title", "boss-title #2 upstream intent");
const originalBossIntro0 = "g_AnmManager->DrawNoRotation(&this->msg.introLines[0]);";
const originalBossIntro1 = "g_AnmManager->DrawNoRotation(&this->msg.introLines[1]);";
requireText(originalGui, originalBossIntro0, "original boss-title line 0 draw");
requireText(originalGui, originalBossIntro1, "original boss-title line 1 draw");
if (originalGui.indexOf(originalBossIntro0) >= originalGui.indexOf(originalBossIntro1))
  throw new Error("original boss-title draw order is not [0,1]");
const portableBossIntro0 = "g_AnmManager->DrawInterpNoRotation(&this->msg.introLines[0]);";
const portableBossIntro1 = "g_AnmManager->DrawInterpNoRotation(&this->msg.introLines[1]);";
const bossOrderAnchor = gui.indexOf("base_tsa/th07 swaps the original two immediate VM offsets");
if (bossOrderAnchor < 0) throw new Error("missing localized boss-title order anchor");
const localizedIntro1 = gui.indexOf(portableBossIntro1, bossOrderAnchor);
const localizedIntro0 = gui.indexOf(portableBossIntro0, localizedIntro1 + 1);
const fallbackElse = gui.indexOf("else", localizedIntro0 + 1);
const japaneseIntro0 = gui.indexOf(portableBossIntro0, fallbackElse);
const japaneseIntro1 = gui.indexOf(portableBossIntro1, japaneseIntro0 + 1);
if (!(localizedIntro1 >= 0 && localizedIntro0 > localizedIntro1 && fallbackElse > localizedIntro0))
  throw new Error("localized boss-title draw order is not [1,0]");
if (!(japaneseIntro0 > fallbackElse && japaneseIntro1 > japaneseIntro0))
  throw new Error("Japanese boss-title fallback draw order is not [0,1]");

// stage_result_align: base_tsa replaces the original currentStage comparison
// at Rx27f73 with a codecave call, then resumes the same Stage/All Clear
// branch. The codecave strings_lookup()s the Clear-row format at 0x4984fc,
// measures its byte length, multiplies by the fixed 8px ASCII advance, and
// writes x = 200 - length*8 into the Stage Result block's shared stringPos.
requireText(th07Version, '"stage_result_align": "56 8d75 f4 6a 00 68 fc844900 e8[strings_lookup]', "stage-result align codecave");
requireText(th07Version, '"stage_result_align_call": {\n\t\t\t"addr": "Rx27f73"', "stage-result align call address");
requireText(th07Version, '"code": "e8[codecave:stage_result_align] 8d45 f4 90 833d 5cf86200 06', "stage-result align call patch");
requireText(originalGui, "stringPos.x = 144.0f;", "original Stage Result x=144 baseline");
requireText(gui, 'Localization::AsciiString("Clear  = %8d")', "portable Stage Result lookup basis");
requireText(gui, "200.0f - static_cast<f32>(std::strlen(clearFormat) * 8u)", "portable Stage Result x formula");
requireText(gui, "th07 thcrap stage result align: localization=0", "Japanese Stage Result baseline audit");

// The detailed assertions above are necessary but not sufficient: also fail
// closed on the complete v1.00b site inventory and reverse portable consumer
// inventory every time this normal contract entrypoint runs.
await import('./thcrap-proof-ledger.mjs');

console.log("th07 thcrap source contract: PASS");
