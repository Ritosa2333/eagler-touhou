import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {unzipSync} from 'fflate';
const root=resolve(import.meta.dirname,'../..');
test('TH15 official prepared MSGs complete for all four characters and seven stages',{timeout:120000},()=>{
 const sdk=process.env.TH15_EMSDK,packs=process.env.TH15_LANGUAGE_PACKS;assert(sdk&&packs,'Set TH15_EMSDK and TH15_LANGUAGE_PACKS');
 const compiler=['install','upstream'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);assert(compiler);
 const out=resolve(root,'artifacts/thcrap-dialogue'),messages=resolve(out,'messages');mkdirSync(out,{recursive:true});
 for(const language of ['lang_en','lang_zh-hans']){
  const z=unzipSync(readFileSync(resolve(packs,'language',language+'.zip'))),dir=resolve(messages,language);mkdirSync(dir,{recursive:true});
  for(const [name,bytes] of Object.entries(z))if(/^thcrap\/th15\/st\d{2}[a-d]\.msg$/.test(name))writeFileSync(resolve(dir,name.split('/').pop()),bytes);
 }
 const env={...process.env,EM_CONFIG:resolve(sdk,'.emscripten')};
 const run=(command,args,cwd=root)=>{const result=spawnSync(command,args,{cwd,env,encoding:'utf8',windowsHide:true,timeout:90000});assert.equal(result.status,0,(result.error?.message||'')+result.stdout+result.stderr);return result.stdout;};
 const compile=(source,name)=>{const object=resolve(out,name+'.o');run('python',[compiler,'-c',resolve(root,source),'-std=c++17','-o',object]);return object;};
 const ruby=compile('tests/cpp/thcrap-ruby.cpp','ruby'),rubyJs=resolve(out,'ruby.cjs');run('python',[compiler,ruby,'-sDEFAULT_TO_CXX','-sENVIRONMENT=node','-o',rubyJs]);run(process.execPath,[rubyJs]);
 const objects=[compile('tests/cpp/thcrap-dialogue.cpp','test'),compile('cpp/game/Dialogue.cpp','dialogue'),compile('cpp/game/MessageProgram.cpp','messages')],js=resolve(out,'dialogue.cjs');
 run('python',[compiler,...objects,'-sDEFAULT_TO_CXX','-sENVIRONMENT=node','--preload-file',messages+'@/messages','-o',js]);
 const output=run(process.execPath,[js],out);assert.match(output,/PASS scripts=\d+ lines=\d+ ruby=\d+/);
 writeFileSync(resolve(out,'report.json'),JSON.stringify({passed:true,result:output.trim(),scope:'MSG parser/interpreter progression with synthetic font metrics; not rendering or GDI parity'},null,2));
});
