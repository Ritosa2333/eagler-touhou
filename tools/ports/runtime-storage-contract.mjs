import assert from 'node:assert/strict';
import {extname} from 'node:path';

const quoteList = source => [...source.matchAll(/'([^']+)'/g)].map(match => match[1]);

/**
 * Read the storage-root policy from its production owner. Keeping this parser
 * strict makes a policy refactor require an explicit acceptance update instead
 * of silently drifting from the Runtime's IndexedDB namespace.
 */
export function parseReplayStoragePolicy(source) {
  assert.equal(typeof source, 'string', 'Replay storage policy source is required');
  const database = source.match(/const\s+database\s*=\s*PRODUCT_GAMES\[game\]\.storage\.saveRoot\s*\+\s*\(\s*mp\s*\?\s*\(\s*(\[[^\]]*\])\.includes\(game\)\s*\?\s*'([^']+)'\s*:\s*'([^']+)'\s*\)\s*:\s*''\s*\)\s*;/);
  assert.ok(database, 'Unable to read the production multiplayer storage namespace rule');
  const language = source.match(/root\s*:\s*database\s*\+\s*\(\s*game\s*===\s*'([^']+)'\s*\|\|\s*game\s*===\s*'([^']+)'\s*\?\s*\(\s*language\s*===\s*'([^']+)'\s*\?\s*'([^']+)'\s*:\s*'([^']+)'\s*\)\s*:\s*''\s*\)/);
  assert.ok(language, 'Unable to read the production language-scoped storage-root rule');
  const multiplayerGames = quoteList(database[1]);
  assert.ok(multiplayerGames.length > 0, 'Production multiplayer namespace rule has no title list');
  return Object.freeze({
    multiplayerGames: Object.freeze(multiplayerGames),
    multiplayerListedSuffix: database[2],
    multiplayerDefaultSuffix: database[3],
    languageGames: Object.freeze([language[1], language[2]]),
    chineseLanguage: language[3],
    chineseDirectory: language[4],
    defaultDirectory: language[5],
    source: 'launcher/src/launcher/replay-storage-policy.mts | replayStorageLocation',
  });
}

export function deriveReplayStorageLocations({game, product, policy, language = 'ja'} = {}) {
  assert.equal(typeof game, 'string', 'Storage contract needs a product game ID');
  assert.ok(product?.storage?.saveRoot, `${game}: product directory must declare storage.saveRoot`);
  assert.ok(policy?.multiplayerGames && policy?.languageGames, `${game}: production storage policy is required`);
  const forVariant = variant => {
    assert.ok(variant === 'normal' || variant === 'multiplayer', `Unsupported storage variant ${variant}`);
    const database = product.storage.saveRoot + (variant === 'multiplayer'
      ? (policy.multiplayerGames.includes(game) ? policy.multiplayerListedSuffix : policy.multiplayerDefaultSuffix)
      : '');
    const root = database + (policy.languageGames.includes(game)
      ? (language === policy.chineseLanguage ? policy.chineseDirectory : policy.defaultDirectory)
      : '');
    return Object.freeze({game, variant, database, root, language});
  };
  const normal = forVariant('normal');
  const multiplayer = product.multiplayerRuntime ? forVariant('multiplayer') : null;
  if (multiplayer) {
    assert.notEqual(normal.database, multiplayer.database, `${game}: normal and multiplayer databases must be distinct`);
    assert.notEqual(normal.root, multiplayer.root, `${game}: normal and multiplayer roots must be distinct`);
  }
  return Object.freeze({normal, multiplayer, policySource: policy.source});
}

export function runtimeReplayFilePath({product, fixturePath, suffix} = {}) {
  assert.ok(product?.replay?.prefix, 'Replay file path needs the product directory prefix');
  assert.match(suffix ?? '', /^[a-z0-9]{4}$/, 'Replay file suffix must contain four lowercase alphanumeric characters');
  const extension = extname(fixturePath ?? '').slice(1).toLowerCase();
  const declaredExtensions = product.replay.extensions ?? ['rpy', 'rpyx'];
  assert.ok(declaredExtensions.includes(extension), `Replay fixture extension .${extension} is not declared by the product`);
  return `replay/${product.replay.prefix}_ud${suffix}.${extension}`;
}

/** A self-contained reader passed to the actual Runtime iframe for mount evidence. */
export function readRuntimeStorageMount({runtime: mapping, expectedDatabase} = {}) {
  const root = typeof window === 'object' ? window : globalThis;
  if (!mapping || ['global','module'].some(key => typeof mapping[key] !== 'string' || !mapping[key]))
    return {available:false,expectedDatabase,reason:'Declared Runtime mount mapping is unavailable'};
  const runtime = root[mapping.global];
  if (!runtime) return {available:false,expectedDatabase,runtimeGlobal:mapping.global,
    reason:`Declared Runtime mount ${mapping.global} is unavailable`};
  const module = runtime[mapping.module];
  const fs = module?.FS;
  if (!fs || typeof fs.lookupPath !== 'function') return {available: false, expectedDatabase,
    runtimeGlobal:mapping.global,moduleField:mapping.module,reason: 'Runtime FS mount inspection is unavailable'};
  const rawMounts = fs.mounts;
  const mounts = Array.isArray(rawMounts) ? rawMounts : rawMounts && typeof rawMounts === 'object' ? Object.values(rawMounts) : [];
  let pathMount = null;
  try { pathMount = fs.lookupPath(expectedDatabase).node?.mount ?? null; } catch {}
  const mount = mounts.find(item => item?.mountpoint === expectedDatabase) ?? pathMount;
  if (!mount) return {available: false, expectedDatabase,runtimeGlobal:mapping.global,moduleField:mapping.module,
    reason: 'Declared persistence mount is absent from the Runtime FS'};
  const mountpoint = mount.mountpoint ?? null;
  const typeName = mount.type?.name ?? mount.type?.constructor?.name ?? null;
  const idbfs = mount.type === module?.IDBFS || typeName === 'IDBFS' ? true
    : module?.IDBFS || typeName ? false : null;
  return {available: true, expectedDatabase,runtimeGlobal:mapping.global,moduleField:mapping.module,mountpoint,typeName,idbfs};
}

/** Read native format entry points without invoking the parser or playback. */
export function readRuntimeReplayApis({runtime: mapping} = {}) {
  const root = typeof window === 'object' ? window : globalThis;
  if (!mapping || ['global','core','module'].some(key => typeof mapping[key] !== 'string' || !mapping[key]))
    return {available:false,reason:'Declared Runtime API mapping is unavailable'};
  const runtime = root[mapping.global];
  if (!runtime) return {available:false,runtimeGlobal:mapping.global,
    reason:`Declared Runtime mount ${mapping.global} is unavailable`};
  const module = runtime[mapping.module];
  const core = runtime[mapping.core];
  const api = name => [core, module].some(owner => typeof owner?.[name] === 'function' || typeof owner?.[`_${name}`] === 'function');
  return {available:true,runtimeGlobal:mapping.global,coreField:mapping.core,moduleField:mapping.module,
    inspect: api('eagler_replay_inspect'), open: api('eagler_replay_open'),
    heap: !!(core?.memory?.buffer ?? module?.HEAPU8?.buffer), fileSystem: !!module?.FS};
}

export function validateReplayInspection({description, inspectedPath, expectedPath} = {}) {
  assert.equal(inspectedPath, expectedPath, 'Replay inspection must use the file written by this scenario');
  assert.ok(description && typeof description === 'object', 'Official Runtime Replay inspection returned no description');
  assert.ok(Array.isArray(description.stages) && description.stages.length > 0,
    'Official Runtime Replay inspection did not recognize a stage');
  return Object.freeze({status: 'passed', method: 'official-runtime-inspection', path: inspectedPath,
    stageCount: description.stages.length, naturalEndpointClaimed: false});
}

