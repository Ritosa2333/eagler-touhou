// Narrow TH128 resource-fetch boundary.
//
// This module is the single explicit resource exit for the page's renderer,
// audio and ending modules. It is NOT a fetch polyfill and it never mutates
// globalThis.fetch, Fetch.prototype or a service worker.
//
// Two explicit modes exist:
//   * standalone (default): the caller gets the page's normal network fetch.
//     This is the existing browser/server behaviour.
//   * imported: the launcher installs an already-validated provider before the
//     dynamic import of play.mjs. Resource requests then go to that provider,
//     and a missing target throws. There is deliberately no asset fallback to
//     the network in this mode. The one narrow exception is the same-origin
//     GET/HEAD runtime/build.json next to the active Runtime entry, which must
//     come from that Runtime directory rather than the asset package. The dynamic core.mjs/
//     wasm files are loaded by the browser module loader and are not fetched by
//     this module.
//
// Caller semantics (AbortSignal, method, headers and any other RequestInit
// member) are passed through unchanged. The provider is responsible for the
// narrow GET/HEAD asset contract and for owner/abort rechecks; this module
// does not catch or rewrite those errors.

let installedProvider = null;

function isRelativeString(input) {
  return typeof input === 'string'
    && !input.startsWith('/')
    && !/^[a-z][a-z0-9+.-]*:/i.test(input);
}

function resolveImportedInput(input) {
  if (!isRelativeString(input)) return input;
  const base = globalThis.location?.href;
  if (!base) {
    throw new TypeError('Imported resource mode cannot resolve a relative resource without a document URL');
  }
  return new URL(input, base).href;
}

/**
 * Installs the validated imported-resource provider for this module instance.
 * The launcher calls this before dynamically importing play.mjs.
 *
 * Re-installing the same provider is idempotent. Replacing a live provider is
 * refused: a page has one installation owner, and silently swapping it would
 * make old callers resolve against new bytes.
 */
export function installResourceFetchProvider(provider) {
  if (!provider || typeof provider.fetch !== 'function') {
    throw new TypeError('Imported resource provider must expose fetch(input, init)');
  }
  if (installedProvider && installedProvider !== provider) {
    throw new Error('A TH128 resource provider is already installed');
  }
  installedProvider = provider;
  return provider;
}

export function getInstalledResourceFetchProvider() {
  return installedProvider;
}

export function isImportedResourceMode() {
  return installedProvider !== null;
}

/**
 * Test/teardown helper. It does not dispose the provider; the installation
 * owner remains responsible for provider.dispose().
 */
export function resetResourceFetchProvider() {
  installedProvider = null;
}

const LOCAL_RUNTIME_BUILD_PATH = 'runtime/build.json';

function resolveRequestUrl(input) {
  try {
    if (typeof Request !== 'undefined' && input instanceof Request) {
      return new URL(input.url);
    }
    if (input instanceof URL) return new URL(input.href);
    if (typeof input !== 'string') return null;
    const base = globalThis.location?.href;
    return base ? new URL(input, base) : null;
  } catch {
    return null;
  }
}

function isLocalRuntimeBuildRequest(input, init) {
  const page = globalThis.location?.href ? resolveRequestUrl(globalThis.location.href) : null;
  const url = resolveRequestUrl(input);
  if (!page || !url) return false;
  if (url.href !== new URL(LOCAL_RUNTIME_BUILD_PATH, page).href) return false;
  const method = String(init?.method ?? (typeof Request !== 'undefined' && input instanceof Request
    ? input.method
    : 'GET')).toUpperCase();
  if (method !== 'GET' && method !== 'HEAD') return false;
  if (init?.body !== undefined && init.body !== null) return false;
  return true;
}

/**
 * The page's single resource exit.
 *
 * @param {string|URL|Request} input
 * @param {RequestInit} [init]
 * @returns {Promise<Response>}
 */
export async function fetchResource(input, init) {
  if (installedProvider) {
    if (isLocalRuntimeBuildRequest(input, init)) {
      if (typeof globalThis.fetch !== 'function') {
        throw new Error('TH128 local Runtime build fetch is unavailable');
      }
      return globalThis.fetch(input, init);
    }
    return installedProvider.fetch(resolveImportedInput(input), init);
  }
  if (typeof globalThis.fetch !== 'function') {
    throw new Error('TH128 standalone resource fetch is unavailable');
  }
  return globalThis.fetch(input, init);
}
