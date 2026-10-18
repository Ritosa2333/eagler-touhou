// Offline Windows raster measurements; the browser loads data and C++ only.
import {FontOracle,fontParams} from './native/font-oracle.mjs';
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const root=fileURLToPath(new URL('../',import.meta.url)),out=resolve(root,'assets/sdl-native/fonts');mkdirSync(out,{recursive:true});
const sha=b=>createHash('sha256').update(b).digest('hex'),decoder=new TextDecoder('shift_jis'),mapping=Buffer.alloc(131072),chars=new Set();
for(let n=0;n<65536;++n){const b=n<256?Uint8Array.of(n):Uint8Array.of(n>>8,n&255),v=decoder.decode(b),c=v.length===1&&v!=='�'?v.charCodeAt(0):0x30fb;mapping.writeUInt16LE(c,n*2);if(c>=32&&c!==127&&c!==0x30fb)chars.add(c);}chars.add(0x30fb);
const fonts=[[32,400,'ＭＳ ゴシック'],[32,600,'ＭＳ 明朝'],[15,700,'ＭＳ ゴシック'],[15,700,'ＭＳ 明朝']];
const oracle=new FontOracle(128,72),coverageOracle=new FontOracle(128,72,25),pairs=new Map(),baked=[];
try{
 for(let index=0;index<fonts.length;++index){const [height,weight,face]=fonts[index];coverageOracle.select(fontParams(height,weight),face);const glyphs=[];
  for(const code of [...chars].sort((a,b)=>a-b)){const char=String.fromCharCode(code);coverageOracle.pixels.fill(0x8000);const light=coverageOracle.draw(char,0xffffff,16,16).slice();coverageOracle.pixels.fill(0xffff);const dark=coverageOracle.draw(char,0,16,16);let left=128,top=72,right=0,bottom=0;
   for(let p=0;p<light.length;++p)if(light[p]!==0x8000){const x=p%128,y=Math.floor(p/128);left=Math.min(left,x);top=Math.min(top,y);right=Math.max(right,x+1);bottom=Math.max(bottom,y+1);}
   const advance=coverageOracle.advance(char),width=Math.max(0,right-left),rows=Math.max(0,bottom-top),coverage=Buffer.alloc(width*rows);
   for(let y=top;y<bottom;++y)for(let x=left;x<right;++x){const p=y*128+x;if(light[p]===0x8000)continue;const key=((light[p]&31)<<5)|(dark[p]&31);if(!pairs.has(key))pairs.set(key,{id:pairs.size+1,index,char,p});coverage[(y-top)*width+x-left]=pairs.get(key).id;}
   glyphs.push({code,advance,left:width?left-16:0,top:rows?top-16:0,width,height:rows,coverage});
  }
  baked.push(glyphs);console.log(`Font ${index}: ${glyphs.length} glyphs`,JSON.stringify(fonts[index]));
 }
 if(pairs.size>255)throw Error('Unexpected GDI coverage classes');
 const blend=Buffer.alloc((pairs.size+1)*4096);
 for(const {id,index,char,p}of pairs.values()){const [height,weight,face]=fonts[index];oracle.select(fontParams(height,weight),face);
  for(let background=0;background<16;++background)for(let foreground=0;foreground<256;++foreground){oracle.pixels.fill(0xf000|background*0x111);oracle.draw(char,foreground*0x10101,16,16);blend[id*4096+background*256+foreground]=oracle.pixels[p]&15;}
 }
 const files=[];function save(name,bytes){writeFileSync(resolve(out,name),bytes);files.push({file:name,bytes:bytes.length,sha256:sha(bytes)});}
 save('cp932.bin',mapping);save('blend4444.bin',blend);
 for(let index=0;index<fonts.length;++index){const glyphs=baked[index],offset=32+glyphs.length*20,total=offset+glyphs.reduce((n,g)=>n+g.coverage.length,0),bytes=Buffer.alloc(total);bytes.write('T11G');[1,index,fonts[index][0],glyphs.length,offset,total,0].forEach((n,i)=>bytes.writeUInt32LE(n,4+i*4));let at=offset;
  glyphs.forEach((g,i)=>{const p=32+i*20;bytes.writeUInt32LE(g.code,p);bytes.writeUInt32LE(at,p+4);bytes.writeInt16LE(g.advance,p+8);bytes.writeInt16LE(g.left,p+10);bytes.writeInt16LE(g.top,p+12);bytes.writeUInt16LE(g.width,p+14);bytes.writeUInt16LE(g.height,p+16);g.coverage.copy(bytes,at);at+=g.coverage.length;});save('font'+index+'.bin',bytes);
 }
 const fontSources=['msgothic.ttc','msmincho.ttc'].map(name=>resolve(process.env.WINDIR||'C:/Windows','Fonts',name)).filter(existsSync).map(p=>({file:p.split(/[\\/]/).pop(),sha256:sha(readFileSync(p))}));
 writeFileSync(resolve(out,'manifest.json'),JSON.stringify({format:'T11G-v1',source:'TH11 1.00a 444480 CreateFont parameters; offline GDI ARGB4444 coverage',fonts,coverageClasses:pairs.size,fontSources,files},null,2)+'\n');
 console.log(JSON.stringify({fonts:4,glyphs:baked.reduce((n,g)=>n+g.length,0),coverageClasses:pairs.size,bytes:files.reduce((n,f)=>n+f.bytes,0)}));
}finally{oracle.close();coverageOracle.close();}
