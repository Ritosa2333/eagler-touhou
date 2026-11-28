export function safeAssetUrl(base, relative) {
  if (!/^[A-Za-z0-9_./-]+$/.test(relative) || relative.startsWith('/') || relative.split('/').includes('..')) {
    throw new Error(`不安全的资源路径：${relative}`);
  }
  const root = new URL(base);
  const resolved = new URL(relative, root);
  if (resolved.origin !== root.origin || !resolved.pathname.startsWith(root.pathname)) {
    throw new Error('资源路径越过当前版本目录');
  }
  return resolved.href;
}

export function words(hex) {
  if (!/^(?:[a-f0-9]{8})*$/i.test(hex)) throw new Error('参数不是完整的 32 位原始字');
  const bytes = Uint8Array.from(hex.match(/../g) ?? [], pair => Number.parseInt(pair, 16));
  const view = new DataView(bytes.buffer);
  return Array.from({ length: bytes.length / 4 }, (_, index) => {
    const value = view.getFloat32(index * 4, true);
    return { integer: view.getInt32(index * 4, true), float: Number.isFinite(value) ? value : null };
  });
}

export function filterItems(items, query) {
  const needle = query.trim().toLowerCase();
  return items.filter(item => String(item.name ?? item.index ?? '').toLowerCase().includes(needle));
}
