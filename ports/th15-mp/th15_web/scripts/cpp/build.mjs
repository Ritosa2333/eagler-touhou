// Build only the verified-module comparison fixture. This is not a playable game.
import {spawnSync,spawn} from 'node:child_process';
import {mkdirSync,readdirSync,readFileSync,writeFileSync,existsSync} from 'node:fs';
import {resolve,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const root=fileURLToPath(new URL('../../',import.meta.url));
const compiler=process.env.WASI_SDK_ROOT?resolve(process.env.WASI_SDK_ROOT,'bin/clang++.exe'):resolve(root,'../th10_web/tools/wasi-sdk-34.0-x86_64-windows/bin/clang++.exe');
const out=resolve(root,'artifacts/cpp');mkdirSync(out,{recursive:true});
const sources=readdirSync(resolve(root,'cpp/game')).filter(n=>n.endsWith('.cpp')).sort().map(n=>'cpp/game/'+n);
sources.push('tests/cpp/core-exports.cpp');
const headers=[...readdirSync(resolve(root,'cpp/game')).filter(n=>/\.(hpp|inc)$/.test(n)).map(n=>'cpp/game/'+n),...readdirSync(resolve(root,'tests/cpp')).filter(n=>n.endsWith('.hpp')).map(n=>'tests/cpp/'+n),...readdirSync(resolve(root,'../portable/sdl')).filter(n=>n.endsWith('.hpp')).map(n=>'../portable/sdl/'+n)].sort();
const wasm=resolve(out,'game-core-test.wasm');
const flags=['--target=wasm32-wasip1','-std=c++17','-O2','-g0','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-mexec-model=reactor','-Wl,--no-entry','-Wl,-z,stack-size=1048576','-Wl,--export-memory','-Wl,--strip-all'];
const objects=resolve(out,'objects');mkdirSync(objects,{recursive:true});
const hash=b=>createHash('sha256').update(b).digest('hex'),headersKey=hash(headers.map(n=>n+hash(readFileSync(resolve(root,n)))).join('\n')),compileFlags=flags.filter(s=>!s.startsWith('-Wl,')&&s!=='-mexec-model=reactor');
const run=args=>new Promise((accept,reject)=>{const child=spawn(compiler,args,{cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe']});let log='';child.stdout.on('data',b=>log+=b);child.stderr.on('data',b=>log+=b);child.on('error',reject);child.on('exit',code=>code?reject(Error(log)):accept());});
let cursor=0;const compiled=new Array(sources.length);
await Promise.all(Array.from({length:4},async()=>{while(cursor<sources.length){const index=cursor++,source=sources[index],object=resolve(objects,source.replaceAll('/','_')+'.o'),key=hash(JSON.stringify([compileFlags,headersKey,hash(readFileSync(resolve(root,source)))]));if(!existsSync(object)||!existsSync(object+'.key')||readFileSync(object+'.key','utf8')!==key){await run([...compileFlags,'-c',source,'-o',object]);writeFileSync(object+'.key',key);}compiled[index]=object;}}));
// Keep the large link input list relative so extraction paths cannot exceed Windows' command limit.
await run([...flags,...compiled.map(p=>relative(root,p)),'-o',wasm]);
const bytes=readFileSync(wasm),mod=new WebAssembly.Module(bytes);
const compilerVersion=spawnSync(compiler,['--version'],{windowsHide:true,encoding:'utf8'}).stdout.trim();
const report={kind:'th15-original-comparison-fixture',completeGame:false,sources,headers,flags,compilerVersion,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex'),sourceHashes:Object.fromEntries([...sources,...headers,'scripts/cpp/build.mjs'].map(s=>[s,createHash('sha256').update(readFileSync(resolve(root,s))).digest('hex')])),imports:WebAssembly.Module.imports(mod)};
writeFileSync(resolve(out,'game-core-test-build.json'),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify(report,null,2));
