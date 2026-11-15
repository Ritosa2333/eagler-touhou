import crypto from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const pinnedExeSha256='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4';
const sourceAddress=0x4b24d0+13*0x120;
const profileBytes=288;
const args=process.argv.slice(2);
function argument(name){
  const index=args.indexOf(name);
  return index>=0?args[index+1]:undefined;
}
const exePath=argument('--exe')??process.env.TH128_EXE;
const write=args.includes('--write');
if(!exePath)throw new Error('usage: node tools/prepare-type13-profile.mjs --exe <pinned th128.exe> [--write]');
if(!Number.isInteger(sourceAddress)||sourceAddress<0)throw new Error('invalid type13 source address');

function u16(buffer,offset){return buffer.readUInt16LE(offset);}
function u32(buffer,offset){return buffer.readUInt32LE(offset);}
function i32(buffer,offset){return buffer.readInt32LE(offset);}
function f32(buffer,offset){return buffer.readFloatLE(offset);}
function fileOffsetForVirtualAddress(exe,virtualAddress){
  if(exe.readUInt16LE(0)!==0x5a4d)throw new Error('not a PE image: missing MZ header');
  const peOffset=u32(exe,0x3c);
  if(exe.readUInt32LE(peOffset)!==0x00004550)throw new Error('not a PE image: missing PE signature');
  const coff=peOffset+4;
  const sectionCount=u16(exe,coff+2);
  const optionalSize=u16(exe,coff+16);
  const optional=coff+20;
  const magic=u16(exe,optional);
  let imageBase;
  if(magic===0x20b)imageBase=Number(exe.readBigUInt64LE(optional+24));
  else if(magic===0x10b)imageBase=u32(exe,optional+28);
  else throw new Error('unsupported PE optional-header magic');
  const rva=virtualAddress-imageBase;
  if(!Number.isInteger(rva)||rva<0)throw new Error('virtual address is below the PE image base');
  const sections=optional+optionalSize;
  for(let index=0;index<sectionCount;index++){
    const section=sections+index*40;
    const virtualSize=u32(exe,section+8);
    const virtualAddressStart=u32(exe,section+12);
    const rawSize=u32(exe,section+16);
    const rawOffset=u32(exe,section+20);
    const span=Math.max(virtualSize,rawSize);
    if(rva<virtualAddressStart||rva>=virtualAddressStart+span)continue;
    const offset=rawOffset+(rva-virtualAddressStart);
    if(offset+profileBytes>exe.length)throw new Error('profile span exceeds EXE image');
    return offset;
  }
  throw new Error('virtual address is not mapped by a PE section');
}

const exe=fs.readFileSync(exePath);
const actualExeSha256=crypto.createHash('sha256').update(exe).digest('hex');
if(actualExeSha256!==pinnedExeSha256)throw new Error(`EXE SHA-256 mismatch: ${actualExeSha256}`);
const fileOffset=fileOffsetForVirtualAddress(exe,sourceAddress);
const bytes=exe.subarray(fileOffset,fileOffset+profileBytes);
if(bytes.length!==profileBytes)throw new Error('type13 profile byte length is not 288');
const sha256=crypto.createHash('sha256').update(bytes).digest('hex');
const script=i32(bytes,0);
const secondaryScript=i32(bytes,0x11c);
const hitboxSize=f32(bytes,0x104);
const auxiliarySize=f32(bytes,0x108);
const appearance=i32(bytes,0x10c);
const selector=i32(bytes,0x110);
const freezePolicy=i32(bytes,0x118);
const identitySpriteRemap=u32(bytes,4)>=0x80000000;
const perColor=selector===6?Array.from({length:16},(_,color)=>i32(bytes,(4+color*4)*4)):undefined;
if(selector!==6)throw new Error(`type13 selector ${selector} is not the supported per-color mode6 profile`);
if(!perColor.every(value=>Number.isInteger(value)&&value>=-1&&value<=4095))throw new Error('type13 per-color cancellation script is outside the supported range');

const profile={
  schema:'th128-original-typed-bullet-profile/1',
  type:13,
  sourceExeSha256:pinnedExeSha256,
  sourceAddress:`0x${sourceAddress.toString(16).padStart(8,'0')}`,
  bytes:profileBytes,
  sha256,
  hex:bytes.toString('hex'),
  script,
  secondaryScript,
  hitboxSize,
  auxiliarySize,
  appearance,
  selector,
  freezePolicy,
  perColor,
  identitySpriteRemap,
  supportedTypes:[13],
};
const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
const assetRoot=path.join(root,'local');
const outputs=[];
for(const variant of ['ja','zh']){
  const manifest=catalog.variants?.[variant]?.manifest;
  if(typeof manifest!=='string')throw new Error(`active ${variant} catalog manifest is missing`);
  const target=path.join(assetRoot,path.dirname(manifest.replace(/\//g,path.sep)),'animations','bullet-type13-profile.json');
  if(!fs.existsSync(path.join(assetRoot,manifest)))throw new Error(`active ${variant} manifest is missing: ${manifest}`);
  if(write){
    fs.mkdirSync(path.dirname(target),{recursive:true});
    fs.writeFileSync(target,`${JSON.stringify(profile,null,1)}\n`,'utf8');
  }
  outputs.push(target);
}
console.log(JSON.stringify({
  exeSha256:pinnedExeSha256,
  sourceAddress:profile.sourceAddress,
  fileOffset:`0x${fileOffset.toString(16)}`,
  sha256,
  bytes:profileBytes,
  script,
  secondaryScript,
  hitboxSize,
  auxiliarySize,
  appearance,
  selector,
  freezePolicy,
  perColor,
  identitySpriteRemap,
  written:write,
  outputs,
},null,2));
