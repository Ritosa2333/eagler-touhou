import {existsSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'..');
export const commonRoot=resolve(process.env.EAGLER_COMMON_ROOT||resolve(root,'../../common'));
if(!existsSync(resolve(commonRoot,'include/eagler/thprac/PracticeInput.hpp')))throw Error('Shared common source is missing; set EAGLER_COMMON_ROOT for an explicit diagnostic override.');
export const commonInclude=resolve(commonRoot,'include');
export const commonThpracHeaders=readdirSync(resolve(commonInclude,'eagler/thprac')).filter(p=>/\.(hpp|inc)$/.test(p)).map(p=>resolve(commonInclude,'eagler/thprac',p)).sort();
