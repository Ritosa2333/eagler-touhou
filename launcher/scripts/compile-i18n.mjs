import {ensureI18nBuild} from '../lib/i18n-build.mjs';
import {buildAuthoredContent} from '../lib/content-build.mjs';

await buildAuthoredContent();

const result = await ensureI18nBuild({force: process.argv.includes('--force')});
console.log(`UI messages: ${result.built ? 'compiled' : 'current'}`);
