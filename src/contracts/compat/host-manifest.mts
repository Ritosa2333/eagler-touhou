import {SITE_MANIFEST_FILE, SITE_MANIFEST_SCHEMA, validateSiteManifest, type SiteManifest} from "../site-manifest.mjs";

export const LEGACY_HOST_MANIFEST_FILE = "host-manifest.json";
export const LEGACY_HOST_MANIFEST_SCHEMA = "eagler-touhou/host-manifest/1";

/** Published schema-1 sites are read through this boundary, never a second registry. */
export function readCompatibleSiteManifest(value: unknown): SiteManifest {
  if (value && typeof value === "object" && !Array.isArray(value) &&
      (value as {schema?: unknown}).schema === LEGACY_HOST_MANIFEST_SCHEMA) {
    return validateSiteManifest({...value, schema: SITE_MANIFEST_SCHEMA});
  }
  return validateSiteManifest(value);
}

/** A generated, read-only response for already released clients. */
export function legacyHostManifestResponse(value: SiteManifest) {
  return {...validateSiteManifest(value), schema: LEGACY_HOST_MANIFEST_SCHEMA};
}

export async function loadCompatibleSiteManifest(fetchJson: (file: string) => Promise<unknown>) {
  try {return validateSiteManifest(await fetchJson(SITE_MANIFEST_FILE));}
  catch (error) {
    const status = (error as {status?: unknown})?.status;
    if (status !== 404 && status !== 410 && !/\bHTTP (?:404|410)\b/.test(String(error))) throw error;
  }
  return readCompatibleSiteManifest(await fetchJson(LEGACY_HOST_MANIFEST_FILE));
}
