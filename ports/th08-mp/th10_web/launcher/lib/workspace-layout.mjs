import { readFileSync } from "node:fs";
import { dirname, relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";

export const WORKSPACE_LAYOUT_SCHEMA = "eagler-touhou/workspace-layout/1";

const projectRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const config = JSON.parse(readFileSync(new URL("../config/workspace.json", import.meta.url), "utf8"));
if (config.schema !== WORKSPACE_LAYOUT_SCHEMA || !config.repositories || typeof config.repositories !== "object") {
  throw new Error("invalid config/workspace.json");
}

export function launcherRoot() {
  return projectRoot;
}

const requiredRepositories = ["launcher", "th06", "th07", "th08", "thprac", "dependencies", "toolchains"];
for (const key of requiredRepositories) {
  const value = config.repositories[key];
  if (typeof value !== "string" || !value || value.includes("/") || value.includes("\\") || value === "." || value === "..") {
    throw new Error(`invalid workspace repository path: ${key}`);
  }
}

export const WORKSPACE_REPOSITORIES = Object.freeze({ ...config.repositories });

export function workspaceRoot() {
  return resolve(process.env.EAGLER_WORKSPACE_ROOT || resolve(projectRoot, ".."));
}

export function workspacePath(repository, ...segments) {
  if (repository === "launcher") return resolve(projectRoot, ...segments);
  const directory = WORKSPACE_REPOSITORIES[repository];
  if (!directory) throw new Error(`unknown workspace repository: ${repository}`);
  return resolve(workspaceRoot(), directory, ...segments);
}

export function projectRelativeWorkspacePath(repository, ...segments) {
  const path = relative(projectRoot, workspacePath(repository, ...segments)).replaceAll("\\", "/");
  return path.startsWith(".") ? path : `./${path}`;
}

export function projectRelativeWorkspaceDirectory(repository, ...segments) {
  return `${projectRelativeWorkspacePath(repository, ...segments).replace(/\/$/, "")}/`;
}

export function workspaceRepositoryNames(repositories) {
  return repositories.map(repository => WORKSPACE_REPOSITORIES[repository]);
}
