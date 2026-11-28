import {originalType33Profile} from './original-bullet-type33-record.mjs';
export {originalType33Profile};
const profileHash='b010429a9a63e063aa0889abcbcf472cd38743ea71459410cffa06e8c88ef4ea';
const originalExeHash='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4';
const fields=Object.freeze(Object.keys(originalType33Profile).sort());
function cancelled(){return new DOMException('Typed bullet profile owner was replaced','AbortError');}
export async function originalType33Bytes(record=originalType33Profile){
 if(!record||typeof record!=='object'||Array.isArray(record)
    ||Reflect.ownKeys(record).length!==fields.length||fields.some(key=>!Object.hasOwn(record,key))
    ||record.schema!=='th128-original-typed-bullet-profile/1'||record.type!==33
    ||record.sourceExeSha256!==originalExeHash||record.sourceAddress!=='0x004b49f0'
    ||record.bytes!==288||record.sha256!==profileHash||typeof record.hex!=='string'||!/^[0-9a-f]{576}$/.test(record.hex)
    ||record.script!==55||record.secondaryScript!==0||record.hitboxSize!==4||record.auxiliarySize!==16
    ||record.appearance!==3||record.selector!==6||record.cacheMode!==6||record.freezePolicy!==0||record.opaque114!==30
    ||record.identitySpriteRemap!==false||record.callback!==originalType33Profile.callback
    ||!Array.isArray(record.colorRemaps)||record.colorRemaps.length!==16
    ||record.colorRemaps.some((row,index)=>!Array.isArray(row)||row.length!==4||row.some((word,field)=>word!==originalType33Profile.colorRemaps[index][field]))
    ||!Array.isArray(record.cancellationScripts)||record.cancellationScripts.length!==16
    ||record.cancellationScripts.some((word,index)=>word!==originalType33Profile.cancellationScripts[index])
    ||!Array.isArray(record.supportedTypes)||record.supportedTypes.length!==1||record.supportedTypes[0]!==33)
  throw new Error('Original type33 complete profile provenance rejected');
 const bytes=Uint8Array.from(record.hex.match(/../g),word=>parseInt(word,16));
 const actual=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),byte=>byte.toString(16).padStart(2,'0')).join('');
 if(actual!==profileHash)throw new Error('Original type33 whole byte checksum rejected');
 const view=new DataView(bytes.buffer);
 if(view.getInt32(0,true)!==55||view.getFloat32(0x104,true)!==4||view.getFloat32(0x108,true)!==16
    ||view.getInt32(0x10c,true)!==3||view.getInt32(0x110,true)!==6||view.getInt32(0x114,true)!==30
    ||view.getInt32(0x118,true)!==0||view.getInt32(0x11c,true)!==0)
  throw new Error('Original type33 native fields rejected');
 for(let color=0;color<16;++color)for(let field=0;field<4;++field)
  if(view.getInt32(4+color*16+field*4,true)!==record.colorRemaps[color][field])
   throw new Error('Original type33 full color table rejected');
 return bytes;
}
export async function loadOriginalType33ForWorld({module,memory,profile=originalType33Profile,isCurrent=()=>true}){
 if(typeof isCurrent!=='function'||!isCurrent())throw cancelled();
 if(typeof module?._th128_world_add_bullet_profile!=='function'||typeof module?._free!=='function'
    ||typeof module?._th128_world_error!=='function'||typeof module?.UTF8ToString!=='function'||typeof memory!=='function')
  throw new Error('Original type33 native profile API unavailable');
 const bytes=await originalType33Bytes(profile);
 if(!isCurrent())throw cancelled();
 const pointer=memory(bytes);
 if(!Number.isInteger(pointer)||pointer<=0)throw new Error('Original type33 native profile allocation rejected');
 try{
  if(!isCurrent())throw cancelled();
  const admitted=module._th128_world_add_bullet_profile(33,pointer,bytes.length);
  if(admitted!==1)throw new Error(module.UTF8ToString(module._th128_world_error())||'Original type33 native profile admission rejected');
 }finally{module._free(pointer);}
 return{type:33,script:55,secondaryScript:0,hitboxSize:4,auxiliarySize:16,appearance:3,selector:6,
  freezePolicy:0,cacheMode:6,sourceSHA256:profileHash,nativeAdmission:true,
  colorRemaps:profile.colorRemaps.map(row=>[...row]),cancellationScripts:[...profile.cancellationScripts]};
}
