// MIT upstream THPrac source extraction. Refuse an unreviewed source revision.
import { readFileSync, writeFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { createHash } from 'node:crypto';
import assert from 'node:assert/strict';
const root = resolve(import.meta.dirname, '..');
const upstream = process.argv.slice(2).find(x => !x.startsWith('--'));
assert(upstream, 'Pass the purple checkout explicitly');
const read = p => readFileSync(resolve(upstream, p), 'utf8').replace(/^\uFEFF/, '').replaceAll('\r\n', '\n');
const hash = s => createHash('sha256').update(s).digest('hex');
const source = read('thprac/src/thprac/thprac_th06.cpp');
assert.equal(hash(source), '114a55f6c321a83acf2dea28c29893dd2821a25b1088bdb8683bbc5918d9bb81');
for(const [name,digest] of Object.entries({
 'thprac_res.h':'f7a3a5f2ad3c1d8960725a967e572c810e94a8f351e0edcd94ba410dd128fe10',
 'thprac_games_SSS.cpp':'644b926aefe2c3e5790ddba6200ffdea6ddb76f9aef3786532d6a73db8475d09',
 'thprac_gui_components.cpp':'d88dd6483ea8cea59a1f5a71ce61306f242d63bf3862c5a2498cd73c55bd93d0',
 'thprac_games_def.json':'748ad3affe12d6f30913c6c6401a92885a89afcfb1be665406238e18d0369359'
}))assert.equal(hash(read('thprac/src/thprac/'+name)),digest,name);
const definitions=JSON.parse(read('thprac/src/thprac/thprac_games_def.json'));
const glossary = Object.assign({}, ...Object.values(definitions).map(x => x.glossary ?? {}));
const phaseKeys=['TH_BOOKS_PHASE_INF_TIME','TH_EOSD_SAKUYA_DOLLS','TH06_FINAL_SPELL'];
const phaseGroups=phaseKeys.map(key=>{
    const group=Object.values(definitions).map(x=>x.options?.[key] ?? x[key]).find(Array.isArray);
    // The native generated definitions store combo arrays under game.gui.
    const find=o=>{if(!o||typeof o!=='object')return null;if(Array.isArray(o[key]))return o[key];for(const v of Object.values(o)){const r=find(v);if(r)return r;}return null;};
    const items=group??find(definitions);assert(items,key);return [key,items];
});
const commonArg = process.argv.find(x=>x.startsWith('--common='));
const commonRoot = commonArg ? resolve(commonArg.slice('--common='.length)) : resolve(root,'third_party/eagler-common');
const reaction = readFileSync(resolve(commonRoot,'include/eagler/thprac/PracticeReaction.inc'),'utf8');
const reactionKeys = [...reaction.matchAll(/practice_(TH\w+)/g)].map(m=>m[1]);
const speed=readFileSync(resolve(commonRoot,'include/eagler/thprac/PracticeSpeed.inc'),'utf8');
const speedKeys=[...speed.matchAll(/practice_(TH\w+)/g)].map(m=>m[1]);
const sharedKeys = ['TH_ENEMY_MUTEKI','TH_ADV_DISABLE_X_KEY','TH_ADV_DISABLE_C_KEY_SAMETIME',
 'TH_ADV_DISABLE_SHIFT_KEY','TH_ADV_FORCE_SHIFT_KEY','TH_ADV_DISABLE_Z_KEY',
 'THPRAC_KB_OPEN','THPRAC_KB_RECORD_START','THPRAC_KB_RECORD_STOP','THPRAC_KB_OUTPUT',
 'THPRAC_INFLIVES_MAP','THPRAC_FAST_RETRY','TH_DELAY','TH_GAME_SPEED',
 'TH06_ST6_WALL_PRAC','TH06_ST6_WALL_PRAC_SNIPE_N','TH06_ST6_WALL_PRAC_SNIPE_F',
 'THPRAC_BLIND','THPRAC_BLIND_DESC','THPRAC_BLIND_SZ','THPRAC_AUTO_SHOOT',
 ...['CENTRY','YEAR','MONTH','DAY','HOUR','MINUTE','SECOND','MILLISECOND','MICROSECOND','NANOSECOND'].map(x=>'THPRAC_INGAMEINFO_06_GAMTIME_'+x),
 ...phaseGroups.flatMap(([,items])=>items),
 ...Array.from({length:6},(_,i)=>['TH_BOOK_C'+(i+1),'TH_BOOK_X'+(i+1),'TH_BOOK_Y'+(i+1)]).flat(),...reactionKeys,...speedKeys];
for (const key of sharedKeys) assert(Array.isArray(glossary[key]) && glossary[key].length === 3, key);
const keys = [...new Set([...source.matchAll(/\bS\((TH\w+)\)/g)].map(m => m[1]).concat(sharedKeys))];
const labels = keys.filter(k => Array.isArray(glossary[k]) && glossary[k].length === 3)
 .map(k => `inline constexpr const char* purple_${k}[3]{${glossary[k].map(x=>JSON.stringify(x)).join(',')}};`).join('\n');
const aliases = [...new Set([...reactionKeys,...speedKeys])].map(k=>`inline constexpr auto& practice_${k} = purple_${k};`).join('\n');
const phases=phaseGroups.map(([key,items])=>`inline constexpr const char* purpleOptions_${key}[3][${items.length}]{`+
 Array.from({length:3},(_,locale)=>'{'+items.map(k=>JSON.stringify(glossary[k][locale])).join(',')+'}').join(',')+'};').join('\n');
const glyphs = 'inline void AddPurpleGlyphs(ImFontGlyphRangesBuilder& builder, int locale) {\n'+
 keys.filter(k=>Array.isArray(glossary[k]) && glossary[k].length===3).map(k=>` builder.AddText(purple_${k}[locale]);`).join('\n')+
 '\n builder.AddText("ＲＥＰ");\n for(auto text:Purple::th06_spells_str[locale])builder.AddText(text);\n}\n';
const components = read('thprac/src/thprac/thprac_gui_components.cpp');
const start = components.indexOf('        bool GuiHotKey::OnWidgetUpdate()');
const end = components.indexOf('        bool GuiHotKey::operator()', start);
assert(start >= 0 && end > start);
let widget = components.slice(start, end).trimEnd()
 .replace('bool GuiHotKey::OnWidgetUpdate()', 'inline bool PurpleHotkeyWidget(const char* mKeyText, const char* text, bool mStatus)')
 .replace('            const char* text = mText ? mText : LocaleGetStr(mTextRef);\n', '')
 .replace('std::format("[{}: {}]", mKeyText, text)', 'std::string("[") + mKeyText + ": " + text + "]"')
 .replace('std::format("{}: {}", mKeyText, text)', 'std::string(mKeyText) + ": " + text');
assert(!/std::format|LocaleGetStr|mTextRef|GuiHotKey::/.test(widget));
const result = `// Generated from purple THPrac (MIT); TH06 SHA256 ${hash(source)}.\n#pragma once\n#include <imgui.h>\n#include <string>\n#include "PurpleResources.hpp"\nnamespace PracticeRuntime {\n${widget}\n${labels}\n${phases}\n${aliases}\n${glyphs}\n}\n`;
const file = resolve(root, 'src/PurpleLabels.hpp');
if (process.argv.includes('--write')) writeFileSync(file, result);
else { assert(process.argv.includes('--check')); assert.equal(readFileSync(file,'utf8').replaceAll('\r\n','\n'),result); }
const a = source.indexOf('    static void RenderBtHitbox(ImDrawList* p)');
const b = source.indexOf('    static void RenderLockTimer(', a);
assert(a >= 0 && b > a);
let hitbox = source.slice(a,b)
 .replace('static void RenderBtHitbox(ImDrawList* p)', 'inline void RenderPurpleHitbox(ImDrawList* p, bool enabled)')
 .replace('g_show_bullet_hitbox', 'enabled')
 .replace('DWORD is_practice = *(DWORD*)(0x69d4c3);', 'const bool is_practice = g_GameManager.isInPracticeMode;')
 .replace('DWORD gameState = *(DWORD*)(0x6C6EA4);', 'const int gameState = g_Supervisor.curState;')
 .replace('DWORD is_rep = *(DWORD*)(0x69BCBC);', 'const bool is_rep = g_GameManager.isInReplay;')
 .replace('thPracParam.mode', 'PracticeRuntime::AdvancedActive()')
 .replace('gameState == 2', 'gameState == SUPERVISOR_STATE_GAMEMANAGER')
 .replace('ImVec2 plpos1 = *(ImVec2*)(0x6CAA80);', 'ImVec2 plpos1{g_Player.hitboxTopLeft.x,g_Player.hitboxTopLeft.y};')
 .replace('ImVec2 plpos2 = *(ImVec2*)(0x6CAA8C);', 'ImVec2 plpos2{g_Player.hitboxBottomRight.x,g_Player.hitboxBottomRight.y};')
 .replace('DWORD pbt = 0x005AB5F8 + i * 0x5C4;', 'const auto& bullet = g_BulletManager.bullets[i];')
 .replaceAll('*(WORD*)(pbt + 0x5BE)', 'bullet.state')
 .replace('ImVec2 pos = *(ImVec2*)(pbt + 0x560);', 'ImVec2 pos{bullet.pos.x,bullet.pos.y};')
 .replace('ImVec2 hit = *(ImVec2*)(pbt + 0x550);', 'ImVec2 hit{bullet.sprites.grazeSize.x,bullet.sprites.grazeSize.y};')
 .replace('DWORD pls = 0x00691FF8 + 0x270 * i;', 'const auto& laser = g_BulletManager.lasers[i];')
 .replace('DWORD is_used = *(DWORD*)(pls + 0x258);', 'const bool is_used = laser.inUse;')
 .replace('ImVec2 pos = *(ImVec2*)(pls + 0x220);', 'ImVec2 pos{laser.pos.x,laser.pos.y};');
const laser = {'22C':'angle','23C':'width','230':'startOffset','234':'endOffset','26C':'state',
 '248':'hitboxStartTime','254':'hitboxEndDelay','264':'timer.current','260':'timer.subFrame',
 '244':'startTime','250':'despawnDuration'};
hitbox = hitbox.replace(/\*\((?:float|DWORD)\*\)\(pls \+ 0x([0-9A-F]+)\)/g,(_,offset)=> {
 assert(offset in laser,offset); return 'laser.'+laser[offset];
})
 .replace('DWORD penm = 0x4B79C8 + 0xED0 + i * 0xEC8;', 'const auto& enemy = g_EnemyManager.enemies[i];')
 .replace('*(char*)(0xE50 + penm) < 0', 'enemy.flags.isSlotOccupied')
 .replace('(*(BYTE*)(penm + 0xE51) & 4) != 0 && (*(BYTE*)(penm + 0xE51) & 2) != 0 && (*(BYTE*)(penm + 0xE51) & 1) != 0', 'enemy.flags.hasBeenInBounds && enemy.flags.isCollidable && enemy.flags.isInteractable')
 .replace('ImVec2 pos = *(ImVec2*)(penm + 0xC6C);', 'ImVec2 pos{enemy.position.x,enemy.position.y};')
 .replace('ImVec2 hit = *(ImVec2*)(penm + 0xC78);', 'ImVec2 hit{enemy.hitboxDimensions.x,enemy.hitboxDimensions.y};');
assert(!/DWORD|WORD|BYTE|pbt|pls|penm|thPracParam|\*\([^\n]*\*\)/.test(hitbox),'Unmapped native hitbox address');
const visual = '// Generated from purple TH06 (MIT). Native drawing order and formulas.\n#pragma once\n'+
 '#include "BulletManager.hpp"\n#include "EnemyManager.hpp"\n#include "Player.hpp"\n#include "Supervisor.hpp"\n'+
 'namespace PracticeRuntime {\n'+hitbox+'}\n';
const visualFile = resolve(root,'src/PurpleVisuals.hpp');
if (process.argv.includes('--write')) writeFileSync(visualFile, visual);
else assert.equal(readFileSync(visualFile,'utf8').replaceAll('\r\n','\n'),visual);
const resources=read('thprac/src/thprac/thprac_res.h');
const declaration=name=>{const start=resources.indexOf(name);assert(start>=0,name);const begin=resources.lastIndexOf('\n',start)+1;return resources.slice(begin,resources.indexOf(';',start)+1).trim();};
const data='// Generated purple TH06 resources (MIT).\n#pragma once\nnamespace PracticeRuntime::Purple {\n'+
 ['hitbox_file','th06_spells_str','is_spell_used'].map(declaration).join('\n')+'\n}\n';
let detail=source.slice(source.indexOf('        void ShowDetail(bool* isOpen)'),source.indexOf('        void ContentUpdate()',source.indexOf('        void ShowDetail(bool* isOpen)')))
 .replace('void ShowDetail(bool* isOpen)','void ShowDetail()')
 .replaceAll('TH06Save::singleton()','Stats()').replaceAll('TH06Save::','PurpleSave::')
 .replaceAll('Gui::LocaleGet()','Locale()')
 .replace(/S\((TH\w+)\)/g,(_,key)=>`purple_${key}[Locale()]`)
 .replaceAll('std::format("{}{}timetable", tabs_diff[diff], tabs_player[pl])','(std::string(tabs_diff[diff])+tabs_player[pl]+"timetable")')
 .replaceAll('std::format("{}{}sptable", tabs_diff[diff], tabs_player[pl])','(std::string(tabs_diff[diff])+tabs_player[pl]+"sptable")')
 .replace(/            if \(isOpen != nullptr[\s\S]*?            }\n/,'');
assert(!/std::format|TH06Save|isOpen/.test(detail));
const detailData='// Generated native purple TH06 statistics tabs/tables (MIT).\n'+detail;
const sss=read('thprac/src/thprac/thprac_games_SSS.cpp');
const blind=sss.slice(sss.indexOf('static const unsigned char blind_file[]'),sss.indexOf('ImVec2 texture_size'));
assert(blind.includes('0x89, 0x50, 0x4E, 0x47'));
let player=source.slice(source.indexOf('    static void RenderPlHitbox('),source.indexOf('    static void RenderBtHitbox('))
 .replace('static void RenderPlHitbox(ImDrawList* p)','inline void RenderPurplePlayerHitbox(ImDrawList* p,bool enabled,ImTextureID texture,ImVec2 size,float frameAdvance)')
 .replaceAll('g_adv_igi_options.th06_show_hitbox','enabled').replaceAll('g_hitbox_textureID','texture').replaceAll('g_hitbox_sz','size')
 .replace('*(BYTE*)(0x69BCCC)','g_GameManager.isTimeStopped').replace('DWORD gameState = *(DWORD*)(0x6C6EA4);','int gameState=g_Supervisor.curState;')
 .replace('BYTE pauseMenuState = *(BYTE*)(0x69D4BF);','bool pauseMenuState=g_GameManager.isInGameMenu;')
 .replace('WORD keyState = *(WORD*)(0x69D904);','u16 keyState=g_CurFrameInput;')
 .replace('gameState == 2','gameState == SUPERVISOR_STATE_GAMEMANAGER').replace('t += 1.0f','t += frameAdvance')
 .replace('*(float*)(0x6CAA68)','g_Player.positionCenter.x').replace('*(float*)(0x6CAA6C)','g_Player.positionCenter.y');
assert(!/DWORD|WORD|BYTE|0x6C|0x69|g_adv_igi/.test(player));
const interpolation=source.slice(source.indexOf('    static float MInterpolation('),source.indexOf('    static void RenderRepMarker('));
const playerData='// Generated purple player-hitbox animation and blind asset (MIT).\n#pragma once\nnamespace PracticeRuntime::Purple {\n'+blind+interpolation+player+'}\n';
for(const [name,value] of [['PurpleResources.hpp',data],['PurpleDetail.inc',detailData],['PurplePlayerVisual.hpp',playerData]]) {
 const path=resolve(root,'src',name);
 if(process.argv.includes('--write'))writeFileSync(path,value);
 else assert.equal(readFileSync(path,'utf8').replaceAll('\r\n','\n'),value);
}
console.log(`TH06 purple source verified; ${keys.length} label references; native collision drawing extracted`);
