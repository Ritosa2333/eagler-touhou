import { readdir } from "node:fs/promises";
import { extname, relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { spawn } from "node:child_process";
import { availableParallelism } from "node:os";
import {
  REPOSITORY_NODE_TESTS,
  REPOSITORY_PYTHON_TESTS,
  WORKSPACE_NODE_TESTS,
  WORKSPACE_PRECHECKS,
  WORKSPACE_PYTHON_TESTS,
} from "../tests/test-plan.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const workspaceMode = process.argv.includes("--workspace");
const unknownArgs = process.argv.slice(2).filter(arg => arg !== "--workspace");
if (unknownArgs.length) throw new Error("usage: node scripts/check.mjs [--workspace]");
const startedAt = Date.now();
const requestedJobs = Number(process.env.EAGLER_CHECK_JOBS || 0);
const jobs = Number.isInteger(requestedJobs) && requestedJobs > 0
  ? requestedJobs
  : Math.max(1, Math.min(8, availableParallelism()));

function run(command, args) {
  return new Promise((resolvePromise, reject) => {
    const child = spawn(command, args, {
      cwd: project,
      stdio: "inherit",
      shell: false,
      env: { ...process.env, PYTHONDONTWRITEBYTECODE: "1" },
    });
    child.once("error", reject);
    child.once("exit", (code, signal) => {
      if (code === 0) return resolvePromise();
      reject(new Error(`${command} ${args.join(" ")} failed${signal ? ` (${signal})` : ""} with exit code ${code ?? "unknown"}`));
    });
  });
}

async function runPool(tasks, concurrency = jobs) {
  let next = 0;
  async function worker() {
    while (true) {
      const index = next++;
      if (index >= tasks.length) return;
      const [command, args] = tasks[index];
      await run(command, args);
    }
  }
  await Promise.all(Array.from({ length: Math.min(concurrency, tasks.length) }, () => worker()));
}

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

const sourceFiles = await collectSourceFiles();
const javascriptFiles = sourceFiles.filter(file => [".js", ".mjs", ".cjs"].includes(extname(file)));
const pythonFiles = sourceFiles.filter(file => extname(file) === ".py");

// TypeScript Launcher source is an authoritative source tree. The canonical
// gate always recompiles it so strict type-checking cannot be skipped merely
// because generated output happens to look fresh on disk.
await run(process.execPath, ["scripts/build-launcher.mjs", "--force"]);
await run(process.execPath, ["scripts/build-content-pages.mjs", "--check"]);

await runPool(javascriptFiles.map(file => [process.execPath, ["--check", file]]));
if (pythonFiles.length) await run("python", ["scripts/check-python-syntax.py", ...pythonFiles]);

// App Shell generation is build output only. The repository gate exercises the
// builder without writing a generated Service Worker into the source tree.
await run(process.execPath, ["scripts/build-app-shell.mjs", "--check"]);

// Repo-local, deterministic behavior/format gates. These must work in an
// ordinary standalone clone without sibling game repositories or private game
// resources.
await runPool(REPOSITORY_NODE_TESTS.map(test => [process.execPath, [test]]), Math.min(jobs, 6));
await runPool(REPOSITORY_PYTHON_TESTS.map(test => ["python", [test]]), Math.min(jobs, 4));
await run(process.execPath, ["scripts/audit-publication.mjs"]);

// Cross-repository integration is intentionally explicit. These checks are
// valuable, but they are not reproducible from the Launcher repository alone.
if (workspaceMode) {
  for (const [command, args] of WORKSPACE_PRECHECKS) await run(command, [...args]);
  for (const test of WORKSPACE_NODE_TESTS) await run(process.execPath, [test]);
  for (const test of WORKSPACE_PYTHON_TESTS) await run("python", [test]);
}

const elapsedSeconds = ((Date.now() - startedAt) / 1000).toFixed(1);
console.log(`Check: PASS (${workspaceMode ? "workspace integration" : "repository core"}, ${elapsedSeconds}s, jobs=${jobs})`);
