import assert from "node:assert/strict";
import { mkdtempSync, readFileSync, rmSync } from "node:fs";
import { tmpdir } from "node:os";
import { join, resolve } from "node:path";
import { spawnSync } from "node:child_process";

const root = resolve(import.meta.dirname, "..");
const renderer = resolve(root, "server", "render-coturn-config.cjs");
const temporary = mkdtempSync(join(tmpdir(), "eagler-netplay-config-"));

function cleanEnvironment() {
  const env = { ...process.env };
  for (const key of Object.keys(env)) {
    if (key.startsWith("EAGLER_NETPLAY_") || key.startsWith("TH07_TURN_")) delete env[key];
  }
  return env;
}

function render(name, variables) {
  const output = join(temporary, `${name}.conf`);
  const result = spawnSync(process.execPath, [renderer, "--output", output], {
    cwd: root,
    env: { ...cleanEnvironment(), ...variables },
    encoding: "utf8",
  });
  assert.equal(result.status, 0, result.stderr || result.stdout);
  return readFileSync(output, "utf8");
}

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

  const conflictOutput = join(temporary, "conflict.conf");
  const conflict = spawnSync(process.execPath, [renderer, "--output", conflictOutput], {
    cwd: root,
    env: {
      ...cleanEnvironment(),
      EAGLER_NETPLAY_TURN_REALM: "new.example.test",
      TH07_TURN_REALM: "old.example.test",
      EAGLER_NETPLAY_TURN_SHARED_SECRET: "0123456789abcdef0123456789abcdef",
    },
    encoding: "utf8",
  });
  assert.notEqual(conflict.status, 0, "conflicting canonical/legacy variables must fail closed");
  assert.match(`${conflict.stderr}\n${conflict.stdout}`, /conflicting environment variables/);
} finally {
  rmSync(temporary, { recursive: true, force: true });
}

console.log(JSON.stringify({
  netplayServiceConfig: "PASS",
  canonicalNamespace: "EAGLER_NETPLAY_*",
  legacyNamespace: "TH07_* compatibility-only",
}));
