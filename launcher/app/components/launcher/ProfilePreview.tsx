import type {CloudSaveSync} from '../../services/cloud-save-sync';
import {AccountRequestError, accountRequest} from '../../services/account-client';
import {accountSession, notifyRoomProfileChange, useAccountSession} from '../../services/account-session';

import {AccountAvatar} from './AccountAvatar';
import {ArrowUp, ArrowUpRight, CalendarDays, CodeXml, Link2, MessageCircle, TvMinimal} from 'lucide-react';
import {OverflowText} from '../../ui/overflow-text';
import {Button as UiButton} from '../../ui/controls';
import {Button} from '../../ui/button';
import {BookButton} from './BookButton';
import {lazy, Suspense, useEffect, useLayoutEffect, useRef, useState, type ReactNode} from 'react';
import {Link, Navigate, useLocation, useNavigate} from 'react-router';
import {useLocale, type UiMessageKey} from '../../i18n';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';
import {ProfileLoading, ProfilePanelLoading} from './ProfileLoading';
import {ProfileGameDataCard} from './ProfileGameDataCard';
import {ProfileCover} from './ProfileCover';
import {CountryFlag} from './CountryFlag';
import {profileSocialTab, SocialCounts, SocialFollowControl, SocialProvider, SocialSection} from './SocialSection';
import {getProfileStatistics, type ProfileStatistics} from '../../services/cloud-saves';
import type {LocalSaveReader} from '../../services/local-save-storage';
import {normalizeProfileDisplay, type ProfileDisplay} from '../../models/profile-display';
const AccountSettings=lazy(()=>import('./AccountSettings').then(module=>({default:module.AccountSettings})));
type SocialLink = {label: string; url: string};
export type Profile = {name: string; bio: string; avatar: string; cover: string; links: SocialLink[]; roomAvatarReady?: boolean};
const tabLabels = {overview:'profile.tab.overview', versus:'profile.gameData'} as const;
type ProfileTab = keyof typeof tabLabels;
const blankProfile: Profile = {name: '', bio: '', avatar: '', cover: '', links: []};
const imageData = (value: string) => /^data:image\/(png|jpeg|webp);base64,/i.test(value);
const safeUrl = (value: string) => {try {const url = new URL(value); return ['http:', 'https:'].includes(url.protocol) ? url : null;} catch {return null;}};
export type ProfileReview = Partial<Record<'name' | 'bio' | 'avatar' | 'cover' | 'links', 'pending' | 'rejected'>>;
type RemoteProfile = {username: string; nickname: string; joinedAt: string | null; countryCode?:string | null; regionCode?:string | null; roomAvatarReady?:boolean; reviewPending?:boolean; characterRatingPreview?:boolean; details: (Profile & {display:unknown}) | null};
export function ProfilePage({readLocalSaves,cloudSync}:{readLocalSaves?:LocalSaveReader;cloudSync?:CloudSaveSync|null}={}) {
  const {pathname, search, hash} = useLocation();
  const {state,retry} = useAccountSession();
  const {locale} = useLocale();
  const viewer = state.status === 'member' ? state.user.username : '';
  if (/^\/profile\.html\/?$/.test(pathname)) {
    if (state.status === 'loading') return <ProfileLoading/>;
    if (state.status === 'error') return <ProfileLoading message="account.requestFailed" retry={retry}/>;
    const destination = viewer ? {pathname:'/u/'+encodeURIComponent(viewer),search,hash} :
      withUiLanguage('/login.html?next='+encodeURIComponent(pathname+search+hash),locale);
    return <><ProfileLoading/><Navigate to={destination} replace/></>;
  }
  return <AccountProfile key={pathname} pathname={pathname} viewer={viewer} viewerLoading={state.status==='loading'} readLocalSaves={readLocalSaves} cloudSync={cloudSync}/>;
}
function AccountProfile({pathname,viewer,viewerLoading,readLocalSaves,cloudSync}: {
  pathname:string;viewer:string;viewerLoading:boolean;readLocalSaves?:LocalSaveReader;cloudSync?:CloudSaveSync|null;
}) {
  const {hash} = useLocation();
  const panel = ['#edit','#settings','#security','#game-settings'].includes(hash) ? hash : '';
  const [state,setState] = useState<{profile?:RemoteProfile; error?:UiMessageKey}>({});
  const [attempt,setAttempt] = useState(0);
  // biome-ignore lint/correctness/useExhaustiveDependencies: The retry counter starts a new request for the same profile.
  useEffect(() => {
    const controller = new AbortController();
    let generation = 0, poll: ReturnType<typeof setTimeout> | undefined, latest: RemoteProfile | undefined;
    setState({});
    const refresh = () => {if (latest?.reviewPending !== undefined) void load(true).catch(schedule);};
    const schedule = () => {
      clearTimeout(poll);
      if (controller.signal.aborted || !latest?.reviewPending) return;
      poll = setTimeout(() => {
        if (document.visibilityState === 'visible') refresh();
        else schedule();
      }, 10_000);
    };
    async function load(fresh = false) {
      const request = ++generation;
      const name = decodeURIComponent(pathname.slice(3).replace(/\/$/, ''));
      if (!name) throw new AccountRequestError('PROFILE_NOT_FOUND',404);
      const profile = await accountRequest<RemoteProfile>(`v1/profiles/${encodeURIComponent(name)}`,undefined,{signal:controller.signal,fresh});
      if (controller.signal.aborted || request !== generation) return;
      const previous = latest;
      latest = profile;
      setState({profile});
      schedule();
      const session = accountSession.getSnapshot();
      if (session.status === 'member' && session.user.username === profile.username && previous?.reviewPending && (previous.nickname !== profile.nickname ||
        previous.details?.avatar !== profile.details?.avatar || previous.roomAvatarReady !== profile.roomAvatarReady)) {
        window.dispatchEvent(new Event('account-profile-change'));
        notifyRoomProfileChange();
      }
    }
    load().catch(error => {if (!controller.signal.aborted) setState({error:error instanceof AccountRequestError && error.status === 404 ? 'profile.notFound' : 'account.requestFailed'});});
    window.addEventListener('account-profile-change',refresh);
    return () => {controller.abort();clearTimeout(poll);window.removeEventListener('account-profile-change',refresh);};
  }, [pathname,attempt]);
  if (!state.profile) return <ProfileLoading message={state.error} retry={state.error !== 'profile.notFound' ? ()=>setAttempt(value=>value+1) : undefined}/>;
  const owner = viewer === state.profile.username;
  if (panel && viewerLoading) return <ProfileLoading/>;
  return <ProfileBody key={owner ? 'owner' : 'public'} panel={panel} remote={state.profile} owner={owner}
    viewer={viewer} viewerLoading={viewerLoading} readLocalSaves={readLocalSaves} cloudSync={cloudSync}/>;
}
function ProfileBody({remote,owner,panel,viewer,viewerLoading,readLocalSaves,cloudSync}: {
  remote:RemoteProfile;owner:boolean;panel:string;viewer:string;viewerLoading:boolean;readLocalSaves?:LocalSaveReader;cloudSync?:CloudSaveSync|null;
}) {
  const {locale,t}=useLocale();
  const location=useLocation(),navigate=useNavigate();
  const socialTab=profileSocialTab(location.search,location.hash);
  const showingSocial=!!socialTab;
  const anchor=location.hash.replace(/^#profile-/,'');
  const tab:ProfileTab = ['versus','career','records'].includes(anchor) ? 'versus' : 'overview';
  const sectionAddress=(key:ProfileTab)=>({pathname:location.pathname,search:location.search,hash:`#profile-${key}`});
  const [legacySaved, setLegacySaved] = useState<{source:RemoteProfile;profile:Profile;display:ProfileDisplay}>();
  const local = legacySaved?.source === remote ? legacySaved : undefined;
  const profile = local?.profile ?? {...blankProfile,...remote.details,name:remote.nickname,roomAvatarReady:remote.roomAvatarReady};
  const settings = owner && !!panel;
  const display = local?.display ?? normalizeProfileDisplay(remote.details?.display);
  const showSaveStatistics = !showingSocial && !settings && (display.visible.career || display.visible.records);
  const [statistics,setStatistics]=useState<ProfileStatistics>(),[statisticsError,setStatisticsError]=useState(false);
  useEffect(()=>{
    if (!showSaveStatistics) return;
    let current:AbortController|undefined;
    const load=()=>{current?.abort();const controller=new AbortController();current=controller;setStatisticsError(false);void getProfileStatistics(remote.username,controller.signal,!owner).then(value=>{if(!controller.signal.aborted)setStatistics(value);}).catch(()=>{if(!controller.signal.aborted)setStatisticsError(true);});};
    load();window.addEventListener('cloud-save-change',load);
    return()=>{current?.abort();window.removeEventListener('cloud-save-change',load);};
  },[remote.username,showSaveStatistics,owner]);
  const country = remote.regionCode === 'TW' || remote.countryCode === 'TW' ? 'CN' : remote.countryCode;
  const countryName = country && /^[A-Z]{2}$/.test(country)
    ? new Intl.DisplayNames('en', {type:'region'}).of(country) : undefined;
  const visitor = !owner;
  const characterPreview = import.meta.env?.DEV && remote.characterRatingPreview === true;
  const editProfile = () => navigate({pathname:location.pathname,search:location.search,hash:'#edit'});
  const showGameData = display.visible.versus || display.visible.career || display.visible.records;
  const visibleTabs: ProfileTab[] = showGameData ? ['overview', 'versus'] : ['overview'];
  const links = [...new Map(profile.links.map(link => [link.url,link])).values()];
  const details = characterPreview ? null : <><p className="profile-bio">{profile.bio || t('profile.bio.missing')}</p>{remote.joinedAt && <dl className="profile-about-meta">
    <div><dt><CalendarDays/>{t('profile.registeredAt')}</dt><dd><time dateTime={remote.joinedAt}>{new Intl.DateTimeFormat(locale,{year:'numeric',month:'long',timeZone:'UTC'}).format(new Date(remote.joinedAt))}</time></dd></div>
    </dl>}{links.length > 0 && <div className="profile-links">{links.map(link => {
      const host = safeUrl(link.url)?.hostname ?? '';
      const Icon = /(^|\.)github\.com$/.test(host) ? CodeXml : /(^|\.)(discord\.com|discord\.gg)$/.test(host) ? MessageCircle : /(^|\.)(bilibili\.com|b23\.tv)$/.test(host) ? TvMinimal : Link2;
      return <a key={link.url} href={link.url} target="_blank" rel="noopener noreferrer"><Icon aria-hidden="true"/><span>{link.label}</span><ArrowUpRight aria-hidden="true"/></a>;
    })}</div>}</>;
  const navigation = visibleTabs.map(item => <Link key={item} to={sectionAddress(item)} aria-current={tab===item ? 'location' : undefined}>{t(tabLabels[item])}</Link>);
  if (settings) return <Suspense fallback={<ProfileLoading/>}><AccountSettings username={remote.username} initialProfile={profile} initialDisplay={display} reviewPending={remote.reviewPending}
    panel={panel} readLocalSaves={readLocalSaves} cloudSync={cloudSync}
    onSave={(next,visibility) => {if (remote.reviewPending === undefined) setLegacySaved({source:remote,profile:next,display:visibility});}}/></Suspense>;
  return <main className="player-profile profile-page" aria-labelledby={socialTab ? "profile-social-title" : "profile-name"}>
    {!socialTab && <ProfileBanner src={imageData(profile.cover) ? profile.cover : undefined}>
      <div className="profile-identity">
        <UiButton variant="unstyled" type="button" className="profile-avatar" aria-label={t('profile.avatar.change')}
          disabled={visitor} onClick={editProfile}><AccountAvatar src={profile.avatar} alt={t('profile.avatar.alt')}/></UiButton>
        <div className="profile-name">
          <h1 id="profile-name" tabIndex={-1}><OverflowText>{profile.name || t('profile.name.missing')}</OverflowText></h1>
          <div className="profile-name-meta">
            <Link className="profile-username" to={'/u/'+remote.username}>@{remote.username}</Link>
            {country && countryName && <span className="profile-country" lang="en">
              <CountryFlag code={country}/>{countryName}
            </span>}
          </div>
        </div>
        <div className="profile-actions">
          {!characterPreview && <SocialProvider key={viewer} username={remote.username} includeLists={false} anonymous={!viewer}>
            <SocialCounts username={remote.username}/>
            {!owner && !viewerLoading && <SocialFollowControl username={remote.username}/>}
          </SocialProvider>}
          {owner && <Button variant="outline" size="sm" asChild>
            <Link to={{pathname:location.pathname,search:location.search,hash:'#game-settings'}}>{t('cloudSave.title')}</Link>
          </Button>}
          {owner && <BookButton variant="action" size="sm" type="button" onClick={editProfile}>{t('profile.edit')}</BookButton>}
        </div>
      </div>
    </ProfileBanner>}
    <div className="profile-wrap">
    {socialTab ? <SocialProvider key={viewer} username={remote.username} includeLists list={socialTab} anonymous={!viewer}>
      <SocialSection username={remote.username} tab={socialTab}/>
    </SocialProvider> : <>
      <div className="profile-header-details">{details}</div>
      <nav className="profile-section-nav profile-nav-mobile" aria-label={t('profile.navigationSections')}>{navigation}</nav>
      <div className="profile-columns"><aside className="profile-aside"><div className="profile-sidebar-details">{details}</div><nav className="profile-section-nav profile-nav-desktop" aria-label={t('profile.navigationSections')}>{navigation}</nav></aside>
        <div className="profile-content">
        {showGameData && <ProfileGameDataCard key={remote.username} username={remote.username} owner={owner}
          display={display} statistics={statistics} statisticsError={statisticsError}/>}
        <div className="profile-end-actions"><BackToTop/></div>
        </div></div>
    </>}
  </div></main>;
}

function ProfileBanner({src,children}: {src?: string;children: ReactNode}) {
  const banner = useRef<HTMLElement>(null);
  useLayoutEffect(() => {
    const element = banner.current;
    const header = document.querySelector<HTMLElement>('.book-masthead');
    if (!element || !header || typeof IntersectionObserver === 'undefined') return;
    let intersection: IntersectionObserver | undefined;
    let headerHeight = -1;
    const observeCover = () => {
      const bounds = header.getBoundingClientRect();
      if (bounds.height === headerHeight) return;
      headerHeight = bounds.height;
      intersection?.disconnect();
      header.toggleAttribute('data-profile-cover-overlap', element.getBoundingClientRect().bottom > bounds.bottom);
      intersection = new IntersectionObserver(([entry], observer) => {
        if (observer === intersection) header.toggleAttribute('data-profile-cover-overlap', entry.isIntersecting);
      }, {rootMargin: `-${headerHeight}px 0px 0px`});
      intersection.observe(element);
    };
    observeCover();
    const resize = typeof ResizeObserver === 'undefined' ? null : new ResizeObserver(observeCover);
    resize?.observe(header, {box: 'border-box'});
    return () => {
      resize?.disconnect();
      intersection?.disconnect();
      intersection = undefined;
      header.removeAttribute('data-profile-cover-overlap');
    };
  },[]);
  return <section id="profile-overview" className="profile-page-banner profile-public-section" ref={banner} aria-labelledby="profile-name">
    <div className="profile-page-banner-art">
      <ProfileCover src={src} className="profile-page-banner-cover" responsive />
    </div>
    {children}
  </section>;
}

function BackToTop() {
  const {t} = useLocale();
  const [visible,setVisible] = useState(false);
  useEffect(() => {
    const update = () => setVisible(window.scrollY > 320);
    update(); window.addEventListener('scroll',update,{passive:true});
    return () => window.removeEventListener('scroll',update);
  }, []);
  return visible ? <BookButton variant="action" type="button" className="profile-back-to-top" aria-label={t('profile.backToTop')}
    onClick={() => {window.scrollTo({top:0});document.getElementById('profile-name')?.focus({preventScroll:true});}}><ArrowUp/></BookButton> : null;
}
