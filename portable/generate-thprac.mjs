// Exact source extraction, matching TH08/TH10's portable generator boundary.
// Default output is an apply_patch document; no game resources are embedded.
import {readFileSync,writeFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const repository=resolve(import.meta.dirname,'..');
const upstream=resolve(process.argv[2]||'');
const read=p=>readFileSync(resolve(upstream,p),'utf8').replaceAll('\r\n','\n');
const source=read('thprac/src/thprac/thprac_th11.cpp');
const definitions=JSON.parse(read('thprac/src/thprac/thprac_games_def.json'));
const entries=Object.entries(definitions.th11.sections);
const digest=createHash('sha256').update(source).digest('hex');
const start=source.indexOf('    void* THStage6STD()'),end=source.indexOf('    __declspec(noinline) void THSectionPatch()');
if(start<0||end<=start)throw Error('Upstream TH11 extraction boundary changed');
let body=source.slice(start,end).replaceAll('__declspec(noinline) ','').replaceAll('THPrac::TH11::','').replaceAll('th_sections_t','int');
// EnemyCommands.hpp documents the same original global as the stage-section
// owner (0x4a5730); the caller supplies it independently of GameEconomy.
body=body.replaceAll('*(uint32_t*)0x4a5730','stage_section');
// These are the only executable-address accesses in the extracted region.
// Replace injected process buffers with bounded owned resource byte vectors.
body=body.replace('void* buffer = (void*)GetMemContent(STAGE_PTR, 0x10);','void* buffer = std_data.data();')
 .replace('void* buffer = (void*)GetMemContent(STAGE_PTR, 0x178, 0x108);','void* buffer = anm_data.data();')
 .replace('std.SetFile(buffer, 999999);','std.SetFile(buffer, std_data.size());')
 .replace('anm.SetFile(buffer, 999999);','anm.SetFile(buffer, anm_data.size());')
 .replace('return nullptr;','auxiliary_valid = auxiliary_valid && std.valid;\n        return nullptr;');
// The second helper has its own local writer, named anm upstream.
const anmStart=body.indexOf('    void* THStage6ANM()'),anmEnd=body.indexOf('    void ECLJump',anmStart);
body=body.slice(0,anmStart)+body.slice(anmStart,anmEnd).replace('return nullptr;','auxiliary_valid = auxiliary_valid && anm.valid;\n        return nullptr;')+body.slice(anmEnd);
// Retain the upstream iteration/order, but abort a malformed file rather than
// allowing its instruction-length walk to run outside the owned byte vector.
body=body.replace('i < ins_count; i++','i < ins_count && ecl.valid; i++')
 .replace('p += ecl_length;','if (ecl_length < 16 || ecl_length % 4) { ecl.valid = false; break; }\n            p += ecl_length;');
if(/GetMem|GetPtr|\*\([^\n]*\*\)|__declspec/.test(body))throw Error('Unmapped TH11 platform dependency');
const glossary=Object.assign({},...Object.values(definitions).map(g=>g.glossary||{}));
const sections=entries.map(([key,value],index)=>({id:index+1,key,appearance:value.appearance,spell:!!value.spell,bgm:value.bgm,
 names:Array.from({length:5},(_,difficulty)=>{const selector='ENHLX'[difficulty];const entry=Object.entries(value).find(([k])=>k.startsWith('!')&&k.includes(selector))?.[1];return entry===undefined?['','','']:typeof entry==='string'?glossary[entry]||[entry,entry,entry]:entry;})}));
const files={
 'th11_web/cpp/game/PracticeUiLabels.hpp':`// Generated from thprac_games_def.json (MIT).\n#pragma once\nnamespace th11 {\n${Object.entries({practice_phase_five:definitions.th11.groups.TH11_SPELL_5PHASE,practice_phase_timeout:definitions.th11.groups.TH11_SPELL_PHASE_EXTRA_TIMEOUT,practice_phase_four:Object.assign({},...Object.values(definitions).map(g=>g.groups||{})).TH_SPELL_PHASE2,practice_tracker_shots:['TH_TRACKER_REIMU_YUKARI','TH_TRACKER_REIMU_SUIKA','TH_TRACKER_REIMU_AYA','TH_TRACKER_MARISA_ALICE','TH_TRACKER_MARISA_PATCHOULI','TH_TRACKER_MARISA_NITORI']}).map(([name,keys])=>`inline constexpr const char* ${name}[][3]{${keys.map(key=>'{'+glossary[key].map(v=>JSON.stringify(v)).join(',')+'}').join(',')}};`).join('\n')}\n}\n`,
 'th11_web/cpp/game/PracticeSectionCatalog.hpp':`// Generated from thprac_games_def.json (MIT).\n#pragma once\nnamespace th11 {\nstruct PracticeSectionLabel {int id,appearance,group;bool spell;const char* names[5][3];};\ninline constexpr PracticeSectionLabel practice_section_labels[]{\n${sections.map(s=>`{${s.id},${s.appearance[0]},${s.appearance[1]},${s.spell},{${s.names.map(n=>'{'+n.map(v=>JSON.stringify(v)).join(',')+'}').join(',')}}},`).join('\n')}\n};\n}\n`,
 'th11_web/cpp/game/PracticePatches.inc':`// Generated from thprac TH11 (MIT), sha256 ${digest}.\n// Only Win32 buffer ownership and bounds protection are adapted.\n${body}`,
 'th11_web/cpp/game/PracticeSections.hpp':`// Generated from thprac_games_def.json (MIT); enum order is authoritative.\n#pragma once\nnamespace th11 {\nenum PracticeSection { PracticeNone=0,\n${entries.map(([k],i)=>` ${k}=${i+1},`).join('\n')}\n};\nstruct PracticeSectionInfo {int appearance,group,bgm;bool spell;};\ninline constexpr PracticeSectionInfo practice_sections[]{ {0,0,0,false},\n${sections.map(s=>` {${s.appearance[0]},${s.appearance[1]},${s.bgm},${s.spell}},`).join('\n')}\n};\n}\n`,
 'th11_web/sdl-runtime/practice-sections.mjs':`// Generated from thprac (MIT), source sha256 ${digest}.\nexport const sections=${JSON.stringify(sections,null,2)};\n`,
 'th11_web/cpp/game/THPRAC-LICENSE.txt':read('LICENCE'),
};
if(process.argv.includes('--write')){
 for(const [path,content] of Object.entries(files))writeFileSync(resolve(repository,path),content);
 console.log(JSON.stringify({written:Object.keys(files),source:digest,sections:entries.length}));
}else if(process.argv.includes('--check')){
 for(const [path,content] of Object.entries(files))if(!existsSync(resolve(repository,path))||readFileSync(resolve(repository,path),'utf8').replaceAll('\r\n','\n').trimEnd()!==content.trimEnd())throw Error('Stale TH11 thprac extraction: '+path);
 console.log(JSON.stringify({passed:true,source:digest,sections:entries.length}));
}else console.log('*** Begin Patch\n'+Object.entries(files).map(([path,content])=>'*** Add File: '+resolve(repository,path).replaceAll('\\','/')+'\n'+content.trimEnd().split('\n').map(line=>'+'+line).join('\n')).join('\n')+'\n*** End Patch');
