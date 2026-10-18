// Original D3DX pixel conversion with an in-memory surface test double.
export function textureOracle(m,lib){
 const importArgument=n=>m.u32(m.reg('ESP')+n*4);
 for(const entry of m.importMap.values()){
  if(entry.name==='GetModuleHandleA'){entry.handler=()=>0;entry.argc=1;}
  if(entry.name==='GetProcessHeap'){entry.handler=()=>1;entry.argc=0;}
  if(entry.name==='HeapAlloc'){entry.handler=()=>m.allocate(importArgument(3));entry.argc=3;}
  if(entry.name==='HeapFree'){entry.handler=()=>1;entry.argc=3;}
  if(entry.name==='RegOpenKeyA'){entry.handler=()=>2;entry.argc=3;}
  if(entry.name==='OutputDebugStringA'){entry.handler=()=>0;entry.argc=1;}
  if(['??2@YAPAXI@Z','malloc'].includes(entry.name)){entry.handler=()=>m.allocate(importArgument(1));entry.argc=0;}
  if(['??3@YAXPAX@Z','free'].includes(entry.name)){entry.handler=()=>0;entry.argc=0;}
  if(entry.name==='memcpy'){entry.handler=()=>{const p=importArgument(1);m.write(p,m.bytes(importArgument(2),importArgument(3)));return p;};entry.argc=0;}
  if(entry.name==='memset'){entry.handler=()=>{const p=importArgument(1);m.view(p,importArgument(3)).fill(importArgument(2)&255);return p;};entry.argc=0;}
 }
 const vtable=m.allocate(0x50),surface=m.allocate(4),desc=m.allocate(32),lock=m.allocate(8),rect=m.allocate(16),targetRect=m.allocate(16),dest=m.allocate(1024*1024),source=m.allocate(1024*1024);m.u32(surface,vtable);
 let width=0,height=0,format=21,pitch=0;
 const arg=n=>m.u32(m.reg('ESP')+n*4),hook=(offset,name,argc,handler)=>m.u32(vtable+offset,m.registerImport({dll:'surface-oracle',name,argc,handler}));
 hook(0x30,'GetDesc',2,()=>{const p=arg(2);m.view(p,32).fill(0);m.u32(p,format);m.u32(p+4,1);m.u32(p+24,width);m.u32(p+28,height);return 0;});
 hook(0x34,'LockRect',4,()=>{const p=arg(2),r=arg(3),x=r?m.i32(r):0,y=r?m.i32(r+4):0;m.u32(p,pitch);m.u32(p+4,dest+y*pitch+x*(pitch/width));return 0;});
 hook(0x38,'UnlockRect',1,()=>0);hook(4,'AddRef',1,()=>2);hook(8,'Release',1,()=>1);
 return {convert(bytes,w,h,input,output,bppIn,bppOut,tw=w,th=h,filter=1){width=tw;height=th;format=output;pitch=tw*bppOut;m.view(dest,pitch*th).fill(0);m.write(source,bytes);[0,0,w,h].forEach((n,i)=>m.u32(rect+i*4,n));[0,0,tw,th].forEach((n,i)=>m.u32(targetRect+i*4,n));const result=m.call(lib.exports.get('D3DXLoadSurfaceFromMemory'),{args:[surface,0,targetRect,source,input,w*bppIn,0,rect,filter,0],limit:100000000});if(result!==0)throw Error('D3DX conversion failed '+result.toString(16));return Buffer.from(m.bytes(dest,pitch*th));}};
}
