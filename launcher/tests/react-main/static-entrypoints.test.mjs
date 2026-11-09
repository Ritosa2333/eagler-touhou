/** Build-time original HTML addresses and preview delivery. */
import {test} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, mkdir, writeFile, readFile, rm, stat} from 'node:fs/promises';
import {resolve} from 'node:path';
import {hasUiReviewEntry, uiReviewEnabled} from '../../lib/frontend-static-manifest.mjs';
import {resolveReleaseFeatures} from '../../app/release-features.ts';
import {finalizeOriginalEntrypoints} from '../../scripts/ui-rewrite/static-entrypoints.ts';

const flatFiles = [
  'en.html', 'ja.html', 'lobby.html', 'profile.html', 'login.html', 'faq.html', 'about.html',
  'guide.html', 'multiplayer.html', 'privacy.html',
];
const html = '<script>window.__reactRouterContext = {}; window.__reactRouterRouteModules = {};</script>';

async function prerender(root, path, body = html + path) {
  const directory = resolve(root, path);
  await mkdir(directory, {recursive: true});
  await writeFile(resolve(directory, 'index.html'), body);
}

for (const preview of [false, true]) {
  test(`published documents keep hydration bytes at a nested mount with previews ${preview ? 'on' : 'off'}`, async () => {
    const root = await mkdtemp(resolve('.cache/static-entries-'));
    try {
      const mount = resolve(root, 'nested');
      const documents = [...flatFiles, ...(preview ? ['ui-review.html'] : []), 'uptime', 'u/account-profile-template'];
      for (const path of documents) await prerender(mount, path);
      await writeFile(resolve(root, 'index.html'), 'unchanged fallback');
      await finalizeOriginalEntrypoints(root, '/nested/', preview);
      for (const file of flatFiles) assert.equal(await readFile(resolve(root, file), 'utf8'), html + file);
      assert.equal(await readFile(resolve(root, 'uptime/index.html'), 'utf8'), html + 'uptime');
      assert.equal(await readFile(resolve(root, 'profile-entry.html'), 'utf8'), html + 'u/account-profile-template');
      assert.equal(await readFile(resolve(root, 'index.html'), 'utf8'), 'unchanged fallback');
      await assert.rejects(stat(resolve(mount, 'uptime/index.html')), {code: 'ENOENT'});
      if (preview) assert.equal(await readFile(resolve(root, 'ui-review.html'), 'utf8'), html + 'ui-review.html');
      else await assert.rejects(stat(resolve(root, 'ui-review.html')), {code: 'ENOENT'});
    } finally {
      await rm(root, {recursive: true, force: true});
    }
  });
}

test('the production feature switches enable accounts and music independently of previews', () => {
  const prod = {NODE_ENV: 'production', DEV: false};
  assert.equal(uiReviewEnabled(prod), false);
  assert.equal(uiReviewEnabled({...prod, VITE_PREVIEW_FEATURES: 'true'}), true);
  assert.equal(uiReviewEnabled({NODE_ENV: 'development', VITE_PREVIEW_FEATURES: 'false'}), false);
  assert.equal(hasUiReviewEntry({other: {src: 'app/routes/ui-review.tsx'}}), false);
  assert.equal(hasUiReviewEntry({review: {src: 'app/routes/ui-review.tsx?route', isEntry: true}}), true);
  const features = resolveReleaseFeatures({...prod, VITE_ACCOUNTS_ENABLED: 'true', VITE_LISTEN_TOGETHER_ENABLED: 'true'});
  assert.equal(features.previewFeaturesEnabled, false);
  assert.equal(features.accountsEnabled, true);
  assert.equal(features.listenTogetherEnabled, true);
  assert.equal(features.isPreviewOnlyPage('/ui-review.html'), true);
  assert.equal(features.isPreviewOnlyPage('/u/player'), false);
});

test('unexpected directory content is preserved and rejected instead of removed', async () => {
  const root = await mkdtemp(resolve('.cache/static-entries-'));
  try {
    await prerender(root, 'en.html', html);
    await writeFile(resolve(root, 'en.html/keep.txt'), 'keep');
    await assert.rejects(finalizeOriginalEntrypoints(root), /Unexpected prerender/);
    assert.equal(await readFile(resolve(root, 'en.html/keep.txt'), 'utf8'), 'keep');
    assert.equal(await readFile(resolve(root, 'en.html/index.html'), 'utf8'), html);
  } finally {
    await rm(root, {recursive: true, force: true});
  }
});

test('unhydratable output fails before moving generated files', async () => {
  const root = await mkdtemp(resolve('.cache/static-entries-'));
  try {
    await prerender(root, 'en.html', 'not a hydrated page');
    await assert.rejects(finalizeOriginalEntrypoints(root), /Missing hydration/);
    assert.equal(await readFile(resolve(root, 'en.html/index.html'), 'utf8'), 'not a hydrated page');
  } finally {
    await rm(root, {recursive: true, force: true});
  }
});
