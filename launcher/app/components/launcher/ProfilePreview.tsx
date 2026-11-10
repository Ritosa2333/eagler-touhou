import type {CloudSaveSync} from '../../services/cloud-save-sync';
import {AccountRequestError, accountRequest} from '../../services/account-client';
import {notifyRoomProfileChange, useAccountSession} from '../../services/account-session';

import backgroundUrl from '../../../public/assets/launcher-background.webp?url';
import {AccountAvatar} from './AccountAvatar';
import {ArrowUp, ArrowUpRight, CalendarDays, CodeXml, Link2, MessageCircle, TvMinimal} from 'lucide-react';
import {OverflowText} from '../../ui/overflow-text';
import {Button as UiButton} from '../../ui/controls';
import {Button} from '../../ui/button';
import {BookButton} from './BookButton';
import {lazy, Suspense, useEffect, useState} from 'react';
import {Link, useLocation, useNavigate} from 'react-router';
import {useLocale, type UiMessageKey} from '../../i18n';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';
import './profile-preview.css';
import {AccountSettings} from './AccountSettings';
import {ProfileTh09Duels} from './ProfileTh09Duels';
import {CountryFlag} from './CountryFlag';
import {SocialFollowControl, SocialProvider, SocialSection} from './SocialSection';
import {getProfileStatistics, type ProfileStatistics} from '../../services/cloud-saves';
import type {LocalSaveReader} from '../../services/local-save-storage';
const ProfileGameData=lazy(()=>import('./ProfileGameData'));
type SocialLink = {label: string; url: string};
export type Profile = {name: string; bio: string; avatar: string; cover: string; links: SocialLink[]; roomAvatarReady?: boolean};
type DisplayKey = 'friends' | 'career' | 'records' | 'content';
type OverviewKey = DisplayKey | 'favorites';
export type ProfileDisplay = {visible: Record<DisplayKey, boolean>; order: OverviewKey[]};
const displayLabels: Record<DisplayKey, UiMessageKey> = {friends:'profile.tab.friends',career:'profile.tab.career',records:'profile.tab.records',content:'profile.tab.content'};
const tabLabels = {overview:'profile.tab.overview',career:'profile.tab.career',records:'profile.tab.records',content:'profile.tab.content',friends:'profile.tab.friends'} as const;
type ProfileTab = keyof typeof tabLabels;
const defaultDisplay: ProfileDisplay = {visible: {friends: true, career: true, records: true, content: true}, order: ['career', 'records', 'content', 'friends', 'favorites']};
const validDisplay = (value: unknown): value is ProfileDisplay => {
  if (!value || typeof value !== 'object') return false;
  const settings = value as ProfileDisplay;
  return !!settings.visible && Object.keys(displayLabels).every(key => typeof settings.visible[key as DisplayKey] === 'boolean') &&
    Array.isArray(settings.order) && settings.order.length === 5 && new Set(settings.order).size === 5 && settings.order.every(key => defaultDisplay.order.includes(key));
};
const blankProfile: Profile = {name: '', bio: '', avatar: '', cover: '', links: []};
const imageData = (value: string) => /^data:image\/(png|jpeg|webp);base64,/i.test(value);
const safeUrl = (value: string) => {try {const url = new URL(value); return ['http:', 'https:'].includes(url.protocol) ? url : null;} catch {return null;}};
export type ProfileReview = Partial<Record<'name' | 'bio' | 'avatar' | 'cover' | 'links', 'pending' | 'rejected'>>;
type RemoteProfile = {username: string; nickname: string; joinedAt: string; countryCode?:string | null; regionCode?:string | null; roomAvatarReady?:boolean; reviewPending?:boolean; details: (Profile & {display:ProfileDisplay}) | null};
export function ProfilePage({readLocalSaves,cloudSync}:{readLocalSaves?:LocalSaveReader;cloudSync?:CloudSaveSync|null}={}) {
  const {pathname} = useLocation();
  const {state,retry} = useAccountSession();
  const {t} = useLocale();
  if (state.status === 'loading' || state.status === 'error') return <main className="player-profile"><div className="profile-wrap" aria-busy={state.status === 'loading'}>
    {state.status === 'error' && <p role="alert">{t('account.requestFailed')} <Button variant="outline" onClick={retry}>{t('cloudSave.refresh')}</Button></p>}
  </div></main>;
  const viewer = state.status === 'member' ? state.user.username : '';
  return <AccountProfile key={pathname+'|'+viewer} pathname={pathname} viewer={viewer} readLocalSaves={readLocalSaves} cloudSync={cloudSync}/>;
}
function AccountProfile({pathname,viewer,readLocalSaves,cloudSync}: {pathname:string;viewer:string;readLocalSaves?:LocalSaveReader;cloudSync?:CloudSaveSync|null}) {
  const {locale,t} = useLocale();
  const {hash,search} = useLocation();
  const navigate = useNavigate();
  const aliasSearch = pathname === '/profile.html' ? search : '';
  const aliasHash = pathname === '/profile.html' ? hash : '';
  const signInHref = pathname === '/profile.html' ? withUiLanguage('/login.html?next='+encodeURIComponent(pathname+aliasSearch+aliasHash),locale) : '';
  const panel = ['#edit','#settings','#security','#game-settings'].includes(hash) ? hash : '';
  const [state,setState] = useState<{profile?:RemoteProfile; owner?:boolean; error?:UiMessageKey}>({});
  const [attempt,setAttempt] = useState(0);
  // biome-ignore lint/correctness/useExhaustiveDependencies: The retry counter starts a new request for the same profile.
  useEffect(() => {
    const controller = new AbortController();
    let generation = 0, poll: ReturnType<typeof setTimeout> | undefined, latest: RemoteProfile | undefined;
    setState({});
    const refresh = () => {if (latest?.reviewPending !== undefined) void load().catch(schedule);};
    const schedule = () => {
      clearTimeout(poll);
      if (controller.signal.aborted || !latest?.reviewPending) return;
      poll = setTimeout(() => {
        if (document.visibilityState === 'visible') refresh();
        else schedule();
      }, 10_000);
    };
    async function load() {
      const request = ++generation;
      const name = pathname.startsWith('/u/') ? decodeURIComponent(pathname.slice(3).replace(/\/$/, '')) : viewer;
      if (!name) {
        if (!signInHref) throw new AccountRequestError('PROFILE_NOT_FOUND',404);
        if (!controller.signal.aborted) navigate(signInHref,{replace:true});
        return;
      }
      if (pathname === '/profile.html') {
        if (!controller.signal.aborted) navigate({pathname:'/u/'+encodeURIComponent(name),search:aliasSearch,hash:aliasHash},{replace:true});
        return;
      }
      const profile = await accountRequest<RemoteProfile>(`v1/profiles/${encodeURIComponent(name)}`,undefined,{signal:controller.signal});
      if (controller.signal.aborted || request !== generation) return;
      const previous = latest;
      latest = profile;
      const owner = viewer === profile.username;
      setState({profile,owner});
      schedule();
      if (owner && previous?.reviewPending && (previous.nickname !== profile.nickname ||
        previous.details?.avatar !== profile.details?.avatar || previous.roomAvatarReady !== profile.roomAvatarReady)) {
        window.dispatchEvent(new Event('account-profile-change'));
        notifyRoomProfileChange();
      }
    }
    load().catch(error => {if (!controller.signal.aborted) setState({error:error instanceof AccountRequestError && error.status === 404 ? 'profile.notFound' : 'account.requestFailed'});});
    window.addEventListener('account-profile-change',refresh);
    return () => {controller.abort();clearTimeout(poll);window.removeEventListener('account-profile-change',refresh);};
  }, [pathname,viewer,aliasSearch,aliasHash,signInHref,navigate,attempt]);
  if (state.profile) return <ProfileBody panel={panel} remote={state.profile} owner={!!state.owner} readLocalSaves={readLocalSaves} cloudSync={cloudSync}/>;
  return <main className="player-profile"><div className="profile-wrap" aria-busy={!state.error}>
    {state.error && <p role="alert">{t(state.error)} {state.error !== 'profile.notFound' && <Button variant="outline" onClick={()=>setAttempt(value=>value+1)}>{t('cloudSave.refresh')}</Button>}</p>}
  </div></main>;
}
function ProfileBody({remote,owner,panel,readLocalSaves,cloudSync}: {remote:RemoteProfile; owner:boolean; panel:string;readLocalSaves?:LocalSaveReader;cloudSync?:CloudSaveSync|null}) {
  const {locale,t}=useLocale();
  const location=useLocation(),navigate=useNavigate();
  const anchor=location.hash.replace(/^#profile-/,''),tab:ProfileTab=Object.hasOwn(tabLabels,anchor)?anchor as ProfileTab:'overview';
  const sectionAddress=(key:ProfileTab)=>({pathname:location.pathname,search:location.search,hash:`#profile-${key}`});
  const [legacySaved, setLegacySaved] = useState<{source:RemoteProfile;profile:Profile;display:ProfileDisplay}>();
  const local = legacySaved?.source === remote ? legacySaved : undefined;
  const profile = local?.profile ?? {...blankProfile,...remote.details,name:remote.nickname,roomAvatarReady:remote.roomAvatarReady};
  const settings = owner && !!panel;
  const display = local?.display ?? (validDisplay(remote.details?.display) ? remote.details.display : defaultDisplay);
  const [statistics,setStatistics]=useState<ProfileStatistics>(),[statisticsError,setStatisticsError]=useState(false);
  useEffect(()=>{
    let current:AbortController|undefined;
    const load=()=>{current?.abort();const controller=new AbortController();current=controller;void getProfileStatistics(remote.username,controller.signal).then(value=>{if(!controller.signal.aborted){setStatistics(value);setStatisticsError(false);}}).catch(()=>{if(!controller.signal.aborted)setStatisticsError(true);});};
    load();window.addEventListener('cloud-save-change',load);
    return()=>{current?.abort();window.removeEventListener('cloud-save-change',load);};
  },[remote.username]);
  const visitor = !owner;
  const editProfile = () => navigate({pathname:location.pathname,search:location.search,hash:'#edit'});
  const visible = visitor ? display.visible : defaultDisplay.visible;
  const visibleTabs: ProfileTab[] = ['overview',...display.order.filter((key): key is Exclude<ProfileTab,'overview'> => key==='friends'&&visible.friends || key==='career'&&!!statistics?.career&&statistics.games.some(item=>item.statistics.progress.length>0) || key==='records'&&!!statistics?.records&&statistics.games.some(item=>item.statistics.playtimeSeconds!==null||item.statistics.playCount!==null))];
  const links = [...new Map(profile.links.map(link => [link.url,link])).values()];
  const details = <><p className="profile-bio">{profile.bio || t('profile.bio.missing')}</p><dl className="profile-about-meta">
    <div><dt><CalendarDays/>{t('profile.registeredAt')}</dt><dd><time dateTime={remote.joinedAt}>{new Intl.DateTimeFormat(locale,{year:'numeric',month:'long',timeZone:'UTC'}).format(new Date(remote.joinedAt))}</time></dd></div>
    </dl>{links.length > 0 && <div className="profile-links">{links.map(link => {
      const host = safeUrl(link.url)?.hostname ?? '';
      const Icon = /(^|\.)github\.com$/.test(host) ? CodeXml : /(^|\.)(discord\.com|discord\.gg)$/.test(host) ? MessageCircle : /(^|\.)(bilibili\.com|b23\.tv)$/.test(host) ? TvMinimal : Link2;
      return <a key={link.url} href={link.url} target="_blank" rel="noopener noreferrer"><Icon aria-hidden="true"/><span>{link.label}</span><ArrowUpRight aria-hidden="true"/></a>;
    })}</div>}</>;
  const navigation = visibleTabs.map(item => <Link key={item} to={sectionAddress(item)} aria-current={tab===item ? 'location' : undefined}>{t(tabLabels[item])}</Link>);
  if (settings) return <AccountSettings initialProfile={profile} initialDisplay={display} reviewPending={remote.reviewPending}
    panel={panel} readLocalSaves={readLocalSaves} cloudSync={cloudSync}
    onSave={(next,visibility) => {if (remote.reviewPending === undefined) setLegacySaved({source:remote,profile:next,display:visibility});}}/>;
  return <SocialProvider key={remote.username} username={remote.username}><main className="player-profile profile-page" aria-labelledby="profile-name"><div className="profile-wrap">
      <section id="profile-overview" className="profile-hero profile-public-section">
        <div className="profile-cover"><img src={imageData(profile.cover) ? profile.cover : backgroundUrl} alt=""/></div>
        <div className="profile-identity"><UiButton variant="unstyled" type="button" className="profile-avatar" aria-label={t('profile.avatar.change')} disabled={visitor} onClick={editProfile}><AccountAvatar src={profile.avatar} alt={t('profile.avatar.alt')}/></UiButton>
        <div className="profile-name"><h1 id="profile-name" tabIndex={-1}><OverflowText>{profile.name || t('profile.name.missing')}</OverflowText>{remote.countryCode && <CountryFlag code={remote.regionCode || remote.countryCode} label={regionLabel(remote.regionCode || remote.countryCode,locale)}/>}</h1><Link className="profile-username" to={'/u/'+remote.username}>@{remote.username}</Link></div>
        <div className="profile-header-details">{details}</div><div className="profile-actions">{!owner && <SocialFollowControl username={remote.username}/>}{owner&&<Button variant="outline" asChild><Link to={{pathname:location.pathname,search:location.search,hash:'#game-settings'}}>{t('cloudSave.title')}</Link></Button>}{!visitor && <BookButton variant="action" type="button" onClick={editProfile}>{t('profile.edit')}</BookButton>}</div></div>
        {<nav className="profile-section-nav profile-nav-mobile" aria-label={t('profile.navigationSections')}>{navigation}</nav>}
      </section>
<div className="profile-columns"><aside className="profile-aside"><div className="profile-sidebar-details">{details}</div><nav className="profile-section-nav profile-nav-desktop" aria-label={t('profile.navigationSections')}>{navigation}</nav></aside>
        <div className="profile-content">{statistics&&<Suspense fallback={null}><ProfileGameData data={statistics} order={display.order.filter(key=>key==='career')} owner={owner} settingsHref={location.pathname+location.search+'#game-settings'}/></Suspense>}
        <ProfileTh09Duels key={remote.username} username={remote.username} owner={owner}/>
        {statistics&&<Suspense fallback={null}><ProfileGameData data={statistics} order={display.order.filter(key=>key!=='career')} owner={owner} settingsHref={location.pathname+location.search+'#game-settings'}/></Suspense>}
        {statisticsError&&owner&&<p role="alert">{t('cloudSave.statisticsFailed')} <UiButton variant="quiet" onClick={()=>window.dispatchEvent(new Event('cloud-save-change'))}>{t('cloudSave.refresh')}</UiButton></p>}
        {visible.friends && <SocialSection username={remote.username}/>}
        <div className="profile-end-actions"><BackToTop/></div>
        </div></div>
  </div></main></SocialProvider>;
}

function regionLabel(code:string,locale:string) {
  if (typeof Intl.DisplayNames !== 'function') return ['HK','MO','TW'].includes(code) ? 'CN / '+code : code;
  const names = new Intl.DisplayNames([locale],{type:'region'});
  const special = ['HK','MO','TW'].includes(code);
  const label = special ? names.of('CN')+' | '+new Intl.DisplayNames([locale],{type:'region',style:'short'}).of(code) : names.of(code);
  return label || code;
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
