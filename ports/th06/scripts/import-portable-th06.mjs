// Reproducible source import, never game resources, binaries or .git.
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {createHash} from 'node:crypto';
import assert from 'node:assert/strict';
const root=resolve(import.meta.dirname,'..');
const [legacy,purple]=process.argv.slice(2).filter(x=>!x.startsWith('--')).map(x=>resolve(x));
assert(legacy&&purple,'Pass reallyportable and purple checkouts, --write or --check');
const read=(base,file)=>readFileSync(resolve(base,file),'utf8').replace(/^\uFEFF/,'').replaceAll('\r\n','\n');
const sha=s=>createHash('sha256').update(s).digest('hex');
const source=read(purple,'thprac/src/thprac/thprac_th06.cpp');
assert.equal(sha(source),'114a55f6c321a83acf2dea28c29893dd2821a25b1088bdb8683bbc5918d9bb81');
function region(s,a,b){const i=s.indexOf(a),j=s.indexOf(b,i);assert(i>=0&&j>i,a);return s.slice(i,j);}
const catalog=JSON.parse(read(legacy,'portable/generated/sections.json')).games.th06;
const definitions=JSON.parse(read(purple,'thprac/src/thprac/thprac_games_def.json'));
assert.deepEqual(catalog.map(s=>s.key),Object.keys(definitions.th06.sections),'Stable section IDs required');
let patch=region(source,'    __declspec(noinline) void THPatch','    __declspec(noinline) void THStageWarp');
for(const {key,id} of [...catalog].sort((a,b)=>b.key.length-a.key.length))
    patch=patch.replaceAll('THPrac::TH06::'+key,String(id));
patch=patch.replace('__declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)',
 'static void ApplySection(ECLHelper& ecl, PatchContext& context, const Session& session, std::int32_t section)')
 .replaceAll('thPracParam.dlg','session.dialogue').replaceAll('thPracParam.','session.')
 .replaceAll('GAME_MANAGER->character * 2 + GAME_MANAGER->shotType','session.shotType')
 .replace('(int)(*((int8_t*)0x69d4bd) * 2) + *((int8_t*)0x69d4be)','session.shotType')
 .replaceAll('ECLNameFix();','context.nameFix = true;')
 .replace('std::default_random_engine rand_engine(*(WORD*)(0x69D8F8));','')
 .replace('std::uniform_int_distribution<int32_t> rand_value(0, 640);','')
 .replace('rand_value(rand_engine)','PracticeRuntime::PurpleDollsRandomInsert(context.seed)')
 .replace('DWORD* pBulletInsertCount = (DWORD*)(0x005A5FF8 + 0xF5C00);','auto* pBulletInsertCount = &context.bulletInsertIndex;')
 .replaceAll('ECLWarp(','SetWarp(context, 1, ');
assert(!/THPrac::|thPracParam|GAME_MANAGER|ECLNameFix|__declspec|DWORD|WORD|\*\([^\n]*\*\)/.test(patch),
 'Unmapped native patch: '+patch.match(/.*(?:THPrac::|thPracParam|GAME_MANAGER|ECLNameFix|__declspec|DWORD|WORD|\*\([^\n]*\*\)).*/g));
const jump=region(source,'    void ECLJump(', '    void ECLWarp(');
const helpers=jump+region(source,'    void ECLSetHealth(', '    void ECLNameFix(');
const oldSource=read(legacy,'thprac/src/thprac/thprac_th06.cpp');
assert.equal(region(oldSource,'    __declspec(noinline) void THStageWarp','    __declspec(noinline) void THSectionPatch').replace(/\s/g,''),
 region(source,'    __declspec(noinline) void THStageWarp','    __declspec(noinline) void THSectionPatch').replace(/\s/g,''),'Stage warp semantics changed');
const imported=['include/thprac/portable/session.hpp','include/thprac/portable/ecl_interpreter.hpp',
 'include/thprac/portable/ecl_patch.hpp','include/thprac/portable/th06/adapter.hpp',
 'include/thprac/portable/th06/patches.hpp','src/session.cpp','src/ecl_patch.cpp','src/th06/patches.cpp',
 'generated/section_catalog.hpp','generated/timeline_warps.hpp',
 'tests/ecl_patch_test.cpp','tests/session_test.cpp'];
const files=Object.fromEntries(imported.map(file=>['portable/'+file,read(legacy,'portable/'+file)]));
files['portable/include/thprac/portable/ecl_interpreter.hpp']=files['portable/include/thprac/portable/ecl_interpreter.hpp']
 .replace('    std::int32_t warpCount = 0;', '    std::uint16_t seed = 0;\n    std::int32_t bulletInsertIndex = -1;\n    std::int32_t warpCount = 0;');
files['portable/include/thprac/portable/session.hpp']=files['portable/include/thprac/portable/session.hpp']
 .replace('#include <string>','#include <string>\n#include "PurpleParams.hpp"')
 .replace('struct Session {','struct Session : PracticeRuntime::PurpleParams {');
files['portable/src/session.cpp']=files['portable/src/session.cpp']
 .replace('    s.mode = Clamp', '    s.NormalizePurple();\n    s.mode = Clamp')
 .replace('<< ",\\\"shotType\\\":" << s.shotType << "}}\\n";',
          '<< ",\\\"shotType\\\":" << s.shotType << s.PurpleJsonFields() << "}}\\n";')
 .replace('    Normalize(s);\n    output = s;',
 '    s.purpleSections=json.find("\\\"bF\\\"")!=std::string::npos;\n    s.ReadPurple([&](const char* k, int v){return Number(json,k,v);},\n        [&](const char* k, bool v){return Boolean(json,k,v);});\n    Normalize(s);\n    output = s;');
assert(files['portable/src/session.cpp'].includes('s.PurpleJsonFields()'));
let adapter=read(legacy,'portable/adapters/th06/adapter.cpp')
 .replace('#include <string>','#include <string>\n#include <vector>\n#include <cstring>\n#include "PurpleRandom.hpp"\n#include "Rng.hpp"\n#include "BulletManager.hpp"')
 .replace(region(read(legacy,'portable/adapters/th06/adapter.cpp'),'void ECLSetHealth(', '#include "th06_full_patch.inc"'),'')
 .replace('#include "th06_full_patch.inc"','#include "th06_ecl_helpers.inc"\n#include "th06_full_patch.inc"\n#include "th06_legacy_patch.inc"')
 .replace('    if (g_Session.stage < 0 ||','    if (data == nullptr || g_Session.stage < 0 ||')
 .replace('    ECLHelper ecl(data, size);','    std::vector<std::uint8_t> candidate(data,data+size);\n    PatchContext candidateContext;\n    candidateContext.seed=g_Rng.seed;\n    ECLHelper ecl(candidate.data(),candidate.size());')
 .replace('ApplySection(ecl, g_Context, g_Session, g_Session.section);',
 'if(g_Session.purpleSections) ApplySection(ecl, candidateContext, g_Session, g_Session.section);\n    else ApplyLegacySection(ecl,candidateContext,g_Session,g_Session.section);')
 .replace('    return valid;','    if (!valid) return false;\n    std::memcpy(data,candidate.data(),size);\n    g_Context=candidateContext;\n    return true;')
 .replace('    if (g_Context.warpTime > 0) {',
 '    if (g_Context.bulletInsertIndex >= 0)\n        g_BulletManager.nextBulletIndex=g_Context.bulletInsertIndex % 640;\n    if (g_Context.warpTime > 0) {');
files['portable/adapters/th06/adapter.cpp']=adapter;
files['portable/generated/th06_full_patch.inc']='// Generated from purple TH06 (MIT), sha256 '+sha(source)+'.\n'+patch;
files['portable/generated/th06_ecl_helpers.inc']='// Generated native purple ECL write helpers (MIT).\n'+helpers;
files['portable/generated/th06_legacy_patch.inc']=read(legacy,'portable/generated/th06_full_patch.inc').replace('static void ApplySection(', 'static void ApplyLegacySection(');
files['portable/THPRAC-LICENSE.txt']=read(legacy,'LICENCE');
files['src/PurpleLicense.hpp']='#pragma once\nnamespace PracticeRuntime { inline constexpr const char* purpleLicense = '+JSON.stringify(read(purple,'LICENCE'))+'; }\n';
files['portable/IMPORT.json']=JSON.stringify({schema:'th06/purple-portable-import/1',sourceSha256:sha(source),
 sections:catalog.length,transactionalAdapter:true,files:imported.map(file=>({source:'portable/'+file,sha256:sha(read(legacy,'portable/'+file))}))},null,2)+'\n';
for(const [file,content] of Object.entries(files)){
 if(process.argv.includes('--write')) {mkdirSync(dirname(resolve(root,file)),{recursive:true});writeFileSync(resolve(root,file),content);}
 else {assert(process.argv.includes('--check'));assert.equal(read(root,file),content,file);}
}
console.log('Imported native purple TH06 section patch, '+catalog.length+' stable sections');
