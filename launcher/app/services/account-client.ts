import { accountsEnabled } from '../release-features';

const transientStatuses = new Set([502, 503, 504]);
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

async function accountReady(signal?: AbortSignal) {
  const controller = new AbortController();
  const abort = () => controller.abort();
  signal?.addEventListener('abort', abort, {once: true});
  if (signal?.aborted) controller.abort();
  const timeout = setTimeout(abort, 3500);
  try {
    const response = await fetch('/api/ready', {
      credentials: 'include', cache: 'no-store', signal: controller.signal,
    });
    return response.ok && (await response.json()).ready === true;
  } catch {
    return false;
  } finally {
    clearTimeout(timeout);
    signal?.removeEventListener('abort', abort);
  }
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
  const hasBody = body !== undefined;
  const ready = await accountReady(options.signal);
  if (options.signal?.aborted) throw new DOMException('Request cancelled', 'AbortError');
  if (!ready) throw new AccountRequestError('ACCOUNT_API_UNAVAILABLE');
  const controller = new AbortController(), abort = () => controller.abort();
  options.signal?.addEventListener('abort', abort, {once: true});
  const timeout = setTimeout(abort, options.timeoutMs ?? 20000);
  try {
    const multipart = body instanceof FormData;
    const response = await fetch(`/api/${path}`, {
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
