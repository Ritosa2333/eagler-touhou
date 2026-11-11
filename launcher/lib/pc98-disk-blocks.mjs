const digest = async bytes => Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256', bytes)),
  byte => byte.toString(16).padStart(2, '0')).join('');

// Reconstruct the exact HDI layout, including its boot and FAT metadata.
// loadBlock receives a validated content-addressed relative path.
export async function restoreDiskBlocks(manifest, path, loadBlock) {
  if (manifest?.schema !== 'eagler-touhou/pc98-disks/2' || typeof loadBlock !== 'function' ||
      !Array.isArray(manifest.files) || !manifest.blocks || typeof manifest.blocks !== 'object') throw Error('Invalid PC-98 disk manifest');
  const matches = manifest.files.filter(file => file.path === path);
  if (matches.length !== 1) throw Error('PC-98 disk not found');
  const disk = matches[0];
  if (!Number.isSafeInteger(disk.imageBytes) || disk.imageBytes <= 0 || disk.imageBytes > 256 * 1024 * 1024 ||
      !/^[a-f0-9]{64}$/.test(disk.imageSha256) || !Array.isArray(disk.blocks) || !disk.blocks.length ||
      disk.blocks.length > 1024) throw Error('Invalid PC-98 disk identity');
  const output = new Uint8Array(disk.imageBytes), cache = new Map();
  let offset = 0;
  for (const hash of disk.blocks) {
    if (typeof hash !== 'string' || !/^[a-f0-9]{64}$/.test(hash)) throw Error('Invalid PC-98 block identity');
    const block = manifest.blocks[hash];
    if (!block || block.sha256 !== hash || block.file !== `disk-blocks/${hash}.gz` ||
        !Number.isSafeInteger(block.bytes) || block.bytes <= 0 || block.bytes > 256 * 1024 ||
        !Number.isSafeInteger(block.packedBytes) || block.packedBytes <= 0 || block.packedBytes > 512 * 1024 ||
        !/^[a-f0-9]{64}$/.test(block.packedSha256) || offset + block.bytes > output.length) throw Error('Invalid PC-98 block');
    let raw = cache.get(hash);
    if (!raw) {
      const packed = new Uint8Array(await loadBlock(block.file));
      if (packed.length !== block.packedBytes || await digest(packed) !== block.packedSha256) throw Error('PC-98 block download corrupt');
      const reader = new Blob([packed]).stream().pipeThrough(new DecompressionStream('gzip')).getReader();
      const parts = []; let length = 0;
      try {
        for (;;) {
          const {done,value} = await reader.read(); if (done) break;
          length += value.length;
          if (length > block.bytes) throw Error('PC-98 block exceeds declared size');
          parts.push(value);
        }
      } finally { await reader.cancel().catch(() => {}); }
      raw = new Uint8Array(length); let position = 0;
      for (const part of parts) {raw.set(part, position); position += part.length;}
      if (raw.length !== block.bytes || await digest(raw) !== hash) throw Error('PC-98 block corrupt');
      cache.set(hash, raw);
    }
    output.set(raw, offset); offset += raw.length;
  }
  if (offset !== output.length || await digest(output) !== disk.imageSha256) throw Error('PC-98 disk reconstruction failed');
  return output;
}

export async function restoreManagedDisk(optional, path, game) {
  const source = optional.get('pc98/disks.json');
  if (!source) return null;
  const manifest = JSON.parse(await source.text());
  if (manifest.game !== game || !manifest.files?.some(file => file.path === path)) return null;
  const bytes = await restoreDiskBlocks(manifest, path, async name => {
    const block = optional.get(`pc98/${name}`);
    if (!block) throw Error(`PC-98 disk block not installed: ${name}`);
    return block.arrayBuffer();
  });
  return new Response(new Blob([bytes]).stream().pipeThrough(new CompressionStream('gzip'))).blob();
}
