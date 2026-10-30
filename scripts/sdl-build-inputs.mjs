import {createHash} from 'node:crypto';
import {readFileSync,readdirSync} from 'node:fs';
import {relative,resolve} from 'node:path';

const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const compiled=/\.(?:cpp|c|hpp|h|inc|cmake)$/;
function walk(directory,pattern=compiled){
  return readdirSync(directory,{withFileTypes:true}).flatMap(entry=>{
    const path=resolve(directory,entry.name);
    return entry.isDirectory()?walk(path,pattern):pattern.test(entry.name)?[path]:[];
  });
}

export function sdlBuildInputs(root,common=resolve(process.env.EAGLER_COMMON_ROOT??resolve(root,'../../common'))){
  const files=[...walk(resolve(root,'portable/sdl')),...walk(resolve(root,'src')),
    ...walk(resolve(common,'browser'),/\.(?:mjs|js)$/),...walk(resolve(common,'contracts'),/\.(?:mjs|js)$/),
    ...walk(resolve(common,'include')),...walk(resolve(common,'src')),...walk(resolve(common,'third_party/imgui')),
    resolve(root,'CMakeLists.txt'),resolve(root,'third_party/stb_vorbis/stb_vorbis.h'),
    ...['build-sdl.mjs','sdl-build-inputs.mjs','toolchain.mjs'].map(name=>resolve(root,'scripts',name)),
    resolve(root,'portable/sdl/scripts/gen-cp932.py')];
  return Object.fromEntries([...new Set(files)].sort().map(path=>[
    relative(root,path).replaceAll('\\','/'),sha(readFileSync(path))]));
}

export function verifySdlBuild(root,build){
  if(build.schema!=='th12-sdl-build/1'||build.platform!=='web')throw Error('SDL Web 构建记录无效');
  if(JSON.stringify(sdlBuildInputs(root))!==JSON.stringify(build.sources))throw Error('SDL 编译输入已改变，请重新编译');
  for(const name of ['touhou12.mjs','touhou12.wasm']){
    const bytes=readFileSync(resolve(root,'site/runtime/sdl',name)),expected=build.artifacts?.[name];
    if(!expected||bytes.length!==expected.bytes||sha(bytes)!==expected.sha256)throw Error('SDL 编译产物身份不符：'+name);
  }
}
