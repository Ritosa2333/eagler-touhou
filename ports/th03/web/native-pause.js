import {unpackTouch, ALWAYS_POINT} from './touch-input.js';
import {createPauseSearch} from './pause-search.js';
export class NativePause {
  constructor(getEmulator, meta) {
    this.getEmulator = getEmulator;
    this.meta = meta;
    this.signature = new TextEncoder().encode(meta.signature);
    this.residentSignature = new TextEncoder().encode('YUMEConfig\0');
    this.reset();
  }
  reset() {
    this.cursor = 0; this.candidates = new Map(); this.residents = new Map();
    this.live = -1; this.pending = null; this.selecting = false;
  }
  valid(heap, at) {
    return at >= 0 && at + this.meta.mailboxSize <= heap.length
      && this.signature.every((byte, i) => heap[at + i] === byte)
      && heap[at + this.meta.fields.phase] <= 3;
  }
  update(now = performance.now(), scanRange = null) {
    const emulator = this.getEmulator(), heap = emulator?.module.HEAPU8;
    if (!heap) return null;
    const view = new DataView(heap.buffer, heap.byteOffset, heap.byteLength), f = this.meta.fields;
    const memory=emulator.module.nativeExports?.().memory;
    if(memory&&this.searchMemory!==memory){
      this.searchMemory=memory;this.search=createPauseSearch(memory,this.meta.signature);
    }
    if (emulator.state === 'running') {
      // Raw disk buffers also contain the signature. Only a changing heartbeat
      // with plausible relocated CS/DS can identify the running DOS allocation.
      if (this.live < 0 || !this.valid(heap, this.live)) {
        const scanStart=scanRange?.start??0,scanEnd=scanRange?.end??heap.length;
        if(this.cursor<scanStart||this.cursor>=scanEnd)this.cursor=scanStart;
        const end = Math.min(this.cursor + 2 * 1024 * 1024, scanEnd);
        const slice = heap.subarray(this.cursor, Math.min(end + this.signature.length - 1, heap.length));
        const find=(kind,start)=>this.search?this.search(kind,start,end):
          (()=>{const i=slice.indexOf(kind===0?this.signature[0]:this.residentSignature[0],start-this.cursor);return i<0?-1:this.cursor+i;})();
        for (let at = find(0,this.cursor); at >= 0 && at < end; at = find(0,at+1)) {
          if (this.valid(heap, at) && !this.candidates.has(at)) this.candidates.set(at, {ticks: view.getUint32(at + f.ticks, true), changed: -Infinity});
        }
        for (let at = find(1,this.cursor); at >= 0 && at < end; at = find(1,at+1)) {
          if (at + 58 <= heap.length && this.residentSignature.every((b, j) => heap[at + j] === b) && !this.residents.has(at)) {
            this.residents.set(at, {rand: view.getUint32(at + 0x10, true), changes: 0, changed: -Infinity});
          }
        }
        this.cursor = end === scanEnd ? scanStart : end;
      }
      const active = [];
      for (const [at, candidate] of this.candidates) {
        if (!this.valid(heap, at)) { this.candidates.delete(at); continue; }
        const ticks = view.getUint32(at + f.ticks, true), ds = view.getUint16(at + f.ds, true);
        if (ticks !== candidate.ticks) { candidate.changed = now; candidate.ticks = ticks; }
        const cs = ds - this.meta.dataSegment + this.meta.codeSegment;
        const ram = at - this.meta.mailboxCsOffset - cs * 16;
        if (ticks && cs > 0 && ram >= 0 && ram + 0x100000 <= heap.length && now - candidate.changed < 750) active.push(at);
      }
      this.live = active.length === 1 ? active[0] : -1;
      const selections = [];
      for (const [at, candidate] of this.residents) {
        if (at + 58 > heap.length || !this.residentSignature.every((b, j) => heap[at + j] === b)) { this.residents.delete(at); continue; }
        const rand = view.getUint32(at + 0x10, true);
        if (rand !== candidate.rand) {
          candidate.changes = now - candidate.changed < 400 ? candidate.changes + 1 : 1;
          candidate.changed = now; candidate.rand = rand;
        }
        // OP increments resident.rand every selection frame. MAINL reads it
        // without advancing it; disk buffers and inactive copies stay fixed.
        if (heap[at + 0x0b] <= 3 && !view.getUint16(at + 0x0e, true) && heap[at + 0x16] === 0
          && heap[at + 0x28] === 0x81 && !heap[at + 0x39] && candidate.changes >= 2
          && now - candidate.changed < 250) selections.push(at);
      }
      this.selecting = this.live < 0 && selections.length === 1;
    }
    if (!this.valid(heap, this.live)) return null;
    const at = this.live;
    return {at, phase: heap[at + f.phase], ack: heap[at + f.ack], command: heap[at + f.command],
      generation: view.getUint16(at + f.generation, true)};
  }
  request(command, seat, now=performance.now()) {
    const state = this.update(now);
    if (!state || state.phase !== 1 || state.command || this.pending || ![1, 2].includes(command) || ![0, 1].includes(seat)) return false;
    const heap = this.getEmulator().module.HEAPU8, f = this.meta.fields;
    this.getEmulator().module.thDirtyMark?.(state.at,this.meta.mailboxSize);
    heap[state.at + f.seat] = seat;
    heap[state.at + f.ack] = 0;
    heap[state.at + f.command] = command;
    this.pending = {at: state.at, generation: state.generation, started: now};
    return true;
  }
  setFocus(mask, points) {
    const emulator = this.getEmulator(), heap = emulator?.module.HEAPU8;
    if (!heap || !this.valid(heap, this.live) || heap[this.live + this.meta.fields.phase] !== 1) return;
    const at = this.live, f = this.meta.fields;
    emulator.module.thDirtyMark?.(at,this.meta.mailboxSize);
    heap[at + f.points] = 0; // Draw browser-local markers outside emulated VRAM.
    heap[at + f.focus] = emulator.state === 'running' && heap[at + f.phase] === 1 ? mask & 3 : 0;
  }
  setTouch(inputs) {
    const emulator=this.getEmulator(),heap=emulator?.module.HEAPU8,f=this.meta.fields;
    if(!heap||!this.valid(heap,this.live)||heap[this.live+f.phase]!==1||f.touch===undefined)return;
    const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength),at=this.live;
    emulator.module.thDirtyMark?.(at,this.meta.mailboxSize);
    const running=emulator.state==='running'&&heap[at+f.phase]===1;
    let points=0;
    for(let slot=0;slot<2;slot++){
      const value=inputs[slot]||0,touch=unpackTouch(value),base=at+f.touch+slot*6;
      if(Math.floor(value/ALWAYS_POINT)%2===1)points|=1<<slot;
      const data=at-this.meta.mailboxCsOffset+(this.meta.dataSegment-this.meta.codeSegment)*16;
      const player=data+0x65a6+slot*0x80;
      const blocked=heap[player+0x1f]||heap[player+0x11]||heap[player+0x15];
      if(!running||!touch.active||blocked){view.setUint16(base,0,true);view.setInt32(base+2,0,true);continue;}
      if(touch.begin)view.setInt32(base+2,0,true);
      view.setUint16(base,touch.unlimited?3:1,true);
      for(const [offset,delta] of [[2,touch.x],[4,touch.y]])
        view.setInt16(base+offset,Math.max(-8192,Math.min(8191,view.getInt16(base+offset,true)+delta)),true);
    }
    heap[at+f.alwaysPoint]=0;
  }
  markers() {
    const heap=this.getEmulator()?.module.HEAPU8,at=this.live,f=this.meta.fields;
    if(!heap||!this.valid(heap,at)||heap[at+f.phase]!==1)return [];
    const data=at-this.meta.mailboxCsOffset+(this.meta.dataSegment-this.meta.codeSegment)*16;
    const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength);
    return [0,1].flatMap(seat=>{
      const p=data+0x65a6+seat*0x80;
      if(heap[p+0x1f])return [];
      return [{seat,focused:!!(heap[at+f.generation+2]&(1<<seat)),
        x:(view.getInt16(p,true)>>4)+view.getInt16(data+0x659c+seat*2,true)+16+seat*320,
        y:((view.getInt16(p+2,true)>>5)+8)*2}];
    });
  }
  result(now=performance.now()) {
    if (!this.pending) return null;
    const {at, generation, started} = this.pending, heap = this.getEmulator()?.module.HEAPU8, f = this.meta.fields;
    if (!heap || !this.valid(heap, at)) { this.pending = null; return 'unknown'; }
    this.getEmulator().module.thDirtyMark?.(at,this.meta.mailboxSize);
    const view = new DataView(heap.buffer, heap.byteOffset, heap.byteLength);
    const ack = heap[at + f.ack];
    if (!heap[at + f.command] && (ack === 1 || ack === 2)) {
      // Release the native exit wait only after capturing the receipt.
      heap[at + f.ack] = 3;
      this.pending = null; return ack === 1 ? 'accepted' : 'rejected';
    }
    if (view.getUint16(at + f.generation, true) !== generation || now - started > 4000) {
      heap[at + f.command] = 0; this.pending = null; return 'unknown';
    }
    return 'pending';
  }
  capture() {
    return {cursor:this.cursor, candidates:[...this.candidates].map(([key,value])=>[key,{...value}]), residents:[...this.residents].map(([key,value])=>[key,{...value}]), live:this.live, pending:this.pending?{...this.pending}:null, selecting:this.selecting};
  }
  restore(saved) {
    if(!saved)return;
    this.cursor=saved.cursor;this.candidates=new Map(saved.candidates);this.residents=new Map(saved.residents);
    this.live=saved.live;this.pending=saved.pending?{...saved.pending}:null;this.selecting=saved.selecting;
  }
}
