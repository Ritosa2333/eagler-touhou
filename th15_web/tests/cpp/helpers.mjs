import {readFileSync,writeFileSync,mkdirSync} from "node:fs";
import {fileURLToPath} from "node:url";
import {resolve} from "node:path";
import {createHash} from "node:crypto";
import {WASI} from "node:wasi";
import {NativeMachine} from "../../scripts/native/machine.mjs";
export const root=fileURLToPath(new URL("../../",import.meta.url));
export const target=JSON.parse(readFileSync(resolve(root,"target.json"),"utf8"));
export const sha=data=>createHash("sha256").update(data).digest("hex");
export async function core(){const wasi=new WASI({version:"preview1",args:[],env:{},preopens:{}});const {instance}=await WebAssembly.instantiate(readFileSync(resolve(root,"artifacts/cpp/game-core-test.wasm")),{wasi_snapshot_preview1:wasi.wasiImport,env:{emscripten_notify_memory_growth(){}}});wasi.initialize(instance);return instance.exports;}
export const memory=(c,p,n)=>new Uint8Array(c.memory.buffer,p,n);
export function string(c,p){if(!Number.isInteger(p)||p<0||p>=c.memory.buffer.byteLength)throw Error("Invalid Wasm string pointer "+p);const bytes=memory(c,p,Math.min(1024,c.memory.buffer.byteLength-p)),n=bytes.indexOf(0);return Buffer.from(bytes.subarray(0,n<0?bytes.length:n)).toString();}
export async function oracle(options={}){const bytes=readFileSync(resolve(root,target.executable));if(sha(bytes)!==target.sha256)throw Error("TH15 executable hash mismatch");const m=await NativeMachine.create(bytes,{...options,wasmBinary:readFileSync(resolve(root,"reference/native/unicorn-bounded.wasm"))});m.resetThreadFPU();
 m.replace(0x490432,"malloc",()=>m.allocate(m.u32(m.reg("ESP")+4)));
 for(const address of [0x4903f0,0x4904c4,0x491b88])m.replace(address,"free",()=>0);
 m.replace(0x49039f,"operator_new",()=>m.allocate(m.u32(m.reg("ESP")+4)));
 m.replace(0x492870,"memcpy",()=>{const sp=m.reg("ESP"),out=m.u32(sp+4),input=m.u32(sp+8),n=m.u32(sp+12);m.write(out,m.bytes(input,n));return out;});
 m.replace(0x49b080,"memset",()=>{const sp=m.reg("ESP"),out=m.u32(sp+4),value=m.u32(sp+8),n=m.u32(sp+12);m.view(out,n).fill(value&255);return out;});
 // The successful CRT cookie check preserves EAX. Replacing it with zero
 // would corrupt the return value of every protected original function.
 m.replace(0x490390,"security_check_cookie",()=>m.reg("EAX"));
 m.onImport=e=>{if(e.handler)m.ret(e.handler(),e.argc);else throw Error("Unimplemented original import "+e.dll+"!"+e.name);};return m;}
export function report(name,data){const dir=resolve(root,"artifacts/cpp/verification");mkdirSync(dir,{recursive:true});writeFileSync(resolve(dir,name+".json"),JSON.stringify({target,coreSha256:sha(readFileSync(resolve(root,"artifacts/cpp/game-core-test.wasm"))),...data},null,2)+"\n");}
// Unicorn's bundled TCI backend lacks PEXTRW. This test-only instruction shim
// performs the same word extraction via MOVQ/MOVZX, preserving flags and XMM0.
// It does not replace the original CRT math function or its numeric operations.
export function enableNativeSine(m){const address=0x4bacd1,expected=Buffer.from([0x66,0x0f,0xc5,0xc0,3]);if(!Buffer.from(m.bytes(address,5)).equals(expected))throw Error('Unexpected native sine extraction opcode');const scratch=m.allocate(16),cave=m.allocate(32),code=Buffer.from([0x66,0x0f,0xd6,0x05,0,0,0,0,0x0f,0xb7,0x05,0,0,0,0,0xe9,0,0,0,0]);code.writeUInt32LE(scratch,4);code.writeUInt32LE(scratch+6,11);code.writeInt32LE(address+5-cave-code.length,16);m.write(cave,code);const jump=Buffer.alloc(5);jump[0]=0xe9;jump.writeInt32LE(cave-address-5,1);m.write(address,jump);}
export function enableNativeMath(m){
 const shims=JSON.parse(readFileSync(resolve(root,'reference/native/math-instruction-shims.json'),'utf8'));
 for(const {address,instruction,bytes}of shims){const original=Buffer.from(m.bytes(address,5));if(original.toString('hex')!==bytes)throw Error('Unexpected native SSE word opcode');const register=(original[3]>>3)&7,other=original[3]&7,word=original[4],scratch=m.allocate(16),cave=m.allocate(48);let code;
 if(instruction==='PEXTRW'){code=Buffer.from([0x66,0x0f,0xd6,0x05|(other<<3),0,0,0,0,0x0f,0xb7,0x05|(register<<3),0,0,0,0,0xe9,0,0,0,0]);code.writeUInt32LE(scratch,4);code.writeUInt32LE(scratch+word*2,11);}
 else{code=Buffer.from([0xf3,0x0f,0x7f,0x05|(register<<3),0,0,0,0,0x66,0x89,0x05|(other<<3),0,0,0,0,0xf3,0x0f,0x6f,0x05|(register<<3),0,0,0,0,0xe9,0,0,0,0]);code.writeUInt32LE(scratch,4);code.writeUInt32LE(scratch+word*2,11);code.writeUInt32LE(scratch,19);}
 code.writeInt32LE(address+5-cave-code.length,code.length-4);m.write(cave,code);const jump=Buffer.alloc(5);jump[0]=0xe9;jump.writeInt32LE(cave-address-5,1);m.write(address,jump);
 }
}
export function enableOriginalVectorMath(m){
 const bytes=readFileSync(resolve(root,'reference/native/d3dx9_43.dll'));
 if(sha(bytes)!=='0b28546be22c71834501f7d7185ede5d79742457331c7ee09efc14490dd64f5f')throw Error('Unexpected original vector math library');
 const library=m.loadLibrary(bytes,0x01000000);
 for(const entry of library.image.imports){const address=m.u32(library.base+entry.address-library.image.base),stub=m.importMap.get(address);if(entry.dll==='msvcrt.dll'&&(entry.name==='??2@YAPAXI@Z'||entry.name==='malloc'))stub.handler=()=>m.allocate(m.u32(m.reg('ESP')+4));if(entry.dll==='msvcrt.dll'&&(entry.name==='??3@YAXPAX@Z'||entry.name==='free'))stub.handler=()=>0;if(entry.dll==='advapi32.dll'&&entry.name==='RegOpenKeyA'){stub.handler=()=>2;stub.argc=3;}}
 for(const entry of m.image.imports)if(entry.dll==='d3dx9_43.dll'&&entry.name==='D3DXVec2Normalize'){const address=library.exports.get(entry.name);if(!address)throw Error('Original vector normalize export missing');m.u32(entry.address,address);}
 for(const entry of library.image.imports){const stub=m.importMap.get(m.u32(library.base+entry.address-library.image.base));if(entry.dll==='msvcrt.dll'&&entry.name==='memset')stub.handler=()=>{const sp=m.reg('ESP'),p=m.u32(sp+4);m.view(p,m.u32(sp+12)).fill(m.u32(sp+8)&255);return p;};if(entry.dll==='kernel32.dll'&&entry.name==='GetVersionExA'){stub.handler=()=>{const p=m.u32(m.reg('ESP')+4);m.u32(p+4,10);m.u32(p+8,0);m.u32(p+12,19045);m.u32(p+16,2);return 1;};stub.argc=1;}if(entry.dll==='kernel32.dll'&&entry.name==='IsProcessorFeaturePresent'){stub.handler=()=>[3,6,10].includes(m.u32(m.reg('ESP')+4))?1:0;stub.argc=1;}}
 return library;
}
