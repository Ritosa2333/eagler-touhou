export const RESOURCE_MODE_HOSTED = "hosted";
export const RESOURCE_MODE_IMPORT = "import";

export type ResourceMode = typeof RESOURCE_MODE_HOSTED | typeof RESOURCE_MODE_IMPORT;

export function normalizeResourceMode(value: unknown = RESOURCE_MODE_HOSTED): ResourceMode | null {
  if (value === RESOURCE_MODE_HOSTED) return RESOURCE_MODE_HOSTED;
  if (value === RESOURCE_MODE_IMPORT) return RESOURCE_MODE_IMPORT;
  return null;
}
