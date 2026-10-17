// Extract the original title artwork; no recreated or TH10/TH09 artwork.
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
import {deflateSync} from 'node:zlib';
const root=fileURLToPath(new URL('../',import.meta.url)),data=readFileSync(resolve(root,'reference/assets/title.anm'));
const width=640,height=480,pixels=Buffer.alloc(width*height*4);let offset=0,count=0;
do{const stride=data.readUInt32LE(offset+36),nameStart=offset+data.readUInt32LE(offset+16),name=data.subarray(nameStart,data.indexOf(0,nameStart)).toString();
 if(name==='title/title00a.png'&&data[offset+32]){const p=offset+data.readUInt32LE(offset+28),x=data.readInt16LE(offset+20),y=data.readInt16LE(offset+22),w=data.readUInt16LE(p+8),h=data.readUInt16LE(p+10);if(data.readUInt16LE(p+6)!==1||data.readUInt32LE(p+12)!==w*h*4||x<0||y<0||x+w>width||y+h>height)throw Error('Unexpected title texture');
 for(let row=0;row<h;++row)for(let col=0;col<w;++col){const a=p+16+(row*w+col)*4,b=((row+y)*width+col+x)*4;pixels[b]=data[a+2];pixels[b+1]=data[a+1];pixels[b+2]=data[a];pixels[b+3]=data[a+3];}++count;}
 if(!stride)break;offset+=stride;
}while(offset<data.length);if(count!==2)throw Error('Missing original title texture tiles');
const crc=bytes=>{let c=0xffffffff;for(const b of bytes){c^=b;for(let i=0;i<8;i++)c=(c>>>1)^((c&1)?0xedb88320:0);}return (c^0xffffffff)>>>0;};
const chunk=(name,bytes)=>{const t=Buffer.from(name),n=Buffer.alloc(4),c=Buffer.alloc(4);n.writeUInt32BE(bytes.length);c.writeUInt32BE(crc(Buffer.concat([t,bytes])));return Buffer.concat([n,t,bytes,c]);};
const hdr=Buffer.alloc(13);hdr.writeUInt32BE(width);hdr.writeUInt32BE(height,4);hdr[8]=8;hdr[9]=6;const scan=Buffer.alloc(height*(width*4+1));for(let y=0;y<height;y++)pixels.copy(scan,y*(width*4+1)+1,y*width*4,(y+1)*width*4);
const png=Buffer.concat([Buffer.from([137,80,78,71,13,10,26,10]),chunk('IHDR',hdr),chunk('IDAT',deflateSync(scan)),chunk('IEND',Buffer.alloc(0))]);
mkdirSync(resolve(root,'launcher/public/assets'),{recursive:true});writeFileSync(resolve(root,'launcher/public/assets/th11-card.png'),png);console.log(JSON.stringify({width,height,bytes:png.length,source:'title.anm / title/title00a.png'}));
