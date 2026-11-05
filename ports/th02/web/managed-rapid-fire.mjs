// Replace the native shot-key test with a game-frame-paced pulse. The CALL
// preserves the flags expected by the original conditional jump.
export function installRapidFire(exe, patch, multiplayer) {
  const base=patch.headerSize+patch.codeSegment*16, hook=patch.headerSize+0xf31c;
  const target=multiplayer?0x8930:0x4fb5, frame=patch.entries.mailbox+16;
  const lo=n=>n&255, hi=n=>n>>8&255;
  const testShot=[0xf6,0x06,0x38,0x25,0x10,0xc3];
  const mode=patch.entries.mailbox+22;
  const code=[...(multiplayer?[0x2e,0xf6,0x06,0x39,0x85,0x02,0x74,0x0e]
    :[0x2e,0x80,0x3e,lo(mode),hi(mode),0x01,0x75,0x0e]),
    0x2e,0xf6,0x06,lo(frame),hi(frame),0x01,0x74,0x06,
    0xf6,0x06,0x38,0x25,0x00,0xc3,...testShot];
  const expected=[0xf6,0x06,0x38,0x25,0x10];
  if(!expected.every((b,i)=>exe[hook+i]===b)||!code.every((_,i)=>exe[base+target+i]===0))throw Error('TH02 连射补丁位置不匹配');
  if(multiplayer && (exe[base+patch.entries.update_one+2]!==0x2e||exe[base+patch.entries.update_one+3]!==0xa3||exe[base+patch.entries.update_one+4]!==0x38||exe[base+patch.entries.update_one+5]!==0x85))throw Error('TH02 连射输入位置不匹配');
  const displacement=target-(0xf31c-patch.codeSegment*16+3);
  exe.set([0xe8,lo(displacement),hi(displacement),0x90,0x90],hook);
  exe.set(code,base+target);
}
