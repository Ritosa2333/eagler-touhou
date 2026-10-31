import {spawnSync} from 'node:child_process';
import path from 'node:path';
import fs from 'node:fs';
import {createHash} from 'node:crypto';
import {runtimeSources,runtimeSourceFiles} from './runtime-sources.mjs';
import {practiceBuild} from './practice-build.mjs';
const root=path.resolve(import.meta.dirname,'..');
if(process.argv.includes('--print-inputs')){
  const common=path.resolve(process.env.EAGLER_COMMON_ROOT??path.join(root,'../../common'));
  const practice=practiceBuild(root,common);
  const sources=[...runtimeSources(root),'src/audio/vorbis-decoder.cpp',...practice.sources.map(file=>path.relative(root,file).split(path.sep).join('/'))];
  const names=[...new Set([...runtimeSourceFiles(root),...sources,...practice.inputs.map(file=>path.relative(root,file).split(path.sep).join('/')),
    'third_party/stb_vorbis/stb_vorbis.h',
    path.relative(root,path.join(common,'include/eagler/thprac/PracticeSpeed.hpp')).split(path.sep).join('/')])].sort();
  const inputs=Object.fromEntries(names.map(name=>{const bytes=fs.readFileSync(path.resolve(root,name));
    return [name,{bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')}];}));
  console.log(JSON.stringify({schema:'eagler-touhou/runtime-inputs/1',directory:'th12',
    translationUnits:sources,inputs,compilerFlags:['-std=c++20','-O2','-DTH12_DIAGNOSTIC=0',
      ...practice.flags,'-sENVIRONMENT=web']},null,2));
  process.exit(0);
}
for(const script of ['build.mjs','build-audio.mjs','package-legacy-eagler.mjs']){
  const result=spawnSync(process.execPath,[path.join(import.meta.dirname,script)],{cwd:root,stdio:'inherit',windowsHide:true,env:{...process.env,TH12_BROWSER_BUILD:'1',TH12_DIAGNOSTIC:'0',TH12_CORE_OUTPUT_DIR:path.join(root,'site/runtime'),TH12_THPRAC:process.env.TH12_THPRAC??'1'}});
  if(result.error)throw result.error;if(result.status!==0)process.exit(result.status??1);
}
