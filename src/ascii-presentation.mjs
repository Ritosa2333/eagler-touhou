// Native4018f0/401bxx expands FontManager submissions into ASCII ANM sprites.
// drawFlag selects a queue at401790; it does not change glyphs or enable shadow.
const f32=Math.fround;
const metrics={1:[7,9],2:[7,10],3:[12,16],4:[12,16],5:[7,10]};
const smallSymbols={'/':237,':':238,'-':239,'*':240,'%':241,'.':242,'$':262};
const largeSymbols={'/':253,'.':254,s:255,'*':256,',':257};

export function asciiGlyphs(descriptor) {
  if(!descriptor||!metrics[descriptor.font])return [];
  const {font,alignX=1,alignY=1,colorARGB=0xffffffff}=descriptor;
  const text=String(descriptor.text??'').split('\0')[0];
  const sx=f32(descriptor.scaleX??1),sy=f32(descriptor.scaleY??1),z=f32(descriptor.z??0);
  const originX=f32(descriptor.x??0),originY=f32(descriptor.y??0),[advance,height]=metrics[font];
  const ordinary=f32(advance*sx),large=font===3||font===4;
  let x=originX,y=originY,step=ordinary;
  if(alignX===0||alignX===2) {
    const factor=alignX===0?.5:1;
    if(font===1)x=f32(x-text.length*ordinary*factor);
    else for(const c of text){
      const narrow=large?c===',':c==='.';
      x=f32(x-(narrow?4*sx:ordinary)*factor);
    }
  }
  // Vertical alignment uses the unscaled font height. Font3/4 reset y to
  // the original submission y for each glyph, then lower only commas by3.
  if(alignY===0)y=f32(y-height*.5);
  else if(alignY===2)y=f32(y-height);
  const output=[];
  for(const c of text) {
    if(c==='\n'){y=f32(y+height*sy);x=originX;continue;}
    if(c===' '){x=f32(x+step);continue;}
    step=ordinary;
    let id;
    if(font===1)id=c.charCodeAt(0)+66;
    else if(large){
      id=largeSymbols[c]??c.charCodeAt(0)+195;
      y=c===','?f32(originY+3):originY;
      if(c===',')step=f32(4*sx);
    }else{
      id=smallSymbols[c]??c.charCodeAt(0)+179;
      if(c==='.')step=f32(4*sx);
    }
    output.push({id,x,y,z,scaleX:sx,scaleY:sy,colorARGB:colorARGB>>>0,
      anchorX:1,anchorY:1,point:font===4||font===5,pixel:true});
    x=f32(x+step);
  }
  return output;
}
