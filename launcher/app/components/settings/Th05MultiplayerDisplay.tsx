import type {GameSettingsModel, SettingsSnapshot} from '../../models/game-settings';
import {normalizeTh05MultiplayerDisplay, type Th05MultiplayerDisplay as Display} from '../../../src/launcher/game-preferences.mts';
import {useLocale} from '../../i18n';
import {MainSelect} from '../launcher/MainSelect';
import {OptionRow} from './OptionRow';
import {OptionSwitch} from './TouchSettingsFields';

export function Th05MultiplayerDisplay({model, state}: {model: GameSettingsModel; state: SettingsSnapshot}) {
  const {t} = useLocale(), value = state.options.th05MultiplayerDisplay;
  const set = <K extends keyof Display>(key: K, next: Display[K]) =>
    model.setOption('th05MultiplayerDisplay', normalizeTh05MultiplayerDisplay({...value, [key]: next}));
  const toggles = ['phaseMarkers', 'phaseStars', 'phaseTimeout', 'timeoutAsTime', 'numericDream', 'numericPower'] as const;
  return <>
    <section className="item feature-section" id="mpTh05BossBarOption"><div className="itemtop"><span>{t('settings.th05.bossBar')}</span></div>
      <MainSelect className="option-select" id="mpTh05BossBarSelect" aria-label={t('settings.th05.bossBar')} value={value.bossBar}
        onChange={event => {const next = event.target.value; if (next === 'native' || next === 'eosd' || next === 'ring') set('bossBar', next);}}>
        {(['native', 'eosd', 'ring'] as const).map(style => <option key={style} value={style}>{t(`settings.th05.bar.${style}`)}</option>)}
      </MainSelect>
    </section>
    {toggles.map(key => <OptionRow key={key} className="item" label={t(`settings.th05.${key}`)} control={
      <OptionSwitch id={`mpTh05${key[0].toUpperCase()}${key.slice(1)}Toggle`} label={t(`settings.th05.${key}`)} checked={value[key]}
        disabled={key === 'phaseMarkers' && value.bossBar !== 'native' || key === 'phaseStars' && value.bossBar !== 'eosd' || key === 'phaseTimeout' && value.bossBar !== 'native' || key === 'timeoutAsTime' && !value.phaseTimeout}
        onChange={() => set(key, !value[key])}/>
    }/>)}
  </>;
}
