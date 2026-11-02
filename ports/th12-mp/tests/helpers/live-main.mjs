// These Player fixtures explicitly need EnemyManager+70=1, as prepared by
// their original provider. A waiting main supplies that real live world.
const bytes=Buffer.alloc(100);
bytes.write('SCPT');bytes.writeUInt16LE(1,4);bytes.writeUInt16LE(1,16);
bytes.writeUInt32LE(48,36);bytes.write('main\0',40);bytes.write('ECLH',48);
bytes.writeUInt16LE(12,68);bytes.writeUInt16LE(20,70);bytes[74]=63;bytes[75]=1;bytes.writeInt32LE(1000000,80);
bytes.writeUInt16LE(10,88);bytes.writeUInt16LE(16,90);bytes[94]=63;
export function loadLiveMain(core){
  const pointer=core._malloc(bytes.length);core.HEAPU8.set(bytes,pointer);
  try {if(core._th12_load_ecl(pointer,bytes.length)!==1)throw Error('Synthetic live main rejected');}
  finally {core._free(pointer);}
}
