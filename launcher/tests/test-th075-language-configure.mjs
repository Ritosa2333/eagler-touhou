import {test} from 'vitest';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import vm from 'node:vm';

for(const port of ['th075','th075-mp'])test(`${port} production configure accepts the score-entry variant and rejects unrelated paths`,async()=>{
 const source=await readFile(new URL(`../../ports/${port}/web/sdl-runtime/shell.mjs`,import.meta.url),'utf8');
 const start=source.indexOf('async function command(m)'),end=source.indexOf("window.addEventListener('message'",start);
 assert(start>=0&&end>start);
 const context=vm.createContext({Uint8Array,console});
 vm.runInContext(`let launched=false,options={},music;const game='th075';
 const dispatchInput=()=>false,cancelTouch=()=>{},clearKeyboard=()=>{},updateDisplay=()=>{};
 let writes=0;
 const core={FS:{mkdirTree(){},writeFile(){writes++;}},_th075_music_enabled(){}};
 ${source.slice(start,end)}
 globalThis.configure=command;
 globalThis.languageState=()=>({language:core.th075Language,writes});`,context);
 const pack=paths=>({language:'lang_zh-hans',manifest:{schema:'eagler-touhou/thcrap-static-pack/1',game:'th075',language:'lang_zh-hans',files:paths.map(path=>({path,bytes:1}))},files:paths.map(path=>({path,bytes:new Uint8Array([1])}))});
 const required=['/thcrap/th075/fonts/localized-glyphs.bin','/thcrap/th075/localization/options.json'];
 if(port==='th075-mp')await assert.rejects(context.configure({command:'configure',music:'none',language:'lang_zh-hans'}),/Missing selected TH075 language pack/);
 await context.configure({command:'configure',music:'none',runtimePack:pack([...required,'/thcrap/th075/images/base/data/system/replay.dat.bin'])});
 if(port==='th075-mp'){
  await context.configure({command:'configure',music:'none',language:'lang_zh-hans',options:{netplayMode:'lan'}});
  assert.equal(context.languageState().language,'lang_zh-hans');
  assert.equal(context.languageState().writes,3);
  await assert.rejects(context.configure({command:'configure',music:'none',language:'lang_en'}),/Missing selected TH075 language pack/);
  await context.configure({command:'configure',music:'none',language:'ja'});
  assert.equal(context.languageState().language,null);
 }
 for(const path of ['images/base/data/system/title.dat.bin','text/base/data/system/replay.dat.bin','images/base/../data/system/replay.dat.bin']){
  await assert.rejects(context.configure({command:'configure',music:'none',runtimePack:pack([...required,'/thcrap/th075/'+path])}),/Unsupported TH075 language path/);
 }
});
