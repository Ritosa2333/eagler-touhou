import {PRODUCT_GAMES, HOST_PROTOCOL} from '../../../common/contracts/product-catalog.mjs';
import {PRODUCT_CONTENT} from '../../lib/content-definition.mjs';
import {PACKAGE_DESCRIPTOR_SCHEMA} from '../../package/package-descriptor.mjs';
import {sha256Hex} from '../../package/package-integrity.mjs';
import {buildStoredZip, parseStoredZip, readOriginalZipEntry, isSafeStoredZipName} from '../../package/stored-zip.mjs';
import {parseBgmTracks, readRetailArchive, readRetailBgmIndex} from './retail-archive.mjs';

const MiB = 1024 * 1024;
const flatSlots = {
  th06: [
    ['/eagler-hitbox.png', 40500], ['/紅魔郷CM.DAT', 935275], ['/紅魔郷ED.DAT', 2017777],
    ['/紅魔郷IN.DAT', 760223], ['/紅魔郷MD.DAT', 306003], ['/紅魔郷ST.DAT', 2964458], ['/紅魔郷TL.DAT', 1040706],
  ],
  th07: [['/th07.dat', 23829135]],
};
const supported = new Set(['th06', 'th07', 'th08', 'th09', 'th10', 'th11', 'th15', 'th17', 'th19', 'th20']);
const encoder = new TextEncoder();

export function supportsRetailImport(game) {
  return supported.has(game);
}

export function retailRequiredFiles(game) {
  if (!supportsRetailImport(game)) return [];
  return game === 'th06' ? PRODUCT_CONTENT.th06.original.files : [`${game}.dat`];
}

async function indexSources(files, signal) {
  const sources = new Map(), languagePacks = [];
  function add(path, size, read) {
    if (!isSafeStoredZipName(path)) throw Error(`Invalid input path: ${path}`);
    const key = path.toLowerCase();
    if (sources.has(key)) throw Error(`Duplicate input file: ${path}`);
    sources.set(key, {size, read});
  }
  for (const file of files) {
    signal?.throwIfAborted();
    if (!(file instanceof Blob) || typeof file.name !== 'string') throw Error('Invalid original game file');
    if (!file.webkitRelativePath && /\.zip$/i.test(file.name)) {
      if (!file.size || file.size > 1536 * MiB) throw Error('Invalid original game ZIP size');
      const entries = await parseStoredZip(file, {originalGame: true});
      const language = await languageManifest(file, entries, signal);
      if (language) {languagePacks.push({blob: file, manifest: language}); continue;}
      for (const [path, entry] of entries) {
        add(path, entry.uncompressedSize, () => readOriginalZipEntry(file, entry, signal));
      }
    } else {
      add(file.webkitRelativePath || file.name, file.size, async () => file);
    }
  }
  return {sources, languagePacks};
}

async function languageManifest(blob, entries, signal) {
  const item = entries.get('manifest.json');
  if (!item || item.uncompressedSize > MiB) return null;
  let manifest;
  const text = await (await readOriginalZipEntry(blob, item, signal)).text();
  try {manifest = JSON.parse(text);} catch {return null;}
  if (manifest?.schema !== 'eagler-touhou/thcrap-static-pack/1') return null;
  if (!/^lang_[a-z0-9]+(?:-[a-z0-9]+)*$/.test(manifest.language) || typeof manifest.runtimeVersion !== 'string' ||
      !Array.isArray(manifest.files) || !manifest.files.length || manifest.files.length > 256 ||
      entries.size !== manifest.files.length + 1 || blob.size > 64 * MiB)
    throw Error('Invalid compiled thcrap language package');
  let size = 0;
  const seen = new Set();
  for (const file of manifest.files) {
    const path = file?.path;
    if (typeof path !== 'string' || !path.startsWith(`/thcrap/${manifest.game}/`) || path.includes('..') || seen.has(path) ||
        !Number.isSafeInteger(file.bytes) || file.bytes < 0 || entries.get(path.slice(1))?.uncompressedSize !== file.bytes)
      throw Error('Invalid compiled thcrap resource');
    seen.add(path);
    size += file.bytes;
  }
  if (size > 96 * MiB) throw Error('Compiled thcrap resources exceed the supported size');
  return manifest;
}

function locateGameFolder(sources, required) {
  const primary = required[0].toLowerCase();
  const roots = [...sources.keys()].filter(path => path === primary || path.endsWith(`/${primary}`))
    .map(path => path.slice(0, -primary.length));
  const complete = roots.filter(root => required.every(name => sources.has(root + name.toLowerCase()) || sources.has(name.toLowerCase())));
  if (complete.length !== 1) throw Error(complete.length ? 'The selection contains multiple copies of this game' : `Missing game files: ${required.join(', ')}`);
  return complete[0];
}

async function flatLayout(game) {
  const slots = flatSlots[game];
  let offset = 0;
  const layout = slots.map(([path, size]) => {
    const start = offset;
    offset += size;
    return [path, start, offset];
  });
  return {bytes: offset, layout: `sha256-${await sha256Hex(encoder.encode(JSON.stringify(layout)))}`};
}

async function flatGameData(game, archives, addFile, read) {
  const chunks = [];
  for (const [path, size] of flatSlots[game]) {
    const name = path.slice(1), archive = archives.get(name);
    const input = archive || await read(name, 2 * MiB);
    if (input?.size === size) chunks.push(input);
    else {
      chunks.push(new Uint8Array(size));
      // The native preload has fixed slot sizes. A differently sized archive
      // is mounted by the existing Package resource path before native launch.
      if (input) await addFile(path, input);
    }
  }
  return new Blob(chunks);
}

export async function createRetailPackage({game, expected, inputs, fontTargets = [], loadFont, encodeOgg, onTrack, signal}) {
  const required = retailRequiredFiles(game);
  if (!required.length) throw Error('Original file conversion is unavailable for this game');
  const flat = flatSlots[game] ? await flatLayout(game) : null;
  const layout = flat?.layout ?? PRODUCT_CONTENT[game].dataLayout;
  if (expected && (expected.layout !== layout || (flat && flat.bytes !== expected.bytes)))
    throw Error('This Runtime uses a different game archive layout');
  signal?.throwIfAborted();
  const {sources, languagePacks} = await indexSources(inputs, signal);
  const root = locateGameFolder(sources, required);
  async function read(name, max) {
    const key = name.toLowerCase();
    const entry = sources.get(root + key) ?? sources.get(key);
    if (!entry) return null;
    signal?.throwIfAborted();
    if (!entry.size || entry.size > max) throw Error(`${name}: invalid file size`);
    return entry.read();
  }

  const archives = new Map();
  let directory;
  const archiveLimit = game === 'th19' || game === 'th20' ? 600 * MiB : 128 * MiB;
  for (const name of required) {
    const blob = await read(name, archiveLimit);
    directory = await readRetailArchive(blob, game);
    if (game !== 'th06' && !directory.some(entry => new RegExp(`(?:^|/)${game}_\\d{4}[a-z]\\.ver$`).test(entry.name)))
      throw Error(`${name}: game version marker does not match ${game.toUpperCase()}`);
    archives.set(name, blob);
  }

  const product = PRODUCT_GAMES[game];
  const files = {}, payloads = new Map(), base = [], music = [], language = [];
  const targets = new Set();
  async function addFile(target, blob, ids = base, id = `file-${payloads.size}`) {
    signal?.throwIfAborted();
    if (targets.has(target)) throw Error(`Duplicate package resource: ${target}`);
    const sha256 = await sha256Hex(blob);
    signal?.throwIfAborted();
    files[id] = {source: `files/${id}/${target.split('/').at(-1)}`, target, bytes: blob.size, sha256, revision: `local-${sha256.slice(0, 24)}`};
    targets.add(target);
    payloads.set(id, blob);
    ids?.push(id);
  }
  const archive = archives.get(required[0]);
  const data = flat ? await flatGameData(game, archives, addFile, read) : archive;
  await addFile(product.package.dataTarget, data, base, product.package.dataFileId);
  for (const target of fontTargets) {
    const name = target.slice(1);
    const blob = await read(name, 64 * MiB) ?? await read(`shared/${name}`, 64 * MiB) ?? await loadFont?.(target);
    if (!blob?.size || blob.size > 64 * MiB) throw Error(`Missing shared font: ${name}`);
    const header = new Uint8Array(await blob.slice(0, 4).arrayBuffer());
    const magic = String.fromCharCode(...header);
    if (header.length !== 4 || !['ttcf', 'OTTO', '\0\x01\0\0', 'true'].includes(magic)) throw Error(`Invalid shared font: ${name}`);
    await addFile(target, blob);
  }

  const bgm = game === 'th06' ? null : await read('thbgm.dat', 1024 * MiB);
  const packedTracks = bgm ? parseBgmTracks(await readRetailBgmIndex(archive, game, directory), bgm.size) : [];
  const tracks = packedTracks.length ? packedTracks : PRODUCT_CONTENT[game].music.ogg.files.map(name => ({name}));
  const mount = product.package.musicMounts.ogg;
  for (const [index, track] of tracks.entries()) {
    signal?.throwIfAborted();
    const name = track.name;
    const ogg = await read(`bgm-ogg/${name}`, 64 * MiB) ?? await read(`music/${name}`, 64 * MiB) ?? await read(`bgm/${name}`, 64 * MiB) ?? await read(name, 64 * MiB);
    const wav = !ogg && game === 'th06' ? await read(`bgm/${name.replace(/\.ogg$/, '.wav')}`, 64 * MiB) : null;
    let converted = ogg;
    if (!converted && (wav || bgm)) {
      onTrack?.({name, completed: index, total: tracks.length});
      const input = wav || bgm.slice(track.offset, track.offset + track.length);
      converted = await encodeOgg(input, {pcm: !wav, signal});
    }
    if (converted) {
      if (await converted.slice(0, 4).text() !== 'OggS') throw Error(`Invalid OGG resource: ${name}`);
      await addFile(`${mount}/${name}`, converted, music);
    }
  }

  // Import compiled thcrap assets through the launcher's existing selectable
  // language component so the native runtime activates their lookup paths.
  const prefix = `${root}thcrap/${game}/`;
  const overlay = [];
  let overlayBytes = 0;
  for (const [path, entry] of sources) {
    if (!path.startsWith(prefix)) continue;
    const relative = path.slice(prefix.length);
    if (!/^(?:[a-z0-9_-]+\/)*[a-z0-9_.-]+\.(?:anm|png|jpe?g|bmp|wav|msg|dat|end|etl|ttf|otf|json)$/i.test(relative) || relative.includes('..')) continue;
    overlayBytes += entry.size;
    if (entry.size > 32 * MiB || overlayBytes > 64 * MiB || overlay.length >= 256) throw Error('Compiled thcrap overlay is too large');
    overlay.push([`thcrap/${game}/${relative}`, await entry.read()]);
  }
  if (overlay.length) {
    const manifest = {schema: 'eagler-touhou/thcrap-static-pack/1', game, language: 'lang_local', title: 'thcrap (local)', runtimeVersion: 'independent',
      files: overlay.map(([name, blob]) => ({path: `/${name}`, bytes: blob.size}))};
    const blob = await buildStoredZip([['manifest.json', new Blob([JSON.stringify(manifest)])], ...overlay], signal);
    languagePacks.push({manifest, blob});
  }
  for (const {manifest, blob} of languagePacks) {
    if (manifest.game !== game) throw Error(`The selected thcrap package is for ${manifest.game}`);
    const id = manifest.language, file = `language:${id}`;
    await addFile(`/__eagler/language/${id}.zip`, blob, null, file);
    language.push({id, title: typeof manifest.title === 'string' ? manifest.title : id, file});
  }
  const revision = await sha256Hex(encoder.encode(JSON.stringify(Object.values(files).map(({target, sha256}) => [target, sha256]))));
  const descriptor = {
    schema: PACKAGE_DESCRIPTOR_SCHEMA, game, gameVersion: 'original', revision: `retail-${revision.slice(0, 32)}`,
    runtimeRequirement: {protocol: HOST_PROTOCOL, target: game, dataFile: product.package.dataFileId, dataLayout: layout},
    files, base: {files: base}, components: {
      ...(music.length ? {ogg: {type: 'music', files: music}} : {}),
      ...(language.length ? {language: {type: 'language', entries: language}} : {}),
    },
  };
  return {descriptor, payloads};
}
