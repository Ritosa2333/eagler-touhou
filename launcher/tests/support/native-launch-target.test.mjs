import {readUiHistoryText} from './ui-history.mjs';
/** Original native-launch setup only. No browser, game, Relay, or imported
 * Package is executed. These original harnesses already inherit the production
 * frontend artifact; replacing their dynamic server with a static fixture
 * would remove part of their original setup contract. */
import test from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync, spawn} from 'node:child_process';
import {writeFile, mkdir, mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createServer} from 'node:net';
import {readReactFrontendArtifact} from '../../lib/react-frontend-artifact.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
const baseline = 'edee9633e5e3ee79cd2e1aa334f84f6caf755090';
const paths = ['tests/test-th09mp-launch.py', 'tests/test-th20-import-launch-browser.py', 'tests/test-th20-touch-browser.py'];
const python = source => JSON.parse(execFileSync('python', ['-'], {input: source, cwd: project, encoding: 'utf8', env: {...process.env, PYTHONDONTWRITEBYTECODE: '1'}}));

function setup(environment) {
  // Evaluate only the original environment copy/update/pop statements, then
  // inspect the original HTTP command and kwargs. Never import/execute a test,
  // call Popen, open a Package, probe a game, or launch Playwright.
  return python(`import ast,json\nfrom pathlib import Path\nfrom types import SimpleNamespace\nresults=[]\nfor path in ${JSON.stringify(paths)}:\n tree=ast.parse(Path(path).read_text()); main=next(n for n in tree.body if isinstance(n,ast.FunctionDef) and n.name=='main')\n scope={'os':SimpleNamespace(environ=json.loads(${JSON.stringify(JSON.stringify(environment))})), 'PROJECT':Path.cwd(),'WORKSPACE':Path.cwd().parent,'port':23451,'http_port':23451,'relay_url':'ws://127.0.0.1:23452/','content':Path.cwd().parent/'games'/('th09' if 'th09' in path else 'th20-content'),'vanilla':Path('/explicit/fonts/msgothic.ttc'),'unicode_font':Path('/explicit/fonts/unifont.otf')}\n for n in main.body:\n  if isinstance(n,ast.Assign) and isinstance(n.value,ast.Call) and isinstance(n.value.func,ast.Attribute) and n.value.func.attr=='Popen':\n   call=n.value; command=eval(compile(ast.Expression(call.args[0]),path,'eval'),scope)\n   if command[1]=='scripts/serve.mjs':\n    kw={k.arg:eval(compile(ast.Expression(k.value),path,'eval'),scope) for k in call.keywords if k.arg in ('env','cwd')}\n    results.append({'path':path,'command':command,'cwd':str(kw['cwd']),'environment':kw['env']});break\n  names=[t.id for t in n.targets if isinstance(t,ast.Name)] if isinstance(n,ast.Assign) else []\n  selected=bool(set(names)&{'env','http_env'})\n  if isinstance(n,ast.Expr) and isinstance(n.value,ast.Call) and isinstance(n.value.func,ast.Attribute):\n   owner=n.value.func.value; selected=isinstance(owner,ast.Name) and owner.id in ('env','http_env') and n.value.func.attr in ('update','pop')\n  if selected: exec(compile(ast.Module(body=[n],type_ignores=[]),path,'exec'),scope)\nprint(json.dumps(results))`);
}

test('native harness setup changes retain the independent original scenario assertions', () => {
  const originals = Object.fromEntries(paths.map(path => [path, readUiHistoryText(baseline, path)]));
  const rows = python(`import ast,json\nfrom pathlib import Path\nrows=[]\nfor p in ${JSON.stringify(paths)}:\n old=json.loads(${JSON.stringify(JSON.stringify(originals))})[p];new=Path(p).read_text()\n def assertions(s):return [ast.get_source_segment(s,n) for n in ast.walk(ast.parse(s)) if isinstance(n,ast.Assert)]\n rows.append({'path':p,'count':len(assertions(old)),'equal':assertions(old)==assertions(new)})\nprint(json.dumps(rows))`);
  for (const row of rows) {assert.ok(row.count > 0, row.path); assert.equal(row.equal, true, row.path);}
});

test('HTTP setup preserves explicit artifact, game/font/data overrides, import-only data removal and TH09 dynamic Relay', () => {
  const defaults = setup({}), marker = '/explicit/prepared/react-build';
  const selected = setup({EAGLER_REACT_BUILD_DIRECTORY: marker, EAGLER_TH20_CONTENT_DIR: '/must-not-leak-into-import'});
  assert.equal(defaults.length, 3); assert.equal(selected.length, 3);
  for (let index = 0; index < paths.length; index++) {
    const original = defaults[index], actual = selected[index];
    assert.deepEqual(actual.command, ['node', 'scripts/serve.mjs', '23451']);
    assert.equal(actual.cwd, resolve(project));
    assert.equal(actual.environment.EAGLER_REACT_BUILD_DIRECTORY, marker);
    for (const [key, value] of Object.entries(original.environment)) assert.equal(actual.environment[key], value, `${paths[index]} ${key}`);
  }
  assert.equal(selected[0].environment.EAGLER_TOUHOU_NETPLAY_RELAY, 'ws://127.0.0.1:23452/');
  assert.equal(selected[0].environment.EAGLER_DEVELOPMENT_GAMES, 'th09');
  assert.equal(selected[1].environment.EAGLER_DEVELOPMENT_GAMES, 'th20');
  assert.equal(Object.hasOwn(selected[1].environment, 'EAGLER_TH20_CONTENT_DIR'), false);
  assert.equal(selected[2].environment.EAGLER_TH20_CONTENT_DIR, resolve(project, '../games/th20-content'));
  for (const item of selected.slice(1)) assert.equal(item.environment.EAGLER_DEVELOPMENT_VANILLA_FONT, resolve(project, '../prepared/eagler-touhou-hosted-five-games-20260924/shared/msgothic.ttc'));
});

async function freePort() {
  const server = createServer(); await new Promise((yes, no) => {server.once('error', no); server.listen(0, '127.0.0.1', yes);});
  const port = server.address().port; await new Promise(yes => server.close(yes)); return port;
}
async function start(environment, port) {
  const child = spawn(process.execPath, ['scripts/serve.mjs', String(port)], {cwd: project, env: environment, stdio: ['ignore', 'pipe', 'pipe']});
  let output = ''; child.stdout.on('data', value => {output += value;}); child.stderr.on('data', value => {output += value;});
  const exited = new Promise(yes => child.once('exit', yes));
  const stop = async () => {if (child.exitCode === null && child.signalCode === null) {child.kill(); await exited;}};
  try {
    await new Promise((yes, no) => {
      const timer = setTimeout(() => finish(new Error('Server setup timed out: ' + output)), 30000);
      const ready = () => {if (output.includes(`eagler-touhou: http://127.0.0.1:${port}/`)) finish();};
      const failed = () => finish(new Error('Server setup failed: ' + output));
      const finish = error => {clearTimeout(timer); child.stdout.off('data', ready); child.off('exit', failed); child.off('error', finish); error ? no(error) : yes();};
      child.stdout.on('data', ready); child.once('exit', failed); child.once('error', finish);
    });
    return {base: `http://127.0.0.1:${port}`, stop};
  } catch (error) {await stop(); throw error;}
}

test('actual React development server preserves th20 identity, readable shared font routes and per-run Relay metadata; missing selected build fails closed', {timeout: 180000}, async () => {
  await mkdir(resolve(project, '.cache'), {recursive: true});
  const work = await mkdtemp(resolve(project, '.cache/native-launch-setup-'));
  try {
    const output = resolve(work, 'build');
    const environment = {...process.env, ...setup({EAGLER_REACT_BUILD_DIRECTORY: output})[1].environment,
      EAGLER_REACT_MOUNT_PATH: '/', EAGLER_REACT_APP_SHELL: 'disabled', EAGLER_REACT_APP_SHELL_ORIGIN: '', EAGLER_TOUHOU_HOST: '127.0.0.1', EAGLER_ENABLE_THCRAP: '0'};
    delete environment.EAGLER_TH20_CONTENT_DIR;
    delete environment.EAGLER_TOUHOU_NETPLAY_RELAY;
    delete environment.EAGLER_FRONTEND;
    // Metadata-only inputs: no private installed fonts or retail DATA required.
    for (const [key, name] of [['EAGLER_DEVELOPMENT_VANILLA_FONT', 'msgothic.ttc'], ['EAGLER_DEVELOPMENT_UNICODE_FONT', 'unifont.otf']]) {
      environment[key] = resolve(work, name); await writeFile(environment[key], new Uint8Array([1, 2, 3]));
    }
    execFileSync(process.execPath, ['node_modules/@react-router/dev/bin.cjs', 'build'], {cwd: project, env: environment, stdio: 'pipe', timeout: 120000});
    const artifact = await readReactFrontendArtifact({directory: resolve(output, 'client'), expectedMountPath: '/'});
    assert.equal(artifact.appShell, null, 'ephemeral original ports require a no-App-Shell build');
    const port = await freePort();
    // This is a metadata-only endpoint marker, never a running or mock Relay.
    environment.EAGLER_TOUHOU_NETPLAY_RELAY = 'ws://127.0.0.1:23452/';
    const server = await start(environment, port);
    try {
      const html = await (await fetch(server.base + '/')).text();
      assert.match(html, /__reactRouterContext/); assert.doesNotMatch(html, /<script[^>]+src=["'](?:\.\/)?app\.js/);
      const actual = await (await fetch(server.base + '/site-manifest.json')).json();
      const canonical = JSON.parse(execFileSync(process.execPath, ['--input-type=module', '-e', "import {createDevelopmentSiteManifest} from './lib/development-site-manifest.mjs';console.log(JSON.stringify(await createDevelopmentSiteManifest({games:['th20'],netplayRelay:process.env.EAGLER_TOUHOU_NETPLAY_RELAY})))"], {cwd: project, env: environment, encoding: 'utf8'}));
      for (const [key,name] of [['vanillaFont','msgothic.ttc'],['unicodeFont','unifont.otf']]) {
        assert.match(actual.shared[key],new RegExp('^/__development-content/[a-f0-9]{64}/'+name.replaceAll('.','\\.')+'$'));
        const resource=await fetch(new URL(actual.shared[key],server.base));
        assert.equal(resource.status,200,name);
        assert.deepEqual(new Uint8Array(await resource.arrayBuffer()),new Uint8Array([1,2,3]),name);
        canonical.shared[key]=actual.shared[key];
      }
      assert.deepEqual(actual, canonical);
      assert.equal(actual.shared.netplayRelay, environment.EAGLER_TOUHOU_NETPLAY_RELAY);
      assert.deepEqual(Object.keys(actual.games), ['th20']);
      assert.equal(Object.hasOwn(actual.games.th20.gameData, 'source'), false, 'import-only gate must not gain hosted DATA');
      assert.equal(actual.games.th20.gameData.bytes, 150943726);
      assert.deepEqual(actual.games.th20.music, {midi: {files: []}});
      assert.deepEqual(await (await fetch(server.base + '/release-catalog.json')).json(), {schema: 'eagler-touhou/release-catalog/1', games: {}});
      const retirement = await fetch(server.base + '/app-shell-sw.js');
      assert.equal(retirement.status, 200); assert.match(await retirement.text(), /registration\.unregister/);
      for (const path of ['/app.js', '/assets/launcher/app.mjs', '/public/index.html', '/missing-native-runtime.wasm']) {
        const response = await fetch(server.base + path); assert.equal(response.status, 404, path); assert.doesNotMatch(await response.text(), /__reactRouterContext/, path);
      }
    } finally {await server.stop();}
    assert.throws(() => execFileSync(process.execPath, ['scripts/serve.mjs', String(port)], {cwd: project,
      env: {...environment, EAGLER_REACT_BUILD_DIRECTORY: resolve(work, 'missing')}, stdio: 'pipe', timeout: 30000}), error => {
        assert.match(String(error.stderr), /React frontend artifact is missing/); return true;
      });
  } finally {await rm(work, {recursive: true, force: true});}
});
