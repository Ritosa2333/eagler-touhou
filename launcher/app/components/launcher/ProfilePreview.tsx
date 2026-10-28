import {useEffect, useState, type CSSProperties} from 'react';
import {AppLink} from './AppLink';
import {useLocale} from '../../i18n';
import {PRODUCT_GAMES, gameIdForProduct, isProductId} from '../../../src/contracts/product-catalog.mts';
import {bookGameArtwork} from './book-artwork';
import {BOOK_PREFERENCES_EVENT, readBookPreference, writeBookPreference, stringArray} from './book-preferences';
import './profile-preview.css';

type SocialLink = {label: string; url: string};
type Profile = {name: string; bio: string; avatar: string; cover: string; links: SocialLink[]};
type Guide = {id: string; title: string; body: string};
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
  const {locale} = useLocale();
  const [avatar, setAvatar] = useState('');
  useEffect(() => {
    const refresh = () => setAvatar(readBookPreference('player-profile-v2', blankProfile, validProfile).avatar);
    refresh(); window.addEventListener(BOOK_PREFERENCES_EVENT, refresh); window.addEventListener('storage', refresh);
    return () => {window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh); window.removeEventListener('storage', refresh);};
  }, []);
  const content = <span aria-hidden="true">{imageData(avatar) ? <img src={avatar} alt=""/> : <AvatarPlaceholder/>}</span>;
  return interactive ? <AppLink className="book-profile-trigger" href={href} aria-label={locale === 'en' ? 'Player profile' : '玩家主页'} aria-current={active ? 'page' : undefined}>{content}</AppLink>
    : <a className="book-profile-trigger" href={href} aria-label={locale === 'en' ? 'Player profile' : '玩家主页'} aria-current={active ? 'page' : undefined}>{content}</a>;
}

export function ProfilePage() {
  const [profile, setProfile] = useState(blankProfile), [draft, setDraft] = useState(blankProfile);
  const [tab, setTab] = useState('概览'), [editing, setEditing] = useState(false);
  const [recent, setRecent] = useState<string[]>([]), [favorites, setFavorites] = useState<string[]>([]);
  const [guides, setGuides] = useState<Guide[]>([]), [guideDraft, setGuideDraft] = useState<Guide | null>(null);
  const [article, setArticle] = useState<Guide | null>(null), [socialTab, setSocialTab] = useState('好友');
  const [demoOnline, setDemoOnline] = useState(false), [onlineNotice, setOnlineNotice] = useState(false);
  const [notifications, setNotifications] = useState(false), [notice, setNotice] = useState(''), [error, setError] = useState('');
  useEffect(() => {
    setProfile(readBookPreference('player-profile-v2', blankProfile, validProfile));
    setGuides(readBookPreference('player-guide-drafts-v1', [], validGuides));
    setNotifications(readBookPreference('friend-online-reminder', false, (value): value is boolean => typeof value === 'boolean'));
    const refresh = () => {setRecent(readBookPreference('recent', [], stringArray)); setFavorites(readBookPreference('favorites', [], stringArray));};
    refresh(); window.addEventListener(BOOK_PREFERENCES_EVENT, refresh); window.addEventListener('storage', refresh);
    return () => { window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh); window.removeEventListener('storage', refresh);};
  }, []);
  useEffect(() => {if (!notice) return; const timer = window.setTimeout(() => setNotice(''), 3500); return () => window.clearTimeout(timer);}, [notice]);
  const back = () => {if (editing) setEditing(false); else if (guideDraft) setGuideDraft(null); else if (article) setArticle(null); setError('');};
  const editProfile = () => {setDraft({...profile, links: profile.links.map(link => ({...link}))}); setEditing(true); setError('');};
  const uploadImage = (file: File | undefined, field: 'avatar' | 'cover') => {
    if (!file) return;
    if (!['image/png', 'image/jpeg', 'image/webp'].includes(file.type) || file.size > 750_000) {setError('请选择小于 750 KB 的 PNG、JPEG 或 WebP 图片'); return;}
    const reader = new FileReader();
    reader.onload = () => {if (typeof reader.result === 'string') {const value = reader.result; setDraft(current => ({...current, [field]: value})); setError('');}};
    reader.onerror = () => setError('图片读取失败，请重试'); reader.readAsDataURL(file);
  };
  const saveProfile = () => {
    if (draft.links.some(link => !link.label.trim() || !safeUrl(link.url))) {setError('请填写链接名称和有效的 HTTP / HTTPS 地址，或删除空链接'); return;}
    const next = {...draft, name: draft.name.trim(), links: draft.links.map(link => ({label: link.label.trim(), url: safeUrl(link.url)!.href}))};
    setProfile(next); writeBookPreference('player-profile-v2', next); setEditing(false); setError(''); setNotice('资料已保存到此浏览器');
  };
  const saveGuide = () => {
    if (!guideDraft?.title.trim() || !guideDraft.body.trim()) {setError('请填写标题和正文'); return;}
    const next = guides.some(guide => guide.id === guideDraft.id) ? guides.map(guide => guide.id === guideDraft.id ? guideDraft : guide) : [...guides, guideDraft];
    setGuides(next); writeBookPreference('player-guide-drafts-v1', next); setGuideDraft(null); setArticle(null); setError(''); setNotice('草稿已保存到此浏览器');
  };
  const startGuide = () => {setGuideDraft({id: crypto.randomUUID(), title: '', body: ''}); setError('');};
  return <main className="player-profile" aria-labelledby="profile-name">
    {(editing || article || guideDraft) && <div className="profile-topbar"><button type="button" onClick={back}>← 返回主页</button></div>}
    <div className="profile-wrap">
      <section className="profile-hero" style={imageData(profile.cover) ? {'--profile-cover': `url("${profile.cover}")`} as CSSProperties : undefined}>
        <div className={`profile-cover${imageData(profile.cover) ? ' has-image' : ''}`}><button type="button" onClick={editProfile}>{profile.cover ? '更换封面' : '设置封面'}</button></div>
        <div className="profile-identity"><button type="button" className="profile-avatar" aria-label="修改头像" onClick={editProfile}>{imageData(profile.avatar) ? <img src={profile.avatar} alt="个人头像"/> : <svg viewBox="0 0 48 48" aria-hidden="true"><circle cx="24" cy="17" r="8"/><path d="M9 42v-5a15 15 0 0 1 30 0v5"/></svg>}</button><div className="profile-name"><h1 id="profile-name">{profile.name || '未设置名字'}</h1><p>{profile.bio ? profile.bio.split('\n')[0] : '尚未填写简介'}</p></div><div className="profile-actions"><button type="button" onClick={editProfile}>编辑资料</button></div></div>
      </section>
      <nav className="profile-tabs" aria-label="个人主页栏目">{['概览', '游玩记录', '内容', '好友'].map(item => <button type="button" key={item} aria-current={tab === item && !editing ? 'page' : undefined} onClick={() => {setTab(item); setEditing(false); setArticle(null); setGuideDraft(null); setError('');}}>{item}</button>)}</nav>
      {editing ? <section className="profile-panel profile-editor"><div className="profile-section-title"><h2>编辑个人资料</h2></div><form onSubmit={event => {event.preventDefault(); saveProfile();}}>
        <label>名字<input maxLength={32} value={draft.name} onChange={event => setDraft({...draft, name: event.target.value})} placeholder="填写名字"/></label>
        <label>简介<textarea rows={4} maxLength={500} value={draft.bio} onChange={event => setDraft({...draft, bio: event.target.value})} placeholder="介绍一下自己"/><small>{draft.bio.length} / 500</small></label>
        <div className="profile-image-fields">{(['avatar', 'cover'] as const).map(field => <fieldset key={field}><legend>{field === 'avatar' ? '头像' : '主页封面'}</legend><div className={`profile-image-preview ${field}`}>{imageData(draft[field]) ? <img src={draft[field]} alt={field === 'avatar' ? '头像预览' : '封面预览'}/> : <span>未设置</span>}</div><label className="profile-avatar-upload">选择图片<input type="file" accept="image/png,image/jpeg,image/webp" onChange={event => uploadImage(event.target.files?.[0], field)}/><small>PNG / JPEG / WebP，最大 750 KB</small></label>{draft[field] && <button type="button" onClick={() => setDraft({...draft, [field]: ''})}>移除{field === 'avatar' ? '头像' : '封面'}</button>}</fieldset>)}</div>
        <fieldset><legend>社交媒体链接</legend>{draft.links.map((link, index) => <div className="profile-link-editor" key={index}><label>名称<input maxLength={40} value={link.label} onChange={event => setDraft({...draft, links: draft.links.map((item, i) => i === index ? {...item, label: event.target.value} : item)})}/></label><label>地址<input type="url" placeholder="https://" value={link.url} onChange={event => setDraft({...draft, links: draft.links.map((item, i) => i === index ? {...item, url: event.target.value} : item)})}/></label><button type="button" aria-label={`删除链接 ${index + 1}`} onClick={() => setDraft({...draft, links: draft.links.filter((_, i) => i !== index)})}>删除</button></div>)}<button type="button" onClick={() => setDraft({...draft, links: [...draft.links, {label: '', url: ''}]})}>＋ 添加链接</button></fieldset>
        {error && <p role="alert">{error}</p>}<div className="profile-actions"><button type="button" onClick={() => setEditing(false)}>取消</button><button className="profile-primary" type="submit">保存资料</button></div>
      </form></section> : <div className="profile-columns"><aside className="profile-aside"><section className="profile-panel"><div className="profile-section-title"><h2>个人简介</h2><button type="button" onClick={editProfile}>编辑</button></div>{profile.bio ? <p className="profile-bio">{profile.bio}</p> : <p className="profile-muted">尚未填写简介</p>}{profile.links.length > 0 && <div className="profile-social-links">{profile.links.map((link, index) => <a key={index} href={link.url} target="_blank" rel="noopener noreferrer">{link.label} ↗</a>)}</div>}</section><section className="profile-panel"><div className="profile-section-title"><h2>账号信息</h2></div><dl className="profile-account-meta"><div><dt>注册时间</dt><dd>—</dd></div></dl><p className="profile-muted">尚未关联账号</p></section></aside>
        <div className="profile-content">{tab === '概览' && <><section className="profile-panel"><div className="profile-section-title"><h2>生涯展示柜</h2></div><EmptyState title="暂无展示内容"/></section><section className="profile-panel"><div className="profile-section-title"><h2>最近启动</h2><button type="button" onClick={() => setTab('游玩记录')}>查看全部 →</button></div><LocalGames ids={recent.slice(0, 4)} empty="暂无启动记录"/></section><section className="profile-panel"><div className="profile-section-title"><h2>收藏的游戏</h2><span>{favorites.filter(isProductId).length} 部</span></div><LocalGames ids={favorites} empty="还没有收藏的游戏"/></section></>}
        {tab === '游玩记录' && <><section className="profile-panel"><div className="profile-section-title"><h2>游玩统计</h2></div><EmptyState title="暂无游玩统计" detail="有记录后将在这里显示各作品的游玩时长与成绩。"/></section><section className="profile-panel"><div className="profile-section-title"><h2>最近启动</h2><span>此浏览器</span></div><LocalGames ids={recent} empty="暂无启动记录"/></section></>}
        {tab === '内容' && (guideDraft ? <section className="profile-panel profile-editor"><h2>{guides.some(guide => guide.id === guideDraft.id) ? '编辑草稿' : '新建内容草稿'}</h2><form onSubmit={event => {event.preventDefault(); saveGuide();}}><label>标题<input value={guideDraft.title} maxLength={100} required onChange={event => setGuideDraft({...guideDraft, title: event.target.value})}/></label><label>正文<textarea rows={14} value={guideDraft.body} maxLength={30000} required onChange={event => setGuideDraft({...guideDraft, body: event.target.value})}/></label>{error && <p role="alert">{error}</p>}<div className="profile-actions"><button type="button" onClick={() => setGuideDraft(null)}>取消</button><button type="submit" className="profile-primary">保存到本地</button></div></form></section> : article ? <article className="profile-panel profile-article"><div className="profile-section-title"><button type="button" onClick={() => setArticle(null)}>← 草稿列表</button><button type="button" onClick={() => setGuideDraft({...article})}>编辑</button></div><h2>{article.title}</h2><p className="profile-guide-body">{article.body}</p></article> : <section className="profile-panel"><div className="profile-section-title"><h2>我的内容</h2><button type="button" onClick={startGuide}>＋ 新建草稿</button></div>{guides.length ? <div className="profile-draft-list">{guides.map(guide => <button key={guide.id} type="button" onClick={() => setArticle(guide)}><span><strong>{guide.title}</strong><small>{guide.body.slice(0, 100)}</small></span><span>本地草稿 →</span></button>)}</div> : <EmptyState title="还没有内容" detail="可以先写一份草稿，保存在此浏览器。"/>}</section>)}
        {tab === '好友' && <section className="profile-panel"><div className="profile-section-title"><h2>好友与关注</h2></div><div className="profile-social-tools"><div>{['好友', '关注'].map(item => <button type="button" key={item} aria-pressed={socialTab === item} onClick={() => setSocialTab(item)}>{item}</button>)}</div><label><input type="checkbox" checked={notifications} onChange={event => {setNotifications(event.target.checked); if (!event.target.checked) setOnlineNotice(false); writeBookPreference('friend-online-reminder', event.target.checked);}}/>好友上线提醒</label></div>{demoOnline && socialTab === '好友' ? <div className="profile-demo-friend"><span className="profile-demo-avatar"><AvatarPlaceholder/><i/></span><div><strong>演示好友</strong><p>在线 <small>· 模拟状态</small></p></div><button type="button" onClick={() => {setDemoOnline(false); setOnlineNotice(false);}}>结束模拟</button></div> : <EmptyState title={socialTab === '好友' ? '暂无好友数据' : '暂无关注数据'} detail="关联账号后显示。"/>}<div className="profile-reminder-demo"><span>提醒预览</span><button type="button" onClick={() => {setDemoOnline(true); setOnlineNotice(notifications); if (!notifications) setNotice('已模拟上线；开启好友上线提醒后可预览通知');}}>模拟好友上线</button></div></section>}
        </div></div>}
    </div>{notice && <div className="profile-toast" role="status">{notice}</div>}
  {onlineNotice && <aside className="profile-online-notice" role="status" aria-label="好友上线提醒"><span className="profile-demo-avatar"><AvatarPlaceholder/><i/></span><div className="profile-online-copy"><div><span>好友上线</span><small>刚刚 · 演示</small></div><strong>演示好友</strong><p>现在在线</p><button type="button" onClick={() => {setTab('好友'); setSocialTab('好友'); setEditing(false); setOnlineNotice(false);}}>查看好友 →</button></div><button type="button" className="profile-notice-close" aria-label="关闭上线提醒" onClick={() => setOnlineNotice(false)}>×</button></aside>}</main>;
}

function EmptyState({title, detail}: {title: string; detail?: string}) {return <div className="profile-empty"><h3>{title}</h3>{detail && <p>{detail}</p>}</div>;}
function LocalGames({ids, empty}: {ids: string[]; empty: string}) {
  const products = ids.filter(isProductId);
  if (!products.length) return <EmptyState title={empty}/>;
  return <div className="profile-local-games">{products.map(id => {const gameId = gameIdForProduct(id), game = PRODUCT_GAMES[gameId]; return <div className="profile-local-game" key={id}>{bookGameArtwork(gameId) ? <img src={bookGameArtwork(gameId)} alt=""/> : <span className="profile-game-placeholder">TH{game.number}</span>}<div><small>TH{game.number}</small><strong>{game.title}</strong><p>{game.subtitle}</p></div></div>;})}</div>;
}

function AvatarPlaceholder() {return <svg viewBox="0 0 48 48" aria-hidden="true"><circle cx="24" cy="17" r="8"/><path d="M9 42v-5a15 15 0 0 1 30 0v5"/></svg>;}
