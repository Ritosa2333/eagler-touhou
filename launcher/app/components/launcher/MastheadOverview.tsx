import {useLayoutEffect, useRef, type MouseEvent, type ReactElement, type ReactNode} from 'react';
import {ArrowLeft, Heart, Info, Languages, X} from 'lucide-react';
import {useLocale} from '../../i18n';
import {useSurfaceLayerState} from '../../navigation/surface-layer';
import {Button} from '../../ui/button';
import {Dialog, DialogClose, DialogContent, DialogTitle, DialogTrigger} from '../../ui/dialog';
import {useContentMotion} from '../../ui/use-surface-motion';
import {accountsEnabled} from '../../release-features';
import {SettingsField, SettingsGroup, SettingsRowContent} from '../settings/SettingsGroup';
import type {BackgroundSurface} from './CustomBackground';
import {BookThemeToggle} from './BookThemeToggle';
import {AppLink} from './AppLink';
import {MastheadTooltip} from './MastheadTooltip';
import type {LauncherMastheadProps} from './LauncherMasthead';
import {NavigationIcon} from './NavigationIcon';
import {LauncherWordmark} from './LauncherWordmark';
import {LauncherAccountRow, useLauncherIdentity} from './AccountIdentity';

export interface MastheadSiteLink {
  href: string;
  icon: ReactNode;
  label: string;
  external?: boolean;
  policy?: string;
}
type OverviewAction = 'onDonation' | 'onFirstUse';
type OverviewPanel = 'language' | 'about';

export function MastheadOverview({host, open, onOpenChange, trigger, installsLink, helpLinks, statusLink, aboutLinks, themeSurfaces}: {
  host: LauncherMastheadProps;
  open: boolean;
  onOpenChange(open: boolean): void;
  trigger: ReactElement;
  installsLink?: MastheadSiteLink;
  helpLinks: readonly MastheadSiteLink[];
  statusLink: MastheadSiteLink;
  aboutLinks: readonly MastheadSiteLink[];
  themeSurfaces?: readonly BackgroundSurface[];
}) {
  const {t} = useLocale();
  const [panel, setPanel] = useSurfaceLayerState<OverviewPanel | null>('masthead-subpage',null);
  const panelMotion = useContentMotion<HTMLDivElement>(open ? panel ?? 'main' : null);
  const back = useRef<HTMLButtonElement>(null);
  const content = useRef<HTMLDivElement>(null);
  const returnPanel = useRef<OverviewPanel | undefined>(undefined);
  const afterClose = useRef<OverviewAction | null>(null);
  const currentHost = useRef(host);
  useLayoutEffect(() => {currentHost.current = host;}, [host]);
  useLayoutEffect(() => {
    if (!open) return;
    if (panel) back.current?.focus();
    else if (returnPanel.current) {
      content.current?.querySelector<HTMLButtonElement>(`[data-overview-panel="${returnPanel.current}"]`)?.focus();
      returnPanel.current = undefined;
    }
  }, [open, panel]);
  const identity = useLauncherIdentity(accountsEnabled && open);
  const title = panel === 'language' ? t('ui.language') : panel === 'about' ? t('nav.aboutSite') : t('nav.main');
  const PageLink = host.nativeLinks ? 'a' : AppLink;
  function openPanel(next: OverviewPanel) {
    returnPanel.current = next;
    setPanel(next);
  }
  function closeThen(action: OverviewAction) {
    if (afterClose.current) return;
    afterClose.current = action;
    onOpenChange(false);
  }
  function follow(event: MouseEvent<HTMLAnchorElement>) {
    if (event.defaultPrevented || event.button !== 0 || event.altKey || event.ctrlKey || event.metaKey || event.shiftKey) return;
    const {href, target} = event.currentTarget;
    if (target === '_blank') onOpenChange(false);
    else if (href === window.location.href) {
      event.preventDefault();
      onOpenChange(false);
    } else {
      event.preventDefault();
      window.location.replace(href);
    }
  }
  function overviewLinks(links: readonly MastheadSiteLink[]) {
    return links.map(({href, icon, label, external, policy}) => <Button key={href} asChild variant="ghost" size="row" className="settings-navigation-row">
      <a href={href} data-policy={policy} target={external ? '_blank' : undefined} rel={external ? 'noopener noreferrer' : undefined}
        onClick={follow}><SettingsRowContent icon={icon}>{label}</SettingsRowContent></a>
    </Button>);
  }
  return <Dialog open={open} onOpenChange={next => {
    if (next) {
      afterClose.current = null;
      returnPanel.current = undefined;
      setPanel(null);
    }
    onOpenChange(next);
  }}>
    <MastheadTooltip label={t('nav.more')}><DialogTrigger asChild>{trigger}</DialogTrigger></MastheadTooltip>
    <DialogContent ref={content} id="mastheadMenuPanel" className="masthead-overview" overlayClassName="masthead-overview-overlay" aria-describedby={undefined} showClose={false}
      onEscapeKeyDown={event => {if (panel) {event.preventDefault(); setPanel(null);}}}
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
        {panel ? <div className="masthead-overview-subheading">
          <Button ref={back} type="button" variant="ghost" size="icon" aria-label={t('action.back')} onClick={() => setPanel(null)}><ArrowLeft aria-hidden="true"/></Button>
          <DialogTitle>{title}</DialogTitle>
        </div> : <><DialogTitle className="sr-only">{title}</DialogTitle><LauncherWordmark/></>}
        <DialogClose asChild><Button variant="ghost" size="icon" aria-label={t('action.close')}><X aria-hidden="true"/></Button></DialogClose>
      </header>
      <div ref={panelMotion} className="masthead-overview-scroll" key={panel ?? 'main'}>
        {panel === 'language' ? <SettingsGroup>
          <SettingsField label={title}>{host.languageControl}</SettingsField>
        </SettingsGroup> : panel === 'about' ? <SettingsGroup>
          {overviewLinks(aboutLinks)}
          {!!host.onFirstUse && <Button variant="ghost" size="row" className="settings-navigation-row" id="firstUseNoticeOpen" onClick={() => closeThen('onFirstUse')}>
            <SettingsRowContent icon={<NavigationIcon name="firstUse"/>}>{t('nav.firstUseNotice')}</SettingsRowContent>
          </Button>}
        </SettingsGroup> : <>
          {accountsEnabled && <LauncherAccountRow compact identity={identity} assetUrl={host.assetUrl}/>}
          {installsLink && <div className="masthead-overview-group">
            <SettingsGroup>
              <Button asChild variant="ghost" size="row" className="settings-navigation-row">
                <PageLink href={installsLink.href} {...(host.nativeLinks ? {onClick:follow} : {replace:true, rememberEntry:true})}>
                  <SettingsRowContent icon={installsLink.icon}>{installsLink.label}</SettingsRowContent>
                </PageLink>
              </Button>
            </SettingsGroup>
          </div>}
          <section className="masthead-overview-group" aria-labelledby="masthead-overview-help">
            <h3 id="masthead-overview-help">{t('nav.help')}</h3>
            <SettingsGroup>
              {overviewLinks(helpLinks)}
            </SettingsGroup>
          </section>
          <section className="masthead-overview-group" aria-labelledby="masthead-overview-site">
            <h3 id="masthead-overview-site">{t('nav.site')}</h3>
            <SettingsGroup>
              {host.donationAvailable !== false && <Button variant="ghost" size="row" className="settings-navigation-row" onClick={() => closeThen('onDonation')}>
                <SettingsRowContent icon={<Heart aria-hidden="true"/>}>{t('nav.supportUs')}</SettingsRowContent>
              </Button>}
              {overviewLinks([{...statusLink, label:t('nav.shortServiceStatus')}])}
            </SettingsGroup>
          </section>
          <section className="masthead-overview-group" aria-labelledby="masthead-overview-settings">
            <h3 id="masthead-overview-settings">{t('nav.settings')}</h3>
            <SettingsGroup>
              <BookThemeToggle surfaces={themeSurfaces} row rowLabel={t('nav.switchTheme')}/>
              <Button variant="ghost" size="row" className="settings-navigation-row" data-overview-panel="language" onClick={() => openPanel('language')}>
                <SettingsRowContent icon={<Languages aria-hidden="true"/>}>{t('ui.language')}</SettingsRowContent>
              </Button>
              <Button variant="ghost" size="row" className="settings-navigation-row" data-overview-panel="about" onClick={() => openPanel('about')}>
                <SettingsRowContent icon={<Info aria-hidden="true"/>}>{t('nav.aboutSite')}</SettingsRowContent>
              </Button>
            </SettingsGroup>
          </section>
        </>}
      </div>
    </DialogContent>
  </Dialog>;
}
