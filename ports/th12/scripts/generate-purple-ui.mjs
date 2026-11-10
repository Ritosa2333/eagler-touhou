import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
import {sections,chapterCounts} from '../src/practice/sections.mjs';
const root=path.resolve(import.meta.dirname,'..'),upstream=process.argv[2];
if(!upstream)throw Error('Usage: node scripts/generate-purple-ui.mjs PURPLE_SOURCE --write|--check');
const common=path.resolve(process.env.EAGLER_COMMON_ROOT??path.join(root,'../../common'));
const read=name=>fs.readFileSync(path.join(upstream,name),'utf8').replace(/^\uFEFF/,'').replaceAll('\r\n','\n');
const definitionPath='thprac/src/thprac/thprac_games_def.json';
const definitions=JSON.parse(read(definitionPath));
const glossary=Object.assign({},...Object.values(definitions).map(value=>value.glossary??{}));
const groups=Object.assign({},...Object.values(definitions).map(value=>value.groups??{}));
const native=fs.readFileSync(path.join(root,'src/wasm/PracticeUi.cpp'),'utf8');
const inputs=['include/eagler/thprac/PracticeSpeed.inc','include/eagler/thprac/PracticeReaction.inc','include/eagler/thprac/PracticeKeyHud.inc'];
const text=[native,...inputs.map(name=>fs.readFileSync(path.join(common,name),'utf8'))].join('\n');
const keys=[...new Set([...text.matchAll(/practice_(TH[A-Z0-9_]+)/g)].map(match=>match[1]))].sort();
for(const key of keys)if(!glossary[key])throw Error('Missing purple label '+key);
const versionText=read('thprac/src/thprac/thprac_version.h');
const version=Array.from({length:4},(_,index)=>versionText.match(new RegExp('#define THPRAC_VERSION_'+index+' (\\d+)'))?.[1]??'0').join('.');
const labels=keys.map(key=>'inline constexpr const char* practice_'+key+'[3]{'+glossary[key].map(value=>JSON.stringify(value)).join(',')+'};');
const groupKeys=[...new Set([...text.matchAll(/practice_group_(\w+)/g)].map(match=>match[1]))].sort();
const groupLabels=groupKeys.map(key=>{
 const entries=groups[key];if(!entries)throw Error('Missing purple group '+key);
 return 'inline constexpr const char* practice_group_'+key+'[3]['+entries.length+']{'+[0,1,2].map(locale=>'{'+entries.map(name=>JSON.stringify(glossary[name][locale])).join(',')+'}').join(',')+'};';
});
const rows=sections.map(value=>{
 const entry=definitions.th12.sections[value.key];if(!entry)throw Error('Missing purple section '+value.key);
 const names=Array.from({length:5},()=>['','','']);
 for(const [flags,text]of Object.entries(entry))if(flags.startsWith('!')){
  const label=typeof text==='string'?glossary[text]:text;
  for(const flag of flags.slice(1)){const difficulty='ENHLX'.indexOf(flag);if(difficulty>=0)names[difficulty]=label;}
 }
 return '{'+[value.id,value.stage,value.group,!!value.spell,value.phaseCount,!!value.dialogue].join(',')+',{'+names.map(names=>'{'+names.map(value=>JSON.stringify(value)).join(',')+'}').join(',')+'}},';
});
const header=['// Generated from purple thprac_games_def.json (MIT); native ImGui adapter.','#pragma once','namespace th12::practice::ui {',
 ...labels,...groupLabels,'inline constexpr const char* practice_source_version='+JSON.stringify(version)+';',
 'inline constexpr const char* practice_license='+JSON.stringify(read('LICENCE'))+';',
 'struct SectionLabel {int id,stage,group;bool spell;int phases;bool dialogue;const char* names[5][3];};',
 'inline constexpr SectionLabel section_labels[]{',...rows,'};',
 'inline constexpr int chapter_counts[7][2]{'+chapterCounts.map(value=>'{'+value.join(',')+'}').join(',')+'};','}',''].join('\n');
const target=path.join(root,'src/practice/UiCatalog.hpp');
if(process.argv.includes('--write')){
 fs.writeFileSync(target,header);
 const sources=Object.fromEntries([definitionPath,'thprac/src/thprac/thprac_th12.cpp','thprac/src/thprac/thprac_version.h','LICENCE'].map(name=>[name,createHash('sha256').update(read(name)).digest('hex')]));
 fs.writeFileSync(path.join(root,'src/practice/purple-ui-source.json'),JSON.stringify({schema:'th12-purple-ui-source/1',version,sources,shared:inputs,platformReferences:['ports/th10/web/cpp/sdl/ThpracUi.cpp','ports/th11/web/cpp/sdl/ThpracUi.cpp']},null,2)+'\n');
}else if(process.argv.includes('--check')){if(fs.readFileSync(target,'utf8').replaceAll('\r\n','\n')!==header)throw Error('Purple UI catalog is stale');}
else throw Error('Use --write or --check');
console.log('TH12 purple UI catalog: '+keys.length+' labels, '+sections.length+' sections');
