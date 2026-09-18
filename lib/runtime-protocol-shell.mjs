import { RUNTIME_EPOCH_QUERY_PARAMETER } from "./contracts/runtime-protocol.mjs";

function assertSource(source, game, variant, label) {
  if (typeof source !== "string" || !source) {
    throw new Error(`${game} ${variant}: ${label} is not text`);
  }
}

export function assertRuntimeNavigationEpochShell(source, game, variant) {
  assertSource(source, game, variant, "Runtime protocol shell");
  if (!source.includes(RUNTIME_EPOCH_QUERY_PARAMETER)) {
    throw new Error(`${game} ${variant}: Runtime shell is stale: navigation epoch URL binding is missing`);
  }
  if (!/\.epoch\s*(?:===|!==)\s*[A-Za-z_$][\w$]*/.test(source)) {
    throw new Error(`${game} ${variant}: Runtime shell is stale: Host command epoch rejection is missing`);
  }
  if (!/postMessage\s*\(\s*\{[^}]{0,220}\bepoch\b/s.test(source)) {
    throw new Error(`${game} ${variant}: Runtime shell is stale: Runtime messages do not echo the navigation epoch`);
  }
  return true;
}

export function assertManagedRuntimeDataEpochBridge(source, game, variant) {
  assertSource(source, game, variant, "managed DATA bridge");
  const providerIndex = source.indexOf("__eaglerPrepareManagedRuntimeDataV1");
  if (providerIndex < 0) {
    throw new Error(`${game} ${variant}: managed DATA provider bridge is missing`);
  }
  const providerSource = source.slice(providerIndex, providerIndex + 1800);
  const bridgeEpoch = /\{[^}]{0,320}\bepoch\b\s*(?::|[,}])/s;
  if (!source.includes(RUNTIME_EPOCH_QUERY_PARAMETER) || !bridgeEpoch.test(providerSource)) {
    throw new Error(`${game} ${variant}: managed DATA bridge is stale: navigation epoch binding is missing`);
  }
  return true;
}

export function assertRuntimeProtocolSources(sources, game, variant) {
  if (!Array.isArray(sources) || !sources.length) {
    throw new Error(`${game} ${variant}: Runtime protocol sources are missing`);
  }
  const texts = sources.map(item => typeof item === "string" ? item : item?.source).filter(Boolean);
  const hasNavigationShell = texts.some(source => {
    try { assertRuntimeNavigationEpochShell(source, game, variant); return true; }
    catch { return false; }
  });
  if (!hasNavigationShell) {
    throw new Error(`${game} ${variant}: Runtime shell is stale: navigation epoch protocol contract is missing`);
  }
  const hasManagedDataBridge = texts.some(source => {
    try { assertManagedRuntimeDataEpochBridge(source, game, variant); return true; }
    catch { return false; }
  });
  if (!hasManagedDataBridge) {
    throw new Error(`${game} ${variant}: managed DATA bridge is stale: navigation epoch contract is missing`);
  }
  return true;
}
