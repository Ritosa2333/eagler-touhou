import {readFileSync,writeFileSync,readdirSync,mkdirSync,existsSync,copyFileSync} from 'node:fs';
import {resolve,dirname,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
const root=fileURLToPath(new URL('../',import.meta.url)),launcher=resolve(root,'launcher'),workspace=resolve(root,'..'),out=resolve(root,process.env.TH09_OUTPUT||'artifacts/sdl-release'),site=resolve(out,'site');
const sha=b=>createHash('sha256').update(b).digest('hex'),walk=p=>readdirSync(p,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(p,e.name)):[resolve(p,e.name)]);
execFileSync(process.execPath,[resolve(workspace,'tools/architecture/typescript/node_modules/typescript/bin/tsc'),'-p',resolve(launcher,'tsconfig.launcher.json'),'--pretty','false'],{stdio:'inherit',windowsHide:true});
const manifest=JSON.parse(readFileSync(resolve(site,'manifest.json'),'utf8'));
function put(name,b){const p=resolve(site,name);mkdirSync(dirname(p),{recursive:true});writeFileSync(p,b);manifest.files['/'+name]={path:name,bytes:b.length,sha256:sha(b),immutable:false};}
function copy(name,p){put(name,readFileSync(p));}
function json(name,v){put(name,Buffer.from(JSON.stringify(v)));}
const pending=['app.js','native-audio.js'],seen=new Set();function locate(n){for(const p of [resolve(launcher,'public',n),resolve(launcher,'.cache/build/browser',n),resolve(launcher,'src/browser-facades',n),resolve(launcher,n)])if(existsSync(p))return p;throw Error('Missing module '+n);}
while(pending.length){const n=pending.pop();if(seen.has(n))continue;if(n.startsWith('../'))throw Error('Import escape');seen.add(n);const text=readFileSync(locate(n),'utf8');if(/(?:from\s*|import\s*\()['"]node:/.test(text))throw Error('Node module');put(n,Buffer.from(text));for(const m of text.matchAll(/(?:\b(?:import|export)\s+(?:[^'";]*?\s+from\s+)?|\bimport\s*\()\s*['"](\.[^'"]+)['"]/g))pending.push(relative(site,resolve(site,dirname(n),m[1])).replaceAll('\\','/'));}
for(const p of walk(resolve(launcher,'public'))){const n=relative(resolve(launcher,'public'),p).replaceAll('\\','/');if(!/\.(js|mjs)$/.test(n)||n.startsWith('vendor/'))copy(n,p);}
// TH09 game art only; common launcher assets are unchanged.
const title=resolve(root,'artifacts/bugfix-20260920/title-card.png');if(existsSync(title))copy('assets/th09-card.png',title);else copy('assets/th09-card.png',resolve(root,'reference/assets/title00.png'));
for(const [name,src] of [['th09.html','managed.html'],['managed.mjs','managed.mjs'],['managed.css','managed.css'],['keyboard.mjs','keyboard.mjs'],['shared-netplay.mjs','shared-netplay.mjs'],['motion-replay.mjs','motion-replay.mjs']])copy('runtime/th09/'+name,resolve(root,'sdl-runtime',src));
for(const ext of ['mjs','wasm'])copy('runtime/th09/th09.'+ext,resolve(out,'th09.'+ext));
const chunks=[],files=[];let size=0;for(const [p,name] of [[resolve(workspace,'[th09] 东方花映塚 (日文版)/th09.dat'),'/th09.dat'],...['cp932.bin','blend.bin','msgothic.ttc'].map(n=>[resolve(root,'assets/sdl-native',n),'/fonts/'+n])]){const b=readFileSync(p);files.push({filename:name,start:size,end:size+b.length});chunks.push(b);size+=b.length;}
const data=Buffer.concat(chunks),music=readdirSync(resolve(root,'assets/sdl-native/music')).filter(n=>n.endsWith('.ogg')).sort(),index={files,remote_package_size:size,music};put('packages/th09/th09.data',data);json('runtime/th09/th09.data.json',index);
const declarations={'game-data':{source:'th09.data',target:'/th09.data',bytes:size,revision:'sha256-'+sha(data)}};for(const [i,n] of music.entries()){const b=readFileSync(resolve(root,'assets/sdl-native/music',n));put('packages/th09/music/'+n,b);declarations['music-'+i]={source:'music/'+n,target:'/music/'+n,bytes:b.length,revision:'sha256-'+sha(b)};}
const layout='sha256-'+sha(JSON.stringify(index)),revision='th09-20260920-'+sha(JSON.stringify(declarations)).slice(0,12),descriptor={schema:'eagler-touhou/package/1',game:'th09',revision,files:declarations,base:{files:Object.keys(declarations)},components:{},runtimeRequirement:{protocol:'eagler-touhou/1',target:'th09',dataFile:'game-data',dataLayout:layout}};
json('packages/th09/package.json',descriptor);json('release-catalog.json',{schema:'eagler-touhou/release-catalog/1',games:{th09:{revision,descriptor:'./packages/th09/package.json'}}});
json('host-manifest.json',{schema:'eagler-touhou/host-manifest/1',protocol:'eagler-touhou/1',profile:'th09-sdl3-native',shared:{resourceMode:'hosted',vanillaFont:'',unicodeFont:''},games:{th09:{runtime:'./runtime/th09/th09.html',gameData:{path:'th09.data',bytes:size,sha256:sha(data),version:'sha256-'+sha(data),layout},music:{midi:{files:[]}},languageOptions:[{id:'ja',title:'日本語（原版）',pack:null}],features:{thprac:false,focusHitbox:false}}}});
copy('LICENSE-launcher.txt',resolve(launcher,'LICENSE'));const notices=readFileSync(resolve(root,'THIRD-PARTY-NOTICES.txt'),'utf8')+'\nLauncher: YomotsuHisami/eagler-touhou, GPL-3.0-or-later. Adapted from the local TH08/TH10 final launcher. See LICENSE-launcher.txt.\n';put('THIRD-PARTY-NOTICES.txt',Buffer.from(notices));
put('app/th09.html',Buffer.from('<!doctype html><meta charset="utf-8"><title>东方花映塚</title><script src="/app/redirect.mjs" type="module"></script><a href="/">打开花映塚</a>'));put('app/redirect.mjs',Buffer.from('location.replace(new URL("/",location.href));'));
put('CHANGELOG.txt',Buffer.from('花映塚 WEB · 2026-09-21 录像与测试组问题修复版\n修复奖励敌人标志和连击符卡演出时的激光更新时序；包含标题切换、残机设置、Extra CPU 计时、房间输入、共用启动器与连续触控。\n'));
const cacheFiles=Object.keys(manifest.files).map(n=>n.slice(1)).filter(n=>!n.startsWith('packages/')&&!n.startsWith('music/')&&n!=='th09.dat'&&!n.startsWith('fonts/')&&!n.startsWith('runtime/')||n.startsWith('runtime/th09/'));
const entries=[{url:'./',revision:manifest.files['/index.html'].sha256},...cacheFiles.filter(n=>!['','app-shell-sw.js','index.html','version.json'].includes(n)).map(n=>({url:n,revision:manifest.files['/'+n].sha256}))];const build=sha(JSON.stringify(entries)).slice(0,20);
put('app-shell-sw.js',Buffer.from(readFileSync(resolve(launcher,'src/app-shell-sw.js'),'utf8').replaceAll('__APP_SHELL_BUILD_ID__',build).replace('self.__WB_MANIFEST',JSON.stringify(entries))));
manifest.files['/']={...manifest.files['/index.html']};manifest.version=sha(JSON.stringify(manifest.files)).slice(0,24);manifest.execution.entry='/';manifest.launcher={protocol:'eagler-touhou/1',source:'local TH08/TH10 final launcher'};json('version.json',{game:'th09',build:manifest.version});writeFileSync(resolve(site,'manifest.json'),JSON.stringify(manifest,null,2));console.log(JSON.stringify({site,build:manifest.version,modules:seen.size}));

copyFileSync(resolve(root,'scripts/release-server.mjs'),resolve(out,'scripts/serve.mjs'));
