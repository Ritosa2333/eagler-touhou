import type {GameId} from '../contracts/product-catalog.mts';
import type {PackageDescriptor} from '../contracts/package-read-models.mts';
import type {SiteGameData} from '../contracts/site-manifest.mts';

export type RetailInput = Blob & {name: string; webkitRelativePath?: string; archivePath?: string};
export type RetailTrack = {name: string; completed: number; total: number};
export type RetailPackage = {descriptor: PackageDescriptor; payloads: Map<string, Blob>};

export function supportsRetailImport(game: GameId): boolean;
export function retailRequiredFiles(game: GameId): readonly string[];
export function retailArchiveLimits(game: GameId): Record<string, number>;
export function createRetailPackage(options: {
  game: GameId;
  expected?: Pick<SiteGameData, 'bytes' | 'layout'>;
  inputs: readonly RetailInput[];
  fontTargets?: readonly string[];
  loadFont?(target: string): Promise<Blob | null>;
  encodeOgg(input: Blob, options: {pcm: boolean; signal?: AbortSignal}): Promise<Blob>;
  onTrack?(track: RetailTrack): void;
  signal?: AbortSignal;
}): Promise<RetailPackage>;
