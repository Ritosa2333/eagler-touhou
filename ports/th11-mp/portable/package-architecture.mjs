// Build a local architecture-preview site for the TH11 portable runtime.
// Mirrors th10/th20 portable/package-architecture.mjs: verify the frozen build,
// compile the shared Launcher, assemble the runtime + content packages, and
// write host/release manifests. Output stays under th11_web/artifacts.
import {readFileSync,writeFileSync,mkdirSync,copyFileSync,readdirSync,existsSync} from 'node:fs';
import {resolve,dirname,relative,basename} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
const workspace=fileURLToPath(new URL('../',import.meta.url)),game='th11',root=resolve(workspace,'th11_web'),launcher=resolve(root,'launcher');
const out=resolve(root,process.argv.includes('--out')?process.argv[process.argv.indexOf('--out')+1]:'artifacts/architecture-candidate'),site=resolve(out,'site');
if(!out.startsWith(resolve(root,'artifacts')+'/')&&!out.startsWith(resolve(root,'artifacts')+'\\'))throw Error('Output must remain in TH11 artifacts');
function argument(name,fallback){const inline=process.argv.find(v=>v.startsWith(name+'='));if(inline)return inline.slice(name.length+1);const i=process.argv.indexOf(name);return i>=0?process.argv[i+1]??fallback:fallback;}
const original=resolve(argument('--original','.'));
const content=resolve(argument('--content',resolve(workspace,'assets-ogg')));
const sha=b=>createHash('sha256').update(b).digest('hex'),write=(p,b)=>{mkdirSync(dirname(p),{recursive:true});writeFileSync(p,b);},copy=(a,b)=>{mkdirSync(dirname(b),{recursive:true});copyFileSync(a,b);};
const walk=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(dir,e.name)):[resolve(dir,e.name)]);
const json=(p,v)=>write(p,JSON.stringify(v,null,2)+'\n');
function tsc(){for(const p of [resolve(workspace,'tools/architecture/typescript/node_modules/typescript/bin/tsc'),resolve(workspace,'../eagler-touhou/node_modules/typescript/bin/tsc')])if(existsSync(p))return p;return null;}
const compiler=tsc();if(!compiler)throw Error('TypeScript compiler not found for the Launcher build');
execFileSync(process.execPath,[compiler,'-p',resolve(launcher,'tsconfig.launcher.json'),'--pretty','false'],{stdio:'inherit',windowsHide:true});
const report=JSON.parse(readFileSync(resolve(root,'artifacts/sdl3/build.json')));
if(report.game!==game||report.architecture?.files!=='sdl-io-idbfs')throw Error('Build the native platform first');
for(const [name,hash] of Object.entries(report.sourceFiles))if(sha(readFileSync(resolve(workspace,name)))!==hash)throw Error('Rebuild modified source: '+name);
const runtimeSource=resolve(workspace,'build-eagler');
if(!existsSync(resolve(runtimeSource,'runtime-files.json')))throw Error('Run portable/package-eagler.mjs first');
// Resolve the upstream browser module graph. Node build/server modules stay private.
const pending=['app.js'],seen=new Set();
function locate(name){for(const path of [resolve(launcher,'public',name),resolve(launcher,'.cache/build/browser',name),resolve(launcher,'src/browser-facades',name),resolve(launcher,name)])if(existsSync(path))return path;throw Error('Missing browser module '+name);}
while(pending.length){const name=pending.pop();if(seen.has(name))continue;if(name.startsWith('../'))throw Error('Escaping browser import');seen.add(name);const path=locate(name),source=readFileSync(path,'utf8');if(/(?:from\s*|import\s*\()['"]node:/.test(source))throw Error('Node module reached browser '+name);write(resolve(site,name),source);
 for(const m of source.matchAll(/(?:\b(?:import|export)\s+(?:[^'";]*?\s+from\s+)?|\bimport\s*\()\s*['"](\.[^'"]+)['"]/g))pending.push(relative(site,resolve(site,dirname(name),m[1])).replaceAll('\\','/'));}
for(const path of walk(resolve(launcher,'public'))){const name=relative(resolve(launcher,'public'),path).replaceAll('\\','/');if(!/\.(?:js|mjs)$/.test(name)||name.startsWith('vendor/')||name==='native-audio.js')copy(path,resolve(site,name));}
// Runtime directory: the frozen package-eagler output, shell entry renamed to th11.html.
for(const path of walk(runtimeSource))copy(path,resolve(site,'runtime',game,relative(runtimeSource,path)));
const dataFile=resolve(original,'th11.dat');
if(!existsSync(dataFile))throw Error('Missing retail th11.dat under --original');
const data=readFileSync(dataFile);
write(resolve(site,'packages',game,game+'.data'),data);
const musicDir=resolve(content,'music');
const musicNames=existsSync(musicDir)?readdirSync(musicDir).filter(n=>n.endsWith('.ogg')).sort():[];
const declarations={'game-data':{source:game+'.data',target:'/th11.dat',bytes:data.length,revision:'sha256-'+sha(data)}};
for(const [i,name] of musicNames.entries()){const bytes=readFileSync(resolve(musicDir,name));write(resolve(site,'packages',game,'music',name),bytes);declarations['music-'+i]={source:'music/'+name,target:'/music/'+name,bytes:bytes.length,revision:'sha256-'+sha(bytes)};}
const layout='sha256-'+sha(JSON.stringify({files:[{filename:'/th11.dat'}]})),revision=game+'-'+report.version+'-'+sha(JSON.stringify(declarations)).slice(0,12);
json(resolve(site,'packages',game,'package.json'),{schema:'eagler-touhou/package/1',game,revision,files:declarations,base:{files:Object.keys(declarations)},components:{},runtimeRequirement:{protocol:'eagler-touhou/1',target:game,dataFile:'game-data',dataLayout:layout}});
json(resolve(site,'release-catalog.json'),{schema:'eagler-touhou/release-catalog/1',games:{[game]:{revision,descriptor:'./packages/'+game+'/package.json'}}});
json(resolve(site,'host-manifest.json'),{schema:'eagler-touhou/host-manifest/1',protocol:'eagler-touhou/1',profile:game+'-sdl3-native',shared:{resourceMode:'hosted',vanillaFont:'',unicodeFont:''},games:{[game]:{runtime:'./runtime/'+game+'/'+game+'.html',gameData:{path:game+'.data',bytes:data.length,sha256:sha(data),version:'sha256-'+sha(data),layout},music:{midi:{files:[]}},languageOptions:[{id:'ja',title:'日本語（原版）',pack:null}],features:{thprac:false,focusHitbox:false}}} });
json(resolve(site,'manifest.json'),{game,version:report.version,execution:{kind:report.kind,architecture:report.architecture,sdlVersion:report.sdlVersion,bytes:report.bytes,sha256:report.sha256,loaderSha256:report.loaderSha256}});
copy(resolve(launcher,'LICENSE'),resolve(site,'LICENSE-launcher.txt'));
write(resolve(site,'THIRD-PARTY-NOTICES.txt'),'Original game and assets: Team Shanghai Alice / ZUN.\nLauncher: YomotsuHisami/eagler-touhou, GPL-3.0-or-later (LICENSE-launcher.txt).\nSDL3: zlib. Emscripten: MIT / University of Illinois-NCSA. miniaudio: public domain/MIT. stb_vorbis: public domain/MIT.\n');
write(resolve(site,'CHANGELOG.txt'),'地灵殿 WEB\n本构建为地灵殿 C++ / SDL3 运行时架构预览，支持连续触控、存档、录像与离线启动。\n');
const precache=walk(site).map(p=>relative(site,p).replaceAll('\\','/')).filter(n=>n!=='files.json'&&n!=='app-shell-sw.js'&&!n.startsWith('packages/'));
const entries=[{url:'./',revision:sha(readFileSync(resolve(site,'index.html')))},...precache.sort().map(n=>({url:n,revision:sha(readFileSync(resolve(site,n)))}))],buildId=sha(JSON.stringify(entries)).slice(0,20);
if(existsSync(resolve(launcher,'src/app-shell-sw.js')))write(resolve(site,'app-shell-sw.js'),readFileSync(resolve(launcher,'src/app-shell-sw.js'),'utf8').replaceAll('__APP_SHELL_BUILD_ID__',buildId).replace('self.__WB_MANIFEST',JSON.stringify(entries)));
const inventory=Object.fromEntries(walk(site).filter(p=>relative(site,p)!=='files.json').sort().map(p=>[relative(site,p).replaceAll('\\','/'),sha(readFileSync(p))]));
json(resolve(site,'files.json'),inventory);
json(resolve(out,'release.json'),{manifest:JSON.parse(readFileSync(resolve(site,'manifest.json'))),buildId,revision,publicFiles:Object.keys(inventory).length,dataBytes:data.length,music:musicNames.length});
console.log(JSON.stringify({out,wasm:report.sha256,buildId,revision,dataBytes:data.length,music:musicNames.length,publicFiles:Object.keys(inventory).length},null,2));
