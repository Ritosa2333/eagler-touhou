// Package-owned fonts stay outside Runtime Release. A TTC face is rebuilt as
// a standalone SFNT so browser collection-face defaults cannot select PGothic.
const tag=(bytes,at)=>String.fromCharCode(...bytes.subarray(at,at+4));
const installed=new WeakMap();
const sum=bytes=>{let n=0;const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);for(let at=0;at<bytes.length;at+=4)n=(n+view.getUint32(at,false))>>>0;return n;};
function faceName(bytes,base){
  const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),count=view.getUint16(base+4,false);
  for(let i=0;i<count;i++){
    const record=base+12+i*16;if(tag(bytes,record)!=='name')continue;
    const start=view.getUint32(record+8,false),length=view.getUint32(record+12,false);if(start+length>bytes.length||length<6)return '';
    const records=view.getUint16(start+2,false),strings=start+view.getUint16(start+4,false),names=[];
    for(let j=0;j<records&&6+(j+1)*12<=length;j++){
      const at=start+6+j*12,platform=view.getUint16(at,false),id=view.getUint16(at+6,false),size=view.getUint16(at+8,false),offset=strings+view.getUint16(at+10,false);
      if(![1,4,6].includes(id)||offset+size>start+length)continue;
      names.push(new TextDecoder(platform===0||platform===3?'utf-16be':'windows-1252').decode(bytes.subarray(offset,offset+size)));
    }
    return names.join('\n');
  }
  return '';
}
export function singleFont(value){
  const bytes=value instanceof Uint8Array?value:new Uint8Array(value),view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
  if(bytes.length<12||bytes.length>64*1024*1024)throw Error('共享字体大小无效');
  if(tag(bytes,0)!=='ttcf')return bytes.slice();
  const count=view.getUint32(8,false);if(count<1||count>128||12+count*4>bytes.length)throw Error('字体集合头无效');
  const faces=[];for(let i=0;i<count;i++){const base=view.getUint32(12+i*4,false);if(base+12>bytes.length)throw Error('字体集合目录无效');const n=view.getUint16(base+4,false);if(n<1||n>512||base+12+n*16>bytes.length)throw Error('字体表目录无效');faces.push(base);}
  const preferred=faces.find(base=>/\bMS[- ]?Gothic\b|ＭＳ ゴシック/i.test(faceName(bytes,base))),base=preferred??faces[0],tables=view.getUint16(base+4,false),records=[];
  let size=12+tables*16;
  for(let i=0;i<tables;i++){const at=base+12+i*16,offset=view.getUint32(at+8,false),length=view.getUint32(at+12,false);if(offset+length>bytes.length||length>64*1024*1024)throw Error('字体表范围无效');records.push({at,offset,length,to:size});size+=(length+3)&~3;}
  if(size>64*1024*1024)throw Error('字体展开过大');
  const out=new Uint8Array(size),result=new DataView(out.buffer);out.set(bytes.subarray(base,base+12));let head=null;
  records.forEach(({at,offset,length,to},i)=>{const record=12+i*16;out.set(bytes.subarray(at,at+16),record);out.set(bytes.subarray(offset,offset+length),to);result.setUint32(record+8,to,false);if(tag(bytes,at)==='head'&&length>=12){head=to;result.setUint32(to+8,0,false);}result.setUint32(record+4,sum(out.subarray(to,to+((length+3)&~3))),false);});
  if(head!==null)result.setUint32(head+8,(0xb1b0afba-sum(out))>>>0,false);return out;
}
export async function installRuntimeFonts(fonts,environment=window){
  const gothic=fonts.get('/msgothic.ttc'),unicode=fonts.get('/unifont.otf');
  if(!gothic&&!unicode)return;
  const previous=installed.get(environment);if(previous?.gothic===gothic&&previous?.unicode===unicode)return previous.pending;
  if(typeof environment.FontFace!=='function'||!environment.document?.fonts)throw Error('浏览器不支持游戏字体加载');
  const load=async(family,bytes)=>{const face=new environment.FontFace(family,bytes);await face.load();environment.document.fonts.add(face);};
  const pending=(async()=>{
    if(gothic){const bytes=singleFont(await gothic.arrayBuffer());await Promise.all(['ＭＳ ゴシック','MS Gothic'].map(name=>load(name,bytes)));}
    if(unicode)await load('Eagler Unicode',singleFont(await unicode.arrayBuffer()));
  })();installed.set(environment,{gothic,unicode,pending});
  try{await pending;}catch(error){installed.delete(environment);throw error;}
}
