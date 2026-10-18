import {writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {sections} from '../th10_web/sdl-runtime/practice-sections.mjs';

const quote=value=>JSON.stringify(String(value)).replaceAll('\\u2028','\\u2028').replaceAll('\\u2029','\\u2029');
let out='// Generated verbatim from thprac section data. Do not hand-edit.\n#pragma once\nnamespace th10 {\n';
out+='struct PracticeSectionLabel { int id,stage,group; bool spell; const char* names[5][3]; };\n';
out+='inline constexpr PracticeSectionLabel practice_section_labels[]{\n';
for(const section of sections){
 out+=` {${section.id},${section.stage},${section.group},${section.spell?'true':'false'}, {`;
 out+=section.names.map(row=>`{${row.map(quote).join(',')}}`).join(',');
 out+='}},\n';
}
out+='};\n}\n';
writeFileSync(resolve(import.meta.dirname,'../th10_web/cpp/game/PracticeSectionCatalog.hpp'),out);
