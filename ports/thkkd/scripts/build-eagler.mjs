import {createHash} from 'node:crypto';
import {existsSync,mkdirSync,readFileSync,readdirSync,writeFileSync,copyFileSync} from 'node:fs';
import {resolve,relative,dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
const root=fileURLToPath(new URL('../',import.meta.url));
const args=new Set(process.argv.slice(2));
for(const arg of args)if(!['--probe','--print-inputs','--release','--package-only','--symbols'].includes(arg))throw Error(`Unknown argument: ${arg}`);
const probe=args.has('--probe');
const sdk=resolve(process.env.EMSDK||resolve(root,'../../toolchains/emsdk'));
const common=resolve(process.env.EAGLER_COMMON_ROOT||resolve(root,'../../common'));
const build=resolve(root,'build/web');mkdirSync(build,{recursive:true});
const compiler=resolve(sdk,'upstream/emscripten/em++.py');
const version=readFileSync(resolve(sdk,'upstream/emscripten/emscripten-version.txt'),'utf8').trim().replaceAll('"','');
if(version!=='6.0.9')throw Error(`Expected Emscripten 6.0.9, got ${version}`);
const walk=p=>readdirSync(p,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(p,e.name)):[resolve(p,e.name)]);
const sources=probe?[resolve(root,'tests/support/opening_probe.cpp')]:[
    resolve(root,'src/platform/web/Main.cpp'),resolve(root,'src/platform/web/WebHost.cpp'),
    ...walk(resolve(root,'src/render')).filter(p=>p.endsWith('.cpp')),
];
const loader=resolve(build,probe?'core-probe.cjs':'thkkd.mjs');
const flags=['-std=c++17','-O2','-g0','-fexceptions','-fno-fast-math','-ffp-contract=off','-Wno-c++11-narrowing',
    '-include',resolve(root,'src/platform/web/WebMemory.hpp'),
    '-I'+resolve(root,'src'),'-I'+resolve(root,'tests/support'),'-I'+resolve(root,'tests/compat'),'-I'+resolve(common,'include'),
    '-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=8388608','-sINITIAL_MEMORY=134217728',
    '-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-sEXIT_RUNTIME=0'];
if(probe)flags.push('-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sNODERAWFS=1');
else flags.push('--no-entry','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web','-lidbfs.js',
    '-sEXPORTED_RUNTIME_METHODS=FS,IDBFS,HEAPU8,HEAPU32,UTF8ToString');
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const inputs=Object.fromEntries([...new Set([...walk(resolve(root,'src')), ...walk(resolve(root,'tests/support'))])].filter(p=>/\.(cpp|hpp|inc)$/.test(p)).sort().map(p=>[relative(root,p).replaceAll('\\','/'),sha(readFileSync(p))]));
if(args.has('--print-inputs')){console.log(JSON.stringify({schema:'eagler-touhou/runtime-inputs/1',directory:'thkkd',variant:'normal',translationUnits:sources.map(p=>relative(root,p).replaceAll('\\','/')),inputs,compilerFlags:flags},null,2));}
else {
    if(!args.has('--package-only')){
        const temporary=resolve(build,'tmp');mkdirSync(temporary,{recursive:true});
        const env={...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),EMCC_CORES:process.env.TH_BUILD_JOBS||'4',TEMP:temporary,TMP:temporary};
        const run=arguments_=>{const result=spawnSync(process.env.TH_PYTHON||'python',[compiler,...arguments_],{cwd:root,env,stdio:'inherit',windowsHide:true});if(result.error)throw result.error;if(result.status!==0)process.exit(result.status??1);};
        const compileFlags=flags.filter(value=>!value.startsWith('-s')&&!['--no-entry','-lidbfs.js'].includes(value));
        const objects=[];
        for(const source of sources){
            const rendering=source.includes(resolve(root,'src/render'));
            const headers=Object.entries(inputs).filter(([name])=>/\.(hpp|inc)$/.test(name)&&(!rendering||name.startsWith('src/core/')||name.startsWith('src/render/')||['src/platform/web/WebMemory.hpp','src/platform/web/IndexedGameMemory.hpp'].includes(name)));
            const identity=sha(JSON.stringify({version,flags:compileFlags,headers,source:sha(readFileSync(source))}));
            const object=resolve(build,'objects',relative(root,source)+'.o'),stamp=object+'.sha256';mkdirSync(dirname(object),{recursive:true});
            if(!existsSync(object)||!existsSync(stamp)||readFileSync(stamp,'utf8')!==identity){run([...compileFlags,'-c',source,'-o',object]);writeFileSync(stamp,identity);}
            objects.push(object);
        }
        run([...flags,...objects,...(args.has('--symbols')?['--emit-symbol-map']:[]),'-o',loader]);
    }
    const files=Object.fromEntries([loader,loader.replace(/\.(cjs|mjs)$/,'.wasm')].map(p=>[relative(build,p),{bytes:readFileSync(p).length,sha256:sha(readFileSync(p))}]));
    const revision=spawnSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8',windowsHide:true});
    const sourceRecord=resolve(root,'../../SOURCE.json');
    const sourceCommit=revision.status===0?revision.stdout.trim():existsSync(sourceRecord)?JSON.parse(readFileSync(sourceRecord,'utf8')).sourceCommit:null;
    writeFileSync(resolve(build,probe?'probe-build.json':'build.json'),JSON.stringify({schema:'thkkd/web-build/1',compiler:version,sourceCommit,sourceDigest:sha(JSON.stringify(inputs)),inputs,files},null,2)+'\n');
    if(!probe){
        const output=resolve(root,'runtime');mkdirSync(output,{recursive:true});
        const shell=resolve(root,'web');
        for(const p of walk(shell)){
            const target=resolve(output,relative(shell,p));mkdirSync(dirname(target),{recursive:true});
            if(p.endsWith('.mjs'))writeFileSync(target,readFileSync(p,'utf8').replaceAll('../../../common/browser/','./'));
            else copyFileSync(p,target);
        }
        for(const name of ['directory-keyboard.mjs','keyboard-owners.mjs','keyboard-input-bridge.mjs','direct-input-bridge.mjs','save-sync.mjs','local-replay.mjs'])copyFileSync(resolve(common,'browser',name),resolve(output,name));
        for(const name of ['thkkd.mjs','thkkd.wasm'])copyFileSync(resolve(build,name),resolve(output,name));
        mkdirSync(resolve(output,'fonts'),{recursive:true});copyFileSync(resolve(root,'third_party/unifont.woff2'),resolve(output,'fonts/unifont.woff2'));
        copyFileSync(resolve(root,'../../launcher/public/assets/fonts/OFL-Unifont.txt'),resolve(output,'OFL-Unifont.txt'));
        const names=[...walk(shell).map(p=>relative(shell,p)), 'directory-keyboard.mjs','keyboard-owners.mjs','keyboard-input-bridge.mjs','direct-input-bridge.mjs','save-sync.mjs','local-replay.mjs','thkkd.mjs','thkkd.wasm','fonts/unifont.woff2','OFL-Unifont.txt'];
        for(const name of ['UPNG-LICENSE.txt','FFmpeg-WASM-LICENSE.txt','FFmpeg-GPLv2.txt','GARbro-LICENSE.txt']){
            copyFileSync(resolve(root,'third_party',name),resolve(output,name));names.push(name);
        }
        const runtimeFiles=Object.fromEntries(names.sort().map(n=>{const bytes=readFileSync(resolve(output,n));return [n,{bytes:bytes.length,sha256:sha(bytes)}];}));
        const buildId=sha(JSON.stringify(runtimeFiles));
        writeFileSync(resolve(output,'manifest.json'),JSON.stringify({game:'thkkd',protocol:'eagler-touhou/1',variant:'normal',buildId,features:{thprac:false,languages:false,focusHitbox:false}},null,2)+'\n');
        const manifest=readFileSync(resolve(output,'manifest.json'));runtimeFiles['manifest.json']={bytes:manifest.length,sha256:sha(manifest)};
        writeFileSync(resolve(output,'runtime-files.json'),JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game:'thkkd',variant:'normal',files:runtimeFiles},null,2)+'\n');
    }
    console.log(JSON.stringify({build:relative(root,build),compiler:version,files}));
}
