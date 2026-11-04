import {afterEach, expect, it, vi} from 'vitest';

const backup = 'https://cn-site.touhouonline.com:8443';

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

it('uses the backup for writes when the primary fails its readiness probe', async () => {
  const fetcher = vi.fn(async input => {
    const url = String(input);
    if (url === '/api/ready') throw new Error('primary unavailable');
    if (url === `${backup}/api/ready`) return Response.json({ready: true});
    if (url === `${backup}/api/auth/sign-in/email`) return Response.json({user: {id: 'user-1'}});
    throw new Error(`Unexpected request: ${url}`);
  });
  const {accountRequest} = await loadAccountClient(fetcher);

  await expect(accountRequest('auth/sign-in/email', {email: 'player@example.com', password: 'private-pass'}))
    .resolves.toEqual({user: {id: 'user-1'}});
  expect(fetcher.mock.calls.map(([url]) => String(url))).toEqual([
    '/api/ready', `${backup}/api/ready`, `${backup}/api/auth/sign-in/email`,
  ]);
});

it('retries a GET on the backup after an uncertain primary response', async () => {
  const fetcher = vi.fn(async input => {
    const url = String(input);
    if (url === '/api/ready') return Response.json({ready: true});
    if (url === '/api/v1/profiles/reimu') return new Response(null, {status: 503});
    if (url === `${backup}/api/ready`) return Response.json({ready: true});
    if (url === `${backup}/api/v1/profiles/reimu`) return Response.json({username: 'reimu'});
    throw new Error(`Unexpected request: ${url}`);
  });
  const {accountRequest} = await loadAccountClient(fetcher);

  await expect(accountRequest('v1/profiles/reimu')).resolves.toEqual({username: 'reimu'});
  expect(fetcher.mock.calls.map(([url]) => String(url))).toEqual([
    '/api/ready', '/api/v1/profiles/reimu', `${backup}/api/ready`, `${backup}/api/v1/profiles/reimu`,
  ]);
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
