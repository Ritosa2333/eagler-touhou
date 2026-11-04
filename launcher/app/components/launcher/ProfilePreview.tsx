import {AccountRequestError, accountRequest} from '../../services/account-client';
import {Input, Textarea} from '../../ui/input';
import {Tabs} from 'radix-ui';
import backgroundUrl from '../../../public/assets/launcher-background.webp?url';
import {SiteBrand} from './SiteBrand';
import {ArrowUp, ArrowUpRight, CalendarDays, CodeXml, Link2, MapPin, MessageCircle, TvMinimal, UserRound} from 'lucide-react';
import {OverflowText} from '../../ui/overflow-text';
import {Toggle, Button as UiButton} from '../../ui/controls';
import {BookButton} from './BookButton';
import {Fragment, useEffect, useState, type ReactNode} from 'react';
import {flushSync} from 'react-dom';
import {AppLink} from './AppLink';
import {Link, useLocation, useNavigate} from 'react-router';
import {useLocale, type UiMessageKey} from '../../i18n';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';
import {readBookPreference, writeBookPreference} from './book-preferences';
import './profile-preview.css';
import {SocialFollowControl, SocialSection} from './SocialSection';
import {NavigationIcon} from './NavigationIcon';
import {Button as NavigationButton} from '../../ui/button';
import {animateSurface, type SurfaceAnimation} from '../../ui/surface-motion';

let profileAnimation: SurfaceAnimation | null = null;
function transitionProfile(update: () => void, scope: 'page' | 'content' | 'settings' = 'page') {
  profileAnimation?.cancel();
  flushSync(update);
  const target = document.querySelector<HTMLElement>(scope === 'content' ? '.profile-content' : scope === 'settings' ? '.profile-editor-shell' : '.profile-wrap');
  profileAnimation = target ? animateSurface(target, 'enter', {kind: 'content'}) : null;
}

type SocialLink = {label: string; url: string};
type Profile = {name: string; bio: string; region: string; avatar: string; cover: string; links: SocialLink[]};
type ProfileDraft = Omit<Profile, 'links'> & {links: (SocialLink & {editId: number})[]};
let nextLinkEditId = 0;
const editableLink = (link: SocialLink) => ({...link, editId: ++nextLinkEditId});
type Guide = {id: string; title: string; body: string};
type DisplayKey = 'friends' | 'career' | 'records' | 'content';
type OverviewKey = DisplayKey | 'favorites';
type ProfileDisplay = {visible: Record<DisplayKey, boolean>; order: OverviewKey[]};
type AccountSession = {user?: {username?: string} | null};
const displayLabels: Record<DisplayKey, UiMessageKey> = {friends:'profile.tab.friends',career:'profile.tab.career',records:'profile.tab.records',content:'profile.tab.content'};
const overviewLabels: Record<OverviewKey, UiMessageKey> = {career:'profile.clearChart',records:'profile.tab.records',content:'profile.tab.content',friends:'profile.tab.friends',favorites:'profile.favorites'};
const tabLabels = {overview:'profile.tab.overview',career:'profile.tab.career',records:'profile.tab.records',content:'profile.tab.content',friends:'profile.tab.friends'} as const;
type ProfileTab = keyof typeof tabLabels;
const defaultDisplay: ProfileDisplay = {visible: {friends: true, career: true, records: true, content: true}, order: ['career', 'records', 'content', 'friends', 'favorites']};
const validDisplay = (value: unknown): value is ProfileDisplay => {
  if (!value || typeof value !== 'object') return false;
  const settings = value as ProfileDisplay;
  return !!settings.visible && Object.keys(displayLabels).every(key => typeof settings.visible[key as DisplayKey] === 'boolean') &&
    Array.isArray(settings.order) && settings.order.length === 5 && new Set(settings.order).size === 5 && settings.order.every(key => Object.hasOwn(overviewLabels, key));
};
const blankProfile: Profile = {name: '', bio: '', region:'', avatar: '', cover: '', links: []};
const imageData = (value: string) => /^data:image\/(png|jpeg|webp);base64,/i.test(value);
const safeUrl = (value: string) => {try {const url = new URL(value); return ['http:', 'https:'].includes(url.protocol) ? url : null;} catch {return null;}};
const validGuides = (value: unknown): value is Guide[] => Array.isArray(value) && value.every(item => item && typeof item.id === 'string' && typeof item.title === 'string' && typeof item.body === 'string');

/** Browser-local presentation only. No account or room identity is created. */
export function ProfilePreview({interactive = true, active, href}: {interactive?: boolean; active: boolean; href: string}) {
  const {t} = useLocale();
  const [avatar, setAvatar] = useState('');
  useEffect(() => {
    if (!interactive) return;
    let active = true;
    async function refresh() {
      const session = await accountRequest<AccountSession>('auth/get-session');
      if (!session?.user?.username) {if (active) setAvatar('');return;}
      const profile = await accountRequest<RemoteProfile>(`v1/profiles/${encodeURIComponent(session.user.username)}`);
      if (active) setAvatar(profile.details?.avatar || '');
    }
    const update = () => {void refresh().catch(() => {});};
    update(); window.addEventListener('account-profile-change', update);
    return () => {active=false;window.removeEventListener('account-profile-change',update);};
  }, [interactive]);
  const content = <><span aria-hidden="true">{imageData(avatar) ? <img src={avatar} alt=""/> : <NavigationIcon name="profile"/>}</span><svg className="book-profile-trace" viewBox="0 0 40 40" aria-hidden="true"><rect x="1.5" y="1.5" width="37" height="37" rx="7" pathLength="100"/></svg></>;
  return <NavigationButton asChild variant="ghost" size="icon" className="masthead-action book-profile-trigger [&_svg]:size-5">{interactive ? <AppLink href={href} aria-label={t('profile.navigation')} aria-current={active ? 'page' : undefined}>{content}</AppLink>
    : <a href={href} aria-label={t('profile.navigation')} aria-current={active ? 'page' : undefined}>{content}</a>}</NavigationButton>;
}

type RemoteProfile = {username: string; nickname: string; joinedAt: string; details: (Profile & {display:ProfileDisplay}) | null};
export function ProfilePage() {
  const {pathname} = useLocation();
  return <AccountProfile key={pathname} pathname={pathname}/>;
}
function AccountProfile({pathname}: {pathname:string}) {
  const {locale,t} = useLocale();
  const [state,setState] = useState<{profile?:RemoteProfile; owner?:boolean; error?:UiMessageKey; guest?:boolean}>({});
  useEffect(() => {
    let current = true;
    async function load() {
      const session = await accountRequest<AccountSession>('auth/get-session');
      const name = pathname.startsWith('/u/') ? decodeURIComponent(pathname.slice(3).replace(/\/$/, '')) : session?.user?.username;
      if (!name) {if (current) setState({guest:true}); return;}
      const profile = await accountRequest<RemoteProfile>(`v1/profiles/${encodeURIComponent(name)}`);
      if (current) setState({profile,owner:session?.user?.username === profile.username});
    }
    load().catch(error => {if (current) setState({error:error instanceof AccountRequestError && error.status === 404 ? 'profile.notFound' : 'account.requestFailed'});});
    return () => {current = false;};
  }, [pathname]);
  if (state.profile) return <ProfileBody remote={state.profile} owner={!!state.owner}/>;
  return <main className="player-profile"><div className="profile-wrap" aria-busy={!state.error && !state.guest}>
    {state.error && <p role="alert">{t(state.error)}</p>}
    {state.guest && <AppLink href={withUiLanguage('login.html',locale)}>{t('profile.signIn')}</AppLink>}
  </div></main>;
}
function ProfileBody({remote,owner}: {remote:RemoteProfile; owner:boolean}) {
  const {locale,t}=useLocale();
  const location=useLocation(),navigate=useNavigate();
  const anchor=location.hash.replace(/^#profile-/,''),tab:ProfileTab=Object.hasOwn(tabLabels,anchor)?anchor as ProfileTab:'overview';
  const sectionAddress=(key:ProfileTab)=>({pathname:location.pathname,search:location.search,hash:`#profile-${key}`});
  const jumpTo=(key:ProfileTab,replace=false)=>navigate(sectionAddress(key),{replace});
  const [profile, setProfile] = useState<Profile>({...blankProfile,...remote.details,name:remote.nickname}), [draft, setDraft] = useState<ProfileDraft>({...blankProfile, links: []});
  const [editing, setEditing] = useState(false);
  const [display, setDisplay] = useState(validDisplay(remote.details?.display) ? remote.details.display : defaultDisplay), [displayDraft, setDisplayDraft] = useState(defaultDisplay);
  const [displayEditing, setDisplayEditing] = useState(false);
  const [settings, setSettings] = useState(false), [security, setSecurity] = useState(false);
  const [visitor, setVisitor] = useState(!owner);
  const [saveError,setSaveError] = useState('');
  const [guides, setGuides] = useState<Guide[]>([]), [guideDraft, setGuideDraft] = useState<Guide | null>(null);
  const [article, setArticle] = useState<Guide | null>(null);
  const [notice, setNotice] = useState<UiMessageKey|null>(null), [error, setError] = useState<UiMessageKey|null>(null);
  useEffect(() => {if (settings) window.scrollTo(0, 0);}, [settings]);
  useEffect(() => () => {profileAnimation?.cancel(); profileAnimation = null;}, []);
  useEffect(() => {
    if (!owner) return;
    setGuides(readBookPreference('player-guide-drafts-' + remote.username, [], validGuides));

  }, []);
  useEffect(() => {if (!notice) return; const timer = window.setTimeout(() => setNotice(null), 3500); return () => window.clearTimeout(timer);}, [notice]);
  const back = () => transitionProfile(() => {setSettings(false); setSecurity(false); if (displayEditing) setDisplayEditing(false); else if (editing) setEditing(false); else if (guideDraft) setGuideDraft(null); else if (article) setArticle(null); setError(null);});
  const editProfile = () => transitionProfile(() => {setSettings(true); setSecurity(false); setDraft({...profile, links: profile.links.map(editableLink)}); setDisplayEditing(false); setEditing(true); setError(null);}, settings ? 'settings' : 'page');
  const uploadImage = (file: File | undefined, field: 'avatar' | 'cover') => {
    if (!file) return;
    if (!['image/png', 'image/jpeg', 'image/webp'].includes(file.type) || file.size > 750_000) {setError("profile.image.invalid"); return;}
    const reader = new FileReader();
    reader.onload = () => {if (typeof reader.result === 'string') {const value = reader.result; setDraft(current => ({...current, [field]: value})); setError(null);}};
    reader.onerror = () => setError("profile.image.readFailed"); reader.readAsDataURL(file);
  };
  const saveProfile = async () => {
    if (draft.links.some(link => !link.label.trim() || !safeUrl(link.url))) {setError("profile.links.invalid"); return;}
    const next = {...draft, name: draft.name.trim(), links: draft.links.map(link => ({label: link.label.trim(), url: safeUrl(link.url)!.href}))};
    setSaveError('');
    try {
      if (next.name !== profile.name) {
        await accountRequest('auth/update-user', {name:next.name});
        setProfile(previous => ({...previous,name:next.name}));
      }
      await accountRequest('v1/me/profile', {...next,display});
    } catch (cause) {setSaveError(t('account.requestFailed')); return;}
    setProfile(next); window.dispatchEvent(new Event('account-profile-change')); setError(null); setNotice("profile.saved");
  };
  const saveGuide = () => {
    if (!guideDraft?.title.trim() || !guideDraft.body.trim()) {setError("profile.draft.required"); return;}
    const next = guides.some(guide => guide.id === guideDraft.id) ? guides.map(guide => guide.id === guideDraft.id ? guideDraft : guide) : [...guides, guideDraft];
    setGuides(next); writeBookPreference('player-guide-drafts-' + remote.username, next); setGuideDraft(null); setArticle(null); setError(null); setNotice("profile.draft.saved");
  };
  const startGuide = () => {setGuideDraft({id: crypto.randomUUID(), title: '', body: ''}); setError(null);};
  const editDisplay = () => transitionProfile(() => {setSettings(true); setSecurity(false); setDisplayDraft({visible: {...display.visible}, order: [...display.order]}); setDisplayEditing(true); setEditing(false); setArticle(null); setGuideDraft(null); setError(null);}, settings ? 'settings' : 'page');
  const saveDisplay = async () => {
    setSaveError('');
    try {await accountRequest('v1/me/profile', {...profile,display:displayDraft});}
    catch (cause) {setSaveError(t('account.requestFailed')); return;}
    setDisplay(displayDraft); setNotice("profile.display.saved");
    const tabKey = tab==="overview" ? null : tab;
    if (visitor && tabKey && !displayDraft.visible[tabKey]) jumpTo('overview',true);
  };
  const moveOverview = (key: OverviewKey, offset: number) => setDisplayDraft(current => {
    const order = [...current.order], index = order.indexOf(key), target = index + offset;
    if (target < 0 || target >= order.length) return current;
    [order[index], order[target]] = [order[target], order[index]];
    return {...current, order};
  });
  const visible = visitor ? display.visible : defaultDisplay.visible;
  const visibleTabs: ProfileTab[] = ['overview',...display.order.filter((key): key is Exclude<ProfileTab,'overview'> => key!=='favorites' && key!=='career' && visible[key])];
  const activeTab = settings ? editing ? 'settings-profile' : displayEditing ? 'settings-display' : 'settings-security' : tab;
  const selectTab = (value: string) => {
    if (value === activeTab) return;
    if (value === 'settings-profile') editProfile();
    else if (value === 'settings-display') editDisplay();
    else if (value === 'settings-security') transitionProfile(() => {setEditing(false); setDisplayEditing(false); setSecurity(true); setError(null);}, 'settings');
  };
  const links = [...new Map(profile.links.map(link => [link.url,link])).values()];
  const sections: Record<OverviewKey,ReactNode> = {
    career: null,
    records: visible.records && <section id="profile-records" className="profile-panel profile-public-section"><div className="profile-section-title"><h2>{t('profile.tab.records')}</h2></div><EmptyState title={t('profile.stats.empty')} detail={t('profile.stats.hint')}/></section>,
    content: visible.content && <div id="profile-content" className="profile-public-section">{guideDraft ? <section className="profile-panel profile-editor"><h2>{guides.some(guide => guide.id === guideDraft.id) ? t('profile.draft.edit') : t('profile.draft.newHeading')}</h2><form onSubmit={event => {event.preventDefault(); saveGuide();}}><label>{t('profile.draft.title')}<Input value={guideDraft.title} maxLength={100} required onChange={event => setGuideDraft({...guideDraft, title: event.target.value})}/></label><label>{t('profile.draft.body')}<Textarea rows={14} value={guideDraft.body} maxLength={30000} required onChange={event => setGuideDraft({...guideDraft, body: event.target.value})}/></label>{error && <p role="alert">{t(error)}</p>}<div className="profile-actions"><BookButton variant="action" type="button" onClick={() => setGuideDraft(null)}>{t('action.cancel')}</BookButton><BookButton variant="primary" type="submit" className="profile-primary">{t('profile.draft.save')}</BookButton></div></form></section> : article ? <article className="profile-panel profile-article"><div className="profile-section-title"><BookButton variant="quiet" type="button" onClick={() => setArticle(null)}>{t('profile.draft.back')}</BookButton>{!visitor && <BookButton variant="quiet" type="button" onClick={() => setGuideDraft({...article})}>{t('profile.editAction')}</BookButton>}</div><h2>{article.title}</h2><div className="ui-document prose"><p className="profile-guide-body">{article.body}</p></div></article> : <section className="profile-panel"><div className="profile-section-title"><h2>{t('profile.draft.heading')}</h2>{!visitor && <BookButton variant="quiet" type="button" onClick={startGuide}>{t('profile.draft.new')}</BookButton>}</div>{guides.length ? <div className="profile-draft-list">{guides.map(guide => <BookButton variant="action" key={guide.id} type="button" onClick={() => setArticle(guide)}><span><strong>{guide.title}</strong><small>{guide.body.slice(0, 100)}</small></span><span>{t('profile.draft.local')}</span></BookButton>)}</div> : <EmptyState title={t('profile.draft.empty')} detail={t('profile.draft.emptyHint')}/>}</section>}</div>,
    friends: visible.friends && <SocialSection username={remote.username}/>,
    favorites: null,
  };
  const details = <><p className="profile-bio">{profile.bio || t('profile.bio.missing')}</p><dl className="profile-about-meta">
    {profile.region && <div><dt><MapPin/>{t('profile.region')}</dt><dd>{profile.region}</dd></div>}
    <div><dt><CalendarDays/>{t('profile.registeredAt')}</dt><dd><time dateTime={remote.joinedAt}>{new Intl.DateTimeFormat(locale,{year:'numeric',month:'long',timeZone:'UTC'}).format(new Date(remote.joinedAt))}</time></dd></div>
    </dl>{links.length > 0 && <div className="profile-links">{links.map(link => {
      const host = safeUrl(link.url)?.hostname ?? '';
      const Icon = /(^|\.)github\.com$/.test(host) ? CodeXml : /(^|\.)(discord\.com|discord\.gg)$/.test(host) ? MessageCircle : /(^|\.)(bilibili\.com|b23\.tv)$/.test(host) ? TvMinimal : Link2;
      return <a key={link.url} href={link.url} target="_blank" rel="noopener noreferrer"><Icon aria-hidden="true"/><span>{link.label}</span><ArrowUpRight aria-hidden="true"/></a>;
    })}</div>}</>;
  const navigation = visibleTabs.filter(item => item !== 'records' && (item !== 'content' || owner)).map(item => <Link key={item} to={sectionAddress(item)} aria-current={tab===item ? 'location' : undefined}>{t(tabLabels[item])}</Link>);
  return <Tabs.Root value={activeTab} onValueChange={selectTab} asChild><main className={`player-profile${settings ? ' profile-editing profile-settings' : ' profile-page'}`} aria-label={settings ? t('profile.settings') : undefined} aria-labelledby={settings ? undefined : 'profile-name'}>
    {(settings || article || guideDraft) && <div className="profile-topbar"><BookButton variant="quiet" type="button" onClick={back}>{t('profile.back')}</BookButton></div>}
    <div className="profile-wrap">{saveError && <p role="alert">{saveError}</p>}
      {settings && <div className="profile-settings-heading"><h1>{t('profile.settings')}</h1><Tabs.List asChild><nav aria-label={t('profile.settingsNavigation')}>{[['settings-profile',t('profile.edit')],['settings-display',t('profile.displayTab')],['settings-security',t('profile.security')]].map(([value,label]) => <Tabs.Trigger asChild key={value} value={value}><BookButton variant="tab" aria-current={activeTab===value ? 'page' : undefined}>{label}</BookButton></Tabs.Trigger>)}</nav></Tabs.List></div>}
      <section id="profile-overview" className="profile-hero profile-public-section">
        <div className="profile-cover"><img src={imageData(profile.cover) ? profile.cover : backgroundUrl} alt=""/></div>
        <div className="profile-identity"><UiButton variant="unstyled" type="button" className="profile-avatar" aria-label={t('profile.avatar.change')} disabled={visitor} onClick={editProfile}>{imageData(profile.avatar) ? <img src={profile.avatar} alt={t('profile.avatar.alt')}/> : <SiteBrand className="profile-default-avatar" assetUrl={path => '/' + path.replace(/^\/+/, '')}/>}</UiButton>
        <div className="profile-name"><h1 id="profile-name" tabIndex={-1}><OverflowText>{profile.name || t('profile.name.missing')}</OverflowText></h1><Link className="profile-username" to={'/u/'+remote.username}>/u/{remote.username}</Link></div>
        {!settings && <div className="profile-header-details">{details}</div>}<div className="profile-actions">{owner && <BookButton variant="action" type="button" className="profile-perspective" aria-label={t('profile.perspective.toggle')} aria-pressed={visitor} onClick={() => transitionProfile(() => {setVisitor(value => !value); jumpTo('overview',true); setArticle(null); setGuideDraft(null);}, 'content')}>{t('profile.perspective.prefix')}{visitor ? t('profile.visitor') : t('profile.self')}<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M4 7h15m-4-4 4 4-4 4M20 17H5m4-4-4 4 4 4"/></svg></BookButton>}{!owner && <SocialFollowControl username={remote.username}/>}{!visitor && <BookButton variant="action" type="button" onClick={editProfile}>{t('profile.settings')}</BookButton>}</div></div>
        {!settings && <nav className="profile-section-nav profile-nav-mobile" aria-label={t('profile.navigationSections')}>{navigation}</nav>}
      </section>
      <ProfileContent settings={settings} value={activeTab}>{settings && security ? <AccountSecurity/> : displayEditing ? <section className="profile-panel profile-editor-shell profile-display-editor"><div className="profile-section-title"><h2><ProfileSectionIcon name="layout"/>{t('profile.display')}</h2></div><form onSubmit={event => {event.preventDefault(); saveDisplay();}}>
        <div className="profile-display-columns"><fieldset><legend>{t('profile.display.visible')}</legend><div className="profile-display-options">{(Object.keys(displayLabels) as DisplayKey[]).map(key => <label key={key}><span className="profile-option-name"><ProfileSectionIcon name={key}/><span>{t(displayLabels[key])}</span></span><Toggle checked={displayDraft.visible[key]} onCheckedChange={event => setDisplayDraft({...displayDraft, visible: {...displayDraft.visible, [key]: event}})}/></label>)}</div></fieldset>
        <fieldset><legend>{t('profile.display.order')}</legend><ol className="profile-display-order">{displayDraft.order.map((key, index) => <li key={key} data-overview-key={key} className={key !== 'favorites' && !displayDraft.visible[key] ? 'is-hidden' : undefined}><div className={`profile-order-art profile-order-art-${key}`} aria-hidden="true"><ProfileSectionIcon name={key}/><i/><i/><i/></div><span className="profile-order-label"><small>{String(index + 1).padStart(2, '0')}</small>{t(overviewLabels[key])}</span><div><BookButton variant="action" type="button" aria-label={t('profile.moveUp',{name:t(overviewLabels[key])})} disabled={index === 0} onClick={() => moveOverview(key, -1)}>↑</BookButton><BookButton variant="action" type="button" aria-label={t('profile.moveDown',{name:t(overviewLabels[key])})} disabled={index === displayDraft.order.length - 1} onClick={() => moveOverview(key, 1)}>↓</BookButton></div></li>)}</ol></fieldset></div>
        <div className="profile-actions profile-editor-footer"><BookButton variant="action" type="button" onClick={back}>{t('action.cancel')}</BookButton><BookButton variant="primary" type="submit" className="profile-primary">{t('profile.display.save')}</BookButton></div>
      </form></section> : editing ? <section className="profile-panel profile-editor profile-editor-shell profile-details-editor"><div className="profile-section-title"><h2><ProfileSectionIcon name="profile"/>{t('profile.editHeading')}</h2></div><form onSubmit={event => {event.preventDefault(); saveProfile();}}>
        <div className="profile-details-columns"><div className="profile-media-editor"><div className="profile-draft-preview"><div className={`profile-draft-cover${imageData(draft.cover) ? ' has-image' : ''}`} style={imageData(draft.cover) ? {backgroundImage: `url("${draft.cover}")`} : undefined}/><div className="profile-draft-identity"><span className="profile-draft-avatar">{imageData(draft.avatar) ? <img src={draft.avatar} alt={t('profile.avatar.preview')}/> : <AvatarPlaceholder/>}</span><strong>{draft.name || t('profile.name.missing')}</strong><p>{draft.bio.split('\n')[0] || t('profile.bio.missing')}</p></div></div>
        <div className="profile-image-fields">{(['avatar', 'cover'] as const).map(field => <fieldset key={field}><legend>{field === 'avatar' ? t('profile.avatar') : t('profile.cover')}</legend><label className="profile-image-upload"><ProfileSectionIcon name="upload"/><span>{t('profile.image.choose')}</span><input type="file" aria-label={field === 'avatar' ? t('profile.avatar.choose') : t('profile.cover.choose')} accept="image/png,image/jpeg,image/webp" onChange={event => uploadImage(event.target.files?.[0], field)}/></label>{draft[field] && <BookButton variant="quiet" type="button" className="profile-image-remove" onClick={() => setDraft({...draft, [field]: ''})}>{t('profile.remove')}</BookButton>}</fieldset>)}</div><small className="profile-upload-hint">{t('profile.image.formats')}</small></div><div className="profile-detail-fields">
        <label>{t('profile.name')}<Input maxLength={32} value={draft.name} onChange={event => setDraft({...draft, name: event.target.value})} placeholder={t('profile.name.placeholder')}/></label>
        <label>{t('profile.region')}<Input maxLength={48} value={draft.region} onChange={event => setDraft({...draft,region:event.target.value})}/></label><label>{t('profile.bio')}<Textarea rows={4} maxLength={500} value={draft.bio} onChange={event => setDraft({...draft, bio: event.target.value})} placeholder={t('profile.bio.placeholder')}/><small>{draft.bio.length} / 500</small></label>
        <fieldset><legend>{t('profile.links')}</legend>{draft.links.map((link, index) => <div className="profile-link-editor" key={link.editId}><label>{t('profile.links.name')}<Input maxLength={40} value={link.label} onChange={event => setDraft({...draft, links: draft.links.map((item, i) => i === index ? {...item, label: event.target.value} : item)})}/></label><label>{t('profile.links.address')}<Input type="url" placeholder="https://" value={link.url} onChange={event => setDraft({...draft, links: draft.links.map((item, i) => i === index ? {...item, url: event.target.value} : item)})}/></label><BookButton variant="action" type="button" aria-label={t('profile.links.delete',{number:index + 1})} onClick={() => setDraft({...draft, links: draft.links.filter((_, i) => i !== index)})}>{t('action.delete')}</BookButton></div>)}<BookButton variant="action" type="button" onClick={() => setDraft({...draft, links: [...draft.links, editableLink({label: '', url: ''})]})}>{t('profile.links.add')}</BookButton></fieldset></div></div>
        {error && <p role="alert">{t(error)}</p>}<div className="profile-actions profile-editor-footer"><BookButton variant="action" type="button" onClick={back}>{t('action.cancel')}</BookButton><BookButton variant="primary" className="profile-primary" type="submit">{t('profile.save')}</BookButton></div>
      </form></section> : <div className="profile-columns"><aside className="profile-aside"><div className="profile-sidebar-details">{details}</div><nav className="profile-section-nav profile-nav-desktop" aria-label={t('profile.navigationSections')}>{navigation}</nav></aside>
        <div className="profile-content">{display.order.filter(key => key !== 'records' && key !== 'career' && key !== 'favorites' && (key !== 'content' || owner)).map(key => <Fragment key={key}>{sections[key]}</Fragment>)}
        <div className="profile-end-actions"><BackToTop/>{owner && <BookButton type="button" variant="quiet" onClick={async () => {
          try {await accountRequest('auth/sign-out', {}); window.location.assign('/login.html');} catch (cause) {setSaveError(t('account.requestFailed'));}
        }}>{t('account.signOut')}</BookButton>}</div>
        </div></div>}
      </ProfileContent>
    </div>{notice && <div className="profile-toast" role="status">{t(notice)}</div>}</main></Tabs.Root>;
}

function ProfileContent({settings,value,children}: {settings:boolean;value:string;children:ReactNode}) {
  const content=<div className="profile-tab-panel">{children}</div>;
  return settings ? <Tabs.Content asChild forceMount value={value}>{content}</Tabs.Content> : content;
}

function AccountSecurity() {
  const {t}=useLocale();
  const [email, setEmail] = useState(''), [code, setCode] = useState(''), [sentTo, setSentTo] = useState('');
  const [current, setCurrent] = useState(''), [password, setPassword] = useState(''), [confirmation, setConfirmation] = useState('');
  const [feedback, setFeedback] = useState<UiMessageKey|null>(null);
  const [failure,setFailure] = useState(''), [pending,setPending] = useState(false);
  async function send(path:string, body:Record<string,unknown>, done:UiMessageKey) {
    if (pending) return false;
    setFailure(''); setFeedback(null); setPending(true);
    try {await accountRequest('auth/' + path, body); setFeedback(done); return true;}
    catch (cause) {setFailure(t('account.requestFailed')); return false;}
    finally {setPending(false);}
  }
  return <section className="profile-panel profile-editor-shell profile-security-editor">
    <div className="profile-section-title"><h2>{t('profile.security')}</h2></div>
    <div className="profile-security-body">
      <section className="profile-security-section"><h3>{t('profile.security.changeEmail')}</h3><form method="post" onSubmit={async event => {event.preventDefault(); if (sentTo !== email.trim()) {setFeedback("profile.security.emailCodeRequired");return;} if (await send("email-otp/change-email",{newEmail:email.trim(),otp:code},"profile.security.emailDone")) {setCode('');setSentTo('');}}}>
        <label>{t('profile.security.newEmail')}<Input type="email" name="new-email" autoComplete="email" required value={email} onChange={event => {setEmail(event.target.value); setCode(''); setSentTo('');}} placeholder="name@example.com"/></label>
        <label>{t('profile.security.emailCode')}<div className="profile-security-code"><Input name="email-code" autoComplete="one-time-code" inputMode="numeric" pattern="[0-9]{6}" maxLength={6} required value={code} onChange={event => setCode(event.target.value.replace(/\D/g, ''))} placeholder={t('profile.security.codePlaceholder')}/><BookButton variant="action" type="button" disabled={pending} onClick={async () => {if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email.trim())) {setFeedback("profile.security.emailInvalid"); return;} if (await send("email-otp/request-email-change",{newEmail:email.trim()},"account.codeSent")) setSentTo(email.trim());}}>{t('profile.security.getCode')}</BookButton></div></label>
        <BookButton variant="primary" className="profile-primary" type="submit">{t('profile.security.changeEmail')}</BookButton>
      </form></section>
      <section className="profile-security-section"><h3>{t('profile.security.changePassword')}</h3><form method="post" onSubmit={async event => {event.preventDefault(); if (password !== confirmation) {setFeedback("profile.security.passwordMismatch"); return;} if (await send('change-password',{currentPassword:current,newPassword:password,revokeOtherSessions:true},'profile.security.passwordDone')) {setCurrent(''); setPassword(''); setConfirmation('');}}}>
        <label>{t('profile.security.currentPassword')}<Input name="current-password" type="password" autoComplete="current-password" required value={current} onChange={event => setCurrent(event.target.value)}/></label>
        <label>{t('profile.security.newPassword')}<Input name="new-password" type="password" autoComplete="new-password" minLength={8} required value={password} onChange={event => setPassword(event.target.value)} placeholder={t('profile.security.passwordPlaceholder')}/></label>
        <label>{t('profile.security.confirmPassword')}<Input name="confirm-new-password" type="password" autoComplete="new-password" minLength={8} required value={confirmation} onChange={event => setConfirmation(event.target.value)} aria-invalid={confirmation.length > 0 && password !== confirmation ? true : undefined}/></label>
        <BookButton variant="primary" className="profile-primary" type="submit">{t('profile.security.changePassword')}</BookButton>
      </form></section>
      {failure && <p role="alert">{failure}</p>}
      {feedback && <p className="profile-security-feedback" role="status">{t(feedback)}</p>}
    </div>
  </section>;
}

function EmptyState({title, detail}: {title: string; detail?: string}) {return <div className="profile-empty"><h3>{title}</h3>{detail && <p>{detail}</p>}</div>;}
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
function AvatarPlaceholder() {return <UserRound aria-hidden="true" strokeWidth={1.5}/>;}

function ProfileSectionIcon({name}: {name: string}) {
  const paths: Record<string, string> = {
    friends: 'M16 21v-2a4 4 0 0 0-4-4H6a4 4 0 0 0-4 4v2m20 0v-2a4 4 0 0 0-3-3.87M15 3.13a4 4 0 0 1 0 7.75',
    career: 'M4 20V10h4v10m4 0V4h4v16m4 0V8h2v12M2 20h21',
    records: 'M4 5h16v15H4zM8 2v6m8-6v6M8 12h8m-8 4h5',
    content: 'M6 3h9l4 4v14H6zM14 3v5h5M9 12h7m-7 4h5',
    favorites: 'm12 3 2.8 5.7 6.2.9-4.5 4.4 1.1 6.2-5.6-3-5.6 3 1.1-6.2L3 9.6l6.2-.9Z',
    layout: 'M3 3h18v18H3zM3 9h18M10 9v12',
    profile: 'M5 21v-2a7 7 0 0 1 14 0v2',
    upload: 'M12 16V3m-5 5 5-5 5 5M4 15v6h16v-6',
  };
  return <svg className="profile-section-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.6" strokeLinecap="round" strokeLinejoin="round" aria-hidden="true"><path d={paths[name] || paths.layout}/>{name === 'profile' && <circle cx="12" cy="7" r="4"/>}{name === 'friends' && <circle cx="9" cy="7" r="4"/>}</svg>;
}
