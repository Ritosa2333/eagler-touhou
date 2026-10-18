import {readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {loadCompiledContract} from '../lib/contracts/load-compiled-contract.mjs';
import {SITE_MANIFEST_FILE} from '../lib/contracts/site-manifest.mjs';
const contract = await loadCompiledContract('compat/host-manifest');
export const {LEGACY_HOST_MANIFEST_FILE, LEGACY_HOST_MANIFEST_SCHEMA,
  readCompatibleSiteManifest, legacyHostManifestResponse} = contract;

export function normalizeSiteManifestArguments(args) {
  const legacy = args['host-manifest'], current = args['site-manifest'];
  if (legacy && current && resolve(legacy) !== resolve(current)) throw new Error('Only one site manifest input may be supplied');
  if (legacy && !current) args['site-manifest'] = legacy;
  delete args['host-manifest'];
  return args;
}
export async function readSiteManifestFile(root) {
  try {return readCompatibleSiteManifest(JSON.parse(await readFile(resolve(root, SITE_MANIFEST_FILE), 'utf8')));}
  catch (error) {
    if (error?.code !== 'ENOENT') throw error;
    return readCompatibleSiteManifest(JSON.parse(await readFile(resolve(root, LEGACY_HOST_MANIFEST_FILE), 'utf8')));
  }
}
export async function writeLegacySiteManifestAlias(root, manifest) {
  await writeFile(resolve(root, LEGACY_HOST_MANIFEST_FILE), JSON.stringify(legacyHostManifestResponse(manifest), null, 2) + '\n');
}
export async function verifyLegacySiteManifestAlias(root, manifest) {
  let legacy;
  try {legacy = JSON.parse(await readFile(resolve(root, LEGACY_HOST_MANIFEST_FILE), 'utf8'));}
  catch (error) {if (error?.code === 'ENOENT') return; throw error;}
  if (JSON.stringify(readCompatibleSiteManifest(legacy)) !== JSON.stringify(readCompatibleSiteManifest(manifest))) {
    throw new Error('Legacy site manifest alias differs from the authoritative site manifest');
  }
}
