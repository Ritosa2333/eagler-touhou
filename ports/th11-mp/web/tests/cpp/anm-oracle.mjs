import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {root} from './helpers.mjs';
// Real original loader/interpreter/manager. Only host allocation and GPU resource
// creation are substituted; original sprite tables and animation code execute.
export function animationOracle(m){
 let source,chunk;
 m.replace(0x46a720,'memset',()=>{const sp=m.reg('ESP'),p=m.u32(sp+4);m.view(p,m.u32(sp+12)).fill(m.u32(sp+8)&255);return p;});
 m.replace(0x45fce4,'operator new',()=>m.allocate(m.u32(m.reg('ESP')+4)));
 m.replace(0x45fd49,'operator delete',()=>0);
 m.replace(0x456ad0,'diagnostic log',()=>0);
 m.replace(0x46025c,'sprintf filename',()=>{const sp=m.reg('ESP'),p=m.u32(sp+4),text=m.string(m.u32(sp+12));m.write(p,Buffer.from(text+'\0'));return text.length;});
 m.replace(0x458400,'read fixture file',()=>{const size=m.u32(m.reg('ESP')+4),p=m.allocate(source.length);m.write(p,source);if(size)m.u32(size,source.length);return p;},2);
 const vtable=m.allocate(0x60),texture=m.allocate(16);m.u32(texture,vtable);
 m.u32(vtable+8,m.registerImport({dll:'fixture',name:'texture release',argc:1,handler:()=>0}));
 const desc=m.registerImport({dll:'fixture',name:'texture dimensions',argc:3,handler:()=>{const out=m.u32(m.reg('ESP')+12);m.view(out,32).fill(0);m.u32(out+24,source.readUInt16LE(chunk+10));m.u32(out+28,source.readUInt16LE(chunk+12));return 0;}});m.u32(vtable+0x44,desc);
 m.replace(0x453f80,'embedded texture upload',()=>{m.u32(m.reg('EDI'),texture);return 0;},2);
 m.replace(0x4540a0,'blank texture allocation',()=>{m.u32(m.reg('ESI'),texture);return 0;},1);
 m.replace(0x4540f0,'render target allocation',()=>{m.u32(m.reg('ESI'),texture);return 0;});
 const manager=m.allocate(0x7bd900),path=m.allocate(256),template=m.allocate(0x434),handle=m.allocate(4);m.view(manager,0x7bd900).fill(0);m.u32(0x4c3268,manager);m.view(template,0x434).fill(0);m.reg('ESI',template);m.call(0x401fd0);const initialized=Buffer.from(m.bytes(template,0x434));
 m.replace(0x4569c0,'initialized animation allocation',()=>{const p=m.allocate(0x434);m.write(p,initialized);m.u32(p+4,p);m.u32(p+16,p);return p;});
 const heap=m.heap;
 const states=()=>{const result=[];for(const offset of [0x7b562c,0x7b5634]){let node=m.u32(manager+offset),budget=20000;while(node){if(!--budget)throw Error('animation list cycle');const p=m.u32(node);result.push({id:m.u32(p),p,bytes:Buffer.from(m.bytes(p,0x434))});node=m.u32(node+4);}}return result;};
 return {
  manager,states,
  reset(){for(const off of [0x7b562c,0x7b5630,0x7b5634,0x7b5638,0x7bd888])m.u32(manager+off,0);m.f32(0x4a7948,1);for(const r of [0x4c2ef8,0x4c2f00]){m.view(r,8).fill(0);m.u32(r,12345);}},
  load(name,append=false){if(!append)m.heap=heap;source=readFileSync(resolve(root,'reference/assets',name));m.write(path,Buffer.from(name+'\0'));const original=m.call(0x454190,{ecx:path,args:[manager,0]});let sprite=0,script=0,index=0;chunk=0;while(true){m.call(0x4545a0,{args:[original,index++,sprite,script,m.u32(original+0x108)+chunk]});sprite+=source.readUInt16LE(chunk+4);script+=source.readUInt16LE(chunk+6);const next=source.readUInt32LE(chunk+36);if(!next)break;chunk+=next;}return {original,source,heap:m.heap};},
  spawn(resource,script,layer,mode=0){m.reg('ESI',handle);m.call([0x455a00,0x455df0,0x455bc0,0x456020][mode],{args:[resource,script,layer]});return m.u32(handle);},
  update(overlay){m.reg('EAX',manager);m.call(overlay?0x455560:0x455480,{limit:50000000});},
 };
}
