import { readdir } from "node:fs/promises";
import { extname, relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { mkdir, writeFile } from "node:fs/promises";
import { runTasks } from "../../tools/testing/runner.mjs";
import { availableParallelism } from "node:os";
import {
  REPOSITORY_NODE_TESTS,
  repositoryNodeTestResources,
  REPOSITORY_PYTHON_TESTS,
  WORKSPACE_NODE_TESTS,
  WORKSPACE_PRECHECKS,
  WORKSPACE_PYTHON_TESTS,
} from "../tests/test-plan.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const arguments_ = process.argv.slice(2);
const workspaceMode = arguments_.includes("--workspace");
const lane = arguments_.find(arg => arg.startsWith("--lane="))?.slice(7) ?? "repository";
if (!["repository", "workspace"].includes(lane) || arguments_.some(arg =>
  arg !== "--workspace" && !/^--(?:lane|output)=.+$/.test(arg)))
  throw new Error("usage: node scripts/check.mjs [--workspace|--lane=repository|workspace] [--output=DIR]");
const startedAt = Date.now();
const jobs = Number(process.env.EAGLER_CHECK_JOBS ?? Math.max(1, Math.min(4, availableParallelism())));
if (!Number.isSafeInteger(jobs) || jobs < 1 || jobs > 32) throw new Error("EAGLER_CHECK_JOBS must be 1..32");
const output = resolve(arguments_.find(arg=>arg.startsWith("--output="))?.slice(9) ??
  resolve(project, "../artifacts/validation/tests/launcher-core"));
const reports = [];
const controller = new AbortController();
const cancel = () => controller.abort();
process.once("SIGINT", cancel); process.once("SIGTERM", cancel);
function task(command, args, index=0, resources=[]) {
  return {id:`${index}-${args.join("_").replace(/[^a-zA-Z0-9._-]/g,"_").slice(-150)}`,
    command, args, cwd:project, timeoutMs:180000, resources};
}
async function batch(name, tasks, concurrency=jobs) {
  const report = await runTasks(tasks, {output:resolve(output, name), concurrency,
    signal:controller.signal, metadata:{phase:name}});
  reports.push({phase:name, ...report});
  const failures = report.results.filter(result=>result.status !== "passed");
  console.log(`${name}: ${report.status} (${report.results.length} tasks, ${(report.durationMs/1000).toFixed(1)}s)`);
  for (const failure of failures) console.error(`${failure.id}: ${failure.status}; ${failure.log}`);
  if (report.status !== "passed") throw new Error(`${name} failed; see ${resolve(output, name, "report.json")}`);
}
const run = (command, args) => batch(`prepare-${reports.length}`, [task(command, args)]);
const runPool = (tasks, concurrency=jobs) => batch(`checks-${reports.length}`,
  tasks.map(([command, args, resources=[]], index)=>task(command, args, index, resources)), concurrency);

const excludedDirectories = new Set([
  ".git", ".cache", ".codex-tmp", ".npm-cache", ".deploy-python", "node_modules", "artifacts", "archive",
  "archivetemporary", "dist", "design", "screenshots", "vendor", "private-assets",
  "generated-assets", "server-output", "__pycache__",
]);

async function collectSourceFiles(directory = project) {
  const result = [];
  for (const entry of await readdir(directory, { withFileTypes: true })) {
    if (entry.isDirectory() && excludedDirectories.has(entry.name)) continue;
    const path = resolve(directory, entry.name);
    const rel = relative(project, path).replaceAll("\\", "/");
    if (entry.isDirectory()) {
      if (rel === "eagler-touhou") continue;
      result.push(...await collectSourceFiles(path));
      continue;
    }
    if ([".js", ".mjs", ".cjs", ".py"].includes(extname(entry.name).toLowerCase())) result.push(rel);
  }
  return result.sort();
}

try {
const sourceFiles = await collectSourceFiles();
const javascriptFiles = sourceFiles.filter(file => [".js", ".mjs", ".cjs"].includes(extname(file)));
const pythonFiles = sourceFiles.filter(file => extname(file) === ".py");

// TypeScript Launcher source is an authoritative source tree. The canonical
// gate always recompiles it so strict type-checking cannot be skipped merely
// because generated output happens to look fresh on disk.
// Check authored generated pages before a build can silently refresh them.
await run(process.execPath, ["scripts/build-content-pages.mjs", "--check"]);
for (const config of ['tsconfig.ui-rewrite.json', 'tsconfig.ui-rewrite.tools.json']) {
  await run(process.execPath, [resolve(project, 'node_modules/typescript/bin/tsc'), '-p', config]);
}
await run(process.execPath, ["scripts/build-launcher.mjs", "--force"]);

if (lane === "repository") {
await runPool(javascriptFiles.map(file => [process.execPath, ["--check", file]]));
if (pythonFiles.length) await run("python", ["scripts/check-python-syntax.py", ...pythonFiles]);

// App Shell generation is build output only. The repository gate exercises the
// builder without writing a generated Service Worker into the source tree.
await run(process.execPath, ["scripts/build-app-shell.mjs", "--check"]);

// Repo-local, deterministic behavior/format gates. These must work in an
// ordinary standalone clone without sibling game repositories or private game
// resources.
await runPool(REPOSITORY_NODE_TESTS.map(test => [process.execPath, [test], repositoryNodeTestResources(test)]), Math.min(jobs, 6));
await runPool(REPOSITORY_PYTHON_TESTS.map(test => ["python", [test]]), Math.min(jobs, 4));
await run(process.execPath, ["scripts/audit-publication.mjs"]);

}

// Cross-repository integration is intentionally explicit. These checks are
// valuable, but they are not reproducible from the Launcher repository alone.
if (workspaceMode || lane === "workspace") {
  for (const [command, args] of WORKSPACE_PRECHECKS) await run(command, [...args]);
  for (const test of WORKSPACE_NODE_TESTS) await run(process.execPath, [test]);
  for (const test of WORKSPACE_PYTHON_TESTS) await run("python", [test]);
}

const elapsedSeconds = ((Date.now() - startedAt) / 1000).toFixed(1);
console.log(`Check: PASS (${workspaceMode || lane === "workspace" ? "workspace integration" : "repository core"}, ${elapsedSeconds}s, jobs=${jobs})`);
} finally {
  process.removeListener("SIGINT", cancel); process.removeListener("SIGTERM", cancel);
  await mkdir(output, {recursive:true});
  await writeFile(resolve(output, "check-report.json"), JSON.stringify({schema:"eagler-touhou/launcher-check/1",
    lane:workspaceMode ? "repository+workspace" : lane, durationMs:Date.now()-startedAt, reports}, null, 2) + "\n");
}
