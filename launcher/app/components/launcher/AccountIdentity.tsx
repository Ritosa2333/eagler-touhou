import {useEffect, useState} from 'react';
import {ChevronRight, UserRound} from 'lucide-react';
import {useLocation} from 'react-router';
import {useLocale} from '../../i18n';
import {accountRequest} from '../../services/account-client';
import {useAccountSession} from '../../services/account-session';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';
import {AccountAvatar} from './AccountAvatar';
import {AppLink} from './AppLink';
import {CountryFlag} from './CountryFlag';
import {Button} from '../../ui/button';

export function useLauncherIdentity(interactive = true) {
  const {state, retry} = useAccountSession(interactive);
  const user = state.status === 'member' ? state.user : null;
  const username = user?.username;
  const [profile, setProfile] = useState<{username: string; nickname: string; avatar?: string; country?: string}>();
  useEffect(() => {
    if (!username) return;
    let controller: AbortController;
    function refresh() {
      controller?.abort();
      const current = new AbortController();
      controller = current;
      void accountRequest<{nickname: string; details?: {avatar?: string}; media?: {avatar?: string}; countryCode?: string; regionCode?: string}>(
        `v1/profiles/${encodeURIComponent(username!)}?media=urls`, undefined, {signal: current.signal},
      ).then(result => {
        if (!current.signal.aborted) setProfile({username: username!, nickname: result.nickname, avatar: result.media?.avatar || result.details?.avatar, country: result.regionCode || result.countryCode});
      }).catch(() => {});
    }
    refresh();
    window.addEventListener('account-profile-change', refresh);
    return () => {controller.abort(); window.removeEventListener('account-profile-change', refresh);};
  }, [username]);
  const current = profile?.username === username ? profile : undefined;
  return {state, retry, user, nickname: current?.nickname || user?.name, avatar: current?.avatar, country: current?.country};
}

export function IdentityMetadata({username, country}: {username: string; country?: string | null}) {
  const code = country?.match(/^[A-Za-z]{2}$/u)?.[0].toUpperCase();
  const label = code && typeof Intl.DisplayNames === 'function'
    ? new Intl.DisplayNames(['en'], {type: 'region', fallback: 'none'}).of(code) : code;
  return <span className="launcher-identity-meta">
    <span>@{username}</span>
    <span>{code && label ? <CountryFlag code={code}/> : <span className="launcher-unknown-country" aria-hidden="true">?</span>}{label || 'Unknown'}</span>
  </span>;
}

export function useAccountLinks() {
  const {locale} = useLocale();
  const path = useLocation();
  const login = new URL(withUiLanguage('/login.html', locale), 'https://launcher.invalid');
  if (path.pathname !== '/login.html') login.searchParams.set('next', path.pathname + path.search + path.hash);
  const loginHref = login.pathname + login.search;
  login.searchParams.set('mode', 'register');
  return {loginHref, registerHref: login.pathname + login.search};
}

export function LauncherAccountRow({identity, assetUrl, compact = false, onNavigate}: {
  identity: ReturnType<typeof useLauncherIdentity>;
  assetUrl?: (path: string) => string;
  compact?: boolean;
  onNavigate?: () => void;
}) {
  const {locale, t} = useLocale();
  const {loginHref, registerHref} = useAccountLinks();
  const {state, user, nickname, avatar, country, retry} = identity;
  if (state.status === 'loading') return <div className="launcher-account-row" aria-busy="true"><span className="launcher-account-placeholder"/></div>;
  if (state.status === 'error') return <Button variant="ghost" size="row" onClick={retry}>{t('account.requestFailed')}</Button>;
  if (!user) return <div className="launcher-account-row">
    <span className="launcher-account-placeholder"><UserRound aria-hidden="true"/></span>
    <span className="launcher-account-sign-in">
      <AppLink href={loginHref} replace={compact} rememberEntry onClick={onNavigate}>{t('account.login')}</AppLink>
      <span aria-hidden="true">/</span>
      <AppLink href={registerHref} replace={compact} rememberEntry onClick={onNavigate}>{t('account.register')}</AppLink>
    </span>
  </div>;
  return <AppLink className="launcher-account-row launcher-account-link" href={withUiLanguage('/u/' + encodeURIComponent(user.username), locale)}
    replace={compact} rememberEntry onClick={onNavigate}>
    <AccountAvatar src={avatar} assetUrl={assetUrl}/>
    <span className="launcher-account-name"><strong>{nickname}</strong><IdentityMetadata username={user.username} country={country}/></span>
    <span className="launcher-account-profile-label">{t(compact ? 'profile.pageTitle' : 'launcher.viewProfile')}</span>
    <ChevronRight aria-hidden="true"/>
  </AppLink>;
}
