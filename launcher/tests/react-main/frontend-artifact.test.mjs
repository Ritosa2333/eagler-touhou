import {assertReactFrontendEnvironment} from '../../compat/frontend-environment.mjs';
/** Publication source/artifact contracts only. No server, browser or deployment. */
import {test} from 'vitest';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {cp, mkdir, mkdtemp, readFile, realpath, rm, symlink, writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {dirname, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {STATIC_FRONTEND_PACKAGE_FILES, STATIC_APP_SHELL_FILES} from '../../lib/frontend-static-manifest.mjs';
import {readReactFrontendArtifact, reactFrontendArtifactDirectory} from '../../lib/react-frontend-artifact.mjs';
import {SITE_BRAND_DEFAULT, SITE_BRAND_FACES} from '../../lib/site-brand.mjs';
import {QUICK_CHAT_PHRASES} from '../../../common/contracts/multiplayer-quick-chat.mjs';

test('every quick-chat phrase has a published and precached voice clip', async () => {
  const expected = QUICK_CHAT_PHRASES.map(({id}) => `assets/quick-chat/${id}.wav`).sort();
  const voiceClips = files => files.filter(path => path.startsWith('assets/quick-chat/')).sort();
  assert.deepEqual(voiceClips(STATIC_FRONTEND_PACKAGE_FILES), expected);
  assert.deepEqual(voiceClips(STATIC_APP_SHELL_FILES), expected);
  await Promise.all(expected.map(async path => {
    const clip = await readFile(new URL(`../../public/${path}`, import.meta.url));
    assert.equal(clip.toString('ascii', 0, 4), 'RIFF', path);
    assert.equal(clip.toString('ascii', 8, 12), 'WAVE', path);
  }));
});

async function fixture(run, {appShell = null, mountPath = '/review/'} = {}) {
  const work = await mkdtemp(resolve(tmpdir(), 'react-publication-'));
  const directory = resolve(work, 'client');
  async function put(path, text) {await mkdir(dirname(resolve(directory, path)), {recursive: true}); await writeFile(resolve(directory, path), text);}
  const manifest = {
    entry: {file: 'assets/entry-abcd.js', isEntry: true, dynamicImports: ['lazy'], css: ['assets/entry-abcd.css']},
    lazy: {file: 'assets/lazy-abcd.js', isDynamicEntry: true},
  };
  const generated = ['assets/entry-abcd.js', 'assets/entry-abcd.css', 'assets/lazy-abcd.js', 'assets/manifest-abcd.js'];
  const graph = {schema: 'eagler-touhou/react-app-shell-graph/1', mountPath, appShell,
    shellFiles: [...STATIC_APP_SHELL_FILES.filter(path => !['lobby.css', 'touch-guide.css'].includes(path)), ...generated].sort()};
  const metadata = {manifest, graph};
  try {
    for (const path of [...STATIC_FRONTEND_PACKAGE_FILES, ...generated]) await put(path, 'fixture');
    for (const path of STATIC_FRONTEND_PACKAGE_FILES.filter(path => path.startsWith('legacy/'))) await put(path, "export {value} from './chunks/shared.mjs';\n");
    await put('legacy/chunks/shared.mjs', 'export const value = 1;\n');
    const declaration = appShell ? `<meta name="eagler-react-app-shell" content="${JSON.stringify(appShell).replaceAll('"', '&quot;')}">` : '';
    const html = `<!doctype html><html><head>${declaration}<link rel="modulepreload" href="${mountPath}assets/manifest-abcd.js"><script type="module" src="${mountPath}assets/entry-abcd.js"></script><link rel="stylesheet" href="${mountPath}assets/entry-abcd.css"></head><body></body></html>`;
    for (const path of ['index.html', 'en.html', 'ja.html', 'lobby.html', 'profile.html']) await put(path, html);
    await put('.vite/manifest.json', JSON.stringify(manifest));
    await put('.vite/react-app-shell-graph.json', JSON.stringify(graph));
    await run({work, directory, metadata, put, html});
  } finally {await rm(work, {recursive: true, force: true});}
}

async function previousFixture(run, {mountPath = '/review/'} = {}) {
  await fixture(async ({work, directory, metadata, put, html}) => {
    const generated = ['assets/entry-abcd.js', 'assets/entry-abcd.css', 'assets/lazy-abcd.js', 'assets/manifest-abcd.js'];
    const shellFiles = [
      'index.html', 'en.html', 'lobby.html', 'about.html', 'faq.html', 'compatibility.html',
      'content/FIRST_USE_NOTICE.html', 'content/MULTIPLAYER.html', 'uptime/index.html',
      'site-pages.mjs', 'styles.css', ...generated,
    ].sort();
    const previousMetadata = structuredClone(metadata);
    previousMetadata.graph.shellFiles = shellFiles;
    await rm(resolve(directory, 'ja.html'), {force: true});
    await rm(resolve(directory, 'profile.html'), {force: true});
    await rm(resolve(directory, '.vite'), {recursive: true, force: true});
    await put('index.html', html.replace('</head>', `<link rel="preload" href="${mountPath}assets/donation.webp"></head>`));
    const staticPage = `<!doctype html><html><head><script type="module" src="${mountPath}site-pages.mjs"></script></head><body>Help</body></html>`;
    for (const path of ['about.html', 'faq.html']) await put(path, staticPage);
    await put('compatibility.html', '<!doctype html><html><head><title>Compatibility</title></head><body>Help</body></html>');
    await put('content/FIRST_USE_NOTICE.html', '<div class="first-use-notice-list"><p>Notice</p></div>');
    await put('content/MULTIPLAYER.html', '<p>Multiplayer rules</p>');
    await put('site-pages.mjs', 'export const page = true;');
    await put('styles.css', 'body { color: black; }');
    await put('uptime/index.html', '<!doctype html><html><head><base href="../"><script type="module" src="../site-pages.mjs"></script><link rel="stylesheet" href="../styles.css"></head><body>Status</body></html>');
    const publicationFiles = [...shellFiles, 'assets/donation.webp'];
    await run({work, directory, metadata: previousMetadata, publicationFiles, put, html, shellFiles});
  }, {mountPath});
}

test('launcher refresh blocks a candidate without the published PC-98 catalog', async () => {
  await fixture(async ({work, directory, metadata, put}) => {
    const site = resolve(work, 'site');
    await cp(directory, site, {recursive: true});
    const ids = ['th01', 'th02', 'th03', 'th04'];
    const digest = 'a'.repeat(64);
    const siteManifest = JSON.stringify({
      schema: 'eagler-touhou/site-manifest/1',
      protocol: 'eagler-touhou/1',
      profile: 'web-validation-catalog',
      shared: {resourceMode: 'hosted', vanillaFont: 'shared/msgothic.ttc', unicodeFont: 'shared/unifont.otf'},
      games: Object.fromEntries(ids.map(id => [id, {
        runtime: `runtime/${id}/${id}.html`,
        gameData: {path: `${id}.data`, bytes: 1, sha256: digest, version: 'sha256-' + digest, layout: 'sha256-' + digest},
        music: {midi: {files: []}},
      }])),
    });
    const deployment = JSON.stringify({
      format: 'eagler-touhou-deployment/1', appShell: null,
      frontend: {kind: 'react', mountPath: metadata.graph.mountPath, validationMetadata: metadata},
      files: metadata.graph.shellFiles.map(path => ({path})),
    });
    await writeFile(resolve(site, 'site-manifest.json'), siteManifest);
    await writeFile(resolve(site, 'deployment.json'), deployment);
    const script = fileURLToPath(new URL('../../scripts/refresh-deployment-app-shell.mjs', import.meta.url));
    await put('.vite/react-app-shell-graph.json', JSON.stringify({...metadata.graph, gameIds: ['th06']}));
    assert.throws(() => execFileSync(process.execPath, [script, site, '--frontend'], {
      env: {...process.env, EAGLER_REACT_BUILD_DIRECTORY: work}, stdio: 'pipe',
    }), /React frontend catalog is missing Site Manifest games: th01, th02, th03, th04/);
    assert.equal(await readFile(resolve(site, 'site-manifest.json'), 'utf8'), siteManifest);
    assert.equal(await readFile(resolve(site, 'deployment.json'), 'utf8'), deployment);
  });
});

test('frontend selection is explicit and never coerces invalid values to main', () => {
  assert.doesNotThrow(()=>assertReactFrontendEnvironment({}));
  assert.throws(()=>assertReactFrontendEnvironment({EAGLER_FRONTEND:'main'}),/EAGLER_FRONTEND/);
  assert.doesNotThrow(()=>assertReactFrontendEnvironment({EAGLER_FRONTEND:'react'}));
  for (const value of ['', 'React', 'legacy', 'react ', 'auto']) assert.throws(() => assertReactFrontendEnvironment({EAGLER_FRONTEND: value}), /EAGLER_FRONTEND/);
});

test('artifact directory follows the maintainer source versus packaged prebuilt boundary', async () => {
  await fixture(async ({work}) => {
    const project = resolve(work, 'operator'); await mkdir(resolve(project, 'src'), {recursive: true});
    await writeFile(resolve(project, 'src/app-shell-sw.js'), '// classic worker');
    assert.equal(reactFrontendArtifactDirectory({project, environment: {}}), project);
    await writeFile(resolve(project, 'src/contract.mts'), 'export const version = 1;');
    assert.equal(reactFrontendArtifactDirectory({project, environment: {}}), resolve(project, '.cache/build/ui-rewrite/client'));
    assert.equal(reactFrontendArtifactDirectory({project, environment: {EAGLER_REACT_BUILD_DIRECTORY: 'candidate'}}), resolve(project, 'candidate/client'));
  });
});

test('publication derives complete static, Vite and compatibility closures separately from precache', async () => {
  await fixture(async ({directory, put}) => {
    await put('private.txt', 'unowned artifact file'); await put('app.js', 'retired controller must never be selected');
    const artifact = await readReactFrontendArtifact({directory});
    assert.equal(artifact.kind, 'react'); assert.equal(artifact.appShell, null);
    for (const path of [...STATIC_FRONTEND_PACKAGE_FILES, 'legacy/chunks/shared.mjs', 'assets/lazy-abcd.js', 'assets/manifest-abcd.js']) assert.ok(artifact.packageFiles.includes(path), path);
    for (const path of ['migrate.html', 'legacy/chunks/shared.mjs', 'lobby.css', 'touch-guide.css']) assert.equal(artifact.shellFiles.includes(path), false, path);
    assert.equal(artifact.packageFiles.includes('app.js'), false);
    assert.equal(artifact.packageFiles.some(path => path.startsWith('.vite/')), false);
    assert.equal(artifact.resolveSource('legacy/chunks/shared.mjs'), resolve(directory, 'legacy/chunks/shared.mjs'));
    for (const path of ['private.txt', 'app.js', '.vite/manifest.json', '../client/index.html']) assert.throws(() => artifact.resolveSource(path), /unknown React/);
    assert.ok(Object.isFrozen(artifact.metadata.graph.shellFiles));
    // The deployment verifier can consume identical embedded values without
    // publishing the private build metadata directory.
    await rm(resolve(directory, '.vite'), {recursive: true});
    const packaged = await readReactFrontendArtifact({directory, metadata: artifact.metadata});
    assert.deepEqual(packaged.packageFiles, artifact.packageFiles);
    await assert.rejects(readReactFrontendArtifact({directory}), /run npm run build:ui-rewrite/);
  });
});

test('React documents may reference packaged static assets outside the Vite output graph', async () => {
  const face = SITE_BRAND_FACES.find(value => value !== SITE_BRAND_DEFAULT);
  const staticAsset = `assets/brand/${face}.svg`;
  assert.ok(STATIC_FRONTEND_PACKAGE_FILES.includes(staticAsset));
  await fixture(async ({directory, put, html}) => {
    await put('index.html', html.replace('</head>', `<link rel="icon" href="/review/${staticAsset}"></head>`));
    const artifact = await readReactFrontendArtifact({directory});
    assert.ok(artifact.packageFiles.includes(staticAsset));

    for (const reference of [
      `<link rel="icon" href="/review/assets/not-packaged.svg">`,
      `<script type="module" src="/review/assets/site-brand.mjs"></script>`,
    ]) {
      await put('index.html', html.replace('</head>', `${reference}</head>`));
      await assert.rejects(readReactFrontendArtifact({directory}), /absent from Vite graph/);
    }
  });
});

test('previous publication follows its embedded graph and accepts the real legacy document set', async () => {
  await previousFixture(async ({directory, metadata, publicationFiles, shellFiles}) => {
    const artifact = await readReactFrontendArtifact({
      directory,
      metadata,
      expectedMountPath: '/review/',
      publicationFiles,
    });
    assert.deepEqual(artifact.packageFiles, shellFiles);
    assert.ok(artifact.packageFiles.includes('content/FIRST_USE_NOTICE.html'));
    assert.ok(artifact.packageFiles.includes('uptime/index.html'));
    assert.equal(artifact.packageFiles.includes('assets/donation.webp'), false,
      'a catalogued static link outside the old shell graph is validated but is not retired as a shell-owned file');
    assert.equal(artifact.packageFiles.includes('profile.html'), false);
    assert.throws(() => artifact.resolveSource('profile.html'), /unknown React frontend package file/);

    await assert.rejects(readReactFrontendArtifact({
      directory,
      metadata,
      expectedMountPath: '/review/',
    }), /profile\.html|ENOENT/);
    await assert.rejects(readReactFrontendArtifact({
      directory,
      expectedMountPath: '/review/',
      publicationFiles,
    }), /metadata is missing or invalid|Invalid previous React publication inventory/);
  });
});

test('previous publication rejects files outside its catalog and incomplete Vite graphs', async () => {
  await previousFixture(async ({directory, metadata, publicationFiles}) => {
    const readPrevious = (copy = metadata, files = publicationFiles) => readReactFrontendArtifact({
      directory,
      metadata: copy,
      expectedMountPath: '/review/',
      publicationFiles: files,
    });
    const unlisted = structuredClone(metadata);
    unlisted.graph.shellFiles.push('assets/unlisted.css');
    await assert.rejects(readPrevious(unlisted), /outside its frontend publication/);

    const native = structuredClone(metadata);
    native.graph.shellFiles.push('runtime/th06/unowned.wasm');
    await assert.rejects(readPrevious(native, [...publicationFiles, 'runtime/th06/unowned.wasm']), /outside its frontend publication/);

    const escaped = structuredClone(metadata);
    escaped.graph.shellFiles.push('../outside.html');
    await assert.rejects(readPrevious(escaped, [...publicationFiles, '../outside.html']), /Invalid React publication path/);

    const brokenClosure = structuredClone(metadata);
    brokenClosure.manifest.entry.imports = ['missing-entry'];
    await assert.rejects(readPrevious(brokenClosure), /Missing Vite module dependency/);

    const unownedOutput = structuredClone(metadata);
    unownedOutput.manifest.unlisted = {file: 'assets/unlisted-output.js'};
    await assert.rejects(readPrevious(unownedOutput), /Previous Vite output is absent from its publication graph/);

    await rm(resolve(directory, 'assets/lazy-abcd.js'));
    await assert.rejects(readPrevious(), /ENOENT/);
  });
});

test('previous publication rejects graph paths that resolve outside the artifact directory', async () => {
  await previousFixture(async ({work, directory, metadata, publicationFiles}) => {
    const outside = resolve(work, 'outside-assets');
    await mkdir(outside, {recursive: true});
    await writeFile(resolve(outside, 'unowned.css'), 'outside');
    await symlink(outside, resolve(directory, 'assets/escape'), process.platform === 'win32' ? 'junction' : 'dir');
    const copy = structuredClone(metadata);
    copy.graph.shellFiles.push('assets/escape/unowned.css');
    await assert.rejects(readReactFrontendArtifact({
      directory,
      metadata: copy,
      expectedMountPath: '/review/',
      publicationFiles: [...publicationFiles, 'assets/escape/unowned.css'],
    }), /escapes artifact root/);
  });
});

test('missing output, malformed dependency graphs and foreign source paths fail closed', async () => {
  await fixture(async ({directory, metadata}) => {
    await assert.rejects(readReactFrontendArtifact({directory: resolve(directory, 'absent')}), /run npm run build:ui-rewrite/);
    await assert.rejects(readReactFrontendArtifact({directory, expectedMountPath: '/other/'}), /selected mount/);
    const bad = mutate => {const copy = structuredClone(metadata); mutate(copy); return readReactFrontendArtifact({directory, metadata: copy});};
    await assert.rejects(bad(value => value.manifest.entry.imports = ['absent']), /Missing Vite module dependency/);
    await assert.rejects(bad(value => value.manifest.entry.assets = '../outside'), /Invalid Vite manifest/);
    for (const path of ['../outside.js', '/assets/out.js', 'assets/../outside.js', 'assets/contracts/old.js', 'app.js']) await assert.rejects(bad(value => value.manifest.entry.file = path), /Invalid React publication|Forbidden React generated/);
    await assert.rejects(bad(value => value.graph.shellFiles.push('private.txt')), /publication ownership/);
    await assert.rejects(bad(value => value.graph.shellFiles.pop()), /publication ownership/);
    await assert.rejects(bad(value => delete value.graph.appShell), /Invalid React frontend artifact metadata/);
    await rm(resolve(directory, 'assets/lazy-abcd.js'));
    await assert.rejects(readReactFrontendArtifact({directory}), /ENOENT/);
  });
});

test('bounded compatibility closure rejects external modules and symlink escapes', async () => {
  await fixture(async ({work, directory, put}) => {
    await put('legacy/legacy-game-pack.mjs', "export * from 'node:fs';");
    await assert.rejects(readReactFrontendArtifact({directory}), /local relative imports/);
    await put('legacy/legacy-game-pack.mjs', "export * from '../private.mjs';"); await put('private.mjs', 'export const secret = 1;');
    await assert.rejects(readReactFrontendArtifact({directory}), /bounded legacy publication/);
    await put('legacy/legacy-game-pack.mjs', "export * from './chunks/shared.mjs';");
    await mkdir(resolve(work, 'outside-chunks'));
    await writeFile(resolve(work, 'outside-chunks/shared.mjs'), 'export const value = 1;');
    await rm(resolve(directory, 'legacy/chunks'), {recursive: true});
    await symlink(resolve(work, 'outside-chunks'), resolve(directory, 'legacy/chunks'), process.platform === 'win32' ? 'junction' : 'dir');
    await assert.rejects(readReactFrontendArtifact({directory}), /escapes artifact root/);
  });
});

test('generated JavaScript imports must agree with the official Vite publication graph', async () => {
  await fixture(async ({directory, put}) => {
    await put('assets/entry-abcd.js', "export * from './unlisted.js';");
    await put('assets/unlisted.js', 'export const value = 1;');
    await assert.rejects(readReactFrontendArtifact({directory}), /absent from the Vite publication graph/);
    await put('assets/entry-abcd.js', "import 'unpublished-package';");
    await assert.rejects(readReactFrontendArtifact({directory}), /local relative imports/);
  });
});

test('comments and script text cannot masquerade as active document entrypoints or configuration', async () => {
  await fixture(async ({directory, put, html}) => {
    await put('index.html', `<!doctype html><html><head><!-- ${html} --></head><body></body></html>`);
    await assert.rejects(readReactFrontendArtifact({directory}), /no browser entrypoint/);
    await put('index.html', `<!doctype html><html><head><script type="application/json">${html.replaceAll('</script>', '<\\/script>')}</script></head><body></body></html>`);
    await assert.rejects(readReactFrontendArtifact({directory}), /no browser entrypoint/);
  });
  const appShell = {schema: 'eagler-touhou/react-app-shell/1', origin: 'https://isolated.invalid', mountPath: '/review/'};
  await fixture(async ({directory, put, html}) => {
    await put('index.html', html.replace(/(<meta[^>]+>)/, '<!-- $1 -->'));
    await assert.rejects(readReactFrontendArtifact({directory}), /metadata mismatch/);
  }, {appShell});
});

test('artifact and document share the unchanged isolated origin/mount policy', async () => {
  const appShell = {schema: 'eagler-touhou/react-app-shell/1', origin: 'https://isolated.invalid', mountPath: '/review/'};
  await fixture(async ({directory, metadata, put, html}) => {
    assert.deepEqual((await readReactFrontendArtifact({directory})).appShell, appShell);
    const copy = structuredClone(metadata); copy.graph.appShell.mountPath = '/';
    await assert.rejects(readReactFrontendArtifact({directory, metadata: copy}), /isolated non-root/);
    copy.graph.appShell = {...appShell, origin: 'http://isolated.invalid'};
    await assert.rejects(readReactFrontendArtifact({directory, metadata: copy}), /secure deployment origin/);
    await put('en.html', html.replace('isolated.invalid', 'different.invalid'));
    await assert.rejects(readReactFrontendArtifact({directory}), /metadata mismatch/);
    await put('en.html', html.replace('/review/assets/entry-abcd.js', '/assets/entry-abcd.js'));
    await assert.rejects(readReactFrontendArtifact({directory}), /escapes its graph\/mount/);
  }, {appShell});
});

test('on-demand encoder is published without App Shell precaching', async () => {
  await fixture(async ({directory, metadata, put}) => {
    const wasm = 'assets/ogg-aBc12_fG.wasm';
    metadata.manifest.encoder = {file: wasm, src: 'ogg.wasm'};
    await put(wasm, 'wasm fixture');
    await put('.vite/manifest.json', JSON.stringify(metadata.manifest));
    const artifact = await readReactFrontendArtifact({directory});
    assert.ok(artifact.packageFiles.includes(wasm));
    assert.ok(!artifact.shellFiles.includes(wasm));
    const publication = [...artifact.shellFiles, wasm];
    const published = await readReactFrontendArtifact({directory, metadata, publicationFiles: publication});
    assert.ok(published.packageFiles.includes(wasm));
    await assert.rejects(readReactFrontendArtifact({directory, metadata, publicationFiles: artifact.shellFiles}),
      /Previous Vite output is absent from its publication graph/);
  });
});

test('packaged artifact reader uses operator parsers without compiler or React dependencies', async () => {
  await fixture(async ({work, directory}) => {
    const operator = resolve(work, 'operator');
    for (const file of ['react-frontend-artifact.mjs', 'browser-module-graph.mjs', 'frontend-static-manifest.mjs', 'multiplayer-guide.mjs', 'site-brand.mjs', 'react-app-shell-deployment.mjs']) {
      await mkdir(resolve(operator, 'lib'), {recursive: true}); await cp(resolve('lib', file), resolve(operator, 'lib', file));
    }
    await mkdir(resolve(operator, 'node_modules'), {recursive: true});
    // Worktree dependencies may be junctions into the shared install. Copy the
    // resolved package directory so the temporary operator bundle stays portable.
    for (const dependency of ['acorn', 'parse5', 'entities']) {
      const source = await realpath(resolve('node_modules', dependency));
      await cp(source, resolve(operator, 'node_modules', dependency), {recursive: true});
    }
    const script = `import {readReactFrontendArtifact} from ${JSON.stringify(pathToFileURL(resolve(operator, 'lib/react-frontend-artifact.mjs')).href)}; const artifact = await readReactFrontendArtifact({directory:${JSON.stringify(directory)}}); if (artifact.packageFiles.includes('app.js')) throw new Error('legacy fallback'); console.log(artifact.kind);`;
    assert.equal(execFileSync(process.execPath, ['--input-type=module', '-e', script], {cwd: operator, encoding: 'utf8'}).trim(), 'react');
  });
});

test('operator selector loads before any npm dependencies are installed', async () => {
  await fixture(async ({work}) => {
    const operator = resolve(work, 'before-install'); await mkdir(resolve(operator, 'lib'), {recursive: true});
    for (const file of ['react-frontend-artifact.mjs', 'frontend-static-manifest.mjs', 'multiplayer-guide.mjs', 'site-brand.mjs', 'react-app-shell-deployment.mjs']) await cp(resolve('lib', file), resolve(operator, 'lib', file));
    const script = `import {reactFrontendArtifactDirectory} from './lib/react-frontend-artifact.mjs'; console.log(JSON.stringify({selection:"react", root:reactFrontendArtifactDirectory({project:process.cwd(),environment:{}})}));`;
    const result = JSON.parse(execFileSync(process.execPath, ['--input-type=module', '-e', script], {cwd: operator, encoding: 'utf8'}));
    assert.deepEqual(result, {selection: 'react', root: operator});
  });
});


test('shared static/dynamic CSS accepts the Router empty fragment while other suffixes stay rejected', async () => {
  await fixture(async ({directory,put,html}) => {
    const css='/review/assets/entry-abcd.css';
    await put('en.html',html.replace(css,css+'#'));
    const result=await readReactFrontendArtifact({directory});
    assert.ok(result.shellFiles.includes('assets/entry-abcd.css'));
    assert.equal(result.shellFiles.some(path=>path.includes('#')),false);
    assert.ok((await readFile(resolve(directory,'en.html'),'utf8')).includes(css+'#'));
    for(const suffix of ['#other','##','?preview=1','?preview=1#']) {
      await put('en.html',html.replace(css,css+suffix));
      await assert.rejects(readReactFrontendArtifact({directory}),/absent from Vite graph/);
    }
    await put('en.html',html.replace('/review/assets/entry-abcd.js','/review/assets/entry-abcd.js#'));
    await assert.rejects(readReactFrontendArtifact({directory}),/absent from Vite graph/);
  });
});
