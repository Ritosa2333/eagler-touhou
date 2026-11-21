/*
 * Original TH128 THA1 archive reader. The decryption/format algorithms are
 * adapted from thtk thdat95.c, thcrypt.c and dattypes.h (version128 uses the
 * th12 parameter table). LZSS uses its own zero-initialized8192-byte dictionary
 * per entry, MSB-first tokens, initial head1, offset13/length4+3.
 *
 * Redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that the
 * following conditions are met:
 * 1. Redistributions of source code must retain this list
 *    of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce this
 *    list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
const HARD_ARCHIVE=64*1024*1024,HARD_ENTRY=128*1024*1024,HARD_EXPANDED=512*1024*1024;
const PARAMS=[
 [0x1b,0x73,0x40,0x3800],[0x51,0x9e,0x40,0x4000],
 [0xc1,0x15,0x400,0x2c00],[0x03,0x91,0x80,0x6400],
 [0xab,0xdc,0x80,0x6e00],[0x12,0x43,0x200,0x3c00],
 [0x35,0x79,0x400,0x3c00],[0x99,0x7d,0x80,0x2800]
];
export class Th128DatError extends Error {
 constructor(message){super(message);this.name='Th128DatError';this.code='TH128_DAT_FORMAT';}
}
function need(ok,message){if(!ok)throw new Th128DatError(message);}
function limit(value,fallback,label){
 if(value===undefined)return fallback;
 need(Number.isSafeInteger(value)&&value>0&&value<=fallback,label+' size budget is invalid');
 return value;
}
function decrypt(input,initialKey,step,block,limitBytes){
 const data=new Uint8Array(input),temporary=new Uint8Array(block);
 let key=initialKey,size=data.length;
 if(size<(block>>>2))size=0;
 else size-=((size%block)<(block>>>2)?size%block:0)+size%2;
 if(limitBytes%block)limitBytes+=block-limitBytes%block;
 const end=Math.min(size,limitBytes);
 for(let base=0;base<end;){
  const count=Math.min(block,end-base),increment=(count>>>1)+(count&1);
  let inputIndex=0,outputIndex=count-1;
  while(outputIndex>0){
   temporary[outputIndex--]=data[base+inputIndex]^key;
   temporary[outputIndex--]=data[base+inputIndex+increment]^(key+step*increment);
   inputIndex++;key=(key+step)&255;
  }
  if(count&1){temporary[outputIndex]=data[base+inputIndex]^key;key=(key+step)&255;}
  key=(key+step*increment)&255;
  data.set(temporary.subarray(0,count),base);base+=count;
 }
 return data;
}
function unlzss(data,expected){
 const output=new Uint8Array(expected),dictionary=new Uint8Array(8192);
 let cursor=0,available=0,reservoir=0,written=0,head=1;
 function bits(n){
  while(available<n){
   need(cursor<data.length,'Truncated LZSS token');
   reservoir=(reservoir<<8)|data[cursor++];available+=8;
  }
  available-=n;return(reservoir>>>available)&((1<<n)-1);
 }
 function put(value){output[written++]=value;dictionary[head]=value;head=(head+1)&8191;}
 while(written<expected){
  if(bits(1))put(bits(8));
  else{
   const offset=bits(13);need(offset!==0,'LZSS ended before declared entry size');
   const count=bits(4)+3;need(count<=expected-written,'LZSS entry exceeds declared size');
   for(let i=0;i<count;i++)put(dictionary[(offset+i)&8191]);
  }
 }
 return output;
}
function nameOf(bytes,start,end){
 need(end>start&&end-start<=255,'Invalid archive entry name length');
 let name='';
 for(let p=start;p<end;p++){need(bytes[p]>=32&&bytes[p]<=126,'Non-ASCII archive entry name');name+=String.fromCharCode(bytes[p]);}
 need(/^[A-Za-z0-9][A-Za-z0-9_.-]*$/.test(name),'Unsafe archive entry name');
 return name;
}
function cryptParameters(name){
 let sum=0;for(let i=0;i<name.length;i++)sum=(sum+name.charCodeAt(i))&255;
 return PARAMS[sum&7];
}
export function openTh128Dat(input,options={}){
 need(input instanceof Uint8Array,'Archive must contain byte data');
 const maxArchive=limit(options.maxArchiveBytes,HARD_ARCHIVE,'Archive');
 const maxEntry=limit(options.maxEntryBytes,HARD_ENTRY,'Entry');
 const maxExpanded=limit(options.maxExpandedBytes,HARD_EXPANDED,'Expanded archive');
 need(input.byteLength>=16&&input.byteLength<=maxArchive,'Archive header/size exceeds budget');
 const source=new Uint8Array(input),header=decrypt(source.subarray(0,16),0x1b,0x37,16,16);
 need(String.fromCharCode(...header.subarray(0,4))==='THA1','Invalid THA1 archive magic');
 const view=new DataView(header.buffer,header.byteOffset,header.byteLength);
 const tableSize=(view.getUint32(4,true)-123456789)>>>0;
 const storedTableSize=(view.getUint32(8,true)-987654321)>>>0;
 const entryCount=(view.getUint32(12,true)-135792468)>>>0;
 need(tableSize>0&&tableSize<=1024*1024,'Archive table size exceeds budget');
 need(storedTableSize>0&&storedTableSize<=source.length-16,'Archive table bounds are invalid');
 need(entryCount>0&&entryCount<=4096,'Archive entry count exceeds budget');
 const tableOffset=source.length-storedTableSize;
 const table=unlzss(decrypt(source.subarray(tableOffset),0x3e,0x9b,0x80,storedTableSize),tableSize);
 const tv=new DataView(table.buffer,table.byteOffset,table.byteLength),entries=[],groups=new Map(),folded=new Map();
 let cursor=0,expanded=0;
 for(let i=0;i<entryCount;i++){
  let zero=cursor;while(zero<table.length&&table[zero]!==0)zero++;
  need(zero<table.length,'Unterminated archive entry name');
  const name=nameOf(table,cursor,zero),length=zero-cursor;
  cursor+=length+(4-length%4);
  need(cursor+12<=table.length,'Truncated archive entry table');
  const offset=tv.getUint32(cursor,true),size=tv.getUint32(cursor+4,true),extra=tv.getUint32(cursor+8,true);
  cursor+=12;
  need(size>0&&size<=maxEntry,'Archive entry size exceeds budget');
  need(expanded<=maxExpanded-size,'Expanded archive size exceeds budget');expanded+=size;
  need(offset>=16&&offset<tableOffset,'Archive entry offset outside data bounds');
  if(entries.length)need(offset>entries.at(-1).offset,'Archive entry offsets are not increasing');
  const foldedName=name.toLowerCase();
  need(!folded.has(foldedName)||folded.get(foldedName)===name,'Ambiguous archive entry names');
  folded.set(foldedName,name);
  const entry={name,offset,size,extra,storedBytes:0};entries.push(entry);
  if(!groups.has(name))groups.set(name,[]);
  groups.get(name).push(entry);
 }
 for(let i=0;i<entries.length;i++){
  const end=i+1<entries.length?entries[i+1].offset:tableOffset;
  entries[i].storedBytes=end-entries[i].offset;
  need(entries[i].storedBytes>0&&entries[i].storedBytes<=source.length,'Archive entry storage size is invalid');
 }
 const cache=new Map();
 function decode(entry){
  const compressed=decrypt(source.subarray(entry.offset,entry.offset+entry.storedBytes),...cryptParameters(entry.name));
  return entry.storedBytes===entry.size?compressed:unlzss(compressed,entry.size);
 }
 function read(name){
  need(typeof name==='string'&&/^[A-Za-z0-9][A-Za-z0-9_.-]*$/.test(name),'Unsafe resource name');
  need(groups.has(name),'Missing archive resource: '+name);
  if(!cache.has(name)){
   const copies=groups.get(name),body=decode(copies[0]);
   for(let i=1;i<copies.length;i++){
    const other=decode(copies[i]);
    need(other.length===body.length&&other.every((v,p)=>v===body[p]),'Conflicting duplicate archive resource: '+name);
   }
   cache.set(name,body);
  }
  return cache.get(name).slice();
 }
 return Object.freeze({names:Object.freeze([...groups.keys()]),entries:Object.freeze(entries.map(e=>Object.freeze({...e}))),read});
}
