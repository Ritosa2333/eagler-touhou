import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';
import {NativeMachine} from '../../scripts/native/machine.mjs';
export const root=fileURLToPath(new URL('../../',import.meta.url)),target=JSON.parse(readFileSync(resolve(root,'target.json'),'utf8'));
export const sha=b=>createHash('sha256').update(b).digest('hex');
const coreFile=process.env.TH11_CORE_TEST_FILE?resolve(process.env.TH11_CORE_TEST_FILE):resolve(root,'artifacts/cpp/game-core-test.wasm');
let coreSnapshot;
export async function core(){const wasi=new WASI({version:'preview1',args:[],env:{},preopens:{}}),bytes=coreSnapshot??=readFileSync(coreFile);const {instance}=await WebAssembly.instantiate(bytes,{wasi_snapshot_preview1:wasi.wasiImport});wasi.initialize(instance);return instance.exports;}
export const memory=(c,p,n)=>new Uint8Array(c.memory.buffer,p,n);
export async function oracle(){
 const bytes=readFileSync(resolve(root,target.executable));if(sha(bytes)!==target.sha256)throw Error('TH11 target hash mismatch');
 const m=await NativeMachine.create(bytes,{wasmBinary:readFileSync(resolve(root,'reference/native/unicorn-bounded.wasm'))});m.resetThreadFPU();
 m.replace(0x460192,'malloc',()=>m.allocate(m.u32(m.reg('ESP')+4)));
 m.replace(0x45fc3b,'free',()=>0);
 m.replace(0x46d630,'memcpy',()=>{const p=m.u32(m.reg('ESP')+4),q=m.u32(m.reg('ESP')+8),n=m.u32(m.reg('ESP')+12);m.write(p,m.bytes(q,n));return p;});
 m.onImport=e=>{if(e.handler)m.ret(e.handler(),e.argc);else throw Error('Unimplemented '+e.dll+'!'+e.name);};
 // Original CRT controller defaults exist before any config/menu function.
 m.call(0x48abf0);
 return m;
}
export function report(name,data){const dir=resolve(root,'artifacts/cpp/verification');mkdirSync(dir,{recursive:true});writeFileSync(resolve(dir,name+'.json'),JSON.stringify({target,coreSha256:sha(coreSnapshot??readFileSync(coreFile)),...data},null,2)+'\n');}
