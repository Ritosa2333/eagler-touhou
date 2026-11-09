import { accountsEnabled } from '../release-features';

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
type RequestOptions = {signal?: AbortSignal; responseType?: 'json' | 'blob'; timeoutMs?: number};
const activeRequests = new Set<AbortController>();

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

function accountErrorCode(value: unknown) {
  if (!value || typeof value !== 'object') return 'ACCOUNT_REQUEST_FAILED';
  const record = value as Record<string, unknown>;
  const nested =
    record.error && typeof record.error === 'object' ? (record.error as Record<string, unknown>) : undefined;
  const code = record.code ?? nested?.code;
  return typeof code === 'string' && /^[A-Z0-9_]+$/.test(code) ? code : 'ACCOUNT_REQUEST_FAILED';
}

async function accountReady(origin: string, signal?: AbortSignal) {
  const controller = new AbortController();
  const abort = () => controller.abort();
  signal?.addEventListener('abort', abort, {once: true});
  if (signal?.aborted) controller.abort();
  const timeout = setTimeout(abort, 3500);
  try {
    const response = await fetch(`${origin}/api/ready`, {
      credentials: 'include', cache: 'no-store', signal: controller.signal,
    });
    if (!response.ok || (await response.json()).ready !== true) return null;
    const region = response.headers.get('X-Eagler-Account-Route');
    return {region: region === 'direct' || region === 'edge' ? region : undefined};
  } catch {
    return null;
  } finally {
    clearTimeout(timeout);
    signal?.removeEventListener('abort', abort);
  }
}

async function selectAccountOrigin(signal?: AbortSignal) {
  const origins = configuredAccountOrigins();
  if (!origins) return await accountReady('', signal) ? '' : null;
  const {edge, direct} = origins;
  let siteReady: ReturnType<typeof accountReady> | undefined;
  if (accountRegion === 'direct') {
    siteReady = accountReady('', signal);
    if ((await siteReady)?.region === 'direct') return '';
  }
  if (accountRegion === 'edge') {
    const ready = await accountReady(edge, signal);
    if (ready?.region === 'direct') {
      accountRegion = 'direct';
      return await accountReady('', signal) ? '' : null;
    }
    if (ready) return edge;
    const fallback = await accountReady(direct, signal);
    if (fallback?.region === 'direct') {
      accountRegion = 'direct';
      return await accountReady('', signal) ? '' : null;
    }
    return fallback ? direct : null;
  }

  // The overseas site itself may be served by Cloudflare and have no same-origin API.
  siteReady ??= accountReady('', signal);
  const edgeReady = accountReady(edge, signal);
  const first = await Promise.any([siteReady, edgeReady].map(async probe => {
    const ready = await probe;
    if (!ready) throw new Error('Account ingress unavailable');
    return ready;
  })).catch(() => null);
  accountRegion = first?.region;
  if (accountRegion === 'direct') return await siteReady ? '' : null;
  if (await edgeReady) return edge;
  return await accountReady(direct, signal) ? direct : null;
}

export async function accountRequest<T = unknown>(path: string, body?: Record<string, unknown> | FormData, options: RequestOptions = {}): Promise<T> {
  const controller = new AbortController();
  const abort = () => controller.abort();
  options.signal?.addEventListener('abort', abort, {once: true});
  if (options.signal?.aborted) controller.abort();
  activeRequests.add(controller);
  try {
    const result = await sendAccountRequest<T>(path, body, {...options, signal: controller.signal});
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
  const origin = await selectAccountOrigin(options.signal);
  if (options.signal?.aborted) throw new DOMException('Request cancelled', 'AbortError');
  if (origin === null) throw new AccountRequestError('ACCOUNT_API_UNAVAILABLE');
  try {
    return await fetchAccount<T>(origin, path, body, options);
  } catch (error) {
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
    const response = await fetch(`${origin}/api/${path}`, {
      credentials: 'include',
      cache: 'no-store',
      method: hasBody ? 'POST' : 'GET',
      headers: hasBody && !multipart ? {'Content-Type': 'application/json'} : undefined,
      body: hasBody ? multipart ? body : JSON.stringify(body) : undefined,
      signal: controller.signal,
    });
    if (transientStatuses.has(response.status)) throw new AccountRequestError('ACCOUNT_API_UNAVAILABLE', response.status);
    if (response.ok && options.responseType === 'blob') return await response.blob() as T;
    let data: unknown;
    try {data = await response.json();}
    catch {
      throw new AccountRequestError(response.ok ? 'ACCOUNT_RESPONSE_INVALID' : 'ACCOUNT_REQUEST_FAILED', response.status);
    }
    if (!response.ok) throw new AccountRequestError(accountErrorCode(data), response.status);
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
