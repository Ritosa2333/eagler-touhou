import { existsSync, readFileSync } from "node:fs";
import { dirname, resolve } from "node:path";

export const WORKSPACE_LAYOUT_SCHEMA = "eagler-touhou/workspace-layout/1";

function readJson(path) {
  try { return JSON.parse(readFileSync(path, "utf8")); }
  catch (error) { if (error.code === "ENOENT") return null; throw error; }
}

// A containing workspace must explicitly own this Web directory. A neighboring
// checkout or an unrelated package.json must never change its source authority.
export function sourceProjectRoot(webRoot) {
  const web = resolve(webRoot);
  for (let candidate = dirname(web); candidate !== dirname(candidate); candidate = dirname(candidate)) {
    const config = readJson(resolve(candidate, "config/workspace.json"));
    if (config?.schema === WORKSPACE_LAYOUT_SCHEMA && typeof config.repositories?.launcher === "string" &&
        resolve(candidate, config.repositories.launcher) === web) return candidate;
  }
  return web;
}

export function projectConfigurationPath(name, webRoot) {
  if (!/^[a-z][a-z0-9-]*\.json$/i.test(name)) throw new Error(`invalid project configuration name: ${name}`);
  return resolve(sourceProjectRoot(webRoot), "config", name);
}

export function resolveWorkspaceRoot(webRoot, environment = process.env) {
  if (environment.EAGLER_WORKSPACE_ROOT) return resolve(environment.EAGLER_WORKSPACE_ROOT);
  const project = sourceProjectRoot(webRoot);
  if (project !== resolve(webRoot)) return project;
  for (let candidate = dirname(project); candidate !== dirname(candidate); candidate = dirname(candidate)) {
    if (existsSync(resolve(candidate, "WORKSPACE.md"))) return candidate;
  }
  return dirname(project);
}

export function validateWorkspaceConfiguration(config) {
  if (config?.schema !== WORKSPACE_LAYOUT_SCHEMA || !config.repositories ||
      typeof config.repositories !== "object" || Array.isArray(config.repositories)) {
    throw new Error("invalid config/workspace.json");
  }
  for (const [key, path] of Object.entries(config.repositories)) {
    if (!/^[a-zA-Z0-9_-]+$/.test(key) || typeof path !== "string" || !path || /[\\:\0]/.test(path) ||
        path.split("/").some(part => !part || part === "." || part === "..")) {
      throw new Error(`invalid workspace repository path: ${key}`);
    }
  }
  return config;
}

export function loadSourceLayout(webRoot, { environment = process.env } = {}) {
  const web = resolve(webRoot), project = sourceProjectRoot(web);
  const config = validateWorkspaceConfiguration(readJson(projectConfigurationPath("workspace.json", web)));
  const root = resolveWorkspaceRoot(web, environment);
  const repositories = Object.freeze({ ...config.repositories });
  return Object.freeze({ projectRoot: project, webRoot: web, root, repositories,
    path(repository, ...segments) {
      if (repository === "launcher") return resolve(web, ...segments);
      if (!Object.hasOwn(repositories, repository)) throw new Error(`unknown workspace repository: ${repository}`);
      return resolve(root, repositories[repository], ...segments);
    },
  });
}
