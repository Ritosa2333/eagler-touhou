import fs from 'node:fs';
import path from 'node:path';
import { createHash } from 'node:crypto';
import { fileURLToPath } from 'node:url';

const here = path.dirname(fileURLToPath(import.meta.url));
const project = path.resolve(here, '..');
const workspace = path.resolve(project, '..');

function read(rel) {
    return fs.readFileSync(path.join(workspace, rel), 'utf8').replaceAll('\r\n', '\n');
}

const upstreamBase = read('dependencies/upstream-thcrap-tsa/base_tsa/th06.js');
const upstreamVersion = read('dependencies/upstream-thcrap-tsa/base_tsa/th06.v1.02h.js');
const upstreamStringlocs = read('dependencies/upstream-thcrap-tsa/base_tsa/th06/stringlocs.v1.02h.js');
const upstreamStrings = read('dependencies/upstream-thcrap/thcrap/src/strings.cpp');
const upstreamMusic = read('dependencies/upstream-thcrap/thcrap_tsa/src/music.cpp');
const originalSource = read('th06/src/ResultScreen.cpp');
const portableSource = read('th06-eagler/src/ResultScreen.cpp');
const originalGui = read('th06/src/Gui.cpp');
const portableGui = read('th06-eagler/src/Gui.cpp');
const originalEnding = read('th06/src/Ending.cpp');
const portableEnding = read('th06-eagler/src/Ending.cpp');
const originalMainMenu = read('th06/src/MainMenu.cpp');
const portableMainMenu = read('th06-eagler/src/MainMenu.cpp');
const originalMusicRoom = read('th06/src/MusicRoom.cpp');
const portableMusicRoom = read('th06-eagler/src/MusicRoom.cpp');
const originalEclManager = read('th06/src/EclManager.cpp');
const portableEclManager = read('th06-eagler/src/EclManager.cpp');
const originalGameManager = read('th06/src/GameManager.cpp');
const portableGameManager = read('th06-eagler/src/GameManager.cpp');
const originalGameWindow = read('th06/src/GameWindow.cpp');
const portableGameWindow = read('th06-eagler/src/GameWindow.cpp');
const originalAsciiManager = read('th06/src/AsciiManager.cpp');
const portableAsciiManager = read('th06-eagler/src/AsciiManager.cpp');
const portableBomb = read('th06-eagler/src/BombData.cpp');
const portableAnm = read('th06-eagler/src/AnmManager.cpp');
const portableSupervisor = read('th06-eagler/src/Supervisor.hpp');
const originalFileSystem = read('th06/src/FileSystem.cpp');
const portableFileSystem = read('th06-eagler/src/FileSystem.cpp');
const originalController = read('th06/src/Controller.cpp');
const portableController = read('th06-eagler/src/Controller.cpp');
const originalAnmManager = read('th06/src/AnmManager.cpp');
const portableAnmManager = read('th06-eagler/src/AnmManager.cpp');
const originalTextHelper = read('th06/src/TextHelper.cpp');
const portableTextHelper = read('th06-eagler/src/TextHelper.cpp');
const portableI18n = read('th06-eagler/src/i18n.hpp');
const portableThpracImGui = read('th06-eagler/src/ThpracImGui.cpp');
const portableConfig = read('th06-eagler/src/Config/config.cpp');
const portableCMake = read('th06-eagler/CMakeLists.txt');
const portableLocalization = read('th06-eagler/src/Localization.cpp');
const portableLocalizationHeader = read('th06-eagler/src/Localization.hpp');
const portableGameErrorContext = read('th06-eagler/src/GameErrorContext.cpp');
const upstreamScreenshot = read('dependencies/upstream-thcrap/thcrap_tsa/src/screenshot.cpp');
const upstreamTextImage = read('dependencies/upstream-thcrap/thcrap_tsa/src/textimage.cpp');
const upstreamAnm = read('dependencies/upstream-thcrap/thcrap_tsa/src/anm.cpp');
const upstreamGlobal = read('dependencies/upstream-thcrap-tsa/base_tsa/global.js');
const upstreamAscii = read('dependencies/upstream-thcrap/thcrap_tsa/src/ascii.cpp');
const stringContract = read('eagler-touhou/server/thcrap-string-contract.mjs');
const unifontBytes = fs.readFileSync(path.join(workspace, 'dependencies/unifont-15.1.05/unifont-15.1.05.otf'));
const unifontSha256 = createHash('sha256').update(unifontBytes).digest('hex');

// TH06 keeps ANM textures outside the ANM file and frequently has a separate
// *_a.png alpha mask. A thcrap PNG is nevertheless a sprite-level coordinate
// patch, not a complete replacement texture. The portable path therefore has
// to load the original archive texture + original alpha first, then compose
// the runtime PNG over only the ANM sprite rectangles using upstream's alpha
// decision/blend rules.
for (const needle of [
    'if(sp.rep_x >= image.img.width || sp.rep_y >= image.img.height)',
    'sp.copy_w = MIN(sprite.w, (image.img.width - sp.rep_x));',
    'sp.copy_h = MIN(sprite.h, (image.img.height - sp.rep_y));',
    'if(dst_alpha == SPRITE_ALPHA_OPAQUE)',
    'func = blit_blend;',
]) {
    if (!upstreamAnm.includes(needle))
        throw new Error(`upstream thcrap ANM sprite-patch contract drifted: ${needle}`);
}
if (!portableFileSystem.includes('OpenOriginalPath') ||
    !portableFileSystem.includes('OpenPathImpl(filepath, isExternalResource, false)')) {
    throw new Error('TH06 must expose an override-bypassing original-file read for external ANM composition');
}
for (const needle of [
    'runtimePatch = LoadRuntimePatchSurface(anmName);',
    'false, false, false) != ZUN_SUCCESS',
    'LoadTextureAlphaChannel(anm->textureIdx, alphaName, anm->format,',
    'anm->colorKey, false)',
    'ApplyRuntimeSpritePatch(this, anm->textureIdx, anm, runtimePatch',
    'const i32 copyWidth = std::min(rect.w, patch->w - rect.x);',
    'const i32 copyHeight = std::min(rect.h, patch->h - rect.y);',
    'replacementAlpha == RuntimePatchAlphaState::Empty',
    'destinationAlpha == RuntimePatchAlphaState::Opaque',
    'BlendRuntimePatchRow(destinationRow, replacementRow, copyWidth, texture.format);',
]) {
    if (!portableAnmManager.includes(needle))
        throw new Error(`TH06 portable ANM sprite-patch contract missing: ${needle}`);
}
if (!portableAnmManager.includes('destination[3] = static_cast<u8>(std::min<i32>(destination[3] + replacementAlpha, 0xff));')) {
    throw new Error('TH06 RGBA32 sprite blending must retain upstream additive/clamped alpha semantics');
}
if (!portableAnmManager.includes('const i32 outA = std::min(dstA + repA, 0xf);')) {
    throw new Error('TH06 A4R4G4B4 sprite blending must retain upstream additive/clamped alpha semantics');
}

if (unifontSha256 !== '7b62b50acbb186689dc30c446ce4367b87d79489e9907b83255f9fbe0dcfb9e1') {
    throw new Error(`GNU Unifont 15.1.05 OTF bytes drifted: ${unifontSha256}`);
}
if (!portableI18n.includes('#define TH_FONT_NAME "ＭＳ ゴシック"') ||
    !portableI18n.includes('#define TH_PRIMARY_FONT_FILENAME "msgothic.ttc"') ||
    !portableI18n.includes('#define TH_LOCALIZED_FONT_FILENAME "unifont.otf"')) {
    throw new Error('TH06 must preserve MS Gothic for vanilla text and reserve GNU Unifont for localized text');
}
if (!portableCMake.includes('TH_UNIFONT_FILE') || !portableCMake.includes('unifont-15.1.05.otf') ||
    !portableCMake.includes('msgothic.ttc') || !portableCMake.includes('/unifont.otf')) {
    throw new Error('TH06 build must keep the vanilla MS Gothic and localized/thprac Unifont resources distinct');
}
if (!portableTextHelper.includes('localized ? TH_LOCALIZED_FONT_FILENAME : TH_PRIMARY_FONT_FILENAME') ||
    !portableTextHelper.includes('const wchar_t *faceName = L"Unifont";') ||
    portableTextHelper.includes('NotoSans')) {
    throw new Error('TH06 vanilla text must use MS Gothic while the localized GDI/SDL path uses GNU Unifont');
}
const thpracFontBuilder = portableThpracImGui.slice(
    portableThpracImGui.indexOf('bool BuildLocaleFont'),
    portableThpracImGui.indexOf('bool BuildLocaleFont') + 1800);
if (!thpracFontBuilder.includes('FileSystem::GetBasePath("unifont.otf")') ||
    !thpracFontBuilder.includes('AddFontFromFileTTF') || thpracFontBuilder.includes('AddSystemFont')) {
    throw new Error('TH06 thprac ImGui must load the same GNU Unifont OTF instead of a system-font candidate');
}
if (!portableConfig.includes('TTF_OpenFont("unifont.otf", 18)') ||
    portableConfig.includes('NotoSans') || portableConfig.includes('msgothic.ttc')) {
    throw new Error('TH06 configuration UI must use GNU Unifont');
}

if (!upstreamBase.includes('"result_rank_format"') ||
    !upstreamBase.includes('Run the Result rank string through the variadic ASCII printing function')) {
    throw new Error('Upstream TH06 result_rank_format contract is missing');
}

if (!/"result_rank_format"\s*:\s*\{\s*"addr"\s*:\s*"Rx2d4a4"\s*,\s*"code"\s*:\s*"6800b94700 e8a241"/m.test(upstreamVersion)) {
    throw new Error('TH06 v1.02h result_rank_format address/code drifted');
}

const originalCall = 'g_AsciiManager.AddString(&strPos, g_RightAlignedDifficultyList[g_GameManager.difficulty]);';
const portableCall = 'g_AsciiManager.AddFormatText(&strPos, g_RightAlignedDifficultyList[g_GameManager.difficulty]);';

if (!originalSource.includes(originalCall)) {
    throw new Error('Original TH06 reconstructed source no longer exposes the rank AddString callsite');
}
if (!portableSource.includes(portableCall)) {
    throw new Error('Portable TH06 must mirror result_rank_format by routing the rank string through AddFormatText');
}
if (portableSource.includes(originalCall)) {
    throw new Error('Portable TH06 regressed to the unpatched rank AddString callsite');
}

const leftAlignPatches = [
    ['result_shottype_leftalign_1', 'Rx2bd16', '468e'],
    ['result_shottype_leftalign_2', 'Rx2bd5b', '018e'],
    ['result_shottype_leftalign_3', 'Rx2e0cf', '8d6a'],
    ['result_shottype_leftalign_4', 'Rx2e0fc', '606a'],
];
for (const [name, addr, code] of leftAlignPatches) {
    const escapedName = name.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
    const escapedAddr = addr.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
    const escapedCode = code.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
    const pattern = new RegExp(`"${escapedName}"\\s*:\\s*\\{\\s*"addr"\\s*:\\s*"${escapedAddr}"\\s*,\\s*"code"\\s*:\\s*"${escapedCode}"`);
    if (!pattern.test(upstreamVersion))
        throw new Error(`TH06 v1.02h ${name} address/code drifted`);
}

function countCharacterTextCalls(source, functionName) {
    const pattern = new RegExp(
        `g_AnmManager->${functionName}\\([^;]+g_CharacterList\\[[^;]+\\);`,
        'gs',
    );
    return [...source.matchAll(pattern)].length;
}

const originalCentered = countCharacterTextCalls(originalSource, 'DrawStringFormat2');
if (originalCentered !== 4)
    throw new Error(`Expected four original centered Result shot-type calls, found ${originalCentered}`);

const helperCalls = [...portableSource.matchAll(/DrawResultShotTypeText\([^;]+LocalizedStatsCharacterName\([^;]+\);/gs)].length;
if (helperCalls !== 4)
    throw new Error(`Portable TH06 must route four Result shot-type callsites through localized Stats names, found ${helperCalls}`);

const helperStart = portableSource.indexOf('static void DrawResultShotTypeText(');
const helperEnd = portableSource.indexOf('\n#ifdef TH_DEV_TOOLS\nstatic bool g_DebugStatsAuditRequested', helperStart);
if (helperStart < 0 || helperEnd < 0)
    throw new Error('Portable TH06 is missing DrawResultShotTypeText');
const helper = portableSource.slice(helperStart, helperEnd);
if (!helper.includes('Localization::Active()') ||
    !helper.includes('if (localized)') ||
    !helper.includes('DrawVmTextFmt(') ||
    !helper.includes('else') ||
    !helper.includes('DrawStringFormat2(')) {
    throw new Error('DrawResultShotTypeText must left-align only while localization is active and preserve original centering otherwise');
}

if (!upstreamBase.includes('"hud_force_redraw"') || !upstreamBase.includes('"code": "909090909090"'))
    throw new Error('Upstream TH06 hud_force_redraw contract is missing');
if (!/"hud_force_redraw"\s*:\s*\{\s*"addr"\s*:\s*"Rx19f81"/m.test(upstreamVersion))
    throw new Error('TH06 v1.02h hud_force_redraw address drifted');
if (!originalGui.includes('vm->currentInstruction != NULL || g_Supervisor.unk198 != 0 || g_Supervisor.IsUnknown()'))
    throw new Error('Original TH06 HUD redraw gate no longer matches reconstructed source');
if (!portableGui.includes('vm->currentInstruction != NULL || g_Supervisor.unk198 != 0 || g_Supervisor.RedrawWholeFrame()'))
    throw new Error('Portable TH06 HUD redraw gate no longer routes through RedrawWholeFrame');
if (!/u32 RedrawWholeFrame\(\) const[^]*?GCOS_DISPLAY_MINIMUM_GRAPHICS[^]*?\| 1;/.test(portableSupervisor))
    throw new Error('Portable TH06 must keep unconditional whole-frame redraw semantics required by SDL swap behavior');

if (!upstreamBase.includes('"right_align"') || !upstreamBase.includes('GetTextExtentForFontID] 83c002') ||
    !upstreamBase.includes('"center_align"') || !upstreamBase.includes('GetTextExtentForFontID] 83c002 d1f8')) {
    throw new Error('Upstream TH06 right/center alignment +2 contracts are missing');
}
if (!/"right_align"\s*:\s*\{\s*"addr"\s*:\s*"0x434d6c"/m.test(upstreamVersion) ||
    !/"center_align"\s*:\s*\{\s*"addr"\s*:\s*"0x434f3c"/m.test(upstreamVersion)) {
    throw new Error('TH06 v1.02h right/center alignment addresses drifted');
}
const activeMeasurePlusTwo = [...portableAnm.matchAll(/Localization::Active\(\) \? TextHelper::MeasureTextWidth\(buf, vm->fontHeight\) \+ 2\.0f/g)].length;
if (activeMeasurePlusTwo < 2)
    throw new Error(`Portable TH06 must apply the thcrap +2 alignment compensation in both right and center paths; found ${activeMeasurePlusTwo}`);
if (!portableAnm.includes(': (f32)strlen(buf) * (f32)(fontWidth + 1) / 2.0f;'))
    throw new Error('Portable TH06 must preserve the original strlen-based alignment fallback while localization is inactive');

const spellGeometryPatches = [
    ['spell_draw_leftaligned', '0x417cc1'],
    ['spell_width', '0x417b92'],
    ['spell_width', '0x417cc8'],
    ['bomb_pos', '0x41afd8'],
    ['spell_pos', '0x41b08c'],
];
for (const [name, addr] of spellGeometryPatches) {
    if (!upstreamBase.includes(`"${name}"`) || !upstreamVersion.includes(`"${addr}"`))
        throw new Error(`Upstream TH06 ${name} contract/address ${addr} is missing`);
}
if (!upstreamBase.includes('"spell_pos_reset"') ||
    !upstreamVersion.includes('"0x41b056"') || !upstreamVersion.includes('"0x41b104"')) {
    throw new Error('Upstream TH06 spell_pos_reset contract is missing');
}

const requiredGuiSpellGeometry = [
    'TextHelper::MeasureTextWidth(bombName',
    'TextHelper::MeasureTextWidth(displayName',
    'SetActiveSpriteWidth(&this->impl->bombSpellcardName, spriteWidth)',
    'SetActiveSpriteWidth(&this->impl->enemySpellcardName, spriteWidth)',
    'this->bombSpellcardBarLength = spriteWidth + 16.0f',
    'this->blueSpellcardBarLength = spriteWidth + 16.0f',
    'nameDraw.pos.x += -nameDraw.sprite->textureWidth + nameDraw.sprite->widthPx / 2.0f',
    'nameDraw.pos.x += nameDraw.sprite->textureWidth - nameDraw.sprite->widthPx / 2.0f',
    'backgroundDraw.pos.x += (nameDraw.sprite->widthPx - this->bombSpellcardBarLength) / 2.0f',
    'backgroundDraw.pos.x += (nameDraw.sprite->widthPx - this->blueSpellcardBarLength) / 2.0f',
];
for (const contract of requiredGuiSpellGeometry) {
    if (!portableGui.includes(contract))
        throw new Error(`Portable TH06 spell/bomb geometry contract drifted: ${contract}`);
}
if ((portableGui.match(/g_AnmManager->DrawNoRotation\(&nameDraw\);/g) ?? []).length < 2)
    throw new Error('Portable TH06 spell/bomb localized drawing must use temporary VM copies without mutating ANM positions');

const generalStringlocs = [
    ['Rx6a98c', 'th06 BGM In-game format'],
    ['Rx6a3f4', 'th06 Bomb Reimu A'],
    ['Rx6a410', 'th06 Bomb Reimu B'],
    ['Rx6a420', 'th06 Bomb Marisa A'],
    ['Rx6a450', 'th06 Bomb Marisa B'],
    ['Rx6bb98', 'th06 Stats ReimuA'],
    ['Rx6bb88', 'th06 Stats ReimuB'],
    ['Rx6bb74', 'th06 Stats MarisaA'],
    ['Rx6bb60', 'th06 Stats MarisaB'],
];
for (const [addr, id] of generalStringlocs) {
    if (!upstreamStringlocs.includes(`"${addr}": "${id}"`))
        throw new Error(`TH06 general stringloc contract drifted: ${addr} -> ${id}`);
}
if ((portableGui.match(/StringById\("th06 BGM In-game format", TH_SONG_NAME\)/g) ?? []).length !== 2)
    throw new Error('Portable TH06 must localize both in-game BGM format callsites through EST1');
for (const [id, fallback] of [
    ['th06 Bomb Reimu A', 'TH_REIMU_A_BOMB_NAME'],
    ['th06 Bomb Reimu B', 'TH_REIMU_B_BOMB_NAME'],
    ['th06 Bomb Marisa A', 'TH_MARISA_A_BOMB_NAME'],
    ['th06 Bomb Marisa B', 'TH_MARISA_B_BOMB_NAME'],
]) {
    if (!portableBomb.includes(`Localization::StringById("${id}", ${fallback})`))
        throw new Error(`Portable TH06 Bomb EST1 consumer is missing: ${id}`);
}
for (const id of ['th06 Stats ReimuA', 'th06 Stats ReimuB', 'th06 Stats MarisaA', 'th06 Stats MarisaB']) {
    if (!portableSource.includes(`"${id}"`))
        throw new Error(`Portable TH06 Result Stats EST1 mapping is missing: ${id}`);
}
if (!portableSource.includes('return Localization::StringById(ids[index], g_CharacterList[index]);'))
    throw new Error('Portable TH06 Result Stats names must preserve original fallback strings through EST1 lookup');

if (!upstreamBase.includes('"unpatch_result_spell"') ||
    !upstreamBase.includes('Remove English patch spell translation lookup in the Result screen') ||
    !upstreamBase.includes('"result_spell_cap_pos_1"') ||
    !upstreamBase.includes('Move the capture rate to the right to match the maximum on-screen spell card length (#1: Save original coordinate and move)') ||
    !upstreamBase.includes('"result_spell_cap_pos_2"') ||
    !upstreamBase.includes('(#2: Restore)') ||
    !/"unpatch_result_spell"\s*:\s*\{\s*"addr"\s*:\s*"0x42e2a9"/m.test(upstreamVersion) ||
    !/"result_spell_cap_pos_1"\s*:\s*\{\s*"addr"\s*:\s*"0x42eb18"/m.test(upstreamVersion) ||
    !/"result_spell_cap_pos_2"\s*:\s*\{\s*"addr"\s*:\s*"0x42eb67"/m.test(upstreamVersion) ||
    !/"spell_name#result"\s*:\s*\{\s*"addr"\s*:\s*"0x42e2b4"/m.test(upstreamVersion)) {
    throw new Error('Upstream TH06 Result spell-name/capture-position contracts drifted');
}
if (!originalSource.includes('COLOR_RGB(COLOR_BLACK), g_GameManager.catk[i].name);') ||
    !originalSource.includes('spritePos.AsD3dXVec()->x += 368.0f;') ||
    !originalSource.includes('spritePos.AsD3dXVec()->x -= 368.0f;')) {
    throw new Error('Original TH06 Result spell display/368px capture-position baseline drifted');
}
if (!portableSource.includes('Localization::SpellName(static_cast<u32>(i), g_GameManager.catk[i].name)') ||
    !portableSource.includes('const f32 originalX = spritePos.x;') ||
    !portableSource.includes('spritePos.x += 472.0f;') ||
    !portableSource.includes('spritePos.x = originalX;') ||
    !portableSource.includes('spritePos.x += 368.0f;') ||
    !portableSource.includes('spritePos.x -= 368.0f;')) {
    throw new Error('Portable TH06 Result spell localization / Active 472px / inactive 368px contract drifted');
}

if (!upstreamBase.includes('"ending_copy_rem"') ||
    !upstreamBase.includes('Remove the 32-byte split in ending messages, #1') ||
    !upstreamBase.includes('"ending_copy_rep"') ||
    !upstreamBase.includes('Remove the 32-byte split in ending messages, #2')) {
    throw new Error('Upstream TH06 ending direct-pointer contracts are missing');
}
if (!/"ending_copy_rem"\s*:\s*\{\s*"addr"\s*:\s*"Rx103f2"/m.test(upstreamVersion) ||
    !/"ending_copy_rep"\s*:\s*\{\s*"addr"\s*:\s*"Rx104d5"/m.test(upstreamVersion) ||
    !/"unpatch_ending_halfskip"\s*:\s*\{\s*"addr"\s*:\s*"Rx10521"/m.test(upstreamVersion)) {
    throw new Error('TH06 v1.02h ending patch addresses drifted');
}
if (!originalEnding.includes('// Read 2 characters at a time') ||
    !originalEnding.includes('if (charactersReaded >= 32)') ||
    !originalEnding.includes('COLOR_END_TEXT_SHADOW, textBuffer);')) {
    throw new Error('Original TH06 32-byte/two-sprite ending baseline drifted');
}
for (const contract of [
    'static const char *FindTranslatedEndingLine',
    "while (*scan != '\\0' && *scan != '\\n' && *scan != '\\r')",
    "if (*scan != '\\0')",
    'COLOR_END_TEXT_SHADOW, "%s", translatedLine',
    'COLOR_END_TEXT_SHADOW, textBuffer',
    'charactersReaded = static_cast<i32>(translatedLineEnd - translatedLine);',
    'this->endFileDataPtr = translatedLineEnd;',
]) {
    if (!portableEnding.includes(contract))
        throw new Error(`Portable TH06 ending direct-pointer contract drifted: ${contract}`);
}
if (!portableEnding.includes('// Read 2 characters at a time') ||
    !portableEnding.includes('if (charactersReaded >= 32)')) {
    throw new Error('Portable TH06 must preserve the vanilla CP932 ending fallback path while localization is inactive');
}
if (!/u8 \*FileSystem::OpenRuntimeOverride\(const char \*filepath\)[^]*?#ifndef TH_ENABLE_THCRAP[^]*?\(void\)filepath;[^]*?return NULL;[^]*?#else[^]*?thcrap\/th06\//.test(portableFileSystem)) {
    throw new Error('Portable TH06 must disable the entire runtime-override namespace in THCRAP=OFF builds');
}

for (const [name, addr] of [
    ['sprintf_replay_1', 'Rx2c6dd'],
    ['sprintf_replay_2', 'Rx2cd4a'],
    ['sprintf_replay_3', 'Rx382bd'],
]) {
    const re = new RegExp(`"${name}"\\s*:\\s*\\{\\s*"addr"\\s*:\\s*"${addr}"`);
    if (!re.test(upstreamVersion))
        throw new Error(`TH06 ${name} address drifted`);
}
if (!upstreamBase.includes('"title": "Replay name sprintf 1"') ||
    !upstreamBase.includes('"title": "Replay name sprintf 2"') ||
    !upstreamBase.includes('"title": "Replay name sprintf 3"')) {
    throw new Error('Upstream TH06 replay sprintf hook titles drifted');
}
const replayFormat = '"./replay/th6_%.2d.rpy"';
if ((originalSource.match(/sprintf\([^\n]*"\.\/replay\/th6_%\.2d\.rpy"/g) ?? []).length !== 2 ||
    (originalMainMenu.match(/sprintf\([^\n]*"\.\/replay\/th6_%\.2d\.rpy"/g) ?? []).length !== 1) {
    throw new Error('Original TH06 replay sprintf callsites no longer classify as the two Result + one MainMenu replay file paths');
}
if ((portableSource.match(/std::sprintf\([^\n]*"\.\/replay\/th6_%\.2d\.rpy"/g) ?? []).length !== 2 ||
    (portableMainMenu.match(/std::sprintf\([^\n]*"\.\/replay\/th6_%\.2d\.rpy"/g) ?? []).length !== 1) {
    throw new Error('Portable TH06 must preserve the literal replay filename format instead of localizing file paths');
}
if (!portableSource.includes('FileSystem::OpenPath(replayToReadPath, 1)') ||
    !portableSource.includes('ReplayManager::SaveReplay(replayPath, this->replayName)') ||
    !portableMainMenu.includes('FileSystem::OpenPath(replayFilePath, 1)') ||
    !portableFileSystem.includes('const std::string prefPath = GetPrefPath(filepath);')) {
    throw new Error('Portable TH06 replay storage must remain behind FileSystem rather than language-string path translation');
}

if (!upstreamGlobal.includes('"reacquire_input"') ||
    !upstreamGlobal.includes('"title": "Fix input glitching out in TH06/TH07"') ||
    !upstreamGlobal.includes('"code": "00000000 74"') ||
    !/"reacquire_input"\s*:\s*\{\s*"addr"\s*:\s*"Rx1dc58"/m.test(upstreamVersion)) {
    throw new Error('Upstream TH06 reacquire_input contract drifted');
}
if (!originalController.includes('if (res == DIERR_INPUTLOST)') ||
    !originalController.includes('g_Supervisor.keyboard->Acquire();') ||
    !originalController.includes('return Controller::GetControllerInput(buttons);')) {
    throw new Error('Original TH06 DirectInput keyboard reacquire baseline drifted');
}
if (!portableController.includes('keyboardState = (u8 *)SDL_GetKeyboardState(NULL);') ||
    !portableController.includes('if (keyboardState != NULL)') ||
    !portableController.includes('SDL_GetGamepadButton(g_Supervisor.gameController') ||
    !portableController.includes('buttons |= EaglerOptions::BrowserKeyboardBits();') ||
    !portableController.includes('buttons |= EaglerOptions::BrowserGamepadDirectionBits();') ||
    !portableController.includes('buttons = Controller::GetControllerInput(buttons);') ||
    !portableController.includes('buttons |= Touch::GetButtonBits();') ||
    !portableController.includes('return Netplay::Input::ResolveLocal(buttons);') ||
    !portableController.includes('return buttons;')) {
    throw new Error('Portable TH06 SDL input boundary no longer structurally supersedes DirectInput reacquire_input');
}

if (!/"sprintf_call_ebp-50"\s*:\s*\{\s*"addr"\s*:\s*"Rx34c8b"/m.test(upstreamVersion) ||
    !/"sprintf_rep"\s*:\s*\{\s*"addr"\s*:\s*\[\s*"Rx34d8e",\s*"Rx3830c"\s*\]/m.test(upstreamVersion) ||
    !upstreamGlobal.includes('"title": "Safe sprintf (ebp-50)"') ||
    !upstreamGlobal.includes('"title": "Safe sprintf (replace)"')) {
    throw new Error('Upstream TH06 DrawStringFormat/replay sprintf replacement contract drifted');
}
if (!originalAnmManager.includes('void AnmManager::DrawStringFormat(') ||
    !originalAnmManager.includes('vsprintf(buf, fmt, args);')) {
    throw new Error('Original TH06 DrawStringFormat vsprintf baseline drifted');
}
if (!portableAnmManager.includes('void AnmManager::DrawStringFormat(') ||
    !portableAnmManager.includes('static std::vector<char> FormatThcrapAnmText(const char *format, va_list args)') ||
    !portableAnmManager.includes('const int length = std::vsnprintf(nullptr, 0, format, measureArgs);')) {
    throw new Error('Portable TH06 DrawStringFormat dynamic THCRAP formatter contract drifted');
}
if ((portableGui.match(/DrawStringFormat\(/g) ?? []).length !== 5 ||
    !portableGui.includes('DrawStringFormat(&this->impl->enemySpellcardName, 0xfff0f0, COLOR_RGB(COLOR_BLACK), "%s",\n                                       displayName);') ||
    !portableGui.includes('Localization::StringById("th06 BGM In-game format", TH_SONG_NAME)') ||
    (portableGui.match(/Localization::MusicTitle\(/g) ?? []).length < 2 ||
    !portableGui.includes('DrawStringFormat(&this->msg.introLines[args->text.textLine],') ||
    !portableGui.includes('this->msg.textColorsB[args->text.textColor], "%s", args->text.text);') ||
    !portableGui.includes('this->msg.textColorsB[args->text.textColor], args->text.text);')) {
    throw new Error('Portable TH06 DrawStringFormat callsite localization decomposition drifted');
}

for (const [name, expected] of [
    ['buffer_overflow_rem', ['0x434b85', '0x434e63']],
    ['buffer_overflow_rep_eax', ['0x434ba0', '0x434f74']],
    ['buffer_overflow_rep_ecx', ['0x434d17', '0x434ef7']],
]) {
    if (!upstreamBase.includes(`"${name}"`) || !expected.every(addr => upstreamVersion.includes(`"${addr}"`)))
        throw new Error(`Upstream TH06 ${name} contract/address drifted`);
}
if (!upstreamBase.includes('Fix buffer overflows (remove copy)') ||
    !upstreamBase.includes('Fix buffer overflows (replace EAX)') ||
    !upstreamBase.includes('Fix buffer overflows (replace ECX)')) {
    throw new Error('Upstream TH06 buffer-overflow direct-pointer intent drifted');
}
if ((originalAnmManager.match(/char buffer\[64\]|char buf\[64\]/g) ?? []).length < 3 ||
    (originalAnmManager.match(/vsprintf\(/g) ?? []).length < 3) {
    throw new Error('Original TH06 64-byte text formatter overflow baseline drifted');
}
if (!upstreamStrings.includes('int str_len = vsnprintf(NULL, 0, format, va2);') ||
    !upstreamStrings.includes('strings_storage_resize(ret, str_len);')) {
    throw new Error('Upstream TH06 strings_vsprintf dynamic storage contract drifted');
}
if ((portableAnmManager.match(/char buffer\[1024\]|char buf\[1024\]/g) ?? []).length !== 3 ||
    (portableAnmManager.match(/FormatThcrapAnmText\(fmt, (?:argptr|args)\)/g) ?? []).length !== 3 ||
    !portableAnmManager.includes('std::vector<char> output(static_cast<std::size_t>(length) + 1u);')) {
    throw new Error('Portable TH06 THCRAP-dynamic / strict-OFF-1024 formatter split drifted');
}
for (const typedDataContract of [
    'COLOR_END_TEXT_SHADOW, "%s", translatedLine',
    'this->msg.textColorsB[args->text.textColor], "%s", args->text.text',
    'COLOR_RGB(COLOR_BLACK), "%s", text',
    'COLOR_RGB(COLOR_BLACK), "%s", spellName',
]) {
    const sources = [portableEnding, portableGui, portableSource];
    if (!sources.some(source => source.includes(typedDataContract)))
        throw new Error(`Translated text must be data, not a printf format: ${typedDataContract}`);
}

if (!upstreamBase.includes('"text_1024"') ||
    !upstreamBase.includes('Enlarge the width of the text surface to 1024 pixels') ||
    !/"text_1024"\s*:\s*\{\s*"addr"\s*:\s*"Rx1f014"/m.test(upstreamVersion) ||
    !upstreamBase.includes('"text_prepare_surface_width"') ||
    !/"text_prepare_surface_width"\s*:\s*\{\s*"addr"\s*:\s*\["Rx1f0dd", "Rx1f16a"\]/m.test(upstreamVersion) ||
    !upstreamBase.includes('"text_sprite_height"') ||
    !upstreamBase.includes('Use sprite height for text sprites, not texture height') ||
    !upstreamBase.includes('"text_sprite_width"') ||
    !upstreamBase.includes('Calculate text alignment based on sprite width, not texture width') ||
    !upstreamBase.includes('"text_scale_x"') ||
    !upstreamBase.includes('Fix text scaling, #1: Correct width') ||
    !upstreamBase.includes('"text_scale_y"') ||
    !upstreamBase.includes('Hardcode the DIB copy region height to 32')) {
    throw new Error('Upstream TH06 widened text-surface/sprite/scaling contracts drifted');
}
if (!originalTextHelper.includes('CreateImageSurface(GAME_WINDOW_WIDTH, TEXT_BUFFER_HEIGHT') ||
    !originalTextHelper.includes('textHelper.AllocateBufferWithFallback(textSurfaceDesc.Width, textSurfaceDesc.Height') ||
    !originalTextHelper.includes('srcRect.right = spriteWidth * 2 - 2;') ||
    !originalTextHelper.includes('srcRect.bottom = fontHeight * 2 - 2;')) {
    throw new Error('Original TH06 640px scratch / surface-sized DIB / -2 copy baseline drifted');
}
if (!portableTextHelper.includes('const i32 bufferWidth = std::max(GAME_WINDOW_WIDTH, 1024);') ||
    !portableTextHelper.includes('CreateLocalizedGdiSurface(bufferWidth)') ||
    !portableTextHelper.includes('const i32 surfaceWidth = g_TextBufferSurface->w;') ||
    !portableTextHelper.includes('finalCopySrc.w = Localization::Active() ? spriteWidth * 2 : spriteWidth * 2 - 2;') ||
    !portableTextHelper.includes('finalCopySrc.h = Localization::Active() ? 32 : fontHeight * 2 - 2;')) {
    throw new Error('Portable TH06 widened scratch/GDI/scaling contract drifted');
}
if (!portableTextHelper.includes('static bool FontRasterFitsTh06Copy(TTF_Font *font, i32 targetCellHeight)') ||
    !portableTextHelper.includes('TTF_RenderText_Blended(font, "Agypqj", 0, white)') ||
    !portableTextHelper.includes('return bottomInkRow >= 0 && bottomInkRow < 32;') ||
    !portableTextHelper.includes('if (FontRasterFitsTh06Copy(font, targetHeight))') ||
    !portableTextHelper.includes('!FontRasterFitsTh06Copy(font, targetHeight)')) {
    throw new Error('Portable TH06 SDL_ttf fallback raster-fit calibration drifted');
}
const activeSpriteDimensions =
    'const i32 spriteWidth = Localization::Active() ? static_cast<i32>(vm->sprite->widthPx)';
if ((portableAnmManager.match(new RegExp(activeSpriteDimensions.replace(/[.*+?^${}()|[\]\\]/g, '\\$&'), 'g')) ?? []).length < 3 ||
    (portableAnmManager.match(/const i32 spriteHeight = Localization::Active\(\) \? static_cast<i32>\(vm->sprite->heightPx\)/g) ?? []).length < 3) {
    throw new Error('Portable TH06 text_sprite_width/text_sprite_height consumers drifted');
}

for (const [name, addresses, code] of [
    ['dialog_box_leftedge', ['Rx191ee', 'Rx1927c'], 'a1 88456d00 8d807c880100'],
    ['dialog_box_rightedge_1', ['Rx19232', 'Rx192c3'], 'd820 90909090'],
    ['dialog_box_rightedge_2', ['Rx19244', 'Rx192d5'], 'd800 90909090'],
]) {
    if (!upstreamVersion.includes(`"${name}"`) ||
        !addresses.every(addr => upstreamVersion.includes(`"${addr}"`)) ||
        !upstreamVersion.includes(`"code": "${code}`)) {
        throw new Error(`Upstream TH06 ${name} geometry contract drifted`);
    }
}
const originalDialogueWidth = '(g_GameManager.arcadeRegionSize.x - 256.0f) / 2.0f';
if ((originalGui.match(new RegExp(originalDialogueWidth.replace(/[.*+?^${}()|[\]\\]/g, '\\$&'), 'g')) ?? []).length !== 4 ||
    !originalGui.includes('256.0f + 16.0f')) {
    throw new Error('Original TH06 hard-coded 256px dialogue-box geometry drifted');
}
if (!portableGui.includes('const f32 dialogueBoxWidth = Localization::Active()') ||
    !portableGui.includes('g_AnmManager->sprites[ANM_OFFSET_TEXT + 2].endPixelInclusive.x') ||
    !portableGui.includes('(g_GameManager.arcadeRegionSize.x - dialogueBoxWidth) / 2.0f - 16.0f') ||
    !portableGui.includes('dialogueBoxWidth + 16.0f')) {
    throw new Error('Portable TH06 dialog_box_* must derive both edges from translated text sprite 0x702 width only while localization is active');
}

if (!upstreamBase.includes('"music_room_cmt_print_empty_lines"') ||
    !upstreamBase.includes('In music comment, render text strings even if they are empty in the source file') ||
    !upstreamBase.includes('"music_room_cmt_shift_2nd_part"') ||
    !upstreamBase.includes('In music comment, shift the 2nd half of the text 247px to the left') ||
    !/"music_room_cmt_print_empty_lines"\s*:\s*\{\s*"addr"\s*:\s*\[\s*"0x00425c3a",\s*"0x004250c9"/m.test(upstreamVersion) ||
    !/"music_room_cmt_shift_2nd_part"\s*:\s*\{\s*"addr"\s*:\s*"0x425b9b"/m.test(upstreamVersion)) {
    throw new Error('Upstream TH06 Music Room comment split/empty-line contracts drifted');
}
if (!originalMusicRoom.includes('if (lineCharBuffer[0] != \'\\0\')') ||
    !originalMusicRoom.includes('this->descriptionSprites[i].flags.flag1 = 0;') ||
    !originalMusicRoom.includes('((f32)(i % 2)) * 248.0f + 96.0f') ||
    !originalMusicRoom.includes('memcpy(lineCharBuffer,') ||
    !originalMusicRoom.includes('(i % 2) * 32], 32')) {
    throw new Error('Original TH06 two-sprite/32-byte Music Room description baseline drifted');
}
if (!portableMusicRoom.includes('if (Localization::Active())') ||
    !portableMusicRoom.includes('for (i32 line = 0; line < 8; line++)') ||
    !portableMusicRoom.includes('AnmVm &textVm = musicRoom->descriptionSprites[line * 2];') ||
    !portableMusicRoom.includes('AnmVm &unusedVm = musicRoom->descriptionSprites[line * 2 + 1];') ||
    !portableMusicRoom.includes('unusedVm.flags.flag1 = 0;') ||
    !portableMusicRoom.includes('COLOR_MUSIC_ROOM_SONG_DESC_SHADOW, "%s", text);') ||
    !portableMusicRoom.includes('textVm.pos = ZunVec3(96.0f, 320.0f + line * 16.0f, 0.0f);') ||
    !portableMusicRoom.includes('const char *fallback = musicRoom->trackDescriptors[musicRoom->selectedSongIndex].description[line];') ||
    !portableMusicRoom.includes('const char *text = Localization::MusicComment(track, static_cast<u16>(line), fallback);')) {
    throw new Error('Portable TH06 localized Music Room must replace the byte-32 split with one wide sprite and look up every source line, including empty originals');
}
if (!portableMusicRoom.includes('char lineCharBuffer[64];') ||
    !portableMusicRoom.includes('(index % 2) * 248.0f + 96.0f')) {
    throw new Error('Portable TH06 must retain the vanilla two-sprite Music Room path while localization is inactive');
}

if (!upstreamBase.includes('"spell_prepare"') ||
    !upstreamBase.includes('Prepare ECL instruction register for spell ID fetching') ||
    !upstreamBase.includes('"code": "8b75e489f183c10c894de80fbf460490"') ||
    !upstreamBase.includes('"spell_fetch_id"') ||
    !upstreamBase.includes('"code": "0fbf4e0e89f083c010500fbf560c"') ||
    !/"spell_prepare"\s*:\s*\{\s*"addr"\s*:\s*"0x407509"/m.test(upstreamVersion) ||
    !/"spell_fetch_id"\s*:\s*\{\s*"addr"\s*:\s*"0x409622"/m.test(upstreamVersion)) {
    throw new Error('Upstream TH06 spell_prepare/spell_fetch_id contract drifted');
}
if (!originalEclManager.includes('g_Gui.ShowSpellcard(instruction->args.spellcardStart.spellcardSprite,') ||
    !originalEclManager.includes('instruction->args.spellcardStart.spellcardName);') ||
    !originalEclManager.includes('g_EnemyManager.spellcardInfo.idx = instruction->args.spellcardStart.spellcardId;')) {
    throw new Error('Original TH06 ECL spell start sprite/name call + separate typed spell ID baseline drifted');
}
if (!portableEclManager.includes('g_Gui.ShowSpellcard(instruction->args.spellcardStart.spellcardSprite,') ||
    !portableEclManager.includes('instruction->args.spellcardStart.spellcardId,') ||
    !portableEclManager.includes('instruction->args.spellcardStart.spellcardName);') ||
    !portableGui.includes('void Gui::ShowSpellcard(i32 spellcardSprite, i32 spellcardId, const char *spellcardName)') ||
    !portableGui.includes('Localization::SpellName(static_cast<u32>(spellcardId), spellcardName)')) {
    throw new Error('Portable TH06 must carry the typed ECL spell ID directly into the spell-name lookup instead of emulating register-carry binhacks');
}
if (!portableEclManager.includes('strcpy(local_70->name, instruction->args.spellcardStart.spellcardName);')) {
    throw new Error('Portable TH06 spell display localization must not overwrite CATK persistence with translated names');
}

if (!upstreamBase.includes('"sprite3d_rotated_voodookill"') ||
    !upstreamBase.includes('Correctly scale rotated sprites in 3D space from textures wider than 256 pixels') ||
    !upstreamBase.includes('"sprite3d_unrotated_voodookill"') ||
    !upstreamBase.includes('Correctly scale unrotated sprites in 3D space from textures wider than 256 pixels') ||
    !/"sprite3d_rotated_voodookill"\s*:\s*\{\s*"addr"\s*:\s*"Rx33193"/m.test(upstreamVersion) ||
    !/"sprite3d_unrotated_voodookill"\s*:\s*\{\s*"addr"\s*:\s*"Rx33709"/m.test(upstreamVersion)) {
    throw new Error('Upstream TH06 sprite3d texture-size correction contract drifted');
}
if (!portableAnmManager.includes('worldTransformMatrix.m[0][0] *= vm->sprite->textureWidth / 256.0f;') ||
    !portableAnmManager.includes('worldTransformMatrix.m[1][1] *= vm->sprite->textureHeight / 256.0f;') ||
    (portableAnmManager.match(/worldTransformMatrix\.m\[0\]\[0\] \*= vm->sprite->textureWidth \/ 256\.0f;/g) ?? []).length !== 2 ||
    (portableAnmManager.match(/worldTransformMatrix\.m\[1\]\[1\] \*= vm->sprite->textureHeight \/ 256\.0f;/g) ?? []).length !== 2) {
    throw new Error('Portable TH06 rotated/unrotated 3D paths must compensate their fixed 256px vertex basis with actual backing texture dimensions');
}
if (!originalAnmManager.includes('vm->matrix.m[0][0] = vm->sprite->widthPx / vm->sprite->textureWidth;') ||
    !originalAnmManager.includes('vm->matrix.m[1][1] = vm->sprite->heightPx / vm->sprite->textureHeight;') ||
    !upstreamTextImage.includes('s->abs_w = (float)sprite_w;') ||
    !upstreamTextImage.includes('s->abs_h = (float)sprite_h;') ||
    !upstreamTextImage.includes('s->thtx_w = (float)srcinfo.Width;') ||
    !upstreamTextImage.includes('s->thtx_h = (float)srcinfo.Height;')) {
    throw new Error('TH06 textimage source contract must preserve logical sprite size separately from backing texture size');
}
if (!portableLocalization.includes('g_AnmManager->SetActiveSprite(vm, spriteSlot)') ||
    portableLocalization.includes('vm->matrix.m[0][0] = static_cast<f32>(width) / 256.0f;') ||
    portableLocalization.includes('vm->matrix.m[1][1] = static_cast<f32>(height) / 256.0f;') ||
    portableLocalization.includes('vm->matrix.m[0][0] = 384.0f / 256.0f;') ||
    portableLocalization.includes('vm->matrix.m[1][1] = 32.0f / 256.0f;')) {
    throw new Error('Portable TH06 textimages must not pre-apply the 256px basis correction before Draw2/Draw3; that would double-scale stage/BGM images');
}
if (!portableAnmManager.includes('scaledXCenter = vm->sprite->widthPx * drawScaleX / 2.0f;') ||
    !portableAnmManager.includes('scaledYCenter = vm->sprite->heightPx * drawScaleY / 2.0f;') ||
    !portableAnmManager.includes('worldTransformMatrix.m[3][0] += (vm->sprite->widthPx * drawScaleX) / 2.0f;') ||
    !portableAnmManager.includes('worldTransformMatrix.m[3][1] -= (vm->sprite->heightPx * drawScaleY) / 2.0f;')) {
    throw new Error('Portable TH06 3D anchor offsets must remain based on actual sprite width/height while applying texture-basis correction');
}

if (!upstreamBase.includes('"remove_score_cap"') ||
    !upstreamBase.includes('Don\'t cap the score at 999,999,990') ||
    !upstreamBase.includes('"code": "ffffff7f"') ||
    !/"remove_score_cap"\s*:\s*\{\s*"addr"\s*:\s*\["0x41b8b0", "0x41b8bc"\]/m.test(upstreamVersion)) {
    throw new Error('Upstream TH06 remove_score_cap contract drifted');
}
if (!originalGameManager.includes('#define MAX_SCORE 999999999') ||
    !originalGameManager.includes('if (gameManager->score >= MAX_SCORE + 1)') ||
    !originalGameManager.includes('gameManager->score = MAX_SCORE - 9;')) {
    throw new Error('Original TH06 1,000,000,000 -> 999,999,990 score-cap baseline drifted');
}
if (!portableGameManager.includes('static u32 ApplyScoreCap(u32 score)') ||
    !portableGameManager.includes('#ifdef TH_ENABLE_THCRAP') ||
    (portableGameManager.match(/constexpr u32 SCORE_CAP_TRIGGER = 0x7fffffffu;/g) ?? []).length !== 1 ||
    (portableGameManager.match(/constexpr u32 SCORE_CAP_VALUE = 0x7fffffffu;/g) ?? []).length !== 1 ||
    !portableGameManager.includes('constexpr u32 SCORE_CAP_TRIGGER = 1000000000u;') ||
    !portableGameManager.includes('constexpr u32 SCORE_CAP_VALUE = 999999990u;') ||
    !portableGameManager.includes('gameManager->score = ApplyScoreCap(gameManager->score);')) {
    throw new Error('Portable TH06 score cap must use exact base_tsa INT32_MAX immediates only in THCRAP-enabled builds and preserve the original cap otherwise');
}

if (!upstreamBase.includes('"file_remove_size_assignment"') ||
    !upstreamBase.includes('Remove the assignment of the file size global, which we did in th06_file_load before') ||
    !/"file_remove_size_assignment"\s*:\s*\{\s*"addr"\s*:\s*"Rx1e399"/m.test(upstreamVersion) ||
    !/"th06_file_name"\s*:\s*\{\s*"addr"\s*:\s*"0x41e370"/m.test(upstreamVersion) ||
    !/"th06_file_size"\s*:\s*\{\s*"addr"\s*:\s*"0x43cb91"/m.test(upstreamVersion) ||
    !/"th06_file_load"\s*:\s*\{\s*"addr"\s*:\s*"0x43cbac"/m.test(upstreamVersion) ||
    !/"th06_file_loaded"\s*:\s*\{\s*"addr"\s*:\s*"0x43ce19"/m.test(upstreamVersion) ||
    !upstreamBase.includes('"set_png_buff_size"') ||
    !upstreamBase.includes('Remove a buffer overflow check in PNG reading (because we usually provide a bigger buffer)') ||
    !upstreamBase.includes('"code": "baffffff7f90"') ||
    !/"set_png_buff_size"\s*:\s*\{\s*"addr"\s*:\s*\[\s*"0x4319dc",\s*"0x431aac"\s*\]/m.test(upstreamVersion)) {
    throw new Error('Upstream TH06 file injection / PNG buffer-size contracts drifted');
}
if (!originalFileSystem.includes('g_LastFileSize = g_Pbg3Archives[pbg3Idx]->GetEntrySize(entryIdx);') ||
    !originalFileSystem.includes('g_LastFileSize = fsize;')) {
    throw new Error('Original TH06 g_LastFileSize assignment baseline drifted');
}
if (!portableFileSystem.includes('if (allowRuntimeOverride)') ||
    !portableFileSystem.includes('if (u8 *overrideData = FileSystem::OpenRuntimeOverride(filepath))') ||
    !portableFileSystem.includes('g_LastFileSize = static_cast<u32>(size);') ||
    !portableFileSystem.includes('g_LastFileWasRuntimeOverride = true;') ||
    !portableFileSystem.includes('g_LastFileSize = g_Pbg3Archives[pbg3Idx]->GetEntrySize(entryIdx);') ||
    !portableFileSystem.includes('g_LastFileSize = static_cast<u32>(fsize);')) {
    throw new Error('Portable TH06 file layer must own replacement selection and exact byte-size publication instead of emulating injected file breakpoints');
}
if (!portableAnmManager.includes('rwData = SDL_IOFromConstMem(data, g_LastFileSize);') ||
    !portableAnmManager.includes('imageSrcSurface = IMG_Load_IO(rwData, true);') ||
    portableAnmManager.includes('SDL_IOFromConstMem(data, 0x7fffffff') ||
    portableAnmManager.includes('SDL_IOFromConstMem(data, 2147483647')) {
    throw new Error('Portable TH06 PNG/image loader must pass the exact replacement byte size to bounded SDL IO, never the legacy thcrap 0x7fffffff fake size');
}

if (!upstreamBase.includes('"ascii_patch_1"') ||
    !upstreamBase.includes('Hook ZUN\'s variadic ASCII printing function to perform a bunch of intricate hacks in C++ code (#1: Call, and write the inner function)') ||
    !upstreamBase.includes('"ascii_patch_2"') ||
    !upstreamBase.includes('Hook ZUN\'s variadic ASCII printing function to perform a bunch of intricate hacks in C++ code (#2: Return from the inner function)') ||
    !/"ascii_patch_1"\s*:\s*\{\s*"addr"\s*:\s*"Rx1667"/m.test(upstreamVersion) ||
    !/"ascii_patch_2"\s*:\s*\{\s*"addr"\s*:\s*"Rx1694"/m.test(upstreamVersion)) {
    throw new Error('Upstream TH06 ascii_patch_1/2 contract drifted');
}
if (!originalAsciiManager.includes('void AsciiManager::AddFormatText(D3DXVECTOR3 *position, const char *fmt, ...)') ||
    !originalAsciiManager.includes('vsprintf(tmpBuffer, fmt, args);') ||
    !originalAsciiManager.includes('AddString(position, tmpBuffer);')) {
    throw new Error('Original TH06 variadic ASCII formatter baseline drifted');
}
if (!portableAsciiManager.includes('void AsciiManager::AddFormatText(const ZunVec3 *position, const char *fmt, ...)') ||
    !portableAsciiManager.includes('FormatLocalizedAsciiText(tmpBuffer, sizeof(tmpBuffer), fmt, args, known, entry)') ||
    !portableAsciiManager.includes('Localization::AsciiString(value)') ||
    !portableAsciiManager.includes('Localization::LookupAscii(format, entry)') ||
    !portableAsciiManager.includes('DebugLocalizedFormatSelfTest()')) {
    throw new Error('Portable TH06 AddFormatText must structurally implement ascii_vpatchf format lookup, %s argument lookup, layout hacks, and a permanent self-test');
}

if (!upstreamBase.includes('"th06_time_fix"') ||
    !upstreamBase.includes('"time": "eax"') ||
    !/"th06_time_fix"\s*:\s*\{\s*"addr"\s*:\s*"0x4208fb"/m.test(upstreamVersion)) {
    throw new Error('Upstream TH06 th06_time_fix breakpoint contract drifted');
}
if (!originalGameWindow.includes('timeBeginPeriod(1);') ||
    !originalGameWindow.includes('slowdown = timeGetTime();') ||
    !originalGameWindow.includes('if (slowdown < g_LastFrameTime)') ||
    !originalGameWindow.includes('local_34 = fabs(slowdown - g_LastFrameTime);') ||
    !originalGameWindow.includes('g_LastFrameTime += FRAME_TIME;')) {
    throw new Error('Original TH06 WinMM frame-pacing loop around th06_time_fix drifted');
}
if (!portableGameWindow.includes('constexpr f64 baseTargetDt = 1.0 / 60.0;') ||
    !portableGameWindow.includes('const f64 targetDt = baseTargetDt * Netplay::Th06LanStageProbe::SimulationIntervalScale();') ||
    !portableGameWindow.includes('constexpr f64 targetDt = baseTargetDt;') ||
    !portableGameWindow.includes('const u64 currentCounter = SDL_GetPerformanceCounter();') ||
    !portableGameWindow.includes('static_cast<f64>(SDL_GetPerformanceFrequency())') ||
    !portableGameWindow.includes('this->accumulator += clampedElapsed;') ||
    !portableGameWindow.includes('while (this->accumulator >= targetDt)') ||
    !portableGameWindow.includes('this->accumulator -= targetDt;')) {
    throw new Error('Portable TH06 must structurally supersede the WinMM th06_time_fix site with monotonic SDL performance-counter fixed-step pacing');
}

if (!/"th06_screenshot"\s*:\s*\{[^]*?"pD3DDevice"\s*:\s*"\[0x6c6d20\]"[^]*?"addr"\s*:\s*"Rx1cb67"[^]*?"cavesize"\s*:\s*5/m.test(upstreamVersion) ||
    !upstreamScreenshot.includes('TH_EXPORT size_t BP_th06_screenshot') ||
    !upstreamScreenshot.includes("if ((GetKeyState('P') & 0x8000))") ||
    !upstreamScreenshot.includes('wchar_t dir[] = L"snapshot/th000.png";') ||
    !upstreamScreenshot.includes('constexpr size_t width = 640;') ||
    !upstreamScreenshot.includes('constexpr size_t height = 480;') ||
    !upstreamScreenshot.includes('for (int i = 0; i < 1000; i++)')) {
    throw new Error('Upstream TH06 P-key PNG snapshot breakpoint contract drifted');
}
if (!portableAnmManager.includes('void AnmManager::QueueThcrapSnapshotIfRequested()') ||
    !portableAnmManager.includes('keyboard[SDL_SCANCODE_P]') ||
    !portableAnmManager.includes('static i32 g_ThcrapSnapshotRequests = 0;') ||
    !portableAnmManager.includes('void AnmManager::TakeThcrapSnapshotIfRequested()') ||
    !portableAnmManager.includes('--g_ThcrapSnapshotRequests;') ||
    !portableAnmManager.includes('FileSystem::CreateDir("snapshot");') ||
    !portableAnmManager.includes('"snapshot/th%03d.png"') ||
    !portableAnmManager.includes('for (i32 index = 0; index < 1000; ++index)') ||
    !portableAnmManager.includes('g_GfxBackend->ReadPixels(0, 0, readWidth, readHeight, pixels.data());') ||
    !portableAnmManager.includes('SDL_CreateSurface(GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, SDL_PIXELFORMAT_RGBA32)') ||
    !portableAnmManager.includes('IMG_SavePNG(snapshot, outputPath.c_str())') ||
    !portableGameWindow.includes('g_AnmManager->QueueThcrapSnapshotIfRequested();') ||
    !portableGameWindow.includes('g_AnmManager->TakeThcrapSnapshotIfRequested();')) {
    throw new Error('Portable TH06 must sample P at 60Hz, defer backbuffer read until present, and save 000..999 640x480 PNG snapshots');
}
const snapshotCalc = portableGameWindow.indexOf('res = g_Chain.RunCalcChain();');
const snapshotQueue = portableGameWindow.indexOf('g_AnmManager->QueueThcrapSnapshotIfRequested();', snapshotCalc);
const snapshotTickReturn = portableGameWindow.indexOf('return res;', snapshotCalc);
if (snapshotCalc < 0 || snapshotQueue < snapshotCalc || snapshotQueue > snapshotTickReturn ||
    !/g_AnmManager->TakeScreenshotIfRequested\(\);\s*#ifdef TH_ENABLE_THCRAP[^]*?g_AnmManager->TakeThcrapSnapshotIfRequested\(\);\s*#endif[^]*?g_GfxBackend->SwapBuffers\(\);/m.test(portableGameWindow)) {
    throw new Error('TH06 snapshot must sample immediately after fixed-step calc and capture the completed frame immediately before SwapBuffers');
}

if (!/"gentext#stage_title"\s*:\s*\{\s*"addr"\s*:\s*"Rx1856a"/m.test(upstreamVersion) ||
    !upstreamBase.includes('"gentext#stage_title"') ||
    !upstreamBase.includes('"file": "stages.js"') ||
    !portableGui.includes('Localization::StageName(g_GameManager.currentStage,') ||
    !portableLocalization.includes('const char *Localization::StageName(std::uint32_t id, const char *fallback)')) {
    throw new Error('TH06 stage-title gentext contract must map the typed currentStage directly through the compiled stages table');
}

for (const [name, addr] of [
    ['music_title#track', '0x425a19'],
    ['music_title', '0x425a66'],
    ['th06_music_title_in_game#stage_num', '0x404510'],
    ['th06_music_title_in_game', '0x4185b0'],
    ['th06_music_title_in_game#boss', '0x418d4f'],
    ['music_cmt#track0', '0x425b87'],
    ['music_cmt#track', '0x425090'],
]) {
    const re = new RegExp(`"${name.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}"\\s*:\\s*\\{\\s*"addr"\\s*:\\s*"${addr.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}"`);
    if (!re.test(upstreamVersion))
        throw new Error(`TH06 ${name} address drifted`);
}
if (!/"music_cmt#line_num"\s*:\s*\{\s*"addr"\s*:\s*\[\s*"0x425c17"\s*,\s*"0x4250a6"\s*\]/m.test(upstreamVersion) ||
    !/"music_cmt"\s*:\s*\{\s*"addr"\s*:\s*\[\s*"0x425c67"\s*,\s*"0x4250f6"\s*\]/m.test(upstreamVersion)) {
    throw new Error('TH06 Music Room comment breakpoint addresses drifted');
}
if (!upstreamMusic.includes('void music_title_print(const char **str') ||
    !upstreamMusic.includes('*str = strings_sprintf(0, format, track_id_displayed, title);') ||
    !upstreamMusic.includes('*str = str_rep;') ||
    !portableLocalization.includes('const char *Localization::MusicTitle(std::uint32_t track, const char *fallback)') ||
    !portableLocalization.includes('const char *Localization::MusicComment(std::uint32_t track, std::uint16_t line, const char *fallback)') ||
    !portableMusicRoom.includes('const char *displayTitle = Localization::MusicTitle(') ||
    !portableMusicRoom.includes('const char *text = Localization::MusicComment(') ||
    !portableMusicRoom.includes('if (std::strcmp(text, "@") == 0)') ||
    portableMusicRoom.includes('Localization::CopyText(musicRoom->trackDescriptors') ||
    !portableGui.includes('Localization::MusicTitle(')) {
    throw new Error('Portable TH06 Music Room must late-bind translated title/comment pointers at text generation and never copy them back into fixed TrackDescriptor storage');
}

for (const [name, addr] of [
    ['textimage_set#stage', 'Rx184fd'],
    ['textimage_set#bossbgm', 'Rx18cf9'],
    ['textimage_is_active#sttitle', 'Rx18584'],
    ['textimage_is_active#boss', 'Rx18e64'],
    ['textimage_init', 'Rx2047e'],
]) {
    const re = new RegExp(`"${name.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}"\\s*:\\s*\\{\\s*"addr"\\s*:\\s*"${addr.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}"`);
    if (!re.test(upstreamVersion))
        throw new Error(`TH06 ${name} address drifted`);
}
if (!/"textimage_is_active#stlogo"\s*:\s*\{\s*"addr"\s*:\s*\["Rx1ae99", "Rx1aec3", "Rx1aee4"\]/m.test(upstreamVersion) ||
    !/"textimage_is_active#bgm"\s*:\s*\{\s*"addr"\s*:\s*\["Rx185cf", "Rx18d6b"\]/m.test(upstreamVersion)) {
    throw new Error('TH06 textimage stlogo/BGM active-check addresses drifted');
}
for (const asset of [
    '"filename": "ti_sttitle.png"',
    '"filename": "ti_stlogo.png"',
    '"filename": "ti_bgm.png"',
    '"filename": "ti_bosstitle.png"',
    '"filename": "ti_bossname.png"',
]) {
    if (!upstreamBase.includes(asset))
        throw new Error(`Upstream TH06 textimage asset contract missing: ${asset}`);
}
if (!portableLocalization.includes('LoadTextImage(47, "ti_stlogo.png"') ||
    !portableLocalization.includes('ApplyTextImage(vm, 0x7fe, 47, static_cast<i32>(stage - 1), 384, 48)') ||
    !portableLocalization.includes('LoadTextImage(47, "ti_sttitle.png"') ||
    !portableLocalization.includes('ApplyTextImage(vm, 0x7fe, 47, static_cast<i32>(stage - 1), 384, 16)') ||
    !portableLocalizationHeader.includes('bool StageLogoImageActive();') ||
    !portableGui.includes('if (Localization::StageLogoImageActive())') ||
    !portableGui.includes('if (!Localization::StageLogoImageActive())') ||
    !portableLocalization.includes('LoadTextImage(48, "ti_bgm.png"') ||
    !portableLocalization.includes('ApplyBossImage(vm, stage, 49, "ti_bosstitle.png"') ||
    !portableLocalization.includes('ApplyBossImage(vm, stage, 50, "ti_bossname.png"')) {
    throw new Error('Portable TH06 textimage priority/assets/fallback contract drifted');
}
if (!portableLocalization.includes('kMusicTitleScript[]') ||
    !portableLocalization.includes('ApplyTextImage(vm, 0x7ff, 48, row, 384, 32)') ||
    !portableLocalization.includes('ApplyTextImage(vm, spriteSlot, textureSlot, static_cast<i32>(stage - 1), 384, 64)') ||
    !portableLocalization.includes('g_BossTitleImageReady, 0x7fc);') ||
    !portableLocalization.includes('g_BossNameImageReady, 0x7fd);')) {
    throw new Error('Portable TH06 textimage private sprite/script mapping drifted');
}

if (!upstreamBase.includes('"bosstitle_line_order#1"') ||
    !upstreamBase.includes('"code": "a02a0000"') ||
    !upstreamBase.includes('"bosstitle_line_order#2"') ||
    !upstreamBase.includes('"code": "90290000"') ||
    !/"bosstitle_line_order#1"\s*:\s*\{\s*"addr"\s*:\s*"Rx19545"/m.test(upstreamVersion) ||
    !/"bosstitle_line_order#2"\s*:\s*\{\s*"addr"\s*:\s*"Rx1955c"/m.test(upstreamVersion)) {
    throw new Error('Upstream TH06 boss title line-order contract drifted');
}
const originalBossOrder =
    'g_AnmManager->DrawNoRotation(&this->msg.introLines[0]);\n    g_AnmManager->DrawNoRotation(&this->msg.introLines[1]);';
if (!originalGui.includes(originalBossOrder))
    throw new Error('Original TH06 boss intro draw order no longer matches [0,1] baseline');
if (!/if \(Localization::Active\(\)\)[^]*?DrawInterpNoRotation\(&this->msg\.introLines\[1\]\);[^]*?DrawInterpNoRotation\(&this->msg\.introLines\[0\]\);[^]*?else[^]*?DrawInterpNoRotation\(&this->msg\.introLines\[0\]\);[^]*?DrawInterpNoRotation\(&this->msg\.introLines\[1\]\);/.test(portableGui)) {
    throw new Error('Portable TH06 must swap boss title/name draw order only while localization is active');
}

// ascii_params supplies ascii_vpatchf with TH06 AsciiManager's current X/Y
// scale and character advance. Portable owns that state directly, so the typed
// formatter must derive alignment from manager->scale rather than a detached
// global breakpoint cache.
if (!/"ascii_params"\s*:\s*\{\s*"addr"\s*:\s*"Rx23aa9"/.test(upstreamVersion) ||
    !upstreamBase.includes('".Scale": "0x6228"') ||
    !upstreamBase.includes('"CharWidth": 14.0') ||
    !upstreamAscii.includes('BP_ascii_params') ||
    !portableAsciiManager.includes('float AsciiCharWidth(const AsciiManager *manager)') ||
    !portableAsciiManager.includes('return 14.0f * manager->scale.x;') ||
    !portableAsciiManager.includes('curString->scale.x = this->scale.x;') ||
    !portableAsciiManager.includes('curString->scale.y = this->scale.y;')) {
    throw new Error('TH06 ascii_params typed scale/character-width contract drifted');
}

// strings_lookup#cavesize_6 patches the fmt argument of both original
// GameErrorContext variadic entries (0x41e4d3 / 0x41e623). Portable restores
// this by mapping exact original literals to the compiled EST1 IDs, while a
// printf-signature gate prevents translated vararg type mismatches.
if (!/"strings_lookup#cavesize_6"\s*:\s*\{\s*"addr"\s*:\s*\[\s*"Rx1e623",\s*"Rx1e4d3"/.test(upstreamVersion) ||
    !portableLocalization.includes('const char *Localization::FormatStringById') ||
    !portableLocalization.includes('const char *Localization::LogString') ||
    !portableLocalization.includes('ParsePrintfSignature(fallback, fallbackSignature)') ||
    !portableLocalization.includes('fallbackSignature != translatedSignature') ||
    !portableGameErrorContext.includes('const char *localizedFmt = Localization::LogString(fmt);') ||
    (portableGameErrorContext.match(/Localization::LogString\(fmt\)/g) ?? []).length !== 2) {
    throw new Error('TH06 GameErrorContext strings_lookup#cavesize_6 contract drifted');
}
const th06ContractBlock = stringContract.match(/th06:\s*Object\.freeze\(\[([\s\S]*?)\]\),\s*\/\/ TH07/);
if (!th06ContractBlock)
    throw new Error('TH06 EST1 source-contract block missing');
const th06ContractIds = [...th06ContractBlock[1].matchAll(/\{ id: "([^"]+)"/g)].map(match => match[1]);
if (th06ContractIds.length !== 44 || new Set(th06ContractIds).size !== 44)
    throw new Error(`expected 44 unique TH06 EST1 records, got ${th06ContractIds.length}/${new Set(th06ContractIds).size}`);
const diagnosticIds = [...upstreamStringlocs.matchAll(/"(?:R|r)x[0-9a-f]+": "(th06_(?:log|error)_[^"]+)"/g)].map(match => match[1]);
if (diagnosticIds.length !== 35 || new Set(diagnosticIds).size !== 35)
    throw new Error(`expected 35 unique original TH06 diagnostic stringloc IDs, got ${diagnosticIds.length}/${new Set(diagnosticIds).size}`);
for (const id of diagnosticIds) {
    if (!th06ContractIds.includes(id) || !portableLocalization.includes(`{"${id}",`))
        throw new Error(`TH06 diagnostic strings_lookup mapping missing: ${id}`);
}

// Fail closed on the complete target-version site inventory and reverse
// portable Localization-consumer inventory as part of the normal contract
// entrypoint; this ledger must not depend on somebody remembering to run a
// second script manually.
await import('./audit-thcrap-proof-ledger.mjs');

console.log(
    'TH06 thcrap source contract PASS: result_rank_format -> AddFormatText; ' +
    'result_shottype_leftalign 4/4 -> Active left-align / inactive original centering; ' +
    'hud_force_redraw -> portable whole-frame redraw superset; ' +
    'right_align/center_align -> measured width +2; ' +
    'spell/bomb width+position -> measured/clamped temporary-VM geometry; ' +
    'EST1 BGM/Bomb/Stats + 35 Log/Fatal diagnostics -> source-proven stringloc IDs + safe printf signatures; ' +
    'Result spell -> spell_name#result + Active 472/restore / inactive 368; ' +
    'Ending -> translated NUL direct pointer / inactive 32-byte baseline; ' +
    'THCRAP OFF -> runtime override namespace disabled; ' +
    'Replay sprintf x3 -> file-path only / raw basename + FileSystem storage; ' +
    'reacquire_input -> DirectInput-only bugfix superseded by SDL keyboard/gamepad state; ' +
    'sprintf_call_ebp-50/rep -> DrawStringFormat/replay pointer hooks decomposed at typed callsites; ' +
    'buffer/text surface -> translated data uses %s; THCRAP dynamic formatter + OFF 1024 formatter baseline + 1024 scratch; Active sprite dims/full 2x32 copy; ' +
    'dialog_box_* -> Active text sprite 0x702 width / inactive 256px box; ' +
    'Music Room cmt -> Active every-line lookup + one wide sprite / inactive byte-32 two-sprite baseline; ' +
    'spell_prepare/fetch -> typed ECL spellId passed directly to display lookup; original CATK name persistence retained; ' +
    'sprite3d voodookill -> Active textureWidth/Height divided by 256 in both 3D bases; sprite-size anchors retained; ' +
    'remove_score_cap -> THCRAP build uses 0x7fffffff threshold/value; OFF keeps 1,000,000,000 -> 999,999,990; ' +
    'file hooks/set_png -> typed runtime override owns exact bytes+size; SDL image IO remains exact-bounded, no 0x7fffffff fake length; ' +
    'ascii_patch_1/2 + ascii_params -> typed AddFormatText implements ascii_vpatchf format + %s lookup/layout from live scale/14px advance; ' +
    'stage/music/textimage -> typed stage/track/line tables + optional higher-priority stlogo + private BGM/boss image slots; ' +
    'th06_time_fix -> legacy WinMM timeGetTime frame-pacing breakpoint superseded by SDL performance-counter 60Hz accumulator; ' +
    'th06_screenshot -> THCRAP-only P-key 60Hz snapshot/th000..999.png, portable backbuffer -> 640x480 PNG; ' +
    'boss title line order -> Active [1,0] / inactive [0,1]',
);
