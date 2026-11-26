// 中文原程序的 CreateFontA 实测统一使用微软雅黑，字重按 Thick × 100 传递。
const family='"Microsoft YaHei","Noto Sans CJK SC","PingFang SC",Arial,"Thkkd Unifont",sans-serif';
export const defaultFont=()=>({size:16,weight:400,edge:0,antialias:false,italic:false});
export function fontStyle(args){
  const signed=value=>value|0,type=signed(args[2]),thick=signed(args[1]);
  return {size:Math.max(1,signed(args[0])),weight:thick<=0?400:Math.min(900,thick*100),edge:type&1?Math.max(1,signed(args[4])):0,antialias:type>=2,italic:!!args[5]};
}
export class TextRenderer {
  constructor(){this.measureContext=document.createElement('canvas').getContext('2d');this.layouts=new Map();}
  prepare(ctx,style,size){ctx.font=`${style.italic?'italic ':''}${style.weight} ${size}px ${family}`;ctx.textBaseline='alphabetic';}
  layout(text,style,size,vertical=false){
    const key=JSON.stringify([text,style,size,vertical]),cached=this.layouts.get(key);if(cached)return cached;
    const ctx=this.measureContext;this.prepare(ctx,style,size);
    const lines=text.split('\n').map(line=>{let width=0;const glyphs=Array.from(line,c=>{const x=width,advance=Math.round(ctx.measureText(c).width)+style.edge*2;width+=advance;return {c,x,advance};});return {glyphs,width};});
    const descent=Math.ceil(ctx.measureText('Mg国').fontBoundingBoxDescent||size/3);
    const result={lines,width:vertical?size:Math.max(1,...lines.map(line=>line.width)),height:vertical?Math.max(1,Array.from(text).length)*size:Math.max(1,lines.length)*size,descent};
    this.layouts.set(key,result);if(this.layouts.size>1024)this.layouts.delete(this.layouts.keys().next().value);return result;
  }
  width(text,style,size){return this.layout(text,style,size).width;}
  raster(text,style,size,color,edgeColor,vertical=false){
    const layout=this.layout(text,style,size,vertical),padding=style.edge+2,canvas=document.createElement('canvas');
    canvas.width=layout.width+padding*2;canvas.height=layout.height+padding*2+layout.descent;
    const ctx=canvas.getContext('2d');this.prepare(ctx,style,size);
    const hex=value=>'#'+(value&0xffffff).toString(16).padStart(6,'0');
    ctx.fillStyle=style.antialias?hex(color):'#fff';ctx.strokeStyle=hex(edgeColor);ctx.lineWidth=style.edge*2;ctx.lineJoin='round';
    const draw=(c,x,y)=>{if(style.antialias&&style.edge)ctx.strokeText(c,x,y);ctx.fillText(c,x,y);};
    if(vertical)Array.from(text).forEach((c,i)=>draw(c,padding+style.edge,padding+size-2+i*size));
    else layout.lines.forEach((line,i)=>line.glyphs.forEach(({c,x})=>draw(c,padding+style.edge+x,padding+size-2+i*size)));
    if(!style.antialias){
      const data=ctx.getImageData(0,0,canvas.width,canvas.height),pixels=data.data,mask=Uint8Array.from({length:pixels.length/4},(_,i)=>+(pixels[i*4+3]>=128));
      for(let y=0;y<canvas.height;y++)for(let x=0;x<canvas.width;x++){
        const i=y*canvas.width+x;let covered=mask[i];
        if(!covered&&style.edge)for(let dy=-style.edge;dy<=style.edge&&!covered;dy++)for(let dx=-style.edge;dx<=style.edge;dx++){const xx=x+dx,yy=y+dy;if(xx>=0&&xx<canvas.width&&yy>=0&&yy<canvas.height&&mask[yy*canvas.width+xx]){covered=1;break;}}
        const value=mask[i]?color:edgeColor;pixels[i*4]=value>>>16&255;pixels[i*4+1]=value>>>8&255;pixels[i*4+2]=value&255;pixels[i*4+3]=covered?255:0;
      }
      ctx.putImageData(data,0,0);
    }
    return {canvas,padding};
  }
}
