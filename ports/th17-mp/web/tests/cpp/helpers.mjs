import {readFileSync,writeFileSync,mkdirSync,readdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';
import {NativeMachine} from '../../scripts/native/machine.mjs';
export const root=fileURLToPath(new URL('../../',import.meta.url));
export const target=JSON.parse(readFileSync(resolve(root,'target.json'),'utf8'));
export const sha=b=>createHash('sha256').update(b).digest('hex');
export const memory=(c,p,n)=>new Uint8Array(c.memory.buffer,p,n);
export function string(c,p){const b=memory(c,p,Math.min(1024,c.memory.buffer.byteLength-p)),n=b.indexOf(0);return Buffer.from(b.subarray(0,n<0?b.length:n)).toString();}
const loadedCoreHashes=new WeakMap();
export const coreHash=c=>loadedCoreHashes.get(c);
export async function core(){const wasi=new WASI({version:'preview1',args:[],env:{},preopens:{}}),bytes=readFileSync(resolve(root,'artifacts/cpp/game-core-test.wasm'));const {instance}=await WebAssembly.instantiate(bytes,{wasi_snapshot_preview1:wasi.wasiImport});wasi.initialize(instance);loadedCoreHashes.set(instance.exports,sha(bytes));return instance.exports;}
export async function oracle(options={}){
 const original=readFileSync(resolve(root,target.executable));if(sha(original)!==target.sha256)throw Error('TH17 executable identity changed');
 const m=await NativeMachine.create(original,{...options,wasmBinary:readFileSync(resolve(root,'reference/native/unicorn-bounded.wasm'))});m.resetThreadFPU();
 // Host allocation/CRT memory functions only. Game logic stays in the original image.
 for(const at of [0x47ef94,0x47b250,0x48a9bb])m.replace(at,'allocate',()=>m.allocate(m.u32(m.reg('ESP')+4)));
 for(const at of [0x47ef9f,0x47b280,0x48aa09])m.replace(at,'free',()=>0);
 m.replace(0x47ea20,'memcpy',()=>{const sp=m.reg('ESP'),out=m.u32(sp+4),n=m.u32(sp+12);m.write(out,m.bytes(m.u32(sp+8),n));return out;});
 m.replace(0x47ce60,'memset',()=>{const sp=m.reg('ESP'),out=m.u32(sp+4);m.view(out,m.u32(sp+12)).fill(m.u32(sp+8)&255);return out;});
 m.replace(0x47b23f,'security_check_cookie',()=>m.reg('EAX'));
 m.onImport=e=>{if(e.handler)m.ret(e.handler(),e.argc);else throw Error('Unimplemented original import '+e.dll+'!'+e.name);};
 return m;
}
export function report(name,data){const dir=resolve(root,'artifacts/cpp/verification');mkdirSync(dir,{recursive:true});writeFileSync(resolve(dir,name+'.json'),JSON.stringify({date:new Intl.DateTimeFormat('en-CA',{timeZone:'Asia/Shanghai'}).format(new Date()),target,coreSha256:sha(readFileSync(resolve(root,'artifacts/cpp/game-core-test.wasm'))),...data},null,2)+'\n');}
// Unicorn's TCI backend omits these two SSE word opcodes. Only the test oracle
// expands them into equivalent memory moves; original numeric functions run.
export function enableNativeMath(m){
 const dir=resolve(root,'artifacts/cpp/analysis/jp/functions'),seen=new Set();
 for(const file of readdirSync(dir).filter(n=>n.endsWith('.asm')))for(const line of readFileSync(resolve(dir,file),'utf8').split(/\r?\n/)){
  const hit=/^([0-9a-f]+) (PEXTRW|PINSRW) (?:XMM\d|E[A-Z]{2}),(?:XMM\d|E[A-Z]{2}),0x[0-9a-f]+$/i.exec(line);if(!hit)continue;
  const address=parseInt(hit[1],16);if(seen.has(address))continue;seen.add(address);
  const original=Buffer.from(m.bytes(address,5));if(original[0]!==0x66||original[1]!==15||![0xc4,0xc5].includes(original[2])||(original[3]&0xc0)!==0xc0)throw Error('Unexpected native SSE word encoding '+line);
  const register=(original[3]>>3)&7,other=original[3]&7,word=original[4],scratch=m.allocate(16),cave=m.allocate(48);let code;
  if(hit[2]==='PEXTRW'){code=Buffer.from([0x66,0x0f,0xd6,0x05|(other<<3),0,0,0,0,0x0f,0xb7,0x05|(register<<3),0,0,0,0,0xe9,0,0,0,0]);code.writeUInt32LE(scratch,4);code.writeUInt32LE(scratch+word*2,11);}
  else{code=Buffer.from([0xf3,0x0f,0x7f,0x05|(register<<3),0,0,0,0,0x66,0x89,0x05|(other<<3),0,0,0,0,0xf3,0x0f,0x6f,0x05|(register<<3),0,0,0,0,0xe9,0,0,0,0]);code.writeUInt32LE(scratch,4);code.writeUInt32LE(scratch+word*2,11);code.writeUInt32LE(scratch,19);}
  code.writeInt32LE(address+5-cave-code.length,code.length-4);m.write(cave,code);const jump=Buffer.alloc(5);jump[0]=0xe9;jump.writeInt32LE(cave-address-5,1);m.write(address,jump);
 }
 return seen.size;
}
// Test oracle only: execute the original vector/pixel library in bounded guest
// memory. Production C++ uses typed matrices and the shared SDL3 renderer.
export function enableOriginalVectorMath(m){
 const bytes=readFileSync(resolve(root,'reference/native/d3dx9_43.dll'));
 if(sha(bytes)!=='0b28546be22c71834501f7d7185ede5d79742457331c7ee09efc14490dd64f5f')throw Error('Unexpected original vector math library');
 const library=m.loadLibrary(bytes,0x01000000);
 for(const entry of library.image.imports){
  const stub=m.importMap.get(m.u32(library.base+entry.address-library.image.base)),arg=n=>m.u32(m.reg('ESP')+n*4);
  if(entry.dll==='msvcrt.dll'&&['??2@YAPAXI@Z','malloc'].includes(entry.name))stub.handler=()=>m.allocate(arg(1));
  if(entry.dll==='msvcrt.dll'&&['??3@YAXPAX@Z','free'].includes(entry.name))stub.handler=()=>0;
  if(entry.dll==='msvcrt.dll'&&entry.name==='memset')stub.handler=()=>{const p=arg(1);m.view(p,arg(3)).fill(arg(2)&255);return p;};
  if(entry.dll==='advapi32.dll'&&entry.name==='RegOpenKeyA'){stub.handler=()=>2;stub.argc=3;}
  if(entry.dll==='kernel32.dll'&&entry.name==='GetVersionExA'){stub.handler=()=>{const p=arg(1);m.u32(p+4,10);m.u32(p+8,0);m.u32(p+12,19045);m.u32(p+16,2);return 1;};stub.argc=1;}
  if(entry.dll==='kernel32.dll'&&entry.name==='IsProcessorFeaturePresent'){stub.handler=()=>[3,6,10].includes(arg(1))?1:0;stub.argc=1;}
 }
 for(const entry of m.image.imports)if(entry.dll==='d3dx9_43.dll'&&/^D3DX(?:Matrix|Vec\d)/.test(entry.name)){const address=library.exports.get(entry.name);if(!address)throw Error('Original vector export missing: '+entry.name);m.u32(entry.address,address);}
 return library;
}
