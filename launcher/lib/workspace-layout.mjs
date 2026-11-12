import { dirname, relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { DEFAULT_BUILD_GAMES, normalizeProductSelection } from "./product-selection.mjs";
import { loadSourceLayout, resolveWorkspaceRoot } from "./source-layout.mjs";
export { WORKSPACE_LAYOUT_SCHEMA } from "./source-layout.mjs";

const projectRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const layout = loadSourceLayout(projectRoot);

export function launcherRoot() {
  return projectRoot;
}

const requiredGames = process.env.EAGLER_DEVELOPMENT_GAMES
  ? normalizeProductSelection(process.env.EAGLER_DEVELOPMENT_GAMES) : DEFAULT_BUILD_GAMES;
const requiredRepositories = ["launcher", ...requiredGames, "thprac", "dependencies", "toolchains"];
for (const key of requiredRepositories) {
  if (!layout.repositories[key]) throw new Error(`missing workspace repository: ${key}`);
}

export const WORKSPACE_REPOSITORIES = layout.repositories;

export function workspaceRoot() {
  return resolveWorkspaceRoot(projectRoot);
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
