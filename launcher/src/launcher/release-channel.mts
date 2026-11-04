/** /beta/ owns program publication and native persistence on the same origin. */
export const BETA_STORAGE_PREFIX = 'beta:';
export const BETA_OPT_IN_KEY = 'eagler-touhou-beta-opt-in-v1';

// The stable launcher may predate games introduced by the beta publication.
export function isBetaPublication(site: {
  schema?: unknown;
  protocol?: unknown;
  shared?: {testBuild?: unknown; saveChannel?: unknown};
} | null): boolean {
  return site?.schema === 'eagler-touhou/site-manifest/1' &&
    site.protocol === 'eagler-touhou/1' &&
    site.shared?.testBuild === true && site.shared.saveChannel === 'beta';
}

/** Runs before the beta launcher boots, including when restored from history. */
export function betaAccessGuardScript(mountPath: string): string {
  if (!betaModeForBase(mountPath)) return '';
  const stablePath = mountPath.replace(/beta\/?$/, '');
  return `(() => {
    const key = ${JSON.stringify(BETA_OPT_IN_KEY)};
    function enforce() {
      try { if (localStorage.getItem(key) === '1') return; } catch {}
      const stable = new URL(${JSON.stringify(stablePath)}, location.origin);
      const lang = new URL(location.href).searchParams.get('lang');
      if (lang) stable.searchParams.set('lang', lang);
      location.replace(stable);
    }
    enforce();
    addEventListener('pageshow', event => { if (event.persisted) enforce(); });
    addEventListener('storage', event => { if (event.key === key || event.key === null) enforce(); });
  })();`;
}
export function channelSessionStorage(storage: Storage | null, beta: boolean): Storage | null {
  if (!storage || !beta) return storage;
  const owner = storage;
  const prefix = BETA_STORAGE_PREFIX;
  return {
    get length() {
      return keys().length;
    },
    key(index) {
      return keys()[index] ?? null;
    },
    getItem(key) {
      return storage.getItem(prefix + key);
    },
    setItem(key, value) {
      storage.setItem(prefix + key, value);
    },
    removeItem(key) {
      storage.removeItem(prefix + key);
    },
    clear() {
      for (const key of keys()) storage.removeItem(prefix + key);
    },
  };
  function keys() {
    return Array.from({ length: owner.length }, (_, i) => owner.key(i))
      .filter((key): key is string => key?.startsWith(prefix) === true)
      .map((key) => key.slice(prefix.length));
  }
}
export function betaModeForBase(baseUrl: string): boolean {
  return new URL(baseUrl, 'https://launcher.invalid/').pathname.replace(/\/$/, '').endsWith('/beta');
}
export function releaseChannelUrl(address: string, baseUrl: string, beta: boolean): string {
  const current = new URL(address),
    base = new URL(baseUrl, current);
  const root = betaModeForBase(base.href) ? new URL('../', base) : base;
  const destination = beta ? new URL('beta/', root) : root;
  const page = current.pathname.startsWith(base.pathname) ? current.pathname.slice(base.pathname.length) : '';
  destination.pathname += /^(?:(?:index|en|ja|lobby|faq|about)\.html)?$/.test(page) ? page : '';
  const language = current.searchParams.get('lang');
  if (language) destination.searchParams.set('lang', language);
  return destination.href;
}
