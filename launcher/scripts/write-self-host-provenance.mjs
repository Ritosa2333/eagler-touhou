import { writeFile } from "node:fs/promises";
import { resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { sourceIdentity } from "../lib/release-manifest.mjs";
import { WORKSPACE_REPOSITORIES } from "../lib/workspace-layout.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const outputArg = process.argv.find(value => value.startsWith("--output="));
if (!outputArg) throw new Error("usage: node scripts/write-self-host-provenance.mjs --output=PATH");
const output = resolve(outputArg.slice("--output=".length));

const manifest = {
  schema: "eagler-touhou/self-host-bundle-provenance/1",
  launcherRepository: WORKSPACE_REPOSITORIES.launcher,
  launcherSource: await sourceIdentity(project),
};

await writeFile(output, `${JSON.stringify(manifest, null, 2)}\n`, "utf8");
console.log(JSON.stringify({ output, repository: manifest.launcherRepository, revision: manifest.launcherSource.revision }));
