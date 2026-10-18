import { readFile } from "node:fs/promises";
import { posix, relative, resolve, sep } from "node:path";
import { parse } from "acorn";

function normalizeRelativePath(value) {
  const normalized = String(value || "").replaceAll("\\", "/");
  if (!normalized || normalized.startsWith("/") || normalized.split("/").some(part => !part || part === "." || part === "..")) {
    throw new Error(`invalid browser module path: ${value}`);
  }
  return normalized;
}

function isWithinRoot(root, candidate) {
  const rel = relative(root, candidate);
  return rel === "" || (!rel.startsWith(`..${sep}`) && rel !== "..");
}

function moduleSpecifiers(source, path, { allowDynamicImports = false } = {}) {
  const ast = parse(source, {
    ecmaVersion: "latest",
    sourceType: "module",
    allowHashBang: true,
  });
  const specifiers = [];

  function addLiteral(node, kind) {
    if (!node) return;
    if (node.type !== "Literal" || typeof node.value !== "string") {
      if (allowDynamicImports && kind === "dynamic import") return;
      throw new Error(`${path}: ${kind} must use a static string specifier so offline closure is deterministic`);
    }
    specifiers.push(node.value);
  }

  function visit(node) {
    if (!node || typeof node !== "object") return;
    if (node.type === "ImportDeclaration" || node.type === "ExportAllDeclaration" || node.type === "ExportNamedDeclaration") {
      if (node.source) addLiteral(node.source, node.type);
    } else if (node.type === "ImportExpression") {
      addLiteral(node.source, "dynamic import");
    }
    for (const [key, value] of Object.entries(node)) {
      if (key === "start" || key === "end" || key === "loc" || key === "source") continue;
      if (Array.isArray(value)) value.forEach(visit);
      else if (value && typeof value === "object") visit(value);
    }
  }

  visit(ast);
  return specifiers;
}

function resolveLocalSpecifier(importerPath, specifier) {
  if (!specifier.startsWith(".")) {
    throw new Error(`${importerPath}: browser modules must use local relative imports; found ${JSON.stringify(specifier)}`);
  }
  if (specifier.includes("?") || specifier.includes("#")) {
    throw new Error(`${importerPath}: query/hash module imports are not supported by the offline closure: ${specifier}`);
  }
  const candidate = posix.normalize(posix.join(posix.dirname(importerPath), specifier));
  if (candidate === ".." || candidate.startsWith("../") || candidate.startsWith("/")) {
    throw new Error(`${importerPath}: module import escapes frontend root: ${specifier}`);
  }
  if (!/\.(?:m?js)$/i.test(candidate)) {
    throw new Error(`${importerPath}: browser module imports must include a .js or .mjs extension: ${specifier}`);
  }
  return normalizeRelativePath(candidate);
}

export async function localModuleClosure({
  root,
  entries,
  allowBareImports = false,
  allowDynamicImports = false,
  resolveFile = null,
}) {
  const frontendRoot = resolve(root);
  const pending = [...new Set(entries.map(normalizeRelativePath))].sort();
  const visited = new Set();

  while (pending.length) {
    const path = pending.shift();
    if (visited.has(path)) continue;
    const absolute = resolveFile ? resolve(resolveFile(path)) : resolve(frontendRoot, path);
    if (!isWithinRoot(frontendRoot, absolute)) throw new Error(`browser module escapes frontend root: ${path}`);
    let source;
    try {
      source = await readFile(absolute, "utf8");
    } catch (error) {
      throw new Error(`browser module is missing: ${path}`, { cause: error });
    }
    visited.add(path);
    for (const specifier of moduleSpecifiers(source, path, { allowDynamicImports })) {
      if (!specifier.startsWith(".")) {
        if (allowBareImports) continue;
        throw new Error(`${path}: browser modules must use local relative imports; found ${JSON.stringify(specifier)}`);
      }
      const dependency = resolveLocalSpecifier(path, specifier);
      if (!visited.has(dependency)) pending.push(dependency);
    }
    pending.sort();
  }

  return Object.freeze([...visited].sort());
}

export function browserModuleClosure(options) {
  return localModuleClosure(options);
}
