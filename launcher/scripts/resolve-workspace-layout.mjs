import { resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { loadSourceLayout } from "../lib/source-layout.mjs";

const args = process.argv.slice(2);
if (args.length > 1) throw new Error("usage: node scripts/resolve-workspace-layout.mjs [WEB_ROOT]");
const webRoot = resolve(args[0] || fileURLToPath(new URL("..", import.meta.url)));
const layout = loadSourceLayout(webRoot);
console.log(JSON.stringify({ projectRoot: layout.projectRoot, webRoot: layout.webRoot,
  root: layout.root, repositories: layout.repositories }));
