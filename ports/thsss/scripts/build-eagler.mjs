#!/usr/bin/env node
// Source identity for ports:check, and the Windows publish + stage entry.
import { createHash } from "node:crypto";
import { execFileSync } from "node:child_process";
import { readFileSync, readdirSync } from "node:fs";
import { relative, resolve, sep } from "node:path";
import { fileURLToPath } from "node:url";

const port = resolve(fileURLToPath(new URL("..", import.meta.url)));
const roots = ["THSSS.Browser", "THSSS.Platform", "THSSS.SlimDX", "patches", "tools", "vendor"];
const extras = ["package.json", "package-lock.json", "nuget.config", "global.json", ".config/dotnet-tools.json"];
const skipDir = new Set(["bin", "obj", "node_modules", "assets"]);

function walk(directory, out) {
  for (const entry of readdirSync(directory, { withFileTypes: true })) {
    if (entry.isDirectory()) {
      if (skipDir.has(entry.name)) continue;
      walk(resolve(directory, entry.name), out);
      continue;
    }
    if (entry.isFile()) out.push(resolve(directory, entry.name));
  }
}

function sourceFiles() {
  const paths = [];
  for (const root of roots) walk(resolve(port, root), paths);
  for (const file of extras) paths.push(resolve(port, file));
  return [...new Set(paths.map((path) => relative(port, path).split(sep).join("/")))].sort();
}

if (process.argv.includes("--print-inputs")) {
  const names = sourceFiles();
  const inputs = Object.fromEntries(names.map((name) => {
    const bytes = readFileSync(resolve(port, name));
    return [name, { bytes: bytes.length, sha256: createHash("sha256").update(bytes).digest("hex") }];
  }));
  process.stdout.write(`${JSON.stringify({
    schema: "eagler-touhou/runtime-inputs/1",
    directory: "thsss",
    translationUnits: names.filter((name) => name.endsWith(".cs")),
    inputs,
    compilerFlags: [],
  }, null, 2)}\n`);
  process.exit(0);
}

execFileSync("dotnet", ["publish", "THSSS.Browser/THSSS.Browser.csproj", "-c", "Release"], {
  cwd: port,
  stdio: "inherit",
});
execFileSync("npm", ["run", "stage:eagler"], { cwd: port, stdio: "inherit" });
