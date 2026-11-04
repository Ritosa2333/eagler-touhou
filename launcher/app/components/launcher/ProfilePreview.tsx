import {Input, Textarea} from '../../ui/input';
import {Tabs} from 'radix-ui';
import {UserRound} from 'lucide-react';
import {OverflowText} from '../../ui/overflow-text';
import {Toggle, Button as UiButton} from '../../ui/controls';
import {BookButton} from './BookButton';
import {Fragment, useEffect, useState, type CSSProperties, type ReactNode} from 'react';
import {flushSync} from 'react-dom';
import {AppLink} from './AppLink';
import {Link, useLocation, useNavigate} from 'react-router';
import {useLocale, type UiMessageKey} from '../../i18n';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';
import {PRODUCT_GAMES, gameIdForProduct, isProductId} from '../../../src/contracts/product-catalog.mts';
import {bookGameArtwork} from './book-artwork';
import {BOOK_PREFERENCES_EVENT, readBookPreference, writeBookPreference, stringArray} from './book-preferences';
import './profile-preview.css';
import {CareerShowcase} from './CareerShowcase';
import {CAREER_STORAGE_KEY, loadCareerRecords, type CareerRecords} from './tracker-records';
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
type Profile = {name: string; bio: string; avatar: string; cover: string; links: SocialLink[]};
type ProfileDraft = Omit<Profile, 'links'> & {links: (SocialLink & {editId: number})[]};
let nextLinkEditId = 0;
const editableLink = (link: SocialLink) => ({...link, editId: ++nextLinkEditId});
type Guide = {id: string; title: string; body: string};
type DisplayKey = 'friends' | 'career' | 'records' | 'content';
type OverviewKey = DisplayKey | 'favorites';
type ProfileDisplay = {visible: Record<DisplayKey, boolean>; order: OverviewKey[]};
const displayLabels: Record<DisplayKey, UiMessageKey> = {friends:'profile.tab.friends',career:'profile.tab.career',records:'profile.tab.records',content:'profile.tab.content'};
const overviewLabels: Record<OverviewKey, UiMessageKey> = {career:'profile.clearChart',records:'profile.tab.records',content:'profile.tab.content',friends:'profile.tab.friends',favorites:'profile.favorites'};
const tabLabels = {overview:'profile.tab.overview',career:'profile.tab.career',records:'profile.tab.records',content:'profile.tab.content',friends:'profile.tab.friends'} as const;
type ProfileTab = keyof typeof tabLabels;
const socialLabels = {friends:'profile.tab.friends',following:'profile.following'} as const;
const defaultDisplay: ProfileDisplay = {visible: {friends: true, career: true, records: true, content: true}, order: ['career', 'records', 'content', 'friends', 'favorites']};
const displayStorageKey = 'player-profile-display-v2';
const validDisplay = (value: unknown): value is ProfileDisplay => {
  if (!value || typeof value !== 'object') return false;
  const settings = value as ProfileDisplay;
  return !!settings.visible && Object.keys(displayLabels).every(key => typeof settings.visible[key as DisplayKey] === 'boolean') &&
    Array.isArray(settings.order) && settings.order.length === 5 && new Set(settings.order).size === 5 && settings.order.every(key => Object.hasOwn(overviewLabels, key));
};
function previousDisplay(): ProfileDisplay {
  const old = readBookPreference('player-profile-display-v1', defaultDisplay, (value): value is ProfileDisplay => {
    if (!value || typeof value !== 'object') return false;
    const candidate = value as ProfileDisplay;
    return !!candidate.visible && Object.keys(displayLabels).every(key => typeof candidate.visible[key as DisplayKey] === 'boolean') && Array.isArray(candidate.order);
  });
  const order = [...new Set([...old.order.map(key => String(key) === 'recent' ? 'records' as const : key).filter(key => Object.hasOwn(overviewLabels,key)), ...defaultDisplay.order])];
  const visible = Object.fromEntries((Object.keys(displayLabels) as DisplayKey[]).map(key => [key,old.visible[key]])) as Record<DisplayKey,boolean>;
  return {visible, order};
}
const blankProfile: Profile = {name: '', bio: '', avatar: '', cover: '', links: []};
const imageData = (value: string) => /^data:image\/(png|jpeg|webp);base64,/i.test(value);
const safeUrl = (value: string) => {try {const url = new URL(value); return ['http:', 'https:'].includes(url.protocol) ? url : null;} catch {return null;}};
const validProfile = (value: unknown): value is Profile => {
  if (!value || typeof value !== 'object') return false;
  const profile = value as Profile;
  return ['name', 'bio', 'avatar', 'cover'].every(key => typeof (profile as unknown as Record<string, unknown>)[key] === 'string') &&
    Array.isArray(profile.links) && profile.links.every(link => link && typeof link.label === 'string' && typeof link.url === 'string' && !!safeUrl(link.url));
};
const validGuides = (value: unknown): value is Guide[] => Array.isArray(value) && value.every(item => item && typeof item.id === 'string' && typeof item.title === 'string' && typeof item.body === 'string');

/** Browser-local presentation only. No account or room identity is created. */
export function ProfilePreview({interactive = true, active, href}: {interactive?: boolean; active: boolean; href: string}) {
  const {t} = useLocale();
  const [avatar, setAvatar] = useState('');
  useEffect(() => {
    const refresh = () => setAvatar(readBookPreference('player-profile-v2', blankProfile, validProfile).avatar);
    refresh(); window.addEventListener(BOOK_PREFERENCES_EVENT, refresh); window.addEventListener('storage', refresh);
    return () => {window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh); window.removeEventListener('storage', refresh);};
  }, []);
  const content = <><span aria-hidden="true">{imageData(avatar) ? <img src={avatar} alt=""/> : <NavigationIcon name="profile"/>}</span><svg className="book-profile-trace" viewBox="0 0 40 40" aria-hidden="true"><rect x="1.5" y="1.5" width="37" height="37" rx="7" pathLength="100"/></svg></>;
  return <NavigationButton asChild variant="ghost" size="icon" className="masthead-action book-profile-trigger [&_svg]:size-5">{interactive ? <AppLink href={href} aria-label={t('profile.navigation')} aria-current={active ? 'page' : undefined}>{content}</AppLink>
    : <a href={href} aria-label={t('profile.navigation')} aria-current={active ? 'page' : undefined}>{content}</a>}</NavigationButton>;
}

export function ProfilePage() {
  const {locale,t}=useLocale();
  const location=useLocation(),navigate=useNavigate();
  const anchor=location.hash.replace(/^#profile-/,''),tab:ProfileTab=Object.hasOwn(tabLabels,anchor)?anchor as ProfileTab:'overview';
  const sectionAddress=(key:ProfileTab)=>({pathname:location.pathname,search:location.search,hash:`#profile-${key}`});
  const jumpTo=(key:ProfileTab,replace=false)=>navigate(sectionAddress(key),{replace});
  const [profile, setProfile] = useState(blankProfile), [draft, setDraft] = useState<ProfileDraft>({...blankProfile, links: []});
  const [editing, setEditing] = useState(false);
  const [display, setDisplay] = useState(defaultDisplay), [displayDraft, setDisplayDraft] = useState(defaultDisplay);
  const [displayEditing, setDisplayEditing] = useState(false);
  const [settings, setSettings] = useState(false), [security, setSecurity] = useState(false);
  const [visitor, setVisitor] = useState(false);
  const [favorites, setFavorites] = useState<string[]>([]);
  const [guides, setGuides] = useState<Guide[]>([]), [guideDraft, setGuideDraft] = useState<Guide | null>(null);
  const [career, setCareer] = useState<CareerRecords>({});
  const [article, setArticle] = useState<Guide | null>(null), [socialTab, setSocialTab] = useState<keyof typeof socialLabels>("friends");
  const [demoOnline, setDemoOnline] = useState(false), [onlineNotice, setOnlineNotice] = useState(false);
  const [notifications, setNotifications] = useState(false), [notice, setNotice] = useState<UiMessageKey|null>(null), [error, setError] = useState<UiMessageKey|null>(null);
  useEffect(() => {if (settings) window.scrollTo(0, 0);}, [settings]);
  useEffect(() => () => {profileAnimation?.cancel(); profileAnimation = null;}, []);
  useEffect(() => {
    setProfile(readBookPreference('player-profile-v2', blankProfile, validProfile));
    setDisplay(readBookPreference(displayStorageKey, previousDisplay(), validDisplay));
    setGuides(readBookPreference('player-guide-drafts-v1', [], validGuides));
    setCareer(loadCareerRecords());
    setNotifications(readBookPreference('friend-online-reminder', false, (value): value is boolean => typeof value === 'boolean'));
    const refresh = () => { setFavorites(readBookPreference('favorites', [], stringArray));};
    refresh(); window.addEventListener(BOOK_PREFERENCES_EVENT, refresh); window.addEventListener('storage', refresh);
    return () => { window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh); window.removeEventListener('storage', refresh);};
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
  const saveProfile = () => {
    if (draft.links.some(link => !link.label.trim() || !safeUrl(link.url))) {setError("profile.links.invalid"); return;}
    const next = {...draft, name: draft.name.trim(), links: draft.links.map(link => ({label: link.label.trim(), url: safeUrl(link.url)!.href}))};
    setProfile(next); writeBookPreference('player-profile-v2', next); setError(null); setNotice("profile.saved");
  };
  const saveGuide = () => {
    if (!guideDraft?.title.trim() || !guideDraft.body.trim()) {setError("profile.draft.required"); return;}
    const next = guides.some(guide => guide.id === guideDraft.id) ? guides.map(guide => guide.id === guideDraft.id ? guideDraft : guide) : [...guides, guideDraft];
    setGuides(next); writeBookPreference('player-guide-drafts-v1', next); setGuideDraft(null); setArticle(null); setError(null); setNotice("profile.draft.saved");
  };
  const startGuide = () => {setGuideDraft({id: crypto.randomUUID(), title: '', body: ''}); setError(null);};
  const saveCareer = (next: CareerRecords) => {setCareer(next); writeBookPreference(CAREER_STORAGE_KEY, next); setNotice("profile.career.saved");};
  const editDisplay = () => transitionProfile(() => {setSettings(true); setSecurity(false); setDisplayDraft({visible: {...display.visible}, order: [...display.order]}); setDisplayEditing(true); setEditing(false); setArticle(null); setGuideDraft(null); setError(null);}, settings ? 'settings' : 'page');
  const saveDisplay = () => {
    setDisplay(displayDraft); writeBookPreference(displayStorageKey, displayDraft); setNotice("profile.display.saved");
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
  const visibleTabs: ProfileTab[] = ['overview',...display.order.filter((key): key is Exclude<ProfileTab,'overview'> => key!=='favorites' && visible[key])];
  const activeTab = settings ? editing ? 'settings-profile' : displayEditing ? 'settings-display' : 'settings-security' : tab;
  const selectTab = (value: string) => {
    if (value === activeTab) return;
    if (value === 'settings-profile') editProfile();
    else if (value === 'settings-display') editDisplay();
    else if (value === 'settings-security') transitionProfile(() => {setEditing(false); setDisplayEditing(false); setSecurity(true); setError(null);}, 'settings');
  };
  const links = [...new Map(profile.links.map(link => [link.url,link])).values()];
  const sections: Record<OverviewKey,ReactNode> = {
    career: visible.career && <div id="profile-career" className="profile-public-section"><CareerShowcase records={career} onSave={saveCareer} readOnly={visitor}/></div>,
    records: visible.records && <section id="profile-records" className="profile-panel profile-public-section"><div className="profile-section-title"><h2>{t('profile.tab.records')}</h2></div><EmptyState title={t('profile.stats.empty')} detail={t('profile.stats.hint')}/></section>,
    content: visible.content && <div id="profile-content" className="profile-public-section">{guideDraft ? <section className="profile-panel profile-editor"><h2>{guides.some(guide => guide.id === guideDraft.id) ? t('profile.draft.edit') : t('profile.draft.newHeading')}</h2><form onSubmit={event => {event.preventDefault(); saveGuide();}}><label>{t('profile.draft.title')}<Input value={guideDraft.title} maxLength={100} required onChange={event => setGuideDraft({...guideDraft, title: event.target.value})}/></label><label>{t('profile.draft.body')}<Textarea rows={14} value={guideDraft.body} maxLength={30000} required onChange={event => setGuideDraft({...guideDraft, body: event.target.value})}/></label>{error && <p role="alert">{t(error)}</p>}<div className="profile-actions"><BookButton variant="action" type="button" onClick={() => setGuideDraft(null)}>{t('action.cancel')}</BookButton><BookButton variant="primary" type="submit" className="profile-primary">{t('profile.draft.save')}</BookButton></div></form></section> : article ? <article className="profile-panel profile-article"><div className="profile-section-title"><BookButton variant="quiet" type="button" onClick={() => setArticle(null)}>{t('profile.draft.back')}</BookButton>{!visitor && <BookButton variant="quiet" type="button" onClick={() => setGuideDraft({...article})}>{t('profile.editAction')}</BookButton>}</div><h2>{article.title}</h2><div className="ui-document prose"><p className="profile-guide-body">{article.body}</p></div></article> : <section className="profile-panel"><div className="profile-section-title"><h2>{t('profile.draft.heading')}</h2>{!visitor && <BookButton variant="quiet" type="button" onClick={startGuide}>{t('profile.draft.new')}</BookButton>}</div>{guides.length ? <div className="profile-draft-list">{guides.map(guide => <BookButton variant="action" key={guide.id} type="button" onClick={() => setArticle(guide)}><span><strong>{guide.title}</strong><small>{guide.body.slice(0, 100)}</small></span><span>{t('profile.draft.local')}</span></BookButton>)}</div> : <EmptyState title={t('profile.draft.empty')} detail={t('profile.draft.emptyHint')}/>}</section>}</div>,
    friends: visible.friends && <section id="profile-friends" className="profile-panel profile-public-section"><div className="profile-section-title"><h2>{t('profile.social.heading')}</h2></div><div className="profile-social-tools"><div>{(["friends", "following"] as const).map(item => <BookButton variant="choice" type="button" key={item} aria-pressed={socialTab === item} onClick={() => {if (socialTab !== item) transitionProfile(() => setSocialTab(item), 'content');}}>{t(socialLabels[item])}</BookButton>)}</div>{!visitor && <label><Toggle checked={notifications} onCheckedChange={event => {setNotifications(event); if (!event) setOnlineNotice(false); writeBookPreference('friend-online-reminder', event);}}/>{t('profile.online.notifications')}</label>}</div>{demoOnline && socialTab === "friends" ? <div className="profile-demo-friend"><span className="profile-demo-avatar"><AvatarPlaceholder/><i/></span><div><strong>{t('profile.online.demoFriend')}</strong><p>{t('profile.online')}<small>{t('profile.online.simulated')}</small></p></div><BookButton variant="action" type="button" onClick={() => {setDemoOnline(false); setOnlineNotice(false);}}>{t('profile.online.stopDemo')}</BookButton></div> : <EmptyState title={socialTab === "friends" ? t('profile.friends.empty') : t('profile.following.empty')} detail={t('profile.social.emptyHint')}/>}{!visitor && <div className="profile-reminder-demo"><span>{t('profile.online.preview')}</span><BookButton variant="action" type="button" onClick={() => {setDemoOnline(true); setOnlineNotice(notifications); if (!notifications) setNotice("profile.online.disabledPreview");}}>{t('profile.online.simulate')}</BookButton></div>}</section>,
    favorites: <section className="profile-panel profile-public-section"><div className="profile-section-title"><h2>{t('profile.favorites')}</h2><span>{t('profile.gameCount',{count:favorites.filter(isProductId).length})}</span></div><LocalGames ids={favorites} empty={t('profile.favorites.empty')}/></section>,
  };
  return <Tabs.Root value={activeTab} onValueChange={selectTab} asChild><main className={`player-profile${settings ? ' profile-editing profile-settings' : visitor ? ' profile-visitor' : ''}`} aria-label={settings ? t('profile.settings') : undefined} aria-labelledby={settings ? undefined : 'profile-name'}>
    {(settings || article || guideDraft) && <div className="profile-topbar"><BookButton variant="quiet" type="button" onClick={back}>{t('profile.back')}</BookButton></div>}
    <div className="profile-wrap">
      {settings && <div className="profile-settings-heading"><h1>{t('profile.settings')}</h1><Tabs.List asChild><nav aria-label={t('profile.settingsNavigation')}>{[['settings-profile',t('profile.edit')],['settings-display',t('profile.displayTab')],['settings-security',t('profile.security')]].map(([value,label]) => <Tabs.Trigger asChild key={value} value={value}><BookButton variant="tab" aria-current={activeTab===value ? 'page' : undefined}>{label}</BookButton></Tabs.Trigger>)}</nav></Tabs.List></div>}
      <section id="profile-overview" className="profile-hero profile-public-section" style={imageData(profile.cover) ? {'--profile-cover': `url("${profile.cover}")`} as CSSProperties : undefined}>
        <div className={`profile-cover${imageData(profile.cover) ? ' has-image' : ''}`}/>
        <div className="profile-identity"><UiButton variant="unstyled" type="button" className="profile-avatar" aria-label={t('profile.avatar.change')} disabled={visitor} onClick={editProfile}>{imageData(profile.avatar) ? <img src={profile.avatar} alt={t('profile.avatar.alt')}/> : <AvatarPlaceholder/>}</UiButton><div className="profile-name"><h1 id="profile-name"><OverflowText>{profile.name || t('profile.name.missing')}</OverflowText></h1><p className="profile-bio">{profile.bio || t('profile.bio.missing')}</p></div><div className="profile-actions"><BookButton variant="action" type="button" className="profile-perspective" aria-label={t('profile.perspective.toggle')} aria-pressed={visitor} onClick={() => transitionProfile(() => {setVisitor(value => !value); jumpTo('overview',true); setArticle(null); setGuideDraft(null);}, 'content')}>{t('profile.perspective.prefix')}{visitor ? t('profile.visitor') : t('profile.self')}<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M4 7h15m-4-4 4 4-4 4M20 17H5m4-4-4 4 4 4"/></svg></BookButton>{!visitor && <BookButton variant="action" type="button" onClick={editProfile}>{t('profile.settings')}</BookButton>}</div></div>
      </section>
      {!settings && <nav style={{'--profile-tab-count': visibleTabs.length} as CSSProperties} className="profile-tabs" aria-label={t('profile.navigationSections')}>{visibleTabs.map(item => <Link key={item} to={sectionAddress(item)} aria-current={tab===item ? 'location' : undefined}>{t(tabLabels[item])}</Link>)}</nav>}
      <ProfileContent settings={settings} value={activeTab}>{settings && security ? <AccountSecurity/> : displayEditing ? <section className="profile-panel profile-editor-shell profile-display-editor"><div className="profile-section-title"><h2><ProfileSectionIcon name="layout"/>{t('profile.display')}</h2></div><form onSubmit={event => {event.preventDefault(); saveDisplay();}}>
        <div className="profile-display-columns"><fieldset><legend>{t('profile.display.visible')}</legend><div className="profile-display-options">{(Object.keys(displayLabels) as DisplayKey[]).map(key => <label key={key}><span className="profile-option-name"><ProfileSectionIcon name={key}/><span>{t(displayLabels[key])}</span></span><Toggle checked={displayDraft.visible[key]} onCheckedChange={event => setDisplayDraft({...displayDraft, visible: {...displayDraft.visible, [key]: event}})}/></label>)}</div></fieldset>
        <fieldset><legend>{t('profile.display.order')}</legend><ol className="profile-display-order">{displayDraft.order.map((key, index) => <li key={key} data-overview-key={key} className={key !== 'favorites' && !displayDraft.visible[key] ? 'is-hidden' : undefined}><div className={`profile-order-art profile-order-art-${key}`} aria-hidden="true"><ProfileSectionIcon name={key}/><i/><i/><i/></div><span className="profile-order-label"><small>{String(index + 1).padStart(2, '0')}</small>{t(overviewLabels[key])}</span><div><BookButton variant="action" type="button" aria-label={t('profile.moveUp',{name:t(overviewLabels[key])})} disabled={index === 0} onClick={() => moveOverview(key, -1)}>↑</BookButton><BookButton variant="action" type="button" aria-label={t('profile.moveDown',{name:t(overviewLabels[key])})} disabled={index === displayDraft.order.length - 1} onClick={() => moveOverview(key, 1)}>↓</BookButton></div></li>)}</ol></fieldset></div>
        <div className="profile-actions profile-editor-footer"><BookButton variant="action" type="button" onClick={back}>{t('action.cancel')}</BookButton><BookButton variant="primary" type="submit" className="profile-primary">{t('profile.display.save')}</BookButton></div>
      </form></section> : editing ? <section className="profile-panel profile-editor profile-editor-shell profile-details-editor"><div className="profile-section-title"><h2><ProfileSectionIcon name="profile"/>{t('profile.editHeading')}</h2></div><form onSubmit={event => {event.preventDefault(); saveProfile();}}>
        <div className="profile-details-columns"><div className="profile-media-editor"><div className="profile-draft-preview"><div className={`profile-draft-cover${imageData(draft.cover) ? ' has-image' : ''}`} style={imageData(draft.cover) ? {backgroundImage: `url("${draft.cover}")`} : undefined}/><div className="profile-draft-identity"><span className="profile-draft-avatar">{imageData(draft.avatar) ? <img src={draft.avatar} alt={t('profile.avatar.preview')}/> : <AvatarPlaceholder/>}</span><strong>{draft.name || t('profile.name.missing')}</strong><p>{draft.bio.split('\n')[0] || t('profile.bio.missing')}</p></div></div>
        <div className="profile-image-fields">{(['avatar', 'cover'] as const).map(field => <fieldset key={field}><legend>{field === 'avatar' ? t('profile.avatar') : t('profile.cover')}</legend><label className="profile-image-upload"><ProfileSectionIcon name="upload"/><span>{t('profile.image.choose')}</span><input type="file" aria-label={field === 'avatar' ? t('profile.avatar.choose') : t('profile.cover.choose')} accept="image/png,image/jpeg,image/webp" onChange={event => uploadImage(event.target.files?.[0], field)}/></label>{draft[field] && <BookButton variant="quiet" type="button" className="profile-image-remove" onClick={() => setDraft({...draft, [field]: ''})}>{t('profile.remove')}</BookButton>}</fieldset>)}</div><small className="profile-upload-hint">{t('profile.image.formats')}</small></div><div className="profile-detail-fields">
        <label>{t('profile.name')}<Input maxLength={32} value={draft.name} onChange={event => setDraft({...draft, name: event.target.value})} placeholder={t('profile.name.placeholder')}/></label>
        <label>{t('profile.bio')}<Textarea rows={4} maxLength={500} value={draft.bio} onChange={event => setDraft({...draft, bio: event.target.value})} placeholder={t('profile.bio.placeholder')}/><small>{draft.bio.length} / 500</small></label>
        <fieldset><legend>{t('profile.links')}</legend>{draft.links.map((link, index) => <div className="profile-link-editor" key={link.editId}><label>{t('profile.links.name')}<Input maxLength={40} value={link.label} onChange={event => setDraft({...draft, links: draft.links.map((item, i) => i === index ? {...item, label: event.target.value} : item)})}/></label><label>{t('profile.links.address')}<Input type="url" placeholder="https://" value={link.url} onChange={event => setDraft({...draft, links: draft.links.map((item, i) => i === index ? {...item, url: event.target.value} : item)})}/></label><BookButton variant="action" type="button" aria-label={t('profile.links.delete',{number:index + 1})} onClick={() => setDraft({...draft, links: draft.links.filter((_, i) => i !== index)})}>{t('action.delete')}</BookButton></div>)}<BookButton variant="action" type="button" onClick={() => setDraft({...draft, links: [...draft.links, editableLink({label: '', url: ''})]})}>{t('profile.links.add')}</BookButton></fieldset></div></div>
        {error && <p role="alert">{t(error)}</p>}<div className="profile-actions profile-editor-footer"><BookButton variant="action" type="button" onClick={back}>{t('action.cancel')}</BookButton><BookButton variant="primary" className="profile-primary" type="submit">{t('profile.save')}</BookButton></div>
      </form></section> : <div className="profile-columns"><aside className="profile-aside">{links.length > 0 && <section className="profile-panel"><div className="profile-social-links">{links.map(link => <a key={link.url} href={link.url} target="_blank" rel="noopener noreferrer">{link.label} ↗</a>)}</div></section>}<section className="profile-panel"><div className="profile-section-title"><h2>{t('profile.accountInfo')}</h2></div><dl className="profile-account-meta"><div><dt>{t('profile.registeredAt')}</dt><dd>—</dd></div></dl><p className="profile-muted">{t('profile.accountMissing')}</p><AppLink className="profile-account-login" href={withUiLanguage('login.html',locale)}>{t('profile.signIn')}</AppLink></section></aside>
        <div className="profile-content">{display.order.map(key => <Fragment key={key}>{sections[key]}</Fragment>)}
        </div></div>}
      </ProfileContent>
    </div>{notice && <div className="profile-toast" role="status">{t(notice)}</div>}
  {!visitor && onlineNotice && visible.friends && <aside className="profile-online-notice" role="status" aria-label={t('profile.online.notifications')}><span className="profile-demo-avatar"><AvatarPlaceholder/><i/></span><div className="profile-online-copy"><div><span>{t('profile.online.title')}</span><small>{t('profile.online.justNow')}</small></div><strong>{t('profile.online.demoFriend')}</strong><p>{t('profile.online.now')}</p><BookButton variant="quiet" type="button" onClick={() => {
    flushSync(() => {setSocialTab('friends');setSettings(false);setSecurity(false);setDisplayEditing(false);setEditing(false);setArticle(null);setGuideDraft(null);setOnlineNotice(false);});
    jumpTo('friends');
  }}>{t('profile.online.view')}</BookButton></div><BookButton variant="icon" type="button" className="profile-notice-close" aria-label={t('profile.online.close')} onClick={() => setOnlineNotice(false)}>×</BookButton></aside>}</main></Tabs.Root>;
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
  return <section className="profile-panel profile-editor-shell profile-security-editor">
    <div className="profile-section-title"><h2>{t('profile.security')}</h2><span>{t('profile.security.preview')}</span></div>
    <div className="profile-security-body">
      <section className="profile-security-section"><h3>{t('profile.security.link')}</h3><div className="profile-binding-row"><div><strong>Discord</strong><p className="profile-muted">{t('profile.security.notLinked')}</p></div><BookButton variant="action" type="button" onClick={() => setFeedback("profile.security.discordUnavailable")}>{t('profile.security.discord')}</BookButton></div></section>
      <section className="profile-security-section"><h3>{t('profile.security.changeEmail')}</h3><form method="post" onSubmit={event => {event.preventDefault(); setFeedback(sentTo === email.trim() && code === '123456' ? "profile.security.emailDone" : "profile.security.emailCodeRequired");}}>
        <label>{t('profile.security.newEmail')}<Input type="email" name="new-email" autoComplete="email" required value={email} onChange={event => {setEmail(event.target.value); setCode(''); setSentTo('');}} placeholder="name@example.com"/></label>
        <label>{t('profile.security.emailCode')}<div className="profile-security-code"><Input name="email-code" autoComplete="one-time-code" inputMode="numeric" pattern="[0-9]{6}" maxLength={6} required value={code} onChange={event => setCode(event.target.value.replace(/\D/g, ''))} placeholder={t('profile.security.codePlaceholder')}/><BookButton variant="action" type="button" onClick={() => {if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email.trim())) {setFeedback("profile.security.emailInvalid"); return;} setSentTo(email.trim()); setFeedback("profile.security.demoCode");}}>{t('profile.security.getCode')}</BookButton></div></label>
        <BookButton variant="primary" className="profile-primary" type="submit">{t('profile.security.changeEmail')}</BookButton>
      </form></section>
      <section className="profile-security-section"><h3>{t('profile.security.changePassword')}</h3><form method="post" onSubmit={event => {event.preventDefault(); if (password !== confirmation) {setFeedback("profile.security.passwordMismatch"); return;} setCurrent(''); setPassword(''); setConfirmation(''); setFeedback("profile.security.passwordDone");}}>
        <label>{t('profile.security.currentPassword')}<Input name="current-password" type="password" autoComplete="current-password" required value={current} onChange={event => setCurrent(event.target.value)}/></label>
        <label>{t('profile.security.newPassword')}<Input name="new-password" type="password" autoComplete="new-password" minLength={8} required value={password} onChange={event => setPassword(event.target.value)} placeholder={t('profile.security.passwordPlaceholder')}/></label>
        <label>{t('profile.security.confirmPassword')}<Input name="confirm-new-password" type="password" autoComplete="new-password" minLength={8} required value={confirmation} onChange={event => setConfirmation(event.target.value)} aria-invalid={confirmation.length > 0 && password !== confirmation ? true : undefined}/></label>
        <BookButton variant="primary" className="profile-primary" type="submit">{t('profile.security.changePassword')}</BookButton>
      </form></section>
      {feedback && <p className="profile-security-feedback" role="status">{t(feedback)}</p>}
    </div>
  </section>;
}

function EmptyState({title, detail}: {title: string; detail?: string}) {return <div className="profile-empty"><h3>{title}</h3>{detail && <p>{detail}</p>}</div>;}
function LocalGames({ids, empty}: {ids: string[]; empty: string}) {
  const products = ids.filter(isProductId);
  if (!products.length) return <EmptyState title={empty}/>;
  return <div className="profile-local-games">{products.map(id => {const gameId = gameIdForProduct(id), game = PRODUCT_GAMES[gameId]; return <div className="profile-local-game" key={id}>{bookGameArtwork(gameId) ? <img src={bookGameArtwork(gameId)} alt=""/> : <span className="profile-game-placeholder">th{game.number}</span>}<div><small>th{game.number}</small><strong title={game.title}>{game.title}</strong><p>{game.subtitle}</p></div></div>;})}</div>;
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
