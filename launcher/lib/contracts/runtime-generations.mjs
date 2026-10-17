import { loadCompiledContract } from "./load-compiled-contract.mjs";
const contract = await loadCompiledContract("runtime-generations");
export const {
  RUNTIME_MANIFEST_SCHEMA, RUNTIME_GENERATION_SCHEMA, RUNTIME_MANIFEST_FILE,
  RUNTIME_GENERATION_FILE, RUNTIME_PROTOCOL, RUNTIME_PREPARE, RUNTIME_CAPABILITIES,
  RUNTIME_CACHE_PROTOCOL, RUNTIME_CACHE_MAX_PREVIOUS, isRuntimePath, isRuntimeRoot,
  canonicalRuntimePayload, validateRuntimeGeneration, validateRuntimeManifest,
  runtimeGenerationBase, runtimeGenerationEntry, parseRuntimeGenerationPath, findRuntimeGroup, validateRuntimePrepareRequest,
} = contract;
