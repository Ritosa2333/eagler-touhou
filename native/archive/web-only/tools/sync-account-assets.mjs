import {copyFile, mkdir} from 'node:fs/promises';

const assets = new URL('../assets/', import.meta.url);
const launcher = new URL('../../launcher/', import.meta.url);
await mkdir(new URL('documents/', assets), {recursive: true});
for (const [from, to] of [
  ['content/TERMS_OF_SERVICE.zh-CN.md', 'documents/terms.zh-CN.md'],
  ['content/TERMS_OF_SERVICE.md', 'documents/terms.en.md'],
  ['content/TERMS_OF_SERVICE.ja.md', 'documents/terms.ja.md'],
  ['content/PRIVACY_POLICY.md', 'documents/privacy.zh-CN.md'],
  ['content/PRIVACY_POLICY.en.md', 'documents/privacy.en.md'],
  ['content/PRIVACY_POLICY.ja.md', 'documents/privacy.ja.md'],
  ['public/assets/launcher-background.webp', 'account-background.webp'],
]) {
  await copyFile(new URL(from, launcher), new URL(to, assets));
}

