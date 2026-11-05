import {useEffect,useState} from 'react';
import {DropdownMenu} from 'radix-ui';
import {LogIn,RotateCw} from 'lucide-react';
import {AccountAvatar} from './AccountAvatar';
import {useLocation} from 'react-router';
import {AppLink} from './AppLink';
import {accountRequest} from '../../services/account-client';
import {notifyAccountChange,useAccountSession} from '../../services/account-session';
import {useLocale} from '../../i18n';
import {withUiLanguage,localeHome} from '../../../src/launcher/i18n.mts';
import {uiPortalHost} from '../../ui/portal';
import {useConfirmDiscardChanges} from '../../navigation/unsaved-changes';

export function AccountMenu({interactive}:{interactive:boolean}) {
  const {locale,t} = useLocale(), {state,retry} = useAccountSession(interactive);
  const path = useLocation();
  const confirmDiscard = useConfirmDiscardChanges();
  const user = state.status === 'member' ? state.user : null;
  const [identity,setIdentity] = useState<{username:string; nickname:string; avatar:string}>(), [failed,setFailed] = useState(false), [busy,setBusy] = useState(false);
  const [open,setOpen] = useState(false);
  useEffect(() => {
    if (!user) return;
    let current:AbortController|undefined;
    const refresh = () => {
      current?.abort();
      const controller=new AbortController();current=controller;
      void accountRequest<{nickname:string;details?:{avatar?:string}}>('v1/profiles/'+encodeURIComponent(user.username),undefined,{signal:controller.signal})
        .then(profile => {if (!controller.signal.aborted) setIdentity({username:user.username,nickname:profile.nickname,avatar:profile.details?.avatar || ''});}).catch(() => {});
    };
    refresh(); window.addEventListener('account-profile-change',refresh);
    return () => {current?.abort();window.removeEventListener('account-profile-change',refresh);};
  },[user]);
  if (state.status==='loading') return <span className="masthead-action account-loading" aria-busy="true"><span className="account-avatar account-avatar-loading"/></span>;
  if (state.status==='error') return <button className="masthead-action" onClick={retry} aria-label={t('account.requestFailed')}><RotateCw aria-hidden="true"/></button>;
  if (!user) {
    const login=new URL(withUiLanguage('/login.html',locale),location.origin);
    if (path.pathname !== '/login.html') login.searchParams.set('next',path.pathname+path.search+path.hash);
    const loginHref = login.pathname+login.search;
    login.searchParams.set('mode','register');
    return <span className="masthead-account-entry account-guest">
      <AppLink className="masthead-link account-login-link" href={loginHref} aria-label={t('account.login')+' / '+t('account.register')}>
        <span className="account-login-icon"><LogIn aria-hidden="true"/></span><span className="account-guest-label">{t('account.login')}</span>
      </AppLink>
      <span className="account-guest-separator" aria-hidden="true">/</span>
      <AppLink className="masthead-link account-register-link" href={login.pathname+login.search} aria-label={t('account.register')}>{t('account.register')}</AppLink>
    </span>;
  }
  const current = identity?.username === user.username ? identity : undefined;
  const nickname = current?.nickname || user.name;
  const avatar = current?.avatar;
  const profile=withUiLanguage('/u/'+encodeURIComponent(user.username),locale);
  async function logout() {
    if (busy) return;
    setFailed(false);setBusy(true);
    if (!await confirmDiscard()) {setBusy(false);return;}
    try {await accountRequest('auth/sign-out',{});notifyAccountChange();location.replace(new URL(localeHome(locale),location.origin+'/').href);}
    catch {setFailed(true);setBusy(false);setOpen(true);}
  }
  return <DropdownMenu.Root open={open} onOpenChange={setOpen}><DropdownMenu.Trigger asChild><button className="masthead-account-entry" aria-label={t('account.actions')+' '+nickname}>
    <AccountAvatar src={avatar}/><span className="account-handle">{nickname}</span>
  </button></DropdownMenu.Trigger><DropdownMenu.Portal container={uiPortalHost()}>
    <DropdownMenu.Content align="end" sideOffset={8} className="ui-menu account-menu-panel ui-dropdown-motion">
      <DropdownMenu.Label className="account-menu-identity"><AccountAvatar src={avatar}/><span><strong>{nickname}</strong><small>@{user.username}</small></span></DropdownMenu.Label>
      <DropdownMenu.Item asChild><AppLink className="ui-menu-item" href={profile}>{t('nav.profile')}</AppLink></DropdownMenu.Item>
      <DropdownMenu.Item asChild><AppLink className="ui-menu-item" href={profile+'#edit'}>{t('profile.edit')}</AppLink></DropdownMenu.Item>
      <DropdownMenu.Item asChild><AppLink className="ui-menu-item" href={profile+'#settings'}>{t('profile.settings')}</AppLink></DropdownMenu.Item>
      <DropdownMenu.Separator className="ui-menu-separator"/>
      <DropdownMenu.Item className="ui-menu-item" disabled={busy} onSelect={()=>{void logout();}}>{t('account.signOut')}</DropdownMenu.Item>
      {failed && <span role="alert">{t('account.requestFailed')}</span>}
    </DropdownMenu.Content></DropdownMenu.Portal></DropdownMenu.Root>;
}
