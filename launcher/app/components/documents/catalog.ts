import type {UiMessageKey} from '../../i18n.tsx';
import {multiplayerGuideTopic} from '../../../lib/multiplayer-guide.mjs';
import {TH09_RATINGS_DOCUMENT} from '../../../lib/frontend-static-manifest.mjs';

export const helpDocuments = [
  {id:'firstUse', path:'guide.html', title:'firstUseNotice.title', content:'content.firstUse',source:'content/FIRST_USE_NOTICE.md'},
  {id:'faq', path:'faq.html', title:'nav.faq', content:'content.faq',source:'docs/FAQ.md'},
  {id:'multiplayer', path:'multiplayer.html', title:'multiplayerGuide.title', content:'content.multiplayer',source:'content/MULTIPLAYER.md'},
  {id:'terms', path:'terms.html', title:'document.terms', content:'content.terms',source:'content/TERMS_OF_SERVICE.zh-CN.md'},
  {id:'privacy', path:'privacy.html', title:'document.privacy', content:'content.privacy', source:'content/PRIVACY_POLICY.md'},
  {id:'about', path:'about.html', title:'nav.about', content:'content.about',source:'content/ABOUT.html'},
] as const satisfies readonly {id:string;path:string;title:UiMessageKey;content:UiMessageKey;source:string}[];
export type HelpDocument = typeof helpDocuments[number];
export function helpDocumentAtPath(pathname:string) {
  if (multiplayerGuideTopic(pathname)) return helpDocuments[2];
  return helpDocuments.find(document=>pathname.replace(/\/$/,'').endsWith('/'+document.path));
}
export const isTh09RatingPage=(pathname:string)=>pathname.replace(/\/$/,'').endsWith(TH09_RATINGS_DOCUMENT.path);
export const isStandalonePage=(pathname:string)=>isTh09RatingPage(pathname)||/\/uptime(?:\/(?:index\.html)?)?$/.test(pathname)||!!helpDocumentAtPath(pathname);
