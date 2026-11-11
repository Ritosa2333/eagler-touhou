import {Button as UiButton} from '../../ui/controls';
import {FieldHelp} from '../../ui/field-help';
import {defaultTh15ChapterThreshold} from "../../../src/launcher/th15-chapter-reward.mts";
import {TH05_INITIAL_STOCK} from '../../../src/launcher/th05-initial-stock.mts';
import type {RoomOwnerSnapshot, MultiplayerRoomService} from '../../services/multiplayer-room';
import {useLocale} from '../../i18n';
import {RoomChoiceGroup} from './RoomChoiceGroup';
import {roomPresentation} from './presentation';
import {MainSelect} from '../launcher/MainSelect';
import {RoomTiming, type RoomTimingActions} from './RoomTiming';
import {OptionSwitch} from '../settings/TouchSettingsFields';
import {Th19VersusSettings} from './Th19VersusSettings';
import {SettingsSelect} from '../settings/SettingsSelect';
export function RoomGameSettings({state, service, hidden}: {state: RoomOwnerSnapshot; service: Pick<MultiplayerRoomService, 'setRoomSettings' | 'takeSeat'> & RoomTimingActions; hidden: boolean}) {
  const {t} = useLocale(), {room, ready, owner, config} = roomPresentation(state, t)!;
  const disabled = !ready || !owner || room.phase !== 'lobby';
  const privateHelp = room.visibility === 'private';
  const inGameDifficulty = config.inGameSelection && state.product !== 'th03mp';
  return <section className="mp-room-settings room-grouped-settings options-settings" id="mpRoomSettings" hidden={hidden} data-room-panel="game">
    <section className="room-settings-group" aria-label={t('room.basicSettings')}>
      <div className="room-basic-fields">
        <div className="grid min-w-0 gap-1.5">
          <div className="flex items-center gap-1">
            <label htmlFor="mpRoomVisibility-trigger">{t('room.visibility')}</label>
            {privateHelp && <FieldHelp id="mpRoomPrivateHint" label={t('room.visibility')}>{t('room.privateHint')}</FieldHelp>}
          </div>
          <MainSelect id="mpRoomVisibility" aria-label={t('room.visibility')} aria-describedby={privateHelp ? 'mpRoomPrivateHint' : undefined} className="option-select" value={room.visibility || 'public'} disabled={disabled} onChange={event => service.setRoomSettings({visibility: event.target.value as 'public' | 'private'})}>
            <option value="public">{t('room.public')}</option><option value="private">{t('room.private')}</option>
          </MainSelect>
        </div>
        <label>{t('multiplayer.players')}<MainSelect id="mpRoomPlayerCount" aria-label={t('multiplayer.players')} className="option-select" value={room.playerCount} disabled={disabled || config.playerCounts.length < 2} onChange={event => service.setRoomSettings({playerCount: Number(event.target.value) as 2 | 3})}>
          {config.playerCounts.map(count => <option key={count} value={count}>{count}P</option>)}
        </MainSelect></label>
      </div>
      {state.product === 'th09mp' && room.unrated !== undefined && <div className="room-rule-field">
        <label htmlFor="mpRoomUnrated">{t('room.unrated')}</label>
        <OptionSwitch id="mpRoomUnrated" label={t('room.unrated')} checked={room.unrated} disabled={disabled}
          onChange={() => service.setRoomSettings({unrated: !room.unrated})}/>
      </div>}
      {!inGameDifficulty && <RoomChoiceGroup title={<><span>{t('lobby.difficulty')}</span>{room.challengeMode && <span className="room-challenge-on">{t('room.challengeEnabled')}</span>}</>} name="difficulty" valueAttribute="data-mp-difficulty" choices={config.difficulties.map((label, index) => ({value:index,label,selected:room.difficulty === index,disabled}))} onSelect={difficulty => service.setRoomSettings({difficulty})}/>}
    </section>
    <section className="room-settings-group">
      <h3 className="flex items-center gap-1">
        <span>{t('room.gameSettingsGroup')}</span>
        {inGameDifficulty && <FieldHelp id="mpRoomGameHelp" label={t('room.gameSettingsGroup')}>{t('room.inGameSelectionHint')}</FieldHelp>}
        {state.product === 'th05mp' && <FieldHelp id="mpTh05InitialStockHint" label={t('room.gameSettingsGroup')}>{t('room.th05InitialStockHint')}</FieldHelp>}
      </h3>
      <RoomTiming state={state} service={service}/><RoomTiming state={state} service={service} settings/>
      {state.product === 'th19mp' && <Th19VersusSettings rules={room.th19Rules} disabled={disabled} onChange={th19Rules=>service.setRoomSettings({th19Rules})}/>}
      {state.product === 'th05mp' &&
        <fieldset className="room-basic-fields" aria-describedby="mpTh05InitialStockHint">
          <SettingsSelect id="mpTh05InitialSpares" label={t('room.th05InitialSpares')} value={room.initialSpares ?? TH05_INITIAL_STOCK.spares}
            disabled={disabled} onChange={event => service.setRoomSettings({initialSpares: Number(event.target.value)})}>
            {[0, 1, 2, 3, 4, 5].map(value => <option key={value} value={value}>{value}</option>)}
          </SettingsSelect>
          <SettingsSelect id="mpTh05InitialBombs" label={t('room.th05InitialBombs')} value={room.initialBombs ?? TH05_INITIAL_STOCK.bombs}
            disabled={disabled} onChange={event => service.setRoomSettings({initialBombs: Number(event.target.value)})}>
            {[0, 1, 2, 3].map(value => <option key={value} value={value}>{value}</option>)}
          </SettingsSelect>
        </fieldset>}
      {state.product === 'th15mp' && <div className="room-reward-field">
        <div className="flex min-w-0 items-center gap-1">
          <label htmlFor="mpTh15ChapterThreshold-trigger">{t('room.th15ChapterThreshold')}</label>
          <FieldHelp id="mpTh15ChapterGraze" label={t('room.th15ChapterThreshold')}>{t('room.th15ChapterGraze',{count:(room.chapterThreshold ?? defaultTh15ChapterThreshold(room.playerCount))/5000})}</FieldHelp>
        </div>
        <MainSelect className="option-select" id="mpTh15ChapterThreshold" aria-label={t('room.th15ChapterThreshold')} aria-describedby="mpTh15ChapterGraze" value={String(room.chapterThreshold ?? defaultTh15ChapterThreshold(room.playerCount))} disabled={disabled} onChange={event => service.setRoomSettings({chapterThreshold:Number(event.target.value)})}>
        {Array.from({length:19},(_,index)=>500000+index*250000).map(value=><option key={value} value={value}>{value.toLocaleString()}</option>)}
        </MainSelect>
      </div>}
    </section>
    <section className="room-settings-group"><h3>{t('room.ruleSettings')}</h3>
      {(['cheat','challenge'] as const).map(kind => {
        const cheat = kind === 'cheat';
        const label = t(cheat ? 'lobby.noCheat' : 'room.challengeMode');
        const id = cheat ? 'mpRoomCheatToggle' : 'mpRoomChallengeToggle';
        const hint = cheat ? 'mpRoomCheatHint' : 'mpRoomChallengeHint';
        return <div className="room-rule-field" key={kind} hidden={!cheat && config.gameplay !== 'cooperative'}>
          <div className="flex items-center gap-1">
            <label htmlFor={id}>{label}</label>
            <FieldHelp id={hint} label={label}>{t(cheat ? 'room.cheatHint' : 'room.challengeHint')}</FieldHelp>
          </div>
          <OptionSwitch id={id} label={label} describedBy={hint} checked={cheat ? !!room.disableCheatMovement : !!room.challengeMode} disabled={disabled} onChange={() => service.setRoomSettings(cheat ? {disableCheatMovement:!room.disableCheatMovement} : {challengeMode:!room.challengeMode})}/>
        </div>;
      })}
      {state.product === 'th09mp' &&
        <div className="room-rule-field">
          <div className="flex items-center gap-1">
            <label htmlFor="mpDisableLocalChargePriority">{t('room.disableLocalChargePriority')}</label>
            <FieldHelp id="mpLocalChargeReplayHint" label={t('room.disableLocalChargePriority')}>{t('room.localChargeReplayHint')}</FieldHelp>
          </div>
          <OptionSwitch id="mpDisableLocalChargePriority" label={t('room.disableLocalChargePriority')} describedBy="mpLocalChargeReplayHint" checked={!!room.disableLocalChargePriority} disabled={disabled} onChange={()=>service.setRoomSettings({disableLocalChargePriority:!room.disableLocalChargePriority})}/>
        </div>}
    </section>
    {!owner && <p className="room-setting-description" id="mpRoomSettingsHint">{t(!room.seats?.[0] ? 'room.hostAvailable' : 'multiplayer.ownerRemoteHint')}</p>}
    <UiButton id="mpTakeHostSeat" type="button" hidden={!!room.seats?.[0] || owner} disabled={!ready || room.phase !== 'lobby'} onClick={()=>{void service.takeSeat(0);}}>{t('room.takeHostSeat')}</UiButton>
  </section>;
}
