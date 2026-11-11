// Valid native score files with empty records, not copies of a developer disk.
export function emptyPc98Scores(game){
  if(game==='th02'){
    const out=new Uint8Array(910);for(let section=0;section<5;section++)out.fill(0x12,section*182,section*182+178);return out;
  }
  if(game==='th04'){
    const out=new Uint8Array(1960);
    for(let section=0;section<10;section++){
      const b=new Uint8Array(196);for(let i=0;i<10;i++)b.fill(0xc4,4+i*9,12+i*9);
      b.fill(0xa0,94,174);b[174]=25;b.fill(0xa1,176,186);
      new DataView(b.buffer).setUint16(2,b.subarray(4).reduce((a,n)=>a+n,0)&65535,true);
      let feedback=0;for(let i=195;i>=4;i--){b[i]=(b[i]-feedback)&255;feedback=((b[i]>>>3)|(b[i]<<5))&255;}
      out.set(b,section*196);
    }
    return out;
  }
  throw Error('游戏成绩格式不支持');
}
