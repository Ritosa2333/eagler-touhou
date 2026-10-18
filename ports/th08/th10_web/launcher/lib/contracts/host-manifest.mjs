import { loadCompiledContract } from "./load-compiled-contract.mjs";

const contract = await loadCompiledContract("host-manifest");
export const {
  HOST_MANIFEST_FILE,
  HOST_MANIFEST_SCHEMA,
  hostOriginMigrationAvailable,
  validateHostManifest,
} = contract;
