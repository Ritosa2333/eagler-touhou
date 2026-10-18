import { HOST_MANIFEST_FILE, validateHostManifest } from "../contracts/host-manifest.mjs";
import { RELEASE_CATALOG_FILE, validateReleaseCatalog } from "../contracts/release-catalog.mjs";

type MetadataKind = "release-catalog" | "host-manifest";
type FetchJson = (file: string, kind: MetadataKind) => Promise<unknown>;

type Settled<T> = Readonly<
  | { ok: true; value: T }
  | { ok: false; error: unknown }
>;

async function settle<T>(load: () => Promise<T>): Promise<Settled<T>> {
  try { return Object.freeze({ ok: true as const, value: await load() }); }
  catch (error) { return Object.freeze({ ok: false as const, error }); }
}

export async function loadRemoteMetadata(fetchJson: FetchJson) {
  if (typeof fetchJson !== "function") throw new TypeError("fetchJson must be a function");
  const [releaseCatalog, hostManifest] = await Promise.all([
    settle(async () => validateReleaseCatalog(await fetchJson(RELEASE_CATALOG_FILE, "release-catalog"))),
    settle(async () => validateHostManifest(await fetchJson(HOST_MANIFEST_FILE, "host-manifest"))),
  ]);
  return Object.freeze({ releaseCatalog, hostManifest });
}
