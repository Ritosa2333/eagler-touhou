import {defaultTh15ChapterThreshold} from "../../../src/launcher/th15-chapter-reward.mts";
import type {RoomOwnerSnapshot, MultiplayerRoomService} from '../../services/multiplayer-room';
import {useLocale} from '../../i18n';
import {RoomChoiceGroup} from './RoomChoiceGroup';
import {roomPresentation} from './presentation';
import {MainSelect} from '../launcher/MainSelect';
import {DifficultyLabel} from './DifficultyLabel';
import {RoomTiming} from './RoomTiming';
import {OptionSwitch} from '../settings/TouchSettingsFields';
const difficulties = ['Easy', 'Normal', 'Hard', 'Lunatic', 'Extra', 'Phantasm'];
export function RoomGameSettings({state, service, hidden, assetUrl, grouped = false}: {state: RoomOwnerSnapshot; service: MultiplayerRoomService; hidden: boolean; assetUrl?(path: string): string; grouped?: boolean}) {
  const {t} = useLocale(), {room, ready, owner, config} = roomPresentation(state, t)!;
  const disabled = !ready || !owner || room.phase !== 'lobby';
  if (grouped) return <section className="mp-room-settings room-grouped-settings options-settings" id="mpRoomSettings" hidden={hidden} data-room-panel="game">
    <section className="room-settings-group"><h3>{t('room.basicSettings')}</h3>
      <div className="room-basic-fields">
        <label>{t('room.visibility')}<MainSelect id="mpRoomVisibility" className="option-select" value={room.visibility || 'public'} disabled={disabled} onChange={event => service.setRoomSettings({visibility: event.target.value as 'public' | 'private'})}>
          <option value="public">{t('room.public')}</option><option value="private">{t('room.private')}</option>
        </MainSelect></label>
        <label>{t('multiplayer.players')}<MainSelect id="mpRoomPlayerCount" className="option-select" value={room.playerCount} disabled={disabled || config.playerCounts.length < 2} onChange={event => service.setRoomSettings({playerCount: Number(event.target.value) as 2 | 3})}>
          {config.playerCounts.map(count => <option key={count} value={count}>{count}P</option>)}
        </MainSelect></label>
      </div>
      <p className="room-setting-description" hidden={room.visibility !== 'private'}>{t('room.privateHint')}</p>
      {config.inGameSelection ? <p className="room-setting-description">{t('room.inGameSelectionHint')}</p> : <RoomChoiceGroup title={<><span>{t('multiplayer.difficulty')}</span>{room.challengeMode && <span className="room-challenge-on">{t('room.challengeEnabled')}</span>}</>} name="difficulty" valueAttribute="data-mp-difficulty" choices={config.difficulties.map((label, index) => ({value:index,label,selected:room.difficulty === index,disabled}))} onSelect={difficulty => service.setRoomSettings({difficulty})}/>}
    </section>
    <section className="room-settings-group"><h3>{t('room.gameSettingsGroup')}</h3>
      <RoomTiming state={state} service={service}/><RoomTiming state={state} service={service} settings/>
      {state.product === 'th15mp' && <><label className="room-reward-field" htmlFor="mpTh15ChapterThreshold">{t('room.th15ChapterThreshold')}<MainSelect className="option-select" id="mpTh15ChapterThreshold" aria-describedby="mpTh15ChapterGraze" value={String(room.chapterThreshold ?? defaultTh15ChapterThreshold(room.playerCount))} disabled={disabled} onChange={event => service.setRoomSettings({chapterThreshold:Number(event.target.value)})}>
        {Array.from({length:19},(_,index)=>500000+index*250000).map(value=><option key={value} value={value}>{value.toLocaleString()}</option>)}
      </MainSelect></label><p className="room-setting-description" id="mpTh15ChapterGraze">{t('room.th15ChapterGraze',{count:(room.chapterThreshold ?? defaultTh15ChapterThreshold(room.playerCount))/5000})}</p></>}
    </section>
    <section className="room-settings-group"><h3>{t('room.ruleSettings')}</h3>
      {(['cheat','challenge'] as const).map(kind => <div className="room-rule-field" key={kind} hidden={kind === 'challenge' && config.gameplay !== 'cooperative'}><div><strong>{t(kind === 'cheat' ? 'lobby.noCheat' : 'room.challengeMode')}</strong><p className="room-setting-description" id={kind === 'cheat' ? 'mpRoomCheatHint' : 'mpRoomChallengeHint'}>{t(kind === 'cheat' ? 'room.cheatHint' : 'room.challengeHint')}</p></div>
        <OptionSwitch id={kind === 'cheat' ? 'mpRoomCheatToggle' : 'mpRoomChallengeToggle'} label={t(kind === 'cheat' ? 'lobby.noCheat' : 'room.challengeMode')} checked={kind === 'cheat' ? !!room.disableCheatMovement : !!room.challengeMode} disabled={disabled} onChange={() => service.setRoomSettings(kind === 'cheat' ? {disableCheatMovement:!room.disableCheatMovement} : {challengeMode:!room.challengeMode})}/>
      </div>)}
    </section>
    <p className="room-setting-description" id="mpRoomSettingsHint">{t(owner ? 'multiplayer.ownerLocalHint' : !room.seats?.[0] ? 'room.hostAvailable' : 'multiplayer.ownerRemoteHint')}</p>
    <button id="mpTakeHostSeat" type="button" hidden={!!room.seats?.[0] || owner} disabled={!ready || room.phase !== 'lobby'} onClick={()=>{void service.takeSeat(0);}}>{t('room.takeHostSeat')}</button>
  </section>;
  const rule = (kind: 'cheat' | 'challenge') => {const selected = kind === 'cheat' ? !!room.disableCheatMovement : !!room.challengeMode, hintId = kind === 'cheat' ? 'mpRoomCheatHint' : 'mpRoomChallengeHint'; return <div className="mp-room-rule-setting"><button className={`mp-room-rule-tag${selected ? ' selected' : ''}`} type="button" data-room-rule={kind} aria-pressed={selected} aria-describedby={hintId} disabled={!ready || !owner || room.phase !== 'lobby'} onClick={() => service.setRoomSettings(kind === 'cheat' ? {disableCheatMovement: !selected} : {challengeMode: !selected})}><span>{t(kind === 'cheat' ? 'lobby.noCheat' : 'room.challengeMode')}</span><span className="mp-room-rule-state" data-room-rule-state="">{t(selected ? 'room.modeOn' : 'room.modeOff')}</span></button><p className="mp-room-rule-hint" id={hintId}>{t(kind === 'cheat' ? 'room.cheatHint' : 'room.challengeHint')}</p></div>;};
  return <section className="mp-room-settings" data-room-panel="game" id="mpRoomSettings" hidden={hidden}>
    <RoomChoiceGroup title={t('room.visibility')} valueAttribute="data-room-visibility" choices={(['public', 'private'] as const).map(visibility => {
      const selected = visibility === (room.visibility || 'public');
      return {value: visibility, label: t(`room.${visibility}`), selected, disabled: !ready || !owner || room.phase !== 'lobby' || selected};
    })} onSelect={visibility => service.setRoomSettings({visibility})}/><p className="mp-room-settings-hint">{t('room.privateHint')}</p>
    {rule('cheat')}<div id="mpRoomRuleModes" hidden={config.gameplay !== 'cooperative'}>{rule('challenge')}</div>
    <RoomTiming state={state} service={service} settings/>
    {state.product === 'th15mp' && <section className="mp-room-rule-setting" id="mpTh15ChapterSetting">
      <label htmlFor="mpTh15ChapterThreshold">{t('room.th15ChapterThreshold')}</label>
      <MainSelect className="option-select" id="mpTh15ChapterThreshold" aria-describedby="mpTh15ChapterGraze" value={String(room.chapterThreshold ?? defaultTh15ChapterThreshold(room.playerCount))} disabled={!ready || !owner || room.phase !== 'lobby'} onChange={event => service.setRoomSettings({chapterThreshold:Number(event.currentTarget.value)})}>
        {Array.from({length:19},(_,index)=>500000+index*250000).map(value=><option key={value} value={value}>{value.toLocaleString()}</option>)}
      </MainSelect>
      <p className="mp-room-rule-hint" id="mpTh15ChapterGraze">{t('room.th15ChapterGraze',{count:(room.chapterThreshold ?? defaultTh15ChapterThreshold(room.playerCount))/5000})}</p>
    </section>}
    <RoomChoiceGroup title={t('multiplayer.players')} name="playerCount" valueAttribute="data-mp-player-count" choices={([2, 3] as const).map(playerCount => {
      const supported = config.playerCounts.includes(playerCount), selected = playerCount === room.playerCount;
      return {value: playerCount, label: `${playerCount}P`, selected, hidden: !supported, disabled: !supported || !ready || !owner || selected};
    })} onSelect={playerCount => service.setRoomSettings({playerCount})}/>
    {config.inGameSelection ? <p className="mp-room-settings-hint">{t('room.inGameSelectionHint')}</p> : <RoomChoiceGroup title={<><span>{t('multiplayer.difficulty')}</span> <strong id="mpRoomDifficultyText"><DifficultyLabel name={config.difficulties[room.difficulty] || 'Normal'}/></strong> {room.challengeMode && <span className="room-challenge-on">{t('room.challengeEnabled')}</span>}</>} name="difficulty" valueAttribute="data-mp-difficulty" choices={difficulties.map((label, difficulty) => {
      const supported = difficulty < config.difficulties.length, selected = difficulty === room.difficulty;
      return {value: difficulty, label: <DifficultyLabel name={label} assetUrl={assetUrl}/>, selected, hidden: !supported, disabled: !supported || !ready || !owner || selected};
    })} onSelect={difficulty => service.setRoomSettings({difficulty})}/>}
    <div className="mp-room-native-selects" aria-hidden="true"><select className="option-select" id="mpRoomPlayerCount" value={room.playerCount} disabled={!ready || !owner} onChange={event => service.setRoomSettings({playerCount: Number(event.target.value) as 2 | 3})}>{([2, 3] as const).map(count => <option key={count} value={count} hidden={!config.playerCounts.includes(count)} disabled={!config.playerCounts.includes(count)}>{count}P</option>)}</select><select className="option-select" id="mpRoomDifficulty" value={room.difficulty} disabled={config.inGameSelection || !ready || !owner} onChange={event => service.setRoomSettings({difficulty: Number(event.target.value)})}>{difficulties.map((label, index) => <option key={index} value={index} hidden={index >= config.difficulties.length} disabled={index >= config.difficulties.length}>{label}</option>)}</select></div>
    <p className="mp-room-settings-hint" id="mpRoomSettingsHint">{t(owner ? 'multiplayer.ownerLocalHint' : !room.seats?.[0] ? 'room.hostAvailable' : 'multiplayer.ownerRemoteHint')}</p><button id="mpTakeHostSeat" type="button" hidden={!!room.seats?.[0] || owner} disabled={!ready || room.phase !== 'lobby'} onClick={() => {void service.takeSeat(0);}}>{t('room.takeHostSeat')}</button>
  </section>;
}
