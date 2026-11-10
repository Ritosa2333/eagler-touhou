import {createContext, useContext, useEffect, useState, type Dispatch, type ReactNode, type SetStateAction} from 'react';
import {Link} from 'react-router';
import {UserRound} from 'lucide-react';
import {accountRequest} from '../../services/account-client';
import {BookButton} from './BookButton';
import {AppLink} from './AppLink';
import {useLocale} from '../../i18n';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';

type SocialUser = {username: string; nickname: string};
type SocialSnapshot = {visible: boolean; following: boolean | null; friends: SocialUser[]; followingUsers: SocialUser[]};
type LoadState = {loading: boolean; data?: SocialSnapshot; error?: string};

function parseUser(value: unknown): SocialUser | null {
  if (!value || typeof value !== 'object') return null;
  const item = value as Record<string, unknown>;
  return typeof item.username === 'string' && typeof item.nickname === 'string'
    ? {username: item.username, nickname: item.nickname} : null;
}

async function readSocial(username: string, signal: AbortSignal): Promise<SocialSnapshot> {
  const value = await accountRequest('v1/profiles/' + encodeURIComponent(username) + '/social', undefined, {signal}) as Record<string, unknown> | null;
  if (!value || typeof value.visible !== 'boolean' || !(value.following === null || typeof value.following === 'boolean') ||
      !Array.isArray(value.friends) || !Array.isArray(value.followingUsers)) throw new Error('Invalid social response');
  return {visible: value.visible, following: value.following,
    friends: value.friends.map(parseUser).filter((user): user is SocialUser => !!user),
    followingUsers: value.followingUsers.map(parseUser).filter((user): user is SocialUser => !!user)};
}

type SocialContextValue = {username: string; state: LoadState; setState: Dispatch<SetStateAction<LoadState>>};
const SocialContext = createContext<SocialContextValue | null>(null);
export function SocialProvider({username,children}: {username: string;children: ReactNode}) {
  const [state,setState] = useState<LoadState>({loading:true});
  useEffect(() => {
    let active=true,revision=0;
    let controller:AbortController|undefined;
    const load=()=>{
      controller?.abort();
      const current=++revision,task=new AbortController();controller=task;
      setState(value=>({...value,loading:true,error:undefined}));
      void readSocial(username,task.signal).then(data=>{
        if(active&&!task.signal.aborted&&revision===current)setState({loading:false,data});
      }).catch(cause=>{
        if(active&&!task.signal.aborted&&revision===current)setState({loading:false,error:String(cause instanceof Error?cause.message:cause)});
      });
    };
    load();window.addEventListener('account-social-change',load);
    return()=>{active=false;revision++;controller?.abort();window.removeEventListener('account-social-change',load);};
  },[username]);
  return <SocialContext.Provider value={{username,state,setState}}>{children}</SocialContext.Provider>;
}

function useSocial(username: string) {
  const value=useContext(SocialContext);
  if(!value||value.username!==username)throw new Error('SocialProvider is required for profile social components');
  return [value.state,value.setState] as const;
}

export function SocialSection({username}: {username: string}) {
  const {t} = useLocale();
  const [state] = useSocial(username);
  const [tab, setTab] = useState<'friends' | 'following'>('friends');
  const rows = state.data?.[tab === 'friends' ? 'friends' : 'followingUsers'] ?? [];
  const title = tab === 'friends' ? t('profile.friends.empty') : t('profile.following.empty');
  return <section id="profile-friends" className="profile-panel profile-public-section" aria-busy={state.loading}>
    <div className="profile-section-title"><h2>{t('profile.social.heading')}</h2></div>
    <div className="profile-social-tools"><div>{(['friends', 'following'] as const).map(item => <BookButton variant="choice" type="button" key={item}
      aria-pressed={tab === item} onClick={() => setTab(item)}>{t(item === 'friends' ? 'profile.tab.friends' : 'profile.following')}</BookButton>)}</div></div>
    {state.error && <p role="alert">{t('profile.social.loadFailed')}</p>}
    {state.loading && !state.data && <div className="profile-empty" aria-hidden="true"/>}
    {!state.loading && !state.error && state.data && !state.data.visible && <div className="profile-empty"><p>{t('profile.socialPrivate')}</p></div>}
    {state.data?.visible && !rows.length && <div className="profile-empty"><h3>{title}</h3><p>{t('profile.social.emptyHint')}</p></div>}
    {state.data?.visible && rows.length > 0 && <div className="profile-social-users">{rows.map(user => <div className="profile-social-user" key={user.username}>
      <span className="profile-social-avatar"><UserRound aria-hidden="true" strokeWidth={1.5}/></span>
      <div><Link to={'/u/' + encodeURIComponent(user.username)}><strong>{user.nickname}</strong></Link><small>@{user.username}</small></div>
    </div>)}</div>}
  </section>;
}

export function SocialFollowControl({username}: {username: string}) {
  const {locale, t} = useLocale();
  const [state, setSocial] = useSocial(username);
  const [pending, setPending] = useState(false);
  const [error, setError] = useState('');
  const following = state.data?.following;
  if (state.loading && !state.data) return <span aria-hidden="true"/>;
  if (following === null) return <AppLink className="profile-account-login" href={withUiLanguage('/login.html?next=' + encodeURIComponent('/u/' + username), locale)}>{t('profile.followSignIn')}</AppLink>;
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
  return <><BookButton variant="action" type="button" className="min-w-24" disabled={pending || following === undefined}
    aria-busy={pending} aria-pressed={following}
    aria-label={following ? t('profile.unfollowAction') : t('profile.followAction')} onClick={() => void toggle()}>
    {following ? t('profile.followed') : t('profile.followAction')}
  </BookButton>{error && <span role="alert">{t('profile.social.updateFailed')}</span>}</>;
}
