import type {
  PackageDescriptor,
  PackageRuntimeDeclaration,
} from "../src/contracts/package-read-models.mjs";

export const PACKAGE_DESCRIPTOR_SCHEMA: "eagler-touhou/package/1";
export const PLAYER_PROTOCOL_V1: "eagler-touhou/player/1";
export function validatePackageDescriptor(value: unknown): PackageDescriptor;
export function resolvePackageRuntime(
  descriptor: PackageDescriptor,
  requestedVariant?: string | null,
): { variant: string; runtime: PackageRuntimeDeclaration };
