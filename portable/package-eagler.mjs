import {readFileSync,writeFileSync,mkdirSync,existsSync,readdirSync,rmSync} from 'node:fs';
import {resolve,dirname,relative,sep} from 'node:path';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'..');
const game=existsSync(resolve(root,'th08_web/cpp/game/AnmRenderer.cpp'))?'th08':'th10';
const out=resolve(root,'build-eagler'),buildRoot=resolve(root,game+'_web/artifacts/sdl3');
const fonts=process.env.EAGLER_FONT_ROOT;
if(!fonts)throw Error('Set EAGLER_FONT_ROOT to the private SDL-native font resource directory');
const hash=b=>createHash('sha256').update(b).digest('hex');
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json'),'utf8'));
for(const [name,expected] of Object.entries(build.sourceFiles)){
 if(hash(readFileSync(resolve(root,name)))!==expected)throw Error('Rebuild modified source: '+name);
}
const entry=game==='th08'?'th08.html':'th10.html';
const fontNames=game==='th08'?['blend.bin','cp932.bin']:['msgothic.ttc','simhei.ttf','blend.bin','codepages.bin'];
const runtimeNames=['shell.mjs','eagler-host.mjs',...(game==='th08'?['practice.mjs','practice-config.mjs','practice-sections.mjs']:[])];
const names=[entry,'manifest.json',...runtimeNames,'motion-replay.mjs',game+'-sdl.mjs',game+'-sdl.wasm','resources.json',...fontNames.map(n=>'fonts/'+n)];
const allowed=new Set([...names,'runtime-files.json']);
function walk(dir){return existsSync(dir)?readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(dir,e.name)):[resolve(dir,e.name)]):[];}
if(game==='th08')rmSync(resolve(out,'fonts/msgothic.ttc'),{force:true});
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
write('manifest.json',JSON.stringify({game,protocol:'eagler-touhou/1',adapter:'sdl3-eagler',version:build.version,features,music:['ogg-stream','ogg-full','none',...(game==='th08'?['midi']:[])],touchReplay:false,execution:{kind:build.kind,sha256:build.sha256,loaderSha256:build.loaderSha256,architecture:build.architecture}},null,2)+'\n');
const files=Object.fromEntries(names.map(name=>{const bytes=readFileSync(resolve(out,name));return [name,{bytes:bytes.length,sha256:hash(bytes)}];}));
write('runtime-files.json',JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game,files},null,2)+'\n');
console.log(JSON.stringify({game,out,files:names.length,wasm:build.sha256},null,2));
