// Source extraction only. Does not start the game, decoder, browser or tests.
import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
const root = path.resolve(import.meta.dirname, '../..');
const upstream = path.join(root, 'src/practice/upstream');
const read = name => fs.readFileSync(path.join(upstream, name), 'utf8').replace(/^\uFEFF/, '').replaceAll('\r\n', '\n');
const source = read('thprac_th12_reference.cpp');
const definitions = JSON.parse(read('thprac_games_def.json'));
const revision = read('REVISION').trim();
const digest = createHash('sha256').update(source).digest('hex');
const glossary = Object.assign({}, ...Object.values(definitions).map(game => game.glossary || {}));
const entries = Object.entries(definitions.th12.sections);
const sections = entries.map(([key, value], index) => ({id:index + 1, key,
  stage:value.appearance[0], group:value.appearance[1], order:value.appearance[2], spell:!!value.spell, bgm:value.bgm,
  dialogue:/ST[1-6]_BOSS1$|ST5_MID1$|ST7_END_NS1$|ST7_MID1$/.test(key),
  phaseCount:/ST6_BOSS(?:8|10)$/.test(key) ? 2 : /ST7_END_S10$/.test(key) ? 5 : 1,
  names:Array.from({length:5}, (_, difficulty) => {
    const entry = Object.entries(value).find(([selector]) => selector.startsWith('!') && selector.includes('ENHLX'[difficulty]))?.[1];
    return entry === undefined ? ['', '', ''] : typeof entry === 'string' ? glossary[entry] || [entry, entry, entry] : entry;
  }),
}));
const start = source.indexOf('    void ECLJump('), end = source.indexOf('    __declspec(noinline) void THSectionPatch()');
if (start < 0 || end <= start) throw Error('TH12 source extraction boundary changed');
let patches = source.slice(start, end).replaceAll('__declspec(noinline) ', '').replaceAll('THPrac::TH12::', '').replaceAll('th_sections_t', 'int');
// A multi-character literal is packed big-endian by both MSVC and Clang. Its
// little-endian write spells Card; use the explicit value for portability.
patches = patches.replaceAll("'draC'", '0x64726143');
// The jump destination is executable script, so protect its original header
// even when that destination is not itself overwritten by this patch.
patches = patches.replace('ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;',
  'ecl.CheckInstruction(dest);\n        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;');
const sourceNames = [...patches.matchAll(/case (TH12_\w+):/g)].map(match => match[1]);
for (const name of sourceNames) if (!sections.some(section => section.key === name)) throw Error('Missing section definition: ' + name);
const chapterCounts = [[3,2],[3,2],[4,3],[4,4],[4,3],[3,0],[4,4]];
const j = value => JSON.stringify(value);
const files = {
  'src/game/PracticePatches.inc':`// Exact TH12 THStageWarp/THPatch source extraction (MIT), normalized SHA256 ${digest}.\n// Only injection annotations, namespaces and a multi-character literal are adapted.\n${patches}`,
  'src/game/PracticeSections.hpp':`// TH12 section enumeration follows official JSON insertion order (MIT).\n#pragma once\nnamespace th12::practice {\nenum Section {None=0,\n${sections.map(section => `  ${section.key}=${section.id},`).join('\n')}\n};\nstruct SectionInfo {int stage,group,order,bgm,phaseCount,available;bool spell,dialogue;};\ninline constexpr SectionInfo sections[]{ {0,0,0,0,1,0,false,false},\n${sections.map(section => `  {${section.stage},${section.group},${section.order},${section.bgm},${section.phaseCount},${section.names.reduce((mask, names, difficulty) => mask | (names.some(Boolean) ? 1 << difficulty : 0), 0)},${section.spell},${section.dialogue}},`).join('\n')}\n};\ninline constexpr int chapterCounts[7][2]{{3,2},{3,2},{4,3},{4,4},{4,3},{3,0},{4,4}};\n}\n`,
  'src/practice/sections.mjs':`// Official THPrac TH12 catalog (MIT), revision ${revision}.\n// Patch source supplied by the workspace: normalized SHA256 ${digest}.\nexport const sections=Object.freeze(${JSON.stringify(sections,null,2)});\nexport const chapterCounts=Object.freeze(${j(chapterCounts)});\nexport const source=Object.freeze(${j({revision,referenceSha256:digest,definitionsSha256:createHash('sha256').update(read('thprac_games_def.json')).digest('hex')})});\n`,
};
// Only instruction-site fingerprints are extracted from the private retail
// files: no ECL payload, routine names, strings or game resource is copied.
const assetsArgument = process.argv.find(value => value.startsWith('--assets='));
const assets = assetsArgument ? path.resolve(assetsArgument.slice(9)) : path.resolve(root, '../../th12-eagler/site/assets');
const warpStart = patches.indexOf('void THStageWarp('), patchStart = patches.indexOf('void THPatch(');
const warp = patches.slice(warpStart, patchStart), patch = patches.slice(patchStart);
const numericOffsets = text => [...text.matchAll(/\b0x[0-9a-f]+\b/gi)].map(match => Number(match[0]));
const intervals = Array.from({length:8}, () => []);
const warpBranches = [...warp.matchAll(/(?:if|else if) \(stage == (\d)\)/g)];
for (let i=0; i<warpBranches.length; ++i) {
  const match = warpBranches[i], number = Number(match[1]);
  intervals[number].push(...numericOffsets(warp.slice(match.index, warpBranches[i+1]?.index ?? warp.length)));
}
const cases = [...patch.matchAll(/case (TH12_ST(\d)_\w+):/g)];
for (let i=0; i<cases.length; ++i) {
  const match = cases[i], number = Number(match[2]);
  const offsets = numericOffsets(patch.slice(match.index, cases[i+1]?.index ?? patch.length));
  if (number === 7 && match[1].includes('_END_')) intervals[0].push(...offsets);
  else intervals[number].push(...offsets);
}
const nue = patch.slice(0, cases[0].index), nueSwitch = nue.indexOf('ecl.SetFile(2)');
intervals[7].push(...numericOffsets(nue.slice(0,nueSwitch)), 0xdb14, 0xdd5c);
intervals[0].push(...numericOffsets(nue.slice(nueSwitch)));
const siteRows = [], retailFiles = [];
const crc32 = bytes => {
  let value=0xffffffff;
  for(const byte of bytes){value^=byte;for(let bit=0;bit<8;++bit)value=(value>>>1)^((value&1)?0xedb88320:0);}
  return (value^0xffffffff)>>>0;
};
for (let stage=1; stage<=7; ++stage) {
  for (const ordinal of stage === 7 ? [0,2] : [0]) {
    const name = ordinal === 2 ? 'stage07boss.ecl' : `stage${String(stage).padStart(2,'0')}.ecl`;
    const bytes = fs.readFileSync(path.join(assets,name));
    const table = 36 + bytes.readUInt16LE(6), count = bytes.readUInt16LE(16);
    const starts = Array.from({length:count}, (_, i) => bytes.readUInt32LE(table+i*4)).sort((a,b) => a-b);
    const potential = intervals[ordinal === 2 ? 0 : stage];
    for (let i=0; i<starts.length; ++i) {
      const end = starts[i+1] ?? bytes.length;
      for (let offset=starts[i]+16; offset<end;) {
        const length = bytes.readUInt16LE(offset+6);
        if (length < 16 || length % 4 || offset+length > end) throw Error('Retail ECL source structure invalid: '+name);
        if (potential.some(position => position < offset+length && position+128 > offset)) {
          siteRows.push({stage,ordinal,offset,length,crc32:crc32(bytes.subarray(offset,offset+16))});
        }
        offset += length;
      }
    }
    retailFiles.push({name, bytes:bytes.length, sha256:createHash('sha256').update(bytes).digest('hex')});
  }
}
files['src/game/PracticeSites.hpp'] = `// Retail TH12 1.00b patch-site header CRC32 digests only; no original ECL bytes.\n#pragma once\n#include <cstdint>\nnamespace th12::practice {\nstruct Site {int stage,ordinal;uint32_t offset,length,headerCrc32;};\ninline constexpr Site sites[]{\n${siteRows.map(site => `  {${site.stage},${site.ordinal},0x${site.offset.toString(16)},${site.length},0x${site.crc32.toString(16)}u},`).join('\n')}\n};\n}\n`;
files['src/practice/source-provenance.json'] = JSON.stringify({schema:'th12-thprac-source/1',
  canonicalBase:'b40dfb5c55bec2ebbfa4bdcabdb877a5f9500f27', referenceSha256:digest, officialRevision:revision,
  officialSource:'https://github.com/touhouworldcup/thprac/tree/'+revision,
  definitionsSha256:createHash('sha256').update(read('thprac_games_def.json')).digest('hex'),
  sections:sections.length, chapters:chapterCounts.reduce((sum,counts)=>sum+counts[0]+counts[1],0),
  siteFingerprints:siteRows.length, retailFiles, evidence:{sourceExtraction:'generated',compilation:'not-recorded-here',
    runtime:'not-run',lifecycle:'not-run',device:'not-run',integration:'isolated-experiment'}},null,2)+'\n';
for (const [name, text] of Object.entries(files)) fs.writeFileSync(path.join(root, name), text);
console.log(JSON.stringify({extracted:Object.keys(files), sections:sections.length, chapters:chapterCounts.reduce((sum, counts) => sum + counts[0] + counts[1],0), sites:siteRows.length, referenceSha256:digest, revision}));
