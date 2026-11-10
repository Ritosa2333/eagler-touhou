import { useLayoutEffect, useRef, useState, type ReactNode } from 'react';
import { DropdownMenu, Tooltip } from 'radix-ui';
import { GitHubLogoIcon } from '@radix-ui/react-icons';
import {
  Languages,
  ChevronLeft,
  ChevronRight,
  Heart as HeartIcon,
  Info as InfoCircledIcon,
  Activity as ActivityLogIcon,
  BookOpen as ReaderIcon,
  Check as CheckIcon,
  PlaySquare,
  Code,
  FlaskConical,
  ShieldCheck,
} from 'lucide-react';
import { Button } from '../../ui/button';
import { uiPortalHost } from '../../ui/portal';
import { usePopupPadding } from '../../ui/use-popup-padding';
import { useLocale } from '../../i18n';
import { useCompactNavigation } from '../../navigation/navigation-motion';
import { useSurfaceLayerState } from '../../navigation/surface-layer';
import { AppLink } from './AppLink';
import { BookThemeToggle } from './BookThemeToggle';
import { MastheadOverview, type MastheadSiteLink } from './MastheadOverview';
import { useLocation } from 'react-router';
import { AccountMenu } from './AccountMenu';
import { MastheadTooltip } from './MastheadTooltip';
import { NavigationIcon } from './NavigationIcon';
import { SiteBrand } from './SiteBrand';
import { UI_LOCALES, UI_LOCALE_LABELS, withUiLanguage } from '../../../src/launcher/i18n.mts';
import { MULTIPLAYER_GUIDE_PATH } from '../../../lib/multiplayer-guide.mjs';
import { accountsEnabled, uiReviewEnabled } from '../../release-features';
import '../../ui/menu-motion.css';
import {testFeaturesEnabled,reloadTestFeatures} from '../../../src/launcher/test-features.mts';

export interface LauncherMastheadProps {
  /** Published pre-session markup has native links and no menu owner. */
  interactive?: boolean;
  variant: 'library' | 'lobby' | 'help';
  nativeLinks?: boolean;
  assetUrl: (path: string) => string;
  launcherHref: string;
  faqHref: string;
  aboutHref: string;
  migrationHref?: string;
  migrationPolicyKnown?: boolean;
  updatedAge?: { text: string; title?: string; dateTime?: string };
  menuOpen: boolean;
  onMenuOpenChange: (open: boolean) => void;
  lessMotion: boolean;
  onToggleMotion: () => void;
  diagnosticsEnabled: boolean;
  onToggleDiagnostics: () => void;
  onDonation: () => void;
  donationAvailable?: boolean;
  onFirstUse: () => void;
  /** The shared MainSelect, with the original uiLanguageSelect native props. */
  languageControl: ReactNode;
  onReplay?: () => void;
  replayActive?: boolean;
}

export function LauncherMasthead(props: LauncherMastheadProps) {
  const { t, locale } = useLocale();
  const profileOpen = /^(?:\/(?:profile|login)\.html|\/u\/[^/]+)\/?$/.test(useLocation().pathname);
  const { variant, menuOpen, onMenuOpenChange, interactive = true } = props;
  const currentPage = profileOpen || props.replayActive || variant === 'help' ? undefined : variant;
  const compact = useCompactNavigation();
  const [overviewOpen, setOverviewOpen] = useSurfaceLayerState<boolean>('masthead-overview', false);
  useLayoutEffect(() => {
    if (!compact && overviewOpen) setOverviewOpen(false);
  }, [compact, overviewOpen, setOverviewOpen]);
  const padding = usePopupPadding(interactive && menuOpen && !compact);
  const Link = interactive && !props.nativeLinks ? AppLink : 'a';
  const trigger = useRef<HTMLButtonElement>(null),
    language = useRef<HTMLDivElement>(null),
    [host, setHost] = useState<HTMLElement>();
  const content = useRef<HTMLDivElement>(null),
    returnToLanguage = useRef(false),
    replayAfterClose = useRef(false),
    [languagesOpen, setLanguagesOpen] = useState(false);
  const [developerOpen,setDeveloperOpen]=useState(false);
  const returnToDeveloper=useRef(false);
  const [testFeatures,setTestFeatures]=useState(false);
  useLayoutEffect(() => {if (interactive) setTestFeatures(testFeaturesEnabled(window));}, [interactive]);
  const masthead = useRef<HTMLElement>(null);
  useLayoutEffect(() => {
    if (!interactive) return;
    const header = masthead.current,
      container = header?.parentElement;
    if (!header || !container) return;
    const property = '--site-header-height',
      previous = container.style.getPropertyValue(property);
    let applied = '';
    const sync = () => {
      const height = header.getBoundingClientRect().height;
      if (height > 0) {
        applied = `${height}px`;
        if (container.style.getPropertyValue(property) !== applied) container.style.setProperty(property, applied);
      }
    };
    sync();
    const observer = typeof ResizeObserver === 'undefined' ? null : new ResizeObserver(sync);
    observer?.observe(header);
    return () => {
      observer?.disconnect();
      if (applied && container.style.getPropertyValue(property) === applied) {
        if (previous) container.style.setProperty(property, previous);
        else container.style.removeProperty(property);
      }
    };
  }, [interactive]);
  const backToMenu = () => {
    if(developerOpen)returnToDeveloper.current=true;
    else returnToLanguage.current = true;
    setLanguagesOpen(false);
    setDeveloperOpen(false);
  };
  useLayoutEffect(() => {
    if (!menuOpen) return;
    if(developerOpen)
      content.current?.querySelector<HTMLElement>('#runtimeDiagnosticsToggle')?.focus();
    else if (languagesOpen)
      content.current?.querySelector<HTMLElement>('[role="menuitemradio"][data-state="checked"]')?.focus();
    else if(returnToDeveloper.current){
      returnToDeveloper.current=false;
      content.current?.querySelector<HTMLElement>('.masthead-developer-trigger')?.focus();
    } else if (returnToLanguage.current) {
      returnToLanguage.current = false;
      content.current?.querySelector<HTMLElement>('.masthead-language-trigger')?.focus();
    }
  }, [languagesOpen, developerOpen, menuOpen]);
  const lobbyHref = withUiLanguage(props.assetUrl('lobby.html'), locale);
  const faqLink: MastheadSiteLink = {href: props.faqHref, icon: <NavigationIcon name="faq"/>, label: t('nav.faq')};
  const helpLinks: MastheadSiteLink[] = [
    faqLink,
    {href: withUiLanguage(props.assetUrl(MULTIPLAYER_GUIDE_PATH.slice(1)), locale), icon: <NavigationIcon name="multiplayerGuide"/>, label: t('multiplayerGuide.title')},
    ...(uiReviewEnabled ? [{href: withUiLanguage(props.assetUrl('ui-review.html'), locale), icon: <NavigationIcon name="handbook"/>, label: t('nav.uiHandbook'), external: true}] : []),
  ];
  const statusLink: MastheadSiteLink = {href: withUiLanguage(props.assetUrl('uptime/'), locale), icon: <ActivityLogIcon aria-hidden="true"/>, label: t('nav.serviceStatus')};
  const aboutLinks: MastheadSiteLink[] = [
    {href: props.aboutHref, icon: <InfoCircledIcon/>, label: t('nav.siteInfo')},
    {href: 'https://github.com/YomotsuHisami/eagler-touhou', icon: <GitHubLogoIcon/>, label: 'GitHub', external: true},
    ...(props.migrationHref ? [{href: props.migrationHref, icon: <ReaderIcon/>, label: t('nav.oldSiteMigration'), policy: props.migrationPolicyKnown ? 'host-manifest-origin-migration-policy/1' : undefined}] : []),
    {href: withUiLanguage(props.assetUrl('privacy.html'), locale), icon: <ShieldCheck aria-hidden="true"/>, label: t('nav.privacyPolicy')},
  ];
  const menuLinks = (links: readonly MastheadSiteLink[]) => links.map(({href, icon, label, external, policy}) => <DropdownMenu.Item key={href} asChild>
    <a className="ui-menu-item" href={href} data-policy={policy} target={external ? '_blank' : undefined} rel={external ? 'noopener noreferrer' : undefined}>
      {icon}{label}
    </a>
  </DropdownMenu.Item>);
  const menuTrigger = <Button ref={trigger} variant="ghost" size="icon" className="masthead-action masthead-menu-toggle"
    id="mastheadMenuToggle" aria-label={t('nav.more')} disabled={!interactive}>
    <NavigationIcon name={compact ? 'drawer' : 'menu'}/>
  </Button>;
  return (
    <header
      ref={masthead}
      className={`masthead book-masthead${variant === 'lobby' ? ' lobby-masthead' : ''}`}
    >
      <Link className="brand book-brand" id="launcherLink" href={props.launcherHref} aria-label="EAGLER TOUHOU">
        <SiteBrand assetUrl={props.assetUrl} />
        <time
          className="brand-version"
          id="brandUpdateAge"
          title={props.updatedAge?.title}
          dateTime={props.updatedAge?.dateTime}
        >
          {props.updatedAge?.text ?? t('brand.neverUpdated')}
        </time>
      </Link>
      <nav className="book-main-nav" aria-label={t('nav.main')}>
        <Link
          href={props.launcherHref}
          aria-current={currentPage === 'library' ? 'page' : undefined}
        >
          <NavigationIcon name="single" />
          {t('nav.single')}
        </Link>
        <Link
          className="book-lobby-link"
          href={lobbyHref}
          aria-current={currentPage === 'lobby' ? 'page' : undefined}
        >
          <NavigationIcon name="multiplayer" />
          {t('nav.lobby')}
        </Link>
      </nav>
      <Tooltip.Provider delayDuration={0} disableHoverableContent>
      <nav className="masthead-links" aria-label={t('nav.siteInfo')}>
        {accountsEnabled && (
          <AccountMenu interactive={interactive} assetUrl={props.assetUrl}/>
        )}
        <a
          className="masthead-link"
          id="originMigrationOpen"
          href={props.migrationHref ?? 'migrate.html'}
          hidden={!props.migrationHref}
          data-policy={props.migrationPolicyKnown ? 'host-manifest-origin-migration-policy/1' : undefined}
        >
          {t('nav.oldSiteMigration')}
        </a>
        <MastheadTooltip label={faqLink.label}>
          <Button asChild variant="ghost" size="icon" className="masthead-action book-top-faq">
            <a href={faqLink.href} aria-label={faqLink.label}>{faqLink.icon}</a>
          </Button>
        </MastheadTooltip>
        {props.donationAvailable !== false && (
          <MastheadTooltip label={t('nav.donate')}>
            <Button variant="ghost" size="icon" className="masthead-action book-top-donation"
              aria-label={t('nav.donate')} onClick={props.onDonation} disabled={!interactive}>
              <HeartIcon aria-hidden="true"/>
            </Button>
          </MastheadTooltip>
        )}
        <BookThemeToggle interactive={interactive} tooltip />
        {!compact && <div hidden ref={language}>
          {props.languageControl}
        </div>}
        {compact ? <MastheadOverview host={props} open={interactive && overviewOpen} onOpenChange={setOverviewOpen} trigger={menuTrigger} lobbyHref={lobbyHref} currentPage={currentPage} helpLinks={helpLinks} statusLink={statusLink} aboutLinks={aboutLinks} testFeatures={testFeatures}/> : <DropdownMenu.Root
          modal={false}
          open={interactive && menuOpen}
          onOpenChange={(open) => {
            if (open) {
              setHost(uiPortalHost(trigger.current));
              setLanguagesOpen(false);
              setDeveloperOpen(false);
            }
            onMenuOpenChange(open);
          }}
        >
          <MastheadTooltip label={t('nav.more')}><DropdownMenu.Trigger asChild>{menuTrigger}</DropdownMenu.Trigger></MastheadTooltip>
          <DropdownMenu.Portal container={host}>
            <DropdownMenu.Content
              ref={content}
              className="ui-menu masthead-menu-panel ui-dropdown-motion"
              id="mastheadMenuPanel"
              align="end"
              sideOffset={8}
              collisionPadding={padding}
              onCloseAutoFocus={(event) => {
                if (!replayAfterClose.current) return;
                replayAfterClose.current = false;
                event.preventDefault();
                trigger.current?.focus({preventScroll:true});
                props.onReplay?.();
              }}
              onEscapeKeyDown={(event) => {
                if (languagesOpen||developerOpen) {
                  event.preventDefault();
                  backToMenu();
                }
              }}
              onKeyDown={(event) => {
                if ((languagesOpen||developerOpen) && event.key === 'ArrowLeft') {
                  event.preventDefault();
                  backToMenu();
                }
              }}
            >
              {developerOpen ? <>
                <DropdownMenu.Item className="ui-menu-item" onSelect={event=>{event.preventDefault();backToMenu();}}>
                  <ChevronLeft aria-hidden="true"/>{t('developer.options')}
                </DropdownMenu.Item>
                <DropdownMenu.Separator className="ui-menu-separator"/>
                <DropdownMenu.CheckboxItem className="ui-menu-item" id="runtimeDiagnosticsToggle"
                  checked={props.diagnosticsEnabled} onCheckedChange={props.onToggleDiagnostics}>
                  <ActivityLogIcon/>{t('diagnostics.toggle')}<DropdownMenu.ItemIndicator><CheckIcon/></DropdownMenu.ItemIndicator>
                </DropdownMenu.CheckboxItem>
                <DropdownMenu.CheckboxItem className="ui-menu-item" id="betaFeaturesToggle" checked={testFeatures}
                  onCheckedChange={enabled=>reloadTestFeatures(window,enabled===true)}>
                  <FlaskConical/>{t('developer.betaFeatures')}<DropdownMenu.ItemIndicator><CheckIcon/></DropdownMenu.ItemIndicator>
                </DropdownMenu.CheckboxItem>
              </> : languagesOpen ? (
                <>
                  <DropdownMenu.Item
                    className="ui-menu-item"
                    aria-label={`${t('action.back')} | ${t('ui.language')}`}
                    onSelect={(event) => {
                      event.preventDefault();
                      backToMenu();
                    }}
                  >
                    <ChevronLeft aria-hidden="true" />
                    {t('ui.language')}
                  </DropdownMenu.Item>
                  <DropdownMenu.Separator className="ui-menu-separator" />
                  <DropdownMenu.RadioGroup
                    className="masthead-menu-language"
                    aria-label={t('ui.language')}
                    value={locale}
                    onValueChange={(value) => {
                      const select = language.current?.querySelector('select');
                      if (select) {
                        select.value = value;
                        select.dispatchEvent(new Event('change', { bubbles: true }));
                      }
                    }}
                  >
                    {UI_LOCALES.map((value) => (
                      <DropdownMenu.RadioItem key={value} className="ui-menu-item" value={value}>
                        {t(UI_LOCALE_LABELS[value])}
                        <DropdownMenu.ItemIndicator>
                          <CheckIcon />
                        </DropdownMenu.ItemIndicator>
                      </DropdownMenu.RadioItem>
                    ))}
                  </DropdownMenu.RadioGroup>
                </>
              ) : (
                <>
                  {variant === 'lobby' && props.onReplay && <>
                    <DropdownMenu.Item className="ui-menu-item" id="watchReplayButton" onSelect={() => {replayAfterClose.current = true;}}><PlaySquare aria-hidden="true"/>{t('lobby.watchReplay')}</DropdownMenu.Item>
                    <DropdownMenu.Separator className="ui-menu-separator" />
                  </>}
                  <DropdownMenu.Group aria-label={t('nav.help')}>
                    <DropdownMenu.Label className="masthead-menu-heading">{t('nav.help')}</DropdownMenu.Label>
                    <DropdownMenu.Item className="ui-menu-item" id="firstUseNoticeOpen" onSelect={props.onFirstUse}>
                      <NavigationIcon name="firstUse"/>{t('nav.firstUseNotice')}
                    </DropdownMenu.Item>
                    {menuLinks(helpLinks)}
                  </DropdownMenu.Group>
                  <DropdownMenu.Separator className="ui-menu-separator" />
                  <DropdownMenu.Group aria-label={t('nav.site')}>
                    <DropdownMenu.Label className="masthead-menu-heading">{t('nav.site')}</DropdownMenu.Label>
                    {props.donationAvailable !== false && (
                      <DropdownMenu.Item className="ui-menu-item book-menu-donation" onSelect={props.onDonation}>
                        <HeartIcon aria-hidden="true" />
                        {t('nav.donate')}
                      </DropdownMenu.Item>
                    )}
                    {menuLinks([statusLink])}
                  </DropdownMenu.Group>
                  <DropdownMenu.Separator className="ui-menu-separator" />
                  <DropdownMenu.Group aria-label={t('nav.settings')}>
                    <DropdownMenu.Label className="masthead-menu-heading">{t('nav.settings')}</DropdownMenu.Label>
                    <DropdownMenu.CheckboxItem
                      className="ui-menu-item"
                      id={variant === 'lobby' ? 'lobbyLessMotion' : 'lessMotionToggle'}
                      checked={props.lessMotion}
                      onCheckedChange={props.onToggleMotion}
                    >
                      <ActivityLogIcon />
                      {t('nav.lessMotion')}
                      <DropdownMenu.ItemIndicator>
                        <CheckIcon />
                      </DropdownMenu.ItemIndicator>
                    </DropdownMenu.CheckboxItem>
                    <DropdownMenu.Item
                      className="ui-menu-item masthead-language-trigger"
                      onSelect={(event) => {
                        event.preventDefault();
                        setLanguagesOpen(true);
                      }}
                      onKeyDown={(event) => {
                        if (event.key === 'ArrowRight') {
                          event.preventDefault();
                          setLanguagesOpen(true);
                        }
                      }}
                    >
                      <Languages aria-hidden="true" />
                      {t('ui.language')}
                      <span className="masthead-language-name">{t(UI_LOCALE_LABELS[locale])}</span>
                      <ChevronRight aria-hidden="true" />
                    </DropdownMenu.Item>
                    <DropdownMenu.Item className="ui-menu-item masthead-developer-trigger"
                      onSelect={event=>{event.preventDefault();setDeveloperOpen(true);}}
                      onKeyDown={event=>{if(event.key==='ArrowRight'){event.preventDefault();setDeveloperOpen(true);}}}>
                      <Code aria-hidden="true"/>{t('developer.options')}<ChevronRight aria-hidden="true"/>
                    </DropdownMenu.Item>
                  </DropdownMenu.Group>
                  <DropdownMenu.Separator className="ui-menu-separator" />
                  <DropdownMenu.Group aria-label={t('nav.aboutSite')}>
                    <DropdownMenu.Label className="masthead-menu-heading">{t('nav.aboutSite')}</DropdownMenu.Label>
                    {menuLinks(aboutLinks)}
                  </DropdownMenu.Group>
                </>
              )}
            </DropdownMenu.Content>
          </DropdownMenu.Portal>
        </DropdownMenu.Root>}
      </nav>
      </Tooltip.Provider>
    </header>
  );
}
