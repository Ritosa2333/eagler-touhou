// The development machine's default bump heap is unsuitable for long stages.
// Recycle only allocations observed at the original malloc/free boundary.
export function recycleOriginalAllocations(m) {
  const bump=m.allocate.bind(m),sizes=new Map(),freeLists=new Map();
  m.allocate=size=>{size=Math.max(16,(size+15)&~15);const available=freeLists.get(size),p=available?.length?available.pop():bump(size);if(sizes.has(p))throw Error('Original allocation is still live');sizes.set(p,size);return p;};
  const free=p=>{const size=sizes.get(p);if(size===undefined)return;sizes.delete(p);if(!freeLists.has(size))freeLists.set(size,[]);freeLists.get(size).push(p);};
  for(const address of [0x4903f0,0x4904c4,0x491b88])m.replace(address,'original allocator free boundary',()=>{free(m.u32(m.reg('ESP')+4));return 0;});
}
