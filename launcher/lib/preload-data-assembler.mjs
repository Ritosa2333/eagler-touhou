import { mkdir, readFile, writeFile } from "node:fs/promises";
import { basename, dirname, resolve } from "node:path";
import { extractGameDataLayout } from "./runtime-data-layout.mjs";

export async function assemblePreloadData({ game, runtimeScript, sourceDirectory, output }) {
  const source = typeof runtimeScript === "string" && runtimeScript.includes("loadPackage(")
    ? runtimeScript
    : await readFile(runtimeScript, "utf8");
  const layout = extractGameDataLayout(source, game);
  const chunks = [];
  for (const [target, start, end] of layout.files) {
    const path = resolve(sourceDirectory, basename(target));
    const bytes = await readFile(path);
    const expected = end - start;
    if (bytes.length !== expected) {
      throw new Error(`${game}: ${basename(target)} size mismatch for Runtime DATA layout: ${bytes.length}/${expected}`);
    }
    chunks.push(bytes);
  }
  const data = Buffer.concat(chunks);
  if (data.length !== layout.bytes) throw new Error(`${game}: assembled Runtime DATA size mismatch`);
  if (output) {
    await mkdir(dirname(output), { recursive: true });
    await writeFile(output, data);
  }
  return { data, layout };
}
