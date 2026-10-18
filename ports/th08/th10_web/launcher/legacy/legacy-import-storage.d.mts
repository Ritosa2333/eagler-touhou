import type { ParsedPackageZip } from "../package/package-zip.mjs";

export interface LegacyMigrationResult {
  status: "migrated" | "already-current" | "incomplete" | "absent";
  missing?: unknown;
}

export function migrateLegacyStoredImport(game: string, options: {
  protocol: string;
  fallbackGameData?: unknown;
  currentRevision?: string | null;
  install(parsed: ParsedPackageZip): Promise<unknown>;
  origin: string;
}): Promise<LegacyMigrationResult>;
