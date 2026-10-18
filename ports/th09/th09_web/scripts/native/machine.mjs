// Development oracle only. Never included in the production browser build.
import {PEImage} from './pe.mjs';

export const hex=n=>'0x'+(Number(n)>>>0).toString(16);
export class NativeMachine {
  static async create(bytes,options={}){const machine=new NativeMachine();await machine.init(bytes,options);return machine;}
  async init(bytes,options){
    const MUnicorn=options.factory??globalThis.MUnicorn??(await import('../../../th08_web/node_modules/@alexaltea/unicorn-js/dist/unicorn_x86.js')).default;
    const instantiate=WebAssembly.instantiate;
    WebAssembly.instantiate=async(...args)=>{if(options.wasmBinary)args[0]=options.wasmBinary;const result=await instantiate(...args);this.wasmBinary=args[0];this.wasmExports=result.instance?.exports;return result;};
    try{this.uc=await MUnicorn(options);}finally{WebAssembly.instantiate=instantiate;}
    const u=this.uc;this.cpu=new u.Unicorn(u.ARCH_X86,u.MODE_32);const c=this.cpu;
    this.image=new PEImage(bytes);this.hooks=[];this.heap=0x02000000;this.stack=0x07efff00;this.returnAddress=0x07f00000;
    this.hostMemory=u._malloc(0x07ff0000);this.wasmMemory=Object.values(this.wasmExports).find(v=>v instanceof WebAssembly.Memory);
    const status=u._uc_mem_map_ptr(u.getValue(c.handle_ptr,'*'),0x10000n,0x07ff0000n,u.PROT_ALL,this.hostMemory);if(status)throw new Error('uc_mem_map_ptr failed '+status);
    this.view(0x10000,0x07ff0000).fill(0);c.mem_write(this.image.base,bytes.subarray(0,this.image.headerSize));
    for(const s of this.image.sections)if(s.rawSize)c.mem_write(this.image.base+s.rva,bytes.subarray(s.raw,s.raw+s.rawSize));
    this.reg('ESP',this.stack);this.reg('EFLAGS',2);this.reg('FPCW',0x37f);
    // Flat 32-bit descriptors plus a real FS-based thread environment block.
    const gdt=new Uint8Array(32),gv=new DataView(gdt.buffer);
    function descriptor(index,base,type){const o=index*8;gv.setUint16(o,0xffff,true);gv.setUint16(o+2,base&0xffff,true);gdt[o+4]=(base>>>16)&255;gdt[o+5]=type;gdt[o+6]=0xcf;gdt[o+7]=base>>>24;}
    descriptor(1,0,0x9b);descriptor(2,0,0x93);descriptor(3,0x11000,0x93);this.write(0x10000,gdt);
    const gdtr=new Uint8Array(24),dv=new DataView(gdtr.buffer);dv.setBigUint64(8,0x10000n,true);dv.setUint32(16,31,true);c.reg_write(u.X86_REG_GDTR,gdtr);
    for(const [name,selector] of [['CS',8],['DS',16],['ES',16],['SS',16],['FS',24]])this.reg(name,selector);
    this.u32(0x11000,0xffffffff);this.u32(0x11018,0x11000);this.u32(0x11004,this.stack+256);this.u32(0x11008,this.stack-0x100000);
    this.hooks.push(c.hook_add(u.HOOK_MEM_UNMAPPED,(cpu,type,address,size)=>{this.fault={type,address:hex(address),size,pc:hex(this.reg('EIP')),stack:this.readWords(this.reg('ESP'),12).map(hex)};return false;}));
    this.importMap=new Map();this.stubAddress=0x07e00000;
    for(const entry of this.image.imports)this.u32(entry.address,this.registerImport(entry));
    this.hooks.push(c.hook_add(u.HOOK_CODE,(cpu,address)=>{
      const entry=this.importMap.get(Number(address));if(!entry)return;
      if(this.onImport)this.onImport(entry);else throw new Error('Unimplemented import '+entry.dll+'!'+entry.name+' return '+hex(this.u32(this.reg('ESP'))));
    },null,0x07e00000,0x07e0ffff));
  }
  registerImport(entry){const address=this.stubAddress;this.stubAddress+=16;this.importMap.set(address,entry);this.cpu.mem_write(address,[0xc3]);return address;}
  replace(address,name,handler,argc=0){const dest=this.registerImport({dll:'native-host',name,handler,argc});const jump=new Uint8Array(5);jump[0]=0xe9;new DataView(jump.buffer).setInt32(1,dest-address-5,true);this.write(address,jump);}
  reg(name,value){const id=this.uc['X86_REG_'+name];if(value===undefined)return this.cpu.reg_read_i32(id)>>>0;this.cpu.reg_write_i32(id,value|0);}
  resetThreadFPU(){this.reg('FPCW',0x27f);this.reg('FPSW',0);this.reg('FPTAG',0xffff);this.reg('MXCSR',0x1f80);}
  view(address,length){if(address<0x10000||length<0||address+length>0x08000000)throw new Error('Guest memory bounds '+hex(address)+' + '+length);return new Uint8Array(this.wasmMemory.buffer,this.hostMemory+address-0x10000,length);}
  bytes(address,length){return this.view(address,length).slice();}
  write(address,bytes){if(this.image.sections.some(s=>(s.flags&0x20000000)&&address>=this.image.base+s.rva&&address<this.image.base+s.rva+Math.max(s.rawSize,s.virtualSize)))this.cpu.mem_write(address,bytes);else this.view(address,bytes.length).set(bytes);}
  u32(address,value){const b=this.view(address,4),v=new DataView(b.buffer,b.byteOffset,4);if(value===undefined)return v.getUint32(0,true);v.setUint32(0,value>>>0,true);}
  i32(address,value){if(value===undefined)return this.u32(address)|0;this.u32(address,value);}
  f32(address,value){const b=this.view(address,4),v=new DataView(b.buffer,b.byteOffset,4);if(value===undefined)return v.getFloat32(0,true);v.setFloat32(0,value,true);}
  readWords(address,count){const b=this.bytes(address,count*4),v=new DataView(b.buffer);return Array.from({length:count},(_,i)=>v.getUint32(i*4,true));}
  allocate(size){size=Math.max(16,(size+15)&~15);const address=this.heap;this.heap+=size;if(this.heap>=0x07000000)throw new Error('Native heap exhausted');return address;}
  string(address,max=1024){const b=this.bytes(address,max);const n=b.indexOf(0);return new TextDecoder('windows-1252').decode(b.subarray(0,n<0?max:n));}
  ret(value=0,args=0){const sp=this.reg('ESP'),pc=this.u32(sp);this.reg('EAX',value);this.reg('ESP',sp+4+args*4);this.reg('EIP',pc);}
  call(address,{ecx=0,edx=0,args=[],limit=10_000_000}={}){
    let sp=this.stack;for(let i=args.length-1;i>=0;i--){sp-=4;this.u32(sp,args[i]);}sp-=4;this.u32(sp,this.returnAddress);
    this.reg('ESP',sp);this.reg('ECX',ecx);this.reg('EDX',edx);this.fault=null;
    try{this.cpu.emu_start(address,this.returnAddress,0,limit);}catch(error){throw new Error(String(error)+'\n'+JSON.stringify(this.fault??{pc:hex(this.reg('EIP')),stack:this.readWords(this.reg('ESP'),12).map(hex)}));}
    if(this.reg('EIP')!==this.returnAddress)throw new Error('Native instruction limit at '+hex(this.reg('EIP')));
    return this.reg('EAX');
  }
  close(){for(const hook of this.hooks)this.cpu.hook_del(hook);this.cpu.close();this.uc._free(this.hostMemory);}
}
