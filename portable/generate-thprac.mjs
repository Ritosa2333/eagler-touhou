// Emit an apply_patch document. No generated output is written implicitly.
import {existsSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const repository=resolve(import.meta.dirname,'..');
const sourceArgument=process.argv.slice(2).find(value=>!value.startsWith('--'));
if(!sourceArgument)throw Error('Usage: node portable/generate-thprac.mjs <path-to-thprac> [--check|--write]');
const upstream=resolve(sourceArgument);
if(!existsSync(resolve(upstream,'thprac/src/thprac/thprac_th10.cpp')))
 throw Error('Missing upstream thprac checkout at '+upstream+'; pass its path explicitly: node portable/generate-thprac.mjs <path-to-thprac> --check');
const read=p=>readFileSync(resolve(upstream,p),'utf8').replaceAll('\r\n','\n');
const source=read('thprac/src/thprac/thprac_th10.cpp');
const definitions=JSON.parse(read('thprac/src/thprac/thprac_games_def.json'));
const entries=Object.entries(definitions.th10.sections);
const digest=createHash('sha256').update(source).digest('hex');
// Live game globals referenced by the extracted patch region. Each absolute
// address maps to the th10 port field that owns the same storage; the th08
// generator likewise rewrites only the known globals and rejects the rest.
// GameEconomy lives at TH10 0x474c40; the field offsets are verified against
// GameEconomy.hpp's static_asserts (faith_timer 0x14, rank 0x58, flags 0x60).
const globals={
 '0x474c44':'game.score',
 '0x474c48':'game.power',
 '0x474c4c':'game.item_value',
 '0x474c54':'game.faith_timer.previous',
 '0x474c58':'game.faith_timer.current',
 '0x474c5c':'game.faith_timer.fractional',
 '0x474c68':'game.character',
 '0x474c6c':'game.shot_type',
 '0x474c70':'game.lives',
 '0x474c74':'game.difficulty',
 '0x474c7c':'game.stage',
 '0x474c84':'game.section',
 '0x474c9c':'game.rank',
 '0x474ca0':'game.flags',
};
// Rewrite every pointer-cast global access, in all the spellings the upstream
// file uses: *((int32_t*)0xADDR), *(uint32_t*)0xADDR, *(int8_t*)(0xADDR), ...
const globalAccess=/\*\(\s*\(?(?:unsigned\s+|signed\s+)?(?:u?int(?:8|16|32|64)_t|float|double)\s*\*\)\s*\(?(0x[0-9a-fA-F]+)\)?/g;
let body=source.slice(source.indexOf('    __declspec(noinline) void THStageWarp'),source.indexOf('    __declspec(noinline) void THSectionPatch'));
body=body.replaceAll('__declspec(noinline) ','').replaceAll('THPrac::TH10::','').replaceAll('th_sections_t','int');
body=body.replace(globalAccess,(match,address)=>globals[address.toLowerCase()]??match);
if(/GetMem|\*\([^\n]*\*\)/.test(body))throw Error('Unmapped executable address in upstream patch');
const glossary=Object.assign({},...Object.values(definitions).map(g=>g.glossary||{}));
const sections=entries.map(([key,value],index)=>({id:index+1,key,stage:value.appearance[0]-1,group:value.appearance[1],spell:!!value.spell,bgm:value.bgm,
 names:Array.from({length:5},(_,difficulty)=>{const selector='ENHLX'[difficulty];const entry=Object.entries(value).find(([k])=>k.startsWith('!')&&(k.includes(selector)||k.includes('X')))?.[1];if(entry!==undefined)return typeof entry==='string'?glossary[entry]||[entry,entry,entry]:entry;return ['','',''];})}));
const files={
 'th10_web/cpp/game/PracticePatches.inc':`// Generated from thprac (MIT), sha256 ${digest}.\n// Regenerate with portable/generate-thprac.mjs; included inside PracticePatcher.\n${body}`,
 'th10_web/cpp/game/PracticeSections.hpp':`// Generated from thprac_games_def.json (MIT); upstream enum order is significant.\n#pragma once\nnamespace th10 {\nenum PracticeSection {\n PracticeNone=0,\n${entries.map(([key],i)=>` ${key}=${i+1},`).join('\n')}\n};\nstruct PracticeSectionInfo {int stage,bgm;};\ninline constexpr PracticeSectionInfo practice_sections[]{\n {-1,0},\n${sections.map(s=>` {${s.stage},${s.bgm}},`).join('\n')}\n};\n}\n`,
 'th10_web/sdl-runtime/practice-sections.mjs':`// Generated from thprac (MIT), source sha256 ${digest}.\nexport const sections=${JSON.stringify(sections,null,2)};\n`,
 'th10_web/cpp/game/THPRAC-LICENSE.txt':read('LICENCE'),
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
