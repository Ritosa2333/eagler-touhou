import {readFileSync,readdirSync,mkdirSync,existsSync,writeFileSync} from 'node:fs';
import {resolve,basename} from 'node:path';
import {createHash} from 'node:crypto';
import {spawn,execFileSync} from 'node:child_process';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'../../../..');
const compiler=process.env.EAGLER_WASI_CXX;
if(!compiler)throw Error('Set EAGLER_WASI_CXX to the WASI SDK clang++ executable');
const variant=process.argv.find(v=>v.startsWith('--variant='))?.split('=')[1]||'both';
if(!['normal','multiplayer','both'].includes(variant))throw Error('Unknown variant: '+variant);
const hash=b=>createHash('sha256').update(b).digest('hex');
const run=args=>new Promise((yes,no)=>{const child=spawn(compiler,args,{stdio:'inherit',windowsHide:true});child.on('error',no);child.on('exit',code=>code?no(Error('C++ compilation failed: '+code)):yes());});
for(const name of variant==='both'?['th075','th075-mp']:[variant==='normal'?'th075':'th075-mp']){
 const port=resolve(root,'ports',name),game=resolve(port,'web/cpp/game'),out=resolve(port,'web/artifacts/animation-tests');mkdirSync(out,{recursive:true});
 const flags=['-O0','-std=c++17','-fno-exceptions','-fno-rtti','-I'+game];
 const headers=[...readdirSync(game).filter(p=>p.endsWith('.hpp')).map(p=>readFileSync(resolve(game,p))),readFileSync(resolve(port,'portable/sdl/GraphicsState.hpp'))];
 const headerHash=hash(Buffer.concat(headers));
 const sources=[resolve(import.meta.dirname,'animation-presentation.cpp'),...readdirSync(game).filter(p=>p.endsWith('.cpp')).map(p=>resolve(game,p))];
 let next=0;const objects=Array(sources.length);
 await Promise.all(Array.from({length:4},async()=>{while(next<sources.length){const i=next++,source=sources[i],object=resolve(out,basename(source)+'.o'),key=hash(headerHash+JSON.stringify(flags)+readFileSync(source));objects[i]=object;if(!existsSync(object+'.key')||readFileSync(object+'.key','utf8')!==key){await run([...flags,'-c',source,'-o',object]);writeFileSync(object+'.key',key);}}}));
 const wasm=resolve(out,'animation-presentation.wasm');execFileSync(compiler,[...flags,...objects,'-Wl,-z,stack-size=2097152','-o',wasm],{stdio:'inherit'});
 console.log('Testing '+name);const wasi=new WASI({version:'preview1',args:[],env:{},preopens:{}});const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});wasi.start(instance);
}
