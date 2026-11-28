export async function installLanguagePack(core, pack, game) {
  if (!pack) return;
  if (typeof pack.url !== 'string' || typeof pack.language !== 'string' ||
      !Number.isInteger(pack.bytes) || pack.bytes <= 0 || !pack.manifest ||
      !Array.isArray(pack.files)) throw Error('Invalid TH13 language pack');
  if (pack.language !== 'lang_zh-hans') throw Error('TH13 only accepts Simplified Chinese');
  if (new URL(pack.url, location.href).origin !== location.origin) throw Error('Cross-origin TH13 language pack');
  const manifest = pack.manifest;
  const prefix = `/thcrap/${game}/`;
  if (manifest.schema !== 'eagler-touhou/thcrap-static-pack/1' || manifest.game !== game ||
      manifest.language !== pack.language || typeof manifest.runtimeVersion !== 'string' ||
      !Array.isArray(manifest.files) || manifest.files.length > 256) throw Error('Invalid TH13 language manifest');
  const safe = path => typeof path === 'string' && path.startsWith(prefix) && path.length <= 240 &&
    !path.includes('\\') && path.split('/').slice(1).every(part => part && part !== '.' && part !== '..') &&
    !/\.(?:ecl|std|sht)$/i.test(path);
  const expected = new Map();
  for (const file of manifest.files) {
    if (!safe(file.path) || !Number.isInteger(file.bytes) || file.bytes < 0 || file.bytes > 64 * 1024 * 1024 ||
        expected.has(file.path)) throw Error('Invalid TH13 language file');
    expected.set(file.path, file);
  }
  if (pack.files.length !== expected.size) throw Error('TH13 language file count mismatch');
  const seen = new Set();
  for (const file of pack.files) {
    if (!safe(file?.path) || seen.has(file.path) || !(file.bytes instanceof Uint8Array) ||
        file.bytes.length !== expected.get(file.path)?.bytes) throw Error('TH13 language file mismatch');
    seen.add(file.path);
  }
  for (const path of core.__th13LanguageFiles || []) {
    try { core.FS.unlink(path); } catch {}
  }
  core.__th13LanguageFiles = [];
  for (const file of pack.files) {
    core.FS.mkdirTree(file.path.slice(0, file.path.lastIndexOf('/')));
    core.FS.writeFile(file.path, file.bytes, {canOwn: true});
    core.__th13LanguageFiles.push(file.path);
  }
  const marker = prefix + 'runtime-language.txt';
  core.FS.writeFile(marker, pack.language);
  core.__th13LanguageFiles.push(marker);
}
