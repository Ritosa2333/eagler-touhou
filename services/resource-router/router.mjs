import http from 'node:http';
import { createHash } from 'node:crypto';
import { readFile, readdir } from 'node:fs/promises';
import { pathToFileURL } from 'node:url';

// Weighted rendezvous hashing keeps one client on one eligible node.
export function rankNodes(nodes, key) {
  return nodes.map(node => {
    const value = createHash('sha256').update(key + '\0' + node.id).digest().readUInt32BE();
    return { node, score: -Math.log((value + 1) / 4294967297) / node.weight };
  }).sort((a, b) => a.score - b.score).map(value => value.node);
}

export function chooseNode(nodes, key) { return rankNodes(nodes, key)[0]; }

export function safePath(raw) {
  const path = raw.split('?')[0];
  let decoded;
  try { decoded = decodeURIComponent(path); } catch { return false; }
  return (/^\/(games|shared|language|language-updates)\//.test(path) || /^\/runtime\/(?:[A-Za-z0-9_-]+\/)+[a-f0-9]{64}\/fonts\/[A-Za-z0-9._-]+$/.test(path)) && !/[\\\x00-\x20\x7f]/.test(decoded)
    && !decoded.split('/').some(part => part === '.' || part === '..')
    && !/%(?:2f|5c|25)/i.test(path);
}

const metadataRequests = new Map();
const rangeRequests = new Map();
let activeProbes = 0;
const probeQueue = [];
async function boundedProbe(task) {
  if (activeProbes >= 4) await new Promise(resolve => probeQueue.push(resolve));
  else activeProbes++;
  try { return await task(); }
  finally {
    const next = probeQueue.shift();
    if (next) next();
    else activeProbes--;
  }
}
async function fetchJson(url) {
  if (!metadataRequests.has(url)) metadataRequests.set(url, boundedProbe(async () => {
    const response = await fetch(url, { redirect: 'error', signal: AbortSignal.timeout(15000) });
    if (!response.ok) throw new Error(`HTTP ${response.status}`);
    const body = await response.text();
    if (body.length > 4_000_000) throw new Error('metadata too large');
    return JSON.parse(body);
  }));
  return metadataRequests.get(url);
}
async function probeRange(url, host) {
  // Public resource nodes expose wildcard CORS. Keep host-specific responses
  // separate by probing again when the first response does not cover this host.
  const key = url + '\0' + host;
  const shared = rangeRequests.get(url);
  if (shared) {
    const response = await shared;
    if (response.headers.get('access-control-allow-origin') === '*') return response;
  }
  if (!rangeRequests.has(key)) {
    const promise = boundedProbe(async () => {
      const response = await fetch(url, {
        headers: { Range: 'bytes=0-0', Origin: `https://${host}` },
        redirect: 'error', signal: AbortSignal.timeout(15000),
      });
      const result = { status: response.status, headers: response.headers };
      await response.body?.cancel();
      return result;
    });
    rangeRequests.set(key, promise);
    if (!shared) rangeRequests.set(url, promise);
  }
  return rangeRequests.get(key);
}

export async function start(config) {
  const states = new Map();
  for (const [host, pool] of Object.entries(config.sites)) {
    for (const node of pool.nodes) {
      const url = new URL(node.origin);
      if (url.protocol !== 'https:' || url.pathname !== '/' || url.search || url.hash || url.username
          || !(node.weight > 0)) throw new Error(`Invalid node: ${node.id}`);
    }
    states.set(host, { pool, nodes: [], inventory: new Map(), updated: 0 });
  }
  async function refreshSite(host, state) {
    try {
      const names = (await readdir(state.pool.root)).filter(name => /^th\d+\.package\.json$/.test(name));
      if (!names.length) throw new Error('No local package descriptors');
      const descriptors = await Promise.all(names.map(async name => [name,
        JSON.parse(await readFile(`${state.pool.root}/${name}`, 'utf8'))]));
      const inventory = new Map();
      for (const [, descriptor] of descriptors) for (const file of Object.values(descriptor.files)) {
        const path = '/' + file.source.replace(/^\/+/, '');
        if (!safePath(path)) throw new Error(`Unsafe package source: ${path}`);
        const previous = inventory.get(path);
        if (previous && previous.sha256 !== file.sha256) throw new Error(`Conflicting resource: ${path}`);
        inventory.set(path, file);
      }
      const deployment = JSON.parse(await readFile(`${state.pool.root}/deployment.json`, 'utf8'));
      const fonts = deployment.files.filter(file => file.path.startsWith('runtime/') && file.path.includes('/fonts/') && safePath('/' + file.path));
      for (const file of fonts) inventory.set('/' + file.path, file);
      const nodes = await Promise.all(state.pool.nodes.map(async node => {
        const paths = new Set();
        const errors = [];
        if (node.inventoryFile) {
          // Deployment-owned inventory is published after byte/hash verification
          // on the data node. Client redirects do not depend on origin-side RTT.
          const published = JSON.parse(await readFile(node.inventoryFile, 'utf8'));
          for (const [path, expected] of inventory) {
            const actual = published[path];
            if (actual?.sha256 === expected.sha256 && actual.bytes === expected.bytes) paths.add(path);
          }
          return { ...node, paths, errors };
        }
        await Promise.all(descriptors.map(async ([name, local]) => {
          try {
            const remote = await fetchJson(node.origin + '/' + name);
            const files = new Map(Object.values(remote.files).map(file => [file.source, file]));
            const data = local.files['game-data'];
            const response = await probeRange(node.origin + '/' + data.source, host);
            const cors = response.headers.get('access-control-allow-origin');
            const good = response.status === 206
              && response.headers.get('content-range') === `bytes 0-0/${data.bytes}`
              && (cors === '*' || cors === `https://${host}`);
            await response.body?.cancel();
            if (!good) throw new Error('Range/CORS/data length mismatch');
            for (const file of Object.values(local.files)) {
              const match = files.get(file.source);
              if (match?.sha256 === file.sha256 && match.bytes === file.bytes) paths.add('/' + file.source);
            }
          } catch (error) { errors.push(`${name}: ${error.message}`); }
        }));
        try {
          const remote = await fetchJson(node.origin + '/runtime-font-inventory.json');
          const published = new Map(remote.files.map(file => [file.path, file]));
          const matching = fonts.filter(file => {
            const other = published.get(file.path);
            return other?.sha256 === file.sha256 && other.bytes === file.bytes;
          });
          if (matching.length) {
            const sample = matching[0];
            const response = await probeRange(node.origin + '/' + sample.path, host);
            const cors = response.headers.get('access-control-allow-origin');
            const good = response.status === 206 && response.headers.get('content-range') === `bytes 0-0/${sample.bytes}` && (cors === '*' || cors === `https://${host}`);
            await response.body?.cancel();
            if (!good) throw new Error('Font Range/CORS mismatch');
            for (const file of matching) paths.add('/' + file.path);
          }
        } catch (error) { errors.push(`runtime-fonts: ${error.message}`); }
        return { ...node, paths, errors };
      }));
      state.inventory = inventory;
      state.nodes = nodes;
      state.updated = Date.now();
      state.error = null;
    } catch (error) {
      state.error = error.message;
      state.nodes = [];
      console.error(host, error.message);
    }
  }
  async function refresh() {
    metadataRequests.clear();
    rangeRequests.clear();
    await Promise.all([...states].map(([host, state]) => refreshSite(host, state)));
  }
  const server = http.createServer((request, response) => {
    response.setHeader('Cache-Control', 'no-store');
    if (request.url === '/_status') {
      response.setHeader('Content-Type', 'application/json');
      response.end(JSON.stringify(Object.fromEntries([...states].map(([host, state]) => [host, {
        updated: state.updated, error: state.error,
        nodes: state.nodes.map(({ id, paths, errors }) => ({ id, eligibleFiles: paths.size, errors })),
      }]))));
      return;
    }
    if (!['GET', 'HEAD'].includes(request.method)) {
      response.writeHead(405, { Allow: 'GET, HEAD' }); response.end(); return;
    }
    const discovery = new URL(request.url, 'https://resource.invalid');
    const discover = discovery.pathname === '/_eagler/resource-routes';
    const target = discover ? discovery.searchParams.get('path') : request.url;
    const state = states.get(request.headers['x-resource-site']);
    if (!state || !target?.startsWith('/') || target.startsWith('//') || !safePath(target)) { response.writeHead(404); response.end(); return; }
    const path = target.split('?')[0];
    if (!state.inventory.has(path)) { response.writeHead(404); response.end(); return; }
    const healthy = Date.now() - state.updated < 90000 ? state.nodes.filter(node => node.paths.has(path)) : [];
    const bucket = Math.floor(Date.now() / 300000);
    const primary = healthy.filter(node => !node.emergency);
    const eligible = primary.length ? primary : healthy;
    const key = `${request.headers['x-resource-client'] || request.socket.remoteAddress}:${bucket}`;
    const node = chooseNode(eligible, key);
    const url = new URL(target, 'https://resource.invalid');
    if (discover) {
      const sources = [...rankNodes(primary, key), ...rankNodes(healthy.filter(item => item.emergency), key)]
        .map(item => item.origin + url.pathname + url.search);
      const file = state.inventory.get(path);
      response.setHeader('Content-Type', 'application/json');
      response.writeHead(sources.length ? 200 : 503);
      response.end(JSON.stringify({schema: 'eagler-touhou/resource-routes/1', path, bytes: file.bytes, sha256: file.sha256, sources}));
      return;
    }
    if (!node) { response.writeHead(503, { 'Retry-After': '15' }); response.end(); return; }
    response.writeHead(307, { Location: node.origin + request.url }); response.end();
  });
  // Only nginx may access this service; never bind a public interface.
  await new Promise((resolve, reject) => {
    server.once('error', reject);
    server.listen(config.port ?? 18290, '127.0.0.1', resolve);
  });
  await refresh();
  let stopped = false;
  async function loop() {
    if (stopped) return;
    await refresh();
    if (!stopped) timer = setTimeout(loop, 15000);
  }
  let timer = setTimeout(loop, 15000);
  return { server, refresh, close() { stopped = true; clearTimeout(timer); server.close(); } };
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  start(JSON.parse(await readFile(process.argv[2], 'utf8'))).catch(error => {
    console.error(error); process.exit(1);
  });
}
