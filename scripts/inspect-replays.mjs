import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';

export function decrypt(input, key, step, block, limit=input.length) {
  const out=Buffer.from(input); let cursor=0;
  const untouched=(input.length%block<block/4?input.length%block:0)+(input.length&1);
  let remaining=input.length-untouched;
  while(remaining>0&&limit>0) {
    const n=Math.min(block,remaining);let linear=0;
    for(let parity=1;parity<=2;parity++)for(let pos=n-parity;pos>=0;pos-=2) {
      out[cursor+pos]=input[cursor+linear++]^key; key=(key+step)&255;
    }
    cursor+=n;remaining-=n;limit-=n;
  }
  return out;
}
export function decompress(input, size) {
  if(size<0||size>64*1024*1024)throw Error('invalid decoded size');
  const out=Buffer.alloc(size),dict=Buffer.alloc(8192);let p=0,mask=128,byte=0,w=0,head=1;
  const bit=()=>{if(mask===128)byte=p<input.length?input[p++]:0;const b=!!(byte&mask);mask>>=1;if(!mask)mask=128;return b?1:0;};
  const bits=n=>{let v=0;while(n--)v=v*2+bit();return v;};
  const put=v=>{if(w>=size)throw Error('decompressed data exceeds declared size');out[w++]=v;dict[head]=v;head=(head+1)&8191;};
  for(;;){if(bit())put(bits(8));else {const offset=bits(13);if(!offset)break;const length=bits(4)+3;for(let i=0;i<length;i++)put(dict[(offset+i)&8191]);}}
  if(w!==size)throw Error(`incomplete decoded data: ${w}/${size}`);return out;
}

if(process.argv[1]&&path.resolve(process.argv[1])===path.resolve(new URL(import.meta.url).pathname.replace(/^\/(.:)/,'$1'))) {
  const root=path.resolve(import.meta.dirname,'../..');
  const files=[...fs.readdirSync(path.join(root,'th12/replay')).filter(n=>n.endsWith('.rpy')).map(n=>path.join(root,'th12/replay',n)),...fs.readdirSync(path.join(root,'th12-eagler/local/retail')).filter(n=>/^demo\d\.rpy$/.test(n)).map(n=>path.join(root,'th12-eagler/local/retail',n))];
  for(const file of files) {
    const b=fs.readFileSync(file);console.log(path.basename(file),b.subarray(0,36).toString('hex'));
    try {
      const packed=b.readUInt32LE(28),size=b.readUInt32LE(32);
      const d=decompress(decrypt(decrypt(b.subarray(36,36+packed),0x5e,0xe1,0x800),0x7d,0x3a,0x40),size);
      console.log('decoded',d.length,d.subarray(0,200).toString('hex'));
      fs.mkdirSync(path.join(root,'th12-eagler/local/replays'),{recursive:true});fs.writeFileSync(path.join(root,'th12-eagler/local/replays',path.basename(file)+'.bin'),d);
    }catch(e){console.log('ERROR',e.message);}
  }
}
