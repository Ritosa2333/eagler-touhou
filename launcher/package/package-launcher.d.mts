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
  gameVersion?: "original" | "beauty";
  catalog: unknown;
  catalogUrl: string;
  preserveLocalSource?: boolean;
  expectedGenerationId?: string | null;
  fetchImpl?: typeof fetch;
  onProgress?: ((progress: PackageInstallProgress) => void) | null;
  signal?: AbortSignal | null;
}

export function canUseExistingInstallationAfterRemoteFailure(options?: {
  hostManifestAvailable?: boolean;
  installedGeneration?: InstalledPackageGeneration | null;
}): boolean;

export function desiredFilesForPublishedPackage(
  descriptor: PackageDescriptor,
  options?: PublishedPackageFileSelection,
): string[];

export function fetchPublishedPackage(game: string, options: {
  catalog: unknown;
  catalogUrl: string;
  fetchImpl?: typeof fetch;
  signal?: AbortSignal | null;
}): Promise<{entry: unknown; descriptor: PackageDescriptor; descriptorUrl: string} | null>;

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
