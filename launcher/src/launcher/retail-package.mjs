import {PRODUCT_GAMES, HOST_PROTOCOL} from '../../../common/contracts/product-catalog.mjs';
import {PRODUCT_CONTENT} from '../../lib/content-definition.mjs';
import {PACKAGE_DESCRIPTOR_SCHEMA} from '../../package/package-descriptor.mjs';
import {sha256Hex} from '../../package/package-integrity.mjs';
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

export function retailArchiveLimits(game) {
  const datLimit = game === 'th19' || game === 'th20' ? 600 * MiB : 128 * MiB;
  const limits = Object.fromEntries(retailRequiredFiles(game).map(name => [name.toLowerCase(), datLimit]));
  if (game === 'th06') {
    for (const suffix of ['cm', 'ed', 'in', 'md', 'st', 'tl']) limits['th06_' + suffix + '.dat'] = datLimit;
    limits['eagler-hitbox.png'] = 2 * MiB;
  } else {
    limits['thbgm.dat'] = 1024 * MiB;
  }
  for (const name of PRODUCT_CONTENT[game].music.ogg.files) {
    limits[name] = 64 * MiB;
    if (game === 'th06') limits[name.replace(/\.ogg$/, '.wav')] = 64 * MiB;
  }
  limits['msgothic.ttc'] = limits['unifont.otf'] = 64 * MiB;
  return limits;
}

function indexSources(files, game) {
  const sources = new Map();
  for (const file of files) {
    if (!(file instanceof Blob) || typeof file.name !== 'string') throw Error('Invalid original game file');
    const path = (file.archivePath || file.webkitRelativePath || file.name).toLowerCase();
    const slash = path.lastIndexOf('/');
    const name = path.slice(slash + 1);
    if (!/\.(?:dat|wav|ogg|ttc|otf)$/.test(name) && name !== 'eagler-hitbox.png') continue;
    const th06Alias = game === 'th06' && /^th06_(cm|ed|in|md|st|tl)\.dat$/.exec(name);
    const key = th06Alias ? path.slice(0, slash + 1) + `紅魔郷${th06Alias[1]}.dat` : path;
    if (sources.has(key)) throw Error(`Duplicate input file: ${path}`);
    sources.set(key, file);
  }
  return sources;
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
  const sources = indexSources(inputs, game);
  const root = locateGameFolder(sources, required);
  async function read(name, max) {
    const key = name.toLowerCase();
    let entry = sources.get(root + key) ?? sources.get(key);
    if (!entry && key === 'thbgm.dat') {
      const matches = [...sources].filter(([path]) => path.endsWith('/thbgm.dat'));
      if (matches.length > 1) throw Error('The selection contains multiple thbgm.dat files');
      entry = matches[0]?.[1];
    }
    if (!entry) return null;
    signal?.throwIfAborted();
    if (!entry.size || entry.size > max) throw Error(`${name}: invalid file size`);
    return entry;
  }

  const archives = new Map();
  let directory;
  const limits = retailArchiveLimits(game);
  for (const name of required) {
    const blob = await read(name, limits[name.toLowerCase()]);
    directory = await readRetailArchive(blob, game);
    if (game !== 'th06' && !directory.some(entry => new RegExp(`(?:^|/)${game}_\\d{4}[a-z]\\.ver$`).test(entry.name)))
      throw Error(`${name}: game version marker does not match ${game.toUpperCase()}`);
    archives.set(name, blob);
  }

  const product = PRODUCT_GAMES[game];
  const files = {}, payloads = new Map(), base = [], music = [];
  const targets = new Set();
  async function addFile(target, blob, ids = base, id = `file-${payloads.size}`) {
    signal?.throwIfAborted();
    if (targets.has(target)) throw Error(`Duplicate package resource: ${target}`);
    const sha256 = await sha256Hex(blob);
    signal?.throwIfAborted();
    files[id] = {source: `files/${id}/${target.split('/').at(-1)}`, target, bytes: blob.size, sha256, revision: `local-${sha256.slice(0, 24)}`};
    targets.add(target);
    payloads.set(id, blob);
    ids.push(id);
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
  let packedTracks = [];
  if (bgm) {
    const header = await bgm.slice(0, 8).arrayBuffer();
    if (header.byteLength !== 8 || new TextDecoder().decode(header.slice(0, 4)) !== 'ZWAV' ||
        new DataView(header).getUint32(4, true) !== 1) throw Error('Invalid thbgm.dat header');
    packedTracks = parseBgmTracks(await readRetailBgmIndex(archive, game, directory), bgm.size);
  }
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

  const revision = await sha256Hex(encoder.encode(JSON.stringify(Object.values(files).map(({target, sha256}) => [target, sha256]))));
  const descriptor = {
    schema: PACKAGE_DESCRIPTOR_SCHEMA, game, gameVersion: 'original', revision: `retail-${revision.slice(0, 32)}`,
    runtimeRequirement: {protocol: HOST_PROTOCOL, target: game, dataFile: product.package.dataFileId, dataLayout: layout},
    files, base: {files: base}, components: {
      ...(music.length ? {ogg: {type: 'music', files: music}} : {}),
    },
  };
  return {descriptor, payloads};
}
