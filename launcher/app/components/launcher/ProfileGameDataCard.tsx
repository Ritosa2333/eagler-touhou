import {LayoutGrid} from 'lucide-react';
import {useState} from 'react';
import {useLocation} from 'react-router';
import {useLocale} from '../../i18n';
import {gameDataViewLabels, gameDataViews, hasProfileSaveSection, type GameDataView, type ProfileDisplay} from '../../models/profile-display';
import type {ProfileStatistics} from '../../services/cloud-saves';
import {Button} from '../../ui/button';
import {ChoiceGroup, ChoiceItem} from '../../ui/choice-group';
import {ProfileAllGames} from './ProfileGameData';
import {ProfilePanelLoading} from './ProfileLoading';
import {ProfileTh09Duels} from './ProfileTh09Duels';
import cover from './book-covers/th09.webp?url';
import './profile-game-data-card.css';

export function GameDataIcon({view}: {view:GameDataView}) {
  return view === 'all' ? <LayoutGrid aria-hidden="true"/> : <img src={cover} alt="" width={24} height={24}/>;
}

export function ProfileGameDataCard({username, owner, display, statistics, statisticsError}: {
  username: string; owner: boolean; display: ProfileDisplay; statistics?: ProfileStatistics; statisticsError: boolean;
}) {
  const {t} = useLocale();
  const {search,hash} = useLocation();
  const rawCharacter = new URLSearchParams(search).get('character');
  const requested = rawCharacter !== null && /^(?:[0-9]|1[0-5])$/.test(rawCharacter) ? 'th09'
    : hash === '#profile-career' || hash === '#profile-records' ? 'all' : undefined;
  const [selection, setSelection] = useState<GameDataView>();
  const enabled = {all: display.visible.career || display.visible.records, th09: display.visible.versus};
  const views = gameDataViews.filter(view => enabled[view]);
  const preferred = selection ?? requested ?? display.gameDataDefault ?? 'th09';
  const view = views.includes(preferred) ? preferred : views[0];
  if (!view) return null;
  const icons = <ChoiceGroup type="single" value={view} className="profile-game-data-switch" aria-label={t('profile.gameData')}
      onValueChange={value=>{if (value === 'all' || value === 'th09') setSelection(value);}}>
      {views.map(item => <ChoiceItem key={item} value={item} aria-label={t(gameDataViewLabels[item])} title={t(gameDataViewLabels[item])}>
        <GameDataIcon view={item}/>
      </ChoiceItem>)}
    </ChoiceGroup>;
  return <section className="profile-panel profile-public-section profile-game-data profile-game-data-card" id="profile-versus"
    aria-labelledby="profile-game-data-title">
    <h2 id="profile-game-data-title" className="sr-only">{t('profile.gameData')}</h2>
    {view === 'th09' ? <ProfileTh09Duels username={username} owner={owner} icons={icons}/> : statistics
      ? <ProfileAllGames data={statistics} showCareer={display.visible.career && hasProfileSaveSection(statistics,'career')}
          showRecords={display.visible.records && hasProfileSaveSection(statistics,'records')}/>
      : statisticsError ? <p role="alert">{t('cloudSave.statisticsFailed')}{' '}
          <Button variant="ghost" onClick={()=>window.dispatchEvent(new Event('cloud-save-change'))}>{t('cloudSave.refresh')}</Button></p>
        : <ProfilePanelLoading label={t('profile.gameDataAll')}/>}
    {view === 'all' && icons}
  </section>;
}
