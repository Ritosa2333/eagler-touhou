// Install a version-pinned native pause bridge in the private per-room MZ.
// Existing edits and launcher configuration are retained byte for byte.
export function installNetPause(exe, spec) {
  const fromHex=s=>Uint8Array.from(s.match(/../g)||[],v=>parseInt(v,16));
  const source=Uint8Array.from(exe),v=new DataView(source.buffer,source.byteOffset,source.byteLength);
  if(v.getUint16(0,true)!==0x5a4d)throw Error('联机暂停程序格式不匹配');
  if(Number.isInteger(spec.replace)){
    const hs=v.getUint16(8,true)*16,table=v.getUint16(24,true),count=v.getUint16(6,true),before=fromHex(spec.before),code=fromHex(spec.code),start=spec.replace;
    if(code.length>before.length||start<0||hs+start+before.length>source.length||!before.every((b,i)=>source[hs+start+i]===b))throw Error('联机暂停补丁版本不匹配');
    const removed=new Set(spec.removeRelocations),kept=[];
    for(let i=0;i<count;i++){const p=table+i*4,loc=v.getUint16(p,true)+v.getUint16(p+2,true)*16;if(removed.has(loc))removed.delete(loc);else kept.push(loc);}
    if(removed.size)throw Error('联机暂停重定位不匹配');
    source.fill(0x90,hs+start,hs+start+before.length);source.set(code,hs+start);
    v.setUint16(6,kept.length,true);kept.forEach((loc,i)=>{v.setUint16(table+i*4,loc%16,true);v.setUint16(table+i*4+2,Math.floor(loc/16),true);});
    return source;
  }
  const hs=v.getUint16(8,true)*16,table=v.getUint16(24,true),count=v.getUint16(6,true),at=spec.insert,gap=spec.bytes;
  const code=fromHex(spec.code),expected=fromHex(spec.before);
  if(!Number.isInteger(at)||at%16||gap%16||code.length>gap||at>source.length-hs||spec.hook+3>at||
     !expected.every((b,i)=>source[hs+spec.hook+i]===b))throw Error('联机暂停补丁版本不匹配');
  const image=source.slice(hs),iv=new DataView(image.buffer,image.byteOffset,image.byteLength),rel=[];
  for(let i=0;i<count;i++){
    const pos=table+i*4,loc=v.getUint16(pos,true)+v.getUint16(pos+2,true)*16;
    if(loc+2>image.length)throw Error('联机程序重定位无效');
    const segment=iv.getUint16(loc,true);if(segment*16>=at)iv.setUint16(loc,segment+gap/16,true);
    rel.push(loc>=at?loc+gap:loc);
  }
  const expanded=new Uint8Array(image.length+gap);expanded.set(image.subarray(0,at));expanded.set(code,at);expanded.set(image.subarray(at),at+gap);
  const ov=new DataView(expanded.buffer);expanded[spec.hook]=0xe8;ov.setUint16(spec.hook+1,(at-spec.hook-3)&65535,true);
  for(const loc of spec.extraRelocations)rel.push(loc);
  if(new Set(rel).size!==rel.length)throw Error('联机暂停重定位重复');
  const headerSize=Math.max(hs,(table+rel.length*4+15)&~15),header=new Uint8Array(headerSize);header.set(source.subarray(0,hs));const hv=new DataView(header.buffer);
  for(const field of [14,22]){const segment=hv.getUint16(field,true);if(segment*16>=at)hv.setUint16(field,segment+gap/16,true);}
  hv.setUint16(8,headerSize/16,true);hv.setUint16(6,rel.length,true);
  rel.forEach((loc,i)=>{hv.setUint16(table+i*4,loc%16,true);hv.setUint16(table+i*4+2,Math.floor(loc/16),true);});
  const total=headerSize+expanded.length;hv.setUint16(2,total%512,true);hv.setUint16(4,Math.ceil(total/512),true);
  const result=new Uint8Array(total);result.set(header);result.set(expanded,headerSize);return result;
}
