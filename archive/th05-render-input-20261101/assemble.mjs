import {readFile,writeFile,readdir,mkdir,realpath,lstat,rename,unlink} from 'node:fs/promises';
import {resolve,dirname} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {createHash,randomUUID} from 'node:crypto';
import {execFileSync} from 'node:child_process';
import {isDeepStrictEqual} from 'node:util';

const ops=dirname(fileURLToPath(import.meta.url)),input=JSON.parse(await readFile(resolve(ops,'input.json'),'utf8'));
if(input.base!=='/var/www/eagler-touhou-test'||input.siteUrl!=='https://test.touhou.vip/')throw Error('Unexpected publication target');
const previous=await realpath(resolve(input.base,'current')),candidate=resolve(input.base,'releases',input.releaseName),tool='/opt/th12-sdl-test-20261029/launcher';
if(dirname(previous)!==resolve(input.base,'releases')||dirname(candidate)!==resolve(input.base,'releases'))throw Error('Unexpected release path');
const sha=bytes=>createHash('sha256').update(bytes).digest('hex'),json=value=>JSON.stringify(value,null,2)+'\n',read=async path=>JSON.parse(await readFile(path,'utf8'));
if(sha(await readFile(resolve(previous,'deployment.json')))!==input.baselineDeploymentSha256)throw Error('Test site advanced after preparation');
try{await lstat(candidate);throw Error('Candidate already exists');}catch(error){if(error.code!=='ENOENT')throw error;}
const lib=async name=>import(pathToFileURL(resolve(tool,'lib',name+'.mjs')).href);
const {verifyReleaseManifestDeclaration,writeReleaseManifest,verifyReleaseManifest}=await lib('release-manifest');
const {verifyRuntimePublication}=await lib('runtime-generations');
const {readReactFrontendArtifact}=await lib('react-frontend-artifact');
const {writeSiteMetadata}=await lib('site-metadata');
const {buildAppShell}=await lib('app-shell-build');
const {runtimeAppShellPaths}=await lib('app-shell-validation');
const before=await verifyReleaseManifestDeclaration(previous),sourceExceptions=[];
for(const file of before.files){const bytes=await readFile(resolve(previous,file.path));if(file.bytes===bytes.length&&file.sha256===sha(bytes))continue;
 if(file.path!=='sync-version.json'||JSON.parse(bytes).deploymentSha256!==before.deploymentSha256)throw Error('Published source mismatch: '+file.path);sourceExceptions.push(file.path);}
execFileSync('cp',['-al','--',previous,candidate]);
await writeFile(resolve(ops,'state.json'),json({base:input.base,previous,candidate,baselineDeploymentSha256:input.baselineDeploymentSha256}));
async function atomic(path,bytes){await mkdir(dirname(path),{recursive:true});const temporary=path+'.th05-next-'+randomUUID();await writeFile(temporary,bytes,{flag:'wx'});await rename(temporary,path);}
const allowed=new Set(input.files.map(file=>file.path));
for(const file of input.files){if(file.path.startsWith('/')||file.path.split('/').some(part=>!part||part==='..'||part==='.')||file.bytes>96*1024*1024)throw Error('Invalid payload path');const bytes=await readFile(resolve(ops,'site-files',file.path));if(bytes.length!==file.bytes||sha(bytes)!==file.sha256)throw Error('Payload changed: '+file.path);await atomic(resolve(candidate,file.path),bytes);}
const site=await read(resolve(candidate,'site-manifest.json')),oldSite=await read(resolve(previous,'site-manifest.json'));const compare=structuredClone(site);delete compare.games.th05;delete oldSite.games.th05;if(!isDeepStrictEqual(compare,oldSite))throw Error('Other site entries changed');
const runtimes=await read(resolve(candidate,'runtime-manifest.json')),oldRuntimes=await read(resolve(previous,'runtime-manifest.json'));if(!isDeepStrictEqual(runtimes.groups.filter(group=>group.root!=='runtime/th05/'),oldRuntimes.groups.filter(group=>group.root!=='runtime/th05/')))throw Error('Other runtimes changed');
const catalog=await read(resolve(candidate,'release-catalog.json')),oldCatalog=await read(resolve(previous,'release-catalog.json'));const catalogCompare=structuredClone(catalog);delete catalogCompare.games.th05;delete oldCatalog.games.th05;if(!isDeepStrictEqual(catalogCompare,oldCatalog))throw Error('Other packages changed');
await verifyRuntimePublication(candidate,site);
const deployment=await read(resolve(previous,'deployment.json'));deployment.frontend=input.frontend;deployment.generatedAt=new Date().toISOString();
for(const name of ['index.html','en.html','faq.html','about.html','robots.txt','sitemap.xml']){const path=resolve(candidate,name),bytes=await readFile(path);await atomic(path,bytes);allowed.add(name);}await writeSiteMetadata(candidate,input.siteUrl);
const artifact=await readReactFrontendArtifact({directory:candidate,metadata:input.frontend.validationMetadata});
await unlink(resolve(candidate,'app-shell-sw.js'));
const shell=await buildAppShell({quiet:true,globDirectory:candidate,swDest:resolve(candidate,'app-shell-sw.js'),shellFiles:artifact.shellFiles,additionalGlobPatterns:['assets/th05-card.webp'],deferredPaths:runtimeAppShellPaths(site),deferredPathPrefixes:['runtime/'],workerContractSource:await readFile(resolve(ops,'runtime-generations-worker.js'),'utf8')});
allowed.add('app-shell-sw.js');deployment.appShell=shell.contract;
const inventory=[];async function walk(folder,prefix=''){for(const entry of await readdir(folder,{withFileTypes:true})){const name=prefix+entry.name,path=resolve(folder,entry.name);if(entry.name==='.tmp')continue;if(entry.isDirectory())await walk(path,name+'/');else if(entry.isFile()&&!['deployment.json','release-manifest.json','checksums.txt','sync-version.json'].includes(name)){const bytes=await readFile(path);inventory.push({path:name,bytes:bytes.length,sha256:sha(bytes)});}else if(!entry.isFile())throw Error('Unexpected published file '+name);}}
await walk(candidate);deployment.files=inventory.sort((a,b)=>a.path.localeCompare(b.path,'en'));
const nextFiles=new Map(inventory.map(file=>[file.path,file]));let preserved=0;
for(const file of before.files){if(allowed.has(file.path)||file.path==='sync-version.json')continue;const next=nextFiles.get(file.path);if(!next||next.bytes!==file.bytes||next.sha256!==file.sha256)throw Error('Unrelated file changed: '+file.path);preserved++;}
await atomic(resolve(candidate,'deployment.json'),json(deployment));await unlink(resolve(candidate,'release-manifest.json'));await unlink(resolve(candidate,'checksums.txt'));
const publication=await writeReleaseManifest(candidate,{profile:before.profile,sources:{...before.sources,th05NativeTest:input.source},parameters:{...before.parameters,th05NativeTest:{previousRelease:before.releaseId,generation:input.generation.generation,packageRevision:input.packageRevision,legacyEntry:'multiplayer/th05-standalone'}}});
await verifyReleaseManifest(candidate);
if(await realpath(resolve(input.base,'current'))!==previous||sha(await readFile(resolve(previous,'deployment.json')))!==input.baselineDeploymentSha256)throw Error('Test site advanced during preparation');
const result={siteUrl:input.siteUrl,previous,candidate,releaseId:publication.releaseId,buildId:shell.buildId,generation:input.generation.generation,packageRevision:input.packageRevision,files:inventory.length,preserved,sourceExceptions};await writeFile(resolve(ops,'assembled.json'),json(result));console.log(json(result));
