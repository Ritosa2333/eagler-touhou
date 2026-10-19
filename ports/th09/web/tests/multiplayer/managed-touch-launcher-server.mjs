/**
 * Real React + directory Runtime integration, hosted web-development only.
 * No build/download, fake DATA owner, Blob Runtime, or replacement iframe.
 * start() only reads explicitly owned inputs; all copies/evidence are create-only under
 * this worktree's artifacts/multiplayer-tests. Importing this module does nothing.
 */
import {createHash, randomBytes} from 'node:crypto';
import {createReadStream} from 'node:fs';
import {mkdir, open, readFile, realpath, stat, writeFile} from 'node:fs/promises';
import {basename, dirname, isAbsolute, relative, resolve, sep} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {performance} from 'node:perf_hooks';

const selfPath = fileURLToPath(import.meta.url);
const ownedArtifactBase = fileURLToPath(new URL('../../artifacts/multiplayer-tests/', import.meta.url));
const starts = new Map();
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
const stamp = () => ({utcMs: Date.now(), monotonicMs: performance.now()});
const errorInfo = error => ({name: error?.name || 'Error', message: String(error?.message || error),
  ...(error?.code ? {code: error.code} : {}), stack: String(error?.stack || '').slice(0, 12000)});
const within = (root, path) => {const p = relative(root, path); return p === '' || p !== '..' && !p.startsWith('..' + sep) && !isAbsolute(p);};
const same = (a, b) => relative(a, b) === '';
const safeName = name => typeof name === 'string' && /^[A-Za-z0-9][A-Za-z0-9._/-]*$/.test(name) &&
  name.split('/').every(part => part !== '.' && part !== '..' && !part.startsWith('.'));
const ordinaryHelpers = [
  'lib/development-host-manifest.mjs', 'lib/development-content.mjs', 'lib/content-definition.mjs',
  'lib/workspace-layout.mjs', 'lib/runtime-data-layout.mjs', 'lib/runtime-data-provider.mjs',
  'lib/release-manifest.mjs', 'lib/th11-content-layout.mjs', 'lib/th15-content-layout.mjs',
  'lib/th20-content-layout.mjs', 'config/workspace.json', 'lib/ui-artifact.mjs',
  'server/ui-static-server.mjs', 'server/static-content-policy.mjs',
  'scripts/ui-deployment-contract.mjs', 'src/launcher/room-invite.mts',
];
const compiledContracts = ['host-manifest', 'product-catalog', 'resource-mode', 'runtime-generations'];
const runtimeRequired = ['th09.html', 'shell.mjs', 'managed.css', 'keyboard.mjs',
  'directory-keyboard.mjs', 'shared-netplay.mjs', 'spectator-timeline.mjs', 'spectator-timeline-view.mjs',
  'spectator-tape.mjs', 'motion-replay.mjs', 'auto-replay.mjs', 'replay-player.mjs', 'startup-branding.mjs',
  'th09.mjs', 'th09.wasm', 'fonts/cp932.bin', 'fonts/blend.bin', 'manifest.json', 'version.json'];

function normalize(input) {
  const out = {...input};
  if (out.uiSourceRoot === undefined) out.uiSourceRoot = out.canonicalRoot;
  for (const key of ['canonicalRoot', 'uiSourceRoot', 'uiArtifactRoot', 'normalRuntimeRoot', 'multiplayerRuntimeRoot', 'dataPath',
    'vanillaFontPath', 'unicodeFontPath', 'artifactDir']) {
    if (typeof out[key] !== 'string' || !isAbsolute(out[key])) throw Error(key + ' must be an explicit absolute path');
    out[key] = resolve(out[key]);
  }
  if (!/^[a-f0-9]{64}$/i.test(out.expectedWasmSha256 || '')) throw Error('expectedWasmSha256 must be an explicit SHA-256');
  out.expectedWasmSha256 = out.expectedWasmSha256.toLowerCase();
  if (out.expectedUiArtifactId != null && !/^[a-f0-9]{64}$/i.test(out.expectedUiArtifactId)) throw Error('Invalid expectedUiArtifactId');
  if (out.expectedUiArtifactId) out.expectedUiArtifactId = out.expectedUiArtifactId.toLowerCase();
  out.host ??= '127.0.0.1'; out.port ??= 0; out.probeTimeoutMs ??= 10000;
  if (!['127.0.0.1', '::1'].includes(out.host)) throw Error('This integration server binds loopback only');
  if (!Number.isInteger(out.port) || out.port < 0 || out.port > 65535) throw Error('Invalid port');
  if (!Number.isInteger(out.probeTimeoutMs) || out.probeTimeoutMs < 100 || out.probeTimeoutMs > 30000) throw Error('probeTimeoutMs must be 100..30000');
  if (out.netplayRelay != null) {
    const relay = new URL(out.netplayRelay);
    if (!['ws:', 'wss:'].includes(relay.protocol) || relay.username || relay.password || relay.hash) throw Error('Invalid netplayRelay URL');
    out.netplayRelay = relay.href;
  }
  return out;
}

async function fileIdentity(path) {
  const before = await stat(path);
  if (!before.isFile() || before.size <= 0) throw Error('Missing, empty, or non-file input: ' + path);
  const hash = createHash('sha256');
  for await (const bytes of createReadStream(path)) hash.update(bytes);
  const after = await stat(path);
  if (before.size !== after.size || before.mtimeMs !== after.mtimeMs) throw Error('Input changed while hashing: ' + path);
  return {bytes: before.size, sha256: hash.digest('hex'), mtimeMs: before.mtimeMs};
}
async function containedFile(root, name) {
  if (!safeName(name)) throw Error('Unsafe file name: ' + name);
  const path = await realpath(resolve(root, name));
  if (!within(root, path)) throw Error('Input escapes its explicit owner: ' + name);
  return path;
}
async function writeJson(path, value) {
  const body = JSON.stringify(value, null, 2) + '\n';
  await writeFile(path, body, {flag: 'wx'});
  return {path, bytes: Buffer.byteLength(body), sha256: digest(body)};
}
async function bodyJson(request) {
  if (!/^application\/json(?:;|$)/i.test(request.headers['content-type'] || '')) throw Error('Control POST requires application/json');
  const chunks = []; let bytes = 0;
  for await (const chunk of request) {
    bytes += chunk.length;
    if (bytes > 2048) throw Error('Control body exceeds 2048 bytes');
    chunks.push(chunk);
  }
  const value = JSON.parse(Buffer.concat(chunks).toString('utf8'));
  if (!value || typeof value !== 'object' || Array.isArray(value)) throw Error('Control body must be an object');
  return value;
}
function jsonResponse(response, status, body) {
  const bytes = Buffer.from(JSON.stringify(body) + '\n');
  response.writeHead(status, {'Content-Type': 'application/json; charset=utf-8',
    'Cache-Control': 'no-store', 'Content-Length': bytes.length});
  response.end(bytes);
}

/** Repeated starts with the same artifactDir/options return the same instance.
 * A failed attempt or a closed instance is retained; a retry needs a NEW directory.
 */
export function startManagedTouchLauncherServer(input) {
  const options = normalize(input), key = JSON.stringify(Object.keys(options).sort().map(name => [name, options[name]]));
  const existing = starts.get(options.artifactDir);
  if (existing) {
    if (existing.key !== key) throw Error('artifactDir already belongs to different server options');
    return existing.promise;
  }
  const promise = startOwned(options);
  starts.set(options.artifactDir, {key, promise});
  return promise;
}

async function startOwned(options) {
  let created = false, journal = null, server = null, stopping = false, stopPromise = null;
  let logTail = Promise.resolve(), journalError = null, origin = '', mountPath = '/';
  const events = [], errors = [], tracked = new Map(), routes = new Map(), gates = new Map(), sockets = new Set();
  let nextRequest = 0, nextGate = 0, activeGate = null, finishClosed;
  const closed = new Promise(resolveClosed => {finishClosed = resolveClosed;});
  const token = randomBytes(24).toString('hex');
  const controlPath = '/__managed_touch_control/' + token;
  const probePrefix = '/__managed_touch_probe/' + randomBytes(24).toString('hex') + '/';
  const record = (kind, fields = {}) => {
    const item = {seq: events.length + 1, ...stamp(), kind, ...fields};
    events.push(item);
    if (journal) logTail = logTail.then(() => journal.write(JSON.stringify(item) + '\n')).catch(error => {
      if (!journalError) {journalError = errorInfo(error); errors.push({kind: 'journal-write', error: journalError});}
    });
    return item;
  };
  const fault = (kind, error, extra = {}) => {
    const value = {kind, error: errorInfo(error), ...extra}; errors.push(value); record('error', value); return value;
  };
  const remember = (path, identity, role) => {
    const previous = tracked.get(path);
    if (previous && (previous.sha256 !== identity.sha256 || previous.bytes !== identity.bytes)) throw Error('Conflicting input identity: ' + path);
    const value = {...identity, path, role: previous ? previous.role + ',' + role : role};
    tracked.set(path, value); return value;
  };
  const identify = async (path, role) => remember(path, await fileIdentity(path), role);
  const auditInputs = async () => {
    const mismatches = [];
    for (const previous of tracked.values()) {
      try {
        const current = await fileIdentity(previous.path);
        if (current.sha256 !== previous.sha256 || current.bytes !== previous.bytes) mismatches.push({path: previous.path, before: previous, after: current});
      } catch (error) {mismatches.push({path: previous.path, before: previous, error: errorInfo(error)});}
    }
    return {checked: tracked.size, stable: mismatches.length === 0, mismatches};
  };
  const stateFor = gate => gate ? {gateId: gate.id, label: gate.label, path: gate.path,
    armed: gate.armed, deadlineUtcMs: gate.deadlineUtcMs, released: gate.released,
    releaseReason: gate.releaseReason, requests: gate.requests, pending: gate.pending} : null;
  function releaseGate(gate, reason) {
    if (gate.released) return stateFor(gate);
    clearTimeout(gate.timer); gate.released = stamp(); gate.releaseReason = reason;
    record('probe-release', {gateId: gate.id, reason, pending: gate.pending});
    if (reason === 'timeout') fault('probe-timeout', Error('Controlled font wait exceeded its deadline'), {gateId: gate.id});
    if (reason === 'server-stop') fault('probe-aborted', Error('Server stopped with a held font gate'), {gateId: gate.id});
    gate.resolve(); return stateFor(gate);
  }
  function armGate(body) {
    if (stopping) throw Error('Server is stopping');
    if (activeGate && (!activeGate.released || activeGate.pending)) throw Error('Previous font gate is still active');
    if (gates.size >= 32) throw Error('At most 32 explicit probes per server');
    if (typeof body.label !== 'string' || !/^[A-Za-z0-9][A-Za-z0-9._-]{0,95}$/.test(body.label)) throw Error('Probe label must be 1..96 safe characters');
    const timeoutMs = body.maxWaitMs ?? options.probeTimeoutMs;
    if (!Number.isInteger(timeoutMs) || timeoutMs < 100 || timeoutMs > options.probeTimeoutMs) throw Error('Invalid probe maxWaitMs');
    const id = String(++nextGate), armed = stamp();
    const gate = {id, label: body.label, path: probePrefix + id + '/blend.bin', armed,
      deadlineUtcMs: armed.utcMs + timeoutMs, released: null, releaseReason: null, requests: [], pending: 0};
    gate.wait = new Promise(resolveGate => {gate.resolve = resolveGate;});
    gate.timer = setTimeout(() => releaseGate(gate, 'timeout'), timeoutMs);
    gates.set(gate.path, gate); activeGate = gate;
    record('probe-armed', {gateId: id, label: gate.label, timeoutMs, path: gate.path});
    return {probe: stateFor(gate), resource: {path: '/fonts/blend.bin', url: origin + mountPath.slice(0, -1) + gate.path}};
  }

  async function closeOwnedServer() {
    if (!server?.listening) return;
    await new Promise((resolveClose, rejectClose) => {
      const timer = setTimeout(() => {
        const count = sockets.size;
        record('cleanup-close-connections', {count});
        for (const socket of sockets) socket.destroy();
      }, 3000);
      server.close(error => {clearTimeout(timer); if (error) rejectClose(error); else resolveClose();});
      server.closeIdleConnections?.();
    });
  }
  async function stop(reason = 'api') {
    if (stopPromise) return stopPromise;
    stopping = true;
    stopPromise = (async () => {
      record('server-stop-requested', {reason});
      for (const gate of gates.values()) if (!gate.released) releaseGate(gate, 'server-stop');
      await closeOwnedServer();
      const inputsAfter = await auditInputs();
      if (!inputsAfter.stable) fault('source-drift', Error('Frozen inputs changed during server lifetime'));
      record('server-stopped', {inputsStable: inputsAfter.stable});
      await logTail;
      const result = {schema: 'th09/managed-touch-launcher-stop/1', ...stamp(), reason,
        clean: inputsAfter.stable && errors.length === 0 && !journalError,
        scope: 'server/source integrity only; browser and input assertions belong to the driver',
        inputsAfter, probes: [...gates.values()].map(stateFor), events: events.length, errors,
        journalError, artifactDir: options.artifactDir};
      if (journal) {await journal.close(); journal = null;}
      result.evidence = await writeJson(resolve(options.artifactDir, 'server-stopped.json'), result);
      finishClosed(result); return result;
    })().catch(async error => {
      const failure = {clean: false, artifactDir: options.artifactDir, error: errorInfo(error), errors, journalError};
      try {failure.evidence = await writeJson(resolve(options.artifactDir, 'server-stop-failure.json'), failure);}
      catch (saveError) {failure.evidenceError = errorInfo(saveError);}
      finishClosed(failure); throw error;
    });
    return stopPromise;
  }
  try {
    // realpath prevents an output parent symlink from redirecting writes outside
    // this worktree. Never create a missing parent or reuse a failed directory.
    const [base, outputParent] = await Promise.all([realpath(ownedArtifactBase), realpath(dirname(options.artifactDir))]);
    if (!within(base, outputParent) || !/^[A-Za-z0-9][A-Za-z0-9._-]*$/.test(basename(options.artifactDir))) throw Error('artifactDir must be a new child of this worktree artifact area');
    await mkdir(options.artifactDir); created = true;
    options.artifactDir = await realpath(options.artifactDir);
    journal = await open(resolve(options.artifactDir, 'server-events.jsonl'), 'wx');
    record('initializing', {scope: 'hosted web-development integration, not immutable publication'});
    for (const key of ['canonicalRoot', 'uiSourceRoot', 'uiArtifactRoot', 'normalRuntimeRoot', 'multiplayerRuntimeRoot']) {
      options[key] = await realpath(options[key]);
      if (!(await stat(options[key])).isDirectory()) throw Error(key + ' is not a directory');
    }
    if (!within(options.uiSourceRoot, options.uiArtifactRoot)) throw Error('UI artifact must belong to the explicit UI source root');
    if (same(options.normalRuntimeRoot, options.multiplayerRuntimeRoot)) throw Error('Normal and multiplayer must be distinct stamped directories');
    for (const key of ['dataPath', 'vanillaFontPath', 'unicodeFontPath']) options[key] = await realpath(options[key]);
    const script = await identify(selfPath, 'server-script');
    const helperRoot = resolve(options.artifactDir, 'helper'), helpers = [];
    const helperInputs = ordinaryHelpers.map(name => ({source: name, target: name}));
    for (const name of compiledContracts) helperInputs.push({
      source: '.cache/build/browser/assets/contracts/' + name + '.mjs',
      target: 'lib/contracts/' + name + '.mjs',
    });
    // No load-compiled-contract/front-end-manifest/launcher-build is copied or
    // imported. The real factory and generated validators remain byte-for-byte.
    for (const item of helperInputs) {
      const source = await realpath(resolve(options.canonicalRoot, item.source));
      if (!within(options.canonicalRoot, source)) throw Error('Helper source escapes canonical owner: ' + item.source);
      const identity = await identify(source, 'helper-source');
      const bytes = await readFile(source);
      if (bytes.length !== identity.bytes || digest(bytes) !== identity.sha256) throw Error('Helper source changed before copying: ' + source);
      const target = resolve(helperRoot, item.target);
      await mkdir(dirname(target), {recursive: true});
      await writeFile(target, bytes, {flag: 'wx'});
      const copy = await identify(target, 'helper-copy');
      if (copy.sha256 !== identity.sha256 || copy.bytes !== identity.bytes) throw Error('Helper copy identity mismatch: ' + item.target);
      helpers.push({source, target, ...identity});
    }
    const [{readUiArtifact}, {createPublishedSiteServer, safeSiteFile},
      {createDevelopmentHostManifestFromContent}, {PRODUCT_CONTENT}, {isUiDeploymentNavigation}] = await Promise.all([
      import(pathToFileURL(resolve(helperRoot, 'lib/ui-artifact.mjs')).href),
      import(pathToFileURL(resolve(helperRoot, 'server/ui-static-server.mjs')).href),
      import(pathToFileURL(resolve(helperRoot, 'lib/development-host-manifest.mjs')).href),
      import(pathToFileURL(resolve(helperRoot, 'lib/content-definition.mjs')).href),
      import(pathToFileURL(resolve(helperRoot, 'scripts/ui-deployment-contract.mjs')).href),
    ]);
    const ui = await readUiArtifact(options.uiArtifactRoot);
    if (options.expectedUiArtifactId && ui.artifactId !== options.expectedUiArtifactId) throw Error('UI artifact differs from expectedUiArtifactId');
    mountPath = ui.mountPath;
    const uiMetadataPath = await containedFile(options.uiArtifactRoot, 'ui-artifact.json');
    const uiMetadata = await identify(uiMetadataPath, 'ui-metadata');
    if (JSON.parse(await readFile(uiMetadataPath, 'utf8')).artifactId !== ui.artifactId) throw Error('UI metadata changed during validation');
    for (const item of ui.files) {
      const path = await containedFile(options.uiArtifactRoot, item.path);
      const observed = await identify(path, 'ui-file');
      if (observed.sha256 !== item.sha256 || observed.bytes !== item.bytes) throw Error('UI changed after readUiArtifact: ' + item.path);
    }
    async function routeFile(route, root, name) {
      if (routes.has(route)) throw Error('Duplicate HTTP route: ' + route);
      const path = await containedFile(root, name);
      const identity = tracked.get(path) || await identify(path, 'served-file');
      routes.set(route, {root, name, path, identity});
    }
    for (const name of ui.publishedFiles) await routeFile('/' + name, options.uiArtifactRoot, name);
    routes.set('/', routes.get('/index.html'));

    async function runtime(root, variant, prefix) {
      const inventoryPath = await containedFile(root, 'runtime-files.json');
      const inventoryIdentity = await identify(inventoryPath, 'runtime-inventory');
      const inventory = JSON.parse(await readFile(inventoryPath, 'utf8'));
      if (inventory.schema !== 'eagler-touhou/runtime-directory/1' || !inventory.files ||
          Array.isArray(inventory.files) || typeof inventory.files !== 'object') throw Error('Invalid Runtime inventory: ' + root);
      const names = Object.keys(inventory.files);
      if (names.length > 256 || runtimeRequired.some(name => !names.includes(name))) throw Error('Incomplete Runtime directory: ' + root);
      for (const name of names) {
        const item = inventory.files[name];
        if (!safeName(name) || name === 'runtime-files.json' || !Number.isSafeInteger(item?.bytes) ||
            item.bytes <= 0 || !/^[a-f0-9]{64}$/i.test(item.sha256 || '')) throw Error('Invalid Runtime file declaration: ' + name);
        const path = await containedFile(root, name), actual = await identify(path, 'runtime-' + variant);
        if (actual.bytes !== item.bytes || actual.sha256 !== item.sha256.toLowerCase()) throw Error('Runtime file identity mismatch: ' + name);
        await routeFile(prefix + name, root, name);
      }
      const html = await readFile(resolve(root, 'th09.html'), 'utf8');
      if (html.split('<meta name="eagler-runtime-variant" content="' + variant + '">').length !== 2 ||
          !html.includes('<meta name="eagler-data-provider" content="retail-memory">')) throw Error('Incorrect Runtime carrier/variant marker: ' + root);
      const version = JSON.parse(await readFile(resolve(root, 'version.json'), 'utf8'));
      const wasm = inventory.files['th09.wasm'].sha256.toLowerCase();
      if (wasm !== options.expectedWasmSha256 || version.game !== 'th09' || version.build !== wasm.slice(0, 24)) throw Error('Runtime is not the frozen release WASM: ' + root);
      await routeFile(prefix + 'runtime-files.json', root, 'runtime-files.json');
      return {root, variant, inventory: inventoryIdentity, files: inventory.files, version};
    }
    const normal = await runtime(options.normalRuntimeRoot, 'normal', '/runtime/th09/');
    const multiplayer = await runtime(options.multiplayerRuntimeRoot, 'multiplayer', '/runtime/th09/multiplayer/');
    if (runtimeRequired.filter(name => name !== 'th09.html').some(name =>
        normal.files[name].sha256 !== multiplayer.files[name].sha256)) throw Error('Variant code/resource bytes disagree');
    const data = await identify(options.dataPath, 'retail-data');
    const vanillaFont = await identify(options.vanillaFontPath, 'vanilla-font');
    const unicodeFont = await identify(options.unicodeFontPath, 'unicode-font');
    if (data.bytes < 1000000 || !/^th09\.(?:dat|data)$/i.test(basename(options.dataPath))) throw Error('TH09 DATA must be the explicit real th09.dat/th09.data');
    await routeFile('/' + basename(options.dataPath), dirname(options.dataPath), basename(options.dataPath));
    await routeFile('/shared/msgothic.ttc', dirname(options.vanillaFontPath), basename(options.vanillaFontPath));
    await routeFile('/shared/unifont.otf', dirname(options.unicodeFontPath), basename(options.unicodeFontPath));
    const host = await createDevelopmentHostManifestFromContent({
      shared: {vanillaFont: 'shared/msgothic.ttc', unicodeFont: 'shared/unifont.otf'},
      games: {th09: {runtime: 'runtime/th09/th09.html?hosted=1',
        multiplayerRuntime: 'runtime/th09/multiplayer/th09.html?hosted=1',
        data: {source: basename(options.dataPath), layout: PRODUCT_CONTENT.th09.dataLayout}, music: {}}},
    }, {sourceRoot: dirname(options.dataPath), games: ['th09'], ...(options.netplayRelay ? {netplayRelay: options.netplayRelay} : {})});
    if (host.games.th09.gameData.sha256 !== data.sha256 || host.games.th09.gameData.bytes !== data.bytes) throw Error('Factory DATA identity differs from served DATA');
    const metadata = new Map([
      ['/host-manifest.json', JSON.stringify(host) + '\n'],
      ['/release-catalog.json', JSON.stringify({schema: 'eagler-touhou/release-catalog/1', games: {}}) + '\n'],
    ]);
    const inputCheck = await auditInputs();
    if (!inputCheck.stable) throw Error('Input bytes changed during initialization');
    const identity = {schema: 'th09/managed-touch-launcher-inputs/1', ...stamp(),
      scope: 'hosted web-development; does not claim immutable publication or browser/input acceptance',
      script, helpers, ui: {sourceRoot: options.uiSourceRoot, root: ui.root, artifactId: ui.artifactId, mountPath, metadata: uiMetadata, files: ui.files},
      runtimes: {normal, multiplayer}, content: {data, vanillaFont, unicodeFont},
      hostManifest: host, hostManifestSha256: digest(metadata.get('/host-manifest.json')),
      routes: [...routes.keys(), ...metadata.keys()].sort(), probe: {target: '/fonts/blend.bin',
        source: resolve(options.normalRuntimeRoot, 'fonts/blend.bin'), sha256: normal.files['fonts/blend.bin'].sha256,
        timeoutMs: options.probeTimeoutMs, explicitlyInjected: true},
      inputCheck, inputs: [...tracked.values()]};
    const identityFile = await writeJson(resolve(options.artifactDir, 'server-inputs.json'), identity);

    const requestIds = new WeakMap();
    const fullPath = path => mountPath.slice(0, -1) + path;
    const status = () => ({scope: 'controlled font probe, not normal gameplay traffic', stopping,
      ...stamp(), active: stateFor(activeGate), probes: [...gates.values()].map(stateFor),
      eventCount: events.length, errors: structuredClone(errors), journalError});
    async function middleware(request, response, url) {
      const requestId = ++nextRequest, began = stamp(); requestIds.set(request, requestId);
      record('http-request', {requestId, method: request.method, path: url.pathname});
      let finished = false;
      response.once('finish', () => {finished = true; record('http-response', {requestId,
        status: response.statusCode, elapsedMs: performance.now() - began.monotonicMs});});
      response.once('close', () => {if (!finished) record('http-closed', {requestId,
        elapsedMs: performance.now() - began.monotonicMs});});
      if (url.pathname.startsWith(fullPath(controlPath) + '/')) {
        const peer = request.socket.remoteAddress;
        if (!['127.0.0.1', '::1', '::ffff:127.0.0.1'].includes(peer) ||
            request.headers.origin && request.headers.origin !== origin) {
          jsonResponse(response, 403, {error: 'Control API is loopback and same-origin only'}); return true;
        }
        const action = url.pathname.slice(fullPath(controlPath).length + 1);
        try {
          if (url.search) throw Error('Control API does not accept query parameters');
          if (!['status', 'arm', 'release', 'stop'].includes(action)) {jsonResponse(response, 404, {error: 'Not found'}); return true;}
          if (action === 'status' && request.method === 'GET') {jsonResponse(response, 200, status()); return true;}
          if (request.method !== 'POST' || !['arm', 'release', 'stop'].includes(action)) {
            jsonResponse(response, 405, {error: 'Use GET status or POST arm/release/stop'}); return true;
          }
          const body = await bodyJson(request);
          if (action === 'arm') {jsonResponse(response, 200, armGate(body)); return true;}
          if (action === 'release') {
            const gate = [...gates.values()].find(value => value.id === body.gateId);
            if (!gate) throw Error('Unknown gateId');
            jsonResponse(response, 200, {probe: releaseGate(gate, 'control')}); return true;
          }
          jsonResponse(response, 200, {stopping: true, artifactDir: options.artifactDir});
          void stop('control-api').catch(error => console.error(JSON.stringify({event: 'server-stop-failed', error: errorInfo(error)})));
          return true;
        } catch (error) {
          record('control-rejected', {requestId, action, error: errorInfo(error)});
          jsonResponse(response, 400, {error: String(error.message || error)}); return true;
        }
      }
      const localPath = url.pathname.startsWith(mountPath) ? '/' + url.pathname.slice(mountPath.length) : null;
      const gate = localPath ? gates.get(localPath) : null;
      if (!localPath || !routes.has(localPath) && !metadata.has(localPath) && !gate &&
          !isUiDeploymentNavigation(url.pathname, ui.navigation)) {
        jsonResponse(response, 404, {error: 'Not found'}); return true;
      }
      if (gate && request.method === 'GET') {
        const entry = {requestId, entered: stamp(), ended: null, outcome: null};
        gate.requests.push(entry); gate.pending++;
        record('probe-request', {gateId: gate.id, requestId, releasedBeforeRequest: !!gate.released});
        const cancelled = () => {
          if (!response.writableEnded) {record('probe-client-abort', {gateId: gate.id, requestId}); releaseGate(gate, 'client-abort');}
        };
        response.once('close', cancelled);
        try {
          await gate.wait;
          entry.ended = stamp(); entry.outcome = response.destroyed ? 'client-abort' : 'serve-original-font';
          record('probe-wait-ended', {gateId: gate.id, requestId, outcome: entry.outcome, reason: gate.releaseReason});
        } finally {response.off('close', cancelled); gate.pending--;}
        if (response.destroyed) return true;
      }
      return false;
    }
    async function resolveResource(pathname) {
      if (metadata.has(pathname)) return {body: metadata.get(pathname), contentType: 'application/json; charset=utf-8'};
      const entry = gates.has(pathname) ? routes.get('/runtime/th09/fonts/blend.bin') : routes.get(pathname);
      if (!entry) return null;
      const file = await safeSiteFile(entry.root, '/' + entry.name);
      if (!file || !same(file.path, entry.path) || file.info.size !== entry.identity.bytes ||
          file.info.mtimeMs !== entry.identity.mtimeMs) throw Error('Served input disappeared or changed: ' + pathname);
      return {...file, cacheControl: 'no-store'};
    }
    server = await createPublishedSiteServer({root: options.artifactDir, publication: false,
      navigation: ui.navigation, resolveResource, middleware,
      onError: (error, request) => fault('http-error', error, {requestId: requestIds.get(request), url: request.url}),
    });
    server.on('connection', socket => {sockets.add(socket); socket.once('close', () => sockets.delete(socket));});
    server.on('error', error => fault('server-error', error));
    await new Promise((resolveListen, rejectListen) => {
      const failed = error => {server.off('listening', ready); rejectListen(error);};
      const ready = () => {server.off('error', failed); resolveListen();};
      server.once('error', failed); server.once('listening', ready);
      server.listen(options.port, options.host);
    });
    const address = server.address();
    if (!address || typeof address === 'string') throw Error('Server did not obtain a TCP address');
    origin = 'http://' + (options.host === '::1' ? '[::1]' : options.host) + ':' + address.port;
    const url = origin + mountPath, controlUrl = origin + fullPath(controlPath);
    const started = {schema: 'th09/managed-touch-launcher-start/1', ...stamp(), url, controlUrl,
      pid: process.pid, artifactDir: options.artifactDir, identityFile,
      uiSourceRoot: options.uiSourceRoot, uiArtifactId: ui.artifactId, wasmSha256: options.expectedWasmSha256,
      endpoints: {normal: new URL(host.games.th09.runtime, url).href,
        multiplayer: new URL(host.games.th09.multiplayerRuntime, url).href},
      scope: 'real hosted-development carrier; immutable publication/browser assertions not executed by this server'};
    record('listening', {url});
    await writeJson(resolve(options.artifactDir, 'server-started.json'), started);
    return Object.freeze({url, controlUrl, identity, started, closed, stop,
      getStatus: () => structuredClone(status())});
  } catch (error) {
    fault('initialization', error);
    for (const gate of gates.values()) if (!gate.released) releaseGate(gate, 'server-stop');
    try {await closeOwnedServer();}
    catch (cleanupError) {fault('initialization-cleanup', cleanupError);}
    await logTail;
    if (journal) {
      try {await journal.close(); journal = null;}
      catch (closeError) {fault('journal-close', closeError);}
    }
    if (created) {
      try {await writeJson(resolve(options.artifactDir, 'server-initialization-failure.json'),
        {schema: 'th09/managed-touch-launcher-failure/1', ...stamp(), error: errorInfo(error), errors,
          journalError, copiedInputs: [...tracked.values()], artifactDir: options.artifactDir});}
      catch (saveError) {throw new AggregateError([error, saveError], 'Server initialization failed; saving its failure record also failed');}
    }
    throw error;
  }
}

async function cli() {
  const env = process.env;
  const api = await startManagedTouchLauncherServer({
    canonicalRoot: env.MANAGED_TOUCH_CANONICAL_ROOT,
    uiSourceRoot: env.MANAGED_TOUCH_UI_SOURCE_ROOT,
    uiArtifactRoot: env.MANAGED_TOUCH_UI_ROOT,
    normalRuntimeRoot: env.MANAGED_TOUCH_NORMAL_RUNTIME_ROOT,
    multiplayerRuntimeRoot: env.MANAGED_TOUCH_MULTIPLAYER_RUNTIME_ROOT,
    dataPath: env.MANAGED_TOUCH_DATA_PATH,
    vanillaFontPath: env.MANAGED_TOUCH_VANILLA_FONT_PATH,
    unicodeFontPath: env.MANAGED_TOUCH_UNICODE_FONT_PATH,
    artifactDir: env.MANAGED_TOUCH_ARTIFACT_DIR,
    expectedWasmSha256: env.MANAGED_TOUCH_WASM_SHA256,
    ...(env.MANAGED_TOUCH_UI_ARTIFACT_ID ? {expectedUiArtifactId: env.MANAGED_TOUCH_UI_ARTIFACT_ID} : {}),
    ...(env.MANAGED_TOUCH_RELAY_URL ? {netplayRelay: env.MANAGED_TOUCH_RELAY_URL} : {}),
    host: env.MANAGED_TOUCH_HOST || '127.0.0.1',
    port: Number(env.MANAGED_TOUCH_PORT || '0'),
    probeTimeoutMs: Number(env.MANAGED_TOUCH_PROBE_TIMEOUT_MS || '10000'),
  });
  console.log(JSON.stringify({event: 'managed-touch-launcher-ready', ...api.started}));
  const halt = signal => {void api.stop(signal).catch(error => console.error(JSON.stringify({event: 'managed-touch-launcher-stop-error', error: errorInfo(error)})));};
  const sigint = () => halt('SIGINT'), sigterm = () => halt('SIGTERM');
  process.once('SIGINT', sigint); process.once('SIGTERM', sigterm);
  const stopped = await api.closed;
  process.off('SIGINT', sigint); process.off('SIGTERM', sigterm);
  console.log(JSON.stringify({event: 'managed-touch-launcher-stopped', ...stopped,
    inputsAfter: stopped.inputsAfter ? {stable: stopped.inputsAfter.stable, checked: stopped.inputsAfter.checked,
      mismatchCount: stopped.inputsAfter.mismatches.length} : undefined, probes: undefined, errors: undefined}));
  if (!stopped.clean) process.exitCode = 1;
}
if (process.argv[1] && same(resolve(process.argv[1]), selfPath)) {
  cli().catch(error => {console.error(JSON.stringify({event: 'managed-touch-launcher-failed', error: errorInfo(error)})); process.exitCode = 1;});
}
