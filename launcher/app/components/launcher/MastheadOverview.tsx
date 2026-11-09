import {useLayoutEffect, useRef, type MouseEvent, type ReactElement, type ReactNode} from 'react';
import {Collapsible} from 'radix-ui';
import {Activity, Check, ChevronDown, Code, FlaskConical, Heart, Info, Languages, PlaySquare, X} from 'lucide-react';
import {useLocale} from '../../i18n';
import {Button} from '../../ui/button';
import {Dialog, DialogClose, DialogContent, DialogTitle, DialogTrigger} from '../../ui/dialog';
import {reloadTestFeatures} from '../../../src/launcher/test-features.mts';
import {previewFeaturesEnabled} from '../../release-features';
import {AppLink} from './AppLink';
import {BookThemeToggle} from './BookThemeToggle';
import {MastheadTooltip} from './MastheadTooltip';
import type {LauncherMastheadProps} from './LauncherMasthead';
import {NavigationIcon} from './NavigationIcon';

export interface MastheadSiteLink {
  href: string;
  icon: ReactNode;
  label: string;
  external?: boolean;
  policy?: string;
}
type OverviewAction = 'onDonation' | 'onFirstUse' | 'onReplay';

export function MastheadOverview({host, open, onOpenChange, trigger, lobbyHref, currentPage, helpLinks, statusLink, aboutLinks, testFeatures}: {
  host: LauncherMastheadProps;
  open: boolean;
  onOpenChange(open: boolean): void;
  trigger: ReactElement;
  lobbyHref: string;
  currentPage?: 'library' | 'lobby';
  helpLinks: readonly MastheadSiteLink[];
  statusLink: MastheadSiteLink;
  aboutLinks: readonly MastheadSiteLink[];
  testFeatures: boolean;
}) {
  const {t} = useLocale();
  const afterClose = useRef<OverviewAction | null>(null);
  const currentHost = useRef(host);
  useLayoutEffect(() => {currentHost.current = host;}, [host]);
  const replay = host.variant === 'lobby' ? host.onReplay : undefined;
  function closeThen(action: OverviewAction) {
    if (afterClose.current) return;
    afterClose.current = action;
    onOpenChange(false);
  }
  function follow(event: MouseEvent<HTMLAnchorElement>, native: boolean) {
    if (event.defaultPrevented || event.button !== 0 || event.altKey || event.ctrlKey || event.metaKey || event.shiftKey) return;
    const {href, target} = event.currentTarget;
    if (target === '_blank') onOpenChange(false);
    else if (href === window.location.href) {
      event.preventDefault();
      onOpenChange(false);
    } else if (native) {
      event.preventDefault();
      window.location.replace(href);
    }
  }
  function mainLink(href: string, current: boolean, label: string, icon: ReactNode) {
    const props = {href, 'aria-current': current ? 'page' as const : undefined, children: <>{icon}<span>{label}</span></>};
    return <Button asChild variant="ghost" size="row">{host.nativeLinks
      ? <a {...props} onClick={event => follow(event, true)}/>
      : <AppLink {...props} replace onClick={event => follow(event, false)}/>}
    </Button>;
  }
  function overviewLinks(links: readonly MastheadSiteLink[]) {
    return links.map(({href, icon, label, external, policy}) => <Button key={href} asChild variant="ghost" size="row">
      <a href={href} data-policy={policy} target={external ? '_blank' : undefined} rel={external ? 'noopener noreferrer' : undefined}
        onClick={event => follow(event, true)}>{icon}<span>{label}</span></a>
    </Button>);
  }
  return <Dialog open={open} onOpenChange={next => {
    if (next) afterClose.current = null;
    onOpenChange(next);
  }}>
    <MastheadTooltip label={t('nav.more')}><DialogTrigger asChild>{trigger}</DialogTrigger></MastheadTooltip>
    <DialogContent id="mastheadMenuPanel" className="masthead-overview" overlayClassName="masthead-overview-overlay" aria-describedby={undefined} showClose={false}
      onInteractOutside={event => {
        if (!open || (event.target instanceof Element && event.target.closest('#mastheadMenuToggle'))) event.preventDefault();
      }} onCloseAutoFocus={() => {
      const action = afterClose.current;
      afterClose.current = null;
      // Closing the drawer pops its history entry. Resolve the action on the
      // committed parent so a routed dialog cannot copy the retired drawer.
      if (action) queueMicrotask(() => currentHost.current[action]?.());
    }}>
      <header className="masthead-overview-heading">
        <DialogTitle className="sr-only">{t('nav.main')}</DialogTitle>
        <img src={host.assetUrl('assets/favicon.png')} width={32} height={32} alt=""/>
        <DialogClose asChild><Button variant="ghost" size="icon" aria-label={t('action.close')}><X aria-hidden="true"/></Button></DialogClose>
      </header>
      <div className="masthead-overview-scroll">
        <nav className="masthead-overview-group" aria-label={t('nav.main')}>
          {mainLink(host.launcherHref, currentPage === 'library', t('nav.single'), <NavigationIcon name="single"/>)}
          {mainLink(lobbyHref, currentPage === 'lobby', t('nav.lobby'), <NavigationIcon name="multiplayer"/>)}
          {replay && <Button variant="ghost" size="row" id="watchReplayButton" onClick={() => closeThen('onReplay')}>
            <PlaySquare aria-hidden="true"/><span>{t('lobby.watchReplay')}</span>
          </Button>}
        </nav>
        <section className="masthead-overview-group" aria-labelledby="masthead-overview-help">
          <h3 id="masthead-overview-help">{t('nav.help')}</h3>
          <Button variant="ghost" size="row" id="firstUseNoticeOpen" onClick={() => closeThen('onFirstUse')}>
            <NavigationIcon name="firstUse"/><span>{t('nav.firstUseNotice')}</span>
          </Button>
          {overviewLinks(helpLinks)}
        </section>
        <section className="masthead-overview-group" aria-labelledby="masthead-overview-site">
          <h3 id="masthead-overview-site">{t('nav.site')}</h3>
          {host.donationAvailable !== false && <Button variant="ghost" size="row" onClick={() => closeThen('onDonation')}>
            <Heart aria-hidden="true"/><span>{t('nav.donate')}</span>
          </Button>}
          {overviewLinks([statusLink])}
        </section>
        <section className="masthead-overview-group" aria-labelledby="masthead-overview-settings">
          <h3 id="masthead-overview-settings">{t('nav.settings')}</h3>
          <BookThemeToggle row/>
          <Button variant="ghost" size="row" id={host.variant === 'lobby' ? 'lobbyLessMotion' : 'lessMotionToggle'} aria-pressed={host.lessMotion} onClick={host.onToggleMotion}>
            <Activity aria-hidden="true"/><span>{t('nav.lessMotion')}</span>{host.lessMotion && <Check aria-hidden="true"/>}
          </Button>
          <div className="masthead-overview-language">
            <Languages size={16} aria-hidden="true"/><span>{t('ui.language')}</span>{host.languageControl}
          </div>
          <Collapsible.Root>
            <Collapsible.Trigger asChild><Button variant="ghost" size="row">
              <Code aria-hidden="true"/><span>{t('developer.options')}</span><ChevronDown aria-hidden="true"/>
            </Button></Collapsible.Trigger>
            <Collapsible.Content className="masthead-overview-subnav">
              <Button variant="ghost" size="row" id="runtimeDiagnosticsToggle" aria-pressed={host.diagnosticsEnabled} onClick={host.onToggleDiagnostics}>
                <Activity aria-hidden="true"/><span>{t('diagnostics.toggle')}</span>{host.diagnosticsEnabled && <Check aria-hidden="true"/>}
              </Button>
              {previewFeaturesEnabled && <Button variant="ghost" size="row" id="betaFeaturesToggle" aria-pressed={testFeatures} onClick={() => reloadTestFeatures(window, !testFeatures)}>
                <FlaskConical aria-hidden="true"/><span>{t('developer.betaFeatures')}</span>{testFeatures && <Check aria-hidden="true"/>}
              </Button>}
            </Collapsible.Content>
          </Collapsible.Root>
        </section>
      </div>
      <Collapsible.Root asChild>
        <footer className="masthead-overview-group masthead-overview-footer">
          <Collapsible.Trigger asChild><Button variant="ghost" size="row" id="mastheadAboutToggle" className="masthead-overview-disclosure">
            <Info aria-hidden="true"/><span>{t('nav.aboutSite')}</span><ChevronDown aria-hidden="true"/>
          </Button></Collapsible.Trigger>
          <Collapsible.Content className="masthead-overview-subnav">
            {overviewLinks(aboutLinks)}
          </Collapsible.Content>
        </footer>
      </Collapsible.Root>
    </DialogContent>
  </Dialog>;
}
