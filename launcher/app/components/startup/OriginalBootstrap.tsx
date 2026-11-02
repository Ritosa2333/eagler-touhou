import backgroundUrl from '../../../public/assets/launcher-background.webp?url';
import touhouFontUrl from '../../../public/assets/fonts/touhou98.woff2?url';
import yatraFontUrl from '../../../public/assets/fonts/yatra-one-latin.woff2?url';
import mediumFontUrl from '../../../public/assets/fonts/chill-round-gothic-site-medium-critical.woff2?url';
import boldFontUrl from '../../../public/assets/fonts/chill-round-gothic-site-bold-critical.woff2?url';
import {originalBootstrap as original} from 'virtual:original-bootstrap';
import {useLocation} from 'react-router';
import {translate} from '../../i18n';
import {useDocumentUiLocale} from '../../navigation/document-locale';

// ES5 dispatch only: the original compatibility gate must precede the modern
// bundle. Static SPA fallback HTML serves both original document contexts.
const dispatch = `(function(){var lobby=/\\/(?:lobby|lobby\\.html)\\/?$/.test(location.pathname);document.documentElement.setAttribute('data-original-entry',lobby?'lobby':'library');if(lobby){document.documentElement.setAttribute('data-lobby-boot','loading');}else{${original.library.compatibility}\n${original.library.script}\n}}());`;
const tail = `(function(){var module=document.querySelector('script[type="module"][data-eagler-entry]');if(module)module.id='lobbyModule';if(document.documentElement.getAttribute('data-original-entry')==='lobby'){${original.lobby.script}\n}}());`;
// One authored loading presentation for both document entries and warm routes.
// Keep the lobby's failure carrier, while deriving its motion from Launcher.
const sharedLobbyPreloadCss = original.library.css.replaceAll('#eaglerPreload', '#lobbyPreload').replaceAll('eagler-preload-', 'lobby-preload-') +
  '\n.lobby-preload-card{padding:0;gap:normal}html[data-lobby-boot="failed"] .lobby-preload-card{width:min(420px,calc(100vw - 48px));height:auto;min-height:132px;padding:24px;gap:18px;animation:none}';
/** Source-extracted original pre-module safety gate and preload presentation. */
export function OriginalBootstrapHead() {
  const {pathname} = useLocation();
  const locale = useDocumentUiLocale();
  const lobby = /\/(?:lobby|lobby\.html)\/?$/.test(pathname);
  const account = /\/login\.html\/?$/.test(pathname);
  const profile = /\/profile\.html\/?$/.test(pathname);
  const title = account ? translate(locale, 'account.documentTitle') : lobby ? `${translate(locale, 'lobby.title')} ~ EAGLER TOUHOU` : profile ? `${translate(locale, 'profile.navigation')} ~ EAGLER TOUHOU` : translate(locale, 'site.documentTitle');
  const description = translate(locale, 'site.description');
  return <>
    <script>{dispatch}</script>
    <style>{original.library.css + '\n' + original.lobby.css + '\n' + sharedLobbyPreloadCss +
      '\nhtml[data-original-entry="lobby"] #eaglerPreload,html:not([data-original-entry="lobby"]) #lobbyPreload{display:none}'}</style>
    <style id="launcherNavigationTransition">{'@view-transition{navigation:auto}'}</style>
    <style id="bookStartupTheme">{original.theme+`
      html{background:var(--kit-background)}
      body:not(.mp-room-active){background-color:var(--kit-background)}
      :is(#eaglerPreload,#lobbyPreload){background:var(--kit-background);color:var(--kit-foreground);font-family:var(--kit-font)}
      :is(.eagler-preload-card,.lobby-preload-card){width:72px;height:72px;border-radius:12px;background:var(--kit-card);border-color:var(--kit-border);box-shadow:none}
      :is(.eagler-preload-spinner,.lobby-preload-spinner){border-color:var(--kit-border);border-top-color:var(--kit-brand)}
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
      <meta property="og:title" content={title} data-i18n-content="site.documentTitle"/>
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
    <link rel="preload" href={backgroundUrl} as="image" fetchPriority="high"/>
    <link rel="preload" href={touhouFontUrl} as="font" type="font/woff2" crossOrigin="anonymous"/>
    <link rel="preload" href={yatraFontUrl} as="font" type="font/woff2" crossOrigin="anonymous"/>
    <link rel="preload" href={mediumFontUrl} as="font" type="font/woff2" crossOrigin="anonymous"/>
    <link rel="preload" href={boldFontUrl} as="font" type="font/woff2" crossOrigin="anonymous"/>
  </>;
}
export function OriginalPreloads() {
  // The original boot owns cold entry. Templates retain that same authored cue
  // for later same-document page entries without restarting its watchdog.
  // biome-ignore lint/security/noDangerouslySetInnerHtml: Build-extracted repository HTML supplies the cold boot and inert templates; request and user data never enter it.
  return <div style={{display: 'contents'}} data-surface-preload-host="" dangerouslySetInnerHTML={{__html:
    original.library.preload + original.lobby.preload +
    `<template data-surface-preload="library">${original.library.preload}</template>` +
    `<template data-surface-preload="lobby">${original.lobby.preload}</template>`}}/>;
}
export function OriginalModuleRequestMark() {
  return <script>{'window.__eaglerBoot && window.__eaglerBoot.mark("app-module-request");'}</script>;
}
export function OriginalBootstrapTail() {return <script>{tail}</script>;}
