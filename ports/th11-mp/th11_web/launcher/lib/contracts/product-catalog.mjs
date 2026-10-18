import { loadCompiledContract } from "./load-compiled-contract.mjs";

const contract = await loadCompiledContract("product-catalog");
export const {
  HOST_PROTOCOL,
  PRODUCT_GAMES,
  PRODUCT_IDS,
  createLocalProductManifest,
  gameIdForProduct,
  isGameId,
  isMultiplayerProductId,
  isProductId,
  languagePriority,
  multiplayerConfigForProduct,
  multiplayerProductIdForGame,
  productFeatureAvailable,
} = contract;
