import {createHash} from 'node:crypto';
import {readFileSync, writeFileSync} from 'node:fs';
import {resolve} from 'node:path';

const root = resolve(import.meta.dirname, '..');
const upstream = process.argv[2];
const check = process.argv.includes('--check');
if (!upstream) throw Error('Usage: node portable/generate-thprac.mjs <RUEEE/thprac> [--check]');

const read = name => readFileSync(resolve(upstream, name), 'utf8').replace(/^\uFEFF/, '').replaceAll('\r\n', '\n');
const definitions = JSON.parse(read('thprac/src/thprac/thprac_games_def.json'));
const license = read('LICENCE');
if (!license.startsWith('MIT License')) throw Error('RUEEE/thprac license is not the expected MIT notice');
const source = read('thprac/src/thprac/thprac_th13.cpp');
const commit = process.env.THPRAC_SOURCE_SHA || '';
const digest = createHash('sha256').update(read('thprac/src/thprac/thprac_games_def.json')).digest('hex');
const game = definitions.th13;
if (!game?.sections || !definitions.common?.glossary) throw Error('th13 practice definitions missing');

const glossary = {...definitions.common.glossary, ...game.glossary};
function label(value) {
  if (Array.isArray(value)) return value;
  if (typeof value === 'string') {
    const found = glossary[value];
    if (!Array.isArray(found)) throw Error('unresolved practice label ' + value);
    return found;
  }
  throw Error('invalid practice label');
}

const sections = Object.entries(game.sections).map(([key, entry], index) => {
  const appearance = entry.appearance;
  if (!Array.isArray(appearance) || appearance.length !== 3 || appearance.some(n => !Number.isInteger(n)))
    throw Error(key + ' appearance is not a stage/group/index triple');
  const marker = ['!ENHL', '!EN', '!HL', '!ENH', '!X', '!N', '!L'].find(key => entry[key] != null);
  if (!marker) throw Error(key + ' has no label');
  const names = label(entry[marker]);
  if (names.length < 3) throw Error(key + ' needs zh/en/ja labels');
  return {
    id: index + 1,
    key,
    stage: appearance[0],
    group: appearance[1],
    index: appearance[2],
    spell: entry.spell ? 1 : 0,
    zh: names[0],
    en: names[1],
    ja: names[2],
  };
});
if (sections.length !== 72) throw Error('expected 72 TH13 sections, got ' + sections.length);
const boss = sections.find(section => section.key === 'TH13_ST2_BOSS1');
if (!boss || boss.stage !== 2 || boss.group !== 2 || boss.index !== 1) throw Error('TH13_ST2_BOSS1 mapping drifted');

const schema = {
  source: 'https://github.com/RUEEE/thprac',
  sourceSha: commit,
  definitionsSha256: digest,
  defaults: {
    mode: 1, stage: 0, section: 0, phase: 0, dlg: false,
    score: 0, life: 8, extend: 0, life_fragment: 0, bomb: 8, bomb_fragment: 0,
    power: 400, value: 10000, graze: 0, trance_meter: 200,
    spirit_start_left: false, useOD: false,
  },
  ranges: {
    mode: [0, 1], stage: [0, 6], section: [0, 19999], phase: [0, 8],
    score: [0, 9999999990], life: [0, 8], extend: [0, 6], life_fragment: [0, 8],
    bomb: [0, 8], bomb_fragment: [0, 8], power: [0, 400], value: [0, 9999990],
    graze: [0, 99999999], trance_meter: [0, 600],
  },
  functional: ['coarse-stage-warp', 'initial-resources'],
  deferred: ['exact-section-warp', 'multi-phase-spell-start', 'section-dialogue', 'overdrive', 'spirit-side'],
  sections,
};

const header = `// Generated from RUEEE/thprac th13 sections (MIT, Ack 2022).
// Source SHA ${commit || 'unpinned'} definitions ${digest}.
// Do not edit. Regenerate with portable/generate-thprac.mjs.
#pragma once
namespace th13 {
struct PracticeSectionInfo {int stage, group, index, spell;};
inline constexpr PracticeSectionInfo practice_sections[]{{0,0,0,0},
${sections.map(section => ` {${section.stage},${section.group},${section.index},${section.spell}}, // ${section.key}`).join('\n')}
};
inline constexpr int practice_section_count = ${sections.length};
}
`;

const catalog = `// Generated from RUEEE/thprac (MIT). definitions ${digest}
export const practiceSections = ${JSON.stringify(sections, null, 2)};
export function practiceSection(key){return practiceSections.find(section => section.key === key) ?? null;}
`;

const outputs = {
  [resolve(root, 'web/cpp/game/PracticeSections.gen.hpp')]: header,
  [resolve(root, 'web/sdl-runtime/practice-catalog.mjs')]: catalog,
  [resolve(root, '../th13-mp/web/sdl-runtime/practice-catalog.mjs')]: catalog,
  [resolve(root, '../../launcher/integrations/th13-thprac.mjs')]:
    `// Generated from RUEEE/thprac (MIT). definitions ${digest}\nexport const TH13_THPRAC = ${JSON.stringify({
      defaults: schema.defaults, ranges: schema.ranges, functional: schema.functional, deferred: schema.deferred,
    }, null, 2)};\n`,
  [resolve(root, 'web/cpp/game/THPRAC-LICENSE.txt')]: license,
};

if (!source.includes('struct THPracParam')) throw Error('thprac_th13.cpp has no THPracParam');
for (const [path, text] of Object.entries(outputs)) {
  if (check) {
    const current = readFileSync(path, 'utf8').replaceAll('\r\n', '\n');
    if (current !== text) throw Error('stale generated practice file: ' + path);
  } else writeFileSync(path, text);
}
console.log(JSON.stringify({sections: sections.length, definitionsSha256: digest, check}));
