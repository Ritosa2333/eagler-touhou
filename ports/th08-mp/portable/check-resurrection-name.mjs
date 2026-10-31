import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'..'),sdk=process.env.WASI_SDK_PATH;
if(!sdk)throw Error('Set WASI_SDK_PATH');
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
for(const folder of [root,resolve(root,'../th08')]){
 const source=readFileSync(resolve(folder,'web/cpp/game/EclNative.cpp'),'utf8');
 const block=/case 22:\{[\s\S]*?\n    \}/.exec(source)?.[0];assert(block,'native resurrection callback missing');
 const input=resolve(out,'resurrection.cpp'),wasm=resolve(out,'resurrection.wasm');
 writeFileSync(input,`#include <cassert>\n#include <cstring>\n#include <string>\nusing u32=unsigned;\nnamespace th08::Localization {const char* translated=nullptr;const char* StringById(const char* key,const char* fallback){assert(std::strcmp(key,"th08 Spell Resurrection")==0);return translated?translated:fallback;}}\nstruct Actions{std::string shown;bool spell_announcement(int portrait,const char* name,int style){assert(portrait==-1&&style==1);shown=name;return true;}};\nint main(){for(const char* translation:{static_cast<const char*>(nullptr),"translated resurrection"}){th08::Localization::translated=translation;Actions recorder;auto* actions=&recorder;struct {u32 spell_flags=17;}globals;struct {bool invalid=false;}vm;using namespace th08;switch(22){${block}}assert(!vm.invalid&&globals.spell_flags==1024);if(translation)assert(recorder.shown==translation);else{assert(recorder.shown.size()==18);assert(static_cast<unsigned char>(recorder.shown[0])==129);}}}\n`);
 execFileSync(resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++'),['--target=wasm32-wasip1','-O2','-std=c++17',input,'-o',wasm],{windowsHide:true,stdio:'inherit'});
 const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
 const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});assert.equal(wasi.start(instance),0);
}
console.log('Resurrection callback: translated name and unchanged Japanese fallback PASS (normal + multiplayer)');

