import {readFileSync,writeFileSync,mkdirSync,existsSync,readdirSync,rmSync} from 'node:fs';
import {resolve,dirname,relative,sep} from 'node:path';
import {createHash} from 'node:crypto';
import {TH10_PRESENTATION_LAB_EXPORTS} from './presentation-lab/native-abi.mjs';
const root=resolve(import.meta.dirname,'..');
const game=existsSync(resolve(root,'th08_web/cpp/game/AnmRenderer.cpp'))?'th08':'th10';
const presentationLab=process.argv.includes('--presentation-lab');
const out=presentationLab?resolve(root,'artifacts/presentation-lab/runtime'):resolve(root,'build-eagler');
const buildRoot=resolve(root,game+'_web/artifacts',presentationLab?'presentation-lab':'sdl3');
const fonts=process.env.EAGLER_FONT_ROOT;
if(!fonts)throw Error('Set EAGLER_FONT_ROOT to the private SDL-native font resource directory');
const hash=b=>createHash('sha256').update(b).digest('hex');
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json'),'utf8'));
if(!!build.diagnostic!==presentationLab)throw Error('Build profile does not match requested package');
const exported=new Set((build.exports||[]).map(entry=>entry.name));
const auditExports=game==='th10'?TH10_PRESENTATION_LAB_EXPORTS:['presentation_lab_freeze','presentation_lab_resume','presentation_lab_draw'];
if(presentationLab){for(const name of auditExports)if(!exported.has(name))throw Error('Diagnostic build is missing '+name);}
else for(const name of exported)if(name.startsWith('presentation_lab_')||name.startsWith('audit_'))throw Error('Production build contains diagnostic export '+name);
for(const [name,expected] of Object.entries(build.sourceFiles)){
 if(hash(readFileSync(resolve(root,name)))!==expected)throw Error('Rebuild modified source: '+name);
}
const entry=game==='th08'?'th08-modern.html':'th10.html';
const fontNames=game==='th08'?['msgothic.ttc','blend.bin','cp932.bin']:['blend.bin','codepages.bin'];
const thprac=build.features?.thprac===true;
const runtimeNames=['shell.mjs','eagler-host.mjs',...(thprac?['practice.mjs','practice-config.mjs','practice-sections.mjs']:[])];
const names=[entry,'manifest.json',...runtimeNames,'motion-replay.mjs',game+'-sdl.mjs',game+'-sdl.wasm','resources.json',...fontNames.map(n=>'fonts/'+n)];
const allowed=new Set([...names,'runtime-files.json']);
function walk(dir){return existsSync(dir)?readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(dir,e.name)):[resolve(dir,e.name)]):[];}
if(game==='th10')for(const name of ['fonts/msgothic.ttc','fonts/simhei.ttf'])rmSync(resolve(out,name),{force:true});
for(const path of walk(out))if(!allowed.has(relative(out,path).split(sep).join('/')))throw Error('Unexpected file in output; select a clean output directory: '+path);
const write=(name,bytes)=>{const path=resolve(out,name);mkdirSync(dirname(path),{recursive:true});writeFileSync(path,bytes);};
const copy=(from,name)=>write(name,readFileSync(from));
const shellRoot=resolve(root,game+'_web/sdl-runtime');
const html=readFileSync(resolve(shellRoot,game+'.html'),'utf8').replace('<head>','<head><meta name="eagler-data-provider" content="retail-memory">');
write(entry,html);
for(const name of runtimeNames)copy(resolve(shellRoot,name),name);
copy(resolve(root,'portable/browser/motion-replay.mjs'),'motion-replay.mjs');
for(const ext of ['mjs','wasm']){
 const bytes=readFileSync(resolve(buildRoot,game+'-sdl.'+ext));
 if(hash(bytes)!==(ext==='wasm'?build.sha256:build.loaderSha256))throw Error('Build identity mismatch');
 write(game+'-sdl.'+ext,bytes);
}
const resources=fontNames.map(name=>{const bytes=readFileSync(resolve(fonts,name));write('fonts/'+name,bytes);return {path:'/fonts/'+name,url:'./fonts/'+name,bytes:bytes.length};});
write('resources.json',JSON.stringify({schema:'eagler-sdl-resources/1',game,resources},null,2)+'\n');
const features={thprac:build.features?.thprac===true,languages:build.features?.languages===true,focusHitbox:build.features?.focusHitbox===true};
write('manifest.json',JSON.stringify({game,protocol:'eagler-touhou/1',adapter:'sdl3-eagler',profile:presentationLab?'presentation-lab':'production',version:build.version,features,music:['ogg-stream','ogg-full','none',...(game==='th08'?['midi']:[])],touchReplay:false,execution:{kind:build.kind,sha256:build.sha256,loaderSha256:build.loaderSha256,architecture:build.architecture}},null,2)+'\n');
const files=Object.fromEntries(names.map(name=>{const bytes=readFileSync(resolve(out,name));return [name,{bytes:bytes.length,sha256:hash(bytes)}];}));
write('runtime-files.json',JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game,files},null,2)+'\n');
console.log(JSON.stringify({game,out,files:names.length,wasm:build.sha256},null,2));
