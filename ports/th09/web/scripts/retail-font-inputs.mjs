import { existsSync, statSync } from 'node:fs';
import { resolve } from 'node:path';

export const retailAssetUsage = `Usage: node ports/th09/web/scripts/prepare-retail-assets.mjs ORIGINAL_GAME_DIRECTORY [--font-root DIR]

Font directory must contain blend.bin (131072 bytes), msgothic.ttc, and either
cp932.bin (131072 bytes) or codepages.bin (262144 bytes). Set --font-root or
EAGLER_FONT_ROOT. With neither set, the legacy TH10 asset directory is used.`;

export function parseRetailAssetArgs(args, env, legacyFontRoot) {
  let original;
  let fontRoot;
  let help = false;
  for (let i = 0; i < args.length; i += 1) {
    const value = args[i];
    if (value === '--help' || value === '-h') {
      help = true;
    } else if (value === '--font-root') {
      if (!args[i + 1] || args[i + 1].startsWith('--')) throw Error('Missing value for --font-root');
      fontRoot = args[++i];
    } else if (value.startsWith('--font-root=')) {
      fontRoot = value.slice('--font-root='.length);
      if (!fontRoot) throw Error('Missing value for --font-root');
    } else if (value.startsWith('-')) {
      throw Error(`Unknown option: ${value}`);
    } else if (original === undefined) {
      original = value;
    } else {
      throw Error(`Unexpected argument: ${value}`);
    }
  }
  return {
    original,
    help,
    fontRoot: resolve(fontRoot || env.EAGLER_FONT_ROOT || legacyFontRoot),
  };
}

export function resolveRetailFontInputs(fontRoot) {
  const root = resolve(fontRoot);
  const cp932Path = resolve(root, 'cp932.bin');
  const codepagesPath = resolve(root, 'codepages.bin');
  const blendPath = resolve(root, 'blend.bin');
  const msgothicPath = resolve(root, 'msgothic.ttc');
  const missing = [];
  const cp932Exists = existsSync(cp932Path);
  const codepagesExists = existsSync(codepagesPath);
  if (!cp932Exists && !codepagesExists) missing.push('cp932.bin or codepages.bin (one is required)');
  if (!existsSync(blendPath)) missing.push('blend.bin');
  if (!existsSync(msgothicPath)) missing.push('msgothic.ttc');
  if (missing.length) {
    throw Error(`Missing TH09 font input(s) in ${root}: ${missing.join(', ')}. Set --font-root DIR or EAGLER_FONT_ROOT to a directory containing these files.`);
  }

  const codepagePath = cp932Exists ? cp932Path : codepagesPath;
  const codepageName = cp932Exists ? 'cp932.bin' : 'codepages.bin';
  const codepageBytes = statSync(codepagePath).size;
  const expectedCodepageBytes = cp932Exists ? 131072 : 262144;
  if (codepageBytes !== expectedCodepageBytes) {
    throw Error(`Invalid TH09 font input ${codepagePath}: ${codepageName} must be ${expectedCodepageBytes} bytes; found ${codepageBytes}.`);
  }
  const blendBytes = statSync(blendPath).size;
  if (blendBytes !== 131072) {
    throw Error(`Invalid TH09 font input ${blendPath}: blend.bin must be 131072 bytes; found ${blendBytes}.`);
  }
  if (statSync(msgothicPath).size === 0) throw Error(`Invalid TH09 font input ${msgothicPath}: msgothic.ttc is empty.`);

  return { root, codepagePath, codepageName, blendPath, msgothicPath };
}
