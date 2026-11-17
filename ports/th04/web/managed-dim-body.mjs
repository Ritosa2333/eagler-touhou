// Add the sprite tagging routine after the cooperative code, before game data.
const hooks=[
  '6660a14e46c1f80483c0102ea3329aa150462d800050e8e7572ea3349ab80100833e56460074087c05b80300eb01402ea32e9a2e0fb60698932ea3309a6661e8aba72e66c7062e9a00000000c300000053e804df5b84c0750e502ec7062e9a0d002e891e309a58c30000000000000000e822d26a072eff1ef18cc300000000006660e8d4d1bea59531d22e0fb60ea2952e807c0602740b2e807c046473042efe44044283c610e2e8e8ead16661c3',
  '6660a14e46c1f80483c0102ea3329aa150462d800050e847572ea3349ab80100833e56460074087c05b80300eb01402ea32e9a2e0fb60698932ea3309a6661e80ba72e66c7062e9a00000000c300000053e864de5b84c0750e502ec7062e9a0d002e891e309a58c30000000000000000e882d16a072eff1ef18cc300000000006660e834d1bea59531d22e0fb60ea2952e807c0602740b2e807c046473042efe44044283c610e2e8e84ad16661c3'
].map(s=>Uint8Array.from(s.match(/../g),v=>parseInt(v,16)));

export function installDimBody(source,patch,challenge){
  const v=new DataView(source.buffer,source.byteOffset,source.byteLength);
  const headerBytes=v.getUint16(8,true)*16,cs=patch.code_segment,offset=0xb920+(challenge?160:0);
  const at=cs*16+offset,gap=224,table=v.getUint16(24,true),count=v.getUint16(6,true);
  const image=source.slice(headerBytes),iv=new DataView(image.buffer),rel=[];
  for(let i=0;i<count;i++){
    const pos=table+i*4,loc=v.getUint16(pos,true)+v.getUint16(pos+2,true)*16;
    const segment=iv.getUint16(loc,true);if(segment*16>=at)iv.setUint16(loc,segment+gap/16,true);
    rel.push(loc>=at?loc+gap:loc);
  }
  const result=new Uint8Array(source.length+gap),header=source.slice(0,headerBytes),hv=new DataView(header.buffer);
  for(const field of [14,22]){const segment=hv.getUint16(field,true);if(segment*16>=at)hv.setUint16(field,segment+gap/16,true);}
  rel.forEach((loc,i)=>{hv.setUint16(table+i*4,loc%16,true);hv.setUint16(table+i*4+2,Math.floor(loc/16),true);});
  hv.setUint16(2,result.length%512,true);hv.setUint16(4,Math.ceil(result.length/512),true);
  result.set(header);result.set(image.subarray(0,at),headerBytes);result.set(hooks[challenge?1:0],headerBytes+at);result.set(image.subarray(at),headerBytes+at+gap);
  // A nearby living teammate must not interrupt rescue of a ghost.
  const priority=Uint8Array.from('2e807c060075066605000000012e663b06869ec3'.match(/../g),v=>parseInt(v,16));
  result.set(priority,headerBytes+cs*16+offset+192);
  const rescue=headerBytes+cs*16+0xa004;
  result[rescue]=0xe8;new DataView(result.buffer).setUint16(rescue+1,(offset+192-0xa007)&65535,true);result.fill(0x90,rescue+3,rescue+6);
  const call=headerBytes+cs*16+0x9bd4;
  if(result[call]!==0xe8||result[call+1]!==0x36||result[call+2]!==0xc5)throw Error('TH04 人物绘制入口不匹配');
  new DataView(result.buffer).setUint16(call+1,(offset-0x9bd7)&65535,true);
  for(const location of [0x998e,0x99f3,0x9a51])new DataView(result.buffer).setUint16(headerBytes+cs*16+location+1,(offset+80-location-3)&65535,true);
  new DataView(result.buffer).setUint16(headerBytes+cs*16+0xa0bf+1,(offset+112-0xa0c2)&65535,true);
  new DataView(result.buffer).setUint16(headerBytes+cs*16+0xb2f4+1,(offset+128-0xb2f7)&65535,true);
  return result;
}
