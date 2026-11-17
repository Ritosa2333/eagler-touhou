import {readFile,writeFile,rename} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'../..');
const compiler=process.env.WASI_CXX||'clang++';
const output=resolve(root,'common/pc98/rollback-core.wasm'),temporary=output+'.tmp';
const names=['create','destroy','local','remote','prepare','buttons','mode','x','y','flags','predicted','commit','dirty','rewind','confirmed','retire','refresh','discard','set_decision'];
const args=['-O2','-std=c++17','-fno-exceptions','-mexec-model=reactor','-Icommon/include','-Icommon/tests/fixtures/include',
  'common/pc98/rollback-bridge.cpp','common/src/netplay/NetplayCore.cpp','common/src/netplay/NetplayProtocol.cpp',
  ...names.map(name=>`-Wl,--export=pc98_rollback_${name}`),'-Wl,--strip-all','-o',temporary];
execFileSync(compiler,args,{cwd:root,stdio:'inherit'});
const bytes=await readFile(temporary),module=new WebAssembly.Module(bytes);
if(WebAssembly.Module.exports(module).filter(item=>item.name.startsWith('pc98_rollback_')).length!==names.length)throw Error('PC-98 rollback bridge exports differ');
await rename(temporary,output);
console.log(`PC-98 rollback WASM: ${bytes.length} bytes, sha256 ${createHash('sha256').update(bytes).digest('hex')}`);
