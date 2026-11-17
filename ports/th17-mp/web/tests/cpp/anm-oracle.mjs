import assert from "node:assert/strict";import {memory} from "./helpers.mjs";
export function resourceLoader(m){const bank=m.allocate(0x13c),path=m.allocate(256),vtable=m.allocate(0x60),texture=m.allocate(16);let source,width,height;m.u32(texture,vtable);const previousImport=m.onImport;m.onImport=e=>{if(e.name==='D3DXCreateTexture'){m.u32(m.u32(m.reg('ESP')+32),texture);m.ret(0,8);}else previousImport(e);};
 m.replace(0x409440,"fixture format path",()=>0);m.replace(0x4020d0,"read ANM fixture",()=>{const p=m.allocate(source.length);m.write(p,source);if(m.reg("EDX"))m.u32(m.reg("EDX"),source.length);return p;},1);
 const desc=m.registerImport({dll:"fixture",name:"texture description",argc:3,handler:()=>{const p=m.u32(m.reg("ESP")+12);m.view(p,32).fill(0);m.u32(p+24,width);m.u32(p+28,height);return 0;}});m.u32(vtable+0x44,desc);
 for(const address of [0x474010,0x474260,0x474430,0x474430])m.replace(address,"dimension preserving texture allocation",()=>{m.u32(m.u32(m.reg("ESP")+4),texture);return 0;});m.replace(0x402670,"original diagnostic",()=>{throw Error("Original ANM diagnostic");});m.f32(0x524718,2);m.u32(0x524710,640);m.u32(0x524714,480);
 return{bank,load(name,bytes,id=0){source=bytes;m.view(bank,0x13c).fill(0);m.u32(bank,id);m.write(path,Buffer.from(name+"\0"));m.call(0x474480,{ecx:bank,args:[path]});let at=0,chunk=0,ns=0,nc=0;do{width=source.readUInt16LE(at+10);height=source.readUInt16LE(at+12);m.call(0x474a90,{args:[bank,chunk,ns,nc,m.u32(bank+0x108)+at]});ns+=source.readUInt16LE(at+4);nc+=source.readUInt16LE(at+6);chunk++;const next=source.readUInt32LE(at+36);if(!next)break;at+=next;}while(at<source.length);}};
}
const fields=[[0,0x530,8],[8,0x18,4],[16,0x4e0,12],[28,0x44,12],[40,0x50,8],[48,0x58,8],[56,0x60,8],[64,0x68,8],[72,0x78,8],[80,0x3c8,8],[88,0x80,8],[96,0x520,8],[104,0x3a8,32],[136,0x70,8],[144,0x3d0,192]];
export function compareAnm(c,m,vm,native,label){const vp=c.anm_vm_visual(vm),vars=c.anm_vm_variables(vm);for(const [co,no,n]of fields)assert.deepEqual(Buffer.from(memory(c,vp+co,n)),Buffer.from(m.bytes(native+no,n)),label+" visual "+no.toString(16));for(const [co,no,n]of [[0,0x4a0,32],[64,0x2c,12],[76,0x38,12]])assert.deepEqual(Buffer.from(memory(c,vars+co,n)),Buffer.from(m.bytes(native+no,n)),label+" variable "+no.toString(16));assert.equal(c.anm_vm_offset(vm),m.i32(native+0x28),label+" cursor");assert.deepEqual(Buffer.from(memory(c,c.anm_vm_timer(vm),20)),Buffer.from(m.bytes(native+0x540,20)),label+" timer");}
export function nativePool(m){const manager=m.allocate(0x1c90b00),sentinel=manager+0x18600ec;m.u32(0x509a20,manager);m.u32(sentinel+4,manager+0x6ec+0x600);for(let i=0;i<0x3fff;i++){const vm=manager+0x6ec+i*0x618,node=vm+0x600;m.u32(node,vm);m.u32(node+4,i+1<0x3fff?node+0x618:0);m.u32(node+8,i?node-0x618:sentinel);m.u32(vm+0x614,i);m.u32(vm+0x28,0xffffffff);}return manager;}
export function nativeInstances(m,manager,alternate){const result=[];for(let node=m.u32(manager+(alternate?0x6e4:0x6dc));node;node=m.u32(node+4))result.push(m.u32(node));return result;}
export function retireOriginalAnimations(c,m,manager,nativeManager){
 // The native recursive retirement only marks VMs; the update pass releases
 // their pool slots. Match that lifecycle rather than looping on a marked VM.
 for(let alternate=0;alternate<2;alternate++)for(const original of nativeInstances(m,nativeManager,alternate)){
  const handle=m.u32(original+0x538);assert.equal(c.anm_manager_retire(manager,handle),1);m.call(0x476680,{args:[original]});
 }
 m.f32(0x4b5918,1);
 for(let alternate=0;alternate<2;alternate++){
  assert.equal(c.anm_manager_update(manager,alternate,1),1);m.call(alternate?0x475940:0x475830,{ecx:nativeManager});
  assert.equal(c.anm_manager_count(manager,alternate),0,'Portable comparison fixture cleanup');assert.equal(nativeInstances(m,nativeManager,alternate).length,0,'Original comparison fixture cleanup');
 }
}

export function prepareBank(m,loader,name,bytes,manager,id){loader.load(name,bytes,id);const bank=m.allocate(0x13c);m.write(bank,m.bytes(loader.bank,0x13c));m.u32(bank,id);m.u32(manager+0x1860100+id*4,bank);m.u32(bank+0x128,0);m.call(0x4747f0,{ecx:manager,args:[bank]});return bank;}
