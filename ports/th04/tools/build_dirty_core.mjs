// Experimental complete WASM write coverage. The bitmap is separate from the
// machine heap; over-marking is safe, omitting a live write is not.
import fs from 'node:fs';
import b from '../build/binaryen-132/package/index.js';
const root=new URL('../',import.meta.url);
const m=b.readBinary(fs.readFileSync(new URL(process.env.CORE_INPUT||'web/vendor/dosbox-x/wdosbox-x-rollback.wasm',root)));
m.setFeatures(b.Features.All);m.addMemoryImport('thDirty','env','thDirty');
const globals=Array.from({length:m.getNumGlobals()},(_,i)=>b.getGlobalInfo(m.getGlobalByIndex(i)));
const stackGlobal=globals[0].name,stackTop=b.getExpressionInfo(globals[0].init).value;
const staticPages=new Set(Array.from({length:Math.ceil(1048576/4096)},(_,i)=>(stackTop>>>12)-i));
const live='0',get=(i)=>m.local.get(i,b.i32),c=n=>m.i32.const(n);
const mark=(ptr,offset,bytes)=>m.call('thMark',[ptr,c(offset),c(bytes)],b.i32);
const bitmap=(address)=>m.i32.store8(0,1,m.i32.shr_u(address,c(12)),c(1),'thDirty');
m.addFunction('thMark',b.createType([b.i32,b.i32,b.i32]),b.i32,[],m.block(null,[
  bitmap(m.i32.add(get(0),get(1))),
  bitmap(m.i32.add(m.i32.add(get(0),get(1)),m.i32.sub(get(2),c(1)))),get(0)
],b.i32));
m.addFunction('thMarkRange',b.createType([b.i32,b.i32]),b.none,[b.i32,b.i32],m.block(null,[
  m.if(m.i32.eqz(get(1)),m.return()),
  m.local.set(2,m.i32.shr_u(get(0),c(12))),
  m.local.set(3,m.i32.shr_u(m.i32.add(get(0),m.i32.sub(get(1),c(1))),c(12))),
  m.loop('markNext',m.block(null,[m.i32.store8(0,1,get(2),c(1),'thDirty'),m.local.set(2,m.i32.add(get(2),c(1))),m.br('markNext',m.i32.le_u(get(2),get(3)))]))
]));
const functions=Array.from({length:m.getNumFunctions()-2},(_,i)=>b.getFunctionInfo(m.getFunctionByIndex(i))),stats={stores:0,staticStores:0,stackStores:0,copy:0,fill:0,init:0};
const childFields={Block:['children'],If:['condition','ifTrue','ifFalse'],Loop:['body'],Break:['condition','value'],Switch:['condition','value'],Call:['operands'],CallIndirect:['target','operands'],LocalGet:[],LocalSet:['value'],GlobalGet:[],GlobalSet:['value'],Load:['ptr'],Store:['ptr','value'],Const:[],Unary:['value'],Binary:['left','right'],Select:['ifTrue','ifFalse','condition'],Drop:['value'],Return:['value'],MemorySize:[],MemoryGrow:['delta'],Nop:[],Unreachable:[],Try:['body','catchBodies'],Throw:['operands'],Rethrow:[],Pop:[],RefNull:[],RefFunc:[],RefIsNull:['value'],RefEq:['left','right'],MemoryCopy:['dest','source','size'],MemoryFill:['dest','value','size'],MemoryInit:['dest','offset','size'],DataDrop:[]};
const names=new Map(Object.keys(childFields).map(name=>[b[name+'Id'],name]));
function walk(body,visit){const work=[body];while(work.length){const ref=work.pop(),id=b.getExpressionId(ref);if([b.UnreachableId,b.PopId,b.NopId].includes(id))continue;const info=b.getExpressionInfo(ref),kind=names.get(id);if(!kind)throw Error('Unhandled expression '+id);visit(ref,info,kind);for(const key of childFields[kind]){const value=info[key];if(Array.isArray(value))for(const child of value){if(child)work.push(child);}else if(value)work.push(value);}}}
function stackDerived(ref,locals){const id=b.getExpressionId(ref);if(![b.GlobalGetId,b.LocalGetId,b.LocalSetId,b.BinaryId].includes(id))return false;const info=b.getExpressionInfo(ref);if(id===b.GlobalGetId)return info.name===stackGlobal;if(id===b.LocalGetId)return locals.has(info.index);if(id===b.LocalSetId)return stackDerived(info.value,locals);if(id===b.BinaryId&&[b.AddInt32,b.SubInt32,b.AndInt32].includes(info.op))return stackDerived(info.left,locals)&&b.getExpressionId(info.right)===b.ConstId;return false;}
for(const f of functions){
 if(!f.body)continue;const extra=[],sets=new Map(),stackLocals=new Set();
 walk(f.body,(_,info,kind)=>{if(kind==='LocalSet'){if(!sets.has(info.index))sets.set(info.index,[]);sets.get(info.index).push(info.value);}});
 for(let changed=true;changed;){changed=false;for(const [index,values]of sets)if(!stackLocals.has(index)&&values.every(value=>stackDerived(value,stackLocals))){stackLocals.add(index);changed=true;}}
 const stack=[f.body];
 while(stack.length){
  const ref=stack.pop(),id=b.getExpressionId(ref);if([b.UnreachableId,b.PopId,b.NopId].includes(id))continue;
  let info;try{info=b.getExpressionInfo(ref);}catch{throw Error('Expression API missing '+id+' '+Object.keys(b).filter(key=>key.endsWith('Id')&&b[key]===id)+' in '+f.name);}
  const kind=names.get(id);if(!kind)throw Error('Unhandled write-capable expression '+id);
  for(const key of childFields[kind]){const value=info[key];if(Array.isArray(value))for(const child of value){if(child)stack.push(child);}else if(value)stack.push(value);}
  if(kind==='Store'){
   if(info.isAtomic)throw Error('atomic store');
   if(b.getExpressionId(info.ptr)===b.ConstId){const address=(b.getExpressionInfo(info.ptr).value+info.offset)>>>0;staticPages.add(address>>>12);staticPages.add((address+info.bytes-1)>>>12);stats.staticStores++;}
   else {b.Store.setPtr(ref,mark(info.ptr,info.offset,info.bytes));stats.stores++;}
  }
  if(['MemoryCopy','MemoryFill','MemoryInit'].includes(kind)){
   const index=b.expandType(f.params).length+f.vars.length+extra.length,other=kind==='MemoryCopy'?'source':kind==='MemoryFill'?'value':'offset';
   extra.push(b.i32,b.i32,b.i32);
   const pointer=m.block(null,[m.local.set(index,info.dest),m.local.set(index+1,info[other]),m.local.set(index+2,info.size),m.call('thMarkRange',[get(index),get(index+2)],b.none),get(index)],b.i32);
   b[kind].setDest(ref,pointer);b[kind]['set'+other[0].toUpperCase()+other.slice(1)](ref,get(index+1));b[kind].setSize(ref,get(index+2));
   stats[kind==='MemoryCopy'?'copy':kind==='MemoryFill'?'fill':'init']++;
  }
 }
 if(extra.length){m.removeFunction(f.name);m.addFunction(f.name,f.params,f.results,[...f.vars,...extra],f.body);}
}
if(!m.validate())throw Error('dirty core validation');
fs.writeFileSync(new URL(process.env.CORE_OUTPUT||'build/core-dirty-native.wasm',root),m.emitBinary());
fs.writeFileSync(new URL(process.env.CORE_PAGES_OUTPUT||'build/core-dirty-pages.json',root),JSON.stringify({stackTop,staticPages:[...staticPages].sort((a,b)=>a-b),stats}));
m.dispose();console.log({...stats,staticPages:staticPages.size,stackTop});
