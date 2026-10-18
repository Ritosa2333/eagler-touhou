import type { InstalledPackageGeneration } from "../contracts/package-read-models.mjs";

export interface ManagedRuntimeGenerationRequest {
  game?: unknown;
  generation?: unknown;
}

export function createManagedRuntimeGenerationLease() {
  let game: string | null = null;
  let generation: InstalledPackageGeneration | null = null;

  return Object.freeze({
    bind(nextGame: string, nextGeneration: InstalledPackageGeneration) {
      if (!nextGame || !nextGeneration?.id || nextGeneration.game !== nextGame) {
        throw new Error("Cannot bind Managed Runtime to an invalid game generation");
      }
      game = nextGame;
      generation = nextGeneration;
    },
    clear() {
      game = null;
      generation = null;
    },
    resolve(request: ManagedRuntimeGenerationRequest | null | undefined) {
      if (!generation?.id || request?.game !== game || request.generation !== generation.id) {
        throw new Error("Managed Runtime requested an inactive game generation");
      }
      return generation;
    },
  });
}
