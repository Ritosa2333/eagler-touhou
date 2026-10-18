import { resolve } from "node:path";

const LOOPBACK_HOSTS = new Set(["127.0.0.1", "::1", "localhost"]);

export function assertSafeDevelopmentServerScope({ host, project, root }) {
  const normalizedHost = String(host || "").trim().toLowerCase();
  const projectRoot = resolve(project);
  const servedRoot = resolve(root);
  if (servedRoot !== projectRoot && !LOOPBACK_HOSTS.has(normalizedHost)) {
    throw new Error("refusing to expose the workspace root on a non-loopback development server");
  }
  return Object.freeze({ host: normalizedHost, project: projectRoot, root: servedRoot });
}
