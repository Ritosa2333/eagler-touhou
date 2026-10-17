import assert from "node:assert/strict";
import { mkdtemp, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { browserModuleClosure, localModuleClosure } from "../lib/browser-module-graph.mjs";

const root = await mkdtemp(join(tmpdir(), "eagler-browser-graph-"));
try {
  await writeFile(join(root, "entry.js"), [
    'import "./feature.mjs";',
    'export { value } from "./shared.js";',
    'await import("./lazy.js");',
  ].join("\n"));
  await writeFile(join(root, "feature.mjs"), 'import "./shared.js";\n');
  await writeFile(join(root, "shared.js"), 'export const value = 1;\n');
  await writeFile(join(root, "lazy.js"), 'export default 1;\n');

  assert.deepEqual(await browserModuleClosure({ root, entries: ["entry.js"] }), [
    "entry.js",
    "feature.mjs",
    "lazy.js",
    "shared.js",
  ]);

  await writeFile(join(root, "bad-dynamic.js"), 'const name = "lazy.js"; import(`./${name}`);\n');
  await assert.rejects(
    () => browserModuleClosure({ root, entries: ["bad-dynamic.js"] }),
    /dynamic import must use a static string specifier/,
  );
  assert.deepEqual(await localModuleClosure({
    root,
    entries: ["bad-dynamic.js"],
    allowDynamicImports: true,
  }), ["bad-dynamic.js"]);

  await writeFile(join(root, "bad-bare.js"), 'import "some-package";\n');
  await assert.rejects(
    () => browserModuleClosure({ root, entries: ["bad-bare.js"] }),
    /browser modules must use local relative imports/,
  );
} finally {
  await rm(root, { recursive: true, force: true });
}

console.log(JSON.stringify({ browserModuleGraph: "PASS" }));
