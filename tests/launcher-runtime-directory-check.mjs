import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {existsSync, readFileSync, readdirSync} from 'node:fs';
import {join, posix, resolve} from 'node:path';
import test from 'node:test';

const [coreSite, out, upstreamShell, upstreamWire] = process.argv.slice(2);
assert.ok(coreSite && out && upstreamShell && upstreamWire, 'Need fixed Core122, fresh output and the pinned upstream protocol files');
const script = new URL('../tools/package-launcher-runtime.mjs', import.meta.url);
const read = (root, path) => readFileSync(join(root, path));
const text = (root, path) => new TextDecoder('utf-8', {fatal: true}).decode(read(root, path));
const sha = bytes => createHash('sha256').update(bytes).digest('hex');

test('the new directory carries the current page, unchanged Core122 and no original-content files', async () => {
  assert.ok(existsSync(script), 'the Launcher directory packager is missing');
  const {packageLauncherRuntime} = await import(script);
  await packageLauncherRuntime({coreSite, out});
  const descriptor = JSON.parse(text(out, 'runtime-files.json'));
  const actual = readdirSync(out, {recursive: true, withFileTypes: true}).filter(entry => entry.isFile())
    .map(entry => resolve(entry.parentPath, entry.name).slice(resolve(out).length + 1).replaceAll('\\', '/')).sort();
  assert.equal(descriptor.schema, 'eagler-touhou/runtime-directory/1');
  assert.deepEqual(actual, ['runtime-files.json', ...Object.keys(descriptor.files)].sort());
  for (const [file, identity] of Object.entries(descriptor.files)) {
    assert.match(file, /^(?:[a-z0-9._-]+\/)*[a-z0-9._-]+\.(?:html|mjs|css|wasm|json|txt)$/);
    const bytes = read(out, file);
    assert.deepEqual(identity, {bytes:bytes.length,sha256:sha(bytes)}, file);
  }
  const entry = text(out, 'th128.html');
  assert.match(entry, /<meta name="eagler-data-provider" content="retail-memory">/);
  assert.match(entry, /src="\.\/import-entry\.mjs"/);
  assert.match(text(out, 'import-entry.mjs'), /openManagedDatPackage/);
  assert.ok(existsSync(join(out, 'licenses/eagler-touhou-launcher-gpl-3.0.txt')));
  assert.ok(existsSync(join(out, 'licenses/thprac-mit.txt')));
  assert.ok(!existsSync(join(out, 'runtime/th128-cover.png')));
  const build = JSON.parse(text(out, 'runtime/build.json'));
  assert.equal(build.generation, 'd491ae9dda125966ef873df62779bcbf6aeddd09dc2c09d04eb8afe435aa6f63');
  assert.deepEqual(read(out, build.entry), read(coreSite, build.entry));
  assert.deepEqual(read(out, build.entry.replace('core.mjs','core.wasm')), read(coreSite, build.entry.replace('core.mjs','core.wasm')));
  const sources = actual.filter(file => file.endsWith('.mjs') && file !== build.entry);
  for (const file of sources) {
    const code = text(out, file);
    for (const match of code.matchAll(/(?:\bfrom\s*|\bimport\s*\(\s*)['"](\.[^'"]+)['"]/g)) {
      const target = posix.normalize(posix.join(posix.dirname(file), match[1]));
      assert.ok(Object.hasOwn(descriptor.files, target), `${file} imports unknown ${target}`);
    }
  }
  await assert.rejects(() => packageLauncherRuntime({coreSite, out}), /already exists/);
});

test('upstream source checks recognize the actual session envelope and managed DATA bridge', async () => {
  // Use the fixed upstream assertions and its actual epoch constant without
  // materializing its generated TypeScript package for this offline check.
  const code = readFileSync(upstreamShell, 'utf8'), wire = readFileSync(upstreamWire, 'utf8');
  const constant = wire.match(/^export const RUNTIME_EPOCH_QUERY_PARAMETER = "([a-zA-Z]+)";$/m)?.[1];
  assert.ok(constant);
  const input = code.replace(/^import \{ RUNTIME_EPOCH_QUERY_PARAMETER \} from "\.\/contracts\/runtime-protocol\.mjs";$/m,
    `const RUNTIME_EPOCH_QUERY_PARAMETER = ${JSON.stringify(constant)};`);
  assert.notEqual(input, code);
  const {assertRuntimeProtocolSources} = await import(`data:text/javascript;base64,${Buffer.from(input).toString('base64')}`);
  const root = existsSync(join(out, 'runtime-files.json')) ? out : new URL('../site/', import.meta.url);
  const sources = ['runtime/launcher-session.mjs','runtime/managed-dat-owner.mjs'].map(file =>
    typeof root === 'string' ? text(root, file) : readFileSync(new URL(file, root), 'utf8'));
  assertRuntimeProtocolSources(sources, 'th128', 'normal');
});
