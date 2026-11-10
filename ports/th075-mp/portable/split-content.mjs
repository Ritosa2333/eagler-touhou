export const SPLIT_SCHEMA = 'eagler-touhou/th075-multiplayer-assets/1';
const sha256 = async bytes => Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256', bytes)),
  byte => byte.toString(16).padStart(2, '0')).join('');
const validPath = path => typeof path === 'string' && path.length <= 240 &&
  /^\/(?:fonts\/original-glyphs\.bin|music\/(?:catalog\.bin|[a-z0-9_]+\.ogg)|resources\/(?:base|patched)\/[a-zA-Z0-9_./ -]+\.z)$/.test(path) &&
  path.split('/').slice(1).every(part => part && part !== '.' && part !== '..');

export async function openSplitContent(indexUrl, indexHash, fetchAsset = fetch) {
  const url = new URL(indexUrl);
  if (!/^https?:$/.test(url.protocol) || url.username || url.password || url.hash ||
      !/^[a-f0-9]{64}$/.test(indexHash) || !url.pathname.endsWith('/' + indexHash + '.json'))
    throw Error('Invalid TH075 multiplayer asset index');
  async function fetchVerified(location, hash, limit, expected) {
    // Keep the authorized index at its exact URL. Public, content-addressed
    // blobs may be redirected to a download host; verify their bytes either way.
    const response = await fetchAsset(location, {
      cache: 'force-cache', credentials: 'omit', redirect: expected === undefined ? 'error' : 'follow',
    });
    if (!response.ok) throw Error('TH075 asset HTTP ' + response.status);
    if (Number(response.headers.get('Content-Length')) > limit) throw Error('Invalid TH075 asset size');
    const bytes = await response.arrayBuffer();
    if (!bytes.byteLength || bytes.byteLength > limit || (expected && bytes.byteLength !== expected) ||
        await sha256(bytes) !== hash) throw Error('Corrupt TH075 multiplayer asset');
    return bytes;
  }

  const index = JSON.parse(new TextDecoder('utf-8', {fatal: true}).decode(
    await fetchVerified(url, indexHash, 1024 * 1024)));
  if (index.schema !== SPLIT_SCHEMA || !Array.isArray(index.files) || index.files.length > 4096)
    throw Error('Invalid TH075 multiplayer asset schema');
  const files = new Map(), pending = new Map();
  for (const file of index.files) {
    if (!file || !validPath(file.path) || files.has(file.path) ||
        !/^[a-f0-9]{64}$/.test(file.sha256) || !Number.isSafeInteger(file.bytes) ||
        file.bytes <= 0 || file.bytes > 128 * 1024 * 1024)
      throw Error('Invalid TH075 multiplayer asset file');
    files.set(file.path, file);
  }
  if (!files.has('/fonts/original-glyphs.bin') || !files.has('/music/catalog.bin'))
    throw Error('Incomplete TH075 multiplayer asset index');
  return {
    identity: SPLIT_SCHEMA + ':' + indexHash,
    paths: [...files.keys()],
    async read(path) {
      const file = files.get(path);
      if (!file) throw Error('TH075 DATA path is not declared: ' + path);
      const key = file.sha256 + ':' + file.bytes;
      let request = pending.get(key);
      if (!request) {
        request = fetchVerified(new URL(file.sha256 + '.bin', url), file.sha256, file.bytes, file.bytes)
          .finally(() => pending.delete(key));
        pending.set(key, request);
      }
      return new Uint8Array(await request);
    },
  };
}
