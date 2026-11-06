// Independent development decoder following the pinned thtk TH14-TH17 format.
import {readFileSync} from 'node:fs';
export const cryptParams=[[27,115,256,14336],[18,67,512,15872],[53,121,1024,15360],[3,145,128,25600],[171,220,128,28672],[81,158,256,16384],[193,21,1024,11264],[153,125,128,17408]];
export function decrypt(input,[key,step,block,limit]){
 const out=Buffer.from(input);let untouched=(input.length%block<block/4?input.length%block:0)+(input.length&1),remaining=input.length-untouched,at=0;
 while(remaining>0&&limit>0){const length=Math.min(block,remaining);let read=0;for(let parity=1;parity<=2;parity++)for(let i=length-parity;i>=0;i-=2){out[at+i]=input[at+read++]^key;key=(key+step)&255;}at+=length;remaining-=length;limit-=length;}
 return out;
}
export function unlzss(input,capacity,dictionary=Buffer.alloc(8192)){
 const out=Buffer.alloc(capacity);let cursor=0,head=1,written=0,byte=0,mask=0;
 const bit=()=>{if(!mask){byte=cursor<input.length?input[cursor++]:0;mask=128;}const v=+(!!(byte&mask));mask>>=1;return v;};
 const bits=n=>{let result=0;while(n--)result=(result<<1)|bit();return result;};
 const put=v=>{if(written>=capacity)throw Error('LZSS output exceeds declared capacity');out[written++]=v;dictionary[head]=v;head=(head+1)&8191;};
 for(;;){if(bit())put(bits(8));else{const offset=bits(13);if(!offset)break;const length=bits(4)+3;for(let i=0;i<length;i++)put(dictionary[(offset+i)&8191]);}}
 if(written!==capacity)throw Error('LZSS length mismatch: '+written+' != '+capacity);
 return out;
}
export function openArchive(path){
 const bytes=readFileSync(path),header=decrypt(bytes.subarray(0,16),[27,55,16,16]);if(header.toString('ascii',0,4)!=='THA1')throw Error('Not a THA1 archive');
 const unpacked=(header.readUInt32LE(4)-123456789)>>>0,packed=(header.readUInt32LE(8)-987654321)>>>0,count=(header.readUInt32LE(12)-135792468)>>>0;
 if(packed>bytes.length-16||unpacked>64*1024*1024||count>100000)throw Error('Invalid archive bounds');
 const tableAt=bytes.length-packed,table=unlzss(decrypt(bytes.subarray(tableAt),[62,155,128,packed]),unpacked),entries=[];let at=0;
 for(let i=0;i<count;i++){const end=table.indexOf(0,at);if(end<at)throw Error('Unterminated archive name');const name=table.toString('ascii',at,end);at=end+4-(end-at)%4;
  if(at+12>table.length||!name||name.includes('..')||name.startsWith('/')||name.includes('\\'))throw Error('Unsafe archive entry');
  const offset=table.readUInt32LE(at),size=table.readUInt32LE(at+4),reserved=table.readUInt32LE(at+8);at+=12;if(offset<16||offset>tableAt||size>64*1024*1024)throw Error('Invalid resource bounds');entries.push({name,offset,size,reserved});
 }
 for(let i=0;i<count;i++){entries[i].compressed=(i+1<count?entries[i+1].offset:tableAt)-entries[i].offset;if(entries[i].compressed<0)throw Error('Unsorted archive');}
 return {entries,read(entry){let index=0;for(const c of Buffer.from(entry.name))index=(index+c)&255;const source=bytes.subarray(entry.offset,entry.offset+entry.compressed),decoded=decrypt(source,cryptParams[index&7]);return entry.compressed===entry.size?decoded:unlzss(decoded,entry.size);}};
}
