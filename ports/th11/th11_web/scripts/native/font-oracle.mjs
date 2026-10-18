// Build-time Windows GDI oracle. All drawing is into an offscreen DIB.
import {createRequire} from 'node:module';
const koffi=createRequire(new URL('../../../th08_web/package.json',import.meta.url))('koffi');
export class FontOracle {
  constructor(width=128,height=72,format=26){const dll=koffi.load('gdi32.dll');this.fn={flush:dll.func('int __stdcall GdiFlush()'),dib:dll.func('void * __stdcall CreateDIBSection(void *, const void *, uint32_t, _Out_ void **, void *, uint32_t)'),dc:dll.func('void * __stdcall CreateCompatibleDC(void *)'),select:dll.func('void * __stdcall SelectObject(void *, void *)'),deleteDC:dll.func('int __stdcall DeleteDC(void *)'),deleteObject:dll.func('int __stdcall DeleteObject(void *)'),font:dll.func('void * __stdcall CreateFontW(int,int,int,int,int,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,str16)'),mode:dll.func('int __stdcall SetBkMode(void *, int)'),color:dll.func('uint32_t __stdcall SetTextColor(void *, uint32_t)'),text:dll.func('int __stdcall TextOutW(void *,int,int,str16,int)'),extent:dll.func('int __stdcall GetTextExtentPoint32W(void *,str16,int,void *)')};
    this.width=width;this.height=height;const info=new Uint8Array(108),v=new DataView(info.buffer);v.setUint32(0,108,true);v.setInt32(4,width,true);v.setInt32(8,-height,true);v.setUint16(12,1,true);v.setUint16(14,16,true);v.setUint32(16,3,true);(format===25?[0x7c00,0x3e0,0x1f,0x8000]:[0xf00,0xf0,0xf,0xf000]).forEach((n,i)=>v.setUint32(40+i*4,n,true));const bits=[null];this.bitmap=this.fn.dib(null,info,0,bits,null,0);if(!this.bitmap)throw Error("Original ARGB4444 DIB unavailable");this.dc=this.fn.dc(null);this.oldBitmap=this.fn.select(this.dc,this.bitmap);this.fn.mode(this.dc,1);this.pixels=new Uint16Array(koffi.view(bits[0],width*height*2));this.size=new Uint8Array(8);
  }
  select(font,face='ＭＳ ゴシック'){if(this.font){this.fn.select(this.dc,this.oldFont);this.fn.deleteObject(this.font);}this.font=this.fn.font(...font,face);this.oldFont=this.fn.select(this.dc,this.font);}
  draw(text,color=0xffffff,x=0,y=0){this.fn.color(this.dc,color);this.fn.text(this.dc,x,y,text,text.length);this.fn.flush();return this.pixels;}
  advance(text){this.fn.extent(this.dc,text,text.length,this.size);return new DataView(this.size.buffer).getInt32(0,true);}
  close(){if(this.font){this.fn.select(this.dc,this.oldFont);this.fn.deleteObject(this.font);}this.fn.select(this.dc,this.oldBitmap);this.fn.deleteObject(this.bitmap);this.fn.deleteDC(this.dc);}
}
export const fontParams=(size,weight)=>[size,0,0,0,weight,0,0,0,128,0,0,4,17];
