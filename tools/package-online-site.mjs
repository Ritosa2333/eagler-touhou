// TH128 online static-site builder; uploads and public-host selection remain external.
//
// Purpose: assemble a publishable static root for the one-link Web launch:
//   /index.html                      bytes of site/play.html (the game entry; NOT the inspector page)
//   /play.html, /play.css, /play.mjs, /import-entry.mjs, /resource-fetch.mjs, /runtime/**
//   /runtime/build.json              copied from the matching delivered runtime root
//   /runtime/builds/<generation>/    core.mjs + core.wasm, verified against build.json
//   /games/th128/th128-dat-ogg-<revision>.zip   the unchanged resource archive
//   /deployment.json                 {schema, game, resource{url,bytes,sha256,revision}}
//   /_headers                        static-provider cache/MIME rules (Netlify/Cloudflare style)
//   /DEPLOY.md                       portable nginx instructions and scope limits
//
// Scope rules enforced here:
//   * outputRoot must not exist (never overwrite an existing deployment root)
//   * only the reviewed whitelist below is copied; `site/runtime/imported-resources.mjs`
//     and any test/debug source are never copied
//   * the resource ZIP is copied byte-for-byte and re-hashed; its size/sha256 must equal
//     the recorded package manifest, and the manifest revision must be the exact archive revision
//   * build.json must describe the same generation / artifacts as the copied core files
//   * no service worker, no fetch monkeypatch, no upload, no domain logic, no runtime execution
//
// The module performs no network access and starts no server; it only reads local files and writes
// the output tree. It is synchronous on purpose so callers get a complete report or a thrown error.

import { createHash } from 'node:crypto';
import { mkdirSync, readFileSync, writeFileSync, existsSync, statSync, readdirSync } from 'node:fs';
import { dirname, join, posix, resolve, sep } from 'node:path';

export const DEPLOYMENT_SCHEMA = 'th128-online-deployment/1';
export const DEPLOYMENT_FILE = 'deployment.json';
export const RESOURCE_DIR = 'games/th128';
export const GAME_ID = 'th128';
export const DATA_LAYOUT = 'sha256-58cd45fe7cac9af281d9de993a527ccfbbe6f27b89bcd0ca98a7031333fce200';

// 36 fixed files of the delivered runtime root plus 3 generation-bound core files = 39 site entries;
// the 3 workstation helpers of the delivered ZIP (serve.py, Start.ps1, README.md) are intentionally
// not shipped to a public static root (see HANDOFF.md).
export const SITE_FILES = Object.freeze([
  'app.mjs',
  'import-entry.mjs',
  'index.html',
  'inspector.mjs',
  'play.css',
  'play.html',
  'play.mjs',
  'resource-fetch.mjs',
  'style.css',
  'runtime/background.mjs',
  'runtime/bullet-type9-profile.json',
  'runtime/crc32.mjs',
  'runtime/dat-ogg-provider.mjs',
  'runtime/dat-package-install.mjs',
  'runtime/draw.mjs',
  'runtime/ecl-includes.mjs',
  'runtime/ex-qualification.mjs',
  'runtime/extra-eligibility.mjs',
  'runtime/extra-menu.css',
  'runtime/extra-menu.mjs',
  'runtime/last-name-field.mjs',
  'runtime/native-ending-bridge-v6.mjs',
  'runtime/native-ending-browser-host-v4.mjs',
  'runtime/native-ending-pixels.mjs',
  'runtime/native-extra-run.mjs',
  'runtime/native-normal-postrun.mjs',
  'runtime/original-bullet-type33-record.mjs',
  'runtime/original-bullet-type33.mjs',
  'runtime/renderer-owned-ending-v2.mjs',
  'runtime/renderer.mjs',
  'runtime/replay.mjs',
  'runtime/sound.mjs',
  'runtime/th128-dat.mjs',
  'runtime/timing.mjs',
  'runtime/title-records.mjs',
  'runtime/zip-reader.mjs',
  'runtime/world-presentation.mjs',
  'runtime/web-input.mjs',
  'runtime/launcher-session.mjs',
  'runtime/page-launch-context.mjs',
  'runtime/managed-dat-owner.mjs',
]);

// REQUIRED online module: the automatic download/cache entry MAIN is implementing. A deployment
// without it would publish a local-ZIP-only page under an online URL, so the builder refuses to
// produce any output when it is missing instead of emitting a publishable-but-wrong site.
export const REQUIRED_ONLINE_FILES = Object.freeze(['runtime/online-resources.mjs']);

// Optional modules MAIN owns; copied only when present.
export const OPTIONAL_FILES = Object.freeze(['online-entry.mjs']);

// Present in some local roots but must never reach a public site root.
export const FORBIDDEN_FILES = Object.freeze(['runtime/imported-resources.mjs']);

export const WORKSTATION_HELPERS = Object.freeze(['serve.py', 'Start.ps1', 'README.md']);

const NO_CACHE = new Set([
  'index.html',
  'deployment.json',
  'play.html',
  'play.css',
  'play.mjs',
  'import-entry.mjs',
  'resource-fetch.mjs',
  'online-entry.mjs',
  'runtime/online-resources.mjs',
  'runtime/build.json',
]);

export class OnlineSiteError extends Error {
  constructor(message, details) {
    super(message);
    this.name = 'OnlineSiteError';
    this.details = details ?? null;
  }
}

function fail(message, details) {
  throw new OnlineSiteError(`online-site: ${message}`, details);
}

function sha256(bytes) {
  return createHash('sha256').update(bytes).digest('hex');
}

function readBytes(file, what) {
  let st;
  try {
    st = statSync(file);
  } catch {
    fail(`${what} is missing: ${file}`);
  }
  if (!st.isFile()) fail(`${what} is not a regular file: ${file}`);
  const bytes = readFileSync(file);
  if (bytes.length !== st.size) fail(`${what} changed while reading: ${file}`);
  return bytes;
}

function readJson(file, what) {
  const bytes = readBytes(file, what);
  try {
    return JSON.parse(bytes.toString('utf8'));
  } catch {
    fail(`${what} is not valid JSON: ${file}`);
  }
}

function assertSafeRelativePath(path, what) {
  if (typeof path !== 'string' || path.length === 0) fail(`${what} must be a non-empty string`);
  if (path.includes('\\') || path.includes('\0')) fail(`${what} is not a portable relative path: ${path}`);
  if (path.startsWith('/')) fail(`${what} must be relative: ${path}`);
  for (const part of path.split('/')) {
    if (part === '' || part === '.' || part === '..') fail(`${what} has an unsafe segment: ${path}`);
  }
}

function resolveInside(root, relative, what) {
  assertSafeRelativePath(relative, what);
  const abs = resolve(root, relative);
  if (abs !== root && !abs.startsWith(root + sep)) fail(`${what} escapes its root: ${relative}`);
  return abs;
}

function writeOutput(root, relative, bytes) {
  const abs = resolveInside(root, relative, 'output path');
  mkdirSync(dirname(abs), { recursive: true });
  writeFileSync(abs, bytes);
  return { path: relative, bytes: bytes.length, sha256: sha256(bytes) };
}

function listTree(root) {
  const out = [];
  const walk = (dir, prefix) => {
    for (const entry of readdirSync(dir, { withFileTypes: true }).sort((a, b) => a.name.localeCompare(b.name))) {
      const rel = prefix ? `${prefix}/${entry.name}` : entry.name;
      if (entry.isDirectory()) walk(join(dir, entry.name), rel);
      else if (entry.isFile()) out.push(rel);
      else fail(`unexpected non-regular entry in output tree: ${rel}`);
    }
  };
  walk(root, '');
  return out;
}

function normalizeResourceManifest(packageManifest) {
  if (!packageManifest || typeof packageManifest !== 'object') fail('packageManifest is required');
  const bytes = packageManifest.zipBytes ?? packageManifest.bytes;
  const sha = packageManifest.zipSha256 ?? packageManifest.sha256;
  const revision = packageManifest.revision;
  if (!Number.isSafeInteger(bytes) || bytes <= 0) fail('packageManifest bytes must be a positive integer');
  if (typeof sha !== 'string' || !/^[0-9a-f]{64}$/.test(sha)) fail('packageManifest sha256 must be 64 lowercase hex');
  if (typeof revision !== 'string' || !/^[0-9a-f]{16}$/.test(revision)) fail('packageManifest revision must be 16 lowercase hex');
  return { bytes, sha256: sha, revision };
}

function normalizeCoreBuild(coreBuild) {
  if (typeof coreBuild === 'string') {
    if (!/^[0-9a-f]{64}$/.test(coreBuild)) fail('coreBuild generation must be 64 lowercase hex');
    return { generation: coreBuild, core: null, wasm: null };
  }
  if (!coreBuild || typeof coreBuild !== 'object') fail('coreBuild is required');
  if (typeof coreBuild.generation !== 'string' || !/^[0-9a-f]{64}$/.test(coreBuild.generation)) {
    fail('coreBuild.generation must be 64 lowercase hex');
  }
  return {
    generation: coreBuild.generation,
    core: coreBuild.core ?? null,
    wasm: coreBuild.wasm ?? null,
  };
}

function checkArtifact(bytes, expected, what) {
  if (!expected) return;
  const wantBytes = expected.bytes;
  const wantSha = expected.sha256;
  if (Number.isSafeInteger(wantBytes) && bytes.length !== wantBytes) {
    fail(`${what} size mismatch: expected ${wantBytes}, got ${bytes.length}`);
  }
  if (typeof wantSha === 'string' && sha256(bytes) !== wantSha) {
    fail(`${what} sha256 mismatch against the requested core build`);
  }
}

function buildHeaders(files, resourcePath) {
  const lines = [
    '/*',
    '  Cache-Control: no-cache',
    '  X-Content-Type-Options: nosniff',
    "  Content-Security-Policy: default-src 'self'; script-src 'self' 'wasm-unsafe-eval'; style-src 'self'; img-src 'self' data: blob:; media-src 'self' blob:; connect-src 'self'; object-src 'none'; base-uri 'none'",
    '',
    '/*.mjs', '  Content-Type: text/javascript; charset=utf-8', '',
    '/runtime/*.mjs', '  Content-Type: text/javascript; charset=utf-8', '',
    '/*.css', '  Content-Type: text/css; charset=utf-8', '',
    '# TH128 online deployment headers (static providers that support `_headers`).',
    '# Portable alternative: see DEPLOY.md.',
    '',
  ];
  for (const path of [...files].sort()) {
    if (NO_CACHE.has(path)) lines.push(`/${path}`, '  Cache-Control: no-cache', '');
  }
  lines.push('/deployment.json', '  Cache-Control: no-cache', '');
  lines.push('/runtime/builds/*', '  Cache-Control: public, max-age=31536000, immutable', '');
  lines.push(`/${RESOURCE_DIR}/*`, '  Cache-Control: public, max-age=31536000, immutable', '');
  lines.push('/*.wasm', '  Content-Type: application/wasm', '');
  lines.push('/runtime/builds/*.wasm', '  Content-Type: application/wasm', '');
  lines.push(`/${resourcePath}`, '  Content-Type: application/zip', '');
  lines.push(`/${RESOURCE_DIR}/parts/*`, '  Content-Type: application/octet-stream', '');
  return lines.join('\n');
}

function buildDeployDoc({ deployment, core, resource, files }) {
  const noCache = [...files].filter((p) => NO_CACHE.has(p)).sort();
  return [
    '# TH128 online deployment (static root)',
    '',
    'This directory is a static site. It contains no upload step, no domain logic and no server code.',
    '',
    '## Required origin behaviour',
    '',
    '- Serve the tree at the origin root: `play.html` references `/play.css` and `/import-entry.mjs`,',
    '  and the runtime requests `/assets/...` on the same origin, so the site must not be nested under a sub-path.',
    '- Same-origin HTTPS is required: the resource archive is fetched from a same-origin path and the',
    '  runtime stores it with IndexedDB; cross-origin fetches are rejected by the runtime boundary.',
    ...(deployment.resource.parts ? ['- The archive is transported as verified binary parts, each at most 25 MiB. The browser combines them,', '  checks the complete original archive hash, then uses the same DAT installer. The virtual ZIP URL is not fetched.', '  Upload this whole root directory; no additional storage bucket or public resource ZIP is required.'] : []),
    '- `Content-Type: application/wasm` for `.wasm`, `text/javascript` for `.mjs`, `application/json` for `.json`.',
    '- `deployment.json`, `index.html`, `play.html` and the entry modules must be `Cache-Control: no-cache`.',
    `- \`/${RESOURCE_DIR}/*\` and \`/runtime/builds/*\` are content-addressed and may be cached immutably.`,
    '- The archive or its parts use verified full GET requests; byte ranges remain useful for HTTP diagnostics.',
    '- Serve ZIP files without an extra Content-Encoding layer so Content-Length matches the pinned archive bytes.',
    '',
    '## deployment.json',
    '',
    '```json',
    JSON.stringify(deployment, null, 2),
    '```',
    '',
    `- generation: \`${core.generation}\``,
    `- resource archive: \`/${resource.path}\` (${resource.bytes} bytes, sha256 \`${resource.sha256}\`, revision \`${resource.revision}\`)`,
    '',
    '## nginx',
    '',
    '```nginx',
    'server {',
    '    listen 443 ssl;',
    '    server_name example.invalid;   # replace with the real host',
    '    root /srv/th128;',
    '',
    '    include mime.types;',
    '    types { application/wasm wasm; text/javascript mjs; application/json json; text/html html; text/css css; application/zip zip; }',
    '    index index.html;',
    '    default_type application/octet-stream;',
    '',
    '    location = /deployment.json { add_header Cache-Control "no-cache" always; }',
    '    location = /index.html      { add_header Cache-Control "no-cache" always; }',
    '    location = /play.html       { add_header Cache-Control "no-cache" always; }',
    `    location /${RESOURCE_DIR}/  { add_header Cache-Control "public, max-age=31536000, immutable" always; }`,
    '    location /runtime/builds/   { add_header Cache-Control "public, max-age=31536000, immutable" always; }',
    '    location / { add_header Cache-Control "no-cache" always; try_files $uri $uri/ =404; }',
    '}',
    '```',
    '',
    '## Cache policy summary',
    '',
    `- no-cache: ${noCache.join(', ')}`,
    `- immutable: /${RESOURCE_DIR}/*, /runtime/builds/*`,
    '',
    '## Scope limits',
    '',
    '- No compatibility promise is made for arbitrary old browsers; current Chromium-class engines with',
    '  WebAssembly, module scripts and IndexedDB are assumed.',
    '- No physical-audio claim is made; the runtime uses WebAudio and device output is untested here.',
    '- The first visit downloads the whole archive once (93 MB). IndexedDB quota, private-mode denial and',
    '  single-blob memory behaviour are environment acceptances, not source guarantees.',
    '- Browser autoplay policy may still require an existing user gesture for audio.',
    '- Cache-busting/reload guidance: after a deployment change, reload the page once; a second module',
    '  instance would create a second owner, so no cache-bust parameter is injected.',
    '',
  ].join('\n');
}

/**
 * Build a publishable static root. Throws OnlineSiteError on any violated precondition.
 *
 * @param {object} input
 * @param {string} input.siteRoot        reviewed `site/` directory of the delivery
 * @param {string} input.coreRoot        delivered runtime root (contains `site/runtime/build.json`)
 * @param {string|object} input.coreBuild expected generation, or {generation, core?, wasm?} pins
 * @param {number} input.resourceChunkBytes optional transport-part size; zero keeps the single archive
 * @param {string} input.resourceZip     the unchanged DAT+OGG resource archive
 * @param {string} input.outputRoot      new directory to create (must not exist)
 * @param {string|object} input.packageManifest path to the package manifest JSON, or the manifest object
 * @returns {{outputRoot:string, files:Array, deployment:object, resource:object, core:object, skipped:string[]}}
 */
export function buildOnlineSite({ siteRoot, coreRoot, coreBuild, resourceZip, outputRoot, packageManifest, resourceChunkBytes = 0 } = {}) {
  if (!siteRoot) fail('siteRoot is required');
  if (!coreRoot) fail('coreRoot is required');
  if (!resourceZip) fail('resourceZip is required');
  if (!outputRoot) fail('outputRoot is required');
  const site = resolve(siteRoot);
  const core = resolve(coreRoot);
  const zip = resolve(resourceZip);
  const out = resolve(outputRoot);
  if (!existsSync(site)) fail(`siteRoot does not exist: ${site}`);
  if (!existsSync(core)) fail(`coreRoot does not exist: ${core}`);
  if (!existsSync(zip)) fail(`resourceZip does not exist: ${zip}`);
  if (existsSync(out)) fail(`outputRoot already exists and must not be overwritten: ${out}`);
  if (!Number.isSafeInteger(resourceChunkBytes) || resourceChunkBytes < 0 || resourceChunkBytes > 25 * 1024 * 1024) {
    fail('resource chunk size must be zero or a positive integer up to 25 MiB');
  }

  const build = normalizeCoreBuild(coreBuild);
  const resource = normalizeResourceManifest(
    typeof packageManifest === 'string' ? readJson(resolve(packageManifest), 'packageManifest') : packageManifest,
  );

  // 1. Required site files.
  const payload = new Map();
  for (const relative of SITE_FILES) {
    const source = resolveInside(site, relative, 'site file');
    payload.set(relative, readBytes(source, `site file ${relative}`));
  }
  // 1b. REQUIRED online module (MAIN-owned automatic download/cache entry).
  const requiredOnline = [];
  for (const relative of REQUIRED_ONLINE_FILES) {
    const source = resolveInside(site, relative, 'required online module');
    if (!existsSync(source)) {
      fail(
        `required online module ${relative} is missing; an online deployment needs the automatic download/cache entry`,
      );
    }
    const bytes = readBytes(source, `required online module ${relative}`);
    payload.set(relative, bytes);
    requiredOnline.push({ path: relative, bytes: bytes.length, sha256: sha256(bytes) });
  }
  // FORBIDDEN_FILES are simply absent from SITE_FILES/OPTIONAL_FILES, so they can never be copied.

  // 2. Optional MAIN-owned modules.
  const optional = [];
  for (const relative of OPTIONAL_FILES) {
    const source = resolve(site, relative);
    if (existsSync(source)) {
      payload.set(relative, readBytes(source, `optional site file ${relative}`));
      optional.push(relative);
    }
  }
  // 3. Core build metadata and artifacts.
  const buildJsonBytes = readBytes(join(core, 'site', 'runtime', 'build.json'), 'core build.json');
  const buildJson = JSON.parse(buildJsonBytes.toString('utf8'));
  if (buildJson.game !== GAME_ID) fail(`core build.json game must be ${GAME_ID}`);
  if (buildJson.generation !== build.generation) {
    fail(`core generation mismatch: build.json ${buildJson.generation} vs requested ${build.generation}`);
  }
  const entry = `runtime/builds/${build.generation}/core.mjs`;
  if (buildJson.entry !== entry) fail(`core build.json entry must be ${entry}`);
  const artifacts = buildJson.artifacts ?? {};
  const coreBytes = readBytes(join(core, 'site', 'runtime', 'builds', build.generation, 'core.mjs'), 'core.mjs');
  const wasmBytes = readBytes(join(core, 'site', 'runtime', 'builds', build.generation, 'core.wasm'), 'core.wasm');
  checkArtifact(coreBytes, artifacts['core.mjs'], 'core.mjs');
  checkArtifact(wasmBytes, artifacts['core.wasm'], 'core.wasm');
  checkArtifact(coreBytes, build.core, 'core.mjs');
  checkArtifact(wasmBytes, build.wasm, 'core.wasm');
  if (buildJson.packageDataLayout && buildJson.packageDataLayout !== DATA_LAYOUT) {
    fail(`core build.json packageDataLayout is not the reviewed DAT+OGG layout: ${buildJson.packageDataLayout}`);
  }

  // 4. Resource archive identity.
  const zipBytes = readBytes(zip, 'resource archive');
  if (zipBytes.length !== resource.bytes) {
    fail(`resource archive size mismatch: manifest ${resource.bytes}, file ${zipBytes.length}`);
  }
  const zipSha = sha256(zipBytes);
  if (zipSha !== resource.sha256) fail(`resource archive sha256 mismatch: manifest ${resource.sha256}, file ${zipSha}`);

  if (resourceChunkBytes && Math.ceil(zipBytes.length / resourceChunkBytes) > 64) fail('resource chunk size creates too many parts');

  // 5. Write the tree.
  mkdirSync(dirname(out), { recursive: true });
  mkdirSync(out);
  const written = [];
  const skipped = [];
  for (const [relative, bytes] of [...payload].sort((a, b) => a[0].localeCompare(b[0]))) {
    if (relative === 'index.html') {
      skipped.push('index.html');
      continue;
    }
    written.push(writeOutput(out, relative, bytes));
  }
  written.push(writeOutput(out, 'index.html', payload.get('play.html')));
  written.push(writeOutput(out, 'runtime/build.json', buildJsonBytes));
  written.push(writeOutput(out, entry, coreBytes));
  written.push(writeOutput(out, `runtime/builds/${build.generation}/core.wasm`, wasmBytes));
  const resourcePath = `${RESOURCE_DIR}/th128-dat-ogg-${resource.revision}.zip`;
  const parts = [];
  if (resourceChunkBytes) {
    for (let offset = 0; offset < zipBytes.length; offset += resourceChunkBytes) {
      const body = zipBytes.subarray(offset, Math.min(offset + resourceChunkBytes, zipBytes.length));
      const digest = sha256(body);
      const chunkPath = `${RESOURCE_DIR}/parts/${resource.revision}/${digest}.bin`;
      const row = writeOutput(out, chunkPath, body);
      written.push(row);
      parts.push({ url: '/' + chunkPath, bytes: row.bytes, sha256: row.sha256 });
    }
  } else written.push(writeOutput(out, resourcePath, zipBytes));

  const deployment = {
    schema: DEPLOYMENT_SCHEMA,
    game: GAME_ID,
    resource: {
      url: `/${resourcePath}`,
      bytes: resource.bytes,
      sha256: resource.sha256,
      revision: resource.revision,
      ...(parts.length ? { parts } : {}),
    },
  };
  written.push(writeOutput(out, DEPLOYMENT_FILE, Buffer.from(`${JSON.stringify(deployment, null, 2)}\n`, 'utf8')));

  const copied = written.map((row) => row.path);
  written.push(writeOutput(out, '_headers', Buffer.from(`${buildHeaders(copied, resourcePath)}\n`, 'utf8')));
  written.push(
    writeOutput(
      out,
      'DEPLOY.md',
      Buffer.from(
        `${buildDeployDoc({ deployment, core: { generation: build.generation }, resource: { ...resource, path: resourcePath }, files: copied })}\n`,
        'utf8',
      ),
    ),
  );

  const referenced = new Set(written.map((row) => row.path));
  for (const relative of SITE_FILES) {
    if (relative === 'index.html') continue;
    if (!referenced.has(posix.normalize(relative))) fail(`internal error: ${relative} was not written`);
  }
  for (const relative of REQUIRED_ONLINE_FILES) {
    if (!referenced.has(posix.normalize(relative))) fail(`internal error: required ${relative} was not written`);
  }
  for (const helper of WORKSTATION_HELPERS) {
    if (existsSync(join(site, '..', helper))) skipped.push(helper);
  }
  const onDisk = listTree(out);
  const expected = [...referenced].sort((a, b) => a.localeCompare(b));
  if (onDisk.length !== expected.length || onDisk.some((path, index) => path !== expected[index])) {
    fail(`output tree does not match the written report: ${onDisk.length} files on disk`);
  }

  return {
    outputRoot: out,
    files: written,
    deployment,
    resource: { path: resourcePath, bytes: resource.bytes, sha256: resource.sha256, revision: resource.revision },
    core: { generation: build.generation, entry, exports: buildJson.exports ?? null },
    optional,
    requiredOnline,
    skipped,
  };
}

export function readDeployment(root) {
  return readJson(join(resolve(root), DEPLOYMENT_FILE), 'deployment.json');
}
