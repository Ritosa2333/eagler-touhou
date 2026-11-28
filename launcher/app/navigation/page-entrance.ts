import {LESS_MOTION_STORAGE_KEY} from '../models/site-preferences';

export const pageEntranceOpacity = '.28';
export const pageEntranceTiming = {duration: 240, ease: [.25, .1, .25, 1] as [number, number, number, number]};
export const launcherPageContent: Partial<Record<string, string>> = {
  library: '.book-content', lobby: '.lobby-rooms-column', profile: '.profile-wrap', account: '.account-content',
  installs: '.game-install-page',
};
export const documentPageContent = '#nd-page';

const marker = 'data-ui-document-entry';
const animation = 'ui-page-entrance';
const root = `html[${marker}]:not(:has(body.less-motion))`;

// Full-document navigation starts this same entrance at first paint, before hydration.
export const documentEntranceCss = `
@keyframes ${animation}{from{opacity:${pageEntranceOpacity}}to{opacity:1}}
@media(prefers-reduced-motion:no-preference){
  ${root} :is(.standalone-page-shell > main,.documentation-shell ${documentPageContent}),
  ${root}[data-original-entry="library"]:not(:has(#eaglerPreload:not(.is-done))) :is(${Object.values(launcherPageContent).join(',')}),
  ${root}[data-original-entry="lobby"]:not(:has(#lobbyPreload:not(.is-done))) .lobby-rooms-column{
    animation:${animation} ${pageEntranceTiming.duration}ms cubic-bezier(${pageEntranceTiming.ease.join(',')}) both
  }
}`;

export const documentEntranceScript = `(function(){
  if(matchMedia('(prefers-reduced-motion: reduce)').matches)return;
  var nav=performance.getEntriesByType('navigation')[0], from=document.referrer;
  if(nav&&nav.type==='reload')return;
  if(!(nav&&nav.type==='back_forward')&&(!from||new URL(from).origin!==location.origin))return;
  try{if(localStorage.getItem(${JSON.stringify(LESS_MOTION_STORAGE_KEY)})==='1')return;}catch(e){}
  var root=document.documentElement;
  root.setAttribute('${marker}','');
  function finish(event){
    if(event.animationName!=='${animation}')return;
    root.removeAttribute('${marker}');
    document.removeEventListener('animationend',finish);
  }
  document.addEventListener('animationend',finish);
})();`;

export function clearDocumentEntrance(doc: Document) {
  doc.documentElement.removeAttribute(marker);
}
