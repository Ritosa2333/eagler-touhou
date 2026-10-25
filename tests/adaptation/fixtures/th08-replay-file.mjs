// Synthetic format fixture | literal payloads, no game assets or original captures.
function packBits(bits){
  const bytes=[];
  for(let start=0;start<bits.length;start+=8){let byte=0;for(let i=0;i<8;i++)byte=(byte<<1)|(bits[start+i]??0);bytes.push(byte);}
  return Buffer.from(bytes);
}
function encodeLiteralPayload(payload){
  const bits=[];for(const byte of payload){bits.push(1);for(let shift=7;shift>=0;shift--)bits.push((byte>>shift)&1);}
  bits.push(0,...Array(13).fill(0));return packBits(bits);
}

export function replayFile({debugRecords=false,inputFrames=[0,1,128,0,4],timingBytes=2,lag=0,practice=false}={}){
  const stride=debugRecords?6:2,inputBytes=inputFrames.length*stride,stageOffset=0x134;
  const timingOffset=stageOffset+0x24+inputBytes,payloadEnd=Math.max(0x134+0xcc,timingOffset+timingBytes+1);
  const payload=Buffer.alloc(payloadEnd-0x68),putU32=(buffer,offset,value)=>buffer.writeUInt32LE(value>>>0,offset);
  payload.writeUInt8(practice?1:0,0x7b-0x68);payload.writeFloatLE(lag,0x118-0x68);
  for(let index=0;index<inputFrames.length;index++)payload.writeUInt16LE(inputFrames[index],stageOffset-0x68+0x24+index*stride);
  payload.fill(0x1d,timingOffset-0x68,timingOffset-0x68+timingBytes);
  const compressed=encodeLiteralPayload(payload),plain=Buffer.alloc(0x68+compressed.length);
  putU32(plain,0,0x50523854);plain.writeUInt16LE(6,4);plain[6]=debugRecords?1:0;
  putU32(plain,12,plain.length);plain[21]=37;putU32(plain,24,compressed.length);putU32(plain,28,payload.length);
  putU32(plain,32,stageOffset);putU32(plain,68,timingOffset);putU32(plain,72,timingOffset+timingBytes);
  compressed.copy(plain,0x68);let checksum=0x3f000318;
  for(let i=21;i<plain.length;i++)checksum=(checksum+plain[i])>>>0;putU32(plain,16,checksum);
  const encoded=Buffer.from(plain);let key=encoded[21];
  for(let i=24;i<encoded.length;i++){encoded[i]=(encoded[i]+key)&0xff;key=(key+7)&0xff;}
  return encoded;
}

export function withPointerMotion(replay,trajectory,{stage=0}={}){
  const payload=Buffer.alloc(72+trajectory.length*20);payload.writeUInt32LE(trajectory.length,36+stage*4);
  for(const [i,event] of trajectory.entries()){
    const offset=72+i*20;payload.writeUInt32LE(i+1,offset);payload.writeInt32LE(event.id,offset+4);
    payload.writeFloatLE(event.x,offset+8);payload.writeFloatLE(event.y,offset+12);payload.writeUInt32LE({down:0,move:1,up:2}[event.action],offset+16);
  }
  let hash=2166136261;for(const byte of payload)hash=Math.imul(hash^byte,16777619)>>>0;
  const footer=Buffer.alloc(24);footer.write('THMOTION');footer.writeUInt32LE(8,8);footer.writeUInt32LE(2,12);footer.writeUInt32LE(payload.length,16);footer.writeUInt32LE(hash,20);
  return Buffer.concat([replay,payload,footer]);
}
