import type {
  InstalledPackageGeneration,
  PackageDescriptor,
} from "../src/contracts/package-read-models.mjs";

export function componentFileIds(
  descriptor: PackageDescriptor,
  componentId: string,
  entryIds?: readonly string[] | null,
): string[];

export function attachGenerationFile(
  generation: InstalledPackageGeneration,
  fileId: string,
  objectId: string,
  options?: { storageMode?: "arraybuffer" | null },
): InstalledPackageGeneration;
