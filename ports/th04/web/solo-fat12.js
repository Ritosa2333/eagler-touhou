import {sha256} from './netplay/sha256.js';
export {sha256};

// HDI partition CHS and the FAT12 BPB describe the disk, including saved copies.
export class Fat12 {
  constructor(data) {
    this.data = data;
    this.view = new DataView(data.buffer, data.byteOffset, data.byteLength);
    const u16 = at => this.view.getUint16(at, true), u32 = at => this.view.getUint32(at, true);
    const header = u32(8), sector = u32(16), spt = u32(20), heads = u32(24);
    const part = header + sector;
    this.base = header + ((u16(part + 10) * heads + data[part + 9]) * spt + data[part + 8]) * sector;
    const b = this.base, bps = u16(b + 11), spc = data[b + 13];
    this.clusterSize = bps * spc;
    this.fatCount = data[b + 16];
    this.fatSize = u16(b + 22) * bps;
    this.fat = b + u16(b + 14) * bps;
    this.root = this.fat + this.fatCount * this.fatSize;
    this.rootSize = u16(b + 17) * 32;
    this.clusters = this.root + Math.ceil(this.rootSize / bps) * bps;
    const total = u16(b + 19) || u32(b + 32);
    this.clusterCount = Math.floor((b + total * bps - this.clusters) / this.clusterSize);
    if (![128, 256, 512, 1024, 2048, 4096].includes(bps) || !spc || (spc & (spc - 1))
      || this.fatCount < 1 || this.fatCount > 4 || this.clusterCount < 1 || this.clusterCount >= 4085
      || b + total * bps > data.length || (this.clusterCount + 2) * 1.5 > this.fatSize) {
      throw Error('不支持的 FAT12 镜像结构');
    }
  }
  next(cluster) {
    const value = this.view.getUint16(this.fat + Math.floor(cluster * 3 / 2), true);
    return cluster & 1 ? value >>> 4 : value & 0xfff;
  }
  setNext(cluster, value) {
    for (let copy = 0; copy < this.fatCount; copy++) {
      const at = this.fat + copy * this.fatSize + Math.floor(cluster * 3 / 2);
      const old = this.view.getUint16(at, true);
      this.view.setUint16(at, cluster & 1 ? (old & 0xf) | (value << 4) : (old & 0xf000) | value, true);
    }
  }
  chain(first) {
    const result = [], seen = new Set();
    for (let cluster = first; cluster < 0xff8; cluster = this.next(cluster)) {
      if (cluster < 2 || cluster >= this.clusterCount + 2 || cluster >= 0xff0 || seen.has(cluster)) {
        throw Error('镜像文件簇链损坏');
      }
      seen.add(cluster); result.push(cluster);
    }
    return result;
  }
  offset(cluster) { return this.clusters + (cluster - 2) * this.clusterSize; }
  find(path, {allowDirectory = false, optional = false} = {}) {
    let ranges = [[this.root, this.rootSize]], entry;
    const parts = path.split('/');
    for (let index = 0; index < parts.length; index++) {
      entry = undefined;
      const [stem, ext = ''] = parts[index].split('.');
      const name = stem.padEnd(8, ' ') + ext.padEnd(3, ' ');
      outer: for (const [start, size] of ranges) {
        for (let at = start; at < start + size; at += 32) {
          if (!this.data[at]) break outer;
          if (this.data[at] === 0xe5 || this.data[at + 11] & 8) continue;
          if (Array.from(this.data.subarray(at, at + 11), b => String.fromCharCode(b)).join('') === name) {
            entry = at; break outer;
          }
        }
      }
      if (entry === undefined) {
        if (optional && index === parts.length - 1) return undefined;
        throw Error(`镜像缺少 ${path}`);
      }
      if (index < parts.length - 1) {
        if (!(this.data[entry + 11] & 16)) throw Error('镜像目录无效');
        ranges = this.chain(this.view.getUint16(entry + 26, true)).map(c => [this.offset(c), this.clusterSize]);
      }
    }
    if (!allowDirectory && this.data[entry + 11] & 16) throw Error('目标不是文件');
    return entry;
  }
  put(path, bytes) {
    let entry = this.find(path, {optional: true});
    if (entry === undefined) {
      const slash = path.lastIndexOf('/'), name = path.slice(slash + 1);
      if (!/^[A-Z0-9_]{1,8}(\.[A-Z0-9_]{1,3})?$/.test(name)) throw Error('无效的 DOS 文件名');
      let ranges = [[this.root, this.rootSize]];
      if (slash >= 0) {
        const parent = this.find(path.slice(0, slash), {allowDirectory: true});
        if (!(this.data[parent + 11] & 16)) throw Error('目标不是目录');
        ranges = this.chain(this.view.getUint16(parent + 26, true)).map(c => [this.offset(c), this.clusterSize]);
      }
      for (const [start, size] of ranges) {
        for (let at = start; at < start + size; at += 32) {
          if (this.data[at] === 0 || this.data[at] === 0xe5) { entry = at; break; }
        }
        if (entry !== undefined) break;
      }
      if (entry === undefined) throw Error('镜像目录空间不足');
      // Reject a full disk before creating its directory entry.
      let free = 0;
      for (let c = 2; c < this.clusterCount + 2; c++) if (!this.next(c)) free++;
      if (free < Math.ceil(bytes.length / this.clusterSize)) throw Error('镜像空间不足');
      const [stem, ext = ''] = name.split('.');
      this.data.fill(0, entry, entry + 32);
      this.data.set(new TextEncoder().encode(stem.padEnd(8, ' ') + ext.padEnd(3, ' ')), entry);
      this.data[entry + 11] = 0x20;
    }
    this.replace(entry, bytes);
    return entry;
  }
  read(entry) {
    const size = this.view.getUint32(entry + 28, true);
    const chain = this.chain(this.view.getUint16(entry + 26, true));
    if (size > chain.length * this.clusterSize) throw Error('镜像文件长度无效');
    const result = new Uint8Array(size);
    chain.forEach((c, i) => {
      const count = Math.min(this.clusterSize, Math.max(0, size - i * this.clusterSize));
      if (count) result.set(this.data.subarray(this.offset(c), this.offset(c) + count), i * this.clusterSize);
    });
    return result;
  }
  replace(entry, bytes) {
    const first = this.view.getUint16(entry + 26, true);
    const old = first === 0 && this.view.getUint32(entry + 28, true) === 0 ? [] : this.chain(first);
    const needed = Math.ceil(bytes.length / this.clusterSize), chain = old.slice(0, needed);
    for (let c = 2; chain.length < needed && c < this.clusterCount + 2; c++) {
      if (this.next(c) === 0) chain.push(c);
    }
    if (chain.length !== needed) throw Error('镜像空间不足，无法安装暂停补丁');
    chain.forEach((c, i) => {
      this.setNext(c, chain[i + 1] || 0xfff);
      const at = this.offset(c);
      this.data.fill(0, at, at + this.clusterSize);
      this.data.set(bytes.subarray(i * this.clusterSize, (i + 1) * this.clusterSize), at);
    });
    for (const c of old.slice(needed)) this.setNext(c, 0);
    this.view.setUint16(entry + 26, chain[0] || 0, true);
    this.view.setUint32(entry + 28, bytes.length, true);
  }
}
