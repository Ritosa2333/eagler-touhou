import {useState, type ComponentPropsWithoutRef} from 'react';
import type {RoomAccountProfile} from '../../../src/launcher/room-account-profile.mts';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';
import {useLocale} from '../../i18n';
import {accountsEnabled} from '../../release-features';
import './participant-avatar.css';

export function ParticipantAvatar({
  profile, name, className, children, role, ...props
}: ComponentPropsWithoutRef<'span'> & {profile?: RoomAccountProfile; name: string}) {
  const {locale, t} = useLocale();
  const [failed, setFailed] = useState('');
  if (!accountsEnabled || !profile) return <span className={className} role={role} {...props}>{children}</span>;
  const username = encodeURIComponent(profile.username);
  const url = profile.avatar ? '/api/v1/profiles/' + username + '/avatar/' + profile.avatar : '';
  const label = t('room.viewProfile', {name: name || profile.username});
  const description = props['aria-label'] ? label + ' | ' + props['aria-label'] : label;
  return <a {...props} className={className ? className + ' participant-avatar' : 'participant-avatar'} data-registered="true"
    href={withUiLanguage('/u/' + username, locale)} target="_blank" rel="noopener"
    aria-label={description} title={label}>
    {url && failed !== url ? <img className="participant-avatar-image" src={url} alt="" loading="lazy" decoding="async"
      width={64} height={64} onError={() => setFailed(url)}/> : children}
  </a>;
}
