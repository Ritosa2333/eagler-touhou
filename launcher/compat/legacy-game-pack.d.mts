import type { GameId } from "../src/contracts/product-catalog.mjs";

export interface LegacyDeclaredFile {
  name: string;
  path: string;
  bytes: number;
  sha256: string;
  blob: Blob;
  [key: string]: unknown;
}

export interface LegacyRuntimeFile extends LegacyDeclaredFile {
  role: "html" | "js" | "wasm";
}

export interface LegacySharedFile extends LegacyDeclaredFile {
  target: string;
}

export interface LegacyLanguageFile extends LegacyDeclaredFile {
  id: string;
  title: string;
  runtimeVersion: string;
}

export interface LegacyGameDataManifest {
  schema: "eagler-touhou/game-data-pack/1" | "eagler-touhou/offline-game-pack/1";
  game: GameId;
  version: string;
  data: { path: string; layout: string; bytes: number; sha256: string };
  music?: { mode: "ogg"; version: string; files: Array<{ path: string; bytes: number; sha256: string }> };
  offline?: {
    runtime: { version: string; files: Array<{ role: "html" | "js" | "wasm"; path: string; bytes: number; sha256: string }> };
    shared: Array<{ target: string; path: string; bytes: number; sha256: string }>;
    languages: Array<{ id: string; title: string; path: string; bytes: number; sha256: string; runtimeVersion: string }>;
  };
}

export interface LegacyGameDataPack {
  manifest: LegacyGameDataManifest;
  data: LegacyDeclaredFile;
  music: LegacyDeclaredFile[];
  offline: {
    runtime: { version: string; files: LegacyRuntimeFile[] };
    shared: LegacySharedFile[];
    languages: LegacyLanguageFile[];
  } | null;
}

export function parseStoredGameDataPack(blob: Blob): Promise<LegacyGameDataPack>;
