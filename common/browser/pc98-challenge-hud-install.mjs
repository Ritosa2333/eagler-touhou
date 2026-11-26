// Install a game-native signed-number routine in the private cooperative MAIN.
// The twelve leading bytes are three signed 32-bit counters in the guest CS.
const code04='000000000000000000000000e8f6d7666006b800a08ec031ed69fd400181c7ea04b90b00b8200551e87ad759e2f983ef16e80f00452e0fb606a29539c572da076661c3665066536651665289ebc1e3022e668b8720b96685c0790d66f7d86650b82de1e83fd7665831c966bb0a0000006631d266f7f380c23052416685c075f058b4e151e81ed759e2f6665a6659665b6658c3';
const fromHex=value=>Uint8Array.from(value.match(/../g),b=>parseInt(b,16));
function check(bytes,at,expected){return at>=0&&at+expected.length<=bytes.length&&expected.every((value,i)=>bytes[at+i]===value);}
function insertImage(source,at,gap,code){
  const v=new DataView(source.buffer,source.byteOffset,source.byteLength),hs=v.getUint16(8,true)*16,table=v.getUint16(24,true),count=v.getUint16(6,true);
  if(at%16||gap%16||code.length>gap||at>source.length-hs)throw Error('挑战模式程序扩展无效');
  const image=source.slice(hs),iv=new DataView(image.buffer),rel=[];
  for(let i=0;i<count;i++){
    const pos=table+i*4,loc=v.getUint16(pos,true)+v.getUint16(pos+2,true)*16;
    if(loc+2>image.length)throw Error('挑战模式程序重定位无效');
    const segment=iv.getUint16(loc,true);if(segment*16>=at)iv.setUint16(loc,segment+gap/16,true);
    rel.push(loc>=at?loc+gap:loc);
  }
  const expanded=new Uint8Array(image.length+gap);expanded.set(image.subarray(0,at));expanded.set(code,at);expanded.set(image.subarray(at),at+gap);
  const header=source.slice(0,hs),hv=new DataView(header.buffer);
  for(const field of [14,22]){const segment=hv.getUint16(field,true);if(segment*16>=at)hv.setUint16(field,segment+gap/16,true);}
  rel.forEach((loc,i)=>{hv.setUint16(table+i*4,loc%16,true);hv.setUint16(table+i*4+2,Math.floor(loc/16),true);});
  const result=new Uint8Array(hs+expanded.length);hv.setUint16(2,result.length%512,true);hv.setUint16(4,Math.ceil(result.length/512),true);result.set(header);result.set(expanded,hs);return result;
}
export function installChallengeHud(source,game,patch){
  if(!(source instanceof Uint8Array)||source.length<32)throw Error('挑战模式程序无效');
  const hs=new DataView(source.buffer,source.byteOffset,source.byteLength).getUint16(8,true)*16;
  if(game!=='04')throw Error('挑战模式作品无效');
  const cs=patch.code_segment,offset=0xb920,call=0x8fe3,base=hs+cs*16,insert=patch.insert_image_offset+patch.insert_size;
  const guard=fromHex(patch.near_code_guard.original),code=fromHex(code04),gap=160;
  if(insert!==cs*16+offset||!check(source,base+call,fromHex('e83f01'))||!check(source,hs+insert,guard))throw Error('TH04 挑战模式 HUD 原程序不匹配');
  const result=insertImage(source,insert,gap,code),view=new DataView(result.buffer);
  if(!check(result,base+0x8ce0,fromHex('e81200')))throw Error('TH04 挑战模式奖励补丁不匹配');
  result.fill(0x90,base+0x8ce0,base+0x8ce3);
  result[base+call]=0xe8;view.setUint16(base+call+1,(offset+12-call-3)&65535,true);
  return {bytes:result,countsOffset:offset,addedParagraphs:gap/16};
}
