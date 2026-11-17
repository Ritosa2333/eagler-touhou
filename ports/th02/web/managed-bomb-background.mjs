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
