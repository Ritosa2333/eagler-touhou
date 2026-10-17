/** L3/security policy. Pure path/host inputs only. Proves that local workspace
 * convenience cannot become an accidental LAN file server. */
import assert from "node:assert/strict";
import { resolve } from "node:path";

import { assertSafeDevelopmentServerScope } from "../lib/development-server-scope.mjs";

const project = resolve("D:/synthetic-workspace/eagler-touhou");
const workspace = resolve(project, "..");

assert.doesNotThrow(() => assertSafeDevelopmentServerScope({ host: "127.0.0.1", project, root: workspace }));
assert.doesNotThrow(() => assertSafeDevelopmentServerScope({ host: "::1", project, root: workspace }));
assert.doesNotThrow(() => assertSafeDevelopmentServerScope({ host: "0.0.0.0", project, root: project }));
assert.throws(
  () => assertSafeDevelopmentServerScope({ host: "0.0.0.0", project, root: workspace }),
  /refusing to expose the workspace root/,
);

console.log(JSON.stringify({ developmentServerScope: "PASS", nonLoopbackWorkspace: "rejected" }));
