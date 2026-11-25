import {useState} from 'react';
import {DropdownMenu} from 'radix-ui';
import {Button} from '../../ui/button';
import {AccountAvatar} from './AccountAvatar';
import {NavigationIcon} from './NavigationIcon';
import {MastheadTooltip} from './MastheadTooltip';
import {ChevronDown, House, LogOut, Pencil, ShieldCheck} from 'lucide-react';
import {AppLink} from './AppLink';
import {accountRequest} from '../../services/account-client';
import {notifyAccountChange} from '../../services/account-session';
import {useLocale} from '../../i18n';
import {withUiLanguage,localeHome} from '../../../src/launcher/i18n.mts';
import {uiPortalHost} from '../../ui/portal';
import {useConfirmDiscardChanges} from '../../navigation/unsaved-changes';
import {IdentityMetadata, useAccountLinks, useLauncherIdentity} from './AccountIdentity';

export function AccountMenu({interactive,assetUrl}:{interactive:boolean;assetUrl:(path:string)=>string}) {
  const {locale,t} = useLocale(), {state,retry,user,nickname,avatar,country} = useLauncherIdentity(interactive);
  const {loginHref} = useAccountLinks();
  const confirmDiscard = useConfirmDiscardChanges();
  const [failed,setFailed] = useState(false), [busy,setBusy] = useState(false);
  const [open,setOpen] = useState(false);
  if (state.status==='loading') return <span className="masthead-action account-loading" aria-busy="true"><span className="account-avatar account-avatar-loading"/></span>;
  if (state.status==='error') return <MastheadTooltip label={t('account.requestFailed')}>
    <Button variant="ghost" size="icon" className="masthead-action book-profile-trigger"
      onClick={retry} aria-label={`${t('nav.account')} | ${t('action.retry')}`}>
      <NavigationIcon name="profile"/>
    </Button>
  </MastheadTooltip>;
  if (!user) {
    const label=t('account.login');
    return <MastheadTooltip label={label}>
      <Button asChild variant="ghost" size="lg" className="masthead-action account-login-link gap-1.5 px-2">
        <AppLink href={loginHref} rememberEntry aria-label={label}>
          <NavigationIcon name="profile"/>
          <span>{label}</span>
        </AppLink>
      </Button>
    </MastheadTooltip>;
  }
  const avatarView = <AccountAvatar src={avatar} assetUrl={assetUrl}/>;
  const profile=withUiLanguage('/u/'+encodeURIComponent(user.username),locale);
  async function logout() {
    if (busy) return;
    setFailed(false);setBusy(true);
    if (!await confirmDiscard()) {setBusy(false);return;}
    try {await accountRequest('auth/sign-out',{});notifyAccountChange();location.replace(new URL(localeHome(locale),location.origin+'/').href);}
    catch {setFailed(true);setBusy(false);setOpen(true);}
  }
  return <DropdownMenu.Root open={open} onOpenChange={setOpen}><MastheadTooltip label={t('account.actions')}><DropdownMenu.Trigger asChild><Button variant="ghost" size="icon" className="masthead-action masthead-account-entry" aria-label={t('account.actions')+' '+nickname}>
    {avatarView}<span>{nickname}</span><ChevronDown aria-hidden="true"/>
  </Button></DropdownMenu.Trigger></MastheadTooltip><DropdownMenu.Portal container={uiPortalHost()}>
    <DropdownMenu.Content align="end" sideOffset={8} className="ui-menu account-menu-panel ui-dropdown-motion">
      <DropdownMenu.Label className="account-menu-identity">{avatarView}<span><strong>{nickname}</strong><IdentityMetadata username={user.username} country={country}/></span></DropdownMenu.Label>
      <DropdownMenu.Item asChild><AppLink className="ui-menu-item" href={profile} rememberEntry><House aria-hidden="true"/>{t('profile.pageTitle')}</AppLink></DropdownMenu.Item>
      <DropdownMenu.Item asChild><AppLink className="ui-menu-item" href={profile+'#edit'} rememberEntry><Pencil aria-hidden="true"/>{t('profile.edit')}</AppLink></DropdownMenu.Item>
      <DropdownMenu.Item asChild><AppLink className="ui-menu-item" href={profile+'#settings'} rememberEntry><ShieldCheck aria-hidden="true"/>{t('profile.settings')}</AppLink></DropdownMenu.Item>
      <DropdownMenu.Separator className="ui-menu-separator"/>
      <DropdownMenu.Item className="ui-menu-item" disabled={busy} onSelect={()=>{void logout();}}><LogOut aria-hidden="true"/>{t('account.signOut')}</DropdownMenu.Item>
      {failed && <span role="alert">{t('account.requestFailed')}</span>}
    </DropdownMenu.Content></DropdownMenu.Portal></DropdownMenu.Root>;
}
