import type { PackageDescriptor } from "../src/contracts/package-read-models.mjs";

export interface ParsedPackageZipFile {
  blob: Blob;
  [key: string]: unknown;
}

export interface ParsedPackageZip {
  descriptor: PackageDescriptor;
  files: Map<string, ParsedPackageZipFile>;
}

export function parsePackageZip(blob: Blob): Promise<ParsedPackageZip>;
