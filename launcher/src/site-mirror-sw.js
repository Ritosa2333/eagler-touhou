"use strict";

// Publication-owned shell, immutable Runtime and public resource discovery.
// Deployment addresses live in a verified, locally cached site-network.json.
function validateSiteNetwork(value) {
  if (value?.schema !== "eagler-touhou/site-network/1" || !Array.isArray(value.mirrors) || value.mirrors.length > 3) {
    throw new Error("Invalid site network configuration");
  }
  const seen = new Set();
  return { schema: value.schema, mirrors: value.mirrors.map(raw => {
    if (typeof raw !== "string") throw new Error("Invalid site mirror URL");
    const url = new URL(raw);
    if (url.protocol !== "https:" || url.username || url.password || url.search || url.hash ||
        !url.pathname.endsWith("/") || seen.has(url.href)) throw new Error("Invalid site mirror URL");
    seen.add(url.href);
    return url.href;
  }) };
}

function createSiteMirrorRouting({ scopeUrl, loadConfig, timeoutMs = 2500, retryAfterMs = 60000 }) {
  const failedUntil = new Map();
  let configuration;
  let lastNode;
  const config = () => configuration ||= Promise.resolve().then(loadConfig).then(validateSiteNetwork)
    .catch(error => { configuration = null; throw error; });
  async function run(url, read, { cache = "no-store", stallMs = timeoutMs } = {}) {
    const original = new URL(url);
    const discovery = original.pathname === "/_eagler/resource-routes";
    if (original.origin !== scopeUrl.origin || !discovery && !original.pathname.startsWith(scopeUrl.pathname)) {
      throw new Error("Site mirror request outside application scope");
    }
    const relative = (discovery ? original.pathname : original.pathname.slice(scopeUrl.pathname.length)) + original.search;
    const bases = [scopeUrl.href, ...(await config()).mirrors].filter((base, index, all) => all.indexOf(base) === index);
    const ready = bases.filter(base => (failedUntil.get(base) || 0) <= Date.now());
    // If every entry failed recently, retry in the declared order.
    const ordered = ready.length ? [...ready, ...bases.filter(base => !ready.includes(base))] : bases;
    let failure;
    for (const base of ordered) {
      const target = new URL(relative, base);
      const controller = new AbortController();
      let timer;
      let status;
      const progress = () => {
        clearTimeout(timer);
        timer = setTimeout(() => controller.abort(new Error("Site mirror download stalled")), stallMs);
      };
      progress();
      try {
        const response = await fetch(new Request(target, {cache, signal: controller.signal,
          credentials: target.origin === scopeUrl.origin ? "same-origin" : "omit",
          redirect: base === scopeUrl.href ? "follow" : "error"}));
        status = response.status;
        if (response.status === 401 || response.status === 403) {
          throw Object.assign(new Error(`Site access denied: HTTP ${response.status}`), {siteAccessDenied: true});
        }
        if (!response.ok) throw new Error(`Site mirror HTTP ${response.status}: ${target}`);
        progress();
        // A synthetic response keeps relative modules and navigation bound to
        // the public origin, rather than the mirror response URL. Fetch already
        // decoded transport compression, so do not carry its wire metadata.
        const headers = new Headers(response.headers);
        headers.delete("content-encoding"); headers.delete("content-length");
        const local = new Response(response.body, {status: response.status, statusText: response.statusText, headers});
        const result = await read(local, progress);
        const node = new URL(response.url || target.href).origin;
        if (lastNode !== node) {
          console.info('[EAGLER|入口]', {entry:new URL(base).origin, node, path:original.pathname});
          lastNode = node;
        }
        failedUntil.delete(base);
        return result;
      } catch (error) {
        controller.abort();
        console.warn('[EAGLER|入口回退]', {node:target.origin, path:original.pathname,
          status, reason:error?.name || 'Error'});
        if (error?.siteAccessDenied) throw error;
        failedUntil.set(base, Date.now() + retryAfterMs);
        failure = error;
      } finally { clearTimeout(timer); }
    }
    throw failure || new Error("No site mirror available");
  }
  return {run};
}
