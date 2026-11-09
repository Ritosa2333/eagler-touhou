import {useEffect,useState} from 'react';
import {DropdownMenu} from 'radix-ui';
import {Button} from '../../ui/button';
import {AccountAvatar} from './AccountAvatar';
import {NavigationIcon} from './NavigationIcon';
import {MastheadTooltip} from './MastheadTooltip';
import {useLocation} from 'react-router';
import {AppLink} from './AppLink';
import {accountRequest} from '../../services/account-client';
import {notifyAccountChange,useAccountSession} from '../../services/account-session';
import {useLocale} from '../../i18n';
import {withUiLanguage,localeHome} from '../../../src/launcher/i18n.mts';
import {uiPortalHost} from '../../ui/portal';
import {useConfirmDiscardChanges} from '../../navigation/unsaved-changes';

export function AccountMenu({interactive,assetUrl}:{interactive:boolean;assetUrl:(path:string)=>string}) {
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
        .then(profile => {if (!controller.signal.aborted) setIdentity({username:user.username,nickname:profile.nickname,avatar:profile.details?.avatar || ''});})
        .catch(() => {if (!controller.signal.aborted) setIdentity(previous => previous?.username === user.username ? previous : {username:user.username,nickname:user.name,avatar:''});});
    };
    refresh(); window.addEventListener('account-profile-change',refresh);
    return () => {current?.abort();window.removeEventListener('account-profile-change',refresh);};
  },[user]);
  if (state.status==='loading') return <span className="masthead-action account-loading" aria-busy="true"><span className="account-avatar account-avatar-loading"/></span>;
  if (state.status==='error') return <MastheadTooltip label={t('account.requestFailed')}>
    <Button variant="ghost" size="icon" className="masthead-action book-profile-trigger"
      onClick={retry} aria-label={`${t('nav.account')} | ${t('action.retry')}`}>
      <NavigationIcon name="profile"/>
    </Button>
  </MastheadTooltip>;
  if (!user) {
    const login=new URL(withUiLanguage('/login.html',locale),'https://launcher.invalid');
    if (path.pathname !== '/login.html') login.searchParams.set('next',path.pathname+path.search+path.hash);
    const label=t('account.login');
    return <MastheadTooltip label={label}>
      <Button asChild variant="ghost" size="lg" className="masthead-action account-login-link gap-1.5 px-2">
        <AppLink href={login.pathname+login.search} aria-label={label}>
          <NavigationIcon name="profile"/>
          <span>{label}</span>
        </AppLink>
      </Button>
    </MastheadTooltip>;
  }
  const current = identity?.username === user.username ? identity : undefined;
  const nickname = current?.nickname || user.name;
  const avatar = current?.avatar;
  const avatarView = current ? <AccountAvatar src={avatar} assetUrl={assetUrl}/> : <span className="account-avatar account-avatar-loading" aria-hidden="true"/>;
  const profile=withUiLanguage('/u/'+encodeURIComponent(user.username),locale);
  async function logout() {
    if (busy) return;
    setFailed(false);setBusy(true);
    if (!await confirmDiscard()) {setBusy(false);return;}
    try {await accountRequest('auth/sign-out',{});notifyAccountChange();location.replace(new URL(localeHome(locale),location.origin+'/').href);}
    catch {setFailed(true);setBusy(false);setOpen(true);}
  }
  return <DropdownMenu.Root open={open} onOpenChange={setOpen}><MastheadTooltip label={t('account.actions')}><DropdownMenu.Trigger asChild><Button variant="ghost" size="icon" className="masthead-action masthead-account-entry" aria-label={t('account.actions')+' '+nickname}>
    {avatarView}
  </Button></DropdownMenu.Trigger></MastheadTooltip><DropdownMenu.Portal container={uiPortalHost()}>
    <DropdownMenu.Content align="end" sideOffset={8} className="ui-menu account-menu-panel ui-dropdown-motion">
      <DropdownMenu.Label className="account-menu-identity">{avatarView}<span><strong>{nickname}</strong><small>@{user.username}</small></span></DropdownMenu.Label>
      <DropdownMenu.Item asChild><AppLink className="ui-menu-item" href={profile}>{t('nav.profile')}</AppLink></DropdownMenu.Item>
      <DropdownMenu.Item asChild><AppLink className="ui-menu-item" href={profile+'#edit'}>{t('profile.edit')}</AppLink></DropdownMenu.Item>
      <DropdownMenu.Item asChild><AppLink className="ui-menu-item" href={profile+'#settings'}>{t('profile.settings')}</AppLink></DropdownMenu.Item>
      <DropdownMenu.Separator className="ui-menu-separator"/>
      <DropdownMenu.Item className="ui-menu-item" disabled={busy} onSelect={()=>{void logout();}}>{t('account.signOut')}</DropdownMenu.Item>
      {failed && <span role="alert">{t('account.requestFailed')}</span>}
    </DropdownMenu.Content></DropdownMenu.Portal></DropdownMenu.Root>;
}
