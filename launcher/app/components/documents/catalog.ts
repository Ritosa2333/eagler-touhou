import type {UiMessageKey} from '../../i18n.tsx';

export const helpDocuments = [
  {id:'firstUse', path:'guide.html', title:'firstUseNotice.title', content:'content.firstUse',source:'content/FIRST_USE_NOTICE.md'},
  {id:'faq', path:'faq.html', title:'nav.faq', content:'content.faq',source:'docs/FAQ.md'},
  {id:'multiplayer', path:'multiplayer.html', title:'multiplayerGuide.title', content:'content.multiplayer',source:'content/MULTIPLAYER.md'},
  {id:'about', path:'about.html', title:'nav.about', content:'content.about',source:'content/ABOUT.html'},
] as const satisfies readonly {id:string;path:string;title:UiMessageKey;content:UiMessageKey;source:string}[];
export type HelpDocument = typeof helpDocuments[number];
export function helpDocumentAtPath(pathname:string) {
  return helpDocuments.find(document=>pathname.replace(/\/$/,'').endsWith('/'+document.path));
}
export const isStandalonePage=(pathname:string)=>/\/uptime(?:\/(?:index\.html)?)?$/.test(pathname)||!!helpDocumentAtPath(pathname);
