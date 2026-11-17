import {createContext, useContext, useEffect, useState, type Dispatch, type ReactNode, type SetStateAction} from 'react';
import {Link, useLocation} from 'react-router';
import {ArrowLeft, UserRound} from 'lucide-react';
import {accountRequest} from '../../services/account-client';
import {BookButton} from './BookButton';
import {ProfilePanelLoading} from './ProfileLoading';
import {AppLink} from './AppLink';
import {useLocale} from '../../i18n';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';

type SocialUser = {username: string; nickname: string};
type SocialSnapshot = {visible: boolean; following: boolean | null; friends: SocialUser[]; followers: SocialUser[]; followingUsers: SocialUser[]; followerCount?: number; followingCount?: number};
type LoadState = {loading: boolean; data?: SocialSnapshot; error?: string};

function parseUser(value: unknown): SocialUser | null {
  if (!value || typeof value !== 'object') return null;
  const item = value as Record<string, unknown>;
  return typeof item.username === 'string' && typeof item.nickname === 'string'
    ? {username: item.username, nickname: item.nickname} : null;
}

async function readSocial(username: string, signal: AbortSignal, includeLists: boolean, list: SocialTab | undefined, anonymous: boolean): Promise<SocialSnapshot> {
  const query = includeLists ? list && new URLSearchParams({list}) : new URLSearchParams({summary:'1'});
  const value = await accountRequest('v1/profiles/' + encodeURIComponent(username) + '/social', undefined, {signal, query, anonymous}) as Record<string, unknown> | null;
  if (!value || typeof value.visible !== 'boolean' || !(value.following === null || typeof value.following === 'boolean') ||
      !Array.isArray(value.friends) || !Array.isArray(value.followingUsers)) throw new Error('Invalid social response');
  return {visible: value.visible, following: value.following,
    followerCount: typeof value.followerCount === 'number' ? value.followerCount : undefined,
    followingCount: typeof value.followingCount === 'number' ? value.followingCount : undefined,
    friends: value.friends.map(parseUser).filter((user): user is SocialUser => !!user),
    followers: Array.isArray(value.followers) ? value.followers.map(parseUser).filter((user): user is SocialUser => !!user) : [],
    followingUsers: value.followingUsers.map(parseUser).filter((user): user is SocialUser => !!user)};
}

type SocialContextValue = {username: string; state: LoadState; setState: Dispatch<SetStateAction<LoadState>>};
const SocialContext = createContext<SocialContextValue | null>(null);
export function SocialProvider({username,children,includeLists,list,anonymous=false}: {username: string;children: ReactNode;includeLists: boolean;list?:SocialTab;anonymous?:boolean}) {
  const [state,setState] = useState<LoadState>({loading:true});
  useEffect(() => {
    let active=true,revision=0;
    let controller:AbortController|undefined;
    const load=()=>{
      controller?.abort();
      const current=++revision,task=new AbortController();controller=task;
      setState(value=>({...value,loading:true,error:undefined}));
      void readSocial(username,task.signal,includeLists,list,anonymous).then(data=>{
        if(active&&!task.signal.aborted&&revision===current)setState({loading:false,data});
      }).catch(cause=>{
        if(active&&!task.signal.aborted&&revision===current)setState({loading:false,error:String(cause instanceof Error?cause.message:cause)});
      });
    };
    load();window.addEventListener('account-social-change',load);
    return()=>{active=false;revision++;controller?.abort();window.removeEventListener('account-social-change',load);};
  },[username,includeLists,list,anonymous]);
  return <SocialContext.Provider value={{username,state,setState}}>{children}</SocialContext.Provider>;
}

function useSocial(username: string) {
  const value=useContext(SocialContext);
  if(!value||value.username!==username)throw new Error('SocialProvider is required for profile social components');
  return [value.state,value.setState] as const;
}

const socialTabs = ['friends','followers','following'] as const;
type SocialTab = typeof socialTabs[number];
const socialLabels = {friends:'profile.tab.friends', followers:'profile.followers', following:'profile.following'} as const;
const socialEmpty = {friends:'profile.friends.empty', followers:'profile.followers.empty', following:'profile.following.empty'} as const;

export function profileSocialTab(search: string, hash: string): SocialTab | undefined {
  const tab = new URLSearchParams(search).get('social');
  return tab === 'friends' || tab === 'followers' || tab === 'following'
    ? tab : hash === '#profile-friends' ? 'friends' : undefined;
}

function socialSearch(search: string, tab?: SocialTab) {
  const query = new URLSearchParams(search);
  if (tab) query.set('social', tab);
  else query.delete('social');
  return query.size ? '?' + query : '';
}

export function SocialSection({username, tab}: {username: string; tab: SocialTab}) {
  const {t} = useLocale();
  const location = useLocation();
  const [state] = useSocial(username);
  const rows = state.data?.[tab === 'following' ? 'followingUsers' : tab] ?? [];
  const ready = !state.loading && !state.error && state.data;
  return <section className="profile-panel profile-social-page" aria-busy={state.loading}>
    <Link className="profile-social-back" to={{pathname:location.pathname, search:socialSearch(location.search)}}>
      <ArrowLeft aria-hidden="true" />{t('action.back')} @{username}
    </Link>
    <h1 id="profile-social-title">{t('profile.social.heading')}</h1>
    <nav className="profile-section-nav profile-social-tabs" aria-label={t('profile.social.heading')}>
      {socialTabs.map(item => <Link key={item} to={{pathname:location.pathname, search:socialSearch(location.search,item)}}
        aria-current={tab === item ? 'page' : undefined}>{t(socialLabels[item])}</Link>)}
    </nav>
    {state.error && <p role="alert">{t('profile.social.loadFailed')}{' '}<BookButton size="sm" onClick={()=>window.dispatchEvent(new Event('account-social-change'))}>{t('action.retry')}</BookButton></p>}
    {state.loading && <ProfilePanelLoading/>}
    {ready && !ready.visible && <div className="profile-empty"><p>{t('profile.socialPrivate')}</p></div>}
    {ready && ready.visible && !rows.length && <div className="profile-empty"><p>{t(socialEmpty[tab])}</p></div>}
    {ready && ready.visible && rows.length > 0 && <div className="profile-social-users">{rows.map(user => <div className="profile-social-user" key={user.username}>
      <span className="profile-social-avatar"><UserRound aria-hidden="true" strokeWidth={1.5}/></span>
      <div><Link to={{pathname:'/u/' + encodeURIComponent(user.username), search:socialSearch(location.search)}}><strong>{user.nickname}</strong></Link><small>@{user.username}</small></div>
    </div>)}</div>}
  </section>;
}

export function SocialCounts({username}: {username: string}) {
  const {t, locale} = useLocale();
  const location = useLocation();
  const [state] = useSocial(username);
  const data = state.data;
  if (!data?.visible || data.followerCount == null || data.followingCount == null) return null;
  const numbers = new Intl.NumberFormat(locale);
  return <div className="profile-social-counts">
    <Link to={{pathname:location.pathname, search:socialSearch(location.search,'followers')}}>
      <strong>{numbers.format(data.followerCount)}</strong> {t('profile.followers')}
    </Link>
    <Link to={{pathname:location.pathname, search:socialSearch(location.search,'following')}}>
      <strong>{numbers.format(data.followingCount)}</strong> {t('profile.following')}
    </Link>
  </div>;
}

export function SocialFollowControl({username}: {username: string}) {
  const {locale, t} = useLocale();
  const [state, setSocial] = useSocial(username);
  const [pending, setPending] = useState(false);
  const [error, setError] = useState('');
  const following = state.data?.following;
  if (state.loading && !state.data) return <span aria-hidden="true"/>;
  if (following === null) return <BookButton variant="action" size="sm" asChild>
    <AppLink href={withUiLanguage('/login.html?next=' + encodeURIComponent('/u/' + username), locale)}>{t('profile.followSignIn')}</AppLink>
  </BookButton>;
  async function toggle() {
    if (pending || following === undefined) return;
    setPending(true); setError('');
    try {
      const result = await accountRequest('v1/me/following', {username, follow: !following}) as {following?: unknown};
      if (typeof result?.following !== 'boolean') throw new Error('Invalid social response');
      setSocial(previous => ({...previous, data: previous.data ? {...previous.data, following: result.following as boolean} : undefined}));
      window.dispatchEvent(new Event('account-social-change'));
    } catch (cause) {setError(String(cause instanceof Error ? cause.message : cause));}
    finally {setPending(false);}
  }
  if (state.error) return <span role="alert">{t('profile.social.loadFailed')}</span>;
  return <><BookButton variant="action" size="sm" type="button" disabled={pending || following === undefined}
    aria-busy={pending} aria-pressed={following}
    aria-label={following ? t('profile.unfollowAction') : t('profile.followAction')} onClick={() => void toggle()}>
    {following ? t('profile.followed') : t('profile.followAction')}
  </BookButton>{error && <span role="alert">{t('profile.social.updateFailed')}</span>}</>;
}
