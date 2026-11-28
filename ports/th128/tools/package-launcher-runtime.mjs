import {createHash} from 'node:crypto';
import {lstatSync, mkdirSync, readFileSync, realpathSync, writeFileSync} from 'node:fs';
import {dirname, isAbsolute, join, relative, resolve, sep} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {describeCoreInputs, matchesBuiltCore} from './build-runtime.mjs';

const root=resolve(dirname(fileURLToPath(import.meta.url)), '..');
const coreReceipts=new Map([
 ['849ed768f4a3438e327ce2580db647a9c9c3e2a751af4b0d21e4fc1db37dd9da',{number:122,generation:'d491ae9dda125966ef873df62779bcbf6aeddd09dc2c09d04eb8afe435aa6f63'}],
 ['acb0336924255e66c976ac4a3e6708fa580adf00a0dfaeb025d2eb6c6ba32be4',{number:123,generation:'071361a3203c099c72b512ad533a0d2920b381f9d38fb9e4c0382ec3a95a0ed1'}],
 ['2756d30105f04e652f99ecab9e56d996112a9817b4354a5e68abb4c6a91d5ff0',{number:124,generation:'c4e97977f1d0c394dc481ffa2dcede13847ea18b9e57973d9fd54e95c40afec1'}],
 ['75d50815689e3f76821cd03c9253adb7397e54ae8a007263a22370adb64a1a62',{number:126,generation:'c5df9dab12659071816382294fd9ce1f3850ea384f49b99021f317b5e0afcefc'}],
 ['f4bc1f5c926c31f6a763bfafa51f55e7590f458f4d9ddf2c16e0e1cda500b028',{number:127,generation:'242292f417b20db60234b7f21604d6e5a0e06d6dca25fc885835844d459491ba'}],
]);
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

export function launcherRuntimeInputs() {
 const {translationUnits,compilerFlags,sourceFiles}=describeCoreInputs();
 const front=JSON.parse(decoder.decode(read(root,'tools/launcher-runtime-files.json')));
 const paths=['tools/build-runtime.mjs','tools/package-launcher-runtime.mjs','tools/launcher-runtime-files.json',
   'tools/sound-identity-library.js', ...front.map(name=>'site/'+name), ...legal.values()];
 const inputs={...sourceFiles};
 for(const name of paths)inputs[name]=sha(read(root,name));
 return {schema:'eagler-touhou/runtime-inputs/1',translationUnits,compilerFlags,inputs};
}

/** Assemble a reviewed build or one compiled from this exact source tree. */
export async function packageLauncherRuntime({coreSite,out}) {
 const core=realpathSync(coreSite), site=realpathSync(join(root,'site'));
 const output=join(realpathSync(dirname(resolve(out))),resolve(out).split(sep).at(-1));
 if(output===core||output===site||below(core,output)||below(site,output))throw new Error('Use a new Runtime output outside both input roots');
 const receiptBytes=read(core,'runtime/build.json');
 const approved=coreReceipts.get(sha(receiptBytes));
 const receipt=JSON.parse(decoder.decode(receiptBytes));
 if(!approved&&!matchesBuiltCore(receipt))throw new Error('TH128 core receipt does not match the current sources');
 const coreGeneration=approved?.generation??receipt.generation;
 const coreDir=`runtime/builds/${coreGeneration}`;
 if(receipt.game!=='th128'||(approved&&receipt.privateCore!==approved.number)||receipt.generation!==coreGeneration||receipt.entry!==`${coreDir}/core.mjs`)
  throw new Error('Unexpected TH128 core identity');
 const payload=new Map([['runtime/build.json',receiptBytes]]);
 for(const name of ['core.mjs','core.wasm']){
  const path=`${coreDir}/${name}`, bytes=read(core,path), expected=receipt.artifacts[name];
  if(bytes.length!==expected.bytes||sha(bytes)!==expected.sha256)throw new Error(`TH128 core file differs: ${name}`);
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
  if(process.argv.slice(2).length===1&&process.argv[2]==='--print-inputs'){
   console.log(JSON.stringify(launcherRuntimeInputs()));
   process.exit(0);
  }
  for(const arg of process.argv.slice(2)){
   const match=arg.match(/^--(assets|output)=(.+)$/);
   if(!match||args.has(match[1]))throw new Error(`Unknown or duplicate Runtime argument: ${arg}`);
   args.set(match[1],match[2]);
  }
  const coreSite=args.get('assets')??process.env.TH128_CORE_SITE??join(root,'site');
  const out=args.get('output')??process.env.EAGLER_RUNTIME_OUTPUT??join(root,'build-eagler');
  console.log(JSON.stringify(await packageLauncherRuntime({coreSite,out})));
 }catch(error){console.error(error.message);process.exitCode=1;}
}
