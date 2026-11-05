import { accountsEnabled } from '../release-features';

const backup = 'https://cn-site.touhouonline.com:8443';
let chosen = '';
const transientStatuses = new Set([502, 503, 504]);

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

/** Writes are never replayed after dispatch; only idempotent reads can fail over on an uncertain response. */
export async function accountRequest<T = unknown>(path: string, body?: Record<string, unknown> | FormData, options: {signal?:AbortSignal;responseType?:'json'|'blob';timeoutMs?:number} = {}): Promise<T> {
  if (!accountsEnabled) throw new AccountRequestError('ACCOUNT_FEATURE_DISABLED');
  const segments = path.split('/');
  if (
    !/^[a-z0-9._/-]+$/i.test(path) ||
    path.startsWith('/') ||
    segments.some((segment) => !segment || segment === '.' || segment === '..')
  )
    throw new AccountRequestError('ACCOUNT_REQUEST_FAILED');
  const entries = location.origin === 'https://touhouonline.com' ? [...new Set([chosen, '', backup])] : [''];
  const hasBody = body !== undefined;
  let lastReadError: unknown;
  for (const entry of entries) {
    if(options.signal?.aborted)throw new DOMException('Request cancelled','AbortError');
    try {
      const probe = await fetch(`${entry}/api/ready`, {
        credentials: 'include',
        cache: 'no-store',
        signal: AbortSignal.timeout(3500),
      });
      if (!probe.ok || !(await probe.json()).ready) continue;
    } catch {
      continue;
    }
    const controller=new AbortController(),abort=()=>controller.abort();
    options.signal?.addEventListener('abort',abort,{once:true});
    if(options.signal?.aborted)controller.abort();
    const timeout=setTimeout(()=>controller.abort(),options.timeoutMs??20000);
    try {
      const multipart=body instanceof FormData;
      const response = await fetch(`${entry}/api/${path}`, {
        credentials: 'include',
        cache: 'no-store',
        method: hasBody ? 'POST' : 'GET',
        headers: hasBody && !multipart ? { 'Content-Type': 'application/json' } : undefined,
        body: hasBody ? multipart ? body : JSON.stringify(body) : undefined,
        signal: controller.signal,
      });
      if (!hasBody && transientStatuses.has(response.status)) {
        lastReadError = new AccountRequestError('ACCOUNT_API_UNAVAILABLE', response.status);
        continue;
      }
      if(response.ok && options.responseType==='blob') {const blob=await response.blob();chosen=entry;return blob as T;}
      let data: unknown;
      try {
        data = await response.json();
      } catch {
        if (!response.ok) {
          chosen = entry;
          throw new AccountRequestError('ACCOUNT_REQUEST_FAILED', response.status);
        }
        if (!hasBody) {
          lastReadError = new AccountRequestError('ACCOUNT_RESPONSE_INVALID', response.status);
          continue;
        }
        throw new AccountRequestError('ACCOUNT_RESPONSE_INVALID', response.status);
      }
      chosen = entry;
      if (!response.ok) throw new AccountRequestError(accountErrorCode(data), response.status);
      return data as T;
    } catch (cause) {
      if(options.signal?.aborted)throw new DOMException('Request cancelled','AbortError');
      if (cause instanceof AccountRequestError) throw cause;
      if (hasBody) throw new AccountRequestError('ACCOUNT_REQUEST_FAILED');
      lastReadError = cause;
    } finally {clearTimeout(timeout);options.signal?.removeEventListener('abort',abort);}
  }
  if (lastReadError instanceof AccountRequestError) throw lastReadError;
  throw new AccountRequestError('ACCOUNT_API_UNAVAILABLE');
}
