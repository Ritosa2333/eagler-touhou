import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdir, mkdtemp, readFile, realpath, rm, writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {basename, dirname, join} from 'node:path';
import {test} from 'node:test';
import {canonicalRuntimePayload, parseRuntimeGenerationPath} from '../../../launcher/src/contracts/runtime-generations.mts';
import {validateSiteManifest} from '../../../launcher/lib/contracts/site-manifest.mjs';
import {assembleTh128LocalHost} from '../tools/assemble-local-host.mjs';

const sha = bytes => createHash('sha256').update(bytes).digest('hex');
const protocol = 'eagler-touhou/1', layout = `sha256-${'f'.repeat(64)}`, digest = 'a'.repeat(64);
const baseline = 'lang_zh-hans', entry = 'th128.html';

async function fixture(t) {
  const parent = await realpath(tmpdir()), root = await mkdtemp(join(parent, 'th128-host-fixture-'));
  t.after(async () => {
    const resolved = await realpath(root);
    if (dirname(resolved) !== parent || !basename(resolved).startsWith('th128-host-fixture-')) throw new Error('Unexpected fixture path');
    await rm(resolved, {recursive: true, force: true});
  });
  const input = {host: join(root, 'input.json'), output: join(root, 'assembled'), normal: join(root, 'normal'), multiplayer: join(root, 'mp')};
  const game = {runtime: 'legacy/th128.html', gameData: {path: 'th128.dat', bytes: 10, sha256: digest, version: `sha256-${digest}`, layout},
    music: {midi: {files: [], supported: false}, ogg: null}, languages: [], languageOptions: [{id: baseline, pack: null}],
    offlineCompatibility: {schema: 'eagler-touhou/offline-game-pack/1', runtimeCompatibility: {protocol, dataLayout: layout, versionSource: 'offline-pack'},
      requiredShared: [], languages: {source: 'offline-pack', baseline: [baseline]}}};
  const host = {schema: 'eagler-touhou/site-manifest/1', protocol, profile: 'development', shared: {resourceMode: 'import', testBuild: true}, games: {th128: game}};
  const text = JSON.stringify(host);
  await writeFile(input.host, text);
  for (const [variant, folder] of [['normal', input.normal], ['multiplayer', input.multiplayer]]) {
    await mkdir(folder);
    const sources = {[entry]: `<!doctype html><title>TH128 ${variant}</title><script type="module" src="./fixture.mjs"></script>`,
      'fixture.mjs': `export const variant = ${JSON.stringify(variant)};`,
      'manifest.json': JSON.stringify({game: 'th128', protocol, entry, dataProvider: 'retail-memory', dataLayout: layout,
        ...(variant === 'multiplayer' ? {variant} : {})})};
    const files = Object.entries(sources).map(([path, data]) => ({path, bytes: Buffer.byteLength(data), sha256: sha(data)}));
    for (const [file, data] of Object.entries(sources)) await writeFile(join(folder, file), data);
    if (variant === 'normal') await writeFile(join(folder, 'runtime-files.json'), JSON.stringify({schema: 'eagler-touhou/runtime-directory/1',
      files: Object.fromEntries(files.map(({path, ...identity}) => [path, identity]))}));
    else {
      input.multiplayerGeneration = sha(canonicalRuntimePayload(entry, files));
      await writeFile(join(folder, 'runtime-generation.json'), JSON.stringify({schema: 'eagler-touhou/runtime-generation/1',
        generation: input.multiplayerGeneration, entry, files}));
    }
  }
  return {input, game, text};
}

test('local TH128 Host pins separate normal and multiplayer programs without changing the Package identity', async t => {
  const {input, game, text} = await fixture(t);
  const result = await assembleTh128LocalHost({...input, relay: 'ws://127.0.0.1:49725/'});
  const host = validateSiteManifest(JSON.parse(await readFile(join(input.output, 'site-manifest.json'), 'utf8')));
  const runtime = JSON.parse(await readFile(join(input.output, 'runtime-manifest.json'), 'utf8'));
  assert.equal(host.profile, 'th128-multiplayer-local-preview');
  assert.equal(host.shared.netplayRelay, 'ws://127.0.0.1:49725/');
  assert.deepEqual(host.games.th128.gameData, game.gameData);
  assert.deepEqual(host.games.th128.offlineCompatibility, game.offlineCompatibility);
  assert.deepEqual(runtime.groups.map(group => group.root), ['runtime/th128/', 'runtime/th128/multiplayer/']);
  assert.deepEqual(parseRuntimeGenerationPath(host.games.th128.multiplayerRuntime),
    {root: 'runtime/th128/multiplayer/', generation: input.multiplayerGeneration, file: entry});
  assert.equal(result.multiplayer.generation, input.multiplayerGeneration);
  assert.notEqual(result.normal.generation, result.multiplayer.generation);
  assert.equal(await readFile(input.host, 'utf8'), text);
  await assert.rejects(assembleTh128LocalHost(input), /EEXIST/);
});

test('local Host assembly fails closed on an altered program or unauthorized location', async t => {
  const {input} = await fixture(t);
  await assert.rejects(assembleTh128LocalHost({...input, relay: 'wss://public.invalid/'}), /loopback/);
  await assert.rejects(assembleTh128LocalHost({...input, output: join(input.normal, 'nested')}), /outside/);
  await assert.rejects(assembleTh128LocalHost({...input, multiplayerGeneration: 'b'.repeat(64)}), /generation/);
  await writeFile(join(input.multiplayer, 'fixture.mjs'), 'export const tampered = true;');
  await assert.rejects(assembleTh128LocalHost(input), /changed|integrity/);
  await assert.rejects(readFile(join(input.output, 'site-manifest.json')), {code: 'ENOENT'});
  await assert.rejects(readFile(join(input.output, 'runtime-manifest.json')), {code: 'ENOENT'});
});

test('a raw Node-compatible Runtime cannot bypass the authored browser-generation check', async t => {
  const {input} = await fixture(t);
  const loader = 'if (globalThis.process?.versions?.node) await import("module");';
  await writeFile(join(input.normal, 'fixture.mjs'), loader);
  const indexPath = join(input.normal, 'runtime-files.json'), index = JSON.parse(await readFile(indexPath, 'utf8'));
  index.files['fixture.mjs'] = {bytes: Buffer.byteLength(loader), sha256: sha(loader)};
  await writeFile(indexPath, JSON.stringify(index));
  await assert.rejects(assembleTh128LocalHost(input), /Runtime dependency is not declared: fixture\.mjs -> module/);
  await assert.rejects(readFile(join(input.output, 'site-manifest.json')), {code: 'ENOENT'});
});
