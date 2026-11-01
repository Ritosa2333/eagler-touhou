import {readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'..'),check=process.argv.includes('--check');
const values=await Promise.all(['original','akyu','sfx'].map(async name=>JSON.parse(await readFile(resolve(root,'resources/audio',name+'.json'),'utf8'))));
const source='// Generated from PR #143 audio metadata; run ports/th05/scripts/generate-audio-layout.mjs.\n'+values.map((value,index)=>`export const ${['ORIGINAL_BGM','AKYU_BGM','ORIGINAL_SFX'][index]}=Object.freeze(${JSON.stringify(value)});\n`).join('')+
 `// Opus is carried in an Ogg container. Only package names change; audio bytes remain identical to the pinned PR.\n`+
 `export const musicFileName=url=>url.replaceAll('/','-').replace(/\\.opus$/,'.ogg');\n`+
 `export const TH05_MUSIC_URLS=Object.freeze([...new Set([ORIGINAL_BGM.tracks['op-m86'].url,AKYU_BGM.tracks['op-m86'].url,...Object.entries(ORIGINAL_BGM.tracks).filter(([id])=>id.endsWith('-m86')).map(([,track])=>track.url),...Object.values(AKYU_BGM.tracks).map(track=>track.url)])]);\n`+
 `export const TH05_MUSIC_FILES=Object.freeze(TH05_MUSIC_URLS.map(musicFileName));\n`;
for(const file of [resolve(root,'web/audio-layout.mjs'),resolve(root,'../../launcher/lib/th05-audio-layout.mjs')]){if(check){if(await readFile(file,'utf8')!==source)throw Error('Stale PR #143 audio metadata: '+file);}else await writeFile(file,source);}
