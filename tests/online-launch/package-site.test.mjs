// Bounded tests for tools/package-online-site.mjs (no network, no server, no product runtime).
// Fixtures are tiny synthetic trees in the OS temp directory; the real 42-file delivery is not required.

import { test, before, after } from 'node:test';
import assert from 'node:assert/strict';
import { createHash } from 'node:crypto';
import { mkdtempSync, mkdirSync, writeFileSync, readFileSync, rmSync, cpSync, existsSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

import {
  buildOnlineSite,
  readDeployment,
  OnlineSiteError,
  SITE_FILES,
  DEPLOYMENT_SCHEMA,
  RESOURCE_DIR,
} from '../../tools/package-online-site.mjs';

const GENERATION = 'a'.repeat(64);
const ZIP_BYTES = Buffer.from('synthetic-th128-resource-archive-for-tests');
const ZIP_SHA = createHash('sha256').update(ZIP_BYTES).digest('hex');
const REVISION = '0123456789abcdef';

let root;
let base;

function sha(bytes) {
  return createHash('sha256').update(bytes).digest('hex');
}

function write(file, bytes) {
  mkdirSync(join(file, '..'), { recursive: true });
  writeFileSync(file, bytes);
}

function makeFixture(dir) {
  const siteRoot = join(dir, 'delivery', 'site');
  for (const relative of SITE_FILES) {
    const bytes = relative === 'index.html' ? Buffer.from('inspector-index') : Buffer.from(`content:${relative}\n`);
    write(join(siteRoot, relative), bytes);
  }
  write(join(siteRoot, 'runtime', 'imported-resources.mjs'), Buffer.from('unused-module'));
  write(join(siteRoot, 'runtime', 'online-resources.mjs'), Buffer.from('online-module'));
  write(join(dir, 'delivery', 'serve.py'), Buffer.from('# workstation helper'));
  write(join(dir, 'delivery', 'Start.ps1'), Buffer.from('# workstation helper'));
  write(join(dir, 'delivery', 'README.md'), Buffer.from('# workstation helper'));

  const coreRoot = join(dir, 'runtime-root');
  const coreDir = join(coreRoot, 'site', 'runtime', 'builds', GENERATION);
  const coreMjs = Buffer.from('export default function factory(){return {};}\n');
  const coreWasm = Buffer.from('synthetic-wasm');
  write(join(coreDir, 'core.mjs'), coreMjs);
  write(join(coreDir, 'core.wasm'), coreWasm);
  const buildJson = {
    game: 'th128',
    privateCore: 109,
    generation: GENERATION,
    entry: `runtime/builds/${GENERATION}/core.mjs`,
    artifacts: {
      'core.mjs': { bytes: coreMjs.length, sha256: sha(coreMjs) },
      'core.wasm': { bytes: coreWasm.length, sha256: sha(coreWasm) },
    },
    exports: 205,
    packageDataLayout: 'sha256-58cd45fe7cac9af281d9de993a527ccfbbe6f27b89bcd0ca98a7031333fce200',
  };
  write(join(coreRoot, 'site', 'runtime', 'build.json'), Buffer.from(JSON.stringify(buildJson, null, 2)));

  const resourceZip = join(dir, 'resource.zip');
  writeFileSync(resourceZip, ZIP_BYTES);

  const manifest = { authority: 'MAIN', ordinal: 674, revision: REVISION, zipBytes: ZIP_BYTES.length, zipSha256: ZIP_SHA };
  const manifestPath = join(dir, 'package-manifest.json');
  write(manifestPath, Buffer.from(JSON.stringify(manifest, null, 2)));

  return { siteRoot, coreRoot, coreGeneration: GENERATION, resourceZip, manifestPath, manifest };
}

function build(dir, overrides = {}) {
  const fixture = makeFixture(dir);
  const outputRoot = join(dir, 'out');
  const report = buildOnlineSite({
    siteRoot: fixture.siteRoot,
    coreRoot: fixture.coreRoot,
    coreBuild: { generation: fixture.coreGeneration },
    resourceZip: fixture.resourceZip,
    outputRoot,
    packageManifest: fixture.manifestPath,
    ...overrides,
  });
  return { ...fixture, outputRoot, report };
}

before(() => {
  base = mkdtempSync(join(tmpdir(), 'th128-online-site-'));
});

after(() => {
  if (base) rmSync(base, { recursive: true, force: true });
});

let seq = 0;

test('assembles a complete static root with the game entry as index.html', () => {
  root = join(base, `case-${++seq}`);
  mkdirSync(root, { recursive: true });
  const { outputRoot, report, fixture } = (() => {
    const f = build(root);
    return { outputRoot: f.outputRoot, report: f.report, fixture: f };
  })();

  assert.equal(existsSync(outputRoot), true);
  assert.equal(report.outputRoot, outputRoot);
  assert.equal(report.deployment.schema, DEPLOYMENT_SCHEMA);
  assert.equal(report.deployment.game, 'th128');
  const deployment = readDeployment(outputRoot);
  assert.deepEqual(deployment, {
    schema: DEPLOYMENT_SCHEMA,
    game: 'th128',
    resource: {
      url: `/games/th128/th128-dat-ogg-${REVISION}.zip`,
      bytes: ZIP_BYTES.length,
      sha256: ZIP_SHA,
      revision: REVISION,
    },
  });
  assert.deepEqual(report.deployment, deployment);

  // index.html is the play page, not the inspector page
  assert.equal(
    readFileSync(join(outputRoot, 'index.html'), 'utf8'),
    readFileSync(join(fixture.siteRoot, 'play.html'), 'utf8'),
  );
  assert.equal(
    readFileSync(join(outputRoot, 'play.html'), 'utf8'),
    readFileSync(join(fixture.siteRoot, 'play.html'), 'utf8'),
  );
  assert.notEqual(readFileSync(join(outputRoot, 'index.html'), 'utf8'), 'inspector-index');
  assert.equal(existsSync(join(outputRoot, 'inspector.mjs')), true);

  // never copies the unused module, does copy the MAIN-owned online module
  assert.equal(existsSync(join(outputRoot, 'runtime', 'imported-resources.mjs')), false);
  assert.equal(existsSync(join(outputRoot, 'runtime', 'online-resources.mjs')), true);
  assert.deepEqual(report.optional, []);
  assert.deepEqual(report.requiredOnline.map((row) => row.path), ['runtime/online-resources.mjs']);

  // core files and build.json
  assert.deepEqual(readFileSync(join(outputRoot, 'runtime', 'builds', GENERATION, 'core.mjs')), readFileSync(join(fixture.coreRoot, 'site', 'runtime', 'builds', GENERATION, 'core.mjs')));
  assert.equal((readFileSync(join(outputRoot, 'runtime', 'build.json'), 'utf8')).includes(GENERATION), true);
  assert.equal(report.core.generation, GENERATION);

  // resource archive and manifest
  assert.deepEqual(
    readFileSync(join(outputRoot, RESOURCE_DIR, `th128-dat-ogg-${REVISION}.zip`)),
    ZIP_BYTES,
  );
  assert.equal(sha(readFileSync(join(outputRoot, RESOURCE_DIR, `th128-dat-ogg-${REVISION}.zip`))), ZIP_SHA);

  // headers and docs
  const headers = readFileSync(join(outputRoot, '_headers'), 'utf8');
  assert.match(headers, /\/deployment\.json\n {2}Cache-Control: no-cache/);
  assert.match(headers, /\/runtime\/builds\/\*\n {2}Cache-Control: public, max-age=31536000, immutable/);
  assert.match(headers, /\*\.wasm\n {2}Content-Type: application\/wasm/);
  const deploy = readFileSync(join(outputRoot, 'DEPLOY.md'), 'utf8');
  assert.match(deploy, /nginx/);
  assert.match(deploy, /application\/wasm/);
  assert.match(deploy, new RegExp(ZIP_SHA));

  // report is self-consistent with the tree
  for (const row of report.files) {
    const bytes = readFileSync(join(outputRoot, row.path));
    assert.equal(bytes.length, row.bytes, row.path);
    assert.equal(sha(bytes), row.sha256, row.path);
  }
  assert.equal(report.skipped.includes('index.html'), true);
  assert.equal(report.skipped.includes('serve.py'), true);
});

test('refuses to overwrite an existing output root', () => {
  const dir = join(base, `case-${++seq}`);
  const fixture = makeFixture(dir);
  const outputRoot = join(dir, 'out');
  mkdirSync(outputRoot, { recursive: true });
  assert.throws(
    () => buildOnlineSite({
      siteRoot: fixture.siteRoot,
      coreRoot: fixture.coreRoot,
      coreBuild: fixture.coreGeneration,
      resourceZip: fixture.resourceZip,
      outputRoot,
      packageManifest: fixture.manifest,
    }),
    /already exists and must not be overwritten/,
  );
});

test('rejects a resource archive with the wrong sha256', () => {
  const dir = join(base, `case-${++seq}`);
  const fixture = makeFixture(dir);
  const manifest = { ...fixture.manifest, zipSha256: 'f'.repeat(64) };
  assert.throws(
    () => buildOnlineSite({
      siteRoot: fixture.siteRoot,
      coreRoot: fixture.coreRoot,
      coreBuild: fixture.coreGeneration,
      resourceZip: fixture.resourceZip,
      outputRoot: join(dir, 'out'),
      packageManifest: manifest,
    }),
    /resource archive sha256 mismatch/,
  );
});

test('rejects a resource archive with the wrong byte count', () => {
  const dir = join(base, `case-${++seq}`);
  const fixture = makeFixture(dir);
  const manifest = { ...fixture.manifest, zipBytes: ZIP_BYTES.length + 1 };
  assert.throws(
    () => buildOnlineSite({
      siteRoot: fixture.siteRoot,
      coreRoot: fixture.coreRoot,
      coreBuild: fixture.coreGeneration,
      resourceZip: fixture.resourceZip,
      outputRoot: join(dir, 'out'),
      packageManifest: manifest,
    }),
    /resource archive size mismatch/,
  );
});

test('rejects a core generation that does not match build.json', () => {
  const dir = join(base, `case-${++seq}`);
  const fixture = makeFixture(dir);
  assert.throws(
    () => buildOnlineSite({
      siteRoot: fixture.siteRoot,
      coreRoot: fixture.coreRoot,
      coreBuild: 'b'.repeat(64),
      resourceZip: fixture.resourceZip,
      outputRoot: join(dir, 'out'),
      packageManifest: fixture.manifest,
    }),
    /core generation mismatch/,
  );
});

test('rejects a core artifact whose bytes do not match build.json', () => {
  const dir = join(base, `case-${++seq}`);
  const fixture = makeFixture(dir);
  const coreFile = join(fixture.coreRoot, 'site', 'runtime', 'builds', GENERATION, 'core.mjs');
  writeFileSync(coreFile, Buffer.from('tampered'));
  assert.throws(
    () => buildOnlineSite({
      siteRoot: fixture.siteRoot,
      coreRoot: fixture.coreRoot,
      coreBuild: fixture.coreGeneration,
      resourceZip: fixture.resourceZip,
      outputRoot: join(dir, 'out'),
      packageManifest: fixture.manifest,
    }),
    /core\.mjs size mismatch/,
  );
});

test('rejects a delivery missing a whitelisted site file', () => {
  const dir = join(base, `case-${++seq}`);
  const fixture = makeFixture(dir);
  rmSync(join(fixture.siteRoot, 'play.mjs'));
  assert.throws(
    () => buildOnlineSite({
      siteRoot: fixture.siteRoot,
      coreRoot: fixture.coreRoot,
      coreBuild: fixture.coreGeneration,
      resourceZip: fixture.resourceZip,
      outputRoot: join(dir, 'out'),
      packageManifest: fixture.manifest,
    }),
    /site file play\.mjs is missing/,
  );
});

test('rejects a delivery without the required online module instead of publishing a local-only site', () => {
  const dir = join(base, `case-${++seq}`);
  const fixture = makeFixture(dir);
  rmSync(join(fixture.siteRoot, 'runtime', 'online-resources.mjs'));
  assert.throws(
    () => buildOnlineSite({
      siteRoot: fixture.siteRoot,
      coreRoot: fixture.coreRoot,
      coreBuild: fixture.coreGeneration,
      resourceZip: fixture.resourceZip,
      outputRoot: join(dir, 'out'),
      packageManifest: fixture.manifest,
    }),
    /required online module runtime\/online-resources\.mjs is missing/,
  );
  assert.equal(existsSync(join(dir, 'out')), false);
});

test('rejects a manifest without an exact archive revision', () => {
  const dir = join(base, `case-${++seq}`);
  const fixture = makeFixture(dir);
  assert.throws(
    () => buildOnlineSite({
      siteRoot: fixture.siteRoot,
      coreRoot: fixture.coreRoot,
      coreBuild: fixture.coreGeneration,
      resourceZip: fixture.resourceZip,
      outputRoot: join(dir, 'out'),
      packageManifest: { ...fixture.manifest, revision: 'HEAD' },
    }),
    /revision must be 16 lowercase hex/,
  );
});

test('rejects a manifest sha256 that is not 64 lowercase hex', () => {
  const dir = join(base, `case-${++seq}`);
  const fixture = makeFixture(dir);
  assert.throws(
    () => buildOnlineSite({
      siteRoot: fixture.siteRoot,
      coreRoot: fixture.coreRoot,
      coreBuild: fixture.coreGeneration,
      resourceZip: fixture.resourceZip,
      outputRoot: join(dir, 'out'),
      packageManifest: { ...fixture.manifest, zipSha256: ZIP_SHA.toUpperCase() },
    }),
    /sha256 must be 64 lowercase hex/,
  );
});

test('two builds of the same inputs produce identical trees', () => {
  const dir = join(base, `case-${++seq}`);
  const fixture = makeFixture(dir);
  const first = buildOnlineSite({
    siteRoot: fixture.siteRoot,
    coreRoot: fixture.coreRoot,
    coreBuild: fixture.coreGeneration,
    resourceZip: fixture.resourceZip,
    outputRoot: join(dir, 'out-a'),
    packageManifest: fixture.manifest,
  });
  const second = buildOnlineSite({
    siteRoot: fixture.siteRoot,
    coreRoot: fixture.coreRoot,
    coreBuild: fixture.coreGeneration,
    resourceZip: fixture.resourceZip,
    outputRoot: join(dir, 'out-b'),
    packageManifest: fixture.manifest,
  });
  const strip = (report) => report.files.map((row) => `${row.path}:${row.bytes}:${row.sha256}`);
  assert.deepEqual(strip(first), strip(second));
});

test('throws a typed error that callers can distinguish', () => {
  const dir = join(base, `case-${++seq}`);
  const fixture = makeFixture(dir);
  try {
    buildOnlineSite({
      siteRoot: fixture.siteRoot,
      coreRoot: fixture.coreRoot,
      coreBuild: fixture.coreGeneration,
      resourceZip: join(dir, 'missing.zip'),
      outputRoot: join(dir, 'out'),
      packageManifest: fixture.manifest,
    });
    assert.fail('expected a throw');
  } catch (error) {
    assert.equal(error instanceof OnlineSiteError, true);
    assert.match(error.message, /resourceZip does not exist/);
  }
});
