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
export async function accountRequest<T = unknown>(path: string, body?: Record<string, unknown>): Promise<T> {
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
    try {
      const response = await fetch(`${entry}/api/${path}`, {
        credentials: 'include',
        cache: 'no-store',
        method: hasBody ? 'POST' : 'GET',
        headers: hasBody ? { 'Content-Type': 'application/json' } : undefined,
        body: hasBody ? JSON.stringify(body) : undefined,
        signal: AbortSignal.timeout(20000),
      });
      if (!hasBody && transientStatuses.has(response.status)) {
        lastReadError = new AccountRequestError('ACCOUNT_API_UNAVAILABLE', response.status);
        continue;
      }
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
      if (cause instanceof AccountRequestError) throw cause;
      if (hasBody) throw new AccountRequestError('ACCOUNT_REQUEST_FAILED');
      lastReadError = cause;
    }
  }
  if (lastReadError instanceof AccountRequestError) throw lastReadError;
  throw new AccountRequestError('ACCOUNT_API_UNAVAILABLE');
}
