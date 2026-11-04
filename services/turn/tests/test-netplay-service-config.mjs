import assert from "node:assert/strict";
import { mkdtempSync, readFileSync, rmSync, statSync } from "node:fs";
import { tmpdir } from "node:os";
import { join, resolve } from "node:path";
import { spawnSync } from "node:child_process";

const root = resolve(import.meta.dirname, "..");
const renderer = resolve(root, "render-coturn-config.cjs");
const temporary = mkdtempSync(join(tmpdir(), "eagler-netplay-config-"));

function cleanEnvironment() {
  const env = { ...process.env };
  for (const key of Object.keys(env)) {
    if (key.startsWith("EAGLER_NETPLAY_") || key.startsWith("TH07_TURN_")) delete env[key];
  }
  return env;
}

function invoke(name, variables) {
  const output = join(temporary, `${name}.conf`);
  const result = spawnSync(process.execPath, [renderer, "--output", output], {
    cwd: root,
    env: { ...cleanEnvironment(), ...variables },
    encoding: "utf8",
  });
  return { output, result };
}

function render(name, variables) {
  const {output, result} = invoke(name, variables);
  assert.equal(result.status, 0, result.stderr || result.stdout);
  return readFileSync(output, "utf8");
}

function reject(name, variables, expected) {
  const {result} = invoke(name, variables);
  assert.notEqual(result.status, 0, `${name} must fail closed`);
  if (expected) assert.match(`${result.stderr}\n${result.stdout}`, expected);
}

const credentials = {
  EAGLER_NETPLAY_TURN_REALM: "turn.example.test",
  EAGLER_NETPLAY_TURN_SHARED_SECRET: "0123456789abcdef0123456789abcdef",
};

try {
  const generic = render("generic", {
    EAGLER_NETPLAY_TURN_REALM: "turn.example.test",
    EAGLER_NETPLAY_TURN_SHARED_SECRET: "0123456789abcdef0123456789abcdef",
    EAGLER_NETPLAY_TURN_MIN_PORT: "50000",
    EAGLER_NETPLAY_TURN_MAX_PORT: "50010",
  });
  const legacy = render("legacy", {
    TH07_TURN_REALM: "turn.example.test",
    TH07_TURN_SHARED_SECRET: "0123456789abcdef0123456789abcdef",
    TH07_TURN_MIN_PORT: "50000",
    TH07_TURN_MAX_PORT: "50010",
  });
  assert.equal(legacy, generic,
    "legacy TH07_* TURN variables must be aliases of the canonical EAGLER_NETPLAY_* service contract");
  assert.match(generic, /realm=turn\.example\.test/);
  assert.match(generic, /min-port=50000/);
  assert.match(generic, /max-port=50010/);
  assert.match(generic, /no-tls\n/);
  if (process.platform !== "win32")
    assert.equal(statSync(join(temporary, "generic.conf")).mode & 0o777, 0o600);

  reject("conflict", {...credentials, EAGLER_NETPLAY_TURN_REALM: "new.example.test",
    TH07_TURN_REALM: "old.example.test"}, /conflicting environment variables/);

  const tls = render("tls", {...credentials, EAGLER_NETPLAY_TURN_CERT: "/etc/coturn/cert.pem",
    EAGLER_NETPLAY_TURN_PKEY: "/etc/coturn/key.pem"});
  assert.match(tls, /tls-listening-port=5349/);
  assert.match(tls, /cert=\/etc\/coturn\/cert\.pem/);
  assert.match(tls, /pkey=\/etc\/coturn\/key\.pem/);
  assert.doesNotMatch(tls, /\nno-tls\n/);
  reject("cert-only", {...credentials, EAGLER_NETPLAY_TURN_CERT: "/etc/coturn/cert.pem"},
    /CERT and EAGLER_NETPLAY_TURN_PKEY must be configured together/);
  reject("key-only", {...credentials, EAGLER_NETPLAY_TURN_PKEY: "/etc/coturn/key.pem"},
    /CERT and EAGLER_NETPLAY_TURN_PKEY must be configured together/);
  reject("short-secret", {...credentials, EAGLER_NETPLAY_TURN_SHARED_SECRET: "x".repeat(23)},
    /must be at least 24 bytes/);
  render("minimum-secret", {...credentials, EAGLER_NETPLAY_TURN_SHARED_SECRET: "x".repeat(24)});

  const boundaries = render("boundaries", {...credentials,
    EAGLER_NETPLAY_TURN_CERT: "/etc/coturn/cert.pem", EAGLER_NETPLAY_TURN_PKEY: "/etc/coturn/key.pem",
    EAGLER_NETPLAY_TURN_LISTENING_PORT: "1", EAGLER_NETPLAY_TURN_TLS_LISTENING_PORT: "65535",
    EAGLER_NETPLAY_TURN_MIN_PORT: "1024", EAGLER_NETPLAY_TURN_MAX_PORT: "65535",
    EAGLER_NETPLAY_TURN_USER_QUOTA: "1", EAGLER_NETPLAY_TURN_TOTAL_QUOTA: "1000000",
    EAGLER_NETPLAY_TURN_MAX_BPS: "1024", EAGLER_NETPLAY_TURN_BPS_CAPACITY: "1000000000"});
  for (const setting of ["listening-port=1", "tls-listening-port=65535", "min-port=1024", "max-port=65535",
    "user-quota=1", "total-quota=1000000", "max-bps=1024", "bps-capacity=1000000000"])
    assert.ok(boundaries.includes(setting), `valid numeric boundary ${setting} is rendered`);

  const invalid = [
    ["EAGLER_NETPLAY_TURN_LISTENING_PORT", "0"], ["EAGLER_NETPLAY_TURN_LISTENING_PORT", "65536"],
    ["EAGLER_NETPLAY_TURN_TLS_LISTENING_PORT", "0"], ["EAGLER_NETPLAY_TURN_TLS_LISTENING_PORT", "65536"],
    ["EAGLER_NETPLAY_TURN_MIN_PORT", "1023"], ["EAGLER_NETPLAY_TURN_MIN_PORT", "65536"],
    ["EAGLER_NETPLAY_TURN_MAX_PORT", "1023"], ["EAGLER_NETPLAY_TURN_MAX_PORT", "65536"],
    ["EAGLER_NETPLAY_TURN_USER_QUOTA", "0"], ["EAGLER_NETPLAY_TURN_USER_QUOTA", "10001"],
    ["EAGLER_NETPLAY_TURN_TOTAL_QUOTA", "0"], ["EAGLER_NETPLAY_TURN_TOTAL_QUOTA", "1000001"],
    ["EAGLER_NETPLAY_TURN_MAX_BPS", "1023"], ["EAGLER_NETPLAY_TURN_MAX_BPS", "1000000001"],
    ["EAGLER_NETPLAY_TURN_BPS_CAPACITY", "1023"], ["EAGLER_NETPLAY_TURN_BPS_CAPACITY", "1000000001"],
    ["EAGLER_NETPLAY_TURN_LISTENING_PORT", "3478junk"], ["EAGLER_NETPLAY_TURN_LISTENING_PORT", "1.5"],
  ];
  for (const [name, value] of invalid)
    reject(`invalid-${name.toLowerCase().replaceAll(/[^a-z0-9]+/g, "-")}-${value.replaceAll(/[^a-z0-9]+/gi, "-")}`,
      {...credentials, [name]: value}, new RegExp(`${name} must be an integer`));
  reject("reversed-port-range", {...credentials, EAGLER_NETPLAY_TURN_MIN_PORT: "50010",
    EAGLER_NETPLAY_TURN_MAX_PORT: "50000"}, /MIN_PORT must not exceed EAGLER_NETPLAY_TURN_MAX_PORT/);
} finally {
  rmSync(temporary, { recursive: true, force: true });
}

console.log(JSON.stringify({
  netplayServiceConfig: "PASS",
  canonicalNamespace: "EAGLER_NETPLAY_*",
  legacyNamespace: "TH07_* compatibility-only",
}));
