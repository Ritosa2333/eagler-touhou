import backgroundUrl from '../../../public/assets/launcher-background.webp?url';
import touhouFontUrl from '../../../public/assets/fonts/touhou98.woff2?url';
import yatraFontUrl from '../../../public/assets/fonts/yatra-one-latin.woff2?url';
import mediumFontUrl from '../../../public/assets/fonts/chill-round-gothic-site-medium-critical.woff2?url';
import boldFontUrl from '../../../public/assets/fonts/chill-round-gothic-site-bold-critical.woff2?url';
import {originalBootstrap as original} from 'virtual:original-bootstrap';
import {useHref, useLocation} from 'react-router';
import {UI_LOCALES} from '../../../src/launcher/i18n.mts';
import {translate} from '../../i18n';
import {LESS_MOTION_STORAGE_KEY} from '../../models/site-preferences';
import {sitePageTitle, useDocumentUiLocale} from '../../navigation/document-locale';
import {isLauncherHomePath} from '../../navigation/addresses';
import {chooseLoadingIndex, LoadingActivityContent, loadingTips, startLoadingTipRotation} from '../../ui/loading-activity';
import {LoadingScene, loadingSceneOptions, repeatStingerDelay} from '../../ui/loading-scene';
import loadingActivityCss from '../../ui/loading-activity.css?inline';
import loadingSceneCss from '../../ui/loading-scene.css?inline';
import loadingVignettesCss from '../../ui/loading-vignettes.css?inline';

// ES5 dispatch only: the original compatibility gate must precede the modern
// bundle. Static SPA fallback HTML serves both original document contexts.
const backgroundWait = 'var sources = ["assets/launcher-background.webp"];';
if (!original.library.script.includes(backgroundWait)) throw new Error('Original launcher bootstrap background wait anchor changed');
const libraryScript = original.library.script.replace(backgroundWait,
  'var sources = document.documentElement.hasAttribute("data-ui-app") && document.documentElement.hasAttribute("data-ui-home") ? [] : ["assets/launcher-background.webp"];');
const dispatch = `(function(){var lobby=/\\/(?:lobby|lobby\\.html)\\/?$/.test(location.pathname);document.documentElement.setAttribute('data-original-entry',lobby?'lobby':'library');if(lobby){document.documentElement.setAttribute('data-lobby-boot','loading');}else{${original.library.compatibility}\n${libraryScript}\n}}());`;
const tail = `(function(){var module=document.querySelector('script[type="module"][data-eagler-entry]');if(module)module.id='lobbyModule';if(document.documentElement.getAttribute('data-original-entry')==='lobby'){${original.lobby.script}\n}}());`;
// One authored loading presentation for both document entries and warm routes.
// Keep the lobby's failure carrier, while deriving its motion from Launcher.
const sharedLobbyPreloadCss = original.library.css.replaceAll('#eaglerPreload', '#lobbyPreload').replaceAll('eagler-preload-', 'lobby-preload-') +
  '\n.lobby-preload-card{padding:0;gap:normal}';

const bootLoadingContexts = ['solo', 'multiplayer'] as const;
const bootLoadingCopy = Object.fromEntries(UI_LOCALES.map(locale => [locale, {
  label: translate(locale, 'player.loading').replace(/(?:…|\.{3})$/, ''),
  tips: Object.fromEntries(bootLoadingContexts.map(context => [context, loadingTips[context].map(key => translate(locale, key))])),
}]));
function showBootstrapLoading(copy: typeof bootLoadingCopy, motionKey: string,
  choose: typeof chooseLoadingIndex, rotate: typeof startLoadingTipRotation, nextStingerDelay: typeof repeatStingerDelay) {
  const lobby = document.documentElement.getAttribute('data-original-entry') === 'lobby';
  const context = lobby ? 'multiplayer' : 'solo';
  const prefix = lobby ? 'lobby' : 'eagler';
  const preload = document.getElementById(prefix + 'Preload')!;
  const slot = preload.querySelector('.' + prefix + '-preload-spinner')!;
  const template = document.querySelector('[data-bootstrap-loading]')!;
  const loading = template.firstElementChild!.cloneNode(true) as HTMLElement;
  const scenes = template.querySelector('[data-bootstrap-scenes]')!.children;
  const artwork = scenes[choose(scenes.length)].cloneNode(true) as HTMLElement;
  const locale = new URLSearchParams(location.search).get('lang');
  const text = copy[locale && Object.hasOwn(copy, locale) ? locale : document.documentElement.lang];
  const tips = text.tips[context];
  const tipIndex = choose(tips.length);
  loading.setAttribute('data-loading-context', context);
  for (const image of artwork.querySelectorAll<HTMLImageElement>('img[data-loading-src]')) {
    image.src = image.getAttribute('data-loading-src')!;
    image.removeAttribute('data-loading-src');
  }
  loading.querySelector('[data-bootstrap-artwork]')!.replaceWith(...artwork.children);
  loading.querySelector('.loading-activity-status > span')!.textContent = text.label;
  const tip = loading.querySelector('.loading-activity-tip > span')!;
  tip.textContent = tips[tipIndex];
  try {if (localStorage.getItem(motionKey) === '1') loading.setAttribute('data-still', '');} catch {}
  slot.replaceWith(loading);
  const isLoading = () => loading.isConnected && !preload.classList.contains('is-done') &&
    !(lobby && document.documentElement.getAttribute('data-lobby-boot') === 'failed');
  rotate(tips.length, tipIndex, index => {
    if (!isLoading()) return false;
    tip.textContent = tips[index];
  });
  loading.addEventListener('animationend', event => {
    if (event.animationName !== 'loading-stinger-window' || !isLoading()) return;
    const previous = event.target as HTMLElement;
    const next = previous.cloneNode(true) as HTMLElement;
    next.style.setProperty('--stinger-delay', nextStingerDelay() + 'ms');
    previous.replaceWith(next);
  });
}
const bootLoadingScript = `(${showBootstrapLoading.toString()})(${JSON.stringify(bootLoadingCopy)},${JSON.stringify(LESS_MOTION_STORAGE_KEY)},${chooseLoadingIndex.toString()},${startLoadingTipRotation.toString()},${repeatStingerDelay.toString()});`;
/** Source-extracted original pre-module safety gate and preload presentation. */
export function OriginalBootstrapHead() {
  const {pathname, search} = useLocation();
  const locale = useDocumentUiLocale();
  const lobby = /\/(?:lobby|lobby\.html)\/?$/.test(pathname);
  const account = /\/login\.html\/?$/.test(pathname);
  const profile = /^(?:\/profile\.html|\/u\/[^/]+)\/?$/.test(pathname);
  const home = isLauncherHomePath(pathname);
  const mode = account ? new URLSearchParams(search).get('mode') : null;
  const page = account ? mode === 'register' ? 'account.register' : mode === 'reset' ? 'account.forgotPassword' : 'account.login'
    : lobby ? 'lobby.title' : profile ? 'profile.pageTitle' : null;
  const title = page ? sitePageTitle(translate(locale, page)) : translate(locale, 'site.documentTitle');
  const description = translate(locale, 'site.description');
  return <>
    <script>{dispatch}</script>
    <style>{original.library.css + '\n' + original.lobby.css + '\n' + sharedLobbyPreloadCss +
      '\nhtml[data-original-entry="lobby"] #eaglerPreload,html:not([data-original-entry="lobby"]) #lobbyPreload{display:none}'}</style>
    <style>{loadingActivityCss + '\n' + loadingSceneCss + '\n' + loadingVignettesCss}</style>
    <style id="bookStartupTheme">{original.theme+`
      html{background:var(--kit-background)}
      body:not(.mp-room-active){background-color:var(--kit-background)}
      :is(#eaglerPreload,#lobbyPreload){background:var(--kit-background);color:var(--kit-foreground);font-family:var(--kit-font)}
      :is(#eaglerPreload,#lobbyPreload).is-done{pointer-events:none}
      :is(.eagler-preload-card,.lobby-preload-card){width:72px;height:72px;background:transparent;border:0;box-shadow:none}
      :is(.eagler-preload-spinner,.lobby-preload-spinner){border-color:var(--kit-border);border-top-color:var(--kit-brand)}
      :is(.eagler-preload-card,.lobby-preload-card):has(>.loading-activity){width:min(510px,calc(100vw - 32px));height:auto;padding:12px;box-sizing:border-box;animation:none}
      html[data-lobby-boot="failed"] .lobby-preload-card{width:min(420px,calc(100vw - 48px));height:auto;min-height:132px;padding:24px;gap:18px;animation:none;border:1px solid var(--kit-border);border-radius:12px;background:var(--kit-card)}
      html[data-lobby-boot="failed"] .lobby-preload-card > .loading-activity{display:none}
      .lobby-preload-failure a{color:var(--kit-foreground)}
      #eaglerBootEmergency>div{border-radius:12px!important;background:var(--kit-card)!important;color:var(--kit-foreground)!important;border-color:var(--kit-border)!important;box-shadow:0 16px 48px #0003!important;font-family:var(--kit-font)!important}
      #eaglerBootEmergency :is(strong,pre){color:var(--kit-foreground)!important}
      #eaglerBootEmergency button{border-radius:6px!important;background:var(--kit-secondary)!important;color:var(--kit-foreground)!important}
      #eaglerBootEmergency button:first-of-type{background:var(--kit-primary)!important;color:var(--kit-primary-foreground)!important}
    `}</style>
    <title data-i18n={lobby || profile || account ? undefined : 'site.documentTitle'}>{title}</title>
    <meta name="theme-color" content={lobby ? '#111210' : '#10100f'}/>
    {lobby ? <meta name="robots" content="noindex"/> : <>
      <meta name="color-scheme" content="dark"/>
      <meta name="description" content={description} data-i18n-content="site.description"/>
      <meta property="og:type" content="website"/>
      <meta property="og:site_name" content="EAGLER TOUHOU"/>
      <meta property="og:title" content={title} data-i18n-content={page ? undefined : 'site.documentTitle'}/>
      <meta property="og:description" content={description} data-i18n-content="site.description"/>
      <meta name="mobile-web-app-capable" content="yes"/>
      <meta name="apple-mobile-web-app-capable" content="yes"/>
      <meta name="apple-mobile-web-app-title" content="EAGLER TOUHOU"/>
      <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent"/>
      <link rel="manifest" href="site.webmanifest"/>
      <link rel="apple-touch-icon" sizes="180x180" href="assets/pwa/apple-touch-icon.png"/>
    </>}
    <link rel="icon" href={lobby ? 'favicon.ico' : 'assets/favicon.ico'} type={lobby ? undefined : 'image/x-icon'}/>
    <link rel="icon" type="image/png" sizes="512x512" href="assets/brand/portrait.png"/>
    {!home && <link rel="preload" href={backgroundUrl} as="image" fetchPriority="high"/>}
    <link rel="preload" href={touhouFontUrl} as="font" type="font/woff2" crossOrigin="anonymous"/>
    <link rel="preload" href={yatraFontUrl} as="font" type="font/woff2" crossOrigin="anonymous"/>
    <link rel="preload" href={mediumFontUrl} as="font" type="font/woff2" crossOrigin="anonymous"/>
    <link rel="preload" href={boldFontUrl} as="font" type="font/woff2" crossOrigin="anonymous"/>
  </>;
}
export function OriginalPreloads() {
  const base = useHref('/');
  // The original boot owns reveal and failure. The parser fills its indicator
  // with the same markup that React uses for waits after startup.
  const preload = original.library.preload + original.lobby.preload;
  return <>
    {/* biome-ignore lint/security/noDangerouslySetInnerHtml: Build-extracted repository HTML, with no request or user data. */}
    <div style={{display: 'contents'}} data-surface-preload-host="" suppressHydrationWarning dangerouslySetInnerHTML={{__html: preload}}/>
    <div hidden data-bootstrap-loading="">
      <LoadingActivityContent artwork={<span data-bootstrap-artwork=""/>} label="" tip="" dots/>
      <div data-bootstrap-scenes="">
        {loadingSceneOptions.map(({id}) => <div key={id}><LoadingScene scene={id} assetUrl={path => base + path} deferred/></div>)}
      </div>
    </div>
    <script suppressHydrationWarning>{bootLoadingScript}</script>
  </>;
}
export function OriginalModuleRequestMark() {
  return <script>{'window.__eaglerBoot && window.__eaglerBoot.mark("app-module-request");'}</script>;
}
export function OriginalBootstrapTail() {return <script>{tail}</script>;}
