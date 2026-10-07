// Package the frozen TH11 runtime directory consumed by eagler-touhou.
// Mirrors th10/portable/package-eagler.mjs: verify the build attestation,
// copy the runtime shell + wasm + resource manifest, and write the
// runtime-files.json inventory the Launcher freezes into a generation.
import {readFileSync,writeFileSync,mkdirSync,existsSync,readdirSync,rmSync} from 'node:fs';
import {resolve,dirname,relative,sep} from 'node:path';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'..');
const game='th11';
const out=resolve(root,'build-eagler');
const buildRoot=resolve(root,'th11_web/artifacts/sdl3');
const fonts=process.env.EAGLER_FONT_ROOT;
if(!fonts)throw Error('Set EAGLER_FONT_ROOT to the private SDL-native font resource directory');
const hash=b=>createHash('sha256').update(b).digest('hex');
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json'),'utf8'));
if(!Number.isFinite(Date.parse(build.builtAt)))throw Error('Rebuild Runtime with build timestamp');
if(build.game!==game)throw Error('Build report is not TH11');
if(build.features?.thprac===true){
 const exported=new Set((build.exports||[]).map(entry=>entry.name));
 for(const name of ['th11_practice_configure','sdl_thprac_mouse'])if(!exported.has(name))throw Error('THPrac build is missing '+name);
}
for(const name of build.exports||[])if(name.name.startsWith('presentation_lab_')||name.name.startsWith('audit_')||name.name.startsWith('th11_probe_'))throw Error('Production build contains diagnostic export '+name.name);
for(const [name,expected] of Object.entries(build.sourceFiles)){
 if(hash(readFileSync(resolve(root,name)))!==expected)throw Error('Rebuild modified source: '+name);
}
const entry='th11.html';
// TH11 rasterizes original GDI glyph coverage; the baked tables ship as runtime
// resources under /fonts. The shared host installs them alongside the archive.
const fontNames=['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'];
const runtimeNames=['startup-branding.mjs','shell.mjs','managed.css','keyboard.mjs','directory-keyboard.mjs','eagler-host.mjs'];
const names=[entry,'manifest.json',...runtimeNames,'motion-replay.mjs',game+'-sdl.mjs',game+'-sdl.wasm','resources.json',...fontNames.map(n=>'fonts/'+n)];
const allowed=new Set([...names,'runtime-files.json']);
function walk(dir){return existsSync(dir)?readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(dir,e.name)):[resolve(dir,e.name)]):[];}
for(const path of walk(out))if(!allowed.has(relative(out,path).split(sep).join('/')))throw Error('Unexpected file in output; select a clean output directory: '+path);
const write=(name,bytes)=>{const path=resolve(out,name);mkdirSync(dirname(path),{recursive:true});writeFileSync(path,bytes);};
const copy=(from,name)=>write(name,readFileSync(from));
const shellRoot=resolve(root,'th11_web/sdl-runtime');
const html=readFileSync(resolve(shellRoot,'managed.html'),'utf8').replace('<head>','<head><meta name="eagler-data-provider" content="retail-memory">');
write(entry,html);
for(const name of runtimeNames){
 if(name==='shell.mjs')write(name,readFileSync(resolve(shellRoot,name),'utf8').replace(/\/\*TH11_BUILD_INFO\*\/\{[^;]*\}/,'/*TH11_BUILD_INFO*/'+JSON.stringify({version:build.version,completeGame:true})));
 else copy(resolve(shellRoot,name),name);
}
copy(resolve(root,'portable/browser/motion-replay.mjs'),'motion-replay.mjs');
for(const ext of ['mjs','wasm']){
 const bytes=readFileSync(resolve(buildRoot,game+'-sdl.'+ext));
 if(hash(bytes)!==(ext==='wasm'?build.sha256:build.loaderSha256))throw Error('Build identity mismatch');
 write(game+'-sdl.'+ext,bytes);
}
const resources=fontNames.map(name=>{const bytes=readFileSync(resolve(fonts,name));write('fonts/'+name,bytes);return {path:'/fonts/'+name,url:'./fonts/'+name,bytes:bytes.length};});
write('resources.json',JSON.stringify({schema:'eagler-sdl-resources/1',game,resources},null,2)+'\n');
const features={thprac:build.features?.thprac===true,languages:build.features?.languages===true,focusHitbox:build.features?.focusHitbox===true};
write('manifest.json',JSON.stringify({game,protocol:'eagler-touhou/1',adapter:'sdl3-eagler',profile:'production',builtAt:build.builtAt,version:build.version,features,music:['ogg-stream','ogg-full','none'],touchReplay:false,execution:{kind:build.kind,sha256:build.sha256,loaderSha256:build.loaderSha256,architecture:build.architecture}},null,2)+'\n');
const files=Object.fromEntries(names.map(name=>{const bytes=readFileSync(resolve(out,name));return [name,{bytes:bytes.length,sha256:hash(bytes)}];}));
write('runtime-files.json',JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game,files},null,2)+'\n');
console.log(JSON.stringify({game,out,files:names.length,wasm:build.sha256},null,2));
