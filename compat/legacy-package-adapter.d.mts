import type { LegacyGameDataPack } from "./legacy-game-pack.mjs";
import type { ParsedPackageZip } from "../package/package-zip.mjs";

export function adaptLegacyGamePackToPackage(
  pack: LegacyGameDataPack,
  options: { protocol: string },
): ParsedPackageZip;
