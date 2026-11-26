import {useLocale} from '../../i18n';
import {accountsEnabled} from '../../release-features';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';
import {LauncherAccountRow, useLauncherIdentity} from './AccountIdentity';
import {LauncherPreferences, type LauncherPreferencesProps} from './LauncherPreferences';
import {SocialCounts, SocialProvider} from './SocialSection';

export function LauncherHome({active, settings, assetUrl}: {active: boolean; settings?: LauncherPreferencesProps; assetUrl: (path: string) => string}) {
  const {locale, t} = useLocale();
  const identity = useLauncherIdentity(accountsEnabled && !!settings);
  const username = identity.user?.username;
  const profileHref = username && withUiLanguage('/u/' + encodeURIComponent(username), locale);
  return <main className="launcher-home" id={active ? 'main' : undefined} aria-labelledby="launcher-home-heading">
    <h1 id="launcher-home-heading">{t('launcher.home')}</h1>
    {accountsEnabled && <section className="launcher-home-account" data-member={!!username} aria-label={t('nav.account')}>
      <LauncherAccountRow identity={identity} assetUrl={assetUrl}/>
      {username && profileHref && <SocialProvider key={username} username={username} includeLists={false}>
        <div className="launcher-home-counts"><SocialCounts username={username} href={profileHref}/></div>
      </SocialProvider>}
    </section>}
    {settings && <LauncherPreferences settings={settings} profileHref={profileHref || undefined}/>}
  </main>;
}
