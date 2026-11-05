/** Real build output contracts only. These tests never start a browser/server. */
import {test, beforeAll as before, afterAll as after} from 'vitest';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {cp, mkdir, mkdtemp, readFile, readdir, rm, stat, symlink, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';
import {parse} from 'parse5';
import {parse as parseCss} from 'postcss';
import {parse as parseJavaScript} from 'acorn';
import {buildReactAppShell} from '../../scripts/ui-rewrite/app-shell-build.ts';
import {APP_SHELL_RETIREMENT_WORKER} from '../../lib/app-shell-retirement.mjs';
import {STATIC_FRONTEND_PACKAGE_FILES} from '../../lib/frontend-static-manifest.mjs';
import {readReactAppShellDeployment} from '../../app/services/app-shell-deployment.ts';
let work, coldSource;
const outputs = new Map();
const origin = 'http://127.0.0.1:43219';
const configuration = mountPath => ({schema: 'eagler-touhou/react-app-shell/1', origin, mountPath});
const exists = path => stat(path).then(() => true, error => {if (error.code === 'ENOENT') return false; throw error;});
before(async () => {
  await mkdir(resolve('.cache'), {recursive: true});
  work = await mkdtemp(resolve('.cache/react-pwa-contract-'));
  coldSource = resolve(work, 'source'); await mkdir(coldSource);
  // Exercise the supported build from authored inputs, never a warm ignored
  // launcher cache. Keep dependency installation shared; copy no build output.
  for (const path of ['app', 'src', 'lib', 'messages', 'project.inlang/settings.json', 'compat', 'package', 'public', 'config', 'scripts', 'host', 'server', 'integrations',
    'content', 'favicon.ico', 'docs/FAQ.md', 'docs/SELF_HOSTING.md', 'docs/SELF_HOSTING_REFERENCE.md', 'docs/EXTERNAL_RESOURCE_MODE.md',
    'product-catalog.mjs','site-manifest.mjs','release-catalog.mjs','resource-mode.mjs','runtime-protocol.mjs','NOTICE.txt', 'README.md', 'ASSETS.md', 'THIRD_PARTY.md', 'package.json', 'package-lock.json', 'bun.lock', '.bun-version', 'tsconfig.launcher.json', 'tsconfig.contracts.json',
    'tsconfig.ui-rewrite.json', 'tsconfig.ui-rewrite.tools.json', 'vite.config.ts', 'react-router.config.ts']) {
    await mkdir(resolve(coldSource, path, '..'), {recursive: true});
    await cp(resolve(path), resolve(coldSource, path), {recursive: true});
  }
  await cp(resolve('../common/contracts'), resolve(work, 'common/contracts'), {recursive: true});
  // Change only authored Markdown. Pre-generated HTML deliberately remains
  // stale so the real build must use main's content compiler, not copy it.
  for (const file of ['docs/FAQ.md', 'content/FIRST_USE_NOTICE.md', 'content/MULTIPLAYER.md']) {
    const path = resolve(coldSource, file);
    await writeFile(path, await readFile(path, 'utf8') + '\n\nReact build authored-content freshness probe\n');
  }
  const touchStyle=resolve(coldSource,'app/components/player/touch-help-ui.css');
  await writeFile(touchStyle,await readFile(touchStyle,'utf8')+'\n.game-help-content{--build-authored-touch-proof:1}\n');
  await symlink(resolve('node_modules'), resolve(coldSource, 'node_modules'), process.platform === 'win32' ? 'junction' : 'dir');
  assert.equal(await exists(resolve(coldSource, '.cache/build/browser')), false);
  assert.equal(await exists(resolve(coldSource, 'assets/contracts')), false);
  for (const [name, mountPath, enabled] of [['default', '/', true], ['disabled', '/', false], ['root', '/', true], ['nested', '/nested/', true]]) {
    const output = resolve(work, name);
    const env = {...process.env, EAGLER_REACT_BUILD_DIRECTORY: output, EAGLER_REACT_MOUNT_PATH: mountPath,
      EAGLER_REACT_APP_SHELL: name === 'default' ? '' : enabled ? 'isolated' : 'disabled', EAGLER_REACT_APP_SHELL_ORIGIN: name !== 'default' && enabled ? origin : '', EAGLER_REACT_APP_SHELL_ISOLATED_ORIGIN: ''};
    try {execFileSync(process.execPath, ['node_modules/@react-router/dev/bin.cjs', 'build'], {cwd: coldSource, env, encoding: 'utf8', stdio: 'pipe', timeout: 120000});}
    catch (error) {throw new Error(`React ${name} build failed:\n${error.stdout ?? ''}\n${error.stderr ?? ''}`, {cause: error});}
    outputs.set(name, {directory: resolve(output, 'client'), mountPath, enabled});
    assert.equal(await exists(resolve(coldSource, '.cache/build/browser')), false, `${name}: must not build the legacy Launcher`);
    assert.equal(await exists(resolve(coldSource, '.cache/build/optimized')), false, `${name}: must not optimize the legacy Launcher`);
    assert.equal(await exists(resolve(coldSource, 'assets/contracts')), false, `${name}: must not copy compiled contracts`);
  }
});
after(async () => {if (work) await rm(work, {recursive: true, force: true});});
function nodes(html) {
  const result = []; const visit = node => {if (node.tagName) result.push(node); for (const child of node.childNodes ?? []) visit(child);};
  visit(parse(html)); return result;
}
async function builtCssRules(directory) {
  const assets = resolve(directory, 'assets');
  const rules = [];
  for (const name of (await readdir(assets)).filter(name => name.endsWith('.css'))) {
    const root = parseCss(await readFile(resolve(assets, name), 'utf8'));
    root.walkRules(rule => {
      const declarations = new Map();
      rule.walkDecls(declaration => declarations.set(declaration.prop.toLowerCase(), declaration.value.toLowerCase()));
      rules.push({selector: rule.selector.replace(/\s+/g, ''), declarations});
    });
  }
  return rules;
}
test('actual builds preserve composed individual transforms in shared and route styles', async () => {
  for (const [name, {directory}] of outputs) {
    const rules = await builtCssRules(directory);
    const contains = (selector, translate, transform) => rules.some(rule => rule.selector.includes(selector)
      && rule.declarations.get('translate') === translate
      && rule.declarations.get('transform') === transform);
    assert.ok(contains('.book-lobby-checksvg', '-2px -6px', 'scale(.7)'), `${name}: lobby check keeps its optical offset and scale`);
    assert.ok(contains('.ui-button:active', 'none', 'none'), `${name}: shared button active state keeps both transform properties`);
    assert.ok(contains('.less-motion.book-recent-link', 'none', 'none'), `${name}: reduced-motion rule keeps both transform properties`);
  }
});
test('default actual build publishes an enabled same-origin offline worker', async () => {
  const {directory} = outputs.get('default');
  const worker = await readFile(resolve(directory, 'app-shell-sw.js'), 'utf8');
  assert.notEqual(worker, APP_SHELL_RETIREMENT_WORKER);
  assert.match(worker, /App Shell integrity mismatch/);
  for (const file of ['index.html', 'en.html', 'ja.html', 'lobby.html', 'profile.html']) {
    const html = await readFile(resolve(directory, file), 'utf8');
    assert.match(html, /name="eagler-react-app-shell"/);
    assert.match(html, /same-origin/);
  }
});
test('explicitly disabled actual build publishes the retirement worker', async () => {
  const {directory} = outputs.get('disabled');
  assert.equal(await readFile(resolve(directory, 'app-shell-sw.js'), 'utf8'), APP_SHELL_RETIREMENT_WORKER);
  for (const file of ['index.html', 'en.html', 'ja.html', 'lobby.html', 'profile.html']) assert.doesNotMatch(await readFile(resolve(directory, file), 'utf8'), /name="eagler-react-app-shell"/);
});
for (const name of ['root', 'nested']) test(`${name} output uses actual React chunks, exact mount, canonical worker and byte identities`, async () => {
  const {directory, mountPath} = outputs.get(name), appShell = configuration(mountPath);
  const result = await buildReactAppShell({clientDirectory: directory, mountPath, appShell});
  assert.ok(result); assert.equal(result.warnings.length, 0);
  const files = result.contract.entries;
  assert.ok(files.includes('./')); assert.ok(files.includes('about.html')); assert.ok(files.includes('faq.html')); assert.ok(files.includes('styles.css'));
  for (const path of ['guide.html','multiplayer.html','compatibility.html', 'compatibility.en.html', 'compatibility.ja.html']) assert.ok(files.includes(path), `${path}: help is available offline`);
  for(const path of ['about.css','site-help.mjs'])assert.equal(await exists(resolve(directory,path)),false,`${path}: retired document resource`);
  assert.equal(await exists(resolve(directory, 'migrate.html')), true); assert.equal(files.includes('migrate.html'), false);
  assert.equal(files.some(path => path.startsWith('legacy/')), false);
  assert.ok(files.some(path => /^assets\/manifest-.+\.js$/.test(path)));
  assert.ok(files.some(path => /^assets\/root-.+\.js$/.test(path)));
  assert.ok(files.some(path => /^assets\/root-.+\.css$/.test(path)));
  for (const path of files) assert.doesNotMatch(path, /^(?:app\.js$|assets\/(?:launcher|contracts)\/|runtime\/|runtime-manifest\.json$|docs\/|tests\/|src\/|app\/|\.cache\/)/);
  for (const entry of result.manifestEntries) {
    assert.match(entry.revision, /^[a-f0-9]{64}$/);
    const bytes = await readFile(resolve(directory, entry.url === './' ? 'index.html' : entry.url));
    assert.equal(createHash('sha256').update(bytes).digest('hex'), entry.revision, entry.url);
  }
  for (const path of ['index.html', 'en.html', 'ja.html', 'lobby.html', 'profile.html']) {
    const html = await readFile(resolve(directory, path), 'utf8'), elements = nodes(html);
    assert.equal((await stat(resolve(directory, path))).isFile(), true, 'original flat document URL');
    const htmlAttributes = elements.find(node => node.tagName === 'html').attrs;
    assert.equal(htmlAttributes.find(attr => attr.name === 'lang').value, path === 'en.html' ? 'en' : path === 'ja.html' ? 'ja' : 'zh-CN');
    assert.equal(htmlAttributes.some(attr => attr.name === 'data-ui-home'), ['index.html', 'en.html', 'ja.html'].includes(path), `${path}: cold entry shares home layout`);
    assert.match(html, path === 'en.html' ? /<title[^>]*>Touhou Project Original STGs ~ EAGLER TOUHOU<\/title>/ : path === 'ja.html' ? /<title[^>]*>東方Project 原作STG ~ EAGLER TOUHOU<\/title>/ : path === 'lobby.html' ? /<title>联机大厅 ~ EAGLER TOUHOU<\/title>/ : path === 'profile.html' ? /<title>玩家主页 ~ EAGLER TOUHOU<\/title>/ : /<title[^>]*>东方Project 原作 STG ~ EAGLER TOUHOU<\/title>/);
    assert.ok(html.includes(`"basename":${JSON.stringify(mountPath)}`));
    const metadata = elements.filter(node => node.tagName === 'meta' && node.attrs.some(attr => attr.name === 'name' && attr.value === 'eagler-react-app-shell'));
    const doc = {querySelectorAll: () => metadata.map(node => ({getAttribute: name => node.attrs.find(attr => attr.name === name)?.value}))};
    assert.deepEqual(readReactAppShellDeployment(doc, {href: origin + mountPath + path}), {
      workerUrl: origin + mountPath + 'app-shell-sw.js', scope: origin + mountPath,
      previousWorkerUrls: [origin + mountPath + 'app-shell-sw-fontfix-1fefcaa790f0.js'],
    });
    assert.equal(readReactAppShellDeployment(doc, {href: 'https://production.invalid' + mountPath + path}), undefined);
    for (const node of elements) for (const attr of node.attrs) if (['src', 'href'].includes(attr.name) && attr.value.startsWith('/') && attr.value.includes('/assets/')) {
      assert.ok(attr.value.startsWith(mountPath + 'assets/'), attr.value);
      const asset=new URL(attr.value,origin);
      assert.equal(asset.search,'');assert.equal(asset.hash,'');
      if(attr.value.endsWith('#'))assert.ok(node.tagName==='link'&&attr.value.endsWith('.css#'),attr.value);
      assert.ok(files.includes(asset.pathname.slice(mountPath.length)), attr.value);
    }
  }
  const worker = await readFile(resolve(directory, 'app-shell-sw.js'), 'utf8');
  assert.ok(worker.includes(`encodeURIComponent(scopeUrl.pathname)`));
  assert.ok(worker.includes('App Shell integrity mismatch'));
  assert.ok(worker.includes('createRuntimeCache('));
  assert.doesNotMatch(worker, /self\.clients\.claim\s*\(/);
});
test('mismatched graph mount, forbidden additions and missing graph inputs fail closed', async () => {
  const {directory, mountPath} = outputs.get('nested'), appShell = configuration(mountPath);
  await assert.rejects(buildReactAppShell({clientDirectory: directory, mountPath: '/', appShell}), /does not match this mount/);
  for (const file of ['migrate.html', 'legacy/legacy-game-pack.mjs', 'runtime/unused.js', 'docs/private.md', '.cache/private.md', '../private.md']) await assert.rejects(buildReactAppShell({clientDirectory: directory, mountPath, appShell, additionalShellFiles: [file]}), /Forbidden/);
  const graphPath = resolve(directory, '.vite/react-app-shell-graph.json'), original = await readFile(graphPath, 'utf8'), graph = JSON.parse(original);
  try {
    graph.shellFiles.push('assets/absent-real-chunk.js'); await writeFile(graphPath, JSON.stringify(graph));
    await assert.rejects(buildReactAppShell({clientDirectory: directory, mountPath, appShell}), /ENOENT/);
  } finally {await writeFile(graphPath, original);}
});

test('actual build keeps canonical public URLs including bounded legacy readers', async () => {
  for (const {directory} of outputs.values()) {
    for (const file of STATIC_FRONTEND_PACKAGE_FILES) assert.equal(await exists(resolve(directory, file)), true, file);
    for(const file of ['index.html','faq.html','about.html','guide.html','multiplayer.html']){
      const html=await readFile(resolve(directory,file),'utf8');
      const order=html.indexOf('id="ui-style-layers"'),style=html.indexOf('rel="stylesheet"');
      assert.ok(order>=0&&order<style,`${file}: layer order precedes split stylesheets`);
      assert.equal([...html.matchAll(/name="darkreader-lock"/g)].length,1,`${file}: explicit theme ownership`);
    }
    for (const file of ['README.md', 'ASSETS.md', 'THIRD_PARTY.md', 'NOTICE.txt']) {
      assert.deepEqual(await readFile(resolve(directory, file)), await readFile(resolve(file)));
    }
    for (const file of ['faq.html', 'content/FIRST_USE_NOTICE.html', 'content/MULTIPLAYER.html']) {
      assert.match(await readFile(resolve(directory, file), 'utf8'), /React build authored-content freshness probe/);
    }
    const styles = (await readdir(resolve(directory, 'assets'))).filter(file => file.endsWith('.css'));
    const touch = (await Promise.all(styles.map(file => readFile(resolve(directory,'assets',file),'utf8')))).join('\n');
    assert.match(touch,/--build-authored-touch-proof:\s*1/,'Shared help styles must come from current authored CSS');
    for (const file of STATIC_FRONTEND_PACKAGE_FILES.filter(file => file.startsWith('legacy/'))) {
      const source = await readFile(resolve(directory, file), 'utf8');
      assert.doesNotMatch(source, /node:|__vite-browser-external/);
      // Loading every published entry in Node catches unresolved browser ESM
      // imports; this is a module-closure check, never a browser-run claim.
      await import(pathToFileURL(resolve(directory, file)).href);
    }
  }
});

test('original legacy compatibility assertions pass unchanged against published modules', async () => {
  const registered = await readFile(resolve('tests/test-legacy-game-pack.mjs'), 'utf8');
  const program=parseJavaScript(registered,{ecmaVersion:'latest',sourceType:'module'});
  const registrations=program.body.filter(node=>node.type==='ExpressionStatement'&&node.expression.type==='CallExpression'&&node.expression.callee.name==='test');
  assert.equal(registrations.length,1,'one complete original assertion block');
  const statement=registrations[0],callback=statement.expression.arguments[1];
  const registrationImport=program.body.find(node=>node.type==='ImportDeclaration'&&node.source.value==='vitest');
  assert.ok(registrationImport);
  let original=registered;
  for(const[start,end,value]of [[statement.start,statement.end,registered.slice(callback.body.start+1,callback.body.end-1)],[registrationImport.start,registrationImport.end,'']].sort((a,b)=>b[0]-a[0]))original=original.slice(0,start)+value+original.slice(end);

  for (const [name, {directory}] of outputs) {
    let replacements = 0;
    const adapted = original.replace(/"\.\.\/compat\/([^"\n]+\.mjs)"/g, (_match, file) => {
      replacements++; return JSON.stringify(pathToFileURL(resolve(directory, "legacy", file)).href);
    });
    assert.equal(replacements, 3, 'only the three subject imports are adapted');
    const fixture = resolve(work, `original-legacy-${name}.mjs`);
    await writeFile(fixture, adapted);
    execFileSync(process.execPath, [fixture], {cwd: resolve('.'), encoding: 'utf8', stdio: 'pipe', timeout: 30000});
  }
});

test('selected self-host bundle consumes prebuilt React and private graph inputs without a compiler', async () => {
  const {directory} = outputs.get('default');
  const sourceEnvironment = {...process.env, EAGLER_FRONTEND: 'react', EAGLER_REACT_BUILD_DIRECTORY: resolve(directory, '..'), EAGLER_REACT_MOUNT_PATH: '/'};
  const rules = JSON.parse(execFileSync(process.execPath, ['scripts/list-self-host-bundle-files.mjs'], {cwd: coldSource, env: sourceEnvironment, encoding: 'utf8', timeout: 60000}));
  assert.deepEqual(rules.filter(rule => rule.target.startsWith('.vite/')).map(rule => rule.target).sort(), ['.vite/manifest.json', '.vite/react-app-shell-graph.json']);
  assert.equal(rules.some(rule => rule.target === 'app.js' || rule.target.startsWith('assets/launcher/')), false);
  assert.equal(rules.some(rule => /\.mts$/.test(rule.target)), false);
  assert.equal(await exists(resolve(coldSource, '.cache/build/browser')), false);
  assert.equal(await exists(resolve(coldSource, '.cache/build/optimized')), false);
  const operator = await mkdtemp(resolve(tmpdir(), 'react-self-host-prebuilt-'));
  try {
    for (const rule of rules) {
      await mkdir(resolve(operator, rule.target, '..'), {recursive: true});
      await cp(resolve(coldSource, rule.source), resolve(operator, rule.target));
    }
    // These are declared operator parsers and their locked transitive decoder,
    // not the maintainer's TS/React Router/esbuild dependency environment.
    for (const dependency of ['acorn', 'parse5', 'entities']) await cp(resolve('node_modules', dependency), resolve(operator, 'node_modules', dependency), {recursive: true});
    const environment = {...process.env, EAGLER_FRONTEND: 'react'};
    delete environment.EAGLER_REACT_BUILD_DIRECTORY; delete environment.EAGLER_REACT_MOUNT_PATH;
    const script = `import {FRONTEND_PACKAGE_FILES, REACT_FRONTEND_ARTIFACT} from './lib/frontend-manifest.mjs';
      import {ensureContractsBuild} from './lib/contracts-build.mjs';
      const contracts = await ensureContractsBuild();
      console.log(JSON.stringify({selection: "react", frontend: REACT_FRONTEND_ARTIFACT.kind, mode: contracts.mode,
        legacy: FRONTEND_PACKAGE_FILES.includes('app.js'), metadataPublic: FRONTEND_PACKAGE_FILES.some(path => path.startsWith('.vite/'))}));`;
    const result = JSON.parse(execFileSync(process.execPath, ['--input-type=module', '-e', script], {cwd: operator, env: environment, encoding: 'utf8', timeout: 30000}));
    assert.deepEqual(result, {selection: 'react', frontend: 'react', mode: 'prebuilt', legacy: false, metadataPublic: false});
    assert.equal(await exists(resolve(operator, 'node_modules/typescript')), false);
    assert.equal(await exists(resolve(operator, '.cache')), false);
  } finally {await rm(operator, {recursive: true, force: true});}
});

// Same actual CLI used by maintainers; no browser or worker registration.
test('standalone App Shell command respects selected React permission and graph', async () => {
  const output = resolve(work, 'standalone-worker.js');
  await writeFile(output, 'unchanged-output');
  const command = ['scripts/build-app-shell.mjs', `--output=${output}`];
  const environment = name => ({...process.env, EAGLER_FRONTEND: 'react',
    EAGLER_REACT_BUILD_DIRECTORY: resolve(outputs.get(name).directory, '..'),
    EAGLER_REACT_MOUNT_PATH: outputs.get(name).mountPath});
  assert.throws(() => execFileSync(process.execPath, command, {cwd: coldSource, env: environment('disabled'), stdio: 'pipe'}), /disabled/);
  assert.equal(await readFile(output, 'utf8'), 'unchanged-output');
  execFileSync(process.execPath, command, {cwd: coldSource, env: environment('nested'), stdio: 'pipe'});
  const worker = await readFile(output, 'utf8');
  assert.match(worker, /App Shell integrity mismatch/);
  assert.match(worker, /assets\/root-/);
  assert.doesNotMatch(worker, /assets\/launcher\/app\.mjs/);
  assert.equal(await exists(resolve(coldSource, '.cache/build/browser')), false);
});
