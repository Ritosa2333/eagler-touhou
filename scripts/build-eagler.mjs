// Main station source-build entry. The multiplayer directory keeps its own owner.
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(import.meta.dirname,'..');
function walk(directory){return fs.readdirSync(directory,{withFileTypes:true}).sort((a,b)=>a.name.localeCompare(b.name)).flatMap(entry=>entry.isDirectory()?walk(path.join(directory,entry.name)):[path.join(directory,entry.name)]);}
if(process.argv.includes('--print-inputs')){
  const common=path.resolve(process.env.EAGLER_COMMON_ROOT??path.join(root,'../../common'));
  const files=[...walk(path.join(root,'portable/sdl')),...walk(path.join(root,'src')).filter(f=>/\.(?:cpp|hpp|inc)$/.test(f)),...walk(path.join(common,'third_party/imgui')).filter(f=>/\.(?:cpp|h)$/.test(f)),...walk(path.join(common,'include/eagler/thprac')),path.join(common,'browser/runtime-crash-reporter.js'),path.join(root,'CMakeLists.txt'),path.join(root,'third_party/stb_vorbis/stb_vorbis.h')];
  const inputs=Object.fromEntries([...new Set(files)].sort().map(file=>{const bytes=fs.readFileSync(file);return [path.relative(root,file).split(path.sep).join('/'),{bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')}];}));
  const translationUnits=[...fs.readdirSync(path.join(root,'src/game')).filter(n=>n.endsWith('.cpp')).map(n=>'src/game/'+n),'src/wasm/Exports.cpp','src/wasm/PracticeUi.cpp',...fs.readdirSync(path.join(root,'portable/sdl')).filter(n=>n.endsWith('.cpp')&&n!=='Memory.cpp').map(n=>'portable/sdl/'+n),...['imgui.cpp','imgui_draw.cpp','imgui_tables.cpp','imgui_widgets.cpp','imgui_freetype.cpp'].map(n=>path.relative(root,path.join(common,'third_party/imgui',n)).split(path.sep).join('/'))];
  const data=JSON.stringify({schema:'eagler-touhou/runtime-inputs/1',directory:'th12',translationUnits,inputs,compilerFlags:['-std=c++20','-O3','-ffp-contract=off','-DTH12_SDL_HOST=1','-DTH12_ENABLE_THPRAC=1','-sENVIRONMENT=web']},null,2)+'\n';
  await new Promise((resolve,reject)=>process.stdout.write(data,error=>error?reject(error):resolve()));
}else{
  for(const [script,args]of [['build-sdl.mjs',['--web',...(process.env.TH12_SDL_BUILD_DIRECTORY?['--build-dir='+process.env.TH12_SDL_BUILD_DIRECTORY]:[])]],['package-eagler.mjs',[]]]){
    const result=spawnSync(process.execPath,[path.join(import.meta.dirname,script),...args],{cwd:root,stdio:'inherit',windowsHide:true,env:process.env});
    if(result.error)throw result.error;if(result.status!==0)process.exit(result.status??1);
  }
}
