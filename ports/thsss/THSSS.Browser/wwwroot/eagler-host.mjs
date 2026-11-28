export async function mountManagedData(parentWindow, query, game) {
  const epoch = Number(query.get("runtimeEpoch"));
  const generation = query.get("gameGeneration") || "";
  if (!parentWindow || typeof parentWindow.__eaglerPrepareManagedRuntimeDataV1 !== "function") return null;
  return parentWindow.__eaglerPrepareManagedRuntimeDataV1({ game, generation, epoch });
}
