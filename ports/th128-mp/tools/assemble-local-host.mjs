import {lstat, mkdir, readFile, realpath, writeFile} from 'node:fs/promises';
import {basename, dirname, isAbsolute, join, relative, resolve, sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {parseArgs} from 'node:util';
import {publishRuntimeManifest, runtimeGenerationId, verifyRuntimePublication, writeRuntimeGeneration} from '../../../launcher/lib/runtime-generations.mjs';
import {SITE_MANIFEST_FILE, TH128_MULTIPLAYER_PREVIEW_PROFILE, validateSiteManifest} from '../../../launcher/lib/contracts/site-manifest.mjs';
import {RUNTIME_GENERATION_FILE, RUNTIME_GENERATION_SCHEMA, RUNTIME_MANIFEST_FILE, isRuntimePath, runtimeGenerationEntry,
  validateRuntimeGeneration} from '../../../launcher/src/contracts/runtime-generations.mts';

const normalRoot = 'runtime/th128/', multiplayerRoot = 'runtime/th128/multiplayer/';
const entry = 'th128.html';
const sha256 = /^[a-f0-9]{64}$/;
const packageFile = /\.(?:dat|ogg|zip|rpy|th128mp)$/i;
const json = data => `${JSON.stringify(data, null, 2)}\n`;

async function readJson(path) {
  if (!(await lstat(path)).isFile()) throw new Error(`Expected an ordinary input file: ${path}`);
  return JSON.parse(await readFile(path, 'utf8'));
}

function contains(root, path) {
  const sub = relative(root, path);
  return !isAbsolute(sub) && sub !== '..' && !sub.startsWith(`..${sep}`);
}

function localRelay(value) {
  if (value === undefined) return undefined;
  const url = new URL(value);
  if (!['ws:', 'wss:'].includes(url.protocol) || !['127.0.0.1', '[::1]', 'localhost'].includes(url.hostname) ||
      url.username || url.password || url.hash) throw new Error('TH128 local Host needs a loopback Relay URL');
  return url.href;
}

function programInput(source, root, files) {
  const names = Object.keys(files);
  if (!names.includes(entry) || !names.includes('manifest.json') || names.some(name => !isRuntimePath(name) || packageFile.test(name)))
    throw new Error(`TH128 preview program inventory contains an invalid path or Package asset: ${root}`);
  return {source, root, entry, names, expected: files};
}

async function checkShell(directory, variant, host) {
  const shell = await readJson(join(directory, 'manifest.json'));
  if (shell.game !== 'th128' || shell.entry !== entry || shell.protocol !== host.protocol || shell.dataProvider !== 'retail-memory' ||
      (shell.variant ?? 'normal') !== variant || shell.dataLayout !== host.games.th128.gameData.layout)
    throw new Error(`TH128 ${variant} program and Host Package layout disagree`);
}

/** Write a fresh local program root. Does not build/serve a frontend or install Package files. */
export async function assembleTh128LocalHost({host, normal, multiplayer, multiplayerGeneration, output, relay}) {
  const netplayRelay = localRelay(relay);
  const [baseHost, normalSource, mpSource] = await Promise.all([
    readJson(resolve(host)).then(validateSiteManifest), realpath(normal), realpath(multiplayer),
  ]);
  if (baseHost.shared.resourceMode !== 'import' || !baseHost.games.th128 || baseHost.games.th128.multiplayerRuntime)
    throw new Error('TH128 local Host assembly needs the original normal import Host');
  const outputPath = resolve(output);
  const site = join(await realpath(dirname(outputPath)), basename(outputPath));
  if (contains(normalSource, site) || contains(mpSource, site)) throw new Error('Use a fresh output outside both Runtime sources');
  await Promise.all([checkShell(normalSource, 'normal', baseHost), checkShell(mpSource, 'multiplayer', baseHost)]);
  const normalIndex = await readJson(join(normalSource, 'runtime-files.json'));
  if (normalIndex.schema !== 'eagler-touhou/runtime-directory/1' || !normalIndex.files ||
      typeof normalIndex.files !== 'object' || Array.isArray(normalIndex.files)) throw new Error('Invalid TH128 normal program inventory');
  const mpIndex = await readJson(join(mpSource, RUNTIME_GENERATION_FILE));
  const mp = validateRuntimeGeneration(mpIndex);
  if (!sha256.test(multiplayerGeneration) || mpIndex.schema !== RUNTIME_GENERATION_SCHEMA || mp.entry !== entry ||
      mp.generation !== multiplayerGeneration || runtimeGenerationId(mp.entry, mp.files) !== multiplayerGeneration)
    throw new Error('TH128 multiplayer generation does not match the pinned program');
  const normalSpec = programInput(normalSource, normalRoot, normalIndex.files);
  const mpSpec = programInput(mpSource, multiplayerRoot, Object.fromEntries(mp.files.map(file => [file.path, file])));
  await mkdir(site); // An existing site, including the accepted single-player site, is never modified.
  const normalProgram = await writeRuntimeGeneration({site, ...normalSpec});
  const multiplayerProgram = await writeRuntimeGeneration({site, ...mpSpec});
  if (multiplayerProgram.generation !== multiplayerGeneration || normalProgram.generation === multiplayerProgram.generation)
    throw new Error('TH128 program generations must match their separate variants');
  const manifest = validateSiteManifest({schema: baseHost.schema, protocol: baseHost.protocol, profile: TH128_MULTIPLAYER_PREVIEW_PROFILE,
    shared: {resourceMode: 'import', testBuild: true, runtimeManifest: RUNTIME_MANIFEST_FILE, ...(netplayRelay ? {netplayRelay} : {})},
    games: {th128: {...baseHost.games.th128, runtime: runtimeGenerationEntry(normalRoot, normalProgram),
      multiplayerRuntime: runtimeGenerationEntry(multiplayerRoot, multiplayerProgram)}}});
  await publishRuntimeManifest(site, [{root: normalRoot, current: normalProgram}, {root: multiplayerRoot, current: multiplayerProgram}]);
  await verifyRuntimePublication(site, manifest);
  const siteManifest = join(site, SITE_MANIFEST_FILE);
  await writeFile(siteManifest, json(manifest), {flag: 'wx'});
  return {siteManifest, runtimeManifest: join(site, RUNTIME_MANIFEST_FILE), normal: normalProgram, multiplayer: multiplayerProgram};
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  const {values} = parseArgs({options: {host: {type: 'string'}, normal: {type: 'string'}, multiplayer: {type: 'string'},
    'multiplayer-generation': {type: 'string'}, output: {type: 'string'}, relay: {type: 'string'}}});
  for (const key of ['host', 'normal', 'multiplayer', 'multiplayer-generation', 'output'])
    if (!values[key]) throw new Error(`Missing --${key}`);
  const result = await assembleTh128LocalHost({...values, multiplayerGeneration: values['multiplayer-generation']});
  console.log(JSON.stringify({siteManifest: result.siteManifest, runtimeManifest: result.runtimeManifest,
    normal: {generation: result.normal.generation, files: result.normal.files.length},
    multiplayer: {generation: result.multiplayer.generation, files: result.multiplayer.files.length}}));
}
