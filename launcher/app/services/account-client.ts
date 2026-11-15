import { accountsEnabled } from '../release-features';
import { openMailCaptcha } from './mail-captcha';

const transientStatuses = new Set([502, 503, 504, 520, 521, 522, 523, 524, 525, 526, 527]);
const edgeOrigin = import.meta.env?.VITE_ACCOUNT_EDGE_ORIGIN;
const directOrigin = import.meta.env?.VITE_ACCOUNT_DIRECT_ORIGIN;
type AccountRegion = 'direct' | 'edge';
let accountRegion: AccountRegion | undefined;

function configuredAccountOrigins() {
  const site = globalThis.location?.origin;
  if (!site?.startsWith('https://')) return null;
  const domain = site.slice('https://'.length);
  return edgeOrigin === `https://api.${domain}` && directOrigin === `https://direct-api.${domain}`
    ? {edge: edgeOrigin, direct: directOrigin} : null;
}
type RequestOptions = {signal?: AbortSignal; query?: URLSearchParams; responseType?: 'json' | 'blob'; timeoutMs?: number; emailCaptcha?: boolean; anonymous?: boolean; fresh?: boolean};
type AccountReady = {origin: string; region?: AccountRegion; captchaAppId?: string};
type SharedRead<T> = {controller: AbortController; result: Promise<T>; waiters: number};
const activeRequests = new Set<AbortController>();
const readiness = new Map<string, SharedRead<AccountReady | null>>();
const readyOrigins = new Map<string, {value: AccountReady; until: number}>();
const publicProfiles = new Map<string, SharedRead<unknown>>();
const readinessTtlMs = 30_000;
const cancelRead = () => new DOMException('Request cancelled', 'AbortError');

function sharedRead<T>(pending: Map<string, SharedRead<T>>, key: string, load: (signal: AbortSignal) => Promise<T>, signal?: AbortSignal): Promise<T> {
  if (signal?.aborted) return Promise.reject(cancelRead());
  let read = pending.get(key);
  if (!read) {
    const controller = new AbortController();
    const result = load(controller.signal).finally(() => {
      if (pending.get(key)?.controller === controller) pending.delete(key);
    });
    read = {controller, result, waiters:0};
    pending.set(key, read);
  }
  const current = read;
  current.waiters++;
  return new Promise((resolve, reject) => {
    let waiting = true;
    const finish = () => {
      if (!waiting) return false;
      waiting = false;
      signal?.removeEventListener('abort', abort);
      if (--current.waiters === 0 && pending.get(key) === current) {
        // React can replace a subscriber in the same turn during hydration.
        queueMicrotask(() => {
          if (current.waiters || pending.get(key) !== current) return;
          pending.delete(key);
          current.controller.abort();
        });
      }
      return true;
    };
    const abort = () => {if (finish()) reject(cancelRead());};
    signal?.addEventListener('abort', abort, {once:true});
    void current.result.then(value => {if (finish()) resolve(value);}, error => {if (finish()) reject(error);});
    if (signal?.aborted) abort();
  });
}

export function cancelAccountRequests() {
  for (const controller of activeRequests) controller.abort();
}

export class AccountRequestError extends Error {
  constructor(
    public readonly code: string,
    public readonly status?: number,
  ) {
    super(code);
    this.name = 'AccountRequestError';
  }
}

export function mailCaptchaError(cause: unknown) {
  if (!(cause instanceof AccountRequestError)) return;
  if (cause.code === 'CAPTCHA_INVALID') return 'account.captchaInvalid' as const;
  if (cause.code === 'CAPTCHA_UNAVAILABLE') return 'account.captchaUnavailable' as const;
}

function accountErrorCode(value: unknown) {
  if (!value || typeof value !== 'object') return 'ACCOUNT_REQUEST_FAILED';
  const record = value as Record<string, unknown>;
  const nested =
    record.error && typeof record.error === 'object' ? (record.error as Record<string, unknown>) : undefined;
  const code = record.code ?? nested?.code;
  return typeof code === 'string' && /^[A-Z0-9_]+$/.test(code) ? code : 'ACCOUNT_REQUEST_FAILED';
}

async function probeAccountOrigin(origin: string, signal: AbortSignal): Promise<AccountReady | null> {
  const controller = new AbortController();
  const abort = () => controller.abort();
  signal?.addEventListener('abort', abort, {once: true});
  if (signal?.aborted) controller.abort();
  const timeout = setTimeout(abort, import.meta.env?.DEV ? 15000 : 3500);
  try {
    const response = await fetch(`${origin}/api/ready`, {
      credentials: 'include', cache: 'no-store', signal: controller.signal,
    });
    if (!response.ok) return null;
    const data = await response.json();
    if (data?.ready !== true) return null;
    const region = response.headers.get('X-Eagler-Account-Route');
    return {origin, region: region === 'direct' || region === 'edge' ? region : undefined,
      captchaAppId: typeof data.captchaAppId === 'string' && data.captchaAppId ? data.captchaAppId : undefined};
  } catch {
    return null;
  } finally {
    clearTimeout(timeout);
    signal?.removeEventListener('abort', abort);
  }
}

function accountReady(origin: string, signal?: AbortSignal): Promise<AccountReady | null> {
  if (signal?.aborted) return Promise.reject(cancelRead());
  const cached = readyOrigins.get(origin);
  if (cached && cached.until > Date.now()) return Promise.resolve(cached.value);
  return sharedRead(readiness, origin, async probeSignal => {
    const value = await probeAccountOrigin(origin, probeSignal);
    if (value && !probeSignal.aborted) readyOrigins.set(origin, {value, until:Date.now() + readinessTtlMs});
    return value;
  }, signal);
}

async function selectAccountEndpoint(signal?: AbortSignal) {
  const origins = configuredAccountOrigins();
  if (!origins) return accountReady('', signal);
  const {edge, direct} = origins;
  let siteReady: ReturnType<typeof accountReady> | undefined;
  if (accountRegion === 'direct') {
    siteReady = accountReady(direct, signal);
    const ready = await siteReady;
    return ready ?? accountReady('', signal);
  }
  if (accountRegion === 'edge') {
    const ready = await accountReady(edge, signal);
    if (ready?.region === 'direct') {
      accountRegion = 'direct';
      return (await accountReady(direct, signal)) ?? accountReady('', signal);
    }
    if (ready) return ready;
    const fallback = await accountReady(direct, signal);
    if (fallback?.region === 'direct') {
      accountRegion = 'direct';
      return fallback;
    }
    return fallback;
  }

  // The overseas site itself may be served by Cloudflare and have no same-origin API.
  siteReady = accountReady(direct, signal);
  const edgeReady = accountReady(edge, signal);
  const first = await Promise.any([siteReady, edgeReady].map(async probe => {
    const ready = await probe;
    if (!ready) throw new Error('Account ingress unavailable');
    return ready;
  })).catch(() => null);
  accountRegion = first?.region;
  if (accountRegion === 'direct') return (await siteReady) ?? accountReady('', signal);
  return (await edgeReady) ?? (await siteReady) ?? accountReady('', signal);
}

export async function accountRequest<T = unknown>(path: string, body?: Record<string, unknown> | FormData, options: RequestOptions = {}): Promise<T> {
  const publicProfile = body === undefined && !options.query && !options.responseType && /^v1\/profiles\/[a-z0-9._-]+$/i.test(path);
  if (publicProfile && !options.fresh) {
    return sharedRead(publicProfiles, path.toLowerCase(), signal => sendAccountRequest(path, undefined,
      {...options, signal, anonymous:true}), options.signal) as Promise<T>;
  }
  const anonymous = body === undefined && (publicProfile || options.anonymous);
  const controller = new AbortController();
  const abort = () => controller.abort();
  options.signal?.addEventListener('abort', abort, {once: true});
  if (options.signal?.aborted) controller.abort();
  if (!anonymous) activeRequests.add(controller);
  try {
    const result = await sendAccountRequest<T>(path, body, {...options, anonymous, signal: controller.signal});
    if (controller.signal.aborted) throw new DOMException('Request cancelled', 'AbortError');
    return result;
  } finally {
    activeRequests.delete(controller);
    options.signal?.removeEventListener('abort', abort);
  }
}

async function sendAccountRequest<T>(path: string, body: Record<string, unknown> | FormData | undefined, options: RequestOptions): Promise<T> {
  if (!accountsEnabled) throw new AccountRequestError('ACCOUNT_FEATURE_DISABLED');
  const segments = path.split('/');
  if (
    !/^[a-z0-9._/-]+$/i.test(path) ||
    path.startsWith('/') ||
    segments.some((segment) => !segment || segment === '.' || segment === '..')
  )
    throw new AccountRequestError('ACCOUNT_REQUEST_FAILED');
  const endpoint = await selectAccountEndpoint(options.signal);
  if (options.signal?.aborted) throw new DOMException('Request cancelled', 'AbortError');
  if (!endpoint) throw new AccountRequestError('ACCOUNT_API_UNAVAILABLE');
  const {origin, captchaAppId} = endpoint;
  if (options.emailCaptcha && captchaAppId) {
    if (!body || body instanceof FormData) throw new AccountRequestError('ACCOUNT_REQUEST_FAILED');
    try {
      const captchaToken = await openMailCaptcha(captchaAppId, options.signal);
      body = {...body, captchaToken};
    } catch (error) {
      if (error instanceof DOMException && error.name === 'AbortError') throw error;
      throw new AccountRequestError('CAPTCHA_UNAVAILABLE');
    }
  }
  try {
    return await fetchAccount<T>(origin, path, body, options);
  } catch (error) {
    if (error instanceof AccountRequestError && error.code === 'ACCOUNT_API_UNAVAILABLE') readyOrigins.delete(origin);
    const origins = configuredAccountOrigins();
    // A write can reach the server even when the client never sees the response.
    if (!origins || origin !== origins.edge || body !== undefined || !(error instanceof AccountRequestError)
      || error.code !== 'ACCOUNT_API_UNAVAILABLE' || !await accountReady(origins.direct, options.signal)) throw error;
    return fetchAccount<T>(origins.direct, path, body, options);
  }
}

async function fetchAccount<T>(origin: string, path: string, body: Record<string, unknown> | FormData | undefined, options: RequestOptions): Promise<T> {
  const hasBody = body !== undefined;
  const controller = new AbortController(), abort = () => controller.abort();
  options.signal?.addEventListener('abort', abort, {once: true});
  if (options.signal?.aborted) controller.abort();
  const timeout = setTimeout(abort, options.timeoutMs ?? 20000);
  try {
    const multipart = body instanceof FormData;
    const query = options.query?.toString();
    const response = await fetch(`${origin}/api/${path}${query ? '?' + query : ''}`, {
      credentials: options.anonymous ? 'omit' : 'include',
      cache: 'no-store',
      method: hasBody ? 'POST' : 'GET',
      headers: hasBody && !multipart ? {'Content-Type': 'application/json'} : undefined,
      body: hasBody ? multipart ? body : JSON.stringify(body) : undefined,
      signal: controller.signal,
    });
    const transient = transientStatuses.has(response.status);
    if (transient && (response.status !== 503 || !options.emailCaptcha))
      throw new AccountRequestError('ACCOUNT_API_UNAVAILABLE', response.status);
    if (response.ok && options.responseType === 'blob') return await response.blob() as T;
    let data: unknown;
    try {data = await response.json();}
    catch {
      if (transient) throw new AccountRequestError('ACCOUNT_API_UNAVAILABLE', response.status);
      throw new AccountRequestError(response.ok ? 'ACCOUNT_RESPONSE_INVALID' : 'ACCOUNT_REQUEST_FAILED', response.status);
    }
    if (transient && accountErrorCode(data) !== 'CAPTCHA_UNAVAILABLE')
      throw new AccountRequestError('ACCOUNT_API_UNAVAILABLE', response.status);
    if (!response.ok) throw new AccountRequestError(accountErrorCode(data), response.status);
    if (origin && path.endsWith('?media=urls') && data && typeof data === 'object' && 'media' in data) {
      const media = data.media;
      if (media && typeof media === 'object') {
        for (const kind of ['avatar', 'cover'] as const) {
          const fields = media as Record<string, unknown>;
          const value = fields[kind];
          if (typeof value === 'string' && /^\/api\/v1\/profiles\/[\w.-]+\/media\/(avatar|cover)\/[a-f0-9]{32}$/.test(value))
            fields[kind] = origin + value;
        }
      }
    }
    return data as T;
  } catch (error) {
    if (options.signal?.aborted) throw new DOMException('Request cancelled', 'AbortError');
    if (error instanceof AccountRequestError) throw error;
    throw new AccountRequestError(hasBody ? 'ACCOUNT_REQUEST_FAILED' : 'ACCOUNT_API_UNAVAILABLE');
  } finally {
    clearTimeout(timeout);
    options.signal?.removeEventListener('abort', abort);
  }
}
