import {useLocation} from 'react-router';
import {useLocale, type UiMessageKey} from '../../i18n';
import {Button} from '../../ui/button';
import {localeHome} from '../../../src/launcher/i18n.mts';
import {LauncherBack} from './LauncherBack';
import {ProfileBanner} from './ProfileBanner';
import './profile-preview.css';

export function ProfilePanelLoading({label}: {label?: string}) {
  return <div className="profile-panel profile-loading-panel" role="status" aria-busy="true" aria-label={label}>
    <span aria-hidden="true"/><span aria-hidden="true"/>
  </div>;
}

export function ProfileLoading({message, retry}: {message?: UiMessageKey; retry?: () => void}) {
  const {pathname, search, hash} = useLocation();
  const {locale, t} = useLocale();
  const username = pathname.match(/^\/u\/([\w.-]+)\/?$/)?.[1];
  const social = hash === '#profile-friends' || ['friends','followers','following'].includes(new URLSearchParams(search).get('social') ?? '');
  return <main className="player-profile profile-page" aria-label={t('nav.profile')} aria-busy={!message}>
    {!social && <ProfileBanner loading>
      <div className="profile-identity">
        <span className="profile-avatar" />
        <div className="profile-name">
          <span className="profile-loading-name" />
          {username && <div className="profile-name-meta"><span className="profile-username">@{username}</span></div>}
        </div>
      </div>
    </ProfileBanner>}
    <div className="profile-wrap">
      {message ? <>
        <LauncherBack href={localeHome(locale)} replace returnToEntry/>
        <div className="profile-panel" role="alert">{t(message)}{' '}
          {retry && <Button size="sm" variant="outline" onClick={retry}>{t('action.retry')}</Button>}
        </div>
      </> : social ? <ProfilePanelLoading/> : <div className="profile-columns" aria-hidden="true">
        <div className="profile-aside profile-loading-aside"><div className="profile-social-summary"/><ProfilePanelLoading/></div>
        <div className="profile-content"><ProfilePanelLoading/></div>
      </div>}
    </div>
  </main>;
}
