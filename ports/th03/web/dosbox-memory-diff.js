export async function makeMemoryDiff(module){
  // Storage only: keep undo data in helper memory, leaving native execution
  // unchanged. Engines without multi-memory retain full-copy snapshots.
  try{
    const tracked=!!module.thDirty;
    const response=await fetch(new URL(tracked?'./vendor/dosbox-x/snapshot-diff-tracked.wasm':'./vendor/dosbox-x/snapshot-diff.wasm',import.meta.url));
    if(!response.ok)throw Error('snapshot diff missing');
    const {instance}=await WebAssembly.instantiate(await response.arrayBuffer(),{env:{memory:module.nativeExports().memory,...(tracked?{thDirty:module.thDirty}:{})}});
    const api=instance.exports,pageBytes=4096,directoryBytes=262144,free=[],live=new Set();
    let size=0,end=0,restoreSeen=new Uint8Array(0);
    if(!api.packPages||!api.restorePages)throw Error('snapshot helper version mismatch');
    function reserve(needed){
      if(api.shadow.buffer.byteLength<needed)api.shadow.grow(Math.ceil((needed-api.shadow.buffer.byteLength)/65536));
    }
    function resize(){
      const next=module.HEAPU8.length;
      if(size===next)return;
      const base=directoryBytes+next;
      if(live.size){
        // Move every retained undo block out of the growing native shadow
        // before extend() writes the new pages. Store offsets, never views
        // that become detached when the helper memory grows.
        let at=Math.max(base,end);
        reserve(at+[...live].reduce((n,b)=>n+b.length,0));
        const bytes=new Uint8Array(api.shadow.buffer);
        for(const block of live){bytes.copyWithin(at,block.offset,block.offset+block.length);block.offset=at;at+=block.length;}
        end=at;
      }else{end=base;reserve(base);}
      free.length=0;
      if(size)api.extend(size,next);else api.init(next);
      size=next;
    }
    function allocate(length){
      const index=free.findIndex(span=>span.length>=length);
      if(index>=0){
        const span=free[index],offset=span.offset;span.offset+=length;span.length-=length;
        if(!span.length)free.splice(index,1);return offset;
      }
      const offset=end;reserve(end+length);end+=length;return offset;
    }
    function release(block){
      if(!live.delete(block))return;
      free.push({offset:block.offset,length:block.length});free.sort((a,b)=>a.offset-b.offset);
      for(let i=free.length-1;i>0;i--)if(free[i-1].offset+free[i-1].length===free[i].offset){free[i-1].length+=free[i].length;free.splice(i,1);}
      while(free.length&&free.at(-1).offset+free.at(-1).length===end){end=free.at(-1).offset;free.pop();}
    }
    function validate(block){
      if(!live.has(block))throw Error('Snapshot undo block already released');
      if(!(block.pages instanceof Uint32Array)||block.length!==block.pages.length*pageBytes)throw Error('Invalid undo block');
      let previous=-1;
      for(const page of block.pages){if(page<=previous||page>=size/pageBytes)throw Error('Invalid undo page');previous=page;}
    }
    return {begin:resize,
      changes(){if(tracked){const dirty=new Uint8Array(module.thDirty.buffer);for(const page of module.thDirtyStatic)dirty[page]=1;}const count=api.scan(module.HEAPU8.length>>>12);return new Uint32Array(api.shadow.buffer,0,count).slice();},
      pack(pages){
        if(!pages.length)return null;
        const length=pages.length*pageBytes,offset=allocate(length);
        api.packPages(pages.length,offset);
        const block={pages,offset,length};live.add(block);return block;
      },
      restoreBlock(block){
        validate(block);
        new Uint32Array(api.shadow.buffer,0,block.pages.length).set(block.pages);
        api.restorePages(block.pages.length,block.offset);
      },
      // Input order matches the old undo walk: newest block first. For each
      // page only the oldest retained preimage contributes to the final state.
      restoreBlocks(blocks){
        for(const block of blocks)validate(block);
        const pagesInHeap=size>>>12;
        if(restoreSeen.length!==pagesInHeap)restoreSeen=new Uint8Array(pagesInHeap);
        else restoreSeen.fill(0);
        for(let b=blocks.length-1;b>=0;b--){
          const block=blocks[b],pages=block.pages;
          let index=0;
          while(index<pages.length){
            while(index<pages.length&&restoreSeen[pages[index]])index++;
            const first=index;
            while(index<pages.length&&!restoreSeen[pages[index]])restoreSeen[pages[index++]]=1;
            if(index===first)continue;
            // Kept indices form a contiguous range in this packed source.
            // Reacquire views after any earlier arena memory.grow.
            new Uint32Array(api.shadow.buffer,0,index-first).set(pages.subarray(first,index));
            api.restorePages(index-first,block.offset+first*pageBytes);
          }
        }
      },
      release,
      old(page){return new Uint8Array(api.shadow.buffer,directoryBytes+(page<<12),pageBytes).slice();},
      sync(page){api.copyPage(page);},
      restore(page,bytes){module.HEAPU8.set(bytes,page<<12);new Uint8Array(api.shadow.buffer,directoryBytes+(page<<12),pageBytes).set(bytes);},
      pageBytes,get bytes(){return api.shadow.buffer.byteLength;}};
  }catch{return null;}
}
