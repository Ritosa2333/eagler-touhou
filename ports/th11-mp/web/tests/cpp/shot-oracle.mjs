export function normalizeAnimation(bytes,read,base){const b=Buffer.from(bytes);
 for(const off of [0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0])b.writeUInt32LE(0,off);
 for(const off of [4,16,0x1c]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(read(p),off);}
 for(const off of [8,12,20,24]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(read(read(p)),off);}
 for(const off of [0x394,0x3a4,0x3a8]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE((p-base)>>>0,off);}
 for(const off of [0x3ac,0x400,0x410,0x428,0x42c])b.writeUInt32LE(b.readUInt32LE(off)?1:0,off);
 b.writeUInt32LE(0,0x414);return b;
}
export function firstDifference(a,b){for(let off=0;off<a.length;off+=4)if(a.readUInt32LE(off)!==b.readUInt32LE(off))return `0x${off.toString(16)} actual=0x${a.readUInt32LE(off).toString(16)} expected=0x${b.readUInt32LE(off).toString(16)}`;return '';}
