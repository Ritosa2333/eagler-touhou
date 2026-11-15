import {mkdir,readFile,writeFile,copyFile,readdir} from 'node:fs/promises';
import {resolve,dirname,relative} from 'node:path';
import {createHash} from 'node:crypto';
import {readReactFrontendArtifact} from '../../launcher/lib/react-frontend-artifact.mjs';
import {resolveRuntimeGenerationWorkerSource} from '../../launcher/lib/contracts-build.mjs';
import {writeRuntimeGeneration} from '../../launcher/lib/runtime-generations.mjs';
import {runtimeGenerationEntry} from '../../launcher/lib/contracts/runtime-generations.mjs';
import {runtimeFileNames} from '../../launcher/lib/runtime-release.mjs';
import {fileSetIdentity} from '../../launcher/lib/release-manifest.mjs';
import {validateSiteManifest} from '../../launcher/lib/contracts/site-manifest.mjs';
import {validatePackageDescriptor,canonicalPackagePayload} from '../../launcher/package/package-descriptor.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {TH05_DATA_LAYOUT,TH05_MUSIC_FILES} from '../../launcher/lib/th05-content-layout.mjs';

const ops=import.meta.dirname,repo=resolve(ops,'../..'),source=resolve(repo,'../../eagler-touhou'),baseOps=resolve(ops,'baseline');
const payload=resolve(ops,'payload-v2'),site=resolve(payload,'site-files'),content=resolve(source,'ports/th05/artifacts/content'),runtime=resolve(source,'ports/th05/artifacts/runtime-boss');
const sha=bytes=>createHash('sha256').update(bytes).digest('hex'),json=value=>JSON.stringify(value,null,2)+'\n';
const read=async path=>JSON.parse(await readFile(path,'utf8'));
await mkdir(site,{recursive:true});
async function place(name,bytes){const target=resolve(site,name);await mkdir(dirname(target),{recursive:true});await writeFile(target,bytes);}
const artifact=await readReactFrontendArtifact({directory:resolve(repo,'launcher/.cache/build/ui-rewrite/client')});
for(const name of artifact.packageFiles)await place(name,await readFile(artifact.resolveSource(name)));
const generationFiles=await read(resolve(runtime,'runtime-files.json'));
const generation=await writeRuntimeGeneration({site,root:'runtime/th05/',entry:'th05.html',source:runtime,names:runtimeFileNames('th05',generationFiles.files),expected:generationFiles.files});
await place('runtime/th05/th05.html',`<!doctype html><meta charset="utf-8"><script>location.replace(${JSON.stringify(generation.generation+'/th05.html')}+location.search+location.hash);</script>\n`);
await place('assets/th05-card.webp',await readFile(resolve(content,'artwork/th05-card.webp')));
const data=await readFile(resolve(content,'th05.data'));await place('games/th05/th05.data',data);
const inherited=await read(resolve(baseOps,'th12.package.json'));
const file=(source,target,bytes)=>({source,target,revision:sha(bytes).slice(0,16),bytes:bytes.length,sha256:sha(bytes)});
const files={'game-data':file('games/th05/th05.data','/th05.data',data),'shared-msgothic':inherited.files['shared-msgothic'],'shared-unifont':inherited.files['shared-unifont']};
const identities=[];
for(const name of TH05_MUSIC_FILES){const path='games/th05/music/ogg/'+name,bytes=await readFile(resolve(content,'music',name));await place(path,bytes);const info=file(path,'/music/'+name,bytes);files['ogg:'+name]=info;identities.push(info);}
const catalog=await read(resolve(source,'ports/th05/artifacts/languages/catalog.json'));
const language=catalog.languages[0],languagePath='games/th05/'+language.pack.url,languageBytes=await readFile(resolve(source,'ports/th05/artifacts/languages',language.pack.url));
await place(languagePath,languageBytes);files['language:lang_zh-hans']=file(languagePath,'/__eagler/language/lang_zh-hans.zip',languageBytes);
const descriptor={schema:'eagler-touhou/package/1',game:'th05',revision:'pending',runtimeRequirement:{protocol:'eagler-touhou/1',target:'th05',dataFile:'game-data',dataLayout:TH05_DATA_LAYOUT},files,
 base:{files:['game-data','shared-msgothic','shared-unifont']},components:{ogg:{type:'ogg',files:TH05_MUSIC_FILES.map(name=>'ogg:'+name)},language:{type:'language',entries:[{id:language.id,title:language.title,file:'language:lang_zh-hans'}]}}};
descriptor.revision=sha(canonicalPackagePayload(descriptor)).slice(0,16);validatePackageDescriptor(descriptor);await place('th05.package.json',json(descriptor));
const siteManifest=await read(resolve(baseOps,'site-manifest.json')),runtimeManifest=await read(resolve(baseOps,'runtime-manifest.json')),releaseCatalog=await read(resolve(baseOps,'release-catalog.json'));
const previousRuntime=runtimeManifest.groups.find(g=>g.root==='runtime/th05/');delete siteManifest.games.th05;delete releaseCatalog.games.th05;runtimeManifest.groups=runtimeManifest.groups.filter(g=>g.root!=='runtime/th05/');
const product=PRODUCT_GAMES.th05,languageEntry={...language,pack:{...language.pack,url:languagePath}};
siteManifest.games={th05:{number:product.number,title:product.title,subtitle:product.subtitle,runtime:runtimeGenerationEntry('runtime/th05/',generation)+'?hosted=1',
 gameData:{path:'th05.data',bytes:data.length,sha256:sha(data),version:'sha256-'+sha(data),layout:TH05_DATA_LAYOUT},features:{thprac:true,focusHitbox:true},
 music:{midi:{files:[],supported:false},ogg:{mount:'/music',base:'games/th05/music/ogg/',files:TH05_MUSIC_FILES,...fileSetIdentity(TH05_MUSIC_FILES,identities)}},
 languages:[languageEntry],languageOptions:[{id:'ja',title:'日本語',pack:null},languageEntry],package:{revision:descriptor.revision,descriptor:'th05.package.json'}},...siteManifest.games};
validateSiteManifest(siteManifest);runtimeManifest.groups.push({root:'runtime/th05/',current:generation,previous:previousRuntime?[previousRuntime.current,...previousRuntime.previous].slice(0,2):[]});releaseCatalog.games={th05:{revision:descriptor.revision,descriptor:'th05.package.json'},...releaseCatalog.games};
for(const [name,value] of Object.entries({'site-manifest.json':siteManifest,'runtime-manifest.json':runtimeManifest,'release-catalog.json':releaseCatalog}))await place(name,json(value));
await place('host-manifest.json',json({...siteManifest,schema:'eagler-touhou/host-manifest/1'}));
await copyFile(await resolveRuntimeGenerationWorkerSource({project:resolve(repo,'launcher')}),resolve(payload,'runtime-generations-worker.js'));
const nginx=await readFile(resolve(baseOps,'test.touhou.vip'),'utf8');
await writeFile(resolve(payload,'test.touhou.vip.next'),nginx);
const entries=[];async function walk(folder){for(const entry of await readdir(folder,{withFileTypes:true})){const path=resolve(folder,entry.name);if(entry.isDirectory())await walk(path);else{const bytes=await readFile(path);entries.push({path:relative(site,path).replaceAll('\\','/'),bytes:bytes.length,sha256:sha(bytes)});}}}await walk(site);
const input={schema:'eagler-touhou/th05-test-deploy/1',siteUrl:'https://test.touhou.vip/',base:'/var/www/eagler-touhou-test',baselineDeploymentSha256:sha(await readFile(resolve(baseOps,'deployment.json'))),baselineNginxSha256:sha(await readFile(resolve(baseOps,'test.touhou.vip'))),
 frontend:{kind:'react',mountPath:artifact.mountPath,validationMetadata:artifact.metadata},frontendFiles:artifact.packageFiles,files:entries,generation,packageRevision:descriptor.revision,
 source:{nativeCommit:'0c31261ea528db9747962e73b36c5105d2d41b78',audioCommit:'6eb46ed1d4f581f109ffa494ea0d21d54c8b38df',wasmSha256:(await read(resolve(runtime,'build.json'))).sha256}};
input.releaseName='th05-boss-demo-20261101-'+sha(json(input)).slice(0,12);await writeFile(resolve(payload,'input.json'),json(input));console.log(json({release:input.releaseName,files:entries.length,bytes:entries.reduce((n,f)=>n+f.bytes,0),runtimeGeneration:generation.generation,packageRevision:descriptor.revision}));
