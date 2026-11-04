import {afterEach, test, vi} from 'vitest';
import assert from 'node:assert/strict';
import {mkdir, mkdtemp, readFile, rm, writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {dirname, resolve} from 'node:path';
import {PRODUCT_CONTENT} from '../lib/content-definition.mjs';

const mocks = vi.hoisted(() => ({
  inspectHostWorkspace: vi.fn(),
  ensurePythonEnvironment: vi.fn(),
  ensureThtk: vi.fn(),
  run: vi.fn(),
  readReactFrontendArtifact: vi.fn(),
}));

vi.mock('../lib/host-workspace.mjs', () => ({inspectHostWorkspace: mocks.inspectHostWorkspace}));
vi.mock('../host/lib/python-environment.mjs', () => ({ensurePythonEnvironment: mocks.ensurePythonEnvironment}));
vi.mock('../host/lib/thtk.mjs', () => ({ensureThtk: mocks.ensureThtk}));
vi.mock('../host/lib/process.mjs', () => ({run: mocks.run}));
// TH20 has an unrelated pre-existing languages=true/no-Host-preparer mismatch.
// Isolate the TH05 Host branch while leaving its catalog and completeness checks real.
vi.mock('../lib/contracts/product-catalog.mjs', async importOriginal => {
  const actual = await importOriginal();
  return {...actual, PRODUCT_GAMES: Object.freeze({...actual.PRODUCT_GAMES,
    th20: Object.freeze({...actual.PRODUCT_GAMES.th20,
      features: Object.freeze({...actual.PRODUCT_GAMES.th20.features, languages: false})})})};
});
vi.mock('../lib/react-frontend-artifact.mjs', () => ({
  reactFrontendArtifactDirectory: () => '/fixture/react-client',
  readReactFrontendArtifact: mocks.readReactFrontendArtifact,
}));

import {buildHostedSite} from '../host/lib/site-builder.mjs';

let scratch;
const projectRoot = resolve(import.meta.dirname, '..');
const oldFrontend = process.env.EAGLER_FRONTEND;

afterEach(async () => {
  if (scratch) await rm(scratch, {recursive: true, force: true});
  scratch = undefined;
  if (oldFrontend === undefined) delete process.env.EAGLER_FRONTEND;
  else process.env.EAGLER_FRONTEND = oldFrontend;
  vi.clearAllMocks();
});

async function fixture({completeTh05}) {
  scratch = await mkdtemp(resolve(tmpdir(), 'eagler-th05-prepared-host-'));
  const hostRoot = resolve(scratch, 'host');
  const font = resolve(scratch, 'font.otf');
  await mkdir(hostRoot, {recursive: true});
  await writeFile(font, 'fixture font');
  const layout = {
    root: hostRoot,
    games: Object.fromEntries(Object.keys(PRODUCT_CONTENT).map(game => [game, resolve(hostRoot, 'games', game)])),
    music: ['midi', 'ogg'],
    bundledFonts: {unicode: font, japanese: font},
  };

  for (const [game, content] of Object.entries(PRODUCT_CONTENT)) {
    const prepared = content.hostPreparation?.preparedContent;
    if (!prepared || game === 'th05' && !completeTh05) continue;
    for (const marker of prepared.markerFiles) {
      const path = resolve(layout.games[game], prepared.directory, marker);
      await mkdir(dirname(path), {recursive: true});
      await writeFile(path, `${game}:${marker}`);
    }
  }

  mocks.inspectHostWorkspace.mockResolvedValue(layout);
  mocks.ensurePythonEnvironment.mockResolvedValue('fixture-python');
  mocks.ensureThtk.mockResolvedValue({thdat: 'fixture-thdat', thmsg: 'fixture-thmsg', thanm: 'fixture-thanm'});
  mocks.readReactFrontendArtifact.mockResolvedValue({mountPath: '/', appShell: null});
  mocks.run.mockImplementation(async (_command, args) => {
    if (args.some(value => String(value).includes('prepare-th05-language-pack.mjs'))) {
      return {code: 0, stdout: JSON.stringify({game: 'th05', skipped: true, languages: []}), stderr: ''};
    }
    if (args.some(value => String(value).endsWith('prepare-host-artwork.py'))) {
      throw new Error('stop after prepared-content stage');
    }
    return {code: 0, stdout: '', stderr: ''};
  });
  process.env.EAGLER_FRONTEND = 'react';
  return {layout, hostRoot};
}

test('Host requires the complete TH05 prepared content when its producer is unavailable', async () => {
  const {hostRoot} = await fixture({completeTh05: false});
  await assert.rejects(buildHostedSite({projectRoot, hostRoot}), /provide complete prepared content at .*assets-eagler \(DATA, music and artwork\)/);
  assert.equal(mocks.run.mock.calls.some(([, args]) => args.some(value => String(value).includes('prepare-th05-content.mjs'))), false,
    'Host must fail clearly instead of invoking an undeclared raw-content producer');
});

test('Host accepts and stages the full TH05 DATA, soundtrack, and artwork markers', async () => {
  const {layout, hostRoot} = await fixture({completeTh05: true});
  await assert.rejects(buildHostedSite({projectRoot, hostRoot}), /stop after prepared-content stage/);
  const supplied = PRODUCT_CONTENT.th05.hostPreparation.preparedContent;
  const staged = resolve(layout.root, '.cache', 'generated', 'th05');
  for (const marker of supplied.markerFiles) {
    assert.equal(await readFile(resolve(staged, marker), 'utf8'), `th05:${marker}`, `${marker} should be copied into Host staging`);
  }
});
