import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const here = path.dirname(fileURLToPath(import.meta.url));
const repo = path.resolve(here, '../..');
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
  exact(['known', '__synthetic_new_site__'], ['known'], 'TH07 thcrap mutation');
} catch {
  inventoryMutationCaught = true;
}
if (!inventoryMutationCaught)
  throw new Error('TH07 thcrap site-inventory mutation self-test failed');

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

const version = read('dependencies/upstream-thcrap-tsa/base_tsa/th07.v1.00b.js');
const sourceContract = read('th07-eagler/audit/source-contracts/thcrap-source-contract.mjs');
const expectedBinhacks = [
  'antitamper_remove_check','sprintf_call_ebp-208','sprintf_call_ebp-50','sprintf_rep','sprintf_replay_1',
  'sprintf_replay_2','sprintf_replay_3','menu_desc_align','boss_title_align','spell_loop','music_cmt_prepare',
  'music_cmt_prepare_first','unpatch_result_spell','log_restore','ending_copy_rem','ending_copy_rep',
  'bosstitle_line_order#1','bosstitle_line_order#2','hud_force_redraw','stage_result_align_call','force_disable_vsync',
  'reacquire_input','ascii_patch_1','ascii_patch_2',
];
const expectedBreakpoints = [
  'ascii_params','file_load','file_loaded','file_name','file_size','music_cmt','music_cmt#line_num','music_cmt#track',
  'music_title','music_title#track','spell_name','spell_name#result','strings_lookup#cavesize_5','strings_lookup#cavesize_6',
  'textimage_init','textimage_is_active','textimage_set','th06_screenshot',
];
const binhacks = topLevelKeys(objectBody(version, 'binhacks'));
const breakpoints = topLevelKeys(objectBody(version, 'breakpoints'));
exact(binhacks, expectedBinhacks, 'TH07 v1.00b binhack Proof Ledger');
exact(breakpoints, expectedBreakpoints, 'TH07 v1.00b breakpoint Proof Ledger');
for (const site of [...binhacks, ...breakpoints]) {
  if (!sourceContract.includes(site))
    throw new Error(`TH07 upstream thcrap site has no permanent source-contract proof entry: ${site}`);
}

const methodCategory = new Map(Object.entries({
  Active: 'feature gate / strict OFF baseline',
  ApplyBossNameImage: 'textimage boss row',
  ApplyBossTitleImage: 'textimage boss row',
  AsciiString: 'ascii_vpatchf/ascii_str',
  DebugAsciiTableSelfTest: 'dev-only proof consumer',
  DebugBossImageRowContractSelfTest: 'dev-only proof consumer',
  DebugStringTableSelfTest: 'dev-only proof consumer',
  FormatStringById: 'strings_lookup + translated sprintf formatting',
  LogString: 'log_restore/string lookup',
  LookupAscii: 'ascii_vpatchf/ascii_str lookup',
  MusicComment: 'music_cmt pointer substitution',
  MusicTitle: 'music_title pointer substitution',
  SpellName: 'spell_name / spell_name#result',
  StringById: 'strings_lookup typed replacement',
}));
const consumers = new Set();
for (const file of fs.readdirSync(path.join(repo, 'src'), { recursive: true })) {
  if (!/\.(?:cpp|hpp)$/.test(file) || /Localization(?:Stub)?\.(?:cpp|hpp)$/.test(file)) continue;
  const text = fs.readFileSync(path.join(repo, 'src', file), 'utf8');
  for (const m of text.matchAll(/Localization::([A-Za-z_][A-Za-z0-9_]*)\s*\(/g)) consumers.add(m[1]);
}
exact(consumers, methodCategory.keys(), 'TH07 portable Localization consumer inventory');

// Known highest-risk parser/pointer ownership: path newline uses normal OR;
// title newline keeps the original impossible AND, producing empty slot 0;
// translated comments/titles are late-bound and never copied into descriptor.
const musicRoom = read('th07-eagler/src/MusicRoom.cpp');
function musicRoomParserOk(text) {
  const pathWrite = text.indexOf('arg->trackDescriptors[offset].path[charIdx]');
  const titleWrite = text.indexOf('arg->trackDescriptors[offset].title[charIdx]', pathWrite);
  const normalOr = text.indexOf("*curChar == '\\n' || *curChar == '\\r'", pathWrite);
  const impossibleAnd = text.indexOf("*curChar == '\\n' && *curChar == '\\r'", titleWrite);
  return pathWrite >= 0 && titleWrite >= 0 && normalOr >= pathWrite && normalOr <= titleWrite &&
    impossibleAnd >= titleWrite && !text.includes('Localization::CopyText(musicRoom->trackDescriptors');
}
if (!musicRoomParserOk(musicRoom))
  throw new Error('TH07 Music Room parser/pointer ownership contract drifted');
const mutatedMusicRoom = musicRoom.replace("*curChar == '\\n' && *curChar == '\\r'",
                                           "*curChar == '\\n' || *curChar == '\\r'");
if (musicRoomParserOk(mutatedMusicRoom))
  throw new Error('TH07 Music Room parser mutation self-test failed');

console.log(`TH07 thcrap Proof Ledger PASS: ${binhacks.length} binhacks + ${breakpoints.length} breakpoints; ${consumers.size} reverse Localization consumer classes`);
