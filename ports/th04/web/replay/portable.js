// Browser-only, cross-instance state. No guest hook and no executable payload.
// Function/prototype references resolve exclusively against the destination's
// trusted boot graph; the checkpoint cannot supply code.
export const CHECKPOINT_VERSION='th04-browser-checkpoint/1';
const MAX_RAW=192*1024*1024,MAX_JSON=8*1024*1024;
export function createPortableCheckpoint(mod,ci){
  const functions=[],functionIds=new Map(),prototypes=[null,Object.prototype,Array.prototype],seen=new Set();
  function register(value){
    if(typeof value==='function'){if(!functionIds.has(value)){functionIds.set(value,functions.length);functions.push(value);}return;}
    if(!value||typeof value!=='object'||seen.has(value)||ArrayBuffer.isView(value)||value instanceof ArrayBuffer||value instanceof Date)return;
    seen.add(value);if(seen.size>20000)throw Error('检查点宿主图过大');
    const proto=Object.getPrototypeOf(value);if(!prototypes.includes(proto))prototypes.push(proto);
    for(const key of Object.keys(value).sort()){const d=Object.getOwnPropertyDescriptor(value,key);if(d&&'value'in d)register(d.value);}
  }
  register(mod.MEMFS);register(mod.TTY);register(mod.FS);
  const signature=functions.map(fn=>Function.prototype.toString.call(fn)).join('\n');
  const globals=Object.entries(mod.nativeExports()).filter(([name,value])=>name.startsWith('__rollback_g')&&value instanceof WebAssembly.Global);
  function graph(root){
    const nodes=[],ids=new Map(),buffers=[];
    function value(v){
      if(v===undefined)return {u:1};
      if(typeof v==='function'){const id=functionIds.get(v);if(id===undefined)throw Error('检查点出现未知宿主函数');return {f:id};}
      if(v===null||typeof v!=='object'){if(typeof v==='number'&&!Number.isFinite(v))return {n:String(v)};return v;}
      if(ids.has(v))return {r:ids.get(v)};
      const id=nodes.length;ids.set(v,id);nodes.push(null);if(id>20000)throw Error('检查点对象过多');
      if(v instanceof Date)nodes[id]={date:v.getTime()};
      else if(ArrayBuffer.isView(v)){const bytes=new Uint8Array(v.buffer,v.byteOffset,v.byteLength).slice();nodes[id]={typed:v.constructor.name,b:buffers.length};buffers.push(bytes);}
      else if(v instanceof ArrayBuffer){nodes[id]={typed:'ArrayBuffer',b:buffers.length};buffers.push(new Uint8Array(v).slice());}
      else{
        const proto=prototypes.indexOf(Object.getPrototypeOf(v));if(proto<0)throw Error('检查点出现未知宿主原型');
        const props=[];for(const key of Object.keys(v)){const d=Object.getOwnPropertyDescriptor(v,key);if(!d||!('value'in d))throw Error('检查点不接受动态宿主属性');props.push([key,value(d.value)]);}
        nodes[id]={proto,array:Array.isArray(v)?v.length:null,props};
      }
      return {r:id};
    }
    return {root:value(root),nodes,buffers};
  }
  function decode(g,buffers){
    if(!Array.isArray(g.nodes)||g.nodes.length>20001)throw Error('检查点宿主对象无效');
    const typed={Uint8Array,Int8Array,Uint16Array,Int16Array,Uint32Array,Int32Array,Float32Array,Float64Array,Uint8ClampedArray,DataView};
    const nodes=g.nodes.map(n=>{
      if(n.date!==undefined)return new Date(n.date);
      if(n.typed){const b=buffers[n.b];if(!b)throw Error('检查点字节引用无效');const data=b.slice().buffer;if(n.typed==='ArrayBuffer')return data;if(!typed[n.typed])throw Error('检查点字节类型无效');return new typed[n.typed](data);}
      if(!Number.isInteger(n.proto)||n.proto<0||n.proto>=prototypes.length||!Array.isArray(n.props)||n.array!==null&&(!Number.isInteger(n.array)||n.array<0||n.array>1000000))throw Error('检查点原型无效');
      return n.array===null?Object.create(prototypes[n.proto]):new Array(n.array);
    });
    function value(v){
      if(v===null||typeof v!=='object')return v;
      if(Object.hasOwn(v,'u'))return undefined;
      if(Object.hasOwn(v,'f')){if(!Number.isInteger(v.f)||v.f<0||v.f>=functions.length)throw Error('检查点函数引用无效');return functions[v.f];}
      if(Object.hasOwn(v,'n'))return Number(v.n);
      if(!Number.isInteger(v.r)||v.r<0||v.r>=nodes.length)throw Error('检查点对象引用无效');return nodes[v.r];
    }
    g.nodes.forEach((n,i)=>{if(n.props)for(const [key,v]of n.props){if(typeof key!=='string')throw Error('检查点属性无效');Object.defineProperty(nodes[i],key,{value:value(v),writable:true,configurable:true,enumerable:true});}});
    return value(g.root);
  }
  return {
    capture(host){
      const continuation={...mod.captureContinuation()};delete continuation.pendingWake;
      const fs=mod.FS,root={root:fs.root,streams:fs.streams,nameTable:fs.nameTable,nextInode:fs.nextInode,currentPath:fs.currentPath,devices:fs.devices,mounts:fs.mounts,ttys:mod.TTY.ttys,keyMatrix:ci.keyMatrix};
      const g=graph(root),heap=mod.HEAPU8.slice();
      return {version:CHECKPOINT_VERSION,signature,globals:globals.map(([name,g])=>[name,typeof g.value==='bigint'?String(g.value):g.value]),continuation,host,
        presentation:mod.framePresenter.capture(),graph:{root:g.root,nodes:g.nodes},buffers:[heap,...g.buffers]};
    },
    restore(s){
      if(s.version!==CHECKPOINT_VERSION||s.signature!==signature||s.buffers[0]?.length!==mod.HEAPU8.length||s.globals.length!==globals.length)throw Error('检查点核心／宿主身份不一致');
      const root=decode(s.graph,s.buffers.slice(1)),wake=mod.pendingWake;
      if(typeof wake!=='function')throw Error('目标核心不在挂起边界');
      for(let i=0;i<globals.length;i++)if(s.globals[i][0]!==globals[i][0])throw Error('检查点全局状态不匹配');
      mod.HEAPU8.set(s.buffers[0]);globals.forEach(([name,g],i)=>{g.value=typeof g.value==='bigint'?BigInt(s.globals[i][1]):s.globals[i][1];});
      for(const key of ['root','streams','nameTable','nextInode','currentPath','devices','mounts'])if(root[key]!==undefined)mod.FS[key]=root[key];
      mod.TTY.ttys=root.ttys;ci.keyMatrix=root.keyMatrix;
      mod.restoreContinuation({...s.continuation,pendingWake:wake});mod.framePresenter.restore(s.presentation);
      return s.host;
    },
  };
}
export async function encodeCheckpoint(state,base=null){
  const {buffers:original,...meta}=state,xor=[];
  const buffers=original.map((b,i)=>{
    if(!base||base.buffers[i]?.length!==b.length){xor.push(false);return b;}
    xor.push(true);const bytes=new Uint8Array(b.length),previous=base.buffers[i];
    for(let n=0;n<b.length;n++)bytes[n]=b[n]^previous[n];return bytes;
  });
  const header=new TextEncoder().encode(JSON.stringify({...meta,delta:!!base,xor,lengths:buffers.map(b=>b.length)}));
  const total=4+header.length+buffers.reduce((n,b)=>n+b.length,0);
  if(header.length>MAX_JSON||total>MAX_RAW)throw Error('检查点超过容量');
  const prefix=new Uint8Array(4);new DataView(prefix.buffer).setUint32(0,header.length,true);
  return new Uint8Array(await new Response(new Blob([prefix,header,...buffers]).stream().pipeThrough(new CompressionStream('gzip'))).arrayBuffer());
}
export async function decodeCheckpoint(bytes,base=null){
  if(!(bytes instanceof Uint8Array)||bytes.length>16*1024*1024)throw Error('检查点压缩数据无效');
  const reader=new Blob([bytes]).stream().pipeThrough(new DecompressionStream('gzip')).getReader(),parts=[];let size=0;
  for(;;){const {done,value}=await reader.read();if(done)break;size+=value.length;if(size>MAX_RAW){await reader.cancel();throw Error('检查点解压越界');}parts.push(value);}
  const raw=new Uint8Array(await new Blob(parts).arrayBuffer());if(raw.length<4)throw Error('检查点数据不完整');
  const length=new DataView(raw.buffer).getUint32(0,true);if(length>MAX_JSON||4+length>raw.length)throw Error('检查点文件头无效');
  const meta=JSON.parse(new TextDecoder().decode(raw.subarray(4,4+length)));let at=4+length;
  if(!Array.isArray(meta.lengths)||meta.lengths.length>10000)throw Error('检查点字节索引无效');
  meta.buffers=meta.lengths.map(n=>{if(!Number.isInteger(n)||n<0||at+n>raw.length)throw Error('检查点字节范围无效');const b=raw.subarray(at,at+n);at+=n;return b;});
  if(meta.delta){
    if(!base||base.delta||!Array.isArray(meta.xor)||meta.xor.length!==meta.buffers.length)throw Error('检查点基准缺失');
    meta.buffers=meta.buffers.map((b,i)=>{if(!meta.xor[i])return b;const previous=base.buffers[i];if(previous?.length!==b.length)throw Error('检查点基准长度不符');const bytes=new Uint8Array(b.length);for(let n=0;n<b.length;n++)bytes[n]=b[n]^previous[n];return bytes;});
  }
  if(at!==raw.length)throw Error('检查点尾部无效');return meta;
}
