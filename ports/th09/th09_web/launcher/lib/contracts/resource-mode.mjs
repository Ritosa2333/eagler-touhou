import { loadCompiledContract } from "./load-compiled-contract.mjs";

const contract = await loadCompiledContract("resource-mode");
export const { RESOURCE_MODE_HOSTED, RESOURCE_MODE_IMPORT, normalizeResourceMode } = contract;
