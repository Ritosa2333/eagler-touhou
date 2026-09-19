const { mkdtempSync, readFileSync, rmSync } = require('node:fs');
const { spawnSync } = require('node:child_process');
const { join } = require('node:path');
const { requireHostFile } = require('./integration-support.cjs');
const { tmpdir } = require('node:os');

const renderer = requireHostFile('server/render-coturn-config.cjs', 'coturn config renderer');

function run(extraEnv = {}) {
  const dir = mkdtempSync(join(tmpdir(), 'th07-coturn-'));
  const output = join(dir, 'turnserver.conf');
  const result = spawnSync(process.execPath, [renderer, '--output', output], {
    env: {
      ...process.env,
      TH07_TURN_REALM: 'turn.example.test',
      TH07_TURN_SHARED_SECRET: '0123456789abcdef0123456789abcdef',
      ...extraEnv,
    },
    encoding: 'utf8',
  });
  const text = result.status === 0 ? readFileSync(output, 'utf8') : '';
  rmSync(dir, { recursive: true, force: true });
  return { ...result, text };
}

function requireLine(text, line) {
  if (!text.split(/\r?\n/).includes(line)) throw new Error(`missing coturn config line: ${line}`);
}

const basic = run();
if (basic.status !== 0) throw new Error(basic.stderr || basic.stdout || 'basic coturn render failed');
for (const line of [
  'fingerprint',
  'use-auth-secret',
  'static-auth-secret=0123456789abcdef0123456789abcdef',
  'realm=turn.example.test',
  'no-cli',
  'no-multicast-peers',
  'no-dtls',
  'no-tls',
  'user-quota=12',
  'total-quota=256',
  'max-bps=131072',
  'bps-capacity=786432',
  'denied-peer-ip=10.0.0.0-10.255.255.255',
  'denied-peer-ip=192.168.0.0-192.168.255.255',
]) requireLine(basic.text, line);

const tls = run({
  TH07_TURN_CERT: '/etc/ssl/turn/fullchain.pem',
  TH07_TURN_PKEY: '/etc/ssl/turn/privkey.pem',
});
if (tls.status !== 0) throw new Error(tls.stderr || tls.stdout || 'TLS coturn render failed');
requireLine(tls.text, 'tls-listening-port=5349');
requireLine(tls.text, 'cert=/etc/ssl/turn/fullchain.pem');
requireLine(tls.text, 'pkey=/etc/ssl/turn/privkey.pem');
if (tls.text.split(/\r?\n/).includes('no-tls')) throw new Error('TLS config must not disable TLS');

const invalidPorts = run({ TH07_TURN_MIN_PORT: '60000', TH07_TURN_MAX_PORT: '50000' });
if (invalidPorts.status === 0) throw new Error('invalid relay port range must fail');

const weakSecret = run({ TH07_TURN_SHARED_SECRET: 'short' });
if (weakSecret.status === 0) throw new Error('weak TURN secret must fail');

console.log('TH07 coturn config contract: PASS');
