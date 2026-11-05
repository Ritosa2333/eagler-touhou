// Extract the native purple hotkey widget; platform formatting is the only
// substitution. Never silently accept changed native source boundaries.
import { readFileSync, writeFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { createHash } from 'node:crypto';
import assert from 'node:assert/strict';

const root = resolve(import.meta.dirname, '..');
const upstream = process.argv.slice(2).find(arg => !arg.startsWith('--'));
assert(upstream, 'Pass the purple thprac checkout explicitly');
const read = file => readFileSync(resolve(upstream, file), 'utf8').replace(/^\uFEFF/, '').replaceAll('\r\n', '\n');
const sha = value => createHash('sha256').update(value).digest('hex');
const source = read('thprac/src/thprac/thprac_th07.cpp');
assert.equal(sha(source), 'b5be6dc698f574992ab49696c5c0d8ad756af93bf0760be1504ee9e958bee1b0',
  'The TH07 extraction requires the reviewed purple revision; audit changes before changing this pin');
const components = read('thprac/src/thprac/thprac_gui_components.cpp');
const definitions = JSON.parse(read('thprac/src/thprac/thprac_games_def.json'));
const glossary = Object.assign({}, ...Object.values(definitions).map(game => game.glossary ?? {}));
const tools = read('thprac/src/thprac/thprac_launcher_tools.cpp');
const toolsHeader = read('thprac/src/thprac/thprac_launcher_tools.h');
const nativeReaction = region(tools, 'THGuiTestReactionTest::THGuiTestReactionTest()', 'class THGuiRollPlayer');
const start = components.indexOf('        bool GuiHotKey::OnWidgetUpdate()');
const end = components.indexOf('        bool GuiHotKey::operator()', start);
assert(start >= 0 && end > start);
let widget = components.slice(start, end).trimEnd();
widget = widget.replace('bool GuiHotKey::OnWidgetUpdate()', 'inline bool PurpleHotkeyWidget(const char* mKeyText, const char* text, bool mStatus)');
widget = widget.replace('            const char* text = mText ? mText : LocaleGetStr(mTextRef);\n', '');
widget = widget.replace('std::format("[{}: {}]", mKeyText, text)', 'std::string("[") + mKeyText + ": " + text + "]"');
widget = widget.replace('std::format("{}: {}", mKeyText, text)', 'std::string(mKeyText) + ": " + text');
assert(!/std::format|LocaleGetStr|mTextRef|GuiHotKey::/.test(widget));
const labelKeys = ['THPRAC_FAST_RETRY', 'THPRAC_FAST_RETRY_DESC2', 'THPRAC_AUTO_SHOOT',
  'THPRAC_INGAMEINFO_07_CHERRY', 'THPRAC_INGAMEINFO_07_CHERRY_PLUS', 'THPRAC_INGAMEINFO_07_CHERRY_MAX',
  'THPRAC_INGAMEINFO', 'TH_ENEMY_MUTEKI', 'TH_ADV_DISABLE_X_KEY', 'TH_ADV_DISABLE_SHIFT_KEY',
  'TH_ADV_DISABLE_Z_KEY', 'TH_ADV_DISABLE_C_KEY_SAMETIME', 'TH_ADV_FORCE_SHIFT_KEY', 'THPRAC_INFLIVES_MAP',
  'TH_ENABLE_LOCK_TIMER', 'THPRAC_SHOW_BULLET_HITBOX', 'TH_ONE_KEY_DIE', 'TH_ONE_KEY_DIE_DESC',
  'THPRAC_KB_OPEN', 'THPRAC_KB_RECORD_START', 'THPRAC_KB_RECORD_STOP', 'THPRAC_KB_OUTPUT',
  'THPRAC_TH07_ALWAYS_SAVE_SCORE_SPELL_HIST', 'THPRAC_TH07_ALWAYS_SAVE_SCORE_SPELL_HIST_DESC',
  'THPRAC_TOOLS_REACTION_TEST', ...new Set([...nativeReaction.matchAll(/S\((TH\w+)\)/g)].map(m=>m[1]))];
const labels = [...new Set(labelKeys)].map(key => {
  assert(Array.isArray(glossary[key]) && glossary[key].length === 3, key);
  return `inline constexpr const char* purple_${key}[3]{${glossary[key].map(value => JSON.stringify(value)).join(',')}};`;
}).join('\n');
const banner = `// Generated from purple THPrac (MIT), TH07 source sha256 ${sha(source)}.\n`;
const common = resolve(process.env.EAGLER_COMMON_ROOT ?? resolve(root, '../../common'));
const commonKeys = [...new Set(['PracticeReaction.inc', 'PracticeSpeed.inc'].flatMap(file =>
  [...readFileSync(resolve(common, 'include/eagler/thprac', file), 'utf8').matchAll(/\bpractice_(TH\w+)/g)].map(m => m[1])))];
const sharedLabels = banner + '#pragma once\nnamespace PracticeRuntime {\n' + commonKeys.map(key => {
  assert(Array.isArray(glossary[key]) && glossary[key].length === 3, key);
  return `inline constexpr const char* practice_${key}[3]{${glossary[key].map(v => JSON.stringify(v)).join(',')}};`;
}).join('\n') + '\n}\n';
const header = banner + '#pragma once\n#include <imgui.h>\n#include <string>\nnamespace PracticeRuntime {\n' +
  widget + '\ninline void DrawPurpleHotkey(const char* key, const char* text, bool& status) {\n' +
  '    if (PurpleHotkeyWidget(key, text, status)) status = !status;\n}\n' + labels + '\n}\n';
const hooksSource = read('thprac/src/thprac/thprac_games_hooks.cpp');
const inputStart = hooksSource.indexOf('    if (g_input_opt.disable_xkey) {');
const inputEnd = hooksSource.indexOf('    if (g_input_opt.disable_f10_11_13)', inputStart);
assert(inputStart >= 0 && inputEnd > inputStart);
let input = hooksSource.slice(inputStart, inputEnd).trimEnd();
const scans = { X: 88, C: 67, Z: 90, LSHIFT: 160, RSHIFT: 161 };
input = input.replaceAll('g_input_opt.', '').replaceAll('((BYTE*)state)', 'state')
  .replace(/DIK_([A-Z]+)/g, (_, key) => { assert(key in scans); return scans[key]; });
assert(!/DIK_|BYTE|g_input_opt/.test(input));
const inputHeader = banner + '#pragma once\n#include <eagler/thprac/PracticeInput.hpp>\nnamespace PracticeRuntime {\nstruct PurpleInput : eagler::thprac::PracticeInput {\n' +
  ' std::uint16_t filter(std::uint16_t buttons, bool enabled, bool replay) {\n' +
  '  if (!enabled || replay) return buttons;\n' +
  '  std::uint8_t state[256]{}; state[90]=(buttons&1)?0x80:0;state[88]=(buttons&2)?0x80:0;\n' +
  '  state[160]=state[161]=(buttons&4)?0x80:0; state[27]=(buttons&8)?0x80:0; state[82]=(buttons&0x4000)?0x80:0; apply(state);\n' +
  '  return (buttons & ~std::uint16_t(0x400f)) | (state[90]?1:0) | (state[88]?2:0) | ((state[160]||state[161])?4:0) | (state[27]?8:0) | (state[82]?0x4000:0);\n' +
  ' }\n};\n}\n';
const hooks = [...source.matchAll(/\b(EHOOK_(?:DY|ST)|PATCH_DY)\((\w+),\s*(0x[0-9a-fA-F]+)/g)]
  .map(([, kind, name, address]) => ({ kind, name, address }));
const advStart = source.indexOf('    class THAdvOptWnd');
const advEnd = source.indexOf('    EHOOK_ST(th07_rb', advStart);
assert(advStart >= 0 && advEnd > advStart);
const advanced = source.slice(advStart, advEnd);
const calls = [...new Set([...advanced.matchAll(/\b(\w+Opt)\(/g)].map(match => match[1]))];
for (const required of ['GameFPSOpt', 'DisableKeyOpt', 'KeyHUDOpt', 'InfLifeOpt', 'GameplayOpt', 'InGameReactionTestOpt', 'AboutOpt'])
  assert(calls.includes(required), required);
const inventory = {
  schema: 'th07/purple-source-inventory/1', sourceSha256: sha(source),
  widgetSourceSha256: sha(components), hooks, advancedCalls: calls,
  sections: Object.keys(definitions.th07.sections),
};
const hitboxStart = source.indexOf('    void RenderBtHitbox(ImDrawList* p)');
const hitboxEnd = source.indexOf('    void RenderLockTimer(', hitboxStart);
assert(hitboxStart >= 0 && hitboxEnd > hitboxStart);
let hitbox = source.slice(hitboxStart, hitboxEnd).replace('void RenderBtHitbox(ImDrawList* p)', 'inline void RenderPurpleHitbox(ImDrawList* p, bool enabled)')
  .replace('g_show_bullet_hitbox', 'enabled')
  .replace('DWORD game_state = *(DWORD*)(0x62F648);', 'const auto game_state = g_GameManager.flags;')
  .replace('thPracParam.mode', 'PracticeRuntime::AdvancedActive()')
  .replace('ImVec2 plpos1 = *(ImVec2*)(0x4BE420);', 'ImVec2 plpos1{g_Player.hitboxTopLeft.x, g_Player.hitboxTopLeft.y};')
  .replace('ImVec2 plpos2 = *(ImVec2*)(0x004BE42C);', 'ImVec2 plpos2{g_Player.hitboxBottomRight.x, g_Player.hitboxBottomRight.y};')
  .replace('DWORD pbt = 0x0062F958 + 0xB8C0 + 0x35A000 - 0xD68 * i - 0xD68;', 'const auto& bullet = g_BulletManager.bullets[1023 - i];')
  .replace('*(DWORD*)(pbt + 0xBFC)', 'bullet.state')
  .replace('ImVec2 pos = *(ImVec2*)(pbt + 0xB8C);', 'ImVec2 pos{bullet.pos.x, bullet.pos.y};')
  .replace('ImVec2 hit = *(ImVec2*)(pbt + 0xB7C);', 'ImVec2 hit{bullet.sprites.grazeSize.x, bullet.sprites.grazeSize.y};')
  .replace('DWORD pls = 0x00995F80 + 0x4EC * i;', 'const auto& laser = g_BulletManager.lasers[i];')
  .replace('DWORD is_used = *(DWORD*)(pls + 0x4D4);', 'const auto is_used = laser.isInUse;')
  .replace('ImVec2 pos = *(ImVec2*)(pls + 0x498);', 'ImVec2 pos{laser.pos.x, laser.pos.y};');
const laserFields = {'4A4':'angle','4B4':'width','4A8':'startOffset','4AC':'endOffset','4E8':'state',
 '4C4':'hitboxStartTime','4D0':'hitboxEndTime','4E0':'timer.current','4DC':'timer.subFrame',
 '4C0':'startTime','4CC':'endTime'};
hitbox = hitbox.replace(/\*\((?:float|DWORD)\*\)\(pls \+ 0x([0-9A-F]+)\)/g, (_, offset) => {
 assert(offset in laserFields, offset); return 'laser.' + laserFields[offset];
});
assert(!/DWORD|pbt|pls|thPracParam|\*\([^\n]*\*\)/.test(hitbox), 'Unmapped native hitbox address');
function region(s,a,b) { const i=s.indexOf(a), j=s.indexOf(b,i); assert(i>=0&&j>i,a); return s.slice(i,j); }
const keySource=read('thprac/src/thprac/thprac_igi_key_render.cpp');
const keyHeader=read('thprac/src/thprac/thprac_igi_key_render.h');
const keyEnum=region(keySource,'enum THKey','void static KeysRect')
 .replace('const std::string THKeyNames[]', 'inline static const std::string THKeyNames[]');
const keyStyle=region(keyHeader,'struct KeyRectStyle','void RecordKey');
let record=region(keySource,'void RecordKey(','void KeysHUD(')
 .replace('void RecordKey(int ver, uint32_t cur_key)','void record(int ver, uint32_t cur_key)')
 .replace('    static std::deque<uint32_t> keys_per_sec; // 60 f','');
let hud=region(keySource,'void static KeysRect','void RecordKey(')+region(keySource,'void KeysHUD(','void SaveKeyRecorded()');
hud=hud.replace('    ImGuiIO& io = ImGui::GetIO();',
 '    const auto& g_keys_down=monitor.g_keys_down;\n    const auto& g_key_mask=monitor.g_key_mask;\n    const auto g_aps_cur=monitor.g_aps_cur;\n    ImGuiIO& io = ImGui::GetIO();')
 .replace('const KeyRectStyle& style, bool align_right_arrow, bool align_right_key)',
 'const KeyRectStyle& style, const PurpleKeyMonitor& monitor, bool align_right_arrow=true, bool align_right_key=false)')
 .replace('std::string aps_text = std::format("aps: {:>3}/(60frame)", g_aps_cur);',
 'char aps_buffer[64];std::snprintf(aps_buffer,sizeof(aps_buffer),"aps: %3d/(60frame)",g_aps_cur);std::string aps_text=aps_buffer;');
assert(!hud.includes('std::format'));
const monitor=banner+'#pragma once\n#include <imgui.h>\n#include <deque>\n#include <vector>\n#include <string>\n#include <cstdio>\n#include <cmath>\n#include <cstdint>\nnamespace PracticeRuntime {\n'+keyStyle+
 'struct PurpleKeyMonitor {\n'+keyEnum+
 ' bool g_record_key_aps=false;int g_aps_cur=0;std::vector<int> g_recorded_aps;std::vector<uint16_t> g_recorded_keys;std::deque<uint32_t> keys_per_sec;\n'+record+
 ' void clear_record(){g_recorded_aps.clear();g_recorded_keys.clear();}\n'+
 ' std::string csv() const {std::string out="frame,aps";for(const auto& key:THKeyNames)out+=","+key;out+="\\n";\n'+
 '  for(size_t j=0;j<g_recorded_aps.size();++j){out+=std::to_string(j+1)+","+std::to_string(g_recorded_aps[j]);for(int i=0;i<END;++i)out+=(g_recorded_keys[j]&(1<<i))?",O":",-";out+="\\n";}return out;}\n};\n'+
 hud.replace(/\bKey_(Up|Down|Left|Right|Z|X|C|D|Ctrl|Shift)\b/g,'PurpleKeyMonitor::Key_$1')+'\n}\n';
const reactionClass = toolsHeader.match(/class THGuiTestReactionTest[\s\S]*?\n};/)?.[0];
assert(reactionClass, 'Reaction class drift');
let reaction = nativeReaction.replaceAll('LARGE_INTEGER','PurpleCounter')
 .replaceAll('QueryPerformanceFrequency','purple_counter_frequency')
 .replaceAll('QueryPerformanceCounter','purple_counter_now')
 .replaceAll('GetRndGenerator','purple_random_generator').replaceAll('DWORD','unsigned')
 .replaceAll('std::fabsf','std::fabs')
 .replace('ImGui::Text(S(THPRAC_TOOLS_REACTION_TEST_TOO_EARLY));', 'ImGui::TextUnformatted(S(THPRAC_TOOLS_REACTION_TEST_TOO_EARLY));')
 .replace(/    Gui::KeyboardInputUpdate\([^\n]+\);\n/g,'')
 .replace(/bool isKeyPressed = ImGui::IsKeyDown\(37\)[\s\S]*?;/,'bool isKeyPressed = (buttons & 0xf0) != 0;')
 .replace('bool THGuiTestReactionTest::GuiUpdate(bool ingame)','inline bool THGuiTestReactionTest::GuiUpdate(bool ingame, unsigned buttons, int locale)')
 .replace('    PurpleCounter curTime;','    const auto pressedButtons = buttons & ~mPreviousButtons;\n    mPreviousButtons = buttons;\n    PurpleCounter curTime;')
 .replaceAll("ImGui::IsKeyPressed('Z')", '(pressedButtons & 1)')
 .replaceAll('ImGui::IsKeyPressed(VK_SHIFT)', '(pressedButtons & 4)')
 .replaceAll('GuiCenteredText(S(THPRAC_TOOLS_REACTION_TEST));','ImGui::TextUnformatted(S(THPRAC_TOOLS_REACTION_TEST));')
 .replaceAll('std::format("{}(framecount)", S(THPRAC_TOOLS_REACTION_TEST_RESULT)).c_str()', '(std::string(S(THPRAC_TOOLS_REACTION_TEST_RESULT))+"(framecount)").c_str()')
 .replace(/S\((TH\w+)\)/g, (_,k)=>'purple_'+k+'[locale]')
 .replace('THGuiTestReactionTest::THGuiTestReactionTest()', 'inline THGuiTestReactionTest::THGuiTestReactionTest()')
 .replace('void THGuiTestReactionTest::Reset()', 'inline void THGuiTestReactionTest::Reset()');
assert(!/GetAsyncKeyState|\bGui::|VK_|std::format|QueryPerformance|GetRndGenerator/.test(reaction.replace(/\/\/[^\n]*/g,'')), 'Unmapped reaction boundary');
const reactionHeader = banner+'#pragma once\n#include "PurpleHotkey.hpp"\n#include <SDL3/SDL.h>\n#include <functional>\n#include <random>\n#include <vector>\n#include <cmath>\n#include <algorithm>\nnamespace PracticeRuntime {\n'+
 'struct PurpleCounter { int64_t QuadPart=0; };\ninline void purple_counter_frequency(PurpleCounter* p){p->QuadPart=1000000000;}\ninline void purple_counter_now(PurpleCounter* p){p->QuadPart=SDL_GetTicksNS();}\n'+
 'inline std::function<unsigned()> purple_random_generator(unsigned lo,unsigned hi){return std::bind(std::uniform_int_distribution<unsigned>(lo,hi),std::mt19937(unsigned(SDL_GetTicksNS())));}\n'+
 reactionClass.replaceAll('LARGE_INTEGER','PurpleCounter').replace('bool GuiUpdate(bool ingame = false);','bool GuiUpdate(bool ingame, unsigned buttons, int locale);')
 .replace('    int mFrameCount;','    unsigned mPreviousButtons=0;\n    int mFrameCount;')+'\n'+reaction+'\n}\n';
const files = {
  'src/PurpleHotkey.hpp': header,
  'src/PurpleInput.hpp': inputHeader,
  'src/PurpleLicense.hpp': banner + '#pragma once\nnamespace PracticeRuntime { inline constexpr const char* purple_license = ' + JSON.stringify(read('LICENCE')) + '; }\n',
  'src/PurpleVisuals.hpp': banner + '#pragma once\n#include <cmath>\n#include <imgui.h>\n#include "BulletManager.hpp"\n#include "GameManager.hpp"\n#include "Player.hpp"\nnamespace PracticeRuntime {\n' + hitbox + '\n}\n',
  'src/PurpleSharedLabels.hpp': sharedLabels,
  'audit/source-contracts/th07-purple-inventory.json': JSON.stringify(inventory, null, 2) + '\n',
};
for (const [file, contents] of Object.entries(files)) {
  const normalized = contents.split('\n').map(line => line.trimEnd()).join('\n');
  if (process.argv.includes('--write')) writeFileSync(resolve(root, file), normalized);
  else {
    assert(process.argv.includes('--check'), 'Use --write or --check');
    assert.equal(readFileSync(resolve(root, file), 'utf8').replaceAll('\r\n', '\n'), normalized, file);
  }
}
console.log(JSON.stringify({ sourceSha256: sha(source), hooks: hooks.length, sections: inventory.sections.length, advancedCalls: calls }));
