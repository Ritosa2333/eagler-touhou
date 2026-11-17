import {readFileSync, writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';

const [root,out]=process.argv.slice(2);
if(!out)throw Error('Usage: generate-th14-purple-ui.mjs THPRAC_PURPLE_ROOT SDL_OUTPUT_DIR');
const src=name=>readFileSync(resolve(root,'thprac/src/thprac',name),'utf8').replace(/^\uFEFF/,'').replaceAll('\r\n','\n');
const definition=src('thprac_games_def.json'), data=JSON.parse(definition);
const glossary=Object.assign({},...Object.values(data).map(x=>x.glossary));
const groups=Object.assign({},...Object.values(data).map(x=>x.groups));
const names=new Set((
 'TH_MENU TH_MODE TH_STAGE TH_WARP TH_PHASE TH_DLG TH_DROP_ITEMS TH_CYCLE TH14_FAKE_TYPE TH_CHAPTER TH_SCORE TH_LIFE TH_LIFE_FRAGMENT TH_BOMB TH_BOMB_FRAGMENT TH_POWER TH_BT_PHASE TH_VALUE TH_GRAZE TH_SPELL_PRAC TH_STAGE_PORTION_N TH_STAGE_PORTION_1 TH_STAGE_PORTION_2 '+
 'TH_MUTEKI TH_INFLIVES2 TH_INFBOMBS TH_INFPOWER TH_TIMELOCK TH_AUTOBOMB TH_EL_BGM THPRAC_INGAMEINFO TH_ENEMY_MUTEKI '+
 'THPRAC_INGAMEINFO_MISS_COUNT THPRAC_INGAMEINFO_BOMB_COUNT THPRAC_INGAMEINFO_TH14_ITEMS_CNT THPRAC_INGAMEINFO_TH14_BONUS_NORMAL THPRAC_INGAMEINFO_TH14_BONUS_20 THPRAC_INGAMEINFO_TH14_BONUS_TOTAL THPRAC_INGAMEINFO_TH14_EXTEND_COUNT THPRAC_INGAMEINFO_TH14_BONUS_NEXT THPRAC_INGAMEINFO_TH14_BONUS_BOMB THPRAC_INGAMEINFO_TH14_BONUS_LIFE '+
 'TH_ADV_OPT TH_GAME_SPEED TH_GAMEPLAY TH_FPS_ERR TH_FPS_UNSUPPORTED TH_FPS_ADJ TH_FPS_FREE_ADJ TH_ADV_OPT_APPLY TH_FPS_LOWERING TH_ABOUT_THPRAC TH_ABOUT_VERSION TH_ABOUT_AUTHOR TH_ABOUT_WEBSITE TH_ABOUT_THANKS TH_ABOUT_SHOW_LICENCE TH_ABOUT_HIDE_LICENCE '+
 'TH_ADV_DISABLE_X_KEY TH_ADV_DISABLE_X_KEY_DESC TH_ADV_DISABLE_SHIFT_KEY TH_ADV_DISABLE_SHIFT_KEY_DESC TH_ADV_DISABLE_Z_KEY TH_ADV_DISABLE_Z_KEY_DESC TH_ADV_DISABLE_C_KEY_SAMETIME TH_ADV_FORCE_SHIFT_KEY THPRAC_FAST_RETRY THPRAC_FAST_RETRY_DESC2 THPRAC_AUTO_SHOOT THPRAC_KB_OPEN THPRAC_KB_RECORD_START THPRAC_KB_RECORD_STOP THPRAC_KB_OUTPUT THPRAC_INFLIVES_MAP THPRAC_TH14_FORCE_FIX_BUG THPRAC_TH14_FORCE_FIX_BUG_DESC THPRAC_INGAMEINFO_TH14_SHOW_BONUS THPRAC_INGAMEINFO_TH14_SHOW_ITEMS THPRAC_INGAMEINFO_TH14_SHOW_DROP_BAR THPRAC_INGAMEINFO_ADV_DESC1 THPRAC_INGAMEINFO_ADV_DESC2 TH_BOSS_FORCE_MOVE_DOWN TH_BOSS_FORCE_MOVE_DOWN_DESC TH_BOSS_FORCE_MOVE_DOWN_RANGE THPRAC_SHOW_BULLET_HITBOX TH_DISABLE_MASTER TH_DISABLE_MASTER_DESC TH_ENABLE_LOCK_TIMER TH_FACTOR_ACB TH_FACTOR_ACB_DESC '+
 'TH14_MARISA_LASER TH14_MODE TH14_MODE_NONE_DESC TH14_MODE_NORMAL_DESC TH14_MODE_PLAYBACK_DESC TH14_MODE_RECOVER_DESC TH14_LOCKED TH_REPFIX_SELECTED TH_REPFIX_SELECTED_NONE TH_REPFIX_SAVE TH14_LOAD TH14_CLEAR_RECORDS TH14_NO_RECORDS TH14_REPLAY_NO_RECORDS TH14_RECORD TH14_RECORD_E TH14_DELETE TH14_CORRECTION_VALUE THPRAC_BACK'
).trim().split(/\s+/));
for(const key of Object.keys(glossary))if(key.startsWith('THPRAC_TOOLS_REACTION_'))names.add(key);
const lists='TH_STAGE_SELECT TH_MODE_SELECT TH_WARP_SELECT TH_SPELL_PHASE1 TH14_SPELL_4PHASE TH14_SPELL_5PHASE TH14_SPELL_PHASE_TIMEOUT TH_TIMEOUT_SETTING TH14_FAKE_TYPE_LIST TH14_CYCLE_LIST TH14_MODE_COMBO TH14_CORRECTION IGI_DIFF IGI_PL_14'.split(' ');
for(const list of lists)for(const name of groups[list])names.add(name);
const sha=str=>createHash('sha256').update(str).digest('hex');
const version=Array.from(src('thprac_version.h').matchAll(/#define THPRAC_VERSION_[0-3] (\d+)/g),m=>m[1]).join('.');
const str=JSON.stringify;
const lines=[`// Generated from thprac_purple (MIT) v${version}; glossary LF SHA-256 ${sha(definition)}.`];
for(const name of names){
 const row=glossary[name];
 if(!row||row.length!==3)throw Error('Missing purple label: '+name);
 lines.push(`constexpr Labels practice_${name}={${row.map(str).join(',')}};`);
}
for(const list of lists)lines.push(`constexpr const Labels* purple_${list}[]={${groups[list].map(k=>'&practice_'+k).join(',')}};`);
lines.push('constexpr const Labels* purple_glyphs[]={'+Array.from(names,k=>'&practice_'+k).join(',')+'};');
lines.push('constexpr const char* purple_version='+str(version)+';');
const license=readFileSync(resolve(root,'LICENCE'),'utf8').replace(/^\uFEFF/,'').replaceAll('\r\n','\n');
lines.push('constexpr const char* purple_license='+str(license)+';');
writeFileSync(resolve(out,'PracticeAdvancedLabelsTh14.inc'),lines.join('\n')+'\n');
