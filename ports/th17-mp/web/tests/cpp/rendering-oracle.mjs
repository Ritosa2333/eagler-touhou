import assert from 'node:assert/strict';

// The comparison loads the original vector library and captures graphics
// submissions. Neither is part of the production game or web backend.
export function nativeGraphics(m,existingManager=null){

 const manager=existingManager??m.allocate(0x1c90b00),bank=m.allocate(0x13c),sprites=m.allocate(0x44),textures=m.allocate(24),device=m.allocate(4),vtable=m.allocate(0x1c0),view=m.allocate(0x200),buffer=manager+0x1860830,plain=manager+0x1be083c,vertexBuffer=m.allocate(4),vbTable=m.allocate(0x40),vbData=m.allocate(720);let draws=[],states=new Map,samplers=new Map,matrices=new Map;
 m.u32(0x509a20,manager);m.u32(manager+0x1860100,bank);m.u32(bank+0x11c,sprites);m.u32(bank+0x124,textures);m.u32(textures,0x1234);m.u32(device,vtable);m.u32(0x4b5ae8,device);m.u32(0x4b61c8,view);m.u32(view+0xe8,640);m.u32(view+0xec,480);m.f32(view+0xf4,1);m.u32(0x524710,640);m.u32(0x524714,480);m.f32(0x524718,1);m.u32(vertexBuffer,vbTable);
 const capture=(topology,count,data,stride)=>{const n=topology===1?count:topology===2?count*2:topology===3?count+1:topology===4?count*3:count+2;draws.push({topology,count,stride,bytes:Buffer.from(m.bytes(data,n*stride)),states:new Map(states),samplers:new Map(samplers),matrices:new Map(matrices)});return 0;};
 for(const[off,argc,handler]of [
 [0xe4,3,()=>{const sp=m.reg('ESP');states.set(m.u32(sp+8),m.u32(sp+12));return 0;}],
 [0x10c,4,()=>0],[0x114,4,()=>{const sp=m.reg('ESP');samplers.set(m.u32(sp+12),m.u32(sp+16));return 0;}],[0x104,3,()=>0],[0x164,2,()=>0],[0x190,5,()=>0],
 [0xb0,3,()=>{const sp=m.reg('ESP');matrices.set(m.u32(sp+8),Buffer.from(m.bytes(m.u32(sp+12),64)));return 0;}],
 [0xbc,2,()=>0],
 [0x14c,5,()=>{const sp=m.reg('ESP');return capture(m.u32(sp+8),m.u32(sp+12),m.u32(sp+16),m.u32(sp+20));}],
 [0x144,4,()=>{const sp=m.reg('ESP');return capture(m.u32(sp+8),m.u32(sp+16),vbData+m.u32(sp+12)*20,20);}],
 [0x68,7,()=>{m.u32(m.u32(m.reg('ESP')+24),vertexBuffer);return 0;}]
 ])m.u32(vtable+off,m.registerImport({dll:'fixture',name:'capture native graphics '+off,argc,handler}));
 m.u32(vbTable+0x2c,m.registerImport({dll:'fixture',name:'original static vertex buffer lock',argc:5,handler:()=>{m.u32(m.u32(m.reg('ESP')+16),vbData);return 0;}}));m.u32(vbTable+0x30,m.registerImport({dll:'fixture',name:'original static vertex buffer unlock',argc:1,handler:()=>0}));
 m.call(0x473060);const vertexTemplates=Buffer.from(m.bytes(vbData,720));
 function reset(){draws=[];states.clear();samplers.clear();matrices.clear();m.view(manager+0x18607c4,16).fill(255);m.u32(manager+0x18607c0,0x13579bdf);m.u32(manager+0x186082c,0);m.u32(manager+0x1be0830,buffer);m.u32(manager+0x1be0834,buffer);m.u32(manager+0x1c8083c,plain);m.u32(0x4b64f4,1);m.u32(0x4b64f0,0);m.view(0x529f20,112).fill(0);for(let i=0;i<4;i++)m.f32(0x529f20+i*28+12,1);}
 reset();return{manager,bank,sprites,view,reset,draws:()=>draws,matrices:()=>matrices,vertexTemplates};
}
