import {createHash} from 'node:crypto';
import {lstatSync, mkdirSync, readFileSync, realpathSync, writeFileSync} from 'node:fs';
import {dirname, isAbsolute, join, relative, resolve, sep} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';

const root=resolve(dirname(fileURLToPath(import.meta.url)), '..');
const coreReceiptSha='849ed768f4a3438e327ce2580db647a9c9c3e2a751af4b0d21e4fc1db37dd9da';
const coreGeneration='d491ae9dda125966ef873df62779bcbf6aeddd09dc2c09d04eb8afe435aa6f63';
const decoder=new TextDecoder('utf-8',{fatal:true});
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const json=value=>Buffer.from(JSON.stringify(value,null,2)+'\n');
const below=(directory,file)=>{
 const remainder=relative(directory,file);
 return !!remainder&&!isAbsolute(remainder)&&remainder!=='..'&&!remainder.startsWith('..'+sep);
};
function read(root, name) {
 const file=resolve(root,name), stat=lstatSync(file);
 if(!below(root,realpathSync(file))||!stat.isFile()||stat.size<1||stat.size>32*1024*1024)
  throw new Error(`Invalid Runtime input: ${name}`);
 return readFileSync(file);
}
const legal=new Map([
 ['licenses/notice.txt','NOTICE.md'],
 ['licenses/eagler-touch-notice.txt','licenses/EAGLER-TOUCH-NOTICE.md'],
 ['licenses/thprac-mit.txt','licenses/THPRAC-MIT.txt'],
 ['licenses/tweet-meta-mit.txt','licenses/Tweet-META-MIT.txt'],
 ['licenses/eagler-touhou-launcher-gpl-3.0.txt','licenses/eagler-touhou-launcher-GPL-3.0.txt'],
 ['licenses/eagler-touhou-th12-mit.txt','licenses/eagler-touhou-th12-MIT.txt'],
 ['licenses/kikones34-mit.txt','licenses/kikones34-MIT.txt'],
 ['licenses/animation-upstream.txt','src/animation/UPSTREAM_LICENSE.txt'],
 ['licenses/ecl-upstream.txt','src/ecl/UPSTREAM_LICENSE.txt'],
]);

/** Assemble source and the reviewed, precompiled Web core. Does not build or run the game. */
export async function packageLauncherRuntime({coreSite,out}) {
 const core=realpathSync(coreSite), site=realpathSync(join(root,'site'));
 const output=join(realpathSync(dirname(resolve(out))),resolve(out).split(sep).at(-1));
 if(output===core||output===site||below(core,output)||below(site,output))throw new Error('Use a new Runtime output outside both input roots');
 const receiptBytes=read(core,'runtime/build.json');
 if(sha(receiptBytes)!==coreReceiptSha)throw new Error('Expected the accepted Runtime122 v17 core receipt');
 const receipt=JSON.parse(decoder.decode(receiptBytes));
 const coreDir=`runtime/builds/${coreGeneration}`;
 if(receipt.game!=='th128'||receipt.privateCore!==122||receipt.generation!==coreGeneration||receipt.entry!==`${coreDir}/core.mjs`)
  throw new Error('Unexpected Core122 identity');
 const payload=new Map([['runtime/build.json',receiptBytes]]);
 for(const name of ['core.mjs','core.wasm']){
  const path=`${coreDir}/${name}`, bytes=read(core,path), expected=receipt.artifacts[name];
  if(bytes.length!==expected.bytes||sha(bytes)!==expected.sha256)throw new Error(`Core122 file differs: ${name}`);
  payload.set(path,bytes);
 }
 const frontend=JSON.parse(decoder.decode(read(root,'tools/launcher-runtime-files.json')));
 if(!Array.isArray(frontend)||!frontend.includes('play.html')||new Set(frontend).size!==frontend.length)
  throw new Error('Invalid TH128 frontend file list');
 for(const name of frontend){
  if(name!=='play.html'&&!/^(?:runtime\/)?[a-z0-9][a-z0-9._-]*\.(?:mjs|css|json)$/.test(name))
   throw new Error(`Not a Runtime source file: ${name}`);
  let bytes=read(site,name);
  if(name==='play.html'){
   const html=decoder.decode(bytes);
   if(!html.includes('<head>')||html.includes('eagler-data-provider'))throw new Error('Unexpected TH128 page header');
   bytes=Buffer.from(html.replace('<head>','<head>\n  <meta name="eagler-data-provider" content="retail-memory">'));
  }
  if(name==='runtime/launcher-shell.css'){
   const css=decoder.decode(bytes), cover="url('./th128-cover.png')";
   if(css.split(cover).length!==2)throw new Error('Unexpected standalone cover reference');
   bytes=Buffer.from(css.replace(cover,'none'));
  }
  const target=name==='play.html'?'th128.html':name;
  if(payload.has(target))throw new Error(`Duplicate Runtime output: ${target}`);
  payload.set(target,bytes);
 }
 for(const [target,source] of legal)payload.set(target,read(root,source));
 payload.set('manifest.json',json({game:'th128',protocol:'eagler-touhou/1',entry:'th128.html',
  dataProvider:'retail-memory',dataLayout:receipt.packageDataLayout,coreGeneration,
  features:{thprac:true,languages:false,focusHitbox:false}}));
 const files=Object.fromEntries([...payload.keys()].sort().map(name=>{
  const bytes=payload.get(name);return [name,{bytes:bytes.length,sha256:sha(bytes)}];
 }));
 try{mkdirSync(output);}catch(error){
  if(error.code==='EEXIST')throw new Error(`Runtime output already exists: ${output}`,{cause:error});
  throw error;
 }
 for(const name of Object.keys(files)){
  const file=join(output,name);mkdirSync(dirname(file),{recursive:true});
  writeFileSync(file,payload.get(name),{flag:'wx'});
 }
 writeFileSync(join(output,'runtime-files.json'),json({schema:'eagler-touhou/runtime-directory/1',files}),{flag:'wx'});
 return {output,files:Object.keys(files).length,coreGeneration};
}

if(process.argv[1]&&import.meta.url===pathToFileURL(resolve(process.argv[1])).href){
 try{
  const args=new Map();
  for(const arg of process.argv.slice(2)){
   const match=arg.match(/^--(assets|output)=(.+)$/);
   if(!match||args.has(match[1]))throw new Error(`Unknown or duplicate Runtime argument: ${arg}`);
   args.set(match[1],match[2]);
  }
  const coreSite=args.get('assets')??process.env.TH128_CORE_SITE;
  if(!coreSite)throw new Error('Pass --assets=<Runtime122 v17 site folder> or set TH128_CORE_SITE');
  const out=args.get('output')??process.env.EAGLER_RUNTIME_OUTPUT??join(root,'build-eagler');
  console.log(JSON.stringify(await packageLauncherRuntime({coreSite,out})));
 }catch(error){console.error(error.message);process.exitCode=1;}
}
