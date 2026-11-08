import {createHash} from 'node:crypto';
import {copyFileSync,existsSync,mkdirSync,readFileSync,readdirSync,writeFileSync} from 'node:fs';
import {dirname,relative,resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawn} from 'node:child_process';
const root=fileURLToPath(new URL('../',import.meta.url));
const args=process.argv.slice(2),allowed=new Set(['--print-inputs','--multiplayer','--release','--package-only','--debug']);
for(const arg of args)if(!allowed.has(arg))throw Error(`Unknown build argument: ${arg}`);
const multiplayer=args.includes('--multiplayer'),variant=multiplayer?'multiplayer':'normal';
const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const slash=name=>name.replaceAll('\\','/');
const walk=directory=>readdirSync(directory,{withFileTypes:true}).flatMap(entry=>entry.isDirectory()?walk(resolve(directory,entry.name)):[resolve(directory,entry.name)]);
const common=resolve(process.env.EAGLER_COMMON_ROOT||resolve(root,'../../common'));
const commonAvailable=existsSync(resolve(common,'include/eagler/input/BrowserGamepad.hpp'));
const sources=['web/cpp/game','web/cpp/sdl'].flatMap(directory=>readdirSync(resolve(root,directory)).filter(n=>n.endsWith('.cpp')).sort().map(n=>resolve(root,directory,n)));
sources.push(resolve(root,'portable/sdl/Renderer.cpp'));
const headers=['web/cpp','portable/sdl','portable/input'].flatMap(directory=>walk(resolve(root,directory))).filter(name=>/\.(?:h|hpp|inc)$/.test(name));
if(commonAvailable)headers.push(resolve(common,'include/eagler/input/BrowserGamepad.hpp'),resolve(common,'browser/browser-gamepad-input.hpp'),resolve(common,'include/eagler/input/DialogueGesture.hpp'),resolve(common,'include/eagler/input/LocalPlayerCue.hpp'),resolve(common,'include/eagler/audio/WebSfx.hpp'));
else headers.push(...walk(resolve(root,'portable/compat')).filter(name=>/\.(?:h|hpp|inc)$/.test(name)));
const flags=['-O2',args.includes('--debug')?'-g2':'-g0','-std=c++17','-fno-exceptions','-fno-rtti','-ffp-contract=off',
 '-I'+resolve(root,'portable/input'),'-I'+resolve(root,'web/cpp/game'),'-I'+resolve(root,'web/cpp/sdl'),'-I'+resolve(root,'portable/sdl'),
 '-I'+(commonAvailable?resolve(common,'include'):resolve(root,'portable/compat')),
 '-DTH_NATIVE_PLATFORM=1','--use-port=sdl3','--no-entry','-sDEFAULT_TO_CXX=1',
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1',
 '-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1',
 '-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,HEAPU8,HEAP32','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0',
 '-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2'];
if(args.includes('--print-inputs')){
 const names=[...new Set([...sources,...headers])].sort();
 const inputs=Object.fromEntries(names.map(name=>{const bytes=readFileSync(name);return [slash(relative(root,name)),{bytes:bytes.length,sha256:sha256(bytes)}];}));
 console.log(JSON.stringify({schema:'eagler-touhou/runtime-inputs/1',directory:'th19',variant,translationUnits:sources.map(name=>slash(relative(root,name))),inputs,compilerFlags:flags},null,2));
}else{
 const output=resolve(process.env.TH19_OUTPUT_DIR||resolve(root,'runtime'));
 const standalone=output===resolve(root,'runtime');
 const build=resolve(process.env.TH19_BUILD_DIR||resolve(root,'web/artifacts/sdl3'));
 let loader=resolve(build,'th19.mjs');
 if(!args.includes('--package-only')){
  const candidates=[process.env.TH19_EMSDK,process.env.EMSDK,resolve(root,'../../toolchains/emsdk'),resolve(root,'tools/emsdk')].filter(Boolean).map(p=>resolve(p));
  let sdk,compiler;
  for(const candidate of candidates){const found=['upstream/emscripten/emcc.py','install/emscripten/emcc.py','emscripten/emcc.py','emcc.py'].map(p=>resolve(candidate,p)).find(existsSync);if(found){sdk=candidate;compiler=found;break;}}
  if(!compiler)throw Error('Emscripten SDK not found. Set TH19_EMSDK or EMSDK.');
  const env={...process.env,EMSDK:sdk,EMCC_CORES:process.env.EMCC_CORES||process.env.TH_BUILD_JOBS||'4'};
  const config=process.env.TH19_EM_CONFIG||process.env.EM_CONFIG||(existsSync(resolve(sdk,'.emscripten'))?resolve(sdk,'.emscripten'):null);
  if(config)env.EM_CONFIG=config;
  const python=process.env.TH19_PYTHON||process.env.TH_PYTHON||process.env.EMSDK_PYTHON||(process.platform==='win32'?'python':'python3');
  mkdirSync(build,{recursive:true});
  await new Promise((accept,reject)=>{const child=spawn(python,[compiler,...flags,...sources,'-o',loader],{cwd:root,env,windowsHide:true,stdio:'inherit'});child.once('error',reject);child.once('exit',code=>code===0?accept():reject(Error(`TH19 compilation failed (${code})`)));});
 }else if(!existsSync(loader))loader=resolve(root,'runtime/th19.mjs');
 mkdirSync(output,{recursive:true});
 const shellDirectory=resolve(root,'web/sdl-runtime');
 const names=['resource-integrity.mjs','shell.mjs','managed.css','keyboard.mjs','netplay.mjs','peer-socket.mjs','rollback-session.mjs','native-adapter.mjs','th19-recording.mjs','replay-session.mjs','directory-keyboard.mjs','keyboard-owners.mjs','keyboard-input-bridge.mjs'];
 for(const name of names)copyFileSync(resolve(shellDirectory,name),resolve(output,name));
 let html=readFileSync(resolve(shellDirectory,'managed.html'),'utf8').replace('content="normal"',`content="${variant}"`);
 writeFileSync(resolve(output,'th19.html'),html);
 if(!multiplayer&&standalone)writeFileSync(resolve(output,'th19-multiplayer.html'),html.replace('content="normal"','content="multiplayer"'));
 for(const extension of ['mjs','wasm']){const source=resolve(dirname(loader),`th19.${extension}`),target=resolve(output,`th19.${extension}`);if(source!==target)copyFileSync(source,target);}
 const buildInputs=[...names,'th19.mjs','th19.wasm'].map(name=>[name,sha256(readFileSync(resolve(output,name)))]);
 const buildId=sha256(JSON.stringify(buildInputs));
 writeFileSync(resolve(output,'manifest.json'),JSON.stringify({game:'th19',protocol:'eagler-touhou/1',variant,buildId,features:{thprac:false,languages:false,focusHitbox:false}},null,2)+'\n');
 const artifacts=['th19.html',...names,'th19.mjs','th19.wasm','manifest.json',...(!multiplayer&&standalone?['th19-multiplayer.html']:[])];
 const files=Object.fromEntries(artifacts.map(name=>{const bytes=readFileSync(resolve(output,name));return [name,{bytes:bytes.length,sha256:sha256(bytes)}];}));
 writeFileSync(resolve(output,'runtime-files.json'),JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game:'th19',variant,files},null,2)+'\n');
 console.log(JSON.stringify({output,variant,files:Object.keys(files),wasm:files['th19.wasm'].sha256}));
}
