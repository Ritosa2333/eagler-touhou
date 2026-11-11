// Add a native Extend call at the successful life-transfer branch.
const code=Uint8Array.from('2ec6869c5e016a082eff1e9c8083c402c3'.match(/../g),x=>parseInt(x,16));
export function installRescueSound(source,patch,overlay){
  const v=new DataView(source.buffer,source.byteOffset,source.byteLength),hs=v.getUint16(8,true)*16;
  const at=overlay.insert+overlay.bytes,gap=32,table=v.getUint16(24,true),count=v.getUint16(6,true);
  const image=source.slice(hs),iv=new DataView(image.buffer),rel=[];
  for(let i=0;i<count;i++){
    const pos=table+i*4,loc=v.getUint16(pos,true)+v.getUint16(pos+2,true)*16,segment=iv.getUint16(loc,true);
    if(segment*16>=at)iv.setUint16(loc,segment+gap/16,true);
    rel.push(loc>=at?loc+gap:loc);
  }
  const header=source.slice(0,hs),hv=new DataView(header.buffer),result=new Uint8Array(source.length+gap);
  for(const field of [14,22]){const segment=hv.getUint16(field,true);if(segment*16>=at)hv.setUint16(field,segment+gap/16,true);}
  rel.forEach((loc,i)=>{hv.setUint16(table+i*4,loc%16,true);hv.setUint16(table+i*4+2,Math.floor(loc/16),true);});
  hv.setUint16(2,result.length%512,true);hv.setUint16(4,Math.ceil(result.length/512),true);
  result.set(header);result.set(image.subarray(0,at),hs);result.set(code,hs+at);result.set(image.subarray(at),hs+at+gap);
  const call=hs+patch.codeSegment*16+0x6108,expected=[0x2e,0xc6,0x86,0x9c,0x5e,0x01];
  if(!expected.every((b,i)=>result[call+i]===b))throw Error('TH02 借命入口不匹配');
  result[call]=0xe8;new DataView(result.buffer).setUint16(call+1,(at-patch.codeSegment*16-0x610b)&65535,true);result.fill(0x90,call+3,call+6);
  // The score award stores a reserve for ghosts; a later rescue keeps it.
  const cs=hs+patch.codeSegment*16;
  result.set([0xeb,0x1d],cs+0x8067);
  result.fill(0x90,cs+0x60db,cs+0x60e0);
  result.fill(0x90,cs+0x7f40,cs+0x7f45);
  return result;
}
