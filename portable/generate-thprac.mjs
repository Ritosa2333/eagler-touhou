// Exact source extraction, matching TH08/TH10's portable generator boundary.
// Default output is an apply_patch document; no game resources are embedded.
import {readFileSync,writeFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const repository=resolve(import.meta.dirname,'..');
const upstream=resolve(process.argv[2]||'');
const read=p=>readFileSync(resolve(upstream,p),'utf8').replace(/^\uFEFF/,'').replaceAll('\r\n','\n');
const source=read('thprac/src/thprac/thprac_th11.cpp');
if(!source.includes('wave_passed')||!source.includes('TH11_SPELL_STARTSET'))throw Error('TH11 purple source required');
const versionHeader=read('thprac/src/thprac/thprac_version.h');
const version=Array.from({length:4},(_,i)=>{const m=versionHeader.match(new RegExp('#define THPRAC_VERSION_'+i+' (\\d+)'));if(!m)throw Error('Purple version boundary changed');return m[1];}).join('.');
const definitions=JSON.parse(read('thprac/src/thprac/thprac_games_def.json'));
const entries=Object.entries(definitions.th11.sections);
const digest=createHash('sha256').update(source).digest('hex');
const resources=read('thprac/src/thprac/thprac_res.h');
const payload=resources.match(/const uint8_t MBossCardECLData\[\] = \{([\s\S]*?)\};/);
if(!payload)throw Error('Purple MBoss payload boundary changed');
const payloadBytes=Array.from(payload[1].matchAll(/0x([\da-f]{2})\b/gi),m=>parseInt(m[1],16));
// The native variant allocates/copies 12040 bytes, but the declared source
// array contains 12024 bytes ending in opcode 10 (return). Never reproduce
// that native 16-byte out-of-bounds read in a portable owned resource.
if(payloadBytes.length!==12024)throw Error('Purple MBoss payload size changed');
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
const injectedStart=body.indexOf('                static char MBossCardECLData1[12040];');
const injectedEnd=body.indexOf('            break;\n            }',injectedStart);
if(injectedStart<0||injectedEnd<=injectedStart)throw Error('Purple injected wave boundary changed');
body=body.slice(0,injectedStart)+`                const auto target = ecl.AppendWave(true);
                ECLJump(ecl, 0x20f0, target);
            }
                break;
            case 3:
            {
                const auto target = ecl.AppendWave(false);
                ECLJump(ecl, 0x20f0, target);
            }
`+body.slice(injectedEnd);
if(/GetMem|GetPtr|\*\([^\n]*\*\)|__declspec/.test(body))throw Error('Unmapped TH11 platform dependency');
const glossary=Object.assign({},...Object.values(definitions).map(g=>g.glossary||{}));
// Replay-only compatibility: purple removed blue's Extra timeout phases.
// Keep this exact historical case out of the purple practice menu.
const blue=readFileSync(resolve(upstream,'../thprac_blue/thprac/src/thprac/thprac_th11.cpp'),'utf8').replace(/^\uFEFF/,'').replaceAll('\r\n','\n');
const legacyStart=blue.indexOf('        case THPrac::TH11::TH11_ST7_END_S9:');
const legacyEnd=blue.indexOf('        case THPrac::TH11::TH11_ST7_END_S10:',legacyStart);
if(legacyStart<0||legacyEnd<=legacyStart)throw Error('Blue replay compatibility boundary changed');
const legacy=blue.slice(legacyStart,legacyEnd).replaceAll('THPrac::TH11::','').replace('case TH11_ST7_END_S9:','case TH11_ST7_END_S9:');
const purpleStart=body.indexOf('        case TH11_ST7_END_S9:'),purpleEnd=body.indexOf('        case TH11_ST7_END_S10:',purpleStart);
if(purpleStart<0||purpleEnd<=purpleStart)throw Error('Purple Extra timeout boundary changed');
body=body.slice(0,purpleStart)+`        // Blue source SHA256 ${createHash('sha256').update(blue).digest('hex')}, replay only.\n        case TH11_ST7_END_S9:\n            if(thPracParam.legacy_blue) {\n${legacy.slice(legacy.indexOf('{')+1,legacy.lastIndexOf('}'))}\n            }\n`+body.slice(purpleStart,purpleEnd).replace('        case TH11_ST7_END_S9:\n','')+body.slice(purpleEnd);
const groups=Object.assign({},...Object.values(definitions).map(g=>g.groups||{}));
const sharedTools=['thprac_games.cpp','thprac_launcher_tools.cpp'].map(p=>read('thprac/src/thprac/'+p)).join('\n');
const uiKeys=[...new Set([...source.match(/\bTH[A-Z0-9_]+\b/g),...sharedTools.match(/\bTH[A-Z0-9_]+\b/g),'THPRAC_INFLIVES_MAP','TH_FACTOR_ACB','TH_FACTOR_ACB_DESC'])].filter(k=>glossary[k]);
const labelGroups={practice_phase_five:groups.TH11_SPELL_5PHASE,
 practice_phase_four:groups.TH_SPELL_PHASE2,practice_phase_startset:groups.TH11_SPELL_STARTSET,
 practice_phase_infinite:groups.TH_PHASE_INF_MODE,practice_phase_wave:groups.TH11_WAVE2_START,
 practice_tracker_shots:groups.IGI_PL_11,practice_formations:groups.TH11_MARISAB_FORMATION,practice_tracker_difficulties:groups.IGI_DIFF,practice_shot_categories:groups.TH11_TYPE_SELECT};
const phaseStart=source.indexOf('        const th_glossary_t* SpellPhase()'),phaseEnd=source.indexOf('        void PracticeMenu()',phaseStart);
if(phaseStart<0||phaseEnd<=phaseStart)throw Error('Purple TH11 phase boundary changed');
let phase=source.slice(phaseStart,phaseEnd).replace('const th_glossary_t* SpellPhase()','inline int practice_phase_count(int section)').replace('            auto section = CalcSection();\n','').replaceAll('return nullptr;','return 1;');
phase=phase.replace(/return (TH\w+);/g,(_,k)=>{if(!groups[k])throw Error('Unknown purple phase group '+k);return 'return '+groups[k].length+';';});
const sections=entries.map(([key,value],index)=>({id:index+1,key,appearance:value.appearance,spell:!!value.spell,bgm:value.bgm,
 names:Array.from({length:5},(_,difficulty)=>{const selector='ENHLX'[difficulty];const entry=Object.entries(value).find(([k])=>k.startsWith('!')&&k.includes(selector))?.[1];return entry===undefined?['','','']:typeof entry==='string'?glossary[entry]||[entry,entry,entry]:entry;})}));
// Formatting only: retain native statement order while keeping generated diffs clean.
body=body.replace(/[\t ]+$/gm,'');
const files={
 'th11_web/cpp/game/PracticeWave.hpp':`// Verbatim purple MBossCardECLData payload (MIT), SHA256 ${createHash('sha256').update(Uint8Array.from(payloadBytes)).digest('hex')}.\n#pragma once\nnamespace th11 {inline constexpr unsigned char practice_wave[]{${payloadBytes.join(',')}};}\n`,
 'th11_web/cpp/game/PracticeVersion.hpp':`// Generated from purple thprac_version.h (MIT).\n#pragma once\nnamespace th11 {inline constexpr const char* practice_source_version="${version}";}\n`,
 'th11_web/cpp/game/PracticeUiLabels.hpp':`// Generated from purple thprac_games_def.json (MIT).\n#pragma once\nnamespace th11 {\n${uiKeys.map(k=>`inline constexpr const char* practice_${k}[3]{${glossary[k].map(v=>JSON.stringify(v)).join(',')}};`).join('\n')}\n${Object.entries(labelGroups).map(([name,keys])=>`inline constexpr const char* ${name}[][3]{${keys.map(key=>'{'+glossary[key].map(v=>JSON.stringify(v)).join(',')+'}').join(',')}};`).join('\n')}\n}\n`,
 'th11_web/cpp/game/PracticeSectionCatalog.hpp':`// Generated from thprac_games_def.json (MIT).\n#pragma once\nnamespace th11 {\nstruct PracticeSectionLabel {int id,appearance,group;bool spell;const char* names[5][3];};\ninline constexpr PracticeSectionLabel practice_section_labels[]{\n${sections.map(s=>`{${s.id},${s.appearance[0]},${s.appearance[1]},${s.spell},{${s.names.map(n=>'{'+n.map(v=>JSON.stringify(v)).join(',')+'}').join(',')}}},`).join('\n')}\n};\n}\n`,
 'th11_web/cpp/game/PracticePatches.inc':`// Generated from thprac TH11 (MIT), sha256 ${digest}.\n// Only Win32 buffer ownership and bounds protection are adapted.\n${body}`,
 'th11_web/cpp/game/PracticeSections.hpp':`// Generated from purple thprac_games_def.json (MIT); enum order is authoritative.\n#pragma once\nnamespace th11 {\nenum PracticeSection { PracticeNone=0,\n${entries.map(([k],i)=>` ${k}=${i+1},`).join('\n')}\n};\nstruct PracticeSectionInfo {int appearance,group,bgm;bool spell;};\ninline constexpr PracticeSectionInfo practice_sections[]{ {0,0,0,false},\n${sections.map(s=>` {${s.appearance[0]},${s.appearance[1]},${s.bgm},${s.spell}},`).join('\n')}\n};\n${phase}\n}\n`,
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
