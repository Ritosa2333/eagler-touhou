import type {RetailInput,RetailPackage,RetailTrack} from './retail-package.mjs';
export const THKKD_DATA_LAYOUT: string;
export function selectThkkdArchives(inputs: readonly RetailInput[]): Map<string,RetailInput>;
export function convertThkkdArchives(inputs: readonly RetailInput[], options: {
  convertImage(bytes: Uint8Array, name: string, mask: Uint8Array | null): Promise<{blob: Blob; intrinsicAlpha: boolean}>;
  convertVideo(bytes: Uint8Array, name: string, signal: AbortSignal | undefined, onProgress: (value:number)=>void): Promise<Blob>;
  onTrack?(track: RetailTrack): void; signal?: AbortSignal;
}): Promise<Blob>;
export function thkkdPackage(data: Blob): Promise<RetailPackage>;
