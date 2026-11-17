// Pinned Normal interpreter superinstruction for TH04's RAM-only VSync wait.
// Keep the exact instruction budget and event boundaries; no sleeps are removed.
// Each omitted triplet is MOV AX,[2ab2]; CMP AX,[5390]; JB -9.
// At least two complete iterations still execute, reproducing CPU/decoder scratch.
import fs from 'node:fs';import {createHash} from 'node:crypto';import b from '../build/binaryen-132/package/index.js';
const root=new URL('../',import.meta.url),input=process.argv[2]||'web/vendor/dosbox-x/wdosbox-x-rollback.wasm',output=process.argv[3]||'build/core-idle-native.wasm';
const bytes=fs.readFileSync(new URL(input,root)),hash=createHash('sha256').update(bytes).digest('hex');
if(!new Set(['23f41f943f4a7832d850e66aba0fe64d25e5538caf6121e6978dd755968d0c71','bc7fd56d753667dd11026328c70c86367e905266c157119feca1aea740d76ca1']).has(hash))throw Error('Unaudited input core; do not apply twice');
const m=b.readBinary(bytes);m.setFeatures(b.Features.All);
const c=n=>m.i32.const(n),get=n=>m.local.get(n,b.i32),load=at=>m.i32.load(0,4,c(at),'0'),store=(at,value)=>m.i32.store(0,4,c(at),value,'0');
const read16=ptr=>m.i32.load16_u(0,2,ptr,'0'),read8=at=>m.i32.load8_u(0,1,typeof at==='number'?c(at):at,'0');
const tlb=ptr=>m.i32.load(0,4,m.i32.add(c(6765572),m.i32.shl(m.i32.shr_u(ptr,c(12)),c(2))),'0');
const stop=condition=>m.if(condition,m.return(get(0)));
m.addFunction('thIdleTriplets',b.i32,b.i32,Array(7).fill(b.i32),m.block(null,[
 m.local.set(1,load(28264484)),stop(m.i32.le_s(get(1),c(9))),
 stop(m.i32.or(read8(28264216),read8(28264272))), // cpu.pmode, cpu.code.big
 stop(m.i32.and(load(28264196),c(0x20100))), // VM86 or single-step
 m.local.set(2,tlb(c(113269))),stop(m.i32.eqz(get(2))),
 m.local.set(2,m.i32.add(get(2),c(113269))),
 stop(m.i32.gt_u(get(2),m.i32.sub(m.i32.shl(m.memory.size('0'),c(16)),c(16)))),
 stop(m.i32.ne(m.i32.load(0,1,get(2),'0'),c(0x3b2ab2a1))),
 stop(m.i32.ne(m.i32.load(4,1,get(2),'0'),c(0x72539006))),
 stop(m.i32.ne(m.i32.load8_u(8,1,get(2),'0'),c(0xf7))),
 m.local.set(3,load(28264412)),stop(m.i32.gt_u(get(3),c(0x90000))), // DS physical base
 m.local.set(4,m.i32.add(get(3),c(0x2ab2))),m.local.set(5,m.i32.add(get(3),c(0x5390))),
 m.local.set(6,tlb(get(4))),stop(m.i32.eqz(get(6))),m.local.set(4,m.i32.add(get(4),get(6))),
 m.local.set(6,tlb(get(5))),stop(m.i32.eqz(get(6))),m.local.set(5,m.i32.add(get(5),get(6))),
 stop(m.i32.gt_u(get(4),m.i32.sub(m.i32.shl(m.memory.size('0'),c(16)),c(2)))),
 stop(m.i32.gt_u(get(5),m.i32.sub(m.i32.shl(m.memory.size('0'),c(16)),c(2)))),
 stop(m.i32.ge_u(read16(get(4)),read16(get(5)))), // loop must still be waiting
 m.local.set(7,m.i32.mul(m.i32.div_u(m.i32.sub(get(1),c(6)),c(3)),c(3))),
 store(28264484,m.i32.sub(get(1),get(7))),m.i32.add(get(0),get(7))
],b.i32));
const cpuName=b.getElementSegmentInfo(m.getElementSegmentByIndex(0)).data[2301];
const f=b.getFunctionInfo(m.getFunction(cpuName)),schema={Block:['children'],If:['condition','ifTrue','ifFalse'],Loop:['body'],Break:['condition','value'],Switch:['condition','value'],Call:['operands'],CallIndirect:['target','operands'],LocalGet:[],LocalSet:['value'],GlobalGet:[],GlobalSet:['value'],Load:['ptr'],Store:['ptr','value'],Const:[],Unary:['value'],Binary:['left','right'],Select:['ifTrue','ifFalse','condition'],Drop:['value'],Return:['value'],MemorySize:[],MemoryGrow:['delta'],Nop:[],Unreachable:[],Try:['body','catchBodies'],Throw:['operands'],Rethrow:[],Pop:[],MemoryCopy:['dest','source','size'],MemoryFill:['dest','value','size'],MemoryInit:['dest','offset','size'],DataDrop:[]};
const fields=new Map(Object.entries(schema).map(([name,value])=>[b[name+'Id'],value]));let count=0;
const work=[f.body];while(work.length){const ref=work.pop(),id=b.getExpressionId(ref);if([b.UnreachableId,b.PopId,b.NopId].includes(id))continue;const info=b.getExpressionInfo(ref);if(!fields.has(id))throw Error('Unclassified instruction '+id);
 for(const key of fields.get(id)){const v=info[key];if(Array.isArray(v)){for(const x of v)if(x)work.push(x);}else if(v)work.push(v);}
 if(id===b.StoreId&&b.getExpressionId(info.ptr)===b.ConstId&&(b.getExpressionInfo(info.ptr).value+info.offset)===6764168){
  b.Store.setValue(ref,m.if(m.i32.eq(load(6764208),c(113269)),m.call('thIdleTriplets',[info.value],b.i32),m.copyExpression(info.value)));count++;
 }
}
if(count!==1)throw Error('Instruction counter anchor changed '+count);if(!m.validate())throw Error('Idle core invalid');
fs.writeFileSync(new URL(output,root),m.emitBinary());m.dispose();console.log('Built VSync idle triplets candidate',output);
