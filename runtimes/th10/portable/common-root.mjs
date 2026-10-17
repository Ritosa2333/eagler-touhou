import {existsSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'..');
const pinned=resolve(root,'third_party/eagler-common');
// Never silently bypass an initialized, stale release dependency.
const candidates=process.env.EAGLER_COMMON_ROOT?[resolve(process.env.EAGLER_COMMON_ROOT)]:existsSync(resolve(pinned,'CMakeLists.txt'))?[pinned]:[resolve(root,'../eagler-common'),resolve(root,'../../eagler-common')];
export const commonRoot=candidates.find(p=>existsSync(resolve(p,'include/eagler/thprac/PracticeInput.hpp')));
if(!commonRoot)throw Error('Purple THPrac needs the updated eagler-common; set EAGLER_COMMON_ROOT for an unpublished local build.');
export const commonInclude=resolve(commonRoot,'include');
export const commonThpracHeaders=readdirSync(resolve(commonInclude,'eagler/thprac')).filter(p=>/\.(hpp|inc)$/.test(p)).map(p=>resolve(commonInclude,'eagler/thprac',p)).sort();
