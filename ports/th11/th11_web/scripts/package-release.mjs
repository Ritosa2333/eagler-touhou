import {readFileSync,writeFileSync,readdirSync,mkdirSync,existsSync,copyFileSync,cpSync} from 'node:fs';
import {resolve,dirname,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
const root=fileURLToPath(new URL('../',import.meta.url)),workspace=resolve(root,'..'),launcher=resolve(root,'launcher');
const source=resolve(root,process.env.TH11_OUTPUT||'artifacts/sdl-release'),build=JSON.parse(readFileSync(resolve(source,'build.json'),'utf8'));
const development=process.argv.includes('--development');
if(!development&&!build.completeGame)throw Error('TH11 full reconstruction is incomplete. Final/public packaging is blocked; use --development for the isolated local architecture preview.');
const out=development?resolve(root,'artifacts/architecture-preview'):source,site=resolve(out,'site');
const sha=b=>createHash('sha256').update(b).digest('hex'),walk=p=>readdirSync(p,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(p,e.name)):[resolve(p,e.name)]);
const baseline=JSON.parse(readFileSync(resolve(root,'config/final-baseline.json'),'utf8'));
for(const [p,hash] of Object.entries(baseline.files))if(sha(readFileSync(resolve(workspace,p)))!==hash)throw Error('Final baseline drift: '+p);
const wasm=readFileSync(resolve(source,'th11.wasm')),loader=readFileSync(resolve(source,'th11.mjs'));
if(build.sha256!==sha(wasm))throw Error('Build TH11 SDL3 release first');
for(const [p,hash] of Object.entries({...build.sources,...build.headers}))if(sha(readFileSync(resolve(workspace,p)))!==hash)throw Error('Stale runtime build: '+p);
execFileSync(process.execPath,[resolve(workspace,'tools/architecture/typescript/node_modules/typescript/bin/tsc'),'-p',resolve(launcher,'tsconfig.launcher.json'),'--pretty','false'],{stdio:'inherit',windowsHide:true});
const manifest={game:'th11',version:'',sourceVersion:'Japanese 1.00a',completeGame:build.completeGame,development,execution:{kind:'cpp-sdl3',entry:'/',architecture:build.architecture,wasm:'/runtime/th11/th11.wasm',loader:'/runtime/th11/th11.mjs',sha256:sha(wasm),loaderSha256:sha(loader)},launcher:{protocol:'eagler-touhou/1',source:'TH09 final common launcher / TH08+TH10 shared architecture',baseline:'final-baseline.json'},files:{}};
function put(name,b){if(!name||name.includes('..')||name.startsWith('/'))throw Error('Invalid package path');const p=resolve(site,name);mkdirSync(dirname(p),{recursive:true});writeFileSync(p,b);manifest.files['/'+name]={path:name,bytes:b.length,sha256:sha(b),immutable:false};}
function copy(name,p){put(name,readFileSync(p));}
function json(name,v){put(name,Buffer.from(JSON.stringify(v)));}
const pending=['app.js','native-audio.js'],seen=new Set();function locate(n){for(const p of [resolve(launcher,'public',n),resolve(launcher,'.cache/build/browser',n),resolve(launcher,'src/browser-facades',n),resolve(launcher,n)])if(existsSync(p))return p;throw Error('Missing module '+n);}
while(pending.length){const n=pending.pop();if(seen.has(n))continue;if(n.startsWith('../'))throw Error('Import escape');seen.add(n);const text=readFileSync(locate(n),'utf8');if(/(?:from\s*|import\s*\()['"]node:/.test(text))throw Error('Node module');put(n,Buffer.from(text));for(const m of text.matchAll(/(?:\b(?:import|export)\s+(?:[^'";]*?\s+from\s+)?|\bimport\s*\()\s*['"](\.[^'"]+)['"]/g))pending.push(relative(site,resolve(site,dirname(n),m[1])).replaceAll('\\','/'));}
for(const p of walk(resolve(launcher,'public'))){const n=relative(resolve(launcher,'public'),p).replaceAll('\\','/');if(!/\.(js|mjs)$/.test(n)||n.startsWith('vendor/')){
 if(n==='index.html'&&!development)put(n,Buffer.from(readFileSync(p,'utf8').replace('地灵殿 WEB DEV','地灵殿 WEB 1.0').replace('<span class="brand-version">DEV</span>','<span class="brand-version">1.0</span>')));else copy(n,p);
}}
for(const [name,src] of [['th11.html','managed.html'],['managed.css','managed.css'],['keyboard.mjs','keyboard.mjs'],['eagler-host.mjs','eagler-host.mjs'],['shell.mjs','shell.mjs']])copy('runtime/th11/'+name,resolve(root,'sdl-runtime',src));
const buildMarker="/*TH11_BUILD_INFO*/{version:'development-incomplete',completeGame:false}",managed=readFileSync(resolve(root,'sdl-runtime/shell.mjs'),'utf8');
if(!managed.includes(buildMarker))throw Error('Runtime build-info marker missing');
put('runtime/th11/shell.mjs',Buffer.from(managed.replace(buildMarker,JSON.stringify({version:(development?'development-':'1.0.0-sdl3-')+build.sha256.slice(0,12),completeGame:!development&&build.completeGame}))));
put('runtime/th11/th11.mjs',loader);put('runtime/th11/th11.wasm',wasm);
const chunks=[],files=[];let size=0;function dataFile(filename,path){const bytes=readFileSync(path);chunks.push(bytes);files.push({filename,start:size,end:size+bytes.length});size+=bytes.length;}
dataFile('/th11.dat',resolve(workspace,'[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
for(const name of ['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'])dataFile('/fonts/'+name,resolve(root,'assets/sdl-native/fonts',name));
const data=Buffer.concat(chunks),music=readdirSync(resolve(root,'assets/sdl-native/music')).filter(n=>n.endsWith('.ogg')).sort(),index={files,remote_package_size:size,music};
put('packages/th11/th11.data',data);json('runtime/th11/th11.data.json',index);
const declarations={'game-data':{source:'th11.data',target:'/th11.data',bytes:size,revision:'sha256-'+sha(data)}};
for(const [i,n]of music.entries()){const bytes=readFileSync(resolve(root,'assets/sdl-native/music',n));put('packages/th11/music/'+n,bytes);declarations['music-'+i]={source:'music/'+n,target:'/music/'+n,bytes:bytes.length,revision:'sha256-'+sha(bytes)};}
const layout='sha256-'+sha(JSON.stringify(index)),revision='th11-'+sha(JSON.stringify(declarations)).slice(0,16);
json('packages/th11/package.json',{schema:'eagler-touhou/package/1',game:'th11',revision,files:declarations,base:{files:Object.keys(declarations)},components:{},runtimeRequirement:{protocol:'eagler-touhou/1',target:'th11',dataFile:'game-data',dataLayout:layout}});
json('release-catalog.json',{schema:'eagler-touhou/release-catalog/1',games:{th11:{revision,descriptor:'./packages/th11/package.json'}}});
json('host-manifest.json',{schema:'eagler-touhou/host-manifest/1',protocol:'eagler-touhou/1',profile:'th11-sdl3-native',shared:{resourceMode:'hosted',vanillaFont:'',unicodeFont:''},games:{th11:{runtime:'./runtime/th11/th11.html',gameData:{path:'th11.data',bytes:size,sha256:sha(data),version:'sha256-'+sha(data),layout},music:{midi:{files:[]}},languageOptions:[{id:'ja',title:'日本語（原版）',pack:null}],features:{thprac:false,focusHitbox:false}}}});
copy('LICENSE-launcher.txt',resolve(launcher,'LICENSE'));put('THIRD-PARTY-NOTICES.txt',Buffer.from('Original game and assets: Team Shanghai Alice / ZUN.\nLauncher: YomotsuHisami/eagler-touhou, GPL-3.0-or-later, adapted from the local final TH08/TH10/TH09 launcher. See LICENSE-launcher.txt.\nSDL3: zlib. Emscripten: MIT / University of Illinois-NCSA.\n'));
copy('final-baseline.json',resolve(root,'config/final-baseline.json'));
put('app/th11.html',Buffer.from('<!doctype html><meta charset="utf-8"><title>东方地灵殿</title><script src="/app/redirect.mjs" type="module"></script><a href="/">打开地灵殿</a>'));
put('app/redirect.mjs',Buffer.from('location.replace(new URL("/",location.href));'));
put('CHANGELOG.txt',Buffer.from(development?'地灵殿开发验证构建\n本构建供本地验证。\n支持连续触控、存档、录像与离线启动。\n':'地灵殿 WEB · 日文原版 1.00a\n修复符卡 Bonus 奖励分数与 History 收取记录缺失，恢复失败标记及原版透明度。\n支持连续触控、存档、录像与离线启动。\n修复跨关录像输入交接、退出后重新启动、手机音频解锁与六面音乐暂停时序。\n'));
const entries=[{url:'./',revision:manifest.files['/index.html'].sha256},...Object.keys(manifest.files).map(n=>n.slice(1)).filter(n=>!n.startsWith('packages/')&&!['app-shell-sw.js','index.html','version.json'].includes(n)).map(n=>({url:n,revision:manifest.files['/'+n].sha256}))];
const cacheBuild='th11-'+sha(JSON.stringify(entries)).slice(0,20);put('app-shell-sw.js',Buffer.from(readFileSync(resolve(launcher,'src/app-shell-sw.js'),'utf8').replaceAll('__APP_SHELL_BUILD_ID__',cacheBuild).replace('self.__WB_MANIFEST',JSON.stringify(entries))));
manifest.files['/']={...manifest.files['/index.html']};manifest.version=sha(JSON.stringify(manifest.files)).slice(0,24);json('version.json',{game:'th11',build:manifest.version});writeFileSync(resolve(site,'manifest.json'),JSON.stringify(manifest,null,2)+'\n');
mkdirSync(resolve(out,'scripts'),{recursive:true});copyFileSync(resolve(workspace,'th09_web/scripts/release-server.mjs'),resolve(out,'scripts/serve.mjs'));copyFileSync(resolve(workspace,'th09_web/scripts/netplay-relay.mjs'),resolve(out,'scripts/netplay-relay.mjs'));cpSync(resolve(workspace,'th09_web/node_modules/ws'),resolve(out,'node_modules/ws'),{recursive:true});
writeFileSync(resolve(out,'package.json'),JSON.stringify({name:'th11-native-web',private:true,type:'module',scripts:{start:'node scripts/serve.mjs --port '+(development?'8113':'3007')}},null,2)+'\n');
console.log(JSON.stringify({site,version:manifest.version,modules:seen.size,completeGame:manifest.completeGame,development}));
