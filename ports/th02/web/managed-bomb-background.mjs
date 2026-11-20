// The native tile renderer owns both VRAM pages. A Bomb can dirty the whole
// playfield, so request a complete tile repaint on the following four frames.
export function installBombBackgroundRepair(exe, patch) {
  const base=patch.headerSize+patch.codeSegment*16, hook=patch.entries.render_hook+19, target=0x8a80;
  const lo=n=>n&255, hi=n=>n>>8&255, counter=target+36;
  const call=patch.entries.bomb_art_default-(target+17);
  const body=[0x80,0x3e,0x44,0x3e,0x00,0x74,0x07,
    0x2e,0xc6,0x06,lo(counter),hi(counter),0x04,0xc3,
    0xe8,lo(call),hi(call),0x2e,0x80,0x3e,lo(counter),hi(counter),0x00,0x74,0x0a,
    0x2e,0xfe,0x0e,lo(counter),hi(counter),0xc6,0x06,0xd8,0x52,0x01,0xc3,0x00];
  const expected=[0x80,0x3e,0x44,0x3e,0x00,0x75,0x03,0xe8,0x51,0x2f];
  if(!expected.every((b,i)=>exe[base+hook+i]===b)||!body.every((_,i)=>exe[base+target+i]===0))throw Error('TH02 Bomb 背景补丁位置不匹配');
  const jump=target-(hook+3);
  exe.set([0xe8,lo(jump),hi(jump),0x90,0x90,0x90,0x90,0x90,0x90,0x90],base+hook);
  exe.set(body,base+target);
}

// MAIN loads BOMBS.BFT once before the first stage. The co-op selector frees
// only the outgoing shot type's PI/BFT, so it must not run the shared loader
// again: each reload would strand another 0x438 bytes of conventional memory.
export function installBombResourceReuse(exe, patch) {
  const base=patch.headerSize+patch.codeSegment*16;
  const call=patch.entries.bomb_art_select+0x33, thunk=0x8aa8, loadShotArt=0x3051;
  const relative=(target,next)=>[(target-next)&255,(target-next)>>8&255];
  const original=[0xe8,...relative(0x3018,call+3)];
  const shotArtEntry=[0xc4,0x1e,0xfc,0x27]; // les bx, [resident]
  const body=[0x55,0x8b,0xec,0xe9,...relative(loadShotArt,thunk+6)]; // native prologue + tail jump
  if(!original.every((byte,i)=>exe[base+call+i]===byte)||
     !shotArtEntry.every((byte,i)=>exe[base+loadShotArt+i]===byte)||
     !body.every((_,i)=>exe[base+thunk+i]===0))throw Error('TH02 Bomb 素材切换补丁位置不匹配');
  exe.set(body,base+thunk);
  exe.set([0xe8,...relative(thunk,call+3)],base+call);
}

// Co-op can switch to the largest Bomb PI while stage tiles and wave data are
// already resident. The original 18,000-paragraph arena can't fit that PI's
// decoder and output together in stage 4. Its failed load leaves the decoder
// workspace allocated, so subsequent native stage loads fail as well.
export function installCoopAssetHeap(exe, patch) {
  const at=patch.headerSize+patch.codeSegment*16+0x47b6;
  const original=[0x68,0x50,0x46,0x9a,0x10,0x29]; // push 0x4650; call far hmem_assign
  if(!original.every((byte,i)=>exe[at+i]===byte))throw Error('TH02 联机素材内存补丁位置不匹配');
  exe[at+2]=0x56; // add 0x1000 paragraphs (64 KiB) within conventional DOS memory
}
