// Original TH03 YUME.NEM: four encrypted 206-byte difficulty sections.
const sectionSize = 206, key1Offset = 204, key2Offset = 205, clearOffset = 82;
const rotateRight3 = byte => ((byte >>> 3) | (byte << 5)) & 0xff;

export function decodeScoreSection(bytes) {
  if (bytes.length !== sectionSize) throw Error('梦时空分数区段长度不匹配');
  const section = Uint8Array.from(bytes), key1 = section[key1Offset], key2 = section[key2Offset];
  for (let i = 0; i < key1Offset - 1; i++) {
    section[i] = section[i] + key1 + (rotateRight3(section[i + 1]) ^ key2);
  }
  section[key1Offset - 1] += key1 + key2;
  const sum = section.subarray(2).reduce((total, byte) => total + byte, 0) & 0xffff;
  if (sum !== (section[0] | section[1] << 8)) throw Error('梦时空分数存档校验失败');
  return section;
}

export function unlockScores(bytes) {
  if (bytes.length !== sectionSize * 4) throw Error('梦时空分数存档长度不匹配');
  const result = Uint8Array.from(bytes);
  for (let rank = 0; rank < 4; rank++) {
    const offset = rank * sectionSize;
    const section = decodeScoreSection(bytes.subarray(offset, offset + sectionSize));
    if (section[clearOffset] === 99) continue;
    section[clearOffset] = 99;
    const sum = section.subarray(2).reduce((total, byte) => total + byte, 0) & 0xffff;
    section[0] = sum & 0xff; section[1] = sum >>> 8;
    const key1 = section[key1Offset], key2 = section[key2Offset];
    let feedback = key2;
    for (let i = key1Offset - 1; i >= 0; i--) {
      section[i] -= key1 + feedback;
      feedback = rotateRight3(section[i]) ^ key2;
    }
    result.set(section, offset);
  }
  return result;
}

export function unlockDiskScores(fat) {
  const entry = fat.find('YUMEZIKU/YUME.NEM'), bytes = unlockScores(fat.read(entry));
  const chain = fat.chain(fat.view.getUint16(entry + 26, true));
  // Same-length in-place update retains the file's allocation and cluster tail.
  chain.forEach((cluster, i) => {
    const start = i * fat.clusterSize;
    fat.data.set(bytes.subarray(start, Math.min(start + fat.clusterSize, bytes.length)), fat.offset(cluster));
  });
}
