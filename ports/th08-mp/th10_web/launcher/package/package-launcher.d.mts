import type {
  InstalledPackageGeneration,
  InstalledPackageResult,
  PackageDescriptor,
  PackageInstallation,
} from "../src/contracts/package-read-models.mjs";
import type { PackageInstallProgress } from "./package-installer.mjs";

export type { PackageInstallProgress } from "./package-installer.mjs";

export interface PublishedPackageFileSelection {
  current?: InstalledPackageGeneration | null;
  addComponents?: readonly string[];
  addFileIds?: readonly string[];
  selectedComponentEntries?: Readonly<Record<string, readonly string[]>>;
}

export interface InstallPublishedPackageOptions extends Omit<PublishedPackageFileSelection, "current"> {
  catalog: unknown;
  catalogUrl: string;
  preserveLocalSource?: boolean;
  fetchImpl?: typeof fetch;
  onProgress?: ((progress: PackageInstallProgress) => void) | null;
  signal?: AbortSignal | null;
}

export function desiredFilesForPublishedPackage(
  descriptor: PackageDescriptor,
  options?: PublishedPackageFileSelection,
): string[];

export function installedComponentIds(generation: InstalledPackageGeneration | null): string[];

export function installPublishedPackage(
  game: string,
  options: InstallPublishedPackageOptions,
): Promise<InstalledPackageResult & {
  entry: unknown;
  descriptor: PackageDescriptor;
  descriptorUrl: string;
}>;

export function publishedPackageStatus(game: string, catalog: unknown): Promise<{
  installation: PackageInstallation | null;
  generation: InstalledPackageGeneration | null;
  published: unknown;
  updateAvailable: boolean;
}>;
