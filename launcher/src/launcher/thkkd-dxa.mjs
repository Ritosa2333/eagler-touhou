// DXA v3/v4 的默认编码与压缩布局；格式参考 GARbro，署名见海惠堂 THIRD_PARTY.md。
const key = Uint8Array.from([0x55,0xaa,0x20,0x55,0x55,0x06,0x55,0xaa,0x55,0xd5,0x7c,0x66]);
function decode(bytes, offset = 0) {
  const result = new Uint8Array(bytes.length);
  for (let i = 0; i < bytes.length; i++) result[i] = bytes[i] ^ key[(offset + i) % key.length];
  return result;
}
export function unpackDxLz(bytes, expected) {
  if (bytes.length < 9) throw Error('DXA 压缩头不完整');
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  if (view.getUint32(0,true) !== expected || view.getUint32(4,true) !== bytes.length) throw Error('DXA 压缩长度无效');
  const out = new Uint8Array(expected), marker = bytes[8]; let from = 9, to = 0;
  while (from < bytes.length) {
    const value = bytes[from++];
    if (value !== marker) { if (to >= out.length) throw Error('DXA 输出越界'); out[to++] = value; continue; }
    if (from >= bytes.length) throw Error('DXA 控制字不完整');
    let token = bytes[from++];
    if (token === marker) { if (to >= out.length) throw Error('DXA 输出越界'); out[to++] = marker; continue; }
    if (token > marker) token--;
    let count = token >>> 3;
    if (token & 4) { if (from >= bytes.length) throw Error('DXA 重复长度不完整'); count |= bytes[from++] << 5; }
    count += 4; const mode = token & 3;
    if (mode === 3 || from + mode + 1 > bytes.length) throw Error('DXA 回溯距离无效');
    let distance = 0; for (let i = 0; i <= mode; i++) distance |= bytes[from++] << (8*i); distance++;
    if (distance > to || to + count > out.length) throw Error('DXA 回溯范围越界');
    for (let i = 0; i < count; i++) { out[to] = out[to-distance]; to++; }
  }
  if (to !== expected) throw Error('DXA 解压长度不符');
  return out;
}
function safeName(value) {
  if (!value || /[\\/:\x00-\x1f]/.test(value) || value === '.' || value === '..') throw Error('DXA 资源文件名无效');
  return value;
}
export async function readDxArchive(blob, {signal, maxExpandedBytes = 384*1024*1024} = {}) {
  signal?.throwIfAborted();
  if (!(blob instanceof Blob) || blob.size < 28 || blob.size > 512*1024*1024) throw Error('DXA 文件大小无效');
  const head = decode(new Uint8Array(await blob.slice(0,28).arrayBuffer()));
  const h = new DataView(head.buffer), version = h.getUint16(2,true), indexSize = h.getUint32(4,true), base = h.getUint32(8,true), indexOffset = h.getUint32(12,true), fileTable = h.getUint32(16,true), dirTable = h.getUint32(20,true);
  if (h.getUint16(0,true) !== 0x5844 || ![3,4].includes(version) || base !== (version === 4 ? 28 : 24)) throw Error('需要海惠堂原版 DXA v3/v4 资源');
  if (indexSize > 8*1024*1024 || indexOffset < base || indexOffset + indexSize !== blob.size || fileTable >= dirTable || dirTable >= indexSize) throw Error('DXA 索引范围无效');
  const page = version === 4 ? h.getUint32(24,true) : 932;
  if (![932,936,65001].includes(page)) throw Error('DXA 文件名编码不受支持');
  const decoder = new TextDecoder(page === 936 ? 'gbk' : page === 932 ? 'shift-jis' : 'utf-8', {fatal:true});
  const bytes = decode(new Uint8Array(await blob.slice(indexOffset).arrayBuffer()),indexOffset), view = new DataView(bytes.buffer);
  const entries = [], visited = new Set(), names = new Set(); let expanded = 0;
  function nameAt(offset) {
    if (offset + 4 > fileTable) throw Error('DXA 文件名索引越界');
    const start = offset + 4 + view.getUint16(offset,true)*4;
    if (start >= fileTable) throw Error('DXA 文件名范围无效');
    let end = start; while (end < fileTable && bytes[end] !== 0) end++;
    if (end === fileTable) throw Error('DXA 文件名不完整');
    return safeName(decoder.decode(bytes.subarray(start,end)));
  }
  function directory(offset, parent, depth = 0) {
    if (depth > 16 || visited.has(offset) || visited.size > 2048) throw Error('DXA 目录循环或层级过深');
    visited.add(offset); const at = dirTable + offset;
    if (at < dirTable || at + 16 > bytes.length) throw Error('DXA 目录越界');
    const count = view.getUint32(at+8,true), start = fileTable + view.getUint32(at+12,true);
    if (count > 4096 || start < fileTable || start + count*44 > dirTable) throw Error('DXA 文件表越界');
    for (let i=0;i<count;i++) {
      const p = start+i*44, path = parent+nameAt(view.getUint32(p,true)), attr = view.getUint32(p+4,true), offset = view.getUint32(p+32,true), size = view.getUint32(p+36,true), packed = view.getInt32(p+40,true);
      if (attr & 16) { directory(offset,path+'/',depth+1); continue; }
      const stored = packed === -1 ? size : packed, absolute = base+offset;
      expanded += size;
      if (size <= 0 || size > 96*1024*1024 || stored < 0 || absolute+stored > indexOffset || expanded > maxExpandedBytes || entries.length >= 4096) throw Error('DXA 资源大小或范围无效');
      const lower = path.toLowerCase(); if (names.has(lower)) throw Error('DXA 资源重名'); names.add(lower);
      entries.push({path, size, async read() {
        signal?.throwIfAborted(); const payload = decode(new Uint8Array(await blob.slice(absolute,absolute+stored).arrayBuffer()),absolute);
        signal?.throwIfAborted(); return packed === -1 ? payload : unpackDxLz(payload,size);
      }});
    }
  }
  directory(0,''); return entries;
}
