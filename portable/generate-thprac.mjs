// Emit an apply_patch document. No generated output is written implicitly.
import {existsSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const repository=resolve(import.meta.dirname,'..');
const sourceArgument=process.argv.slice(2).find(value=>!value.startsWith('--'));
if(!sourceArgument)throw Error('Usage: node portable/generate-thprac.mjs <path-to-thprac> [--check|--write]');
const upstream=resolve(sourceArgument);
const legacy=process.argv.includes('--legacy');
if(!existsSync(resolve(upstream,'thprac/src/thprac/thprac_th08.cpp')))
 throw Error('Missing upstream thprac checkout at '+upstream+'; pass its path explicitly: node portable/generate-thprac.mjs <path-to-thprac> --check');
const read=p=>readFileSync(resolve(upstream,p),'utf8').replace(/^\uFEFF/,'').replaceAll('\r\n','\n');
const source=read('thprac/src/thprac/thprac_th08.cpp');
const definitions=JSON.parse(read('thprac/src/thprac/thprac_games_def.json'));
const entries=Object.entries(definitions.th08.sections);
const digest=createHash('sha256').update(source).digest('hex');
let body=source.slice(source.indexOf('    __declspec(noinline) void THStageWarp'),source.indexOf('    __declspec(noinline) void THSectionPatch'));
body=body.replaceAll('__declspec(noinline) ','').replaceAll('THPrac::TH08::','').replaceAll('th_sections_t','int')
 .replace('int32_t diff = *((int32_t*)0x160f538);','int32_t diff = scene.globals.difficulty;')
 .replaceAll('*((int32_t*)0x4ea290)', 'scene.globals.stage_interrupt')
 .replaceAll('*(uint32_t*)STAGE_PENDING_INTERRUPT','scene.globals.stage_interrupt');
// Purple's Mystia first-spell point setup injects ECL, not machine code.
// Preserve its exact payload and relative jumps in a bounded owned extension.
body=body.replace('static DWORD code_cave[]','static const uint32_t code_cave[]');
const caveStart=body.indexOf('                ecl.SetPos(0x3E14);');
const caveEnd=body.indexOf('\n            }',caveStart);
if(!legacy){
 if(caveStart<0||caveEnd<caveStart)throw Error('Purple TH08 point-setup boundary changed');
 body=body.slice(0,caveStart)+'                InstallPointSetup(code_cave, sizeof(code_cave));'+body.slice(caveEnd);
}
if(/GetMem|\*\([^\n]*\*\)/.test(body))throw Error('Unmapped executable address in upstream patch');
body=body.replace(/[ \t]+$/gm,'');
if(legacy){
 const purple=JSON.parse(readFileSync(resolve(repository,'th08_web/sdl-runtime/practice-sections.mjs'),'utf8').split('export const sections=')[1].trim().replace(/;$/,''));
 if(entries.length!==104||entries.some(([key],i)=>purple[i].key!==key))throw Error('Legacy section IDs changed');
 body=body.replaceAll('THStageWarp','THStageWarpLegacy').replaceAll('THPatch','THPatchLegacy');
 const file=resolve(repository,'th08_web/cpp/game/PracticeLegacyPatches.inc');
 const content=`// Blue 2.3.0.3 replay compatibility only (MIT), source sha256 ${digest}.\n// Regenerate with generate-thprac.mjs <blue-checkout> --legacy.\n${body}`;
 if(process.argv.includes('--write'))writeFileSync(file,content);
 else if(process.argv.includes('--check')){if(readFileSync(file,'utf8').replaceAll('\r\n','\n').trimEnd()!==content.trimEnd())throw Error('Stale legacy replay patch');}
 else throw Error('Legacy generation requires --write or --check');
 console.log(JSON.stringify({legacy:true,source:digest,sections:entries.length}));process.exit(0);
}
const glossary=Object.assign({},...Object.values(definitions).map(g=>g.glossary||{}));
const versionHeader=read('thprac/src/thprac/thprac_version.h');
const version=Array.from({length:4},(_,i)=>{const m=versionHeader.match(new RegExp('#define THPRAC_VERSION_'+i+' (\\d+)'));if(!m)throw Error('Purple version boundary changed');return m[1];}).join('.');
const shared=read('thprac/src/thprac/thprac_games.cpp')+read('thprac/src/thprac/thprac_launcher_tools.cpp');
const uiKeys=[...new Set((source+shared).match(/\bTH[A-Z0-9_]+\b/g))].filter(k=>glossary[k]);
// Glossary combo lists encode native NUL separators as literal backslash-zero.
// Emit C++ octal escapes rather than a visible "\\0" or invalid UCN.
const cppLabel=value=>JSON.stringify(value.replaceAll('\\0','\0')).replaceAll('\\u0000','\\000');
const sections=entries.map(([key,value],index)=>({id:index+1,key,stage:value.appearance[0]-1,group:value.appearance[1],spell:!!value.spell,bgm:value.bgm,
 names:Array.from({length:5},(_,difficulty)=>{const selector='ENHLX'[difficulty];const entry=Object.entries(value).find(([k])=>k.startsWith('!')&&(k.includes(selector)||k.includes('X')))?.[1];if(entry!==undefined)return typeof entry==='string'?glossary[entry]||[entry,entry,entry]:entry;return ['','',''];})}));
const files={
 'th08_web/cpp/game/PracticeVersion.hpp':`// Generated from purple THPrac (MIT).\n#pragma once\nnamespace th08 {inline constexpr const char* practice_source_version=${JSON.stringify(version)};}\n`,
 'th08_web/cpp/game/PracticeUiLabels.hpp':`// Generated from purple glossary (MIT).\n#pragma once\nnamespace th08 {\n${uiKeys.map(k=>`inline constexpr const char* practice_${k}[3]{${glossary[k].map(cppLabel).join(',')}};`).join('\n')}\n}\n`,
 'th08_web/cpp/game/PracticePatches.inc':`// Generated from thprac (MIT), sha256 ${digest}.\n// Regenerate with portable/generate-thprac.mjs; included inside PracticePatcher.\n${body}`,
 'th08_web/cpp/game/PracticeSections.hpp':`// Generated from thprac_games_def.json (MIT); upstream enum order is significant.\n#pragma once\nnamespace th08 {\nenum PracticeSection {\n PracticeNone=0,\n${entries.map(([key],i)=>` ${key}=${i+1},`).join('\n')}\n};\nstruct PracticeSectionInfo {int stage,bgm;};\ninline constexpr PracticeSectionInfo practice_sections[]{\n {-1,0},\n${sections.map(s=>` {${s.stage},${s.bgm}},`).join('\n')}\n};\n}\n`,
 'th08_web/sdl-runtime/practice-sections.mjs':`// Generated from thprac (MIT), source sha256 ${digest}.\nexport const sections=${JSON.stringify(sections,null,2)};\n`,
 'th08_web/cpp/game/THPRAC-LICENSE.txt':read('LICENCE'),
};
if(process.argv.includes('--write')){
 for(const [path,content] of Object.entries(files))writeFileSync(resolve(repository,path),content);
 console.log(JSON.stringify({written:Object.keys(files),source:digest}));process.exit(0);
}
if(process.argv.includes('--check')){
 for(const [path,content] of Object.entries(files))if(readFileSync(resolve(repository,path),'utf8').replaceAll('\r\n','\n').trimEnd()!==content.trimEnd())throw Error('Generated thprac file is stale: '+path);
 console.log(JSON.stringify({passed:true,source:digest,sections:entries.length}));process.exit(0);
}
console.log('*** Begin Patch\n'+Object.entries(files).map(([path,content])=>'*** Add File: '+resolve(repository,path).replaceAll('\\','/')+'\n'+content.trimEnd().split('\n').map(line=>'+'+line).join('\n')).join('\n')+'\n*** End Patch');
