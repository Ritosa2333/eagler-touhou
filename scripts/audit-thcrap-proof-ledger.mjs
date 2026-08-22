import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const here = path.dirname(fileURLToPath(import.meta.url));
const repo = path.resolve(here, '..');
const workspace = path.resolve(repo, '..');
const read = rel => fs.readFileSync(path.join(workspace, rel), 'utf8').replaceAll('\r\n', '\n');

function objectBody(text, key) {
  const marker = `"${key}"`;
  let start = text.indexOf(marker);
  if (start < 0) return '';
  start = text.indexOf('{', start);
  let depth = 0, quoted = false, escaped = false;
  for (let i = start; i < text.length; ++i) {
    const ch = text[i];
    if (quoted) {
      if (escaped) escaped = false;
      else if (ch === '\\') escaped = true;
      else if (ch === '"') quoted = false;
      continue;
    }
    if (ch === '"') quoted = true;
    else if (ch === '{') ++depth;
    else if (ch === '}' && --depth === 0) return text.slice(start + 1, i);
  }
  throw new Error(`unterminated ${key}`);
}

let inventoryMutationCaught = false;
try {
  exact(['known', '__synthetic_new_site__'], ['known'], 'TH06 thcrap mutation');
} catch {
  inventoryMutationCaught = true;
}
if (!inventoryMutationCaught)
  throw new Error('TH06 thcrap site-inventory mutation self-test failed');

function topLevelKeys(body) {
  const keys = [];
  let depth = 0, quoted = false, escaped = false, stringStart = -1, lastString = null;
  for (let i = 0; i < body.length; ++i) {
    const ch = body[i];
    if (quoted) {
      if (escaped) escaped = false;
      else if (ch === '\\') escaped = true;
      else if (ch === '"') { quoted = false; lastString = body.slice(stringStart + 1, i); }
      continue;
    }
    if (ch === '"') { quoted = true; stringStart = i; }
    else if (ch === '{' || ch === '[') ++depth;
    else if (ch === '}' || ch === ']') --depth;
    else if (ch === ':' && depth === 0 && lastString !== null) { keys.push(lastString); lastString = null; }
    else if (!/\s|,/.test(ch)) lastString = null;
  }
  return keys;
}

function exact(actual, expected, label) {
  const a = [...new Set(actual)].sort();
  const e = [...expected].sort();
  if (JSON.stringify(a) !== JSON.stringify(e))
    throw new Error(`${label} drifted\nactual=${a.join(',')}\nexpected=${e.join(',')}`);
}

const version = read('dependencies/upstream-thcrap-tsa/base_tsa/th06.v1.02h.js');
const sourceContract = read('th06-eagler/scripts/test-thcrap-source-contract.mjs');

const expectedBinhacks = [
  'ascii_patch_1','ascii_patch_2','bomb_pos','bosstitle_line_order#1','bosstitle_line_order#2','buffer_overflow_rem',
  'buffer_overflow_rep_eax','buffer_overflow_rep_ecx','center_align','dialog_box_leftedge','dialog_box_rightedge_1',
  'dialog_box_rightedge_2','ending_copy_rem','ending_copy_rep','file_remove_size_assignment','hud_force_redraw',
  'music_room_cmt_print_empty_lines','music_room_cmt_shift_2nd_part','reacquire_input','remove_score_cap','result_rank_format',
  'result_shottype_leftalign_1','result_shottype_leftalign_2','result_shottype_leftalign_3','result_shottype_leftalign_4',
  'result_spell_cap_pos_1','result_spell_cap_pos_2','right_align','set_png_buff_size','spell_draw_leftaligned','spell_fetch_id',
  'spell_pos','spell_pos_reset','spell_prepare','spell_width','sprintf_call_ebp-50','sprintf_rep','sprintf_replay_1',
  'sprintf_replay_2','sprintf_replay_3','sprite3d_rotated_voodookill','sprite3d_unrotated_voodookill','text_1024',
  'text_prepare_surface_width','text_scale_x','text_scale_y','text_sprite_height','text_sprite_width','unpatch_ending_halfskip',
  'unpatch_result_spell',
];
const expectedBreakpoints = [
  'ascii_params','gentext#stage_title','music_cmt','music_cmt#line_num','music_cmt#track','music_cmt#track0',
  'music_title','music_title#track','spell_name','spell_name#result','strings_lookup#cavesize_6','textimage_init',
  'textimage_is_active#bgm','textimage_is_active#boss','textimage_is_active#stlogo','textimage_is_active#sttitle',
  'textimage_set#bossbgm','textimage_set#stage','th06_file_load','th06_file_loaded','th06_file_name','th06_file_size',
  'th06_music_title_in_game','th06_music_title_in_game#boss','th06_music_title_in_game#stage_num','th06_screenshot',
  'th06_time_fix',
];
const binhacks = topLevelKeys(objectBody(version, 'binhacks'));
const breakpoints = topLevelKeys(objectBody(version, 'breakpoints'));
exact(binhacks, expectedBinhacks, 'TH06 v1.02h binhack Proof Ledger');
exact(breakpoints, expectedBreakpoints, 'TH06 v1.02h breakpoint Proof Ledger');
for (const site of [...binhacks, ...breakpoints]) {
  if (!sourceContract.includes(site))
    throw new Error(`TH06 upstream thcrap site has no permanent source-contract proof entry: ${site}`);
}

// Reverse ledger: every production Localization consumer method outside the
// Localization implementation must have an explicit upstream category. This
// catches portable-only translation behavior that Source->Portable inventory
// cannot see.
const methodCategory = new Map(Object.entries({
  Active: 'feature gate / strict OFF baseline',
  ApplyBossNameImage: 'textimage_set boss row',
  ApplyBossTitleImage: 'textimage_set boss row',
  ApplyMusicTitleImage: 'textimage_set music row',
  ApplyStageTitleImage: 'textimage_set stage row',
  AsciiString: 'ascii_vpatchf/ascii string replacement',
  AsciiStringById: 'ascii/stringloc typed lookup',
  DebugAsciiTableSelfTest: 'dev-only proof consumer',
  DebugStringTableSelfTest: 'dev-only proof consumer',
  LogString: 'stringloc/log_restore replacement',
  LookupAscii: 'ascii_vpatchf formatter lookup',
  MusicComment: 'music_cmt pointer substitution',
  MusicTitle: 'music_title pointer substitution',
  SpellName: 'spell_name / spell_name#result',
  StageLogoImageActive: 'textimage optional stlogo precedence',
  StageName: 'gentext#stage_title',
  StringById: 'strings_lookup/stringloc typed replacement',
}));

const consumers = new Set();
for (const file of fs.readdirSync(path.join(repo, 'src'), { recursive: true })) {
  if (!/\.(?:cpp|hpp)$/.test(file) || /Localization(?:Stub)?\.(?:cpp|hpp)$/.test(file)) continue;
  const text = fs.readFileSync(path.join(repo, 'src', file), 'utf8');
  for (const m of text.matchAll(/Localization::([A-Za-z_][A-Za-z0-9_]*)\s*\(/g)) consumers.add(m[1]);
}
exact(consumers, methodCategory.keys(), 'TH06 portable Localization consumer inventory');

// Fixed-storage translated writes are forbidden for the Music Room because
// BP_music_title/BP_music_cmt replace the render-call string pointer. Keep the
// original descriptor as fallback state only.
const musicRoom = read('th06-eagler/src/MusicRoom.cpp');
function musicRoomOwnershipOk(text) {
  return !text.includes('Localization::CopyText(musicRoom->trackDescriptors') &&
    text.includes('const char *displayTitle = Localization::MusicTitle(') &&
    text.includes('const char *text = Localization::MusicComment(');
}
if (!musicRoomOwnershipOk(musicRoom))
  throw new Error('TH06 Music Room pointer ownership regressed to fixed TrackDescriptor storage');
if (musicRoomOwnershipOk(musicRoom + '\nLocalization::CopyText(musicRoom->trackDescriptors[0].title, 1, "x");'))
  throw new Error('TH06 Music Room ownership checker mutation self-test failed');

console.log(`TH06 thcrap Proof Ledger PASS: ${binhacks.length} binhacks + ${breakpoints.length} breakpoints; ${consumers.size} reverse Localization consumer classes`);
