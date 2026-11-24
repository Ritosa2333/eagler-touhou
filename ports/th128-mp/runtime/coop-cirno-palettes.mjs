// Remastered alternates for the original 256×256 pl00 body atlas. Keep the
// set identity and IDs stable for live, observer and replay views.
export const cirnoPaletteSet='remastered-6';
export const cirnoPaletteBase=new URL('../site/palettes/',import.meta.url).href;
const atlasWidth=256,atlasHeight=256,bodyPixels=atlasWidth*144;
export const cirnoPalettes=Object.freeze([
  [58436,'09036c97b2b7f63a3ad6958a3eacc07b99f14663a18298a74a781d62d4593597'],
  [61040,'2be06f18ca6ff42c6e7614b031b95396be4a5d7547761b82dd3872adafe30ed0'],
  [58440,'3cd35e5097599bc197315dc061778d2f61198c805881e360e4f87929cd95db3e'],
  [58464,'59953978dbfc57ed499eba60e78b4c611e0d2b26e42396121ddbbb5bf4da9f42'],
  [58464,'5349cd08b945944f0581cee8c6baba99a76c1bccc9f37a80e99e1896a9d50027'],
  [58440,'5499a93ba908f885d7b303bc27c0df6c39a36eadee3daff9a118555692e7c9af'],
].map(([bytes,sha256],i)=>{
  const name=`cirno-0${i+1}`,file=name+'.png',patchFile=name+'.bin';
  return Object.freeze({id:i+1,bank:80+i,file,patchFile,bytes,sha256,
    url:new URL(file,cirnoPaletteBase).href,patchUrl:new URL(patchFile,cirnoPaletteBase).href});
}));

// CRP1: ordered LE u16 (pixel start, count) followed by edited RGBA colors.
// The base and all unedited pixels come from the player's imported Package.
export function applyCirnoPalettePixels(pixels,patch){
  if(pixels.length!==atlasWidth*atlasHeight*4||patch.length<12||
     patch[0]!==67||patch[1]!==82||patch[2]!==80||patch[3]!==49)throw Error('Invalid Cirno palette pixel patch');
  const view=new DataView(patch.buffer,patch.byteOffset,patch.byteLength);
  const rows=[];let offset=4,end=0;
  while(offset<patch.length){
    if(offset+4>patch.length)throw Error('Truncated Cirno palette run');
    const start=view.getUint16(offset,true),count=view.getUint16(offset+2,true);offset+=4;
    if(!count||start<end||start+count>bodyPixels||offset+count*4>patch.length)throw Error('Invalid Cirno palette run');
    const next=offset+count*4;rows.push([start*4,offset,next]);offset=next;end=start+count;
  }
  for(const [start,from,to] of rows)pixels.set(patch.subarray(from,to),start);
}

export async function renderCirnoPalette(baseTexture,patch,signal){
  const bitmap=await createImageBitmap(baseTexture,{premultiplyAlpha:'none',colorSpaceConversion:'none'});
  try{
    signal?.throwIfAborted();
    if(bitmap.width!==atlasWidth||bitmap.height!==atlasHeight)throw Error('Original Cirno atlas dimensions differ');
    const surface=new OffscreenCanvas(atlasWidth,atlasHeight),ctx=surface.getContext('2d',{willReadFrequently:true});
    if(!ctx)throw Error('Cirno palette canvas is unavailable');
    ctx.drawImage(bitmap,0,0);
    const pixels=ctx.getImageData(0,0,atlasWidth,atlasHeight);
    applyCirnoPalettePixels(pixels.data,patch);ctx.putImageData(pixels,0,0);
    const image=await surface.convertToBlob({type:'image/png'});signal?.throwIfAborted();return image;
  }finally{bitmap.close();}
}

function mix(value){
  value=Math.imul(value^(value>>>16),0x7feb352d);
  value=Math.imul(value^(value>>>15),0x846ca68b);
  return (value^(value>>>16))>>>0;
}
export function chooseCirnoPalettes({players,matchId,seed,epoch,randomCirnoPalette,cirnoPaletteSet:chosenSet}){
  if(![2,3].includes(players))throw Error('Cirno palette seats must be 2 or 3');
  if(!randomCirnoPalette)return Object.freeze(Array(players).fill(0));
  if(chosenSet!==cirnoPaletteSet)throw Error('这是旧版染色录像，请使用录制时的多人版本播放。');
  // The lobby seed depends on the room code; mix in the shared run ID so
  // starting a fresh round in the same room can pick a different color.
  let run=0x811c9dc5;
  for(let i=0;i<matchId.length;i++)run=Math.imul(run^matchId.charCodeAt(i),0x01000193);
  const random=mix(run^seed^Math.imul(epoch,0x9e3779b9));
  const count=cirnoPalettes.length;
  const first=random%count,second=(random>>>8)%(count-1);
  return Object.freeze(players===2?[0,first+1]:[0,first+1,second+1+Number(second>=first)]);
}

export function loadCirnoPalette(renderer,palette,signal){
  const entry={index:0,width:256,height:256,name:palette.file,texture:{format:5,width:256,height:256,file:palette.file}};
  return renderer.loadBank(palette.bank,{entries:[entry]},cirnoPaletteBase,{signal});
}

export function applyCirnoPalettes(records,selection){
  if(!selection?.[1])return records;
  return records.map(r=>{
    if(r.drawPass!=='player'||r.bank!==7||r.entry!==0||r.sprite<0||r.sprite>=24)return r;
    const seat=(r.id-4000000)/10;
    const palette=cirnoPalettes[selection[seat]-1];
    // Keep the original motion, outline, color/alpha and sprite UVs. Shots,
    // portraits and special effects still sample the unmodified original bank.
    return palette?{...r,bank:palette.bank}:r;
  });
}
