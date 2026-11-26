// Original PC-98 archives are read from the player's game disk.
const rawName=hex=>String.fromCharCode(...hex.match(/../g).map(byte=>parseInt(byte,16)));
const archives={
  th03:['YUMEZIKU/'+rawName('96b28e9e8bf3312e444154')],
  th04:['GENSO/'+rawName('938c95fb8cb6917a2e8bbd'),'GENSO/'+rawName('8cb6917a8bbd45442e444154')],
};

function unpack(data,compressed,limit){
  if(!compressed)return data.slice(0,limit);
  const decoded=[];let previous=-1;
  for(let i=0;i<data.length&&decoded.length<limit;i++){
    const value=data[i];decoded.push(value);
    if(value===previous&&i+1<data.length){const count=data[++i];for(let n=0;n<count;n++)decoded.push(value);}
    previous=value;
  }
  if(decoded.length<limit)throw Error('PC-98 音乐数据不完整');
  return Uint8Array.from(decoded.slice(0,limit));
}
function nameOf(bytes){return String.fromCharCode(...bytes).split('\0')[0].toLowerCase();}
function extract0304(data){
  const view=new DataView(data.buffer,data.byteOffset,data.byteLength),size=view.getUint16(0,true),table=data.slice(16,16+size);
  let key=data[6];
  for(let i=0;i<table.length;i++){table[i]^=key;key=(key-table[i])&255;}
  const entries=new DataView(table.buffer,table.byteOffset,table.byteLength),tracks=[];
  for(let at=0;at+32<=table.length;at+=32){
    const kind=entries.getUint16(at,true);if(!kind)break;
    const name=nameOf(table.subarray(at+3,at+16));if(!/\.(m|m26|m86|efc)$/.test(name))continue;
    const packed=entries.getUint16(at+16,true),size=entries.getUint16(at+18,true),offset=entries.getUint32(at+20,true);
    const bytes=data.subarray(offset,offset+packed).map(value=>value^table[at+2]);
    tracks.push([name,unpack(bytes,kind===0x9595,size)]);
  }
  return tracks;
}
export function pmdHeaderHash(bytes){let hash=0x811c9dc5;for(const value of bytes.subarray(0,25))hash=Math.imul(hash^value,0x01000193)>>>0;return hash;}
export function extractOriginalPmd(game,fat){
  const tracks=new Map();
  for(const path of archives[game])for(const [name,bytes] of extract0304(fat.read(fat.find(path)))){
    if(!name.endsWith('.efc'))tracks.set(pmdHeaderHash(bytes),{name,bytes});
  }
  return tracks;
}
export function extractOriginalEffectBank(game,fat){
  for(const path of archives[game])for(const [name,bytes] of extract0304(fat.read(fat.find(path)))){
    if(name.endsWith('.efc'))return {name,bytes};
  }
  throw Error('游戏磁盘缺少原始音效数据');
}
