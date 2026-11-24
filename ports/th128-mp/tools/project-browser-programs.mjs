import {createHash} from 'node:crypto';
import {createRequire} from 'node:module';
import {lstat, mkdir, readFile, realpath, writeFile} from 'node:fs/promises';
import {basename, dirname, isAbsolute, join, relative, resolve, sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {parseArgs} from 'node:util';
import {canonicalRuntimePayload, isRuntimePath, RUNTIME_GENERATION_SCHEMA, validateRuntimeGeneration} from '../../../launcher/src/contracts/runtime-generations.mts';

const require=createRequire(new URL('../../../launcher/package.json',import.meta.url));
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const json=value=>Buffer.from(`${JSON.stringify(value,null,2)}\n`);
const identity=bytes=>({bytes:bytes.length,sha256:hash(bytes)});
const probe='globalThis.process?.versions?.node&&globalThis.process?.type!="renderer"';
const nodeBootstrap='if(ENVIRONMENT_IS_NODE){const{createRequire}=await import("module");var require=createRequire(import.meta.url)}';
const normalInventory='runtime-files.json',mpInventory='runtime-generation.json',buildPath='runtime/build.json',programPath='runtime/program.json';
const corePath=(generation,file)=>`runtime/builds/${generation}/${file}`;
const discoveryPath='runtime/online-resources.mjs',managedProgramPath='ports/th128-mp/runtime/managed-program.mjs';
const entry='th128.html',digest=/^[a-f0-9]{64}$/;
const parseModule=source=>require('acorn').parse(source,{ecmaVersion:'latest',sourceType:'module'});

// Version-shaped projection of the reviewed Emscripten glue, not an exception
// in the Launcher's browser dependency checker. Never export this as Node glue.
export function projectEmscriptenWebLoader(source){
  const ast=parseModule(source);
  const [factory,...extra]=ast.body.filter(node=>node.type==='FunctionDeclaration'&&node.async&&node.id.name==='Module');
  const exports=ast.body.filter(node=>node.type==='ExportDefaultDeclaration'&&node.declaration.type==='Identifier'&&node.declaration.name==='Module');
  const deny=()=>new Error('Unrecognized Emscripten Node bootstrap in local browser projection');
  if(!factory||extra.length||exports.length!==1)throw deny();
  const statements=factory.body.body;
  const flags=statements.filter(node=>node.type==='VariableDeclaration').flatMap(node=>node.declarations)
    .filter(node=>node.id.type==='Identifier'&&node.id.name==='ENVIRONMENT_IS_NODE');
  const blocks=statements.filter(node=>node.type==='IfStatement'&&source.slice(node.start,node.end)===nodeBootstrap);
  if(flags.length!==1||blocks.length!==1||!flags[0].init)throw deny();
  const flag=flags[0].init,block=blocks[0];
  if(flag.end>=block.start||source.slice(flag.start,flag.end)!==probe)throw deny();
  return source.slice(0,flag.start)+'false'+source.slice(flag.end,block.start)+'var require;'+source.slice(block.end);
}

export function projectLocalOnlineDiscovery(source){
  const exports=parseModule(source).body.filter(node=>node.type==='ExportNamedDeclaration'&&
    node.declaration?.type==='FunctionDeclaration'&&node.declaration.id.name==='readOnlineDeployment');
  const body=exports[0]?.declaration.body;
  if(exports.length!==1||!source.slice(body.start,body.end).includes("new URL('deployment.json',pageURL)"))
    throw new Error('Unrecognized TH128 optional online deployment discovery');
  // Local import mode has no deployment archive. Match the existing 404 case,
  // preserving cancellation and leaving the managed Package path untouched.
  return source.slice(0,body.start)+'{current(signal,isCurrent);return null;}'+source.slice(body.end);
}

export function projectManagedProgramIndex(source){
  const anchor="const {generation,base}=ownerPage(input),index=new URL('runtime/program.json',base);";
  if(source.split(anchor).length!==2)throw new Error('Unrecognized TH128 managed program index');
  // The packaged owner is exactly three directories below its frozen root.
  // Check that this static dependency is also the current page's program index.
  return source.replace(anchor,"const {generation,base}=ownerPage(input),index=new URL('../../../runtime/program.json',import.meta.url);\n"+
    "  check(index.href===base.href+'runtime/program.json','TH128 program index is outside the owned generation');");
}

function inside(root,path){
  const part=relative(root,path);
  return !isAbsolute(part)&&part!=='..'&&!part.startsWith(`..${sep}`);
}
async function read(root,name){
  if(!isRuntimePath(name)||/\.(?:dat|ogg|zip|rpy|th128mp)$/i.test(name))throw new Error(`Not a browser program asset: ${name}`);
  const path=join(root,name),actual=await realpath(path);
  if(!inside(root,actual)||!(await lstat(path)).isFile())throw new Error(`Not an ordinary owned program file: ${name}`);
  return readFile(path);
}
function samePin(bytes,pin){return bytes&&pin?.bytes===bytes.length&&pin.sha256===hash(bytes);}
async function checkedFiles(root,items){
  const output=new Map();
  for(const [name,pin] of items){
    const bytes=await read(root,name);
    if(!samePin(bytes,pin))throw new Error(`Pinned input bytes changed: ${name}`);
    output.set(name,bytes);
  }
  return output;
}
function parsed(files,name){return JSON.parse(files.get(name));}
function move(files,oldName,newName,bytes){
  if(!files.delete(oldName)||files.has(newName))throw new Error(`Unexpected projected core path: ${newName}`);
  files.set(newName,bytes);
}
async function writeVariant(out,files,index,value){
  await mkdir(out);
  for(const [name,bytes] of files){
    const path=join(out,name);await mkdir(dirname(path),{recursive:true});await writeFile(path,bytes,{flag:'wx'});
  }
  await writeFile(join(out,index),json(value),{flag:'wx'});
}

/** Project a source-built standalone hotfix without changing the MP title/background core. */
export async function projectTh128NormalProgram({normal,normalInventorySHA256,output}){
  const source=await realpath(normal),requested=resolve(output),root=join(await realpath(dirname(requested)),basename(requested));
  if(inside(source,root))throw Error('Standalone browser projection must be outside its source');
  const index=await read(source,normalInventory),inventory=JSON.parse(index);
  if(hash(index)!==normalInventorySHA256||inventory.schema!=='eagler-touhou/runtime-directory/1'||!inventory.files||Array.isArray(inventory.files))
    throw Error('Pinned standalone inventory differs');
  const files=await checkedFiles(source,Object.entries(inventory.files));
  const build=parsed(files,buildPath),shell=parsed(files,'manifest.json'),from=build.generation;
  const oldLoader=corePath(from,'core.mjs'),oldWasm=corePath(from,'core.wasm');
  const loader=files.get(oldLoader),wasm=files.get(oldWasm);
  if(build.schema!=='th128-web-core/1'||build.game!=='th128'||!digest.test(from)||build.entry!==oldLoader||shell.coreGeneration!==from||
    !samePin(loader,build.artifacts?.['core.mjs'])||!samePin(wasm,build.artifacts?.['core.wasm']))throw Error('Standalone build identity differs');
  const web=Buffer.from(projectEmscriptenWebLoader(loader.toString('utf8')));
  const to=hash(JSON.stringify(['th128-local-browser-core/1',from,hash(web),hash(wasm)])),entry=corePath(to,'core.mjs');
  move(files,oldLoader,entry,web);move(files,oldWasm,corePath(to,'core.wasm'),wasm);
  files.set(buildPath,json({...build,generation:to,entry,artifacts:{...build.artifacts,'core.mjs':identity(web)},browserProjection:{schema:'th128-local-browser-core/1',from}}));
  files.set('manifest.json',json({...shell,coreGeneration:to}));
  files.set(discoveryPath,Buffer.from(projectLocalOnlineDiscovery(files.get(discoveryPath).toString('utf8'))));
  const filePins=Object.fromEntries([...files].sort(([a],[b])=>a<b?-1:a>b?1:0).map(([name,bytes])=>[name,identity(bytes)]));
  await writeVariant(root,files,normalInventory,{...inventory,files:filePins});
  return {normal:root,coreGeneration:to,sourceCoreGeneration:from,coreWasmSHA256:hash(wasm)};
}

/** Stage only new browser candidates. Original official normal/MP inputs and both Wasm files remain unchanged. */
export async function projectTh128BrowserPrograms({normal,multiplayer,normalInventorySHA256,multiplayerGeneration,output}){
  const [normalRoot,mpRoot]=await Promise.all([realpath(normal),realpath(multiplayer)]);
  const requested=resolve(output),root=join(await realpath(dirname(requested)),basename(requested));
  if(inside(normalRoot,root)||inside(mpRoot,root))throw new Error('Browser projection output must be outside both inputs');
  const nIndex=await read(normalRoot,normalInventory),n=JSON.parse(nIndex);
  if(!digest.test(normalInventorySHA256)||hash(nIndex)!==normalInventorySHA256||n.schema!=='eagler-touhou/runtime-directory/1'||
     !n.files||Array.isArray(n.files)||typeof n.files!=='object')throw new Error('Pinned normal inventory differs');
  const mIndex=JSON.parse(await read(mpRoot,mpInventory)),m=validateRuntimeGeneration(mIndex);
  if(mIndex.schema!==RUNTIME_GENERATION_SCHEMA||m.entry!==entry||m.generation!==multiplayerGeneration||
     hash(canonicalRuntimePayload(m.entry,m.files))!==multiplayerGeneration)throw new Error('Pinned multiplayer generation differs');
  const [normalFiles,mpFiles]=await Promise.all([checkedFiles(normalRoot,Object.entries(n.files)),checkedFiles(mpRoot,m.files.map(file=>[file.path,file]))]);
  const build=parsed(normalFiles,buildPath),shell=parsed(normalFiles,'manifest.json'),program=parsed(mpFiles,programPath);
  const from=build.generation,oldLoader=corePath(from,'core.mjs'),oldWasm=corePath(from,'core.wasm');
  const coop=program.schema==='th128mp-program/2';
  if(coop&&(!digest.test(program.peerGeneration??'')||!digest.test(program.coopGeneration??'')||
    Object.keys(program.files??{}).length!==7))throw Error('Invalid co-op source program receipt');
  const peerBase=coop?`runtime/transport/${program.peerGeneration}/`:'runtime/transport/',peerPath=peerBase+'author-peer.mjs',peerWasmPath=peerBase+'author-peer.wasm';
  const core=normalFiles.get(oldLoader),wasm=normalFiles.get(oldWasm),peer=mpFiles.get(peerPath),peerWasm=mpFiles.get(peerWasmPath);
  const matches=(key,path,bytes)=>program.files?.[key]?.path===path&&samePin(bytes,program.files[key]);
  if(build.game!=='th128'||build.schema!=='th128-web-core/1'||!coop&&build.privateCore!==127||!digest.test(from)||build.entry!==oldLoader||shell.coreGeneration!==from||
     !samePin(core,build.artifacts?.['core.mjs'])||!samePin(wasm,build.artifacts?.['core.wasm'])||
     (!coop&&program.schema!=='th128mp-program/1')||program.coreGeneration!==from||!core.equals(mpFiles.get(oldLoader)??Buffer.alloc(0))||
     !wasm.equals(mpFiles.get(oldWasm)??Buffer.alloc(0))||!matches('core',oldLoader,core)||!matches('wasm',oldWasm,wasm)||
     !matches('peer',peerPath,peer)||!matches('peerWasm',peerWasmPath,peerWasm))throw new Error('Inconsistent pinned TH128 normal-core/peer receipts');
  let battlePins={};
  if(coop){const base=`runtime/coop/${program.coopGeneration}/`,js=base+'coop-core.mjs',bin=base+'coop-core.wasm';
    if(!matches('coopCore',js,mpFiles.get(js))||!matches('coopWasm',bin,mpFiles.get(bin))||program.coopGeneration!==hash(mpFiles.get(bin))||wasm.equals(mpFiles.get(bin)))throw Error('Cooperative battle artifacts differ');
    // Signed by the enclosing generation. The separately built RTC adapter is web-only,
    // so it receives no bootstrap rewrite; only the two dual-host game loaders need one.
    const battle=Buffer.from(projectEmscriptenWebLoader(mpFiles.get(js).toString('utf8')));mpFiles.set(js,battle);battlePins={coopCore:{path:js,...identity(battle)}};
  }
  const webCore=Buffer.from(projectEmscriptenWebLoader(core.toString('utf8'))),webPeer=coop?peer:Buffer.from(projectEmscriptenWebLoader(peer.toString('utf8')));
  const to=hash(JSON.stringify(['th128-local-browser-core/1',from,hash(webCore),hash(wasm)]));
  const newLoader=corePath(to,'core.mjs'),newWasm=corePath(to,'core.wasm');
  for(const files of [normalFiles,mpFiles]){
    move(files,oldLoader,newLoader,webCore);move(files,oldWasm,newWasm,wasm);
  }
  normalFiles.set(buildPath,json({...build,generation:to,entry:newLoader,artifacts:{...build.artifacts,'core.mjs':identity(webCore)},
    browserProjection:{schema:'th128-local-browser-core/1',from}}));
  normalFiles.set('manifest.json',json({...shell,coreGeneration:to}));
  normalFiles.set(discoveryPath,Buffer.from(projectLocalOnlineDiscovery(normalFiles.get(discoveryPath).toString('utf8'))));
  mpFiles.set(peerPath,webPeer);
  mpFiles.set(managedProgramPath,Buffer.from(projectManagedProgramIndex(mpFiles.get(managedProgramPath).toString('utf8'))));
  mpFiles.set(programPath,json({...program,coreGeneration:to,files:{...program.files,...battlePins,
    core:{path:newLoader,...identity(webCore)},wasm:{path:newWasm,...identity(wasm)},peer:{path:peerPath,...identity(webPeer)}}}));
  const inventory=files=>Object.fromEntries([...files.keys()].sort().map(name=>[name,identity(files.get(name))]));
  const mpIdentities=Object.entries(inventory(mpFiles)).map(([path,id])=>({path,...id}));
  const projectedMP=hash(canonicalRuntimePayload(entry,mpIdentities));
  await mkdir(root);
  const normalOut=join(root,'normal'),mpOut=join(root,'multiplayer');
  await writeVariant(normalOut,normalFiles,normalInventory,{...n,files:inventory(normalFiles)});
  await writeVariant(mpOut,mpFiles,mpInventory,{schema:RUNTIME_GENERATION_SCHEMA,generation:projectedMP,entry,files:mpIdentities});
  const result={normal:normalOut,multiplayer:mpOut,multiplayerGeneration:projectedMP,sourceMultiplayerGeneration:multiplayerGeneration,
    coreGeneration:to,sourceCoreGeneration:from,coreWasmSHA256:hash(wasm),peerWasmSHA256:hash(peerWasm)};
  await writeFile(join(root,'projection.json'),json(result),{flag:'wx'});
  return result;
}

if(process.argv[1]&&resolve(process.argv[1])===fileURLToPath(import.meta.url)){
  const {values}=parseArgs({options:Object.fromEntries(['normal','multiplayer','normal-inventory-sha256','multiplayer-generation','output'].map(key=>[key,{type:'string'}]))});
  for(const key of ['normal','multiplayer','normal-inventory-sha256','multiplayer-generation','output'])if(!values[key])throw new Error(`Missing --${key}`);
  console.log(JSON.stringify(await projectTh128BrowserPrograms({...values,normalInventorySHA256:values['normal-inventory-sha256'],
    multiplayerGeneration:values['multiplayer-generation']})));
}
