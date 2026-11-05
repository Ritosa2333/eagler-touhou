export interface StaticLanguagePackFileDeclaration {
  path: string;
  bytes: number;
  [key: string]: unknown;
}

export interface StaticLanguagePackManifest {
  schema: "eagler-touhou/thcrap-static-pack/1";
  game: string;
  language: string;
  runtimeVersion: string;
  files: StaticLanguagePackFileDeclaration[];
  [key: string]: unknown;
}

export interface ValidatedLanguagePackFile {
  path: string;
  bytes: Uint8Array;
}

export interface ValidatedStaticLanguagePack {
  manifest: StaticLanguagePackManifest;
  files: ValidatedLanguagePackFile[];
}

export function validateStaticLanguagePackEntries(
  entries: Readonly<Record<string, Uint8Array | undefined>>,
  { game, language }: { game: string; language: string },
): ValidatedStaticLanguagePack {
  const manifestBytes = entries["manifest.json"];
  if (!(manifestBytes instanceof Uint8Array)) throw new Error("语言包缺少清单");

  let parsed: unknown;
  try { parsed = JSON.parse(new TextDecoder().decode(manifestBytes)); }
  catch { throw new Error("语言包清单损坏"); }
  if (!parsed || typeof parsed !== "object" || Array.isArray(parsed)) throw new Error("语言包清单不兼容");
  const manifest = parsed as Record<string, unknown>;
  if (manifest.schema !== "eagler-touhou/thcrap-static-pack/1" || manifest.game !== game ||
      manifest.language !== language || typeof manifest.runtimeVersion !== "string" ||
      !Array.isArray(manifest.files) || manifest.files.length > 256) {
    throw new Error("语言包清单不兼容");
  }

  const prefix = `/thcrap/${game}/`;
  const expected = new Map<string, StaticLanguagePackFileDeclaration>();
  for (const rawFile of manifest.files) {
    if (!rawFile || typeof rawFile !== "object" || Array.isArray(rawFile)) throw new Error("语言包文件清单无效");
    const file = rawFile as Record<string, unknown>;
    if (typeof file.path !== "string" || !file.path.startsWith(prefix) || file.path.includes("\\") ||
        file.path.includes("..") || !Number.isInteger(file.bytes) || Number(file.bytes) < 0) {
      throw new Error("语言包文件清单无效");
    }
    if (expected.has(file.path)) throw new Error("语言包文件清单包含重复路径");
    expected.set(file.path, file as unknown as StaticLanguagePackFileDeclaration);
  }

  const names = Object.keys(entries).filter(name => name !== "manifest.json");
  if (names.length !== expected.size) throw new Error("语言包文件数量不一致");
  const files: ValidatedLanguagePackFile[] = [];
  for (const name of names) {
    if (!name.startsWith(prefix.slice(1)) || name.includes("\\") || name.includes("..")) {
      throw new Error("语言包路径无效");
    }
    const path = `/${name}`;
    const declaration = expected.get(path);
    const bytes = entries[name];
    if (!(bytes instanceof Uint8Array) || !declaration || bytes.length !== declaration.bytes) {
      throw new Error(`${path}: 语言包文件大小错误`);
    }
    files.push({ path, bytes });
  }
  files.sort((a, b) => a.path.localeCompare(b.path));
  return {
    manifest: manifest as unknown as StaticLanguagePackManifest,
    files,
  };
}
