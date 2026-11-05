// Minimal PE32 reader. All offsets are decoded from the supplied image.
export class PEImage {
  constructor(bytes){
    this.bytes=bytes;this.view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
    const pe=this.u32(0x3c),opt=pe+24;
    if(this.u32(pe)!==0x4550||this.u16(opt)!==0x10b)throw new Error('Expected PE32 executable');
    this.base=this.u32(opt+28);this.entry=this.base+this.u32(opt+16);this.size=this.u32(opt+56);this.headerSize=this.u32(opt+60);
    this.directories=Array.from({length:16},(_,i)=>({rva:this.u32(opt+96+i*8),size:this.u32(opt+100+i*8)}));
    const sections=opt+this.u16(pe+20);this.sections=Array.from({length:this.u16(pe+6)},(_,i)=>{
      const o=sections+i*40;return {name:this.ascii(o,8),rva:this.u32(o+12),virtualSize:this.u32(o+8),rawSize:this.u32(o+16),raw:this.u32(o+20),flags:this.u32(o+36)};
    });
    this.imports=[];const dir=this.directories[1];
    if(dir.rva)for(let o=this.offset(dir.rva);this.u32(o+12);o+=20){
      const dll=this.ascii(this.offset(this.u32(o+12))).toLowerCase();const first=this.u32(o+16),lookup=this.u32(o)||first;
      for(let n=0;;n++){const thunk=this.u32(this.offset(lookup)+n*4);if(!thunk)break;this.imports.push({dll,name:thunk&0x80000000?'#'+(thunk&0xffff):this.ascii(this.offset(thunk)+2),address:this.base+first+n*4});}
    }
  }
  u16(o){return this.view.getUint16(o,true);}
  u32(o){return this.view.getUint32(o,true);}
  ascii(o,max=4096){let end=o;while(end<this.bytes.length&&end-o<max&&this.bytes[end])end++;return new TextDecoder('windows-1252').decode(this.bytes.subarray(o,end));}
  offset(rva){if(rva<this.headerSize)return rva;const section=this.sections.find(s=>rva>=s.rva&&rva<s.rva+Math.max(s.rawSize,s.virtualSize));if(!section)throw new Error('Unmapped PE RVA '+rva.toString(16));return section.raw+rva-section.rva;}
}
