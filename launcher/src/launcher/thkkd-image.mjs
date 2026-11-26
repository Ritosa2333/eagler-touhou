import UPNG from 'upng-js';
export function decodeThkkdBmp(bytes) {
  const v = new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
  if (bytes.length < 54 || v.getUint16(0,true) !== 0x4d42 || v.getUint32(14,true) < 40 || v.getUint32(30,true) !== 0) throw Error('海惠堂 BMP 格式无效');
  const offset = v.getUint32(10,true), width = v.getInt32(18,true), rawHeight = v.getInt32(22,true), height = Math.abs(rawHeight), depth = v.getUint16(28,true);
  const stride = Math.ceil(width*depth/32)*4, palette = 14+v.getUint32(14,true);
  if (width<=0 || height<=0 || width*height>16*1024*1024 || ![1,4,8,24,32].includes(depth) || offset+height*stride>bytes.length) throw Error('海惠堂 BMP 尺寸无效');
  const rgba = new Uint8Array(width*height*4);
  for (let y=0;y<height;y++) for (let x=0;x<width;x++) {
    const row = offset+(rawHeight<0?y:height-1-y)*stride, target=(y*width+x)*4; let p;
    if (depth>=24) p=row+x*(depth/8);
    else {
      const index=depth===8?bytes[row+x]:depth===4?(bytes[row+(x>>1)]>>((1-(x&1))*4))&15:(bytes[row+(x>>3)]>>(7-(x&7)))&1;
      p=palette+index*4; if(p+4>offset)throw Error('海惠堂 BMP 调色板越界');
    }
    rgba[target]=bytes[p+2];rgba[target+1]=bytes[p+1];rgba[target+2]=bytes[p];rgba[target+3]=255;
  }
  return {width,height,rgba,intrinsicAlpha:false};
}
async function decodeImage(bytes,name) {
  if(bytes[0]===0x42&&bytes[1]===0x4d)return decodeThkkdBmp(bytes);
  if(bytes[0]===0x89&&bytes[1]===0x50&&bytes[2]===0x4e&&bytes[3]===0x47){
    const png=UPNG.decode(bytes.slice().buffer);
    if(png.width*png.height>16*1024*1024)throw Error('海惠堂 PNG 尺寸过大');
    return {width:png.width,height:png.height,rgba:new Uint8Array(UPNG.toRGBA8(png)[0]),intrinsicAlpha:png.ctype===4||png.ctype===6||png.tabs.tRNS!==undefined};
  }
  if(bytes[0]!==0xff||bytes[1]!==0xd8)throw Error('海惠堂图片格式无效：'+name);
  const image=await createImageBitmap(new Blob([bytes],{type:'image/jpeg'}),{colorSpaceConversion:'none',premultiplyAlpha:'none'});
  try{
    if(image.width*image.height>16*1024*1024)throw Error('海惠堂 JPG 尺寸过大');
    const canvas=new OffscreenCanvas(image.width,image.height),context=canvas.getContext('2d',{willReadFrequently:true});
    context.drawImage(image,0,0);return {width:image.width,height:image.height,rgba:new Uint8Array(context.getImageData(0,0,image.width,image.height).data.buffer),intrinsicAlpha:false};
  }finally{image.close();}
}
export async function convertThkkdImage(bytes,name,mask) {
  const image=await decodeImage(bytes,name);
  if(mask){
    const alpha=await decodeImage(mask,name);
    if(image.width!==alpha.width||image.height!==alpha.height)throw Error('海惠堂透明遮罩尺寸不匹配');
    for(let i=0;i<image.rgba.length;i+=4)image.rgba[i+3]=Math.floor(image.rgba[i+3]*alpha.rgba[i]/255);
  }
  return {blob:new Blob([UPNG.encode([image.rgba.buffer],image.width,image.height,0,undefined,true)],{type:'image/png'}),intrinsicAlpha:image.intrinsicAlpha||!!mask};
}
