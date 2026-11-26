import {Palette, Settings2, ShieldCheck} from 'lucide-react';
import type {ReactNode} from 'react';
import {useLocale} from '../../i18n';
import {SettingsDisclosure, SettingsField, SettingsGroup, SettingsRowContent, SettingsSwitch} from '../settings/SettingsGroup';
import {Button} from '../../ui/button';
import {BookThemeToggle} from './BookThemeToggle';
import {AppLink} from './AppLink';

export interface LauncherPreferencesProps {
  languageControl: ReactNode;
  lessMotion: boolean;
  onToggleMotion(): void;
  diagnosticsEnabled: boolean;
  onToggleDiagnostics(): void;
  testFeatures: boolean;
  onToggleTestFeatures(): void;
}

export function LauncherPreferenceFields({settings}: {settings: LauncherPreferencesProps}) {
  const {t} = useLocale();
  return <>
    <SettingsField label={t('ui.language')}>{settings.languageControl}</SettingsField>
    <SettingsSwitch label={t('nav.lessMotion')} checked={settings.lessMotion} onChange={settings.onToggleMotion}/>
    <SettingsDisclosure title={t('developer.options')}>
      <SettingsSwitch label={t('diagnostics.toggle')} checked={settings.diagnosticsEnabled} onChange={settings.onToggleDiagnostics}/>
      <SettingsSwitch label={t('developer.betaFeatures')} checked={settings.testFeatures} onChange={settings.onToggleTestFeatures}/>
    </SettingsDisclosure>
  </>;
}

export function LauncherPreferences({settings, profileHref}: {settings: LauncherPreferencesProps; profileHref?: string}) {
  const {t} = useLocale();
  return <SettingsGroup>
    <SettingsDisclosure title={t('launcher.settings')} icon={Settings2}>
      <LauncherPreferenceFields settings={settings}/>
    </SettingsDisclosure>
    <SettingsDisclosure title={t('launcher.theme')} icon={Palette}>
      <BookThemeToggle row inlineChoices/>
    </SettingsDisclosure>
    {profileHref && <SettingsDisclosure title={t('profile.settings')} icon={ShieldCheck}>
      <Button variant="ghost" size="row" className="settings-navigation-row" asChild><AppLink href={profileHref + '#edit'} rememberEntry state={{settingsFromHome: true}}>
        <SettingsRowContent>{t('profile.personalSettings')}</SettingsRowContent>
      </AppLink></Button>
      <Button variant="ghost" size="row" className="settings-navigation-row" asChild><AppLink href={profileHref + '#security'} rememberEntry state={{settingsFromHome: true}}>
        <SettingsRowContent>{t('profile.security')}</SettingsRowContent>
      </AppLink></Button>
    </SettingsDisclosure>}
  </SettingsGroup>;
}
