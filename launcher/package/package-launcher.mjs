import {normalizeGameVersion} from "./game-version.mjs";
import { validatePackageDescriptor } from "./package-descriptor.mjs";
import { componentFileIds } from "./package-generation.mjs";
import { installPackageFromAcquisition, installPackageFromRemote } from "./package-installer.mjs";
import { packageObjectMatches, readCurrentPackageGeneration, readPackageObject } from "./package-store.mjs";
import { sha256Hex } from "./package-integrity.mjs";
import { releaseCatalogEntryUrl, validateReleaseCatalog } from "../release-catalog.mjs";

function unique(ids) {
  return [...new Set(ids)];
}

async function damagedInstalledFileIds(generation, ids, signal) {
  const damaged = new Set();
  for (const id of ids) {
    signal?.throwIfAborted();
    const file = generation.descriptor.files[id], ref = generation.files[id];
    const object = await readPackageObject(ref?.objectId);
    if (!await packageObjectMatches(object, file.bytes, file.sha256)) damaged.add(id);
  }
  return damaged;
}

export function canUseExistingInstallationAfterRemoteFailure({
  hostManifestAvailable,
  installedGeneration,
} = {}) {
  return hostManifestAvailable === true && !!installedGeneration;
}

export async function fetchPublishedPackage(game, {
  catalog,
  catalogUrl,
  fetchImpl = globalThis.fetch,
  signal = null,
} = {}) {
  validateReleaseCatalog(catalog);
  const entry = catalog.games[game];
  if (!entry) return null;
  if (typeof fetchImpl !== "function") throw new Error("fetch unavailable");
  const descriptorUrl = releaseCatalogEntryUrl(catalogUrl, catalog, game);
  const response = await fetchImpl(descriptorUrl, { cache: "no-store", signal });
  if (!response.ok) throw new Error(`${new URL(descriptorUrl).pathname}: HTTP ${response.status}`);
  const descriptor = validatePackageDescriptor(await response.json());
  if (descriptor.game !== game) throw new Error(`${game}: published Package game mismatch`);
  if (descriptor.revision !== entry.revision) throw new Error(`${game}: Release Catalog revision does not match Package Descriptor`);
  return { entry, descriptor, descriptorUrl };
}

export function installedComponentIds(generation) {
  if (!generation?.descriptor?.components) return [];
  const installed = [];
  for (const componentId of Object.keys(generation.descriptor.components)) {
    const ids = componentFileIds(generation.descriptor, componentId);
    if (ids.some(fileId => !!generation.files?.[fileId]?.objectId)) installed.push(componentId);
  }
  return installed;
}

export function desiredFilesForPublishedPackage(descriptor, {
  current = null,
  addComponents = [],
  addFileIds = [],
  selectedComponentEntries = {},
} = {}) {
  validatePackageDescriptor(descriptor);
  if (!selectedComponentEntries || typeof selectedComponentEntries !== "object" || Array.isArray(selectedComponentEntries)) {
    throw new Error("selected Package component entries must be an object");
  }
  for (const [componentId, selected] of Object.entries(selectedComponentEntries)) {
    if (!Array.isArray(selected)) throw new Error(`Package component ${componentId} selection must be an array`);
    if (!Object.hasOwn(descriptor.components, componentId) && selected.length) {
      throw new Error(`unknown requested Package component: ${componentId}`);
    }
  }
  const ids = [...descriptor.base.files];
  for (const fileId of addFileIds) {
    if (typeof fileId !== "string" || !Object.hasOwn(descriptor.files, fileId)) {
      throw new Error(`unknown requested Package file: ${String(fileId)}`);
    }
    ids.push(fileId);
  }
  const explicitlyAdded = new Set(addComponents);
  for (const [componentId, nextComponent] of Object.entries(descriptor.components)) {
    const hasSelection = Object.hasOwn(selectedComponentEntries, componentId);
    if (hasSelection && explicitlyAdded.has(componentId)) {
      throw new Error(`Package component ${componentId} cannot request both all files and selected entries`);
    }
    if (hasSelection) {
      if (!Array.isArray(nextComponent.entries)) {
        throw new Error(`Package component ${componentId} does not support entry selection`);
      }
      const selected = selectedComponentEntries[componentId];
      const available = new Set(nextComponent.entries.map(entry => entry.id));
      for (const entryId of selected) {
        if (typeof entryId !== "string" || !available.has(entryId)) {
          throw new Error(`unknown requested Package component entry: ${componentId}/${String(entryId)}`);
        }
      }
      ids.push(...componentFileIds(descriptor, componentId, unique(selected)));
      continue;
    }
    if (explicitlyAdded.has(componentId)) {
      ids.push(...componentFileIds(descriptor, componentId));
      continue;
    }
    const previousComponent = current?.descriptor?.components?.[componentId];
    if (!previousComponent) continue;
    if (Array.isArray(nextComponent.entries) && Array.isArray(previousComponent.entries)) {
      const installedEntryIds = previousComponent.entries
        .filter(entry => !!current.files?.[entry.file]?.objectId)
        .map(entry => entry.id);
      ids.push(...componentFileIds(descriptor, componentId, installedEntryIds));
      continue;
    }
    if ((nextComponent.type === "ogg" && previousComponent.type === "ogg") ||
        (nextComponent.type === "pc98-disk" && previousComponent.type === "pc98-disk")) {
      // Progressive components retain only installed files. Shared disk
      // blocks must not cause every disk using them to be acquired.
      ids.push(...componentFileIds(current.descriptor, componentId)
        .filter(fileId => !!current.files?.[fileId]?.objectId && Object.hasOwn(descriptor.files, fileId)));
      continue;
    }
    if (componentFileIds(current.descriptor, componentId).some(fileId => !!current.files?.[fileId]?.objectId)) {
      ids.push(...componentFileIds(descriptor, componentId));
    }
  }
  return unique(ids);
}

export async function installPublishedPackage(game, {
  gameVersion = "original",
  catalog,
  catalogUrl,
  addComponents = [],
  addFileIds = [],
  selectedComponentEntries = {},
  preserveLocalSource = true,
  expectedGenerationId = null,
  fetchImpl = globalThis.fetch,
  onProgress = null,
  signal = null,
} = {}) {
  const published = await fetchPublishedPackage(game, { catalog, catalogUrl, fetchImpl, signal });
  if (!published) throw new Error(`${game}: no published Package`);
  if (normalizeGameVersion(gameVersion) === "beauty") published.descriptor = {...published.descriptor, gameVersion: "beauty"};
  else if (published.descriptor.gameVersion === "beauty") throw new Error(`${game}: original catalog declares a beauty Package`);
  const installed = await installPackageFromRemote(published.descriptor, {
    descriptorUrl: published.descriptorUrl,
    expectedGenerationId: expectedGenerationId ?? undefined,
    // Resolve preservation policy inside the installer's per-game mutation
    // queue. Otherwise a queued import/update can advance current after these
    // decisions were calculated and then be silently dropped.
    desiredFileIds: async currentResult => {
      if (expectedGenerationId !== null && currentResult.generation?.id !== expectedGenerationId) throw new Error('Package changed before background update');
      let current = currentResult.generation;
      if (current) {
        const required = new Set(current.descriptor.base.files);
        const optional = Object.keys(current.files).filter(id => current.files[id]?.objectId && !required.has(id));
        const damaged = await damagedInstalledFileIds(current, optional, signal);
        if (damaged.size) current = {...current,
          files: Object.fromEntries(Object.entries(current.files).filter(([id]) => !damaged.has(id)))};
      }
      return desiredFilesForPublishedPackage(published.descriptor, {
        current, addComponents, addFileIds, selectedComponentEntries,
      });
    },
    source: currentResult => preserveLocalSource && currentResult.installation?.source === "local" ? "local" : "remote",
    fetchImpl,
    onProgress,
    signal,
  });
  return { ...published, ...installed };
}

export async function repairInstalledPackage({generation, installation}, {
  getPublication, fetchImpl = globalThis.fetch, signal, onProgress,
}) {
  const {descriptor} = generation;
  const ids = unique([...descriptor.base.files, ...Object.keys(generation.files)
    .filter(id => generation.files[id]?.objectId)]);
  const damaged = await damagedInstalledFileIds(generation, ids, signal);
  if (!damaged.size) return generation;

  // Required files must be restored. Missing optional files return to their
  // normal acquisition/fallback path instead of blocking an unrelated launch.
  const required = new Set(descriptor.base.files);
  const desiredFileIds = ids.filter(id => required.has(id) || !damaged.has(id));

  let published;
  return (await installPackageFromAcquisition({
    descriptor,
    desiredFileIds,
    expectedGenerationId: generation.id,
    source: installation?.source ?? "remote",
    reuseCurrent: true,
    signal,
    onProgress,
    acquire: async (id, file) => {
      if (!published) {
        const {catalog, catalogUrl} = await getPublication();
        if (catalog?.games[descriptor.game]) {
          published = await fetchPublishedPackage(descriptor.game, {catalog, catalogUrl, fetchImpl, signal});
        }
      }
      // Match content rather than package IDs, which can differ for local imports.
      const replacement = Object.values(published?.descriptor.files ?? {}).find(candidate =>
        candidate.target === file.target && candidate.bytes === file.bytes && candidate.sha256 &&
        (file.sha256 ? candidate.sha256.toLowerCase() === file.sha256.toLowerCase()
          : candidate.revision === file.revision && file.revision.toLowerCase() === candidate.sha256.slice(0, 16).toLowerCase()));
      if (!replacement) throw new Error(`${id}: no matching cloud resource for repair`);
      const response = await fetchImpl(new URL(replacement.source, published.descriptorUrl), {cache: "no-store", signal});
      if (!response.ok) throw new Error(`${id}: resource repair failed (HTTP ${response.status})`);
      const bytes = await response.arrayBuffer();
      if (!file.sha256 && await sha256Hex(bytes) !== replacement.sha256.toLowerCase()) {
        throw new Error(`${id}: resource repair SHA-256 mismatch`);
      }
      return bytes;
    },
  })).generation;
}

export async function publishedPackageStatus(game, catalog) {
  const current = await readCurrentPackageGeneration(game);
  const published = catalog?.games?.[game] || null;
  return {
    installation: current.installation,
    generation: current.generation,
    published,
    updateAvailable: !!current.generation && !!published && current.generation.descriptor?.revision !== published.revision,
  };
}
