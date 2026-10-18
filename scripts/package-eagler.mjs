// Build the code-only directory Runtime. No original game content is copied.
import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
import {runtimeSourceFiles} from './runtime-sources.mjs';
const root=path.resolve(import.meta.dirname,'..');
const argument=(name,fallback)=>process.argv.find(value=>value.startsWith(name+'='))?.slice(name.length+1)??fallback;
const out=path.resolve(argument('--output',process.env.EAGLER_RUNTIME_OUTPUT??path.join(root,'build-eagler')));
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const coreBuild=JSON.parse(fs.readFileSync(path.join(root,'site/runtime/build.json'),'utf8'));
for(const name of runtimeSourceFiles(root))if(sha(fs.readFileSync(path.join(root,name)))!==coreBuild.sourceFiles[name])throw Error('请先重新编译核心：'+name);
const sources=new Set(['site/game.css','site/runtime/core.wasm','site/runtime/vorbis.wasm','site/runtime/vorbis.mjs']);
function collect(name){
  if(sources.has(name))return;sources.add(name);
  const source=fs.readFileSync(path.join(root,name),'utf8');
  for(const match of source.matchAll(/(?:import|export)\s+(?:[^'";]*?\s+from\s*)?['"]([^'"]+)['"]/g)){
    if(!match[1].startsWith('.'))throw Error('运行时存在非相对模块路径：'+name);
    const dependency=path.posix.normalize(path.posix.join(path.posix.dirname(name),match[1]));
    if(dependency.startsWith('../')||!dependency.endsWith('.mjs'))throw Error('运行时模块路径无效：'+dependency);
    collect(dependency);
  }
}
collect('site/game.mjs');
// The shared directory-Runtime contract permits a closed flat module set.
// Rewrite imports at the packaging boundary; source keeps its normal layout.
const outputNames=new Map(),owners=new Map();
for(const source of sources){const name=path.posix.basename(source);if(owners.has(name))throw Error('运行时模块重名：'+source+' / '+owners.get(name));owners.set(name,source);outputNames.set(source,name);}
const names=['th12.html','manifest.json',...outputNames.values()].sort();
function walk(folder){return fs.existsSync(folder)?fs.readdirSync(folder,{withFileTypes:true}).flatMap(entry=>entry.isDirectory()?walk(path.join(folder,entry.name)):[path.relative(out,path.join(folder,entry.name)).split(path.sep).join('/')]):[];}
for(const name of walk(out))if(!names.includes(name)&&name!=='runtime-files.json')throw Error('输出目录存在旧文件，请使用新的空目录：'+name);
function write(name,bytes){const file=path.join(out,name);fs.mkdirSync(path.dirname(file),{recursive:true});fs.writeFileSync(file,bytes);}
function reference(source,value){
  const target=path.posix.normalize(path.posix.join(path.posix.dirname(source),value));
  const name=outputNames.get(target);if(!name)throw Error('运行时引用未封装：'+source+' -> '+value);return './'+name;
}
for(const source of sources){
  let bytes=fs.readFileSync(path.join(root,source));
  if(source.endsWith('.mjs')){
    let text=bytes.toString('utf8');
    text=text.replace(/((?:import|export)\s+(?:[^'";]*?\s+from\s*)?)(['"])(\.[^'"]+)\2/g,(_,prefix,quote,value)=>prefix+quote+reference(source,value)+quote);
    text=text.replace(/new URL\((['"])(\.[^'"]+)\1,\s*import\.meta\.url\)/g,(_,quote,value)=>'new URL('+quote+reference(source,value)+quote+', import.meta.url)');
    bytes=Buffer.from(text);
  }
  write(outputNames.get(source),bytes);
}
const html=fs.readFileSync(path.join(root,'site/index.html'),'utf8');
write('th12.html',html);
write('manifest.json',JSON.stringify({game:'th12',protocol:'eagler-touhou/1',adapter:'cpp-webgl2-eagler',version:coreBuild.version,builtAt:coreBuild.builtAt,profile:'integration-in-progress',completeAdapter:false,features:{thprac:coreBuild.features?.thprac===true,languages:fs.existsSync(path.join(root,'src/localization/loader.mjs')),focusHitbox:false},music:['ogg-stream','ogg-full','none'],touchReplay:true,sourceSha256:coreBuild.sourceSha256},null,2)+'\n');
const files=Object.fromEntries(names.map(name=>{const bytes=fs.readFileSync(path.join(out,name));return [name,{bytes:bytes.length,sha256:sha(bytes)}];}));
write('runtime-files.json',JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game:'th12',files},null,2)+'\n');
console.log(JSON.stringify({runtime:out,files:names.length,profile:'integration-in-progress'}));
