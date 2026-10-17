import { loadCompiledContract } from "./load-compiled-contract.mjs";

const contract = await loadCompiledContract("release-catalog");
export const {
  RELEASE_CATALOG_FILE,
  RELEASE_CATALOG_SCHEMA,
  releaseCatalogEntryUrl,
  validateReleaseCatalog,
} = contract;
