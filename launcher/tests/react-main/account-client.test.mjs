import {afterEach, expect, it, vi} from 'vitest';

afterEach(() => {
  vi.unstubAllGlobals();
  vi.unstubAllEnvs();
  vi.resetModules();
});

async function loadAccountClient(fetcher) {
  vi.stubEnv('VITE_ACCOUNTS_ENABLED', 'true');
  vi.stubGlobal('location', {origin: 'https://touhouonline.com'});
  vi.stubGlobal('fetch', fetcher);
  return import('../../app/services/account-client.ts');
}

it('keeps writes on the account origin when readiness fails', async () => {
  const fetcher = vi.fn(async () => {throw new TypeError('connection lost');});
  const {accountRequest} = await loadAccountClient(fetcher);
  await expect(accountRequest('auth/sign-in/email', {email: 'player@example.com', password: 'private-pass'}))
    .rejects.toMatchObject({code: 'ACCOUNT_API_UNAVAILABLE'});
  expect(fetcher.mock.calls.map(([url]) => String(url))).toEqual(['/api/ready']);
});

it('reports a transient read failure on the account origin', async () => {
  const fetcher = vi.fn(async input => input === '/api/ready' ? Response.json({ready: true}) : new Response(null, {status: 503}));
  const {accountRequest} = await loadAccountClient(fetcher);
  await expect(accountRequest('v1/profiles/reimu')).rejects.toMatchObject({code: 'ACCOUNT_API_UNAVAILABLE', status: 503});
  expect(fetcher.mock.calls.map(([url]) => String(url))).toEqual(['/api/ready', '/api/v1/profiles/reimu']);
});

it('accepts usernames containing dots and underscores in public-profile paths', async () => {
  const fetcher = vi.fn(async input => {
    const url = String(input);
    if (url === '/api/ready') return Response.json({ready: true});
    if (url === '/api/v1/profiles/reimu.1_test') return Response.json({username: 'reimu.1_test'});
    throw new Error(`Unexpected request: ${url}`);
  });
  const {accountRequest} = await loadAccountClient(fetcher);

  await expect(accountRequest('v1/profiles/reimu.1_test')).resolves.toEqual({username: 'reimu.1_test'});
  expect(fetcher.mock.calls.map(([url]) => String(url))).toEqual(['/api/ready', '/api/v1/profiles/reimu.1_test']);
});

it.each([401, 403, 404])('preserves empty %s responses instead of treating them as an outage', async status => {
  const fetcher = vi.fn(async input => {
    const url = String(input);
    if (url === '/api/ready') return Response.json({ready: true});
    if (url === '/api/v1/profiles/missing_user') return new Response(null, {status});
    throw new Error(`Unexpected request: ${url}`);
  });
  const {accountRequest} = await loadAccountClient(fetcher);

  await expect(accountRequest('v1/profiles/missing_user')).rejects.toMatchObject({
    name: 'AccountRequestError', code: 'ACCOUNT_REQUEST_FAILED', status,
  });
  expect(fetcher.mock.calls.map(([url]) => String(url))).toEqual(['/api/ready', '/api/v1/profiles/missing_user']);
});

it('blocks account requests when production flags disable account features', async () => {
  vi.stubEnv('VITE_PREVIEW_FEATURES', 'true');
  vi.stubEnv('VITE_ACCOUNTS_ENABLED', 'false');
  const fetcher = vi.fn();
  vi.stubGlobal('location', {origin: 'https://touhouonline.com'});
  vi.stubGlobal('fetch', fetcher);
  vi.resetModules();
  const {accountRequest} = await import('../../app/services/account-client.ts');

  await expect(accountRequest('auth/get-session')).rejects.toMatchObject({
    name: 'AccountRequestError', code: 'ACCOUNT_FEATURE_DISABLED',
  });
  expect(fetcher).not.toHaveBeenCalled();
});

it('does not replay a write after dispatch when the connection fails', async () => {
  const fetcher = vi.fn(async input => {
    const url = String(input);
    if (url === '/api/ready') return Response.json({ready: true});
    if (url === '/api/auth/email-otp/send-verification-otp') throw new Error('connection lost');
    throw new Error(`Unexpected request: ${url}`);
  });
  const {accountRequest, AccountRequestError} = await loadAccountClient(fetcher);

  await expect(accountRequest('auth/email-otp/send-verification-otp', {email: 'player@example.com'}))
    .rejects.toMatchObject({name: 'AccountRequestError', code: 'ACCOUNT_REQUEST_FAILED'});
  expect(fetcher.mock.calls.map(([url]) => String(url))).toEqual([
    '/api/ready', '/api/auth/email-otp/send-verification-otp',
  ]);
  expect(AccountRequestError).toBeTypeOf('function');
});

it('does not dispatch an account request when its readiness probe is cancelled', async () => {
  let signal, rejectProbe;
  const fetcher = vi.fn((input, init = {}) => {
    const url = String(input);
    if (url === '/api/ready') {
      signal = init.signal;
      return new Promise((resolve, reject) => {
        rejectProbe = reject;
        signal.addEventListener('abort', () => reject(new DOMException('Aborted', 'AbortError')), {once: true});
      });
    }
    throw new Error(`The account request must not be dispatched: ${url}`);
  });
  const {accountRequest} = await loadAccountClient(fetcher);
  const controller = new AbortController();
  const pending = accountRequest('auth/get-session', undefined, {signal: controller.signal});
  for (let attempt = 0; attempt < 20 && !rejectProbe; attempt++) await new Promise(resolve => setTimeout(resolve, 0));
  expect(signal).toBeDefined();
  controller.abort();
  await expect(pending).rejects.toMatchObject({name: 'AbortError'});
  expect(fetcher.mock.calls.map(([url]) => String(url))).toEqual(['/api/ready']);
});

it('cancels a write during readiness and allows a new account request afterward', async () => {
  let rejectFirstProbe, probes = 0;
  const fetcher = vi.fn((input, init = {}) => {
    const url = String(input);
    if (url === '/api/ready') {
      probes++;
      if (probes === 1) return new Promise((resolve, reject) => {
        rejectFirstProbe = reject;
        init.signal.addEventListener('abort', () => reject(new DOMException('Aborted', 'AbortError')), {once: true});
      });
      return Promise.resolve(Response.json({ready: true}));
    }
    if (url === '/api/auth/get-session') return Promise.resolve(Response.json({user: {username: 'bob'}}));
    throw new Error(`The cancelled write must not be dispatched: ${url}`);
  });
  const {accountRequest, cancelAccountRequests} = await loadAccountClient(fetcher);
  const staleWrite = accountRequest('v1/me/profile', {name: 'Alice draft'});
  for (let attempt = 0; attempt < 20 && !rejectFirstProbe; attempt++) await new Promise(resolve => setTimeout(resolve, 0));
  expect(rejectFirstProbe).toBeTypeOf('function');
  cancelAccountRequests();
  await expect(staleWrite).rejects.toMatchObject({name: 'AbortError'});
  await expect(accountRequest('auth/get-session')).resolves.toEqual({user: {username: 'bob'}});
  expect(fetcher.mock.calls.map(([url]) => String(url))).toEqual([
    '/api/ready', '/api/ready', '/api/auth/get-session',
  ]);
});
