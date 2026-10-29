// Separate code-only Runtime for manual acceptance of the shared SDL3 frontend.
import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
const root=path.resolve(import.meta.dirname,'..');
const out=path.resolve(process.argv.find(a=>a.startsWith('--output='))?.slice(9)??path.join(root,'build-sdl-eagler'));
const build=JSON.parse(fs.readFileSync(path.join(root,'site/runtime/sdl/build.json'),'utf8'));
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
for(const [name,expected]of Object.entries(build.sources)){if(sha(fs.readFileSync(path.resolve(root,name)))!==expected)throw Error('请先重新编译 SDL 核心：'+name);}
const sources=new Set(['../../common/browser/runtime-crash-reporter.js','site/game.css','site/runtime/sdl/touhou12.wasm']);
function collect(name){
  if(sources.has(name))return;sources.add(name);
  for(const match of fs.readFileSync(path.join(root,name),'utf8').matchAll(/(?:import|export)\s+(?:[^'";]*?\s+from\s*)?['"]([^'"]+)['"]/g)){
    if(!match[1].startsWith('.'))throw Error('非相对模块路径：'+name);
    const next=path.posix.normalize(path.posix.join(path.posix.dirname(name),match[1]));
    if(next.startsWith('../')||!next.endsWith('.mjs'))throw Error('模块路径无效：'+next);collect(next);
  }
}
collect('site/game-sdl.mjs');
const outputNames=new Map(),owners=new Map();for(const source of sources){const name=({'game-sdl.mjs':'game.mjs','touhou12.mjs':'core.mjs','touhou12.wasm':'core.wasm'})[path.posix.basename(source)]??path.posix.basename(source);if(owners.has(name))throw Error('模块重名：'+source+' / '+owners.get(name));owners.set(name,source);outputNames.set(source,name);}
const allowed=new Set([...outputNames.values(),'th12.html','manifest.json','runtime-files.json']);
if(fs.existsSync(out))for(const name of fs.readdirSync(out))if(!allowed.has(name)&&!['game-sdl.mjs','touhou12.mjs','touhou12.wasm'].includes(name))throw Error('请使用单独的 SDL Runtime 输出目录：'+name);
// Retire the previous SDL development filenames from this generated directory.
for(const name of ['game-sdl.mjs','touhou12.mjs','touhou12.wasm']){const file=path.join(out,name);if(fs.existsSync(file))fs.unlinkSync(file);}
fs.mkdirSync(out,{recursive:true});
function reference(source,value){const name=outputNames.get(path.posix.normalize(path.posix.join(path.posix.dirname(source),value)));if(!name)throw Error('模块未打包：'+value);return './'+name;}
for(const source of sources){
  let bytes=fs.readFileSync(path.join(root,source));
  if(source.endsWith('.mjs')){let text=bytes.toString('utf8');text=text.replace(/((?:import|export)\s+(?:[^'";]*?\s+from\s*)?)(['"])(\.[^'"]+)\2/g,(_,prefix,quote,value)=>prefix+quote+reference(source,value)+quote);text=text.replace(/new URL\((['"])(\.?\/?[^'"]+\.wasm)\1,\s*import\.meta\.url\)/g,(_,quote,value)=>'new URL('+quote+reference(source,value.startsWith('.')?value:'./'+value)+quote+', import.meta.url)');bytes=Buffer.from(text);}
  fs.writeFileSync(path.join(out,outputNames.get(source)),bytes);
}
fs.writeFileSync(path.join(out,'th12.html'),fs.readFileSync(path.join(root,'site/sdl.html'),'utf8').replace('./game-sdl.mjs','./game.mjs'));
fs.writeFileSync(path.join(out,'manifest.json'),JSON.stringify({game:'th12',protocol:'eagler-touhou/1',adapter:'cpp-sdl3-gles',version:'0.5.0-sdl',profile:'normal',completeAdapter:true,features:{thprac:build.features?.thprac!==false,languages:true,focusHitbox:false},music:['ogg-stream','ogg-full','none'],touchReplay:true,sourceSha256:build.sourceSha256},null,2)+'\n');
const files=Object.fromEntries([...allowed].filter(name=>name!=='runtime-files.json').sort().map(name=>{const bytes=fs.readFileSync(path.join(out,name));return [name,{bytes:bytes.length,sha256:sha(bytes)}];}));
fs.writeFileSync(path.join(out,'runtime-files.json'),JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game:'th12',files},null,2)+'\n');
console.log(JSON.stringify({runtime:out,files:Object.keys(files).length,applicationExecuted:false}));
