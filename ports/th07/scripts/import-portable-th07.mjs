// Source-only, reproducible import of the user-supplied reallyportable reference.
// Never imports its .git, executable binaries, game data or build outputs.
import {readFileSync, writeFileSync, mkdirSync} from 'node:fs';
import {resolve, dirname} from 'node:path';
import {createHash} from 'node:crypto';
import assert from 'node:assert/strict';
const root=resolve(import.meta.dirname,'..');
const [legacyArg,purpleArg]=process.argv.slice(2).filter(x=>!x.startsWith('--'));
assert(legacyArg&&purpleArg,'Usage: import-portable-th07.mjs <reallyportable> <purple> --write|--check');
const legacy=resolve(legacyArg),purple=resolve(purpleArg);
const read=(base,file)=>readFileSync(resolve(base,file),'utf8').replace(/^\uFEFF/,'').replaceAll('\r\n','\n');
const sha=s=>createHash('sha256').update(s).digest('hex');
const oldSource=read(legacy,'thprac/src/thprac/thprac_th07.cpp');
const source=read(purple,'thprac/src/thprac/thprac_th07.cpp');
assert.equal(sha(oldSource),'05d78d6fe2f29b327ec25c3d9fc5841bb3f802a2cc9536f5e0b346dbde37b22c');
assert.equal(sha(source),'b5be6dc698f574992ab49696c5c0d8ad756af93bf0760be1504ee9e958bee1b0');
function region(s,a,b){const i=s.indexOf(a),j=s.indexOf(b,i);assert(i>=0&&j>i,a);return s.slice(i,j);}
const chapterStart='    __declspec(noinline) void THStageWarp';
const patchStart='    __declspec(noinline) void THPatch';
const patchEnd='    __declspec(noinline) void THSectionPatch';
assert.equal(region(oldSource,chapterStart,patchEnd).replace(/\s/g,''),region(source,chapterStart,patchEnd).replace(/\s/g,''),
 'Purple/legacy ECL semantics changed: audit Replay version dispatch before importing');
const oldDefs=JSON.parse(read(legacy,'thprac/src/thprac/thprac_games_def.json'));
const defs=JSON.parse(read(purple,'thprac/src/thprac/thprac_games_def.json'));
assert.deepEqual(defs.th07.sections,oldDefs.th07.sections,'Section identity/labels changed');
const entries=Object.entries(defs.th07.sections);
assert.equal(entries.length,103);
let patch=region(source,patchStart,patchEnd);
for(const [key,id] of entries.map(([key],i)=>[key,i+1]).sort((a,b)=>b[0].length-a[0].length))
 patch=patch.replaceAll('THPrac::TH07::'+key,String(id));
patch=patch.replace('__declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)',
 'static void ApplySection(ECLHelper& ecl, PatchContext& context, const Session& session, std::int32_t section)')
 .replaceAll('thPracParam.dlg','session.dialogue').replaceAll('thPracParam.','session.')
 .replaceAll('GAME_MANAGER->character * 2 + GAME_MANAGER->shotType','session.shotType')
 .replaceAll('ECLNameFix();','context.nameFix = true;').replaceAll('ECLST3BG();','context.stage3BackgroundFix = true;')
 .replaceAll('th07_rb.Enable();','context.ranBoundaryFix = true;')
 .replace(/thFakeShot\s*=\s*([^;]+);/g,'context.fakeShot = $1;').replaceAll('ECLTimeWarp(','SetWarp(context, ');
assert(!/THPrac::|thPracParam|GAME_MANAGER|ECLNameFix|ECLST3BG|thFakeShot|th07_rb|GetMem|__declspec/.test(patch));
// The reference adapter's writes were bounded but not transactional. Run its
// unchanged patch statements against a private copy; publish bytes AND context
// together only after every write passes. Whole-file CRC remains mandatory.
let adapter=read(legacy,'portable/adapters/th07/adapter.cpp');
assert(adapter.includes('    ECLHelper ecl(data, size);'));
assert(adapter.includes('    return ecl.Valid();'));
adapter=adapter.replace('#include <string>','#include <string>\n#include <vector>\n#include <cstring>');
adapter=adapter.replace('    ECLHelper ecl(data, size);',
 '    if (data == nullptr) return false;\n    std::vector<std::uint8_t> candidate(data, data + size);\n    PatchContext candidateContext;\n    ECLHelper ecl(candidate.data(), candidate.size());');
adapter=adapter.replace('ApplySection(ecl, g_Context, g_Session, g_Session.section);',
 'ApplySection(ecl, candidateContext, g_Session, g_Session.section);');
adapter=adapter.replace('    return ecl.Valid();',
 '    if (!ecl.Valid()) return false;\n    std::memcpy(data, candidate.data(), size);\n    g_Context = candidateContext;\n    return true;');
// Crc32 must never dereference a null nonempty buffer.
adapter=adapter.replace('    if (g_Session.stage < 0 ||', '    if (data == nullptr || g_Session.stage < 0 ||');
const imported=[
 'include/thprac/portable/session.hpp','include/thprac/portable/ecl_interpreter.hpp',
 'include/thprac/portable/ecl_patch.hpp','include/thprac/portable/th07/adapter.hpp',
 'include/thprac/portable/th07/patches.hpp','src/session.cpp','src/ecl_patch.cpp','src/th07/patches.cpp',
 'generated/section_catalog.hpp','generated/timeline_warps.hpp',
 'tests/ecl_patch_test.cpp','tests/session_test.cpp','tests/th07_patch_test.cpp'];
const files=Object.fromEntries(imported.map(file=>['portable/'+file,read(legacy,'portable/'+file)]));
files['portable/adapters/th07/adapter.cpp']='// Source-derived adapter; see portable/IMPORT.json for provenance.\n'+adapter;
files['portable/generated/th07_full_patch.inc']='// Generated from purple THPrac (MIT), source sha256 '+sha(source)+'.\n'+patch;
files['portable/THPRAC-LICENSE.txt']=read(legacy,'LICENCE');
files['portable/IMPORT.json']=JSON.stringify({schema:'th07/portable-source-import/1',
 archiveSha256:'ef423e0560bcb79ad26b592cd04b4458ce600f3748ccf8a936646866bfa60820',
 legacySourceSha256:sha(oldSource),purpleSourceSha256:sha(source),sections:entries.length,
 legacyPatchEquivalent:true,transactionalAdapter:true,
 files:imported.map(file=>({source:'portable/'+file,sha256:sha(read(legacy,'portable/'+file))}))},null,2)+'\n';
for(const [file,content] of Object.entries(files)){
 if(process.argv.includes('--write')){mkdirSync(dirname(resolve(root,file)),{recursive:true});writeFileSync(resolve(root,file),content);}
 else {assert(process.argv.includes('--check'));assert.equal(read(root,file),content,file);}
}
console.log(JSON.stringify({sourceOnly:true,sections:entries.length,legacyPatchEquivalent:true,files:Object.keys(files).length}));
