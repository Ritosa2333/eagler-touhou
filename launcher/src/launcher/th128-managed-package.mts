import type {InstalledPackageGeneration} from '../contracts/package-read-models.mts';
import type {readManagedRuntimeData, readManagedRuntimeResource} from './runtime-preparation.mts';

export const TH128_PACKAGE_VERSION = 'th128-dat-ogg/1';
const layout = 'sha256-58cd45fe7cac9af281d9de993a527ccfbbe6f27b89bcd0ca98a7031333fce200';
const dataId = 'game-data', maxFile = 128 * 1024 * 1024, maxPackage = 256 * 1024 * 1024;

export function th128MountedLanguage(generation: InstalledPackageGeneration | null) {
  return generation?.descriptor.components.language?.entries?.find(entry => generation.files[entry.file]?.objectId)?.id;
}

export function requireCompleteTh128Package(generation: InstalledPackageGeneration | null) {
  const descriptor = generation?.descriptor, requirement = descriptor?.runtimeRequirement;
  if (!generation?.id || String(generation.game) !== 'th128' || String(descriptor?.game) !== 'th128' ||
      descriptor?.schema !== 'eagler-touhou/package/1' || descriptor.runtime || descriptor.runtimes ||
      requirement?.target !== 'th128' || requirement.protocol !== 'eagler-touhou/1' || requirement.dataFile !== dataId ||
      requirement.dataLayout !== layout || descriptor.files[dataId]?.target !== '/th128.dat' || !descriptor.base.files.includes(dataId)) {
    throw new Error('TH128 needs its installed DAT + OGG resource package');
  }
  const ogg = descriptor.components.ogg?.files;
  const language = descriptor.components.language?.entries;
  const selected = language?.find(entry => generation.files[entry.file]?.objectId);
  if (Object.keys(descriptor.files).length > 100 || descriptor.components.ogg?.type !== 'ogg' || !ogg || ogg.length !== 10 || new Set(ogg).size !== 10 ||
      ogg.some(id => !/^\/music\/th128_\d\d\.ogg$/.test(descriptor.files[id]?.target ?? '')) ||
      descriptor.components.language?.type !== 'language' || !language || !selected || language.length > 3 ||
      new Set(language.map(entry => entry.id)).size !== language.length ||
      language.some(entry => !['ja', 'lang_zh-hans', 'lang_en'].includes(entry.id) ||
        descriptor.files[entry.file]?.target !== `/__eagler/language/${entry.id === 'ja' ? 'lang_ja' : entry.id}.zip`)) {
    throw new Error('TH128 requires its complete OGG and language files');
  }
  const excluded = new Set(language.filter(entry => entry !== selected).map(entry => entry.file));
  const files = Object.entries(descriptor.files).filter(([id]) => !excluded.has(id));
  let total = 0;
  for (const [id, declaration] of files) {
    const ref = Object.hasOwn(generation.files, id) ? generation.files[id] : undefined;
    if (!ref?.objectId || ref.revision !== declaration.revision) throw new Error(`TH128 package is missing installed file: ${id}`);
    const bytes = declaration.bytes;
    if (typeof bytes !== 'number' || !Number.isSafeInteger(bytes) || bytes < 1 || bytes > maxFile ||
        typeof declaration.sha256 !== 'string' || !/^[a-f0-9]{64}$/.test(declaration.sha256) || bytes > maxPackage - total) {
      throw new Error(`TH128 package has an invalid file receipt: ${id}`);
    }
    total += bytes;
  }
  if (!excluded.size) return descriptor;
  return {...descriptor, files: Object.fromEntries(files), components: {
    ...descriptor.components, language: {...descriptor.components.language, entries: [selected]},
  }};
}

export type Th128ManagedPackage = Readonly<{
  version: typeof TH128_PACKAGE_VERSION;
  generation: string;
  epoch: number;
  descriptor: ArrayBuffer;
  readFile(fileId: string): Promise<{fileId: string; path: string; buffer: ArrayBuffer; bytes: number}>;
}>;

export async function provideTh128ManagedPackage({generation, epoch, checkCurrent, readData, readResource}: {
  generation: InstalledPackageGeneration;
  epoch: number;
  checkCurrent(): void;
  readData: typeof readManagedRuntimeData;
  readResource: typeof readManagedRuntimeResource;
}) {
  checkCurrent();
  const descriptor = requireCompleteTh128Package(generation);
  const descriptorBytes = new TextEncoder().encode(JSON.stringify(descriptor));
  if (descriptorBytes.byteLength > 1024 * 1024) throw new Error('TH128 package descriptor exceeds its size budget');
  const read = await readData(generation, {preferBlob: false}); checkCurrent();
  if (!('buffer' in read) || !(read.buffer instanceof ArrayBuffer) || read.fileId !== dataId ||
      read.bytes !== descriptor.files[dataId].bytes || read.buffer.byteLength !== read.bytes) {
    throw new Error('TH128 package DATA receipt mismatch');
  }
  const managedPackage: Th128ManagedPackage = Object.freeze({
    version: TH128_PACKAGE_VERSION, generation: generation.id, epoch, descriptor: descriptorBytes.buffer,
    async readFile(fileId: string) {
      checkCurrent();
      if (typeof fileId !== 'string' || fileId === dataId || !Object.hasOwn(descriptor.files, fileId)) {
        throw new Error('Requested TH128 package resource is not declared');
      }
      const row = descriptor.files[fileId];
      let receipt: Awaited<ReturnType<typeof readManagedRuntimeResource>>;
      try {receipt = await readResource(generation, fileId);} finally {checkCurrent();}
      if (!receipt || receipt.fileId !== fileId || receipt.path !== row.target || receipt.bytes !== row.bytes ||
          !(receipt.buffer instanceof ArrayBuffer) || receipt.buffer.byteLength !== receipt.bytes) {
        throw new Error(`TH128 package resource receipt mismatch: ${fileId}`);
      }
      return receipt;
    },
  });
  return {...read, managedPackage};
}
