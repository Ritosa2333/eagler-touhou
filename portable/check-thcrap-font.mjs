import {execFileSync} from 'node:child_process';
import {mkdirSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createRequire} from 'node:module';
import assert from 'node:assert/strict';
import {pathToFileURL} from 'node:url';
const root=resolve(import.meta.dirname,'..'),workspace=process.env.EAGLER_WORKSPACE;
if(!workspace)throw Error('Set EAGLER_WORKSPACE for private prepared language packs and original fonts');
const {unzipSync}=createRequire(resolve(workspace,'eagler-touhou/package.json'))('fflate');
const sdk=process.env.EMSDK??resolve(workspace,'th08-eagler/tools/emsdk');
const packs=resolve(process.env.TH10_LANGUAGE_PACKS??resolve(workspace,'prepared/th10-auto-dialogue/language'));
const fonts=resolve(process.env.TH10_ORIGINAL_FONTS??resolve(workspace,'th10-eagler/build-eagler/fonts'));
const out=resolve(root,'th10_web/artifacts/thcrap-font');mkdirSync(out,{recursive:true});
const loader=resolve(out,'check.mjs');
execFileSync(process.env.TH_PYTHON??'python',[resolve(sdk,'install/emscripten/emcc.py'),
    '-O2','-std=c++17','-DTH_ENABLE_THCRAP=1','--use-port=sdl3','--use-port=sdl3_ttf',
    '-sDEFAULT_TO_CXX=1','--no-entry','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
    '-sALLOW_MEMORY_GROWTH=1','-sEXPORTED_RUNTIME_METHODS=FS','-sEXPORTED_FUNCTIONS=_test_font_layout',
    resolve(root,'portable/thcrap-font-check.cpp'),'-o',loader],
    {env:{...process.env,EM_CONFIG:process.env.EM_CONFIG??resolve(sdk,'.emscripten')},windowsHide:true,stdio:'inherit'});
const {default:create}=await import(pathToFileURL(loader));
for(const language of ['lang_zh-hans','lang_en']){
    const pack=unzipSync(readFileSync(resolve(packs,language+'.zip')));
    const font=Object.entries(pack).find(([p])=>/fonts\/.*\.(ttf|otf)$/i.test(p));assert(font,'Pack subset font');
    const module=await create({wasmBinary:readFileSync(resolve(out,'check.wasm'))});
    module.FS.mkdirTree('/thcrap/th10/fonts');module.FS.writeFile('/thcrap/th10/fonts/test.ttf',font[1]);
    module.FS.mkdirTree('/fonts');
    for(const name of ['blend.bin','codepages.bin'])module.FS.writeFile('/fonts/'+name,readFileSync(resolve(fonts,name)));
    assert.equal(module._test_font_layout(),0);console.log(language+': actual SDL_ttf raster PASS');
}
