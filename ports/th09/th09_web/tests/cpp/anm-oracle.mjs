// Resource/sprite setup shared by integration oracles. Rendering/texture uploads
// are absent; the original ANM executor and sprite calculations still run.
export function anmMetadata(bytes){
    const v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),scripts=[],sprites=[];let base=0;
    for(;;){const ns=v.getUint32(base,true),nt=v.getUint32(base+4,true),width=v.getUint32(base+12,true),height=v.getUint32(base+16,true);
        for(let i=0;i<ns;i++){const p=base+v.getUint32(base+64+i*4,true);sprites.push({x:v.getFloat32(p+4,true),y:v.getFloat32(p+8,true),w:v.getFloat32(p+12,true),h:v.getFloat32(p+16,true),width,height});}
        for(let i=0;i<nt;i++)scripts.push(base+v.getUint32(base+64+ns*4+i*8+4,true));const next=v.getUint32(base+56,true);if(!next)break;base+=next;
    }return {scripts,sprites};
}
export function installAnm(m,bytes,index=8){
    const meta=anmMetadata(bytes),manager=m.allocate(64),file=m.allocate(28),source=m.allocate(bytes.length),sprites=m.allocate(meta.sprites.length*68),table=m.allocate(meta.scripts.length*4),entry=m.allocate(68);
    // Oracle allocations can reuse a prior scenario's heap. Native resource
    // objects are constructed from zeroed storage before their fields load.
    m.view(manager,64).fill(0);m.view(file,28).fill(0);m.view(sprites,meta.sprites.length*68).fill(0);
    m.u32(0x4dc550,manager);m.u32(file,index);m.u32(file+4,source);m.u32(file+12,sprites);m.u32(file+16,table);m.write(source,bytes);
    meta.scripts.forEach((s,i)=>m.u32(table+i*4,source+s));
    meta.sprites.forEach((s,i)=>{const data=Buffer.alloc(68);data.writeInt32LE(index);
        for(const [offset,value] of [[8,s.x],[12,s.y],[16,Math.fround(s.x+s.w)],[20,Math.fround(s.y+s.h)],[24,s.height],[28,s.width],[56,1],[60,1]])data.writeFloatLE(value,offset);
        m.write(entry,data);m.call(0x43bcb0,{ecx:file,args:[i,entry]});
    });
    return {manager,file,source,sprites,table,meta};
}
export function normalizedAnm(bytes,file,source,sprites){
    const copy=bytes.slice(),v=new DataView(copy.buffer,copy.byteOffset,copy.byteLength);
    for(const [offset,base] of [[0x204,file],[0x21c,source],[0x220,source],[0x224,sprites],[0x234,source]]){const p=v.getUint32(offset,true);v.setUint32(offset,p?p-base+1:0,true);}
    return copy;
}
