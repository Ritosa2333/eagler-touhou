import { loadCompiledContract } from "./load-compiled-contract.mjs";

const contract = await loadCompiledContract("site-manifest");
export const {
  SITE_MANIFEST_FILE,
  SITE_MANIFEST_SCHEMA,
  siteOriginMigrationAvailable,
  validateSiteManifest,
} = contract;
