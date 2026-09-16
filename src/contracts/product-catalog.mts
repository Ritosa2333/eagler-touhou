export const HOST_PROTOCOL = "eagler-touhou/1";

// Product features are declarations for genuine per-game differences. Required
// all-game adapter capabilities live in adapter-capabilities.mts and must not be
// added here as booleans that a title can opt out of.
//
// Keep the three publication layers explicit:
// - product: static capability ceiling/difference;
// - Runtime Release: concrete build attestation (all entries below);
// - Host Manifest: only features the Launcher must query as booleans at run
//   time. Some capabilities have a richer Host surface instead: languages are
//   represented by languages/languageOptions rather than a duplicate flag.
export const PRODUCT_FEATURE_POLICY = Object.freeze({
  thprac: Object.freeze({ runtimeRelease: true, hostManifestFeature: true, hostSurface: "features.thprac", hostValueSource: "server-request-and-runtime" }),
  languages: Object.freeze({ runtimeRelease: true, hostManifestFeature: false, hostSurface: "languages + languageOptions", hostValueSource: "language-catalog" }),
  focusHitbox: Object.freeze({ runtimeRelease: true, hostManifestFeature: true, hostSurface: "features.focusHitbox", hostValueSource: "runtime-release" }),
} as const);
export type ProductFeatureId = keyof typeof PRODUCT_FEATURE_POLICY;
export const PRODUCT_FEATURE_IDS = Object.freeze(Object.keys(PRODUCT_FEATURE_POLICY) as ProductFeatureId[]);
export type HostRuntimeFeatureId = {
  [K in ProductFeatureId]: (typeof PRODUCT_FEATURE_POLICY)[K]["hostManifestFeature"] extends true ? K : never;
}[ProductFeatureId];
export const HOST_RUNTIME_FEATURE_IDS = Object.freeze(Object.entries(PRODUCT_FEATURE_POLICY)
  .filter(([, policy]) => policy.hostManifestFeature)
  .map(([id]) => id as HostRuntimeFeatureId));
export type HostRuntimeFeatures = Readonly<Partial<Record<HostRuntimeFeatureId, boolean>>>;

export function isHostRuntimeFeatureId(value: string): value is HostRuntimeFeatureId {
  return (HOST_RUNTIME_FEATURE_IDS as readonly string[]).includes(value);
}

export function languagePriority(id: string): number {
  return id === "ja" ? 0
    : id === "lang_zh-hans" ? 10
    : id === "lang_zh-hant" ? 11
    : id === "lang_en" ? 20
    : 100;
}

const REIMU_A = Object.freeze({ labelKey: "multiplayer.loadout.reimuA", glyph: "霊", character: 0, shot: 0 });
const REIMU_B = Object.freeze({ labelKey: "multiplayer.loadout.reimuB", glyph: "霊", character: 0, shot: 1 });
const MARISA_A = Object.freeze({ labelKey: "multiplayer.loadout.marisaA", glyph: "魔", character: 1, shot: 0 });
const MARISA_B = Object.freeze({ labelKey: "multiplayer.loadout.marisaB", glyph: "魔", character: 1, shot: 1 });
const SAKUYA_A = Object.freeze({ labelKey: "multiplayer.loadout.sakuyaA", glyph: "咲", character: 2, shot: 0 });
const SAKUYA_B = Object.freeze({ labelKey: "multiplayer.loadout.sakuyaB", glyph: "咲", character: 2, shot: 1 });
const TH06_MULTIPLAYER_LOADOUTS = Object.freeze([REIMU_A, REIMU_B, MARISA_A, MARISA_B]);
const TH07_MULTIPLAYER_LOADOUTS = Object.freeze([...TH06_MULTIPLAYER_LOADOUTS, SAKUYA_A, SAKUYA_B]);
const STANDARD_MULTIPLAYER_DIFFICULTIES = Object.freeze(["Easy", "Normal", "Hard", "Lunatic", "Extra"]);
const TH07_MULTIPLAYER_DIFFICULTIES = Object.freeze([...STANDARD_MULTIPLAYER_DIFFICULTIES, "Phantasm"]);
const STANDARD_MULTIPLAYER_PLAYER_COUNTS = Object.freeze([2, 3] as const);

export const PRODUCT_GAMES = Object.freeze({
  th06: Object.freeze({
    cardArtwork: "th06-card.webp",
    number: "06",
    title: "東方紅魔郷",
    subtitle: "the Embodiment of Scarlet Devil",
    storage: Object.freeze({
      saveRoot: "/savesth06",
      scoreFile: "score.dat",
      configFiles: Object.freeze(["東方紅魔郷.cfg", "th06.cfg"]),
    }),
    runtime: "./runtime/th06/th06.html",
    musicCapabilities: Object.freeze({ midi: true }),
    support: Object.freeze({
      sourceRepository: "https://github.com/YomotsuHisami/th06",
    }),
    dataProvider: "emscripten-preload",
    package: Object.freeze({
      dataFileId: "game-data",
      dataTarget: "/th06.data",
      musicSourceDirectories: Object.freeze({ wav: "bgm", ogg: "bgm" }),
      musicMounts: Object.freeze({ wav: "/bgm", ogg: "/bgm" }),
    }),
    replay: Object.freeze({ prefix: "th6" }),
    multiplayerRuntime: "./runtime/th06/multiplayer/th06.html",
    multiplayer: Object.freeze({
      titleKey: "game.title.th06mp",
      playerCounts: STANDARD_MULTIPLAYER_PLAYER_COUNTS,
      difficulties: STANDARD_MULTIPLAYER_DIFFICULTIES,
      loadouts: TH06_MULTIPLAYER_LOADOUTS,
      peerTransportGlobal: "__th06PeerTransport",
    }),
    features: Object.freeze({ thprac: true, languages: true, focusHitbox: true }),
  }),
  th07: Object.freeze({
    cardArtwork: "th07-card.webp",
    cardPresentation: Object.freeze({
      positionPercent: 58,
      artBrightness: 0.42,
      artSaturation: 0.62,
      glowBrightness: 0.8,
      glowSaturation: 0.88,
    }),
    number: "07",
    title: "東方妖々夢",
    subtitle: "Perfect Cherry Blossom",
    storage: Object.freeze({
      saveRoot: "/savesth07",
      scoreFile: "score.dat",
      configFiles: Object.freeze(["th07.cfg"]),
    }),
    runtime: "./runtime/th07/th07.html",
    musicCapabilities: Object.freeze({ midi: true }),
    support: Object.freeze({
      sourceRepository: "https://github.com/YomotsuHisami/th07",
    }),
    dataProvider: "emscripten-preload",
    package: Object.freeze({
      dataFileId: "game-data",
      dataTarget: "/th07.data",
      musicSourceDirectories: Object.freeze({ wav: ".", ogg: "bgm-ogg" }),
      musicMounts: Object.freeze({ wav: "/", ogg: "/bgm-ogg" }),
    }),
    replay: Object.freeze({ prefix: "th7" }),
    multiplayerRuntime: "./runtime/th07/multiplayer/th07.html",
    multiplayer: Object.freeze({
      titleKey: "game.title.th07mp",
      playerCounts: STANDARD_MULTIPLAYER_PLAYER_COUNTS,
      difficulties: TH07_MULTIPLAYER_DIFFICULTIES,
      loadouts: TH07_MULTIPLAYER_LOADOUTS,
      peerTransportGlobal: "__th07PeerTransport",
    }),
    features: Object.freeze({ thprac: true, languages: true, focusHitbox: false }),
  }),
  th08: Object.freeze({
    cardArtwork: "th08-card.webp",
    number: "08",
    title: "東方永夜抄",
    subtitle: "Imperishable Night",
    storage: Object.freeze({
      saveRoot: "/savesth08",
      scoreFile: "score.dat",
      configFiles: Object.freeze(["th08.cfg"]),
    }),
    runtime: "./runtime/th08/th08.html",
    musicCapabilities: Object.freeze({ midi: true }),
    support: Object.freeze({
      sourceRepository: "https://github.com/YomotsuHisami/th08",
      adaptationNotice: "early-test",
    }),
    runtimeFileLayout: "directory",
    requiredShared: Object.freeze(["/msgothic.ttc", "/unifont.otf"]),
    runtimeAssets: Object.freeze([
      "th08.html",
      "manifest.json",
      "shell.mjs",
      "eagler-host.mjs",
      "motion-replay.mjs",
      "th08-sdl.mjs",
      "th08-sdl.wasm",
      "resources.json",
      "fonts/blend.bin",
      "fonts/cp932.bin",
    ]),
    dataProvider: "retail-memory",
    package: Object.freeze({
      dataFileId: "game-data",
      dataTarget: "/th08.data",
      rawDataImport: Object.freeze({ fileNames: Object.freeze(["th08.dat"]) }),
      musicSourceDirectories: Object.freeze({ ogg: "bgm-ogg" }),
      musicMounts: Object.freeze({ ogg: "/bgm-ogg" }),
    }),
    replay: Object.freeze({ prefix: "th8" }),
    features: Object.freeze({ thprac: false, languages: false, focusHitbox: false }),
  }),
  th10: Object.freeze({
    cardArtwork: "th10-card.webp",
    number: "10",
    title: "東方風神録",
    subtitle: "Mountain of Faith",
    storage: Object.freeze({ saveRoot: "/savesth10", scoreFile: "scoreth10.dat", configFiles: Object.freeze(["th10.cfg"]) }),
    runtime: "./runtime/th10/th10.html",
    musicCapabilities: Object.freeze({ midi: false }),
    support: Object.freeze({
      sourceRepository: "https://github.com/YomotsuHisami/th10",
      adaptationNotice: "early-test",
    }),
    runtimeFileLayout: "directory",
    requiredShared: Object.freeze(["/msgothic.ttc", "/unifont.otf"]),
    runtimeAssets: Object.freeze([
      "th10.html",
      "manifest.json",
      "shell.mjs",
      "eagler-host.mjs",
      "motion-replay.mjs",
      "th10-sdl.mjs",
      "th10-sdl.wasm",
      "resources.json",
      "fonts/blend.bin",
      "fonts/codepages.bin",
    ]),
    dataProvider: "retail-memory",
    package: Object.freeze({
      dataFileId: "game-data", dataTarget: "/th10.data",
      musicSourceDirectories: Object.freeze({ ogg: "bgm-ogg" }),
      musicMounts: Object.freeze({ ogg: "/bgm-ogg" }),
    }),
    replay: Object.freeze({ prefix: "th10" }),
    features: Object.freeze({ thprac: false, languages: false, focusHitbox: false }),
  }),
});

export type GameId = keyof typeof PRODUCT_GAMES;
export type ProductGame = (typeof PRODUCT_GAMES)[GameId];
export type MultiplayerGameId = {
  [K in GameId]: (typeof PRODUCT_GAMES)[K] extends { readonly multiplayerRuntime: string } ? K : never;
}[GameId];
export type MultiplayerProductId = `${MultiplayerGameId}mp`;
export type ProductId = GameId | MultiplayerProductId;
export interface MultiplayerLoadoutConfig {
  labelKey: string;
  glyph: string;
  character: number;
  shot: number;
}
export interface MultiplayerProductConfig {
  titleKey: string;
  playerCounts: readonly (2 | 3)[];
  difficulties: readonly string[];
  loadouts: readonly MultiplayerLoadoutConfig[];
  peerTransportGlobal: string;
}

const productGameEntries = Object.entries(PRODUCT_GAMES) as Array<[GameId, ProductGame]>;
const multiplayerProductGames = Object.freeze(Object.fromEntries(
  productGameEntries
    .filter((entry): entry is [MultiplayerGameId, Extract<ProductGame, { readonly multiplayerRuntime: string }>] =>
      "multiplayerRuntime" in entry[1] && "multiplayer" in entry[1])
    .map(([gameId]) => [`${gameId}mp`, gameId]),
)) as Readonly<Partial<Record<MultiplayerProductId, MultiplayerGameId>>>;

export const PRODUCT_IDS = Object.freeze([
  ...Object.keys(PRODUCT_GAMES),
  ...Object.keys(multiplayerProductGames),
]) as readonly ProductId[];

// Default navigation policy belongs with the product registry, not Launcher
// flow code. Keep today's behavior explicit so adding another product never
// requires hunting for fallback game IDs in routing/lobby logic.
export const DEFAULT_PRODUCT_ID: ProductId = "th06";
export const DEFAULT_MULTIPLAYER_PRODUCT_ID: MultiplayerProductId = "th07mp";

export function isGameId(value: string): value is GameId {
  return Object.hasOwn(PRODUCT_GAMES, value);
}

export function productEnabledForBuild(productId: string, testBuild = false): boolean {
  if (!isProductId(productId)) return false;
  const game = PRODUCT_GAMES[gameIdForProduct(productId as ProductId)];
  return !("testOnly" in game && game.testOnly) || testBuild === true;
}

export function isProductId(value: string): value is ProductId {
  return PRODUCT_IDS.includes(value as ProductId);
}

export function isMultiplayerProductId(productId: string): productId is MultiplayerProductId {
  return Object.hasOwn(multiplayerProductGames, productId);
}

export function gameIdForProduct(productId: ProductId): GameId;
export function gameIdForProduct(productId: string): GameId | string;
export function gameIdForProduct(productId: string): GameId | string {
  return isMultiplayerProductId(productId) ? multiplayerProductGames[productId] ?? productId : productId;
}

export function multiplayerProductIdForGame(gameId: string): MultiplayerProductId | null {
  const productId = `${gameId}mp`;
  return isMultiplayerProductId(productId) ? productId : null;
}

export function multiplayerConfigForProduct(productId: string): MultiplayerProductConfig | null {
  const gameId = gameIdForProduct(productId);
  if (!isGameId(gameId)) return null;
  const product = PRODUCT_GAMES[gameId];
  return "multiplayer" in product
    ? product.multiplayer as MultiplayerProductConfig
    : null;
}

const HOST_RUNTIME_FEATURES = new Set<ProductFeatureId>(HOST_RUNTIME_FEATURE_IDS);

// Static product policy is the capability ceiling. A Host may report that its
// concrete Runtime lacks an attested Runtime feature, but it cannot enable a
// feature the product does not support or override Launcher-owned features.
// Missing Host fields preserve host-manifest/1 deployments from before Runtime
// capability attestation became explicit.
export function productFeatureAvailable(
  gameId: string,
  featureId: ProductFeatureId,
  hostFeatures: HostRuntimeFeatures | null = null,
): boolean {
  const supported = isGameId(gameId) && PRODUCT_GAMES[gameId].features[featureId] === true;
  if (!supported || !HOST_RUNTIME_FEATURES.has(featureId)) return supported;
  const hosted = hostFeatures?.[featureId as HostRuntimeFeatureId];
  return hosted == null ? true : hosted === true;
}

export function createLocalProductManifest() {
  return {
    protocol: HOST_PROTOCOL,
    shared: { resourceMode: "hosted" as const, testBuild: false },
    games: Object.fromEntries(productGameEntries.map(([game, product]) => [game, {
      ...product,
      music: { midi: product.musicCapabilities.midi ? { files: [] } : { files: [], supported: false } },
      languageOptions: [{ id: "ja", title: "日本語(原版)", pack: null }],
    }])),
  };
}
