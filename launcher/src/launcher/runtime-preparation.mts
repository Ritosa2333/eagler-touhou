import { readPackageObject, type StoredPackageObject } from "../../package/package-store.mjs";
import type { InstalledPackageGeneration } from "../contracts/package-read-models.mjs";

type ReadObject = (objectId: string) => Promise<StoredPackageObject | null>;

interface ReadOptions {
  readObject?: ReadObject;
  timeoutMs?: number;
}

export class InstalledGameDataError extends Error {}

function packageDataFileId(generation: InstalledPackageGeneration) {
  const descriptor = generation?.descriptor;
  if (!descriptor?.files || !generation?.files) throw new InstalledGameDataError("Package generation is unavailable");
  const declared = descriptor.runtimeRequirement?.dataFile;
  if (declared) {
    if (!descriptor.files[declared] || !generation.files[declared]?.objectId) {
      throw new InstalledGameDataError("Installed game DATA is missing");
    }
    return declared;
  }
  if (descriptor.files["game-data"] && generation.files["game-data"]?.objectId) return "game-data";
  const matches = Object.entries(descriptor.files)
    .filter(([fileId, declaration]) => generation.files?.[fileId]?.objectId && /\.data$/i.test(String(declaration?.target || "")))
    .map(([fileId]) => fileId);
  if (matches.length !== 1) throw new InstalledGameDataError("Package generation must contain exactly one installed DATA file");
  const [fileId] = matches;
  if (!fileId) throw new InstalledGameDataError("Package generation DATA file resolution failed");
  return fileId;
}

async function storedObjectBuffer(stored: StoredPackageObject | null) {
  if (!stored) return null;
  // Return an owned Runtime copy. A Runtime may then transfer this copy to a worker
  // without detaching a Package Store read object that another caller may still
  // hold in the current task.
  if (stored.data instanceof ArrayBuffer) return stored.data.slice(0);
  if (stored.blob instanceof Blob) return stored.blob.arrayBuffer();
  return null;
}

async function readObjectBuffer(objectId: string, readObject: ReadObject, timeoutMs: number) {
  let timer: ReturnType<typeof setTimeout> | undefined;
  try {
    return await Promise.race([
      readObject(objectId).then(storedObjectBuffer),
      new Promise<never>((_resolve, reject) => {
        timer = setTimeout(() => reject(new Error("本地存储读取超时，请重试")), timeoutMs);
      }),
    ]);
  } finally {
    clearTimeout(timer);
  }
}

export async function readManagedRuntimeData(
  generation: InstalledPackageGeneration,
  { readObject = readPackageObject, timeoutMs = 30_000 }: ReadOptions = {},
) {
  const fileId = packageDataFileId(generation);
  const declaration = generation.descriptor.files[fileId];
  const fileRef = generation.files[fileId];
  if (!declaration || !fileRef?.objectId) throw new InstalledGameDataError("Installed game DATA is missing");
  const objectId = fileRef.objectId;
  const buffer = await readObjectBuffer(objectId, readObject, timeoutMs);
  if (!(buffer instanceof ArrayBuffer)) throw new InstalledGameDataError("Installed game DATA cannot be read");
  if (declaration.bytes != null && buffer.byteLength !== Number(declaration.bytes)) {
    throw new InstalledGameDataError(`Installed game DATA size mismatch: ${buffer.byteLength}/${declaration.bytes}`);
  }
  return { buffer, bytes: buffer.byteLength, fileId };
}

export async function readManagedRuntimeResource(
  generation: InstalledPackageGeneration,
  fileId: string,
  { readObject = readPackageObject, timeoutMs = 30_000 }: ReadOptions = {},
) {
  const declaration = generation?.descriptor?.files?.[fileId];
  const objectId = generation?.files?.[fileId]?.objectId;
  if (!declaration || !objectId) return null;
  const buffer = await readObjectBuffer(objectId, readObject, timeoutMs);
  if (!(buffer instanceof ArrayBuffer)) return null;
  if (declaration.bytes != null && buffer.byteLength !== Number(declaration.bytes)) {
    throw new Error(`${fileId}: installed resource size mismatch`);
  }
  return { buffer, bytes: buffer.byteLength, fileId, path: declaration.target };
}

export function managedRuntimeUrl(
  runtimeUrl: string,
  generation: InstalledPackageGeneration,
  runtimeVariant = "normal",
  baseUrl = globalThis.location?.href,
) {
  if (typeof runtimeUrl !== "string" || !runtimeUrl) throw new Error("Managed Runtime URL is unavailable");
  if (!generation?.id || !generation?.game) throw new Error("Package generation is unavailable");
  const url = new URL(runtimeUrl, baseUrl || "https://runtime.invalid/");
  url.searchParams.set("hosted", "1");
  url.searchParams.set("managedData", "1");
  url.searchParams.set("gameGeneration", generation.id);
  url.searchParams.set("runtimeVariant", runtimeVariant || "normal");
  return url.href;
}
