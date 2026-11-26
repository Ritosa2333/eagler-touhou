const decoder=new TextDecoder('gbk');
const magic=new TextEncoder().encode('THKKDR1\0');
const hex=bytes=>[...bytes].map(n=>n.toString(16).padStart(2,'0')).join('');
const digest=async bytes=>hex(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)));
export function nativeReplay(bytes){
  if(!(bytes instanceof Uint8Array)||bytes.length<63||bytes.length>1048576+62)throw Error('海惠堂录像大小无效');
  const count=bytes.length-62,v=new DataView(bytes.buffer,bytes.byteOffset+count,62),metadata=Array.from({length:7},(_,i)=>v.getUint32(i*4,true));
  if(metadata[5]!==count)throw Error('录像输入长度不一致');
  if(metadata[0]>5||metadata[1]>2||metadata[2]>2||metadata[3]>7||metadata[6]>2)throw Error('录像的难度、机体或关卡元数据无效');
  const checksum=v.getUint32(54,true)%119+((v.getUint32(28,true)+v.getUint32(32,true))>>>0)%119;
  if(checksum!==v.getUint32(58,true))throw Error('录像校验失败');
  const characterLabel=decoder.decode(bytes.subarray(count+36,count+44)).replace(/\0.*$/,'').trim();
  const labels=['ReimuA','ReimuB','ReimuC','MarisaA','MarisaB','MarisaC','SanaeA','SanaeB','SanaeC'],index=labels.indexOf(characterLabel);
  return {native:bytes,metadata,count,motion:new Map(),description:{name:decoder.decode(bytes.subarray(count+28,count+36)).replace(/\0.*$/,'').trim(),character:index<0?metadata[6]:Math.floor(index/3),shot:index<0?metadata[2]:index%3,difficulty:metadata[0],mode:'native',stages:[{id:0,label:metadata[3]===7?'Omniverse':metadata[3]===6?'EX':'原作完整流程',score:null,frames:count}],lastStage:metadata[3]}};
}
export async function decodeReplay(bytes){
  if(!magic.every((value,i)=>bytes[i]===value))return nativeReplay(bytes);
  if(bytes.length<12)throw Error('扩展录像头部不完整');
  const length=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength).getUint32(8,true);
  if(length>48*1024*1024||length+12>=bytes.length)throw Error('扩展录像头部无效');
  const header=JSON.parse(new TextDecoder().decode(bytes.subarray(12,12+length))),native=bytes.subarray(12+length),result=nativeReplay(native);
  if(header.schema!=='thkkd/extended-replay/1'||header.nativeSha256!==await digest(native)||!Array.isArray(header.motion))throw Error('扩展录像身份无效');
  let previous=-1;
  for(const row of header.motion){if(!Array.isArray(row)||row.length!==3||!Number.isSafeInteger(row[0])||row[0]<=previous||row[0]>=result.count||!Number.isFinite(row[1])||!Number.isFinite(row[2])||row[1]<8||row[1]>376||row[2]<16||row[2]>432)throw Error('扩展录像移动记录无效');previous=row[0];result.motion.set(row[0],{x:row[1],y:row[2]});}
  result.description.mode='extended';if(header.processingDrop===100)result.description.slowdown=100;return result;
}
export async function encodeExtended(native,motion,unlimitedUsed){
  nativeReplay(native);
  const header=new TextEncoder().encode(JSON.stringify({schema:'thkkd/extended-replay/1',nativeSha256:await digest(native),motion:[...motion].sort((a,b)=>a[0]-b[0]).map(([cursor,p])=>[cursor,p.x,p.y]),...(unlimitedUsed?{processingDrop:100}:{})}));
  const result=new Uint8Array(12+header.length+native.length);result.set(magic);new DataView(result.buffer).setUint32(8,header.length,true);result.set(header,12);result.set(native,12+header.length);return result;
}
