import assert from "node:assert/strict";
import { spawnSync } from "node:child_process";
import { resolve } from "node:path";
import { fileURLToPath } from "node:url";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const probes = [
  {
    command: process.execPath,
    args: [resolve(project, "tools/maintainer/verify-public-relay-fallback.mjs")],
    usage: "verify-public-relay-fallback.mjs <relay-ws-url>",
  },
  {
    command: process.execPath,
    args: [resolve(project, "tools/maintainer/verify-public-targeted-relay.mjs")],
    usage: "verify-public-targeted-relay.mjs <relay-ws-url>",
  },
  {
    command: "python",
    args: ["-S", resolve(project, "tools/maintainer/verify-public-turn-quality.py")],
    usage: "verify-public-turn-quality.py <launcher-url>",
  },
];

for (const probe of probes) {
  const result = spawnSync(probe.command, probe.args, {
    cwd: project,
    encoding: "utf8",
    timeout: 3000,
    windowsHide: true,
    env: {
      ...process.env,
      HTTP_PROXY: "http://127.0.0.1:1",
      HTTPS_PROXY: "http://127.0.0.1:1",
      ALL_PROXY: "http://127.0.0.1:1",
      NO_PROXY: "",
      PYTHONDONTWRITEBYTECODE: "1",
    },
  });
  assert.equal(result.error, undefined, `${probe.usage}: probe did not fail promptly: ${result.error?.message}`);
  assert.notEqual(result.status, 0, `${probe.usage}: missing target must be rejected`);
  const escapedUsage = probe.usage.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
  assert.match(`${result.stdout}\n${result.stderr}`, new RegExp(escapedUsage),
    `${probe.usage}: failure must explain the required target`);
}

console.log(JSON.stringify({
  remoteTargetPolicy: "PASS",
  behavior: "missing-target-rejected-before-network",
  explicitEntrypoints: probes.length,
}));
