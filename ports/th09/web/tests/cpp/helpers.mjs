import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';
import {NativeMachine} from '../../scripts/native/machine.mjs';
export const root=fileURLToPath(new URL('../../',import.meta.url));
export const target=JSON.parse(readFileSync(resolve(root,'target.json'),'utf8'));
export const bits=value=>new Uint32Array(new Float32Array([value]).buffer)[0];
let loadedCoreSha256=null;
export async function oracle(){
    const bytes=readFileSync(resolve(root,target.executable));
    if(createHash('sha256').update(bytes).digest('hex')!==target.sha256)throw Error('TH09 oracle executable does not match target.json');
    const m=await NativeMachine.create(bytes,{wasmBinary:readFileSync(resolve(root,'reference/native/unicorn-bounded.wasm'))});
    m.resetThreadFPU();m.reg('FPCW',0x7f);
    const allocations=new Map();
    m.onImport=e=>{
        const arg=i=>m.u32(m.reg('ESP')+4+i*4);
        if(e.handler){m.ret(e.handler(),e.argc);return;}
        if(e.name==='GlobalAlloc'){const p=m.allocate(arg(1));allocations.set(p,arg(1));m.ret(p,2);return;}
        if(e.name==='GlobalFree'){allocations.delete(arg(0));m.ret(0,1);return;}
        throw Error('Unimplemented oracle import '+e.dll+'!'+e.name);
    };
    // TH09 wrappers pop size + debug-label (RET 8), or pointer (RET 4).
    m.replace(0x401340,'allocate',()=>{const n=m.u32(m.reg('ESP')+4),p=m.allocate(n);allocations.set(p,n);return p;},2);
    m.replace(0x401360,'release',()=>{allocations.delete(m.u32(m.reg('ESP')+4));return 0;},1);
    return m;
}
export async function core(){
    const wasi=new WASI({version:'preview1',args:[],env:{},preopens:{}});
    const bytes=readFileSync(resolve(root,'artifacts/cpp/game-core-test.wasm'));
    loadedCoreSha256=createHash('sha256').update(bytes).digest('hex');
    const {instance}=await WebAssembly.instantiate(bytes,{wasi_snapshot_preview1:wasi.wasiImport});
    wasi.initialize(instance);return instance.exports;
}
export function floatWrapper(m,address,argc=0){
    const p=m.nextTestStub??0x400200;m.nextTestStub=p+64;const output=0x2010000;
    const code=[];for(let i=0;i<argc;++i)code.push(0xff,0x74,0x24,argc*4);
    code.push(0xe8,...new Uint8Array(new Int32Array([address-(p+code.length)-5]).buffer),0xd9,0x1d,...new Uint8Array(new Uint32Array([output]).buffer),0xa1,...new Uint8Array(new Uint32Array([output]).buffer));
    code.push(...(argc?[0xc2,argc*4,0]:[0xc3]));m.cpu.mem_write(p,code);
    return p;
}
export function memory(c,address,size){return new Uint8Array(c.memory.buffer,address,size);}
// Select the D3DX library's original scalar routines directly. The original
// dispatch initializer otherwise probes the host Windows registry/CPU. Neither
// game routines nor the library's math implementation are replaced here.
export function d3dxScalar(m){m.write(0x4a3630,m.bytes(0x4a3718,0x39*4));m.u32(0x4a3804,0);}
export function report(name,result){const dir=resolve(root,'artifacts/cpp/verification');mkdirSync(dir,{recursive:true});writeFileSync(resolve(dir,name+'.json'),JSON.stringify({target,coreSha256:loadedCoreSha256,...result},null,2)+'\n');}
