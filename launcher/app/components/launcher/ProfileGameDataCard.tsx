import {LayoutGrid} from 'lucide-react';
import {Activity, useLayoutEffect, useRef, useState} from 'react';
import {useLocation} from 'react-router';
import {useLocale} from '../../i18n';
import {useNavigationPageMotion} from '../../navigation/navigation-motion';
import {hasProfileSaveSection, type GameDataView, type ProfileDisplay} from '../../models/profile-display';
import {profileTh09Character} from '../../models/th09-character-ratings';
import type {ProfileStatistics} from '../../services/cloud-saves';
import {Button} from '../../ui/button';
import {ChoiceGroup, ChoiceItem} from '../../ui/choice-group';
import {ProfileAllGames} from './ProfileGameData';
import {ProfilePanelLoading} from './ProfileLoading';
import {ProfileTh09Duels, type WatchTh09Replay} from './ProfileTh09Duels';
import cover from './book-covers/th09.webp?url';
import './profile-game-data-card.css';

const profileViews = ['th09', 'all'] as const;
const profileViewLabels = {th09: 'profile.flowerView', all: 'profile.clear.allGames'} as const;

export function GameDataIcon({view}: {view:GameDataView}) {
  return view === 'all' ? <LayoutGrid aria-hidden="true"/> : <img src={cover} alt="" width={24} height={24}/>;
}

export function ProfileGameDataCard({username, owner, display, statistics, statisticsError, watchTh09Replay}: {
  username: string; owner: boolean; display: ProfileDisplay; statistics?: ProfileStatistics; statisticsError: boolean; watchTh09Replay?: WatchTh09Replay;
}) {
  const {t} = useLocale();
  const {search,hash} = useLocation();
  const requested = profileTh09Character(search) !== null ? 'th09'
    : hash === '#profile-career' || hash === '#profile-records' ? 'all' : undefined;
  const [selection, setSelection] = useState<GameDataView>();
  const [minimumHeight, setMinimumHeight] = useState(0);
  const restoreScroll = useRef<{view:GameDataView;top:number} | undefined>(undefined);
  const enabled = {all: display.visible.career || display.visible.records, th09: display.visible.versus};
  const views = profileViews.filter(view => enabled[view]);
  const preferred = selection ?? requested ?? display.gameDataDefault ?? 'th09';
  const view = views.includes(preferred) ? preferred : views[0];
  const tabContent = useNavigationPageMotion(view ?? null);
  useLayoutEffect(() => {
    const pending = restoreScroll.current;
    if (!pending || pending.view !== view) return;
    restoreScroll.current = undefined;
    window.scrollTo({top:pending.top, behavior:'instant'});
  }, [view]);
  if (!view) return null;
  const select = (next: string) => {
    if ((next !== 'all' && next !== 'th09') || next === view) return;
    const top = window.scrollY;
    const bounds = tabContent.current?.getBoundingClientRect();
    // A shorter destination needs enough height to keep the same viewport.
    setMinimumHeight(top > 0 && bounds
      ? Math.max(0, Math.ceil(Math.min(bounds.height, document.documentElement.clientHeight - bounds.top)))
      : 0);
    restoreScroll.current = {view:next, top};
    setSelection(next);
  };
  const choices = <ChoiceGroup type="single" value={view} className="profile-game-data-switch" aria-label={t('profile.gameData')}
      onValueChange={select}>
      {views.map(item => <ChoiceItem key={item} value={item} aria-label={t(profileViewLabels[item])} title={t(profileViewLabels[item])}>
        <GameDataIcon view={item}/>{t(profileViewLabels[item])}
      </ChoiceItem>)}
    </ChoiceGroup>;
  return <section className="profile-public-section profile-game-data profile-game-data-card" id="profile-versus" data-game-data-view={view}
    aria-labelledby="profile-game-data-title">
    <header className="profile-game-data-heading">
      <h2 id="profile-game-data-title">{t('profile.gameData')}</h2>
      {views.length > 1 && choices}
    </header>
    <div ref={tabContent} className="profile-game-data-body" style={{minHeight:minimumHeight || undefined}}>
      {enabled.th09 && (view === 'th09' || selection) && <Activity mode={view === 'th09' ? 'visible' : 'hidden'}>
        <ProfileTh09Duels username={username} owner={owner} watchTh09Replay={watchTh09Replay}/>
      </Activity>}
      {enabled.all && (view === 'all' || selection) && <Activity mode={view === 'all' ? 'visible' : 'hidden'}>
        {statistics
          ? <ProfileAllGames data={statistics} showCareer={display.visible.career && hasProfileSaveSection(statistics,'career')}
              showRecords={display.visible.records && hasProfileSaveSection(statistics,'records')}/>
          : statisticsError ? <p role="alert">{t('cloudSave.statisticsFailed')}{' '}
              <Button variant="ghost" onClick={()=>window.dispatchEvent(new Event('cloud-save-change'))}>{t('cloudSave.refresh')}</Button></p>
            : <ProfilePanelLoading label={t(profileViewLabels.all)}/>}
      </Activity>}
    </div>
  </section>;
}
