import {createContext, useContext, useEffect, useState, type Dispatch, type ReactNode, type SetStateAction} from 'react';
import {Link, useLocation} from 'react-router';
import {ArrowLeft, UserRound, UsersRound} from 'lucide-react';
import {accountRequest} from '../../services/account-client';
import {PublicAccountAvatar} from './AccountAvatar';
import {BookButton} from './BookButton';
import {ProfilePanelLoading} from './ProfileLoading';
import {AppLink} from './AppLink';
import {useLocale} from '../../i18n';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';
import {getProfileStatistics} from '../../services/cloud-saves';
import type {GameId} from '../../../src/contracts/product-catalog.mts';
import {ChoiceGroup, ChoiceItem} from '../../ui/choice-group';
import {IdentityMetadata} from './AccountIdentity';

type SocialUser = {username: string; nickname: string; country?: string};
type SocialSnapshot = {visible: boolean; following: boolean | null; friends: SocialUser[]; followers: SocialUser[]; followingUsers: SocialUser[]; followerCount?: number; followingCount?: number; friendCount?: number};
type LoadState = {loading: boolean; data?: SocialSnapshot; error?: string};

function socialListHref(profileHref: string, tab: SocialTab) {
  const url = new URL(profileHref, 'https://launcher.invalid');
  url.searchParams.set('social', tab);
  return url.pathname + url.search;
}

function parseUser(value: unknown): SocialUser | null {
  if (!value || typeof value !== 'object') return null;
  const item = value as Record<string, unknown>;
  const country = item.regionCode || item.countryCode;
  return typeof item.username === 'string' && typeof item.nickname === 'string'
    ? {username: item.username, nickname: item.nickname, country: typeof country === 'string' ? country : undefined} : null;
}

async function readSocial(username: string, signal: AbortSignal, includeLists: boolean, list: SocialTab | undefined, anonymous: boolean): Promise<SocialSnapshot> {
  const query = includeLists ? list && new URLSearchParams({list}) : new URLSearchParams({summary:'1'});
  const value = await accountRequest('v1/profiles/' + encodeURIComponent(username) + '/social', undefined, {signal, query, anonymous}) as Record<string, unknown> | null;
  if (!value || typeof value.visible !== 'boolean' || !(value.following === null || typeof value.following === 'boolean') ||
      !Array.isArray(value.friends) || !Array.isArray(value.followingUsers)) throw new Error('Invalid social response');
  return {visible: value.visible, following: value.following,
    followerCount: typeof value.followerCount === 'number' ? value.followerCount : undefined,
    followingCount: typeof value.followingCount === 'number' ? value.followingCount : undefined,
    friendCount: typeof value.friendCount === 'number' ? value.friendCount : includeLists ? value.friends.length : undefined,
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
    <AppLink className="profile-social-back" href={location.pathname + socialSearch(location.search)} replace returnToEntry>
      <ArrowLeft aria-hidden="true" />{t('action.back')}
    </AppLink>
    <h1 id="profile-social-title">{t('profile.social.heading')}</h1>
    <nav className="profile-section-nav profile-social-tabs" aria-label={t('profile.social.heading')}>
      {socialTabs.map(item => <Link key={item} to={{pathname:location.pathname, search:socialSearch(location.search,item)}}
        state={location.state} replace aria-current={tab === item ? 'page' : undefined}>{t(socialLabels[item])}</Link>)}
    </nav>
    {state.error && <p role="alert">{t('profile.social.loadFailed')}{' '}<BookButton size="sm" onClick={()=>window.dispatchEvent(new Event('account-social-change'))}>{t('action.retry')}</BookButton></p>}
    {state.loading && <ProfilePanelLoading/>}
    {ready && !ready.visible && <div className="profile-empty"><p>{t('profile.socialPrivate')}</p></div>}
    {ready && ready.visible && !rows.length && <div className="profile-empty"><p>{t(socialEmpty[tab])}</p></div>}
    {ready && ready.visible && rows.length > 0 && <div className="profile-social-users">{rows.map(user => <div className="profile-social-user" key={user.username}>
      <PublicAccountAvatar className="profile-social-avatar" loading="lazy" fallback={<UserRound strokeWidth={1.5}/>} username={user.username}/>
      <div><AppLink href={'/u/' + encodeURIComponent(user.username) + socialSearch(location.search)} rememberEntry><strong>{user.nickname}</strong></AppLink><IdentityMetadata username={user.username} country={user.country}/></div>
    </div>)}</div>}
  </section>;
}

export function SocialCounts({username, href}: {username: string; href?: string}) {
  const {t, locale} = useLocale();
  const location = useLocation();
  const [state] = useSocial(username);
  const data = state.data;
  if (!data?.visible || data.followerCount == null || data.followingCount == null) return null;
  const numbers = new Intl.NumberFormat(locale);
  const counts = [
    ['following', data.followingCount, 'profile.following'],
    ['followers', data.followerCount, 'profile.followers'],
    ['friends', data.friendCount, 'profile.tab.friends'],
  ] as const;
  return <div className="launcher-social-counts">
    {counts.map(([tab, count, label]) => <AppLink key={tab} href={socialListHref(href ?? location.pathname + location.search, tab)} rememberEntry>
      <strong>{count == null ? '—' : numbers.format(count)}</strong><span>{t(label)}</span>
    </AppLink>)}
  </div>;
}

export function GameFriends({username, game}: {username: string; game: GameId}) {
  const {locale, t} = useLocale();
  const [state] = useSocial(username);
  const friends = state.data?.visible ? state.data.friends : undefined;
  const [activity, setActivity] = useState<{game: GameId; friends: SocialUser[]; players: SocialUser[]; failed: boolean}>();
  useEffect(() => {
    if (!friends?.length) return;
    const controller = new AbortController();
    const {signal} = controller;
    void (async () => {
      const players: SocialUser[] = [];
      let failed = false;
      for (let index = 0; index < friends.length && players.length < 3 && !signal.aborted; index += 3) {
        const matches = await Promise.all(friends.slice(index, index + 3).map(async friend => {
          try {
            const data = await getProfileStatistics(friend.username, signal);
            return data.games.some(({statistics}) => statistics.game === game && (
              (statistics.playtimeSeconds ?? 0) > 0 || (statistics.playCount ?? 0) > 0 ||
              statistics.progress.some(record => record.cleared || record.oneCreditClear || (record.playCount ?? 0) > 0 || (record.bestScore ?? 0) > 0)
            )) ? friend : null;
          } catch {
            failed = true;
            return null;
          }
        }));
        players.push(...matches.filter((friend): friend is SocialUser => friend !== null));
      }
      if (!signal.aborted) setActivity({game, friends, players: players.slice(0, 3), failed});
    })();
    return () => controller.abort();
  }, [game, friends]);
  const current = activity?.game === game && activity.friends === friends ? activity : undefined;
  const pending = state.loading || (!!friends?.length && !current);
  const href = socialListHref(withUiLanguage('/u/' + encodeURIComponent(username), locale), 'friends');
  return <aside className="game-detail-friends" aria-labelledby="game-detail-friends-title" aria-busy={pending}>
    <h2 id="game-detail-friends-title">{t('profile.tab.friends')}</h2>
    {!pending && (current?.players.length ? <ul className="game-detail-friends-list">{current.players.map(player => <li key={player.username}>
      <AppLink className="game-detail-friends-link" href={withUiLanguage('/u/' + encodeURIComponent(player.username), locale)} rememberEntry>{player.nickname}</AppLink>
    </li>)}</ul> : <p>{t(state.error || current?.failed ? 'profile.social.loadFailed' : 'game.friendsNoVisible')}</p>)}
    <BookButton variant="quiet" size="sm" asChild><AppLink className="game-detail-friends-link" href={href} rememberEntry>
      <UsersRound aria-hidden="true"/>{t('game.viewFriends')}
    </AppLink></BookButton>
  </aside>;
}

export function SocialFollowControl({username}: {username: string}) {
  const {locale, t} = useLocale();
  const [state, setSocial] = useSocial(username);
  const [pending, setPending] = useState(false);
  const [error, setError] = useState('');
  const following = state.data?.following;
  if (state.loading && !state.data) return <span aria-hidden="true"/>;
  if (following === null) return <BookButton variant="action" size="sm" asChild>
    <AppLink href={withUiLanguage('/login.html?next=' + encodeURIComponent('/u/' + username), locale)} rememberEntry>{t('profile.followSignIn')}</AppLink>
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
